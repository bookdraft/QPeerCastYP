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

module pcraw.common;

import std.format;
import std.string;

debug
{
	version(Library)
	{
		version(Win32)
		{
			import win32.windows;
		}
	}
}

version(DigitalMars)
{
	wchar[] wformat(...)
	{
		wchar[] wstr;
		doFormat( (dchar c){ wstr ~= c; }, _arguments, _argptr);
		return wstr;
	}

	wchar[] wformat(TypeInfo[] argments, void* argptr)
	{
		wchar[] wstr;
		doFormat( (dchar c){ wstr ~= c; }, argments, argptr);
		return wstr;
	}
}

void writeLog(...)
{
	debug
	{
		version(Library)
		{
			version(Win32)
			{
				wchar[] wstr;
				
				wstr = wformat(_arguments, _argptr);
				wstr = wformat("[%08x] ", GetCurrentThreadId(), wstr, "\0");
				
				synchronized
				{
					OutputDebugStringW(wstr.ptr);
				}
			}
		}
		else
		{
		}
	}
}
