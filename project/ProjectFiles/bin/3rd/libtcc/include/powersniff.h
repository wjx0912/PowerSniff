#ifndef __POWERSNIFF_DEFINE_H_
#define __POWERSNIFF_DEFINE_H_

int trace(const char *format, ...);
int TRACE(const char *format, ...);
int xtrace(const char *format, ...);
int XTRACE(const char *format, ...);

int trace_raw(int color, const char *format, ...);
int TRACE_RAW(int color, const char *format, ...);
int xtrace_raw(int color, const char *format, ...);
int XTRACE_RAW(int color, const char *format, ...);


int dbgview(const char *format, ...);
int DBGView(const char *format, ...);

int plugin_output(const char *format, ...);
int PLUGIN_OUTPUT(const char *format, ...);

int plugin_output_clear();
int PLUGIN_OUTPUT_CLEAR();

int plugin_summary(const char *format, ...);
int PLUGIN_SUMMARY(const char *format, ...);
#endif
