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

module pcraw.pcptools;

import std.socket;
import std.stream;
import std.socketstream;
import std.c.time;

import pcraw.all;
import pcraw.pcrp;

private void pcpGetSid(string host, ushort port, ubyte[16] sid, int timeout)
in
{
	assert(host.length > 0);
	assert(port > 0);
	assert(timeout > 0);
}
body
{
	Socket sock = new TcpSocketEx(new InternetAddress(host, port));
	Stream s = new SocketStream(sock);
	PCP pcp = new PCP(s);
	
	SocketEx.setTimeout(sock, timeout);
	
	pcp.writeInt(PCP_CONNECT, 1);
		pcp.writeParent(PCP_HELO, 0);
	
	ID4 id;
	uint count;
	bool succeed = false;
	
	pcp.readParent(id, count);
	
	if(id != PCP_OLEH)
		throw new StreamExceptionEx("Local server is not PeerCast.");
	
	for(int i=0; i<count+1; i++)
	{
		ubyte[] buf;
		
		pcp.readBin(id, buf);
		
		if(id == PCP_HELO_SESSIONID && buf.length == sid.length)
		{
			sid[] = buf;
			succeed = true;
		}
	}
	
	if(!succeed)
		throw new StreamExceptionEx("Get SID failed.");
}

int pcpPortCheck(PCRPInfo info, int timeout)
in
{
	assert(info.keyMgr !is null);
	assert(info.localHost.length > 0);
	assert(info.localPort > 0);
	assert(info.trackerHost.length > 0);
	assert(info.trackerPort > 0);
	assert(timeout > 0);
}
body
{
	ubyte[16] sid;
	
	info.keyMgr.clear();
	
	pcpGetSid(info.localHost, info.localPort, sid, timeout);
	
	Socket sock = new TcpSocketEx(new InternetAddress(info.trackerHost, info.trackerPort));
	Stream s = new SocketStream(sock);
	PCP pcp = new PCP(s);
	
	SocketEx.setTimeout(sock, timeout);
	
	pcp.writeInt(PCP_CONNECT, 1);
		pcp.writeParent(PCP_HELO, 2);
			pcp.writeBin(PCP_HELO_SESSIONID, sid);
			pcp.writeInt(PCP_HELO_PING, info.localPort);
	
	ID4 id;
	uint count;
	ushort port = 0;
	
	pcp.readParent(id, count);
	
	if(id != PCP_OLEH)
		throw new StreamExceptionEx("Tracket is not PeerCast.");
	
	for(int i; i<count+1; i++)
	{
		ubyte[] buf;
		
		pcp.readBin(id, buf);
		
		switch(id)
		{
		case PCP_HELO_PORT:
			if(buf.length != ushort.sizeof)
				throw new StreamExceptionEx("pcpPortCheck() Invalid PCP data. (1)");
			pcp.fixBO(buf.ptr, buf.length);
			port = *cast(ushort*)buf.ptr;
			break;
			
		case PCP_PCRAW_KEYC:
			writeLog("現在の鍵をセット.");
			
			if(buf.length != Key.key.length)
				throw new StreamExceptionEx("pcpPortCheck() Invalid PCP data. (2)");
			info.keyMgr.add(buf);
			break;
			
		case PCP_PCRAW_KEYN:
			writeLog("次の鍵をセット.");
			
			if(buf.length != Key.key.length)
				throw new StreamExceptionEx("pcpPortCheck() Invalid PCP data. (3)");
			info.keyMgr.add(buf, time(null) + info.keyMgr.lifeTime);
			break;
			
		case PCP_PCRAW_KEYP:
			writeLog("前の鍵をセット.");
			
			if(buf.length != Key.key.length)
				throw new StreamExceptionEx("pcpPortCheck() Invalid PCP data. (4)");
			info.keyMgr.add(buf, time(null) - info.keyMgr.lifeTime);
			break;
			
		default:
		}
	}
	
	return port;
}
