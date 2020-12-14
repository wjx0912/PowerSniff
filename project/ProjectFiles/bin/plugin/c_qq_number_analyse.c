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
	if(0 != strcmp("udp", protocol)) {
		return -1;		// filter
	}

	if(data_len < 100) {
		return -1;		// filter
	}

	short src_port = data[34] * 256 + data[35];
	short dst_port = data[36] * 256 + data[37];
	if(8000 != dst_port)
		return -1;

	if(2 != data[42])		// 0x2是qq udp协议magic number
		return -1;
	if(8 != data[45])		// 8和37是 0x8和0x25是协议类型，表示登录
		return -1;
	if(37 != data[46])
		return -1;

	unsigned int qq_number = data[49] * 256 * 256 * 256 + data[50] * 256 * 256 + data[51] * 256 + data[52];
	plugin_summary("debug-%d, qq_number is: %d", __LINE__, qq_number);
	plugin_output_clear();
	plugin_output("debug-%d, qq_number is: %d\n", __LINE__, qq_number);
	trace("debug-%d, qq_number is: %d\n", __LINE__, qq_number);
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
