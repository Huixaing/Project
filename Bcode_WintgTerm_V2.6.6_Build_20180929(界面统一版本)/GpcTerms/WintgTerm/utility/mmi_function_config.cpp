#include "stdafx.h"
#include <stdio.h>
#include "mmi_function_config.h"
#include "inifile.h"
CMMIFunctionConfig::CMMIFunctionConfig()
{
}

BOOL  CMMIFunctionConfig::InitData(const char *cfgfile)
{
	CIniFile t;
	t.OpenFile(cfgfile);
	if(!t.IsOK())
	{
		return FALSE;
	}
	
	char key[32];
	char content[256];
	if(t.GotoSection("INT_VALUE"))
	while(t.GetContent(key,	content,sizeof(content)-1))
	{
		ConfigItemIntegerStruct x;
		strncpy(x.szConfigName,key,sizeof(x.szConfigName)-1);
		x.lConfigValue=atoi(content);
		m_aConfigItems.push_back(x);
	}
	return TRUE;
}


int   CMMIFunctionConfig::GetConfigIntegerItem(const char *name) const
{
	for(int i=0;i<m_aConfigItems.size();i++)
	{
		ConfigItemIntegerStruct x;
		x=m_aConfigItems[i];
		if(!stricmp(x.szConfigName,name))
			return x.lConfigValue;
	}
	return -1;
}

