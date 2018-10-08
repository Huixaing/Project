#include "sysincl.h"
#include "txtfile.h"
int CTxtFile::is_tap_char(int ch,int tapchar)
     {
      if(ch==0) return 1;
      if(tapchar!=-1)  return ((ch==tapchar)?1:0);
      if(ch==0 || ch==10 || ch==13 || ch==','|| ch==9 || ch==' ')
	 return 1;
      else
	 return 0;
     }

char *CTxtFile::sgetstr(const char *p,char *dest,int len,int tapchar)
     {
      int i;
      if(!p)    return NULL;
      while(*p && is_tap_char(*p,tapchar)) p++;
      if(!(*p)) return NULL;
      i=0;
      while(1)
	   {
	 if((!is_tap_char(*p,tapchar)) && i<len)
	   {
		*dest++=*p++;
	    i++;
	   }
	     else
	       {
		 *dest=0;
		 return (char *)p;
	       }
	   }
     }


int CTxtFile::GetSubStr(const char *s,char strings[][MAXSTRLEN],int len,int tapchar)
     {
      const char *p;
      p=s;
	  int i;
      for(i=0;i<len;i++)
      {
		  p=sgetstr(p,strings[i],MAXSTRLEN,tapchar);
		  if(!p) break;
	  }
      return i;
     }



void CTxtFile::Reset()
	     {
	      if(!fpFile) return;
	      rewind(fpFile);
	     }


int  CTxtFile::IsOK()
	     {
	      if(fpFile) return 1;
	      else  return 0;
	     }

CTxtFile::CTxtFile(const char *file)
	     {
	
	      fpFile=NULL;
		  OpenFile(file);
	      //while(GetLine()) LineCount++;
	      //ResetFile();
	     }

int CTxtFile::OpenFile(const char *file)
{
          if(fpFile) fclose(fpFile);  
	      fpFile=fopen(file,"rt");
	      if(!fpFile) return -1;
	      LineCount=0;
		  return 0;
}

void CTxtFile::CloseFile()
{
	if(fpFile) fclose(fpFile);
	fpFile=NULL;
}

CTxtFile::~CTxtFile()
	     {
	      if(fpFile) fclose(fpFile);
	     }

int CTxtFile::IsCommentLine(const char *s)
{
	while((*s) && (*s==' ' || *s==9)) s++;
	if(*s==';' || *s==0 || *s=='£»') return 1;
	else  return 0;
}

int CTxtFile::GetLineCount()
{
	if(!fpFile) return 0;
	Reset();
	int lc;
	lc=0;
	while(fgets(aLine,MAXLINELEN,fpFile))
	{
	  if(IsCommentLine(aLine)) continue;
	  lc++;
	}
	return lc;
}

char *CTxtFile::GetLine()
{
     char *p;
     if(!fpFile) return NULL;
     while(fgets(aLine,MAXLINELEN-1,fpFile))
       {
              p=aLine;
	      //Change TAB to ' '
	      while(*p)
	       {
		if(*p==9) *p=' ';
                if(*p==13 || *p==10) *p=0;
                else  p++;
	       }

	 if(!IsCommentLine(aLine)) return aLine;
       }
     return NULL;
}
  
char  *CTxtFile::GetLine(char *buf,int len)
{
     char *p;
     p=GetLine();
     if(!p) return NULL;
     strncpy(buf,p,len);
     return buf;
}

int  CTxtFile::GetLineX(char *buf,char *key)
{
     if(!fpFile) return 0;
     while(fgets(aLine,MAXLINELEN-1,fpFile))
       {
		  int ret=Parse1Line(aLine,buf,key);
		  if(ret!=0) return ret;
		}
   return 0;
}

long CTxtFile::GetFileLen()
{
	if(!fpFile) return 0;
	long ret;
	fseek(fpFile,0L,SEEK_END);
	ret=ftell(fpFile);
	fseek(fpFile,0L,SEEK_SET);
	return ret;
}

int  CTxtFile::GetCharFromFile()
{
	if(!fpFile) return -1;
	int ret=fgetc(fpFile);
	if(ret==EOF) return -1;
	else return ret;
}
int  CTxtFile::Parse1Line(const char *line,char *buf,char *key)
{
	  char tmpbuf[256];
	  strncpy(tmpbuf,line,sizeof(tmpbuf));
	  char *p=tmpbuf;
      while(*p)
	       {
				if(*p==9) *p=' ';
                if(*p==13 || *p==10) *p=0;
                else  p++;
	       }
      p=tmpbuf;
	  while(*p==' ')p++;
	  if(!IsCommentLine(p)) 
		{
			strcpy(buf,p);
			return 1;// normal line
		}
		else
		{
			p++;// skip ';'
			char *tmp=strchr(p,'=');
			if(tmp)
			{
				*tmp=0;
			     strcpy(key,p);
			     tmp++;
			     while(*tmp==' ') tmp++;
			     strcpy(buf,tmp);
                 return 2;// KEY=VALUE mode line
			}
		}
	return 0;
}
