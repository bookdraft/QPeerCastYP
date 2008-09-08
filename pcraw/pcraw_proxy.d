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
import std.cstream;

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
	bool terminate;
	Object syncObj;
	
	ushort serverPort;
	string localAddress;
	
	Socket sock;
	
	PCRawInfo[string] cache;
	
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

struct PCRawInfo
{
	ubyte[] data;
	uint dataUpdateTime;
	PCRaw pcraw;
	uint pcrawUpdateTime;
}

private class ServerThread : Thread
{
	Socket sock;
	
	void writeResponse(Stream s, ubyte[] res)
	{
		s.writeString("HTTP/1.1 200 OK\r\n");
		s.writeString("Connection: close\r\n");
		s.writeString("Content-Type: text/plain\r\n");
		s.writeString(format("Content-Length: %d\r\n", res.length));
		s.writeString("\r\n");
		s.writeExact(res.ptr, res.length);
	}
	
	private bool requestHandler(Stream s, string host, ushort port, string object)
	{
		if(std.string.find(object, "/index.txt") >= 0)
		{
			PCRawInfo info;
			string url = format("http://%s:%d%s", host, port, object);
			ubyte[] data;
			bool succeed = false;
			
			synchronized(g.syncObj)
			{
				if(url in g.cache)
					info = g.cache[url];
				
				
				if(info.pcraw)
				{
					if(info.pcraw.getLastError() < 0)
					{
						delete info.pcraw;
					}
					else
					{
						uint t;
						
						t = info.pcraw.getUpdateTime();
						
						if(t && info.pcrawUpdateTime <= t)
						{
							writefln("Returned PCRaw data.");
							
							data = info.pcraw.getData();
							info.pcrawUpdateTime = t;
							succeed = true;
						}
					}
				}
				
				if(!succeed)
				{
					try
					{
						if(std.c.time.time(null) - info.dataUpdateTime >= 60*10)
						{
							data = httpGet(url);
							
							writefln("Returned YP data.");
							
							info.data = data.dup;
							info.dataUpdateTime = std.c.time.time(null);
						}
						else
						{
							writefln("Returned YP cache.");
							
							data = info.data.dup;
						}
						
						IndexTxtInfo txtinfo;
						
						if(info.pcraw is null && extractIndexTxtInfo(cast(string)data, txtinfo))
						{
							try
							{
								info.pcraw = new PCRaw(format("http://%s/stream/%s?tip=%s:%d", g.localAddress, txtinfo.id, txtinfo.host, txtinfo.port));
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
						
						data = info.data.dup;
					}
				}
			}
			
			writeResponse(s, data);
			
			g.cache[url] = info;
			
			return true;
		}
		
		return false;
	}
	
	override int run()
	{
		writefln("Incoming from %s", sock.remoteAddress().toString());
		
		Socket ssock;
		
		do
		{
			try
			{
				string header;
				string[string] attr;
				ubyte[] subject;
				
				string host, object;
				ushort port;
				
				Stream s = new SocketStream(sock);
				
				try
				{
					httpGetRaw(s, header, attr, subject);
				}
				catch(HttpException e)
				{
					throw new StreamException("ServerThread.run() httpGetRaw error. (1) " ~ e.msg);
				}
				
				if(auto m = search(header, `^([^ ]+) http://([^ :/]+)(:([0-9]+))?([^ ]*) (HTTP/[0-9.]+)$`))
				{
					host = m.match(2);
					port = m.match(4) ? toUshort(m.match(4)) : 80;
					if(m.match(5) && m.match(5) != "")
						object = m.match(5);
					else
						object = "/";
					
					if(requestHandler(s, host, port, object))
						break;
					
					header = format("%s %s %s", m.match(1), object, m.match(6));
				}
				else if(auto m = search(header, `^([^ ]+) /http://([^ :/]+)(:([0-9]+))?([^ ]*) (HTTP/[0-9.]+)$`))
				{
					host = m.match(2);
					port = m.match(4) ? toUshort(m.match(4)) : 80;
					if(m.match(5) && m.match(5) != "")
						object = m.match(5);
					else
						object = "/";
					
					if(requestHandler(s, host, port, object))
						break;
					
					header = format("%s %s %s", m.match(1), object, m.match(6));
					attr["Host"] = format(host, ":", port);
				}
				else if(auto m = search(header, `^([^ ]+) ([^ ]+) (HTTP/[0-9.]+)$`))
				{
					writeResponse(s, cast(ubyte[])"");
					
					if(m.match(2) == "/?quit")
					{
						g.terminate = true;
						g.sock.close();
					}
					
					break;
				}
				else
				{
					throw new StreamException("ServerThread.run() Invalid HTTP header.");
				}
				
				attr["Connection"] = "close";
				
				if(ssock is null)
					ssock = new TcpSocketEx(new InternetAddress(host, port));
				Stream ss = new SocketStream(ssock);
				
				httpPutRaw(ss, header, attr, subject);
				
				try
				{
					httpGetRaw(ss, header, attr, subject);
				}
				catch(HttpException e)
				{
					throw new StreamException("ServerThread.run() httpGetRaw error. (2) " ~ e.msg);
				}
				
				httpPutRaw(s, header, attr, subject);
			}
			catch(StreamException e)
			{
				writeLog(e.msg);
			}
			catch(SocketException e)
			{
				writeLog(e.msg);
			}
			finally
			{
				if(ssock)
				{
					ssock.shutdown(SocketShutdown.BOTH);
					ssock.close();
				}
				
				sock.shutdown(SocketShutdown.BOTH);
				sock.close();
			}
		}while(false);
		
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
	g.sock = new SocketEx(AddressFamily.INET, SocketType.STREAM);
	g.sock.setOption(SocketOptionLevel.SOCKET, SocketOption.REUSEADDR, true);
	g.sock.bind(new InternetAddress("127.0.0.1", port));
	g.sock.listen(10);
	
	writefln("pcraw_proxy startup.");
	
	try
	{
		while(!g.terminate)
		{
			Socket cs = g.sock.accept();
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
	
	writefln("pcraw_proxy terminated.");
}

void usage()
{
	writefln("Usage:");
	writefln("    pcraw_proxy <Server port> <PeerCast address>");
	writefln("Example:");
	writefln("    pcraw_proxy 8080 localhost:7144");
}

int main(char[][] args)
{
	if(args.length != 3)
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
	
	server(g.serverPort);
	
	return 0;
}
