#include "stdafx.h"
#include <stdio.h>
#include "soapH.h"
#include "tg_function_config.h"
#include "unicode/unicode_tools.h"
extern CTGFunctionConfig *GetTGFunctionConfig();

typedef int (*GSoapCallFunction)(struct soap *soap, const char *soap_endpoint, const char *soap_action, std::wstring szPara, std::wstring &result_info);
int gs_general_process(const std::string& szPara, std::string& szResult, GSoapCallFunction gsoap_function, const char* remark)
{
    struct soap ns2__soap;
    soap_init(&ns2__soap);

	std::wstring szPara_w;
	std::wstring szResult_w;
	MyUnicodeTools::AnsiTounicode(szPara, szPara_w);

	(*gsoap_function)(&ns2__soap, GetTGFunctionConfig()->GetGSoapURL(), "", szPara_w, szResult_w);
    if(ns2__soap.error && NULL != GetTGFunctionConfig()->GetGSoapURL2())
	{
		soap_init(&ns2__soap);
		(*gsoap_function)(&ns2__soap, GetTGFunctionConfig()->GetGSoapURL2(), "", szPara_w, szResult_w);
	}
	MyUnicodeTools::UnicodeToAnsi(szResult_w,szResult);

    if(ns2__soap.error)
	{
		szResult +=  remark;
		szResult += " soap error:";
		szResult +=  ns2__soap.error;
		szResult += ";";
		const char ** rs = soap_faultcode(&ns2__soap);
		if (NULL != *rs) szResult += *rs;
		szResult += ";";
		rs = soap_faultstring(&ns2__soap);
		if (NULL != *rs) szResult += *rs;
	}

    soap_end(&ns2__soap);
    soap_done(&ns2__soap);
    return ns2__soap.error;
}

int GSLogin(const std::string& szPara, std::string& szResult)
{
	GSoapCallFunction pGSoapCallFunction = soap_call_ns2__Login;

	return gs_general_process(szPara, szResult, pGSoapCallFunction, "login");
}


int GSQueryData(const std::string& szPara, std::string& szResult)
{
	GSoapCallFunction pGSoapCallFunction = soap_call_ns2__QueryData;

	return gs_general_process(szPara, szResult, pGSoapCallFunction, "query");
}

int GSUpdateData(const std::string& szPara, std::string& szResult)
{
	GSoapCallFunction pGSoapCallFunction = soap_call_ns2__UpdateData;

	return gs_general_process(szPara, szResult, pGSoapCallFunction, "update");
}

