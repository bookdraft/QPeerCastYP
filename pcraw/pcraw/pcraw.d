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

module pcraw.pcraw;

import std.stdio;
import std.c.time;
import std.conv;
import std.stream;
import std.thread;
import std.socket;
import std.socketstream;
import std.regexp;
import std.string;

import pcraw.all;
import pcraw.socket;
import pcraw.pcrp;

private const uint VERSION = 0x00030106;
const string USER_AGENT = "PCRaw/0.3.1.6";
const ushort PEERCAST_DEFAULT_PORT = 7144;

private const uint MARKER = 0xFFAA0055;
private const uint MAX_DATA_SIZE = 1_000_000;
private const uint NO_COUNT = 123456789;

private const uint TIMEOUT = 60;
private const uint RECONNECT_WAIT = 30;
private const uint MAX_RECONNECT = 3;
private const uint MAX_SYNC_ERROR = 3;
private const uint MAX_SYNC_ERROR_LIMIT = 3;
private const float MIN_SKIP_AMP = 0.75;
private const uint RECONNECT_RESET_COUNT = 3;

private class Global
{
	bool usable = true;
	bool versionUpNotifyShowed;
	uint syncErrorCount;
	uint typeErrorCount;
	Object syncObj;
	
	this()
	{
		syncObj = new Object;
	}
}

private Global g;

static this()
{
	g = new Global;
}

private class RecvThread : Thread
{
private:
	int syncErrorCount, succeedCount, reconnectCount;
	uint typeErrorCount, lastSendtime, lastRecvtime;
	
	void processPCRP(Stream s)
	{
		ID4 id;
		uint sendTime;
		PCRP pcrp = new PCRP(s, pcraw.info);
		
		pcrp.readId(id);
		if(id != PCRP_START)
			throw new StreamExceptionEx("RecvThread.processPCRP() Invalid data start.");
		
		for(uint i=0; i<PCRP.MAX_DATA_NUM; i++)
		{
			pcrp.readId(id);
			
			switch(id)
			{
			case PCRP_END:
				break;
				
			case PCRP_LIFETIME:
				ubyte[] temp = pcrp.readData();
				
				if(temp.length == uint.sizeof)
				{
					pcrp.fixBO(temp.ptr, temp.length);
					pcraw.info.keyMgr.lifeTime = *cast(uint*)temp.ptr;
				}
				break;
				
			case PCRP_SENDTIME:
				ubyte[] temp = pcrp.readData();
				
				if(temp.length == uint.sizeof)
				{
					uint t;
					bool skip;
					
					pcrp.fixBO(temp.ptr, temp.length);
					sendTime = *cast(uint*)temp.ptr;
					
					t = time(null);
					skip = (sendTime - lastSendtime) > (t - lastRecvtime) / MIN_SKIP_AMP;
					
					if(lastRecvtime == 0)
						skip = true;
					
					writeLog("Time: %d/%d Diff: %d/%d", sendTime, t, sendTime - lastSendtime, t - lastRecvtime);
					
					lastSendtime = sendTime;
					lastRecvtime = t;
					
					if(skip)
					{
						writeLog("データ処理スキップ...");
						
						try
						{
							for(; i<PCRP.MAX_DATA_NUM; i++)
							{
								pcrp.readId(id);
								
								if(id == PCRP_END)
									break;
								else
									pcrp.skipData();
							}
						}
						catch(StreamExceptionEx e)
						{
							throw new StreamExceptionEx(e.msg, NO_COUNT);
						}
					}
				}
				break;
				
			case PCRP_KEYN:
				ubyte[] temp = pcrp.readData();
				
				if(temp.length == 16)
					pcraw.info.keyMgr.add(temp);
				break;
				
			case PCRP_DATA:
				ubyte[] data = pcrp.readData();
				
				synchronized(g.syncObj)
				{
					g.syncErrorCount = 0;
				}
				syncErrorCount = 0;
				succeedCount++;
				
				if(succeedCount >= RECONNECT_RESET_COUNT)
				{
					writeLog("データ格納.");
					
					synchronized(pcraw.syncObj)
					{
						pcraw.data = data;
						
						if(sendTime)
							pcraw.updateTime = sendTime;
						else
							pcraw.updateTime = time(null);
					}
				}
				
				pcraw.error = 0;
				
				break;
				
			default:
				writeLog("データスキップ. " ~ id);
				pcrp.skipData();
			} // switch(id)
			
			if(id == PCRP_END)
				break;
		} // for(uint i=0; i<PCRP.MAX_DATA_NUM; i++)
		
		if(id != PCRP_END)
			throw new StreamExceptionEx("RecvThread.processPCRP() Invalid data end.");
	}
	
	void processStream(Stream s)
	{
		syncErrorCount = succeedCount = 0;
		
		while(!terminate)
		{
			try
			{
				if(syncErrorCount >= MAX_SYNC_ERROR)
				{
					synchronized(g.syncObj)
					{
						g.syncErrorCount++;
						
						if(typeErrorCount >= MAX_SYNC_ERROR)
							g.typeErrorCount++;
					}
					
					throw new AbortException("同期エラー限界.");
				}
				
				{
					uint marker, count;
					
					writeLog("同期マーカを探索中...");
					
					do
					{
						ubyte c;
						
						s.read(c);
						marker <<= 8;
						marker |= c;
						count++;
					}while(!terminate && marker != MARKER && count < MAX_DATA_SIZE*2);
					
					if(marker != MARKER || count >= MAX_DATA_SIZE*2)
						throw new StreamExceptionEx("同期マーカが見つかりません.");
					
					writeLog("同期マーカを発見.");
				}
				
				ubyte ver;
				
				s.read(ver);
				if(ver != STREAM_VERSION)
				{
					typeErrorCount++;
					
					throw new StreamExceptionEx("ストリームバージョンが不正.");
				}
				
				processPCRP(s);
				
				if(succeedCount >= RECONNECT_RESET_COUNT)
					reconnectCount = 0;
			}
			catch(StreamExceptionEx e)
			{
				writeLog(e.msg);
				
				if(e.err != NO_COUNT)
				{
					syncErrorCount++;
					pcraw.error = 1;
				}
			}
			catch(StreamException e)
			{
				writeLog(e.msg);
				
				syncErrorCount++;
				pcraw.error = 1;
			}
		} // while(!terminate)
	}
	
	void processConnect()
	{
		while(!terminate)
		{
			try
			{
				typeErrorCount = lastSendtime = lastRecvtime = 0;
				
				if(reconnectCount >= MAX_RECONNECT)
					throw new AbortException("再接続限界.");
				
				reconnectCount++;
				
				if(pcraw.error)
				{
					writeLog("待機中...");
					
					for(int i; i<RECONNECT_WAIT*10 && !terminate; i++)
						usleep(100*1000);
					
					if(terminate)
						throw new AbortException("待機中断.");
				}
				
				writeLog("接続中...");
				
				scope Socket sock = new TcpSocketEx(new InternetAddress(pcraw.info.localHost, pcraw.info.localPort));
				Stream s = new SocketStream(sock);
				
				SocketEx.setTimeout(sock, TIMEOUT*1000);
				
				synchronized(pcraw.syncObj)
				{
					pcraw.sock = sock;
				}
				
				scope(exit)
				{
					synchronized(pcraw.syncObj)
					{
						pcraw.sock = null;
					}
				}
				
				writeLog("接続成功.");
				writeLog("HTTPヘッダ送信中...");
				
				s.writef("GET %s HTTP/1.0\r\n", pcraw.object);
				s.writef("Host: %s\r\n", pcraw.info.localHost);
				s.writef("User-Agent: %s\r\n", USER_AGENT);
				s.writef("\r\n");
				
				string line = s.readLine();
				if(auto m = search(line, "HTTP/[0-9.]+ ([0-9]+) .*"))
				{
					uint code = toUint(m.match(1));
					
					writeLog("HTTPレスポンスヘッダスキップ...");
					
					while(s.readLine().length > 0 && !terminate){}
					
					if(code != 200)
						throw new StreamExceptionEx(format("HTTPレスポンスヘッダエラー. code:%d", code));
				}
				else
					throw new StreamExceptionEx("HTTPレスポンスヘッダエラー.");
				
				writeLog("HTTPレスポンスヘッダ受信完了.");
				
				processStream(s);
			}
			catch(SocketException e)
			{
				writeLog(e.msg);
				
				pcraw.error = 2;
			}
			catch(AbortException e)
			{
				writeLog(e.msg);
				
				break;
			}
		} // while(!terminate)
	}
	
public:
	bool terminate = false;
	PCRaw pcraw;
	
	override int run()
	{
		syncErrorCount = succeedCount = reconnectCount = 0;
		typeErrorCount = lastSendtime = lastRecvtime = 0;
		
		scope(success)
		{
			pcraw.error = -1;
			writeLog("受信スレッドが正常終了しました.");
		}
		
		scope(failure)
		{
			pcraw.error = -2;
			writeLog("受信スレッドが異常終了しました.");
		}
		
		writeLog("受信スレッドを開始しました.");
		
		try
		{
			processConnect();
		}
		catch(Exception e)
		{
			writeLog("Fatal Error: ", e.msg);
		}
		catch(Object e)
		{
			writeLog("Fatal Error: ", e.toString());
		}
		
		return 0;
	}
}

class PCRawException : Exception
{
	this(string msg)
	{
		super(msg);
	}
}

class PCRaw
{
private:
	RecvThread thread;
	Socket sock;
	PCRPInfo info;
	int error;
	uint updateTime;
	string object;
	ubyte[] data;
	Object syncObj;
	bool closed = false;
	
public:
	this(string url)
	{
		syncObj = new Object;
		
		writeLog("PCRaw.this(%s)", url);
		
		if(!g.usable)
			throw new PCRawException("PCRaw 使用不可.");
		
		if(g.typeErrorCount >= MAX_SYNC_ERROR_LIMIT)
			throw new PCRawException("PCRaw ストリームタイプエラー多発.");
		
		if(g.syncErrorCount >= MAX_SYNC_ERROR_LIMIT)
			throw new PCRawException("PCRaw 同期エラー多発.");
		
		if(auto m = search(url, "http://([^:/]+)(:([0-9]+))?(/[^/]+/([0-9a-fA-F]+)\\?tip=([^:&]+)(:([0-9]+))?)"))
		{
			info.localHost = m.match(1).dup;
			info.localPort = m.match(3) ? toUshort(m.match(3)) : 80;
			info.trackerHost = m.match(6).dup;
			info.trackerPort = m.match(8) ? toUshort(m.match(8)) : PEERCAST_DEFAULT_PORT;
			object = m.match(4).dup;
		}
		else
			throw new PCRawException("PCRaw URL不正.");
		
		info.keyMgr = new KeyManager;
		thread = new RecvThread;
		thread.pcraw = this;
		
		thread.start();
		
		writeLog("PCRaw.this(%s) = %08x", url, thread);
	}
	
	~this()
	{
		close();
	}
	
	void close()
	{
		writeLog("[%08x] PCRaw.close()", thread);
		
		if(!closed)
		{
			thread.terminate = true;
			
			synchronized(syncObj)
			{
				Socket s = sock;
				
				if(s)
					s.close();
			}
			
			try
			{
				thread.wait();
			}
			catch(ThreadError e){}
			
			closed = true;
		}
	}
	
	uint getLength()
	{
		synchronized(syncObj)
		{
			writeLog("[%08x] PCRaw.getLength() = %d", thread, data.length);
			
			return data.length;
		}
	}
	
	ubyte[] getData()
	{
		synchronized(syncObj)
		{
			writeLog("[%08x] PCRaw.getData() = %d", thread, data.length);
			
			return data.dup;
		}
	}
	
	int getLastError()
	{
		synchronized(syncObj)
		{
			writeLog("[%08x] PCRaw.getLastError() = %d", thread, error);
			
			return error;
		}
	}
	
	uint getUpdateTime()
	{
		synchronized(syncObj)
		{
			writeLog("[%08x] PCRaw.getUpdateTime() = %d", thread, updateTime);
			
			return updateTime;
		}
	}
	
	void dataLock()
	{
		writeLog("[%08x] PCRaw.dataLock()", thread);
	}
	
	void dataUnlock()
	{
		writeLog("[%08x] PCRaw.dataUnlock()", thread);
	}
}

int pcrawCheckVersion(out uint ver, uint assumedVer)
{
	ver = VERSION;
	
	if(assumedVer)
	{
		if((VERSION&0xffff0000) > (assumedVer&0xffff0000))
		{
			writeLog("想定するバージョンより新しい.");
			g.usable = false;
			return 1;
		}
		
		if(VERSION < assumedVer)
		{
			writeLog("想定するバージョンより古い.");
			g.usable = false;
			return -1;
		}
		
		writeLog("想定するバージョンと適合.");
		
		g.usable = true;
		return 0;
	}
	
	return 0;
}
