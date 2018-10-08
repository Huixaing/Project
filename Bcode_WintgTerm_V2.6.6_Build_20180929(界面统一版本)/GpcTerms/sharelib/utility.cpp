#include "stdafx.h"
#include <stdio.h>
#include <sys/stat.h>
#include "sysshare.h"

USHORT  OsGetTaskId(USHORT *pTaskId) //兼容性，无用
{
	 *pTaskId=0;
   return 0;
}

int  RunShellCommand(const char *cmdline2)
  {
     char cmdline[128];
	 strcpy(cmdline,cmdline2);
 	 STARTUPINFO si;
	 PROCESS_INFORMATION pi;
 	 ZeroMemory( &si, sizeof(si) ); 
	 si.cb = sizeof(si);

	 if( !CreateProcess( NULL, // No module name (use command line). 
        cmdline,
        NULL,             // Process handle not inheritable. 
        NULL,             // Thread handle not inheritable. 
        FALSE,            // Set handle inheritance to FALSE. 
        0,                // No creation flags. 
        NULL,             // Use parent's environment block. 
        NULL,             // Use parent's starting directory. 
        &si,              // Pointer to STARTUPINFO structure.
        &pi )             // Pointer to PROCESS_INFORMATION structure.    ) 
		)
	 	 return -1;
	 else
		 return 0;

}


int CopyPathStr(char *dest, int destlimit, const char *src, const char *ref) // 相对路径
{
       const char *p=src; 
	   while(*p==' ') p++;
	   if(strstr(p,":\\") || *p=='/') // 全路径
	   {
		   if(strlen(p)>destlimit)  
                return -1;  
		   strcpy(dest, p);
	   }
	   else   // 相对路径
	   {
           if(strlen(ref)>destlimit)
			    return -1;
		   strcpy(dest,ref);
		   char *p2=strrchr(dest,'/');
		   if(!p2) p2=strrchr(dest,'\\');
		   if(p2) *p2=0;
		   if(strlen(dest)+strlen(p)>destlimit)
			    return -1;
		   #ifdef WIN32_SYSTEM
		       strcat(dest,"\\");
           #else
			   strcat(dest,"/"); 
		   #endif
			   strcat(dest,p);
	   }
	   return 0;
}

#define MAX_LOG_FILES 10
static struct{
	CFile  flog;
	char   log_key[32];
}g_log_files[MAX_LOG_FILES];
int g_log_file_count;

///////////////////////
static unsigned short g_nCurrentLogTypeMask=0xFFFF; //默认所有类型都记录
static short g_nCurrentLogLevel=-1; // 默认记录所有级别信息
static char g_szLogPrefix[24];
///////////////////////

CFile *GetLogFile(const char *key)
{
	int i=0;
	for(i=0;i<g_log_file_count;i++)
	{
		if(!stricmp(g_log_files[i].log_key,key)) 
			break;
	}
	if(i>=MAX_LOG_FILES)
		return NULL;
	if(i==g_log_file_count)
	{
	    strcpy(g_log_files[i].log_key,key);
		g_log_file_count++;
	}
	return &g_log_files[i].flog; 
}

void GetTodayLogFileName(const char *key,char *buf)
{
	CTime tm;
	tm=CTime::GetCurrentTime();
	sprintf(buf,"%s\\%s_%s_%s%02d.LOG",gpcdef.GetLogDir(),gpcdef.GetCurUserName(),g_szLogPrefix,key,tm.GetDay());
}

void  GetHistoryLogFileName(const char *key,char *buf,int day_before)
{
	CTime tm;
	tm=CTime::GetCurrentTime();
	tm-=CTimeSpan(day_before,0,0,0);
	sprintf(buf,"%s\\%s_%s_%s%02d.LOG",gpcdef.GetLogDir(),gpcdef.GetCurUserName(),g_szLogPrefix,key,tm.GetDay());
}
void SaveLogMessageStr(const char *key,time_t tm,const char *str)
{
	 CFile *pfLog;
	 char   newfile[128];	
	 pfLog=GetLogFile(key);
	 if(!pfLog)
		 return;
	 GetTodayLogFileName(key,newfile);
	 CString strOldFileName = pfLog->GetFilePath(); // GetFileName();
	 
	 if(stricmp(strOldFileName.GetBuffer(0),newfile)) // new day
	 {
		 if ( !strOldFileName.IsEmpty() )	// old file has been open
		 {
			 if (pfLog->m_hFile != CFile::hFileNull) //增加判断,避免ASSERT
					pfLog->Close();
			  unlink(newfile);
		 }
		 else
		 {
			 CFileStatus fStatus;
			 if ( CFile::GetStatus(newfile, fStatus) )
			 {
				 CTime tmNow = CTime::GetCurrentTime();
				 if ( tmNow.GetYear() != fStatus.m_mtime.GetYear() ||
					  tmNow.GetMonth() != fStatus.m_mtime.GetMonth() ||
					  tmNow.GetDay() != fStatus.m_mtime.GetDay() )
					  unlink(newfile);
			 }
		 }
		 if(!pfLog->Open( newfile, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite|CFile::shareDenyNone))
		 {
			 return;
		}
		 pfLog->SeekToEnd();
	 }

	 CString strFileName = pfLog->GetFileName();
	 if ( strFileName.IsEmpty() )
		 return;
	 if (pfLog->m_hFile == CFile::hFileNull) //增加判断,避免ASSERT
		 return;
	 CString strLog;
	 CTime ctm;
	 if(tm)	 ctm=tm;
	 else ctm=CTime::GetCurrentTime(); 
	 strLog.Format("%d-%d-%d %02d:%02d:%02d- %s\r\n",ctm.GetYear(),ctm.GetMonth(),ctm.GetDay(),ctm.GetHour(),ctm.GetMinute(),ctm.GetSecond(),str);
	 pfLog->Write( strLog.GetBuffer(0), strLog.GetLength() );
	 // pfLog->Flush();  // 2008.12.16 影响操作速度！
}


void SaveLogMessage(const char *key,time_t tm,const char *format,...)
{
	 CString strLog;
	 va_list marker;
	 va_start(marker,format);
	 strLog.FormatV(format,marker); 
	 va_end( marker );
	 SaveLogMessageStr(key,tm,strLog);
}

void SetSysPrintLogMask(const char *prefix,short level,unsigned short mask)
{
	strcpy(g_szLogPrefix,prefix);
	g_nCurrentLogLevel=level;
	g_nCurrentLogTypeMask=mask;
}

void sysprint(short msg_level,unsigned short type_mask,char *format, ... ) //msg_level　越大级别越高，type_mask 用比特表示的消息类型,0表示总记录
{
	if(msg_level<g_nCurrentLogLevel)
		return;
	if((type_mask & g_nCurrentLogTypeMask)!=type_mask)
		return;
	va_list marker;
	va_start(marker,format);
	CString tmpstr;
	tmpstr.FormatV(format,marker); 
	va_end( marker );
	SaveLogMessageStr("alarm",0,tmpstr);
	
}

void LogCTCAlarms(time_t tm,char *format, ... )
{
	va_list marker;
	va_start(marker,format);
	SaveLogMessage("ctcinfo",tm,format,marker);
	va_end( marker );
}


void ShowLogFiles(const char *key)
{
     char filename[256];
	  memset(filename,0,sizeof(filename)-1);
	  GetTodayLogFileName(key,filename);
     if(filename[0])
	 {
		 CString cmd;
		 cmd.Format("notepad %s",filename); 
		 RunShellCommand(cmd);
	 }
}
void ShowLogFiles(const char *key,CListBox *plist,const char *filter,int max_line,int day_num)
{
	  char filename[256];
	  for(int day=day_num-1;day>=0;day--)  // 只取2天的记录,days 无用
	  {
	   memset(filename,0,sizeof(filename)-1);
	   GetHistoryLogFileName(key,filename,day);
      if(filename[0])
	  {
		FILE *fp;
		fp=fopen(filename,"rt");
		if(fp)
		{
			char line[256];
			int line_count=0;
			while(fgets(line,sizeof(line)-1,fp) && line_count<max_line)
			{ 
				strncpy(filename,filter,sizeof(filename)-1);
				char *ptmp=strtok(filename,";");
				while(ptmp)
				{
				  if(!strstr(line,ptmp))
					  break;
				   ptmp=strtok(NULL,";");
				}
				if(!ptmp)
				{
					 line_count++;;
				    plist->AddString(line);
				}
			}
			fclose(fp);
		}

	  }
	 }
}
void OsSleep(unsigned long tm,int flag)
{
	Sleep(tm);
}

void ReplaceSubStr(char *str,const char *key,const char *content) // must strlen<512 
{
	 char  tmp[512];
	 if(strlen(str)>510)
		 return;
	 strcpy(tmp,str);
	 if(!key || !content)
		 return;
	 char *pstr=tmp;
	 char  keylen=strlen(key);
	 *str=0;
 	 while(strlen(pstr)>0)
		{
			char *pr=strstr(pstr,key);
			if(!pr)
				break;
			*pr=0;
			strcat(str,pstr);
			strcat(str,content);
			pstr=pr+keylen;
		}
	strcat(str,pstr);
}
