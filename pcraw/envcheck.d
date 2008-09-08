module envcheck;

static if(char.sizeof != 1)
	static assert(false);
static if(byte.sizeof != 1)
	static assert(false);
static if(short.sizeof != 2)
	static assert(false);
static if(int.sizeof != 4)
	static assert(false);
static if(long.sizeof != 8)
	static assert(false);
