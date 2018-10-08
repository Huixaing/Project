#ifndef __WS_FUN_H__
#define __WS_FUN_H__

#include "./CTCMMServer.nsmap"
#include "./soapH.h"

int ns2__Login(struct soap *pSoap,std::wstring szPara_w,std::wstring &result_w);
int ns2__QueryData(struct soap *pSoap,std::wstring szPara_w,std::wstring &result_info_w);
int ns2__UpdateData(struct soap *pSoap,std::wstring szPara_w,std::wstring &result_info_w);


#endif
