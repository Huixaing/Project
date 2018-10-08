#ifndef BASIC_TYPES_INCLUDE_H_
#define BASIC_TYPES_INCLUDE_H_

#include "private_basictypes.h"
 
 typedef unsigned char IBYTE;
 typedef unsigned short IWORD;
 typedef int I_BOOL; // attention: IBOOL defined as ushort in COMM

#ifdef _BIT_64_32_ 
 typedef unsigned int IDWORD;
#else 
 typedef unsigned long IDWORD;
#endif
 
 typedef  int       ISDWORD;
 typedef const char * ILPCSTR;
 typedef char * ILPSTR;
 
#ifdef _BIT_64_32_  
 typedef  ISDWORD       I_TIME;
#else  
 typedef  long          I_TIME;
#endif
 
 #define  ITRUE     1
 #define  IFALSE    0  

#endif
