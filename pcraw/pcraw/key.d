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

module pcraw.key;

import std.c.time;

import pcraw.all;

struct Key
{
	ubyte[16] key;
	uint t;
}

class KeyManager
{
	Key[] keys;
	uint lifeTime;
	int beforeKey;
	
	this()
	{
		clear();
	}
	
	void clear()
	{
		keys = keys.init;
		lifeTime = 0;
		beforeKey = -1;
	}
	
	void swap(uint pos1, uint pos2)
	in
	{
		assert(keys.length > pos1);
		assert(keys.length > pos2);
	}
	body
	{
		Key temp;
		
		if(pos1 == pos2)
			return;
		
		writeLog("鍵を交換. Key No.%d ⇔ Key No.%d", pos1, pos2);
		
		temp = keys[pos2];
		keys[pos2] = keys[pos1];
		keys[pos1] = temp;
	}
	
	int add(ubyte[] key, uint t = 0)
	in
	{
		assert(key.length == 16);
	}
	body
	{
		if(t == 0)
			t = time(null);
		
		foreach(i, k; keys)
		{
			if(k.key == key)
			{
				writeLog("鍵を更新. Key No.%d", i);
				
				keys[i].t = t;
				return i;
			}
		}
		
		writeLog("鍵を追加.");
		
		Key temp;
		
		temp.key[] = key;
		temp.t = t;
		keys ~= temp;
		
		return keys.length - 1;
	}
	
	int deleteOld()
	{
		uint t = time(null);
		Key[] newKeys;
		
		foreach(k; keys)
			if(t - k.t <= lifeTime)
				newKeys ~= k;
		
		keys = newKeys;
		
		writeLog("生存中の鍵: %d", keys.length);
		
		return keys.length;
	}
	
	uint count()
	{
		return keys.length;
	}
	
	invariant()
	{
		assert(keys.length <= 16);
	}
}
