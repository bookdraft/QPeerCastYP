module pcraw.arcfour;

class Arcfour
{
private:
	uint x, y;
	ubyte[256] key;

public:
	~this()
	{
		uninit();
	}
	
	void init(ubyte[] pass)
	in
	{
		assert(pass.length > 0);
	}
	body
	{
		x = y = 0;
		
		foreach(i, inout c; key)
			c = i;
		
		int j;
		foreach(i, c; key)
		{
			j = (j + c + pass[i % length]) & 0xff;
			key[i] = key[j];
			key[j] = c;
		}
	}
	
	void uninit()
	{
		x = y = 0;
		
		foreach(inout c; key)
			c = 0;
	}
	
	void codec(ubyte[] src, ubyte[] dst)
	in
	{
		assert(src.length <= dst.length);
	}
	body
	{
		ubyte n, m;
		uint pos;
		
		foreach(i, c; src)
		{
			x = (x + 1) & 0xff;
			n = key[x];
			y = (y + n) & 0xff;
			m = key[y];
			key[x] = m;
			key[y] = n;
			pos = (m + n) & 0xff;
			
			dst[i] = c ^ key[pos];
		}
	}
	
	unittest
	{
		Arcfour rc4 = new Arcfour;
		
		ubyte[] plain = [0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00];
		ubyte[] key = [0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef];
		ubyte[] cipher = [0x74, 0x94, 0xc2, 0xe7, 0x10, 0x4b, 0x08, 0x79];
		
		rc4.init(key);
		rc4.codec(plain, plain);
		
		assert(plain == cipher);
	}
}
