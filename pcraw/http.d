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

module http;

import std.stdio;
import std.stream;
import std.socket;
import std.socketstream;
import std.string;
import std.regexp;
import std.conv;
import std.zlib;
import std.array;

import tools;
import pcraw.socket;

private enum
{
	MAX_LINE_LEN = 4096,
}

class HttpException : Exception
{
	this(string msg){ super(msg); }
}

private uint checkGzipHeader(ubyte[] data)
{
	const ubyte[] GZ_MAGIC = [0x1f, 0x8b];
	
	enum
	{
		ASCII_FLAG	= 0x01,
		HEAD_CRC	= 0x02,
		EXTRA_FIELD	= 0x04,
		ORIG_NAME	= 0x08,
		COMMENT		= 0x10,
		RESERVED	= 0xE0,
		Z_DEFLATED	= 0x08,
	}
	
	uint i = 2;
	
	try
	{
		if(data[0..2] != GZ_MAGIC)
			throw new HttpException("checkGzipHeader() gzip header error. (1)");
		
		auto method = data[i++];
		auto flags  = data[i++];
		
		if(method != Z_DEFLATED || (flags & RESERVED) != 0)
			throw new HttpException("checkGzipHeader() gzip flag error.");
		
		i+=6;
		
		if(flags & EXTRA_FIELD)
		{
			uint len = 0;
			
			len  = data[i++];
			len |= data[i++] << 8;
			
			i += len;
		}
		
		if(flags & ORIG_NAME)
			while(data[i++]){}
		
		if(flags & COMMENT)
			while(data[i++]){}
		
		if(flags & HEAD_CRC)
			i+=2;
	}
	catch(ArrayBoundsError e)
	{
		throw new HttpException("checkGzipHeader() gzip header error. (2)");
	}
	
	return i;
}

void httpGetRaw(Stream s, out string head, out string[string] attr, out ubyte[] subject)
{
	string line;
	
	head = readLine(s, MAX_LINE_LEN);
	
	while((line = readLine(s, MAX_LINE_LEN)).length > 0)
	{
		if(auto m = search(line, `([^:]+):(.*)`))
			attr[trim(m.match(1))] = trim(m.match(2));
		else
			throw new HttpException("httpGetRaw() Invalid HTTP header.");
	}
	
	if(std.string.find(head, "POST") == 0 || std.string.find(head, "HTTP") == 0)
	{
		if("Content-Length" in attr)
		{
			auto len = toUint(attr["Content-Length"]);
			
			subject = new ubyte[len];
			s.readExact(subject.ptr, subject.length);
		}
		else
		{
			ubyte[] buf = new ubyte[4096];
			uint len;
			
			try
			{
				while((len = s.read(buf)) > 0)
					subject ~= buf[0 .. len];
			}
			catch(SocketException e){}
		}
	}
	
	attr["Content-Length"] = std.string.toString(subject.length);
}

void httpPutRaw(Stream s, string head, string[string] attr, ubyte[] subject)
{
	s.writeString(head);
	s.writeString("\r\n");
	
	attr["Content-Length"] = std.string.toString(subject.length);
	
	foreach(key, value; attr)
		s.writeString(format("%s: %s\r\n", key, value));
	s.writeString("\r\n");
	
	s.writeExact(subject.ptr, subject.length);
}

ubyte[] httpGet(string url)
{
	string host, object;
	ushort port;
	
	if(auto m = search(url, `http://([^:/]+):?([0-9]+)?(.+)`))
	{
		host = m.match(1);
		port = m.match(2) ? toUshort(m.match(2)) : 80;
		object = m.match(3);
	}
	else
		throw new HttpException("httpGet() Invalid URL.");
	
	Socket sock = new TcpSocketEx(new InternetAddress(host, port));
	Stream s = new SocketStream(sock);
	
	scope(exit)
	{
		sock.shutdown(SocketShutdown.BOTH);
		sock.close();
	}
	
	s.writeString(format("GET %s HTTP/1.0\r\n", object));
	s.writeString("Accept-Encoding: gzip\r\n");
	s.writeString(format("User-Agent: Mozilla/4.0 (PCRaw)\r\n"));
	s.writeString(format("Host: %s:%d\r\n", host, port));
	s.writeString("Connection: close\r\n");
	s.writeString("\r\n");
	
	string line;
	uint code;
	
	line = readLine(s, MAX_LINE_LEN);
	if(auto m = search(line, `HTTP/[0-9.]+ ([0-9]+) .*`))
		code = toUint(m.match(1));
	else
		throw new HttpException("httpGet() Invalid HTTP header. (1)");
	
	string[string] headers;
	
	while((line = readLine(s, MAX_LINE_LEN)).length > 0)
	{
		if(auto m = search(line, `([^:]+):(.*)`))
			headers[trim(m.match(1))] = trim(m.match(2));
		else
			throw new HttpException("httpGet() Invalid HTTP header. (2)");
	}
	
	ubyte[] data;
	ubyte[] buf = new ubyte[4096];
	uint len;
	
	try
	{
		while((len = s.read(buf)) > 0)
			data ~= buf[0 .. len];
	}
	catch(SocketException e){}
	
	switch(code)
	{
	case 200:
		break;
	default:
		throw new HttpException(format("httpGet() HTTP response code %d.", code));
	}
	
	if("Content-Encoding" in headers &&
	   std.string.find(headers["Content-Encoding"], "gzip") >= 0)
	{
		data = data[checkGzipHeader(data) .. $];
		
		try
		{
			data = cast(ubyte[])uncompress(data, data.length*3, -15);
		}
		catch(ZlibException e)
		{
			throw new HttpException("httpGet() uncompress error.");
		}
	}
	
	return data;
}
