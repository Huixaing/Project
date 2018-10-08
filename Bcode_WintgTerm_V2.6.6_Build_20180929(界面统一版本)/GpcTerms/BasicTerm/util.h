/*******************************************************************************
**
** Source File Name = util.h  %I%
**
** Licensed Materials - Property of IBM
**
** (C) COPYRIGHT International Business Machines Corp. 1995
** All Rights Reserved.
**
** US Government Users Restricted Rights - Use, duplication or
** disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
**
**
** PURPOSE : Header file for util.c
**
*******************************************************************************/
#ifndef _UTILS_HEADER
#define _UTILS_HEADER

#include "defines.h"
extern int check_error (ushort usProcessId,char *eString, struct sqlca *caPointer,char *pbFileName,int nLineNo);

void get_info (char *inputStatement, char *inputBuffer, char *fieldName,
   short sizeRestriction);

int passinp (char *ptr);

void init_da (struct sqlda **DAPointer, int DAsqln);

void alloc_host_vars (struct sqlda *sqldaPointer);

void free_da (struct sqlda *sqldaPointer);

void print_var (char *ptr, int type, short collen, short datalen);

void display_col_titles (struct sqlda *sqldaPointer);

void display_da (struct sqlda *sqldaPointer);

#endif