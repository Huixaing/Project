
#ifndef __CLBTH_CONFIG_H__
#define __CLBTH_CONFIG_H__

// 默认GNUC和MSC为当前很新版
#if !defined(__GNUC__) && !defined(_MSC_VER)
	#define __AIXC__
	#define NO_NAMESPACE
#endif

#ifndef NO_NAMESPACE
	#define CLBTH_NAMESPACE_BEGIN namespace CLBTH {
	#define CLBTH_NAMESPACE_END };
	#define USING_NAMESPACE_CLBTH using namespace CLBTH;
#else
	#define CLBTH_NAMESPACE_BEGIN
	#define CLBTH_NAMESPACE_END
	#define USING_NAMESPACE_CLBTH 
#endif


#ifdef _MSC_VER
	#pragma warning(disable:4786)

	#include <afxwin.h>         // MFC core and standard components
	//#include <share/Stdafx.h>
	#include <string>
	using namespace std;

#ifdef  NOT_USED_DLL_MODE_BASEMSG
	#define EXTERN 
	#define MODULE_EXPORT
	#define MODULE_IMPORT
	#define EXTERN_FRIEND EXTERN
	#define AFX_EXT_CLASS
#else
	#ifdef EXTERN_LINKAGE_DEFINE
		#define EXTERN __declspec(dllexport)
	#else
		#define EXTERN __declspec(dllimport)
	#endif
	#define MODULE_EXPORT __declspec(dllexport)
	#define MODULE_IMPORT __declspec(dllimport)
	#define EXTERN_FRIEND EXTERN
#endif	
	//#define AFX_EXT_CLASS

	/*//#include <stdlib.h>
	//#include <memory.h>
	//#include <ctype.h>
	#include <string.h>
	#include <new.h>
	#include <malloc.h>
	#include <assert.h>
	
	#define ASSERT(e) assert(e);
	// types define 
	typedef int   BOOL;
	enum { TRUE = 1, FALSE = 0 };
	#define NULL 0
	
	typedef char  CHAR;
	typedef unsigned char BYTE;
	typedef short SHORT;
	typedef unsigned short USHORT;
	typedef int  INT;
	typedef unsigned int UINT;
	typedef long LONG;
	typedef unsigned long ULONG;
	typedef USHORT  WORD;
	typedef ULONG   DWORD;
	
	typedef char* LPSTR;
	typedef const char* LPCSTR;
	typedef BYTE* LPUSTR;
	typedef const BYTE* LPUCSTR;

	//typedef __stdcal  PASCAL;
	#define PASCAL 

	struct __POSITION { };
	typedef __POSITION* POSITION;
	#define BEFORE_START_POSITION ((POSITION)-1L)*/
#else
	#include <stdlib.h>
	#include <string.h>
	#include <assert.h>
	#include "defines.h"

	#ifdef EXTERN_LINKAGE_DEFINE
		#define EXTERN
	#else
		#define EXTERN extern
	#endif
	#define MODULE_EXPORT extern
	#define MODULE_IMPORT extern

	#define ASSERT(e) assert(e);

	typedef int   BOOL;
	typedef char* LPSTR;
	typedef const char* LPCSTR;
	typedef BYTE* LPUSTR;
	typedef const BYTE* LPUCSTR;

	//typedef __stdcal  PASCAL;
	#define PASCAL 

	struct __POSITION { };
	typedef __POSITION* POSITION;
	#define BEFORE_START_POSITION ((POSITION)-1L)

	#ifdef __GNUC__
		#include <new>
		// types define 
		enum { TRUE = 1, FALSE = 0 };
		#define EXTERN_FRIEND
	#else
		#define EXTERN_FRIEND EXTERN
	#endif
#endif


#endif //#ifndef __CLBTH_CONFIG_H__
