#ifndef  MY_UNICODE_CODE_TOOLS_INCLUDE
#define  MY_UNICODE_CODE_TOOLS_INCLUDE
#include<string>
class MyUnicodeTools{
public:
 static 	bool AnsiTounicode(const std::string  &src,std::wstring &dest);
 static 	bool UnicodeToAnsi(const std::wstring  &src,std::string &dest);
 static std::string  GetTimeStr(time_t);
 static std::wstring  GetTimeStrW(time_t);
 /*
 static void unicodeToUTF_8(char* pOut,const wchar_t* pText);
 static void UTF_8ToGB2312(std::string &pOut, const std::string &pIn);
 static void GB2312ToUTF_8(std::string& pOut,const std::string &pIn);
 static void UTF_8Tounicode(wchar_t* pOut,const char *pText);
 static void GB2312Tounicode(wchar_t* pOut,const char *gbBuffer);
 static void unicodeToGB2312(char* pOut,wchar_t uData);
 */
};
#endif