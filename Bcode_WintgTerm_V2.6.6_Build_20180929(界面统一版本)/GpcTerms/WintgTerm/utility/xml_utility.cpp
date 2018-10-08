#include "stdafx.h"
#include "objbase.h"
#include "gpcdef.h"
#include "sysshare.h"
#include "function.h"
#include "xml_utility.h"
#import  <msxml.dll> named_guids
using namespace MSXML;

#define MAX_XSD_NUM  64

class MyXSDObject{
public:
	struct {
	BOOL    used;
	CString objname;
	CString xsd;
	}m_ObjectXSD[MAX_XSD_NUM];
	MyXSDObject();
};	

MyXSDObject::MyXSDObject()
{
	for(int i=0;i<MAX_XSD_NUM;i++)
	{
		m_ObjectXSD[i].used=FALSE;
		m_ObjectXSD[i].objname.Empty();
		m_ObjectXSD[i].xsd.Empty();
	}
}
static MyXSDObject g_xsdobject;
BOOL LoadXMLFile(CString &xml, const char *objname)
{
     CString filepath=gpcdef.GetDataDir();
	 filepath+="/xsd/";
	 filepath+=objname;
	 filepath+=".xsd";
	 FILE *fp;
	 fp=fopen(filepath,"rt");
	 if(!fp)
		 return FALSE;
	 int ch;
	 while((ch=fgetc(fp))!=EOF)
	 {
		xml+=(char)ch;
	 }
	 fclose(fp);
	 return TRUE;
}

const char *GetObjectXSD(const char *objname)
{
	int no_used_index=-1;
	for(int i=0;i<MAX_XSD_NUM;i++)
	{
		if(g_xsdobject.m_ObjectXSD[i].used==FALSE)
		{
			if(no_used_index==-1)
				no_used_index=i;
			continue;
		}
		if(g_xsdobject.m_ObjectXSD[i].objname==objname)
			return (const char *)g_xsdobject.m_ObjectXSD[i].xsd;
	}
	if(no_used_index<0)
		return NULL;
	if(!LoadXMLFile(g_xsdobject.m_ObjectXSD[no_used_index].xsd,objname))
		return NULL;
	g_xsdobject.m_ObjectXSD[no_used_index].objname=objname;
	g_xsdobject.m_ObjectXSD[no_used_index].used=TRUE;
	return (const char *)g_xsdobject.m_ObjectXSD[no_used_index].xsd;
}

BOOL  GenXMLDataSet(CString &dest,const char *objname,const char *xml)
{
  dest+="<";
  dest+=objname;
  dest+=">";
  //const char *xsd=GetObjectXSD(objname);
  //if(xsd)
  // dest+=xsd;
  dest+=xml;
  dest+="</";
  dest+=objname;
  dest+=">";	
  return TRUE;
}

BOOL WriteGenernalXMLFile(const char *xml,const char *filepath)
{
	 FILE *fp;
	 fp=fopen(filepath,"wt");
	 if(fp)
	 {
		fwrite(xml,1,strlen(xml),fp);
		fclose(fp);
		return TRUE;
	 }
	 return FALSE;
}

BOOL WriteXMLFile(const char *xml,const char *objname,const char *prefix)
{
	 CString filepath=gpcdef.GetXMLDir();
	 CString tmstr;
	 filepath+="/";
	 if(prefix)
		filepath+=prefix;
	 else
	 {
		 CTime ctm=CTime::GetCurrentTime();
		 filepath+=ctm.Format("%Y%m%d");
	 }
	 filepath+="-";
	 filepath+=objname;
	 filepath+=".xml";
	 return WriteGenernalXMLFile(xml,filepath);
}

const char *GetXMLFilePath()
{
	return gpcdef.GetXMLDir();
}

BOOL ReadGenernalXMLFile(CString &dest,const char *filepath)
{
	 FILE *fp;
	 fp=fopen(filepath,"rt");
	 if(fp)
	 {
		int ch;
		while((ch=fgetc(fp))!=EOF)
		{
			dest+=(char)ch;
		}
		fclose(fp);
		return TRUE;
	 }
	 return FALSE;
}

BOOL ReadXMLFile(CString &dest,const char *xmlfile)
{
	 CString filepath=gpcdef.GetXMLDir();
	 CString tmstr;
	 filepath+="/";
	 filepath+=xmlfile;
	 filepath+=".xml";
	 return ReadGenernalXMLFile(dest,filepath);
}

BOOL ReadXMLFile(CString &xml,const char *objname,const char *prefix)
{
	 CString file;
	 if(prefix)
		file+=prefix;
     file+="-";
	 file+=objname;
	 return ReadXMLFile(xml,file);
}


const char *__GetXMLAttribStringValue(const char *xml,const char *attr,char **start,char **end)
{
	static char tmpstr[10000];
	char *retv=NULL;
	if(!xml || strlen(xml)>=10000)
		return NULL;
	char *p= (char*)strstr(xml,attr);
	if(!p)
		return NULL;
	*start=p;
	p+=strlen(attr);
	if(*p!='=')
		return NULL;
	p++;
	if(*p!='"')
		return NULL;
	p++;
	retv=tmpstr;
	char prev_ch=0;
	while(*p)
	{
		if(*p=='"')
		{
			if(prev_ch!='/')
			{
				*retv=0;
				break;
			}
		}
		else
		if(*p=='/')
		{
			if(prev_ch=='/')
			{
			   *retv++=*p++;
			    prev_ch=0;
				continue;
			}
			else
			{
				prev_ch=*p;
				p++;
				continue;
			}
		}
		*retv++=*p;
		prev_ch=*p;
		p++;
	}
	if(*retv==0)
	{
		*end=p+1;
		return tmpstr;
	}
	else
		return NULL;
}
const char *GetXMLAttribStringValue(const char *xml,const char *attr)
{
	char *start,*end;
	return __GetXMLAttribStringValue(xml,attr,&start,&end);
}

//const char *SetXMLAttribStringValue(const char *xml,const char *attr,const char *value)
//{
//static char tmpstr[10000];
//  char *start=NULL;
//  char *end=NULL;
//  if(__GetXMLAttribStringValue(xml,attr,&start,&end))
//  {
//	 strncpy(tmpstr,xml,(int)(start-xml));
//	 start=tmpstr+(int)(start-xml);
//  }
//  else
//  {
//	  strcpy(tmpstr,xml);
//	  start=tmpstr+strlen(tmpstr);
//  }
//  if(!value || *value==0)
//  {
//	   *start=0;
//	   return tmpstr;
//  }
//  *start++=' ';
//  strcpy(start,attr);
//  start+=strlen(attr);
//  *start++='=';
//  *start++='"';
//  while(*value)
//  {
//	  if(*value=='/' || *value=='"')
//		  *start++='/';
//	  *start++=*value++;
//  }
//  *start++='"';
//  if(end)
//  {
//	  while(*end)
//		  *start++=*end++;
//  }
//  *start=0;
//  return tmpstr;
//}
//

void ProcessHalfChineseCode(char *str)
{
	int  status=0;
	while(*str) 
	{
		if(*str<0 && status==0)
		{
				status=1;  // 半个 汉字 
		}
		else
			    status=0;
		str++;
	}
	if(status==1) // 发现半个汉字情况
		*(str-1)=0;
}

const char *SetXMLAttribStringValue(const char *xml,const char *attr,const char *value,int limitlen)
{
	static char tmpstr[10240];
	memset(tmpstr, 0x00, sizeof(tmpstr));
	strncpy(tmpstr, xml, sizeof(tmpstr)-1);

	CString strTitle[10], strContent[10];
	int index = 0;

	CString strXml = xml;
	if(strXml.GetLength() > 0)
	{
		strXml.Replace("=\"\"", "=\" \"");
	}

	char *str = new char[strXml.GetLength() + 1], *delptr;
	delptr = str;
	memset(str, 0x00, strXml.GetLength()+1);
	strncpy(str, strXml, strXml.GetLength());
	

    char *token;
	char seps[]="=\"";

    token =  strtok(str, seps);
	while(NULL != token)
	{
	   strTitle[index]= token;
	   token = strtok(NULL, seps);

	   if(NULL == token)// 不应该发生
		   break;

	   strContent[index]= token;

	   index ++;

	   if (index == sizeof(strTitle)/sizeof(strTitle[0]))
		   break;

	    token = strtok(NULL, seps);
	}

	for(int i = 0; i < index; i++)
	{
	   strTitle[i].Trim();
	   strContent[i].Trim();
	}

	CString strToken = attr;
	int pos = -1;
	for(int i = 0;i < index; i++)
	{		
		if(strToken.Trim() == strTitle[i])
		{
			pos = i;
			break;
		}
	}
	if (-1 == pos)
	{
		if (index == sizeof(strTitle)/sizeof(strTitle[0])) return tmpstr;

		pos = index;
		index ++;
	}
	strTitle[pos] = strToken;
	strContent[pos] = value;

	strContent[pos].Replace("=", " ");
	strContent[pos].Replace("\"", " ");
	strContent[pos].Replace("/", "//");

	CString strAll;
	for(int i = 0;i < index; i++)
	{
		strAll+=strTitle[i];
		strAll+="=\"";
		strAll+=strContent[i];
		strAll+="\" ";
	}
    
	memset(tmpstr, 0x00, sizeof(tmpstr));

	if(limitlen == 1)
		strncpy(tmpstr, strAll, 255);
	else
	    strncpy(tmpstr, strAll, sizeof(tmpstr)-1);

	delete[] delptr;

	ProcessHalfChineseCode(tmpstr);

	return tmpstr;
}

BOOL  InsertXMLNodeValue(CString &dest,const char *node,const char *value)
{
	dest+="<";
	dest+=node;
	dest+=">";
	XMLCopyValueString(dest,value);
	dest+="</";
	dest+=node;
	dest+=">";	
	return TRUE;
}
BOOL  InsertXMLNodeValue(CString &dest,const char *node,int value)
{
	char tmp[64];
	sprintf(tmp,"%d",value);
	return InsertXMLNodeValue(dest,node,tmp);
}
BOOL  InsertXMLNodeTime(CString &dest,const char *node,time_t tm)
{
	return InsertXMLNodeValue(dest,node,TimeToXMLString(tm));
}
BOOL  GetXMLNodeValue(CString &dest,const char *xml,const char *node)
{
	dest.Empty();
	 MSXML::IXMLDOMDocumentPtr pXMLDoc;
	  HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	  if(FAILED(hr))
		{
			_com_error er(hr);
			AfxMessageBox(er.ErrorMessage());
			return FALSE;
		}
	  
	  if(VARIANT_TRUE != pXMLDoc->loadXML( _bstr_t( xml ) ) )
		{
			return FALSE;
		} 
		
	   MSXML::IXMLDOMNodePtr pNode = pXMLDoc->selectSingleNode(_bstr_t(node));//ds_MaintainPlan_List/
	   if(pNode)
			{
				_variant_t v;
				v = pNode->GetnodeTypedValue();
				dest = CString((v.bstrVal));
				SysFreeString(v.bstrVal);
                v.Clear();
				return TRUE;
			}
	  return FALSE;
}		

const char *GenGUIDString()
{
	static char tmp[64];
	GUID   guid;   
	HRESULT   hr   = CoCreateGuid(&guid);   
	LPOLESTR   pstrGuid;   
	StringFromCLSID(guid,   &pstrGuid);     
    CString   strguid(pstrGuid);   
	strcpy(tmp,strguid);
    CoTaskMemFree  (pstrGuid); 	
	if(strlen(tmp)!=38)
		return NULL;
	tmp[36]=0;
	return tmp+1;
}
const char *TimeToXMLString(time_t t)
{
	static char tmp[128];
	CTime ct=t;
	sprintf(tmp,"%04d-%02d-%02dT%02d:%02d:%02d+08:00",ct.GetYear(),ct.GetMonth(),ct.GetDay(),ct.GetHour(),ct.GetMinute(),ct.GetSecond());
	return tmp;
}

time_t XMLStringToTime(const char *str)
{
	CString s=str;
	int slen=strlen(str);
	int year,mon,day,hour,min,sec;
	int x,y;
	if(slen>15)
	{
		year=atoi(s.Mid(0,4));
		mon=atoi(s.Mid(5,2));
		day=atoi(s.Mid(8,2));
		hour=atoi(s.Mid(11,2)); 
		min=atoi(s.Mid(14,2));
		sec=atoi(s.Mid(17,2));
	}
	else if((x=s.Find('-'))>=0) // xxxx-xx-xx
	{
		year=atoi(s.Mid(0,x));
		y=s.Find('-',x+1);
		if(y<0)
			return 0;
		mon=atoi(s.Mid(x+1,y-x-1));
		day=atoi(s.Mid(y+1));
		hour=0;
		min=0;
		sec=0;
	}
	else if((x=s.Find(':'))>=0) // xxxx-xx-xx
	{
		CTime curtm=CTime::GetCurrentTime();
		hour=atoi(s.Mid(0,x)); 
		y=s.Find(':',x+1);
		if(y<0) 
		{
			min=0;sec=0;
		}
		else
		{
			min=atoi(s.Mid(x+1,y-x-1));
			sec=atoi(s.Mid(y+1));
		}
		return ConvertToLongTime(hour,min,sec,curtm.GetTime(),6,18);
	}
	else
	{
		CTime curtm=CTime::GetCurrentTime();
		return curtm.GetTime();
	}
	if(year<2000 || year>2100) return 0;
	if(mon<1 || mon>12) return 0;
	if(day<1 || day>31) return 0;
	if(hour<0 || hour>23) return 0;
	if(min<0 || min>59) return 0;
	if(sec<0 || sec>59) return 0;

	CTime ct(year,mon,day,hour,min,sec);
	return ct.GetTime();
}



 void  XMLCopyValueString(CString &dest,const char *src)  // 转义
 {
	while(*src)
	{
		if(*src=='<')
			dest+="&lt;";
		else
		if(*src=='>')
			dest+="&gt;";
		else
		if(*src=='<')
			dest+="&lt;";
		else
		if(*src=='&')
			dest+="&amp;";
		else
		if(*src=='\"')
			dest+="&quot;";
		else
		if(*src=='\'')
			dest+="&apos;";
		else
		     dest+=*src;
		src++;
	}
 }

 int ConvertToSubStringArray(const CString &str,int tapch,CStringArray &array)
{
	int y=0,x=0;
    int cnt=0;
    while((x=str.Find(tapch,y))>=0)
	  {
        array.Add(str.Mid(y,x-y));
		 y=x+1;
	  }
   if(y<str.GetLength())  
		array.Add(str.Mid(y));
   return array.GetCount(); 
}

int TruncString(char *str,int limit) // 2010.06.21  崔虎 bug
{

	int i=0;
	while(i<limit)
	{
		if(str[i] & 0x80)
		{
			if(i<limit-2)
				i+=2;
			else
				break;
		}
		else
		{
			if(i<limit-1)
				i+=1;
			else
				break;
		}
	}
	for(int j=i;j<limit;j++)
		str[j]=0;
	return i;
}