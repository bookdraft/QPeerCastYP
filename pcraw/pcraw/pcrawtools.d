module pcraw.pcrawtools;

import std.conv;
import std.regexp;
import std.string;

import pcraw.all;

struct IndexTxtInfo
{
	string host;
	ushort port;
	string id;
}

private bool isIndexTxt(string[] items)
{
	if(items.length != 19)
		return false;
	
	if(items[9] == "RAW" &&
	   items[4] == "" &&
	   search(items[0], `^index(_[a-z]+)?\.txt$`))
		return true;
	else
		return false;
}

bool extractIndexTxtInfo(string data, out IndexTxtInfo info)
{
	auto lines = splitlines(data);
	
	foreach(line; lines)
	{
		auto items = std.string.split(line, "<>");
		
		if(isIndexTxt(items))
		{
			info.id = items[1].dup;
			
			if(auto m = search(items[2], `([^:]+):?([0-9]+)?`))
			{
				info.host = m.match(1).dup;
				info.port = m.match(2) ? toUshort(m.match(2)) : PEERCAST_DEFAULT_PORT;
				
				return true;
			}
		}
	}
	
	return false;
}
