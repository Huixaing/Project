#include "sysincl.h"
#include "inifile.h"
int  CIniFile::GotoSection(const char *sect)
{
     Reset();
     char *p,*l;
     while(l=GetLine())
      {
       if(*l!='[') continue;
       p=strchr(l,']');
       if(!p) continue;
	   *p=0;
       if(!stricmp(l+1,sect)) return 1;
      }
     return 0;
}

char *CIniFile::SerchKey(const char *key,char *dest,int limit)
{
	 char tmp[40];
	 while(GetContent(tmp,dest,limit))
	 {
		 if(!stricmp(tmp,key)) return dest;
	 }
	 return NULL;
}

char *CIniFile::GetKeyValue(const char *sect,const char *key,char *dest,int len)
{
     if(!GotoSection(sect)) return NULL;
     return SerchKey(key,dest,len);
}

char *CIniFile::SerchValue(const char *value,char *dest,int limit)
{
     char  tmp[40];
	 while(GetContent(dest,tmp,40))
	 {
		 if(!stricmp(tmp,value)) return dest;
	 }
	 return NULL;
}

char *CIniFile::GetKey(const char *sect,const char *value,char *dest,int len)
{
	 if(!GotoSection(sect)) return NULL;
	 return SerchValue(value,dest,len);
}

int   CIniFile::GetIntKeyValue(const char *sect,const char *key)
{
	  char tmp[10];
	  if(GetKeyValue(sect,key,tmp,9))
	 {
	  return atoi(tmp);
	 }
      else return -1;
}

char *CIniFile::GetKeyValue(const char *file,const char *sect,const char *key,char *dest,int len)
{
	  CIniFile t;
	  t.OpenFile(file);
	  if(!t.IsOK()) return NULL;
	  char *ret=t.GetKeyValue(sect,key,dest,len);
	  t.CloseFile();
	  return ret;
}

char *CIniFile::GetContent(char *key,char *dest,int limit)
{
     char *l=NULL,*p=NULL;
	 while(l=GetLine())
	 {
	   if(*l=='[') break;
	   if(p=strchr(l,'=')) break;
	 }
	 if(!p) return NULL;
	 
	 // remove tail blanks
	 char *tail=p-1;
     while(tail>l && (*tail==' ' || *tail==9))
		 tail--;
	 *(tail+1)=0;
	 ////////////////////
     
     p++;
     while(*p==' ' || *p==9) *p++;
     int len=strlen(p)-1;
     while(len>=0 && *(p+len)==' ') *(p+len--)=0;
     strcpy(key,l);
     strncpy(dest,p,limit);
     *(dest+limit-1)=0;
     return dest;
}
