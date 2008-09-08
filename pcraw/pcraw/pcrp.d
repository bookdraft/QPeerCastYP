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

module pcraw.pcrp;

import std.system;
import std.stream;
import std.socket;
import std.md5;
import std.zlib;

import pcraw.all;

const ubyte STREAM_VERSION = 4;
const ID4 PCRP_MARKER = [0xff, 0xaa, 0x00, 0x55];
const ID4 PCRP_START = "ssss";
const ID4 PCRP_END = "eeee";
const ID4 PCRP_START_DATA = "sdds";
const ID4 PCRP_END_DATA = "edde";
const ID4 PCRP_START_ATOM = "saas";
const ID4 PCRP_END_ATOM = "eaae";
const ID4 PCRP_LIFETIME = "lift";
const ID4 PCRP_SENDTIME = "sndt";
const ID4 PCRP_DATA = "data";
const ID4 PCRP_FLAG = "flag";
const ID4 PCRP_KEYN = "keyn";
const ID4 PCRP_MD5 = "md5\0";

struct PCRPInfo
{
	KeyManager keyMgr;
	ushort localPort;
	string localHost;
	ushort trackerPort;
	string trackerHost;
}

class PCRP : EndianStream
{
private:
	PCRPInfo info;
	
public:
	enum
	{
		MAX_DATA_SIZE = 1_000_000,
		MAX_DATA_NUM = 16,
		TIMEOUT = 5000,
	}
	
	enum
	{
		FLAG_MD5	= 0x00000001,
		FLAG_RC4	= 0x00000002,
		FLAG_GZ		= 0x00000004,
		FLAG_ALL	= FLAG_MD5 | FLAG_RC4 | FLAG_GZ,
	}
	
	this(Stream source, PCRPInfo pcrpInfo)
	in
	{
		assert(pcrpInfo.keyMgr !is null);
	}
	body
	{
		super(source, Endian.LittleEndian);
		info = pcrpInfo;
	}
	
	void readId(out ID4 id)
	{
		id = new char[4];
		readExact(id.ptr, id.length);
	}
	
	ubyte[] readAtom()
	{
		ID4 id;
		uint size;
		ubyte[] data;
		
		readId(id);
		if(id != PCRP_START_ATOM)
			throw new StreamExceptionEx("PCRP.readAtom() Invalid data start.");
		
		read(size);
		if(size > MAX_DATA_SIZE)
			throw new StreamExceptionEx("PCRP.readAtom() Invalid data size.");
		
		if(size > 0)
		{
			data = new ubyte[size];
			readExact(data.ptr, data.length);
		}
		
		readId(id);
		if(id != PCRP_END_ATOM)
			throw new StreamExceptionEx("PCRP.readAtom() Invalid data end.");
		
		return data;
	}
	
	ubyte[] readData()
	{
		ID4 id;
		uint flag;
		ubyte[16] hash;
		ubyte[] data;
		
		readId(id);
		if(id != PCRP_START_DATA)
			throw new StreamExceptionEx("PCRP.readData() Invalid data start.");
		
		for(uint i=0; i<MAX_DATA_NUM; i++)
		{
			readId(id);
			
			if(id == PCRP_END_DATA)
				break;
			
			switch(id)
			{
			case PCRP_DATA:
				data = readAtom();
				break;
				
			case PCRP_FLAG:
				ubyte[] temp = readAtom();
				
				if(temp.length == uint.sizeof)
				{
					fixBO(temp.ptr, temp.length);
					flag = *cast(uint*)temp.ptr;
				}
				break;
				
			case PCRP_MD5:
				ubyte[] temp = readAtom();
				
				if(temp.length == hash.length)
					hash[] = temp;
				break;
				
			default:
				writeLog("Atomスキップ ID: ", id);
				
				readAtom();
			}
		}
		
		if(id != PCRP_END_DATA)
			throw new StreamExceptionEx("PCRP.readData() Invalid data end.");
		
		if((flag & ~FLAG_ALL) ||
		   (flag & FLAG_RC4 && !(flag & FLAG_MD5)))
			throw new StreamExceptionEx("PCRP.readData() Invalid data flag.");
		
		if(flag == 0 || data.length == 0)
		{
			return data;
		}
		else
		{
			bool succeed = false;
			
			if(flag & FLAG_RC4)
			{
				writeLog("データは暗号化されています.");
				
				try
				{
					if(info.keyMgr.count() == 0 && pcpPortCheck(info, TIMEOUT) == 0)
						throw new StreamExceptionEx("Port 0");
				}
				catch(StreamExceptionEx e)
				{
					throw new AbortException("Portcheck error. " ~ e.msg);
				}
				catch(SocketException e)
				{
					throw new AbortException("Portcheck error. " ~ e.msg);
				}
				
				uint n;
				ubyte[] temp = new ubyte[data.length];
				ubyte[16] tempHash, hash2;
				Arcfour rc4 = new Arcfour;
				
				foreach(i, k; info.keyMgr.keys)
				{
					writeLog("データの復号化を試行中... Key No.%d", i);
					
					n = i;
					
					rc4.init(k.key);
					rc4.codec(data, temp);
					
					sum(tempHash, temp);
					
					rc4.init(k.key);
					rc4.codec(hash, hash2);
					
					if(hash2 == tempHash)
					{
						writeLog("データ復号化成功.");
						
						data = temp;
						succeed = true;
						
						break;
					}
				}
				
				if(succeed)
				{
					if(n > 0)
						info.keyMgr.swap(n, 0);
					
					if(info.keyMgr.beforeKey >= 0 && n > 0)
						info.keyMgr.deleteOld();
					info.keyMgr.beforeKey = n;
				}
				else
				{
					if(info.keyMgr.deleteOld() <= 0)
						throw new AbortException("PCRP.readData() Lost keys.");
					
					throw new StreamExceptionEx("PCRP.readData() decryption failed.");
				}
			}
			else if(flag & FLAG_MD5)
			{
				ubyte[16] hash2;
				
				sum(hash2, data);
				
				if(hash == hash2)
					succeed = true;
			}
			else
				succeed = true;
			
			if(!succeed)
				throw new StreamExceptionEx("PCRP.readData() Invalid data.");
			
			if(flag & FLAG_GZ)
			{
				try
				{
					writeLog("データは圧縮されています.");
					
					data = cast(ubyte[])uncompress(data, data.length * 3);
					
					writeLog("データ展開成功.");
				}
				catch(ZlibException e)
				{
					throw new StreamExceptionEx("PCRP.readData() Uncompress error.");
				}
			}
			
			writeLog("データ確定.");
			
			return data;
		}
	}
	
	void skipData()
	{
		ID4 id;
		
		readId(id);
		if(id != PCRP_START_DATA)
			throw new StreamExceptionEx("PCRP.skipData() Invalid data start.");
		
		for(uint i=0; i<MAX_DATA_NUM; i++)
		{
			readId(id);
			
			if(id == PCRP_END_DATA)
				break;
			
			readAtom();
		}
		
		if(id != PCRP_END_DATA)
			throw new StreamExceptionEx("PCRP.skipData() Invalid data end.");
	}
}
