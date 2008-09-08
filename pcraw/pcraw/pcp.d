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

module pcraw.pcp;

import std.system;
import std.stream;

import pcraw.all;

alias char[] ID4;

const ID4 PCP_CONNECT = "pcp\n";
const ID4 PCP_HELO = "helo";
const ID4 PCP_OLEH = "oleh";
const ID4 PCP_HELO_SESSIONID = "sid\0";
const ID4 PCP_HELO_PING = "ping";
const ID4 PCP_HELO_PORT = "port";
const ID4 PCP_PCRAW_KEYC = "keyc";
const ID4 PCP_PCRAW_KEYN = "keyn";
const ID4 PCP_PCRAW_KEYP = "keyp";

class PCP : EndianStream
{
	enum
	{
		MAX_PACKET_SIZE = 1_000_000,
	}
	
	this(Stream source)
	{
		super(source, Endian.LittleEndian);
	}
	
	void readParent(out ID4 id, out uint nc)
	{
		id = new char[4];
		readExact(id.ptr, id.length);
		read(nc);
		
		if(!(nc & 0x80000000))
			throw new StreamExceptionEx("PCP.readParent() Invalid PCP parent.");
		
		nc &= ~0x80000000;
	}
	
	void writeParent(ID4 id, uint nc)
	in
	{
		assert(id.length == 4);
	}
	body
	{
		nc |= 0x80000000;
		writeExact(id.ptr, id.length);
		write(nc);
	}
	
	void readBin(out ID4 id, out ubyte[] buf)
	{
		uint len;
		
		id = new char[4];
		readExact(id.ptr, id.length);
		read(len);
		
		if(len > MAX_PACKET_SIZE)
			throw new StreamExceptionEx("PCP.readBin() Packet size is too big.");
		
		buf = new ubyte[len];
		readExact(buf.ptr, buf.length);
	}
	
	void writeBin(ID4 id, ubyte[] buf)
	in
	{
		assert(id.length == 4);
	}
	body
	{
		writeExact(id.ptr, id.length);
		write(buf.length);
		writeExact(buf.ptr, buf.length);
	}
	
	void readInt(T)(out ID4 id, out T n)
	{
		uint len;
		
		id = new char[4];
		readExact(id.ptr, id.length);
		read(len);
		
		if(n.length != len)
			throw new StreamExceptionEx("PCP.readInt() Invalid data type.");
		
		read(n);
	}
	
	void writeInt(T)(ID4 id, T n)
	in
	{
		assert(id.length == 4);
	}
	body
	{
		writeExact(id.ptr, id.length);
		write(n.sizeof);
		write(n);
	}
}
