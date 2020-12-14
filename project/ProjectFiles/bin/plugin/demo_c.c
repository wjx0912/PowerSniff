// file encode must be UTF8
// sdk function:
// init()		
// handle_data()	
// handle_click()	
// handle_double()
// all data lock at background, you can use pointer any case,  but need ATTENTION also.
// out of memory or invalid pointer may crash the full virutal machine, or cause application fetal error.
// the follow thread .h file should include!
#include <winapi\windows.h>
#include <winapi\wingdi.h>
#include <powersniff.h>

void init()
{
	trace_raw(RGB(255, 0, 0), "do init here\n");
}

// if return -1, the packet will be deleted!
int handle_data(const char *protocol, unsigned char *data, int total_len, int data_len)
{
	trace("protocol: %s，address: 0x%x, total_len: %d, data_len: %d\n", protocol, data, total_len, data_len);
	if(total_len >= 100 && total_len <= 200) {
		return -1;		// filter
	}
	return 0;
}

// fixed return 0
int handle_click(const char *protocol, unsigned char *data, int total_len, int data_len)
{
	trace_raw(RGB(0, 0, 255), "blue output\n");
	return 0;
}

// fixed return 0
int handle_double(const char *protocol, unsigned char *data, int total_len, int data_len)
{
	trace("default color output\n");
	trace_raw(RGB(0, 0, 255), "blue output\n");
	dbgview("output to debugview.exe\n");
	
	plugin_summary("hello1: %d", 123);
	plugin_output_clear();
	plugin_output("hello2: %d", 123);
	plugin_output_clear();
	plugin_output("hello3: %d\n", 456);
	plugin_output("hello4: %d\n", 789);
	return 0;
}

void *my_memcpy(void * to, const void * from, int n)
{
int d0, d1, d2;
__asm__ __volatile__(
        "rep ; movsl\n\t"
        "testb $2,%b4\n\t"
        "je 1f\n\t"
        "movsw\n"
        "1:\ttestb $1,%b4\n\t"
        "je 2f\n\t"
        "movsb\n"
        "2:"
        : "=&c" (d0), "=&D" (d1), "=&S" (d2)
        :"0" (n/4), "q" (n),"1" ((long) to),"2" ((long) from)
        : "memory");
return (to);
}
