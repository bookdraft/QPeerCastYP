/*************************************************************************
 * PCRaw
 * Authors: ◆e5bW6vDOJ.
 * Copyright: Copyright (C) 2007 ◆e5bW6vDOJ.
 * Date: February 11, 2007
 * License: 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

import std.stdio;
import std.thread;
import std.socket;
import std.string;
import std.stream;
import std.socketstream;
import std.regexp;
import std.c.time;
import std.conv;

import envcheck;
import tools;
import http;

import pcraw.pcraw;
import pcraw.pcrawtools;
import pcraw.exception;
import pcraw.socket;
import pcraw.common;

class Global
{
	PCRaw pcraw;
	uint lastUpdateTime;
	
	ubyte[] cache;
	uint cacheUpdateTime;
	
	bool terminate;
	Object syncObj;
	
	string indexTxtAddress;
	string localAddress;
	ushort serverPort;
	
	this()
	{
		syncObj = new Object;
	}
}

Global g;

static this()
{
	g = new Global;
}

private class ServerThread : Thread
{
	Socket sock;
	
	override int run()
	{
		writefln("Incoming from %s", sock.remoteAddress().toString());
		
		try
		{
			Stream s = new SocketStream(sock);
			
			if(auto m = search(readLine(s, MAX_LINE_LEN), `^([^ ]+) ([^ ]+) (.*)$`))
			{
				if(m.match(2) == "/?quit")
				{
					g.terminate = true;
				}
			}
			
			while(readLine(s, MAX_LINE_LEN).length > 0){}
			
			ubyte[] data;
			bool succeed = false;
			
			synchronized(g.syncObj)
			{
				if(g.pcraw)
				{
					if(g.pcraw.getLastError() < 0)
					{
						delete g.pcraw;
					}
					else
					{
						uint updateTime;
						
						updateTime = g.pcraw.getUpdateTime();
						
						if(updateTime && g.lastUpdateTime <= updateTime)
						{
							writefln("Returned PCRaw data.");
							
							data = g.pcraw.getData();
							g.lastUpdateTime = updateTime;
							succeed = true;
						}
					}
				}
				
				if(!succeed)
				{
					try
					{
						if(std.c.time.time(null) - g.cacheUpdateTime >= 60*10)
						{
							data = httpGet(g.indexTxtAddress);
							
							writefln("Returned YP data.");
							
							g.cache = data.dup;
							g.cacheUpdateTime = std.c.time.time(null);
						}
						else
						{
							writefln("Returned YP cache.");
							
							data = g.cache.dup;
						}
						
						IndexTxtInfo info;
						
						if(g.pcraw is null && extractIndexTxtInfo(cast(string)data, info))
						{
							try
							{
								g.pcraw = new PCRaw(format("http://%s/stream/%s?tip=%s:%d", g.localAddress, info.id, info.host, info.port));
							}
							catch(PCRawException e)
							{
								writefln("PCRaw initialize error.");
								g.terminate = true;
							}
						}
					}
					catch(HttpException e)
					{
						writefln("index.txt get failure.");
						
						data = g.cache.dup;
					}
				}
			}
			
			void writeResponse(ubyte[] res)
			{
				s.writeString("HTTP/1.1 200 OK\r\n");
				s.writeString("Connection: close\r\n");
				s.writeString("Content-Type: text/plain\r\n");
				s.writeString(format("Content-Length: %d\r\n", res.length));
				s.writeString("\r\n");
				s.writeExact(res.ptr, res.length);
			}
			
			writeResponse(data);
		}
		catch(StreamException e)
		{
			writefln(e.msg);
		}
		catch(SocketException e)
		{
			writeLog(e.msg);
		}
		finally
		{
			sock.shutdown(SocketShutdown.BOTH);
			sock.close();
		}
		
		return 0;
	}
}

version(Win32)
{
	import std.c.windows.windows;
}
else
{
	import std.c.linux.linux;
	
	extern(C)
	{
		alias void (*sighandler_t)(int);
		sighandler_t signal(int signum, sighandler_t sighandler);
		
		alias uint pthread_t;
		int pthread_detach(pthread_t);
	}
}

void server(ushort port)
{
	Socket sock = new SocketEx(AddressFamily.INET, SocketType.STREAM);
	sock.setOption(SocketOptionLevel.SOCKET, SocketOption.REUSEADDR, true);
	sock.bind(new InternetAddress(port));
	sock.listen(5);
	
	writefln("pcraw_srv startup.");
	
	try
	{
		while(!g.terminate)
		{
			Socket cs = sock.accept();
			ServerThread thread = new ServerThread;
			
			thread.sock = cs;
			thread.start();
			thread.wait();
			
			version(Win32)
			{
				CloseHandle(thread.hdl);
			}
			else
			{
				pthread_detach(thread.id);
			}
			
			thread = null;
		}
	}
	catch(SocketException e)
	{
		writeLog("Error: %s", e.msg);
	}
	
	writefln("pcraw_srv terminated.");
}

void usage()
{
	writefln("Usage:");
	writefln("    pcraw_srv <Server port> <PeerCast address> <YP URL>");
	writefln("Example:");
	writefln("    pcraw_srv 8080 localhost:7144 http://yp.hoge.com/");
}

int main(char[][] args)
{
	if(args.length != 4)
	{
		usage();
		return 0;
	}
	
	version(Win32)
	{
	}
	else
	{
		signal(SIGPIPE, SIG_IGN);
	}
	
	g.serverPort = toUshort(args[1]);
	g.localAddress = args[2];
	g.indexTxtAddress = args[3] ~ "index.txt?host=" ~ g.localAddress;
	
	ubyte[] data;
	
	try
	{
		data = httpGet(g.indexTxtAddress);
		
		g.cache = data;
		g.cacheUpdateTime = std.c.time.time(null);
	}
	catch(HttpException e)
	{
		writefln("Invalid URL. ", e.msg);
		return -1;
	}
	
	IndexTxtInfo info;
	
	if(extractIndexTxtInfo(cast(string)data, info))
	{
		server(g.serverPort);
	}
	else
	{
		writefln("non-PCRaw-compliant YP.");
	}
	
	return 0;
}
