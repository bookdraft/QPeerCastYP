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

import std.c.windows.windows;
import std.c.string;

import envcheck;
import pcraw.pcraw;

HINSTANCE g_hInstance;

extern(C)
{
	void gc_init();
	void gc_term();
	void _minit();
	void _moduleCtor();
	void _moduleDtor();
	void _moduleUnitTests();
}

extern(Windows)
BOOL DllMain(HINSTANCE hInstance, ULONG ulReason, LPVOID pvReserved)
{
	switch (ulReason)
	{
	case DLL_PROCESS_ATTACH:
		gc_init();
		_minit();
		_moduleCtor();
		_moduleUnitTests();
		break;
		
	case DLL_PROCESS_DETACH:
		_moduleDtor();
		gc_term();
		break;
		
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		return false;
	}
	
	g_hInstance = hInstance;
	
	return true;
}

PCRaw[void*] pcraws;
Object syncObj;

static this()
{
	syncObj = new Object;
}

extern(C)
{
	void* PCRawCreate(char* url)
	{
		int len = strlen(url);
		
		try
		{
			PCRaw pcraw = new PCRaw(url[0 .. len]);
			synchronized(syncObj)
			{
				pcraws[cast(void*)pcraw] = pcraw;
			}
			return cast(void*)pcraw;
		}
		catch(PCRawException e)
		{
			return null;
		}
	}
	
	void PCRawClose(void* handle)
	{
		synchronized(syncObj)
		{
			if(handle in pcraws)
			{
				pcraws[handle].close();
				pcraws.remove(handle);
			}
		}
	}
	
	uint PCRawGetLength(void* handle)
	{
		synchronized(syncObj)
		{
			if(handle in pcraws)
				return pcraws[handle].getLength();
			else
				return 0;
		}
	}
	
	uint PCRawGetData(void* handle, char* buf, int len)
	{
		int l;
		ubyte[] data;
		
		synchronized(syncObj)
		{
			if(handle in pcraws)
				data =  pcraws[handle].getData();
		}
		
		l = data.length < len ? data.length : len;
		memcpy(buf, data.ptr, l);
		
		return l;
	}
	
	int PCRawGetLastError(void* handle)
	{
		synchronized(syncObj)
		{
			if(handle in pcraws)
				return pcraws[handle].getLastError();
			else
				return 0;
		}
	}
	
	uint PCRawGetUpdateTime(void* handle)
	{
		synchronized(syncObj)
		{
			if(handle in pcraws)
				return pcraws[handle].getUpdateTime();
			else
				return 0;
		}
	}
	
	void PCRawDataLock(void* handle)
	{
		PCRaw pcraw;
		
		synchronized(syncObj)
		{
			if(handle in pcraws)
				pcraw = pcraws[handle];
		}
		
		pcraw.dataLock();
	}
	
	void PCRawDataUnlock(void* handle)
	{
		PCRaw pcraw;
		
		synchronized(syncObj)
		{
			if(handle in pcraws)
				pcraw = pcraws[handle];
		}
		
		pcraw.dataUnlock();
	}
	
	int PCRawCheckVersion(uint* lpVersion, uint dwAssumedVersion)
	{
		uint ver;
		int ret;
		
		ret = pcrawCheckVersion(ver, dwAssumedVersion);
		
		if(lpVersion)
			*lpVersion = ver;
		
		return ret;
	}
}
