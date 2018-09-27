//
// sproxy.exe generated file
// do not modify this file
//
// Created: 05/22/2009@21:09:48
//

#pragma once


#ifndef _WIN32_WINDOWS
#pragma message("warning: defining _WIN32_WINDOWS = 0x0410")
#define _WIN32_WINDOWS 0x0410
#endif

#include <atlsoap.h>

namespace WS_MaintainPlan
{

struct Result
{
	bool succeed;
	BSTR message;
};

template <typename TClient = CSoapSocketClientT<> >
class CWS_MaintainPlanT : 
	public TClient, 
	public CSoapRootHandler
{
protected:

	const _soapmap ** GetFunctionMap();
	const _soapmap ** GetHeaderMap();
	void * GetHeaderValue();
	const wchar_t * GetNamespaceUri();
	const char * GetServiceName();
	const char * GetNamespaceUriA();
	HRESULT CallFunction(
		void *pvParam, 
		const wchar_t *wszLocalName, int cchLocalName,
		size_t nItem);
	HRESULT GetClientReader(ISAXXMLReader **ppReader);

public:

	HRESULT __stdcall QueryInterface(REFIID riid, void **ppv)
	{
		if (ppv == NULL)
		{
			return E_POINTER;
		}

		*ppv = NULL;

		if (InlineIsEqualGUID(riid, IID_IUnknown) ||
			InlineIsEqualGUID(riid, IID_ISAXContentHandler))
		{
			*ppv = static_cast<ISAXContentHandler *>(this);
			return S_OK;
		}

		return E_NOINTERFACE;
	}

	ULONG __stdcall AddRef()
	{
		return 1;
	}

	ULONG __stdcall Release()
	{
		return 1;
	}

	CWS_MaintainPlanT(ISAXXMLReader *pReader = NULL)
		:TClient(_T("http://192.100.88.226:1111/ws_maintainplan.asmx"))
	{
		SetClient(true);
		SetReader(pReader);
	}
	
	~CWS_MaintainPlanT()
	{
		Uninitialize();
	}
	
	void Uninitialize()
	{
		UninitializeSOAP();
	}	

	HRESULT IsReady(
		bool* IsReadyResult
	);

	HRESULT Delect_MaintianPlan_By_Jhh(
		BSTR Jhh, 
		Result* Delect_MaintianPlan_By_JhhResult
	);

	HRESULT GetData_By_JHH(
		BSTR Jhh, 
		BSTR* GetData_By_JHHResult
	);

	HRESULT Update_MaintainPlan2(
		BSTR xml, 
		Result* Update_MaintainPlan2Result
	);

	HRESULT Get_MaintainPlan_List(
		BSTR* Get_MaintainPlan_ListResult
	);

	HRESULT Update_MaintainPlan(
		BSTR ds, 
		Result* Update_MaintainPlanResult
	);
};

typedef CWS_MaintainPlanT<> CWS_MaintainPlan;

__if_not_exists(__Result_entries)
{
extern __declspec(selectany) const _soapmapentry __Result_entries[] =
{
	{ 
		0xAC8C5ABC, 
		"succeed", 
		L"succeed", 
		sizeof("succeed")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD, 
		offsetof(Result, succeed),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xBA86A185, 
		"message", 
		L"message", 
		sizeof("message")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(Result, message),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __Result_map =
{
	0xC6ABBE1F,
	"Result",
	L"Result",
	sizeof("Result")-1,
	sizeof("Result")-1,
	SOAPMAP_STRUCT,
	__Result_entries,
	sizeof(Result),
	2,
	-1,
	SOAPFLAG_NONE,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};
}

struct __CWS_MaintainPlan_IsReady_struct
{
	bool IsReadyResult;
};

extern __declspec(selectany) const _soapmapentry __CWS_MaintainPlan_IsReady_entries[] =
{

	{
		0x851A67D0, 
		"IsReadyResult", 
		L"IsReadyResult", 
		sizeof("IsReadyResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_MaintainPlan_IsReady_struct, IsReadyResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWS_MaintainPlan_IsReady_map =
{
	0x34FABC20,
	"IsReady",
	L"IsReadyResponse",
	sizeof("IsReady")-1,
	sizeof("IsReadyResponse")-1,
	SOAPMAP_FUNC,
	__CWS_MaintainPlan_IsReady_entries,
	sizeof(__CWS_MaintainPlan_IsReady_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWS_MaintainPlan_Delect_MaintianPlan_By_Jhh_struct
{
	BSTR Jhh;
	Result Delect_MaintianPlan_By_JhhResult;
};

extern __declspec(selectany) const _soapmapentry __CWS_MaintainPlan_Delect_MaintianPlan_By_Jhh_entries[] =
{

	{
		0x0001489A, 
		"Jhh", 
		L"Jhh", 
		sizeof("Jhh")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWS_MaintainPlan_Delect_MaintianPlan_By_Jhh_struct, Jhh),
		NULL,
		NULL,
		-1,
	},
	{
		0xD0982F7E, 
		"Delect_MaintianPlan_By_JhhResult", 
		L"Delect_MaintianPlan_By_JhhResult", 
		sizeof("Delect_MaintianPlan_By_JhhResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_MaintainPlan_Delect_MaintianPlan_By_Jhh_struct, Delect_MaintianPlan_By_JhhResult),
		NULL,
		&__Result_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWS_MaintainPlan_Delect_MaintianPlan_By_Jhh_map =
{
	0x5709274E,
	"Delect_MaintianPlan_By_Jhh",
	L"Delect_MaintianPlan_By_JhhResponse",
	sizeof("Delect_MaintianPlan_By_Jhh")-1,
	sizeof("Delect_MaintianPlan_By_JhhResponse")-1,
	SOAPMAP_FUNC,
	__CWS_MaintainPlan_Delect_MaintianPlan_By_Jhh_entries,
	sizeof(__CWS_MaintainPlan_Delect_MaintianPlan_By_Jhh_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWS_MaintainPlan_GetData_By_JHH_struct
{
	BSTR Jhh;
	BSTR GetData_By_JHHResult;
};

extern __declspec(selectany) const _soapmapentry __CWS_MaintainPlan_GetData_By_JHH_entries[] =
{

	{
		0x0001489A, 
		"Jhh", 
		L"Jhh", 
		sizeof("Jhh")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWS_MaintainPlan_GetData_By_JHH_struct, Jhh),
		NULL,
		NULL,
		-1,
	},
	{
		0x80101E4C, 
		"GetData_By_JHHResult", 
		L"GetData_By_JHHResult", 
		sizeof("GetData_By_JHHResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWS_MaintainPlan_GetData_By_JHH_struct, GetData_By_JHHResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWS_MaintainPlan_GetData_By_JHH_map =
{
	0xC438019C,
	"GetData_By_JHH",
	L"GetData_By_JHHResponse",
	sizeof("GetData_By_JHH")-1,
	sizeof("GetData_By_JHHResponse")-1,
	SOAPMAP_FUNC,
	__CWS_MaintainPlan_GetData_By_JHH_entries,
	sizeof(__CWS_MaintainPlan_GetData_By_JHH_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWS_MaintainPlan_Update_MaintainPlan2_struct
{
	BSTR xml;
	Result Update_MaintainPlan2Result;
};

extern __declspec(selectany) const _soapmapentry __CWS_MaintainPlan_Update_MaintainPlan2_entries[] =
{

	{
		0x00020CF1, 
		"xml", 
		L"xml", 
		sizeof("xml")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWS_MaintainPlan_Update_MaintainPlan2_struct, xml),
		NULL,
		NULL,
		-1,
	},
	{
		0x145A418F, 
		"Update_MaintainPlan2Result", 
		L"Update_MaintainPlan2Result", 
		sizeof("Update_MaintainPlan2Result")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_MaintainPlan_Update_MaintainPlan2_struct, Update_MaintainPlan2Result),
		NULL,
		&__Result_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWS_MaintainPlan_Update_MaintainPlan2_map =
{
	0x9398019F,
	"Update_MaintainPlan2",
	L"Update_MaintainPlan2Response",
	sizeof("Update_MaintainPlan2")-1,
	sizeof("Update_MaintainPlan2Response")-1,
	SOAPMAP_FUNC,
	__CWS_MaintainPlan_Update_MaintainPlan2_entries,
	sizeof(__CWS_MaintainPlan_Update_MaintainPlan2_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWS_MaintainPlan_Get_MaintainPlan_List_struct
{
	BSTR Get_MaintainPlan_ListResult;
};

extern __declspec(selectany) const _soapmapentry __CWS_MaintainPlan_Get_MaintainPlan_List_entries[] =
{

	{
		0x9B43B875, 
		"Get_MaintainPlan_ListResult", 
		L"Get_MaintainPlan_ListResult", 
		sizeof("Get_MaintainPlan_ListResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWS_MaintainPlan_Get_MaintainPlan_List_struct, Get_MaintainPlan_ListResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWS_MaintainPlan_Get_MaintainPlan_List_map =
{
	0x7ABACA05,
	"Get_MaintainPlan_List",
	L"Get_MaintainPlan_ListResponse",
	sizeof("Get_MaintainPlan_List")-1,
	sizeof("Get_MaintainPlan_ListResponse")-1,
	SOAPMAP_FUNC,
	__CWS_MaintainPlan_Get_MaintainPlan_List_entries,
	sizeof(__CWS_MaintainPlan_Get_MaintainPlan_List_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWS_MaintainPlan_Update_MaintainPlan_struct
{
	BSTR ds;
	Result Update_MaintainPlanResult;
};

extern __declspec(selectany) const _soapmapentry __CWS_MaintainPlan_Update_MaintainPlan_entries[] =
{

	{
		0x00000D57, 
		"ds", 
		L"ds", 
		sizeof("ds")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWS_MaintainPlan_Update_MaintainPlan_struct, ds),
		NULL,
		NULL,
		-1,
	},
	{
		0xB93DA41D, 
		"Update_MaintainPlanResult", 
		L"Update_MaintainPlanResult", 
		sizeof("Update_MaintainPlanResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_MaintainPlan_Update_MaintainPlan_struct, Update_MaintainPlanResult),
		NULL,
		&__Result_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWS_MaintainPlan_Update_MaintainPlan_map =
{
	0xFEDE3FAD,
	"Update_MaintainPlan",
	L"Update_MaintainPlanResponse",
	sizeof("Update_MaintainPlan")-1,
	sizeof("Update_MaintainPlanResponse")-1,
	SOAPMAP_FUNC,
	__CWS_MaintainPlan_Update_MaintainPlan_entries,
	sizeof(__CWS_MaintainPlan_Update_MaintainPlan_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};

extern __declspec(selectany) const _soapmap * __CWS_MaintainPlan_funcs[] =
{
	&__CWS_MaintainPlan_IsReady_map,
	&__CWS_MaintainPlan_Delect_MaintianPlan_By_Jhh_map,
	&__CWS_MaintainPlan_GetData_By_JHH_map,
	&__CWS_MaintainPlan_Update_MaintainPlan2_map,
	&__CWS_MaintainPlan_Get_MaintainPlan_List_map,
	&__CWS_MaintainPlan_Update_MaintainPlan_map,
	NULL
};

template <typename TClient>
inline HRESULT CWS_MaintainPlanT<TClient>::IsReady(
		bool* IsReadyResult
	)
{
    if ( IsReadyResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWS_MaintainPlan_IsReady_struct __params;
	memset(&__params, 0x00, sizeof(__params));

	__atlsoap_hr = SetClientStruct(&__params, 0);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/IsReady\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*IsReadyResult = __params.IsReadyResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWS_MaintainPlanT<TClient>::Delect_MaintianPlan_By_Jhh(
		BSTR Jhh, 
		Result* Delect_MaintianPlan_By_JhhResult
	)
{
    if ( Delect_MaintianPlan_By_JhhResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWS_MaintainPlan_Delect_MaintianPlan_By_Jhh_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Jhh = Jhh;

	__atlsoap_hr = SetClientStruct(&__params, 1);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/Delect_MaintianPlan_By_Jhh\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*Delect_MaintianPlan_By_JhhResult = __params.Delect_MaintianPlan_By_JhhResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWS_MaintainPlanT<TClient>::GetData_By_JHH(
		BSTR Jhh, 
		BSTR* GetData_By_JHHResult
	)
{
    if ( GetData_By_JHHResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWS_MaintainPlan_GetData_By_JHH_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Jhh = Jhh;

	__atlsoap_hr = SetClientStruct(&__params, 2);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/GetData_By_JHH\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*GetData_By_JHHResult = __params.GetData_By_JHHResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWS_MaintainPlanT<TClient>::Update_MaintainPlan2(
		BSTR xml, 
		Result* Update_MaintainPlan2Result
	)
{
    if ( Update_MaintainPlan2Result == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWS_MaintainPlan_Update_MaintainPlan2_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.xml = xml;

	__atlsoap_hr = SetClientStruct(&__params, 3);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/Update_MaintainPlan2\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*Update_MaintainPlan2Result = __params.Update_MaintainPlan2Result;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWS_MaintainPlanT<TClient>::Get_MaintainPlan_List(
		BSTR* Get_MaintainPlan_ListResult
	)
{
    if ( Get_MaintainPlan_ListResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWS_MaintainPlan_Get_MaintainPlan_List_struct __params;
	memset(&__params, 0x00, sizeof(__params));

	__atlsoap_hr = SetClientStruct(&__params, 4);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/Get_MaintainPlan_List\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*Get_MaintainPlan_ListResult = __params.Get_MaintainPlan_ListResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWS_MaintainPlanT<TClient>::Update_MaintainPlan(
		BSTR ds, 
		Result* Update_MaintainPlanResult
	)
{
    if ( Update_MaintainPlanResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWS_MaintainPlan_Update_MaintainPlan_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.ds = ds;

	__atlsoap_hr = SetClientStruct(&__params, 5);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/Update_MaintainPlan\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*Update_MaintainPlanResult = __params.Update_MaintainPlanResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
ATL_NOINLINE inline const _soapmap ** CWS_MaintainPlanT<TClient>::GetFunctionMap()
{
	return __CWS_MaintainPlan_funcs;
}

template <typename TClient>
ATL_NOINLINE inline const _soapmap ** CWS_MaintainPlanT<TClient>::GetHeaderMap()
{
	static const _soapmapentry __CWS_MaintainPlan_IsReady_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWS_MaintainPlan_IsReady_atlsoapheader_map = 
	{
		0x34FABC20,
		"IsReady",
		L"IsReadyResponse",
		sizeof("IsReady")-1,
		sizeof("IsReadyResponse")-1,
		SOAPMAP_HEADER,
		__CWS_MaintainPlan_IsReady_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWS_MaintainPlan_Delect_MaintianPlan_By_Jhh_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWS_MaintainPlan_Delect_MaintianPlan_By_Jhh_atlsoapheader_map = 
	{
		0x5709274E,
		"Delect_MaintianPlan_By_Jhh",
		L"Delect_MaintianPlan_By_JhhResponse",
		sizeof("Delect_MaintianPlan_By_Jhh")-1,
		sizeof("Delect_MaintianPlan_By_JhhResponse")-1,
		SOAPMAP_HEADER,
		__CWS_MaintainPlan_Delect_MaintianPlan_By_Jhh_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWS_MaintainPlan_GetData_By_JHH_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWS_MaintainPlan_GetData_By_JHH_atlsoapheader_map = 
	{
		0xC438019C,
		"GetData_By_JHH",
		L"GetData_By_JHHResponse",
		sizeof("GetData_By_JHH")-1,
		sizeof("GetData_By_JHHResponse")-1,
		SOAPMAP_HEADER,
		__CWS_MaintainPlan_GetData_By_JHH_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWS_MaintainPlan_Update_MaintainPlan2_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWS_MaintainPlan_Update_MaintainPlan2_atlsoapheader_map = 
	{
		0x9398019F,
		"Update_MaintainPlan2",
		L"Update_MaintainPlan2Response",
		sizeof("Update_MaintainPlan2")-1,
		sizeof("Update_MaintainPlan2Response")-1,
		SOAPMAP_HEADER,
		__CWS_MaintainPlan_Update_MaintainPlan2_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWS_MaintainPlan_Get_MaintainPlan_List_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWS_MaintainPlan_Get_MaintainPlan_List_atlsoapheader_map = 
	{
		0x7ABACA05,
		"Get_MaintainPlan_List",
		L"Get_MaintainPlan_ListResponse",
		sizeof("Get_MaintainPlan_List")-1,
		sizeof("Get_MaintainPlan_ListResponse")-1,
		SOAPMAP_HEADER,
		__CWS_MaintainPlan_Get_MaintainPlan_List_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWS_MaintainPlan_Update_MaintainPlan_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWS_MaintainPlan_Update_MaintainPlan_atlsoapheader_map = 
	{
		0xFEDE3FAD,
		"Update_MaintainPlan",
		L"Update_MaintainPlanResponse",
		sizeof("Update_MaintainPlan")-1,
		sizeof("Update_MaintainPlanResponse")-1,
		SOAPMAP_HEADER,
		__CWS_MaintainPlan_Update_MaintainPlan_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};


	static const _soapmap * __CWS_MaintainPlan_headers[] =
	{
		&__CWS_MaintainPlan_IsReady_atlsoapheader_map,
		&__CWS_MaintainPlan_Delect_MaintianPlan_By_Jhh_atlsoapheader_map,
		&__CWS_MaintainPlan_GetData_By_JHH_atlsoapheader_map,
		&__CWS_MaintainPlan_Update_MaintainPlan2_atlsoapheader_map,
		&__CWS_MaintainPlan_Get_MaintainPlan_List_atlsoapheader_map,
		&__CWS_MaintainPlan_Update_MaintainPlan_atlsoapheader_map,
		NULL
	};
	
	return __CWS_MaintainPlan_headers;
}

template <typename TClient>
ATL_NOINLINE inline void * CWS_MaintainPlanT<TClient>::GetHeaderValue()
{
	return this;
}

template <typename TClient>
ATL_NOINLINE inline const wchar_t * CWS_MaintainPlanT<TClient>::GetNamespaceUri()
{
	return L"http://tempuri.org/";
}

template <typename TClient>
ATL_NOINLINE inline const char * CWS_MaintainPlanT<TClient>::GetServiceName()
{
	return NULL;
}

template <typename TClient>
ATL_NOINLINE inline const char * CWS_MaintainPlanT<TClient>::GetNamespaceUriA()
{
	return "http://tempuri.org/";
}

template <typename TClient>
ATL_NOINLINE inline HRESULT CWS_MaintainPlanT<TClient>::CallFunction(
	void *, 
	const wchar_t *, int,
	size_t)
{
	return E_NOTIMPL;
}

template <typename TClient>
ATL_NOINLINE inline HRESULT CWS_MaintainPlanT<TClient>::GetClientReader(ISAXXMLReader **ppReader)
{
	if (ppReader == NULL)
	{
		return E_INVALIDARG;
	}
	
	CComPtr<ISAXXMLReader> spReader = GetReader();
	if (spReader.p != NULL)
	{
		*ppReader = spReader.Detach();
		return S_OK;
	}
	return TClient::GetClientReader(ppReader);
}

} // namespace WS_MaintainPlan

template<>
inline HRESULT AtlCleanupValue<WS_MaintainPlan::Result>(WS_MaintainPlan::Result *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->succeed);
	AtlCleanupValue(&pVal->message);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WS_MaintainPlan::Result>(WS_MaintainPlan::Result *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->succeed, pMemMgr);
	AtlCleanupValueEx(&pVal->message, pMemMgr);
	return S_OK;
}
