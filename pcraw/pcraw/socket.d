/*************************************************************************
 * PCRaw
 * Authors: ◆e5bW6vDOJ.
 * Copyright: Copyright (C) 2007 ◆e5bW6vDOJ.
 * Date: February 11, 2007
 * License: 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

module pcraw.socket;

import std.socket;
import std.string;
import std.stdio;

version(Win32)
{


import std.c.windows.winsock;

class SocketEx : Socket
{
	this(AddressFamily af, SocketType type, ProtocolType protocol)
	{
		super(af, type, protocol);
	}
	
	this(AddressFamily af, SocketType type)
	{
		super(af, type);
	}
	
	protected this()
	{
	}
	
	override int receive(void[] buf, SocketFlags flags)
	{
		int read = super.receive(buf, flags);
		if(buf.length > 0 && read <= 0)
			throw new SocketException("SocketEx.receive() error.");
		return read;
	}
	
	override int receive(void[] buf)
	{
		return receive(buf, SocketFlags.NONE);
	}
	
	override int send(void[] buf, SocketFlags flags)
	{
		int sent = super.send(buf, flags);
		if(buf.length > 0 && sent <= 0)
			throw new SocketException("SocketEx.send() error.");
		return sent;
	}
	
	override int send(void[] buf)
	{
		return send(buf, SocketFlags.NONE);
	}
	
	override protected Socket accepting()
	{
		return new SocketEx;
	}
	
	static void setTimeout(Socket s, int millisec)
	{
		s.setOption(SocketOptionLevel.SOCKET, cast(SocketOption)SO_RCVTIMEO, millisec);
		s.setOption(SocketOptionLevel.SOCKET, cast(SocketOption)SO_SNDTIMEO, millisec);
	}
}


} // version(Win32)
else
{


import std.c.string;
import std.c.stdlib;
import std.c.linux.linux;

class SocketEx : Socket
{
	this(AddressFamily af, SocketType type, ProtocolType protocol)
	{
		super(af, type, protocol);
	}
	
	this(AddressFamily af, SocketType type)
	{
		super(af, type);
	}
	
	protected this()
	{
	}
	
	override void connect(Address to)
	{
		if(blocking)
		{
			for(;;)
			{
				try
				{
					super.connect(to);
					return;
				}
				catch(SocketException e)
				{
					int err = getErrno();
					
					if(err == EINTR || err == EAGAIN)
					{
						setErrno(0);
						continue;
					}
					
					throw e;
				}
			}
		}
		else
		{
			super.connect(to);
		}
	}
	
	override int receive(void[] buf, SocketFlags flags)
	{
		if(blocking)
		{
			int read;
			
			for(;;)
			{
				read = super.receive(buf, flags);
				
				if(buf.length > 0 && read <= 0)
				{
					int err = getErrno();
					
					if(err == EINTR || err == EAGAIN)
					{
						setErrno(0);
						continue;
					}
					
					throw new SocketException("SocketEx.receive()", err);
				}
				else
					break;
			}
			
			return read;
		}
		else
		{
			return super.receive(buf, flags);
		}
	}
	
	override int receive(void[] buf)
	{
		return receive(buf, SocketFlags.NONE);
	}
	
	override int send(void[] buf, SocketFlags flags)
	{
		if(blocking)
		{
			int sent;
			
			for(;;)
			{
				sent = super.send(buf, flags);
				
				if(buf.length > 0 && sent <= 0)
				{
					int err;
					
					if(err == EINTR || err == EAGAIN)
					{
						setErrno(0);
						continue;
					}
					
					throw new SocketException("SocketEx.send()", err);
				}
				else
					break;
			}
			
			return sent;
		}
		else
		{
			return super.send(buf, flags);
		}
	}
	
	override int send(void[] buf)
	{
		return send(buf, SocketFlags.NONE);
	}
	
	override Socket accept()
	{
		if(blocking)
		{
			for(;;)
			{
				try
				{
					return super.accept();
				}
				catch(SocketException e)
				{
					int err = getErrno();
					
					if(err == EINTR || err == EAGAIN)
					{
						setErrno(0);
						continue;
					}
					
					throw e;
				}
			}
		}
		else
		{
			return super.accept();
		}
	}
	
	override protected Socket accepting()
	{
		return new SocketEx;
	}
	
	static void setTimeout(Socket s, int millisec)
	{
		std.socket.timeval t;
		
		t.tv_sec = millisec / 1000;
		t.tv_usec = (millisec % 1000) * 1000;
		
		s.setOption(SocketOptionLevel.SOCKET, cast(SocketOption)SO_RCVTIMEO, (&t)[0..1]);
		s.setOption(SocketOptionLevel.SOCKET, cast(SocketOption)SO_SNDTIMEO, (&t)[0..1]);
	}
}


} // version(!Win32)

class TcpSocketEx: SocketEx
{
	this(AddressFamily family)
	{
		super(family, SocketType.STREAM, ProtocolType.TCP);
	}
	
	this()
	{
		this(AddressFamily.INET);
	}
	
	this(Address connectTo)
	{
		this(connectTo.addressFamily());
		connect(connectTo);
	}
}
