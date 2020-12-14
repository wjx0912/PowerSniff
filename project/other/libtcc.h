// 说明：这个文件经过tcc-0.9.27里面的libtcc.h改造而来：
// 1.LIBTCCAPI定义为__cdecl
// 2.函数声明全部改为函数类型
// 3.tcc_new等函数名改为函数类型：tcc_new1
#ifndef LIBTCC_H
#define LIBTCC_H

#ifndef LIBTCCAPI
#define LIBTCCAPI __cdecl
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct TCCState;

typedef struct TCCState TCCState;

/* create a new TCC compilation context */
typedef TCCState *(LIBTCCAPI *tcc_new1)(void);

/* free a TCC compilation context */
typedef void (LIBTCCAPI *tcc_delete1)(TCCState *s);

/* set CONFIG_TCCDIR at runtime */
typedef void (LIBTCCAPI *tcc_set_lib_path1)(TCCState *s, const char *path);

/* set error/warning display callback */
typedef void (LIBTCCAPI *tcc_set_error_func1)(TCCState *s, void *error_opaque,
    void (*error_func)(void *opaque, const char *msg));

/* set options as from command line (multiple supported) */
typedef void (LIBTCCAPI *tcc_set_options1)(TCCState *s, const char *str);

/*****************************/
/* preprocessor */

/* add include path */
typedef int (LIBTCCAPI *tcc_add_include_path1)(TCCState *s, const char *pathname);

/* add in system include path */
typedef int (LIBTCCAPI *tcc_add_sysinclude_path1)(TCCState *s, const char *pathname);

/* define preprocessor symbol 'sym'. Can put optional value */
typedef void (LIBTCCAPI *tcc_define_symbol1)(TCCState *s, const char *sym, const char *value);

/* undefine preprocess symbol 'sym' */
typedef void (LIBTCCAPI *tcc_undefine_symbol1)(TCCState *s, const char *sym);

/*****************************/
/* compiling */

/* add a file (C file, dll, object, library, ld script). Return -1 if error. */
typedef int (LIBTCCAPI *tcc_add_file1)(TCCState *s, const char *filename);

/* compile a string containing a C source. Return -1 if error. */
typedef int (LIBTCCAPI *tcc_compile_string1)(TCCState *s, const char *buf);

/*****************************/
/* linking commands */

/* set output type. MUST BE CALLED before any compilation */
typedef int (LIBTCCAPI *tcc_set_output_type1)(TCCState *s, int output_type);
#define TCC_OUTPUT_MEMORY   1 /* output will be run in memory (default) */
#define TCC_OUTPUT_EXE      2 /* executable file */
#define TCC_OUTPUT_DLL      3 /* dynamic library */
#define TCC_OUTPUT_OBJ      4 /* object file */
#define TCC_OUTPUT_PREPROCESS 5 /* only preprocess (used internally) */

/* equivalent to -Lpath option */
typedef int (LIBTCCAPI *tcc_add_library_path1)(TCCState *s, const char *pathname);

/* the library name is the same as the argument of the '-l' option */
typedef int (LIBTCCAPI *tcc_add_library1)(TCCState *s, const char *libraryname);

/* add a symbol to the compiled program */
typedef int (LIBTCCAPI *tcc_add_symbol1)(TCCState *s, const char *name, const void *val);

/* output an executable, library or object file. DO NOT call
   tcc_relocate() before. */
typedef int (LIBTCCAPI *tcc_output_file1)(TCCState *s, const char *filename);

/* link and run main() function and return its value. DO NOT call
   tcc_relocate() before. */
typedef int (LIBTCCAPI *tcc_run1)(TCCState *s, int argc, char **argv);

/* do all relocations (needed before using tcc_get_symbol()) */
typedef int (LIBTCCAPI *tcc_relocate1)(TCCState *s1, void *ptr);
/* possible values for 'ptr':
   - TCC_RELOCATE_AUTO : Allocate and manage memory internally
   - NULL              : return required memory size for the step below
   - memory address    : copy code to memory passed by the caller
   returns -1 if error. */
#define TCC_RELOCATE_AUTO (void*)1

/* return symbol value or NULL if not found */
typedef void *(LIBTCCAPI *tcc_get_symbol1)(TCCState *s, const char *name);

#ifdef __cplusplus
}
#endif

#endif
