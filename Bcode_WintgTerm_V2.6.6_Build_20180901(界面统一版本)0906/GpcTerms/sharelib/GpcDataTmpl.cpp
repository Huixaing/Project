
/*
 *  Copyright (c)  Casco Signal Ltd. co.   Sep 19, 2002
 *  Author : wsg
 *  The template is written just for wrap accessing data read from gpc.def
 */


#include "Stdafx.h"

#include "GpcDataTmpl.h"


int	GpcDataTmpl<int>::getContent()
{
	return data;
}

BOOL GpcDataTmpl<int>::loadContent(char *ptr)
{
	char *token;
	char seps[]=" ,\t\\/=\n";

	if( token = strtok(ptr, seps) )
		data = (int)atoi(token);
	else
		return FALSE;
	return TRUE;
}

CString GpcDataTmpl<CString>::getContent()
{
	return data;
}

BOOL GpcDataTmpl<CString>::loadContent(char *ptr)
{
	char *token;
	char seps[]=" ,\t\\/=\n";

	if( token = strtok(ptr, seps) )
	{
		data = token;
		return TRUE;
	}
	else
		return FALSE;
}
