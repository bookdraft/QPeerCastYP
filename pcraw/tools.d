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

module tools;

import std.regexp;
import std.stream;

string trim(string str)
{
	if(auto m = search(str, `[ \t\r\n]*(.+)[ \t\r\n]*`))
		return m.match(1);
	else
		return str;
}

string readLine(Stream s, uint maxlen)
{
	char c;
	string ret;
	uint i;
	
	for(i=0; i<maxlen; i++)
	{
		s.read(c);
		if(c == '\n')
			break;
		ret ~= c;
	}
	
	if(i >= maxlen)
		throw new StreamException("readLine() Line is too long.");
	
	if(ret.length > 0 && ret[$-1] == '\r')
		ret = ret[0 .. $-1];
	
	return ret;
}
