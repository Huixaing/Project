#ifndef  COMM_SYS_INCLUDE
#define  COMM_SYS_INCLUDE
#include"systype.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<stdarg.h>
#include<ctype.h>
#include <sys/timeb.h>
#ifdef WIN32_SYSTEM
  #include <windows.h>
  #include <process.h>
  #define  getpid   _getpid
  // #undef GetCurrentTime
  #define  ulong  unsigned long
  #define  vsnprintf  _vsnprintf
#endif

#ifndef WIN32_SYSTEM
 #include <unistd.h>
 #include <sys/types.h>
 #include <sys/time.h> 
#endif

#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

#ifdef  AIX_SYSTEM
#ifdef  __cplusplus
#include<new.h> 
#endif
#define  stricmp  strcmp
#define  strnicmp  strncmp
#endif

#ifdef  LINUX_SYSTEM
#define  stricmp  strcasecmp
#define  strnicmp  strncasecmp
#define  ulong  unsigned long
#endif

// printf some debug message
 //#define   NT_DEBUG

#endif
