#ifndef __INIFILEINCLUDE__
#define __INIFILEINCLUDE__
#include "txtfile.h"
#ifndef   AIX_SYSTEM
///	namespace LAYOUTFORM_IMPL
////	{
#endif
class CIniFile:public CTxtFile{
   public:
      int	 GotoSection(const char *sect);
      char *SerchKey(const char *key,char *dest,int len);
      char *GetKeyValue(const char *sect,const char *key,char *dest,int len);
      char *GetKey(const char *sect,const char *value,char *dest,int len);
      int   GetIntKeyValue(const char *sect,const char *key);
      char *SerchValue(const char *value,char *dest,int limit);
	  static char *GetKeyValue(const char *file,const char *sect,const char *key,char *dest,int len);
	  char *GetContent(char *key,char *dest,int limit);
};
#ifndef AIX_SYSTEM
////};
////using namespace LAYOUTFORM_IMPL;
#endif
#endif
