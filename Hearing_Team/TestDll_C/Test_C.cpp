#include <stdio.h>
#include <string.h>

extern "C" 
{
	__declspec(dllexport) void test(int* value)
	{
		*value = 42;
	}
}
