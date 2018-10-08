#include <string>
#include <time.h>
#include "unicode_tools.h"
#ifdef  LINUX_SYSTEM
 #include <locale.h> 
#endif

#ifdef __GNUC__XXX   // not use conv
#include "iconv.h"
// i_len: 输入信息长度
// o_len: 输出信息长度，由外部保证足够长度,字符串含0
// 返回:  0 - OK, else <0: failed
static short convert_code(iconv_t cd, const char* i, int i_len, char* o, int& o_len)
{
	if (0 == i_len)
	{
		*o = 0;
		o_len = 0;
		return 0;
	}

    memset(o, 0x00, o_len);
	o_len--;

	if ((iconv_t)-1 == cd) return -1;

	iconv(cd, NULL, NULL, NULL, NULL);

	const char* in  = i;
	char* out = o;

	const char** pin = &in;
	char** pout = &out;

	size_t input_len = i_len;
	size_t left_len = o_len;
	size_t ret = iconv(cd,(char**)pin, &input_len, pout, &left_len);
	if (-1 != ret && left_len != o_len && input_len == 0)
	{
		o_len -= left_len;
		return 0;
	}
	else
	{
		return -1;
	}
}
bool MyUnicodeTools::UnicodeToAnsi(const std::wstring  &src,std::string &dest)
{
	     const wchar_t *lpString=src.data();
		 int  len = src.length()*sizeof(wchar_t);
		 int  outlen= sizeof(wchar_t)  *len+5;  // UTF-16
		 char *buf = (char *)malloc(outlen);

		 iconv_t cd= iconv_open("GBK","WCHAR_T");//UTF-8");
		 if(convert_code(cd, (const char *)lpString, len, buf, outlen)==0)
			 dest=buf;
		 iconv_close(cd);
		 free(buf);
}

bool MyUnicodeTools::AnsiTounicode(const std::string  &src,std::wstring &dest)
{
	 const char *lpString=src.data();
	 int  len = src.length();
	 int  outlen= sizeof(wchar_t) *len+5;  // UTF-16
	 wchar_t *buf = (wchar_t *)malloc(outlen);

	 iconv_t cd= iconv_open("WCHAR_T","GBK");
	 if(convert_code(cd, lpString, len, (char *)buf, outlen)==0)
		 dest=buf;
	 iconv_close(cd);
	 free(buf);

}
#else

bool MyUnicodeTools::UnicodeToAnsi(const std::wstring  &src,std::string &dest)
{ 
 const wchar_t* lpString=src.data();
 size_t len = src.length(); 
 if(len<=0)
	 return false;
 size_t newlen=len*2+10;
 char *buf = (char *)malloc(newlen);  
 if(!buf)
	 return false;
#ifdef __GNUC__
 setlocale(LC_ALL,"zh_CN.GB18030");
#else
 setlocale(LC_ALL,"chs");
#endif
 size_t rc = wcstombs(buf,lpString,newlen); 
if(rc>0)
{
 dest=buf;
}
 free(buf);
 setlocale(LC_ALL,"C");
 return true;
}

bool MyUnicodeTools::AnsiTounicode(const std::string  &src,std::wstring &dest)
{
 const char *lpString=src.data();
 size_t len = src.length(); 
 if(len<=0)
	 return false;
 len+=1;
 size_t newlen= sizeof(wchar_t)*len;
 wchar_t *buf = (wchar_t *)malloc(newlen);
#ifdef __GNUC__
 setlocale(LC_ALL,"zh_CN.GB18030");
#else
 setlocale(LC_ALL,"chs");
#endif
 size_t rc=::mbstowcs(buf,lpString,newlen);
 if(rc!=(size_t)-1)
 {
	 dest=buf;
 }
 free(buf);
 setlocale(LC_ALL,"C");
 return true;
}
#endif
std::string MyUnicodeTools::GetTimeStr(time_t t)
{
	char tmp[128];
	//CTime ct=t;
	struct tm mytm;
	struct tm  *lpm=&mytm;
//	localtime_s(lpm,&t);
	sprintf(tmp,"%04d-%02d-%02d %02d:%02d:%02d",lpm->tm_year+1900,lpm->tm_mon+1,lpm->tm_mday,lpm->tm_hour,lpm->tm_min,lpm->tm_sec);
	std::string xml=tmp;
	return xml;
}

std::wstring MyUnicodeTools::GetTimeStrW(time_t t)
{
	std::wstring rc;
	AnsiTounicode(GetTimeStr(t),rc);
	return rc;
}

/*
void MyUnicodeTools::unicodeToGB2312(char* pOut,wchar_t uData)
{
	#ifdef __GNUC__
		setlocale(LC_ALL,"zh_CN.GB18030");
	#else
		setlocale(LC_ALL,"chs");
	#endif
	size_t rc = wcstombs(pOut,&uData,sizeof(uData)); 
	setlocale(LC_ALL,"C");
}     

void MyUnicodeTools::GB2312Tounicode(wchar_t* pOut,const char *gbBuffer)
{
    #ifdef __GNUC__
		setlocale(LC_ALL,"zh_CN.GB18030");
	#else
		 setlocale(LC_ALL,"chs");
	#endif
	size_t rc=::mbstowcs(pOut,gbBuffer,2);
    setlocale(LC_ALL,"C");
}
void MyUnicodeTools::UTF_8Tounicode(wchar_t* pOut,const char *pText)
{
    char* uchar = (char *)pOut;

    uchar[1] = ((pText[0] & 0x0F) << 4) + ((pText[1] >> 2) & 0x0F);
    uchar[0] = ((pText[1] & 0x03) << 6) + (pText[2] & 0x3F);

    return;
}

void MyUnicodeTools::unicodeToUTF_8(char* pOut,const wchar_t* pText)
{
    // 注意 WCHAR高低字的顺序,低字节在前，高字节在后
    char* pchar = (char *)pText;

    pOut[0] = (0xE0 | ((pchar[1] & 0xF0) >> 4));
    pOut[1] = (0x80 | ((pchar[1] & 0x0F) << 2)) + ((pchar[0] & 0xC0) >> 6);
    pOut[2] = (0x80 | (pchar[0] & 0x3F));

    return;
}

void MyUnicodeTools::GB2312ToUTF_8(std::string& OutStr,const std::string &InStr)
{
	const char *pText=InStr.c_str();
	int pLen=InStr.length();
    char buf[4];
    int nLength = pLen* 3;
    memset(buf,0,4);
    int i = 0;
    int j = 0;      
    while(i < pLen)
    {
            //如果是英文直接复制就可以
            if( *(pText + i) >= 0)
            {
                   OutStr+= pText[i++];
            }
            else
            {
                    wchar_t pbuffer;
                    GB2312Tounicode(&pbuffer,pText+i);
                    unicodeToUTF_8(buf,&pbuffer);
                    OutStr+=buf;
                    j += 3;
                    i += 2;
            }
    }
}

void MyUnicodeTools::UTF_8ToGB2312(std::string &OutStr,const std::string &InStr)
{
	const char *pText=InStr.c_str();
	int pLen=InStr.length();
    char Ctemp[4];
    memset(Ctemp,0,4);
    int i =0;
    int j = 0;
    while(i < pLen)
    {
        if(pText[i] > 0)
        {
             OutStr+= pText[i++];                       
        }
        else                 
        {
                wchar_t Wtemp;
                UTF_8Tounicode(&Wtemp,pText + i);
                unicodeToGB2312(Ctemp,Wtemp);
				OutStr+=Ctemp[0];
				OutStr+=Ctemp[1];
	            i += 3;    
                j += 2;   
        }
    }
}  
*/