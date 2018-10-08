//
// sproxy.exe generated file
// do not modify this file
//
// Created: 06/25/2009@21:54:18
//

#pragma once


#ifndef _WIN32_WINDOWS
#pragma message("warning: defining _WIN32_WINDOWS = 0x0410")
#define _WIN32_WINDOWS 0x0410
#endif

#include <atlsoap.h>

namespace WS_DCMD
{

struct Result
{
	bool succeed;
	BSTR message;
};

template <typename TClient = CSoapSocketClientT<> >
class CWS_DCMDT : 
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

	CWS_DCMDT(const char *serveraddr,ISAXXMLReader *pReader = NULL)
		:TClient(_T(serveraddr))
	{
		SetClient(true);
		SetReader(pReader);
	}
	
	~CWS_DCMDT()
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

	HRESULT Update_Dcmd_By_XML(
		BSTR xml, 
		Result* Update_Dcmd_By_XMLResult
	);

	HRESULT Get_dcmd_index_by_Condition(
		BSTR Condition, 
		BSTR* Get_dcmd_index_by_ConditionResult
	);

	HRESULT Hello(
		BSTR* HelloResult
	);
};

typedef CWS_DCMDT<> CWS_DCMD;

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

struct __CWS_DCMD_IsReady_struct
{
	bool IsReadyResult;
};

extern __declspec(selectany) const _soapmapentry __CWS_DCMD_IsReady_entries[] =
{

	{
		0x851A67D0, 
		"IsReadyResult", 
		L"IsReadyResult", 
		sizeof("IsReadyResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_DCMD_IsReady_struct, IsReadyResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWS_DCMD_IsReady_map =
{
	0x34FABC20,
	"IsReady",
	L"IsReadyResponse",
	sizeof("IsReady")-1,
	sizeof("IsReadyResponse")-1,
	SOAPMAP_FUNC,
	__CWS_DCMD_IsReady_entries,
	sizeof(__CWS_DCMD_IsReady_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWS_DCMD_Update_Dcmd_By_XML_struct
{
	BSTR xml;
	Result Update_Dcmd_By_XMLResult;
};

extern __declspec(selectany) const _soapmapentry __CWS_DCMD_Update_Dcmd_By_XML_entries[] =
{

	{
		0x00020CF1, 
		"xml", 
		L"xml", 
		sizeof("xml")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWS_DCMD_Update_Dcmd_By_XML_struct, xml),
		NULL,
		NULL,
		-1,
	},
	{
		0x2A8BB963, 
		"Update_Dcmd_By_XMLResult", 
		L"Update_Dcmd_By_XMLResult", 
		sizeof("Update_Dcmd_By_XMLResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_DCMD_Update_Dcmd_By_XML_struct, Update_Dcmd_By_XMLResult),
		NULL,
		&__Result_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWS_DCMD_Update_Dcmd_By_XML_map =
{
	0xFC06BE73,
	"Update_Dcmd_By_XML",
	L"Update_Dcmd_By_XMLResponse",
	sizeof("Update_Dcmd_By_XML")-1,
	sizeof("Update_Dcmd_By_XMLResponse")-1,
	SOAPMAP_FUNC,
	__CWS_DCMD_Update_Dcmd_By_XML_entries,
	sizeof(__CWS_DCMD_Update_Dcmd_By_XML_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWS_DCMD_Get_dcmd_index_by_Condition_struct
{
	BSTR Condition;
	BSTR Get_dcmd_index_by_ConditionResult;
};

extern __declspec(selectany) const _soapmapentry __CWS_DCMD_Get_dcmd_index_by_Condition_entries[] =
{

	{
		0xF19450E7, 
		"Condition", 
		L"Condition", 
		sizeof("Condition")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWS_DCMD_Get_dcmd_index_by_Condition_struct, Condition),
		NULL,
		NULL,
		-1,
	},
	{
		0x972B408D, 
		"Get_dcmd_index_by_ConditionResult", 
		L"Get_dcmd_index_by_ConditionResult", 
		sizeof("Get_dcmd_index_by_ConditionResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWS_DCMD_Get_dcmd_index_by_Condition_struct, Get_dcmd_index_by_ConditionResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWS_DCMD_Get_dcmd_index_by_Condition_map =
{
	0x0EA4B81D,
	"Get_dcmd_index_by_Condition",
	L"Get_dcmd_index_by_ConditionResponse",
	sizeof("Get_dcmd_index_by_Condition")-1,
	sizeof("Get_dcmd_index_by_ConditionResponse")-1,
	SOAPMAP_FUNC,
	__CWS_DCMD_Get_dcmd_index_by_Condition_entries,
	sizeof(__CWS_DCMD_Get_dcmd_index_by_Condition_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWS_DCMD_Hello_struct
{
	BSTR HelloResult;
};

extern __declspec(selectany) const _soapmapentry __CWS_DCMD_Hello_entries[] =
{

	{
		0x11AFCD73, 
		"HelloResult", 
		L"HelloResult", 
		sizeof("HelloResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWS_DCMD_Hello_struct, HelloResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWS_DCMD_Hello_map =
{
	0x3C801683,
	"Hello",
	L"HelloResponse",
	sizeof("Hello")-1,
	sizeof("HelloResponse")-1,
	SOAPMAP_FUNC,
	__CWS_DCMD_Hello_entries,
	sizeof(__CWS_DCMD_Hello_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};

extern __declspec(selectany) const _soapmap * __CWS_DCMD_funcs[] =
{
	&__CWS_DCMD_IsReady_map,
	&__CWS_DCMD_Update_Dcmd_By_XML_map,
	&__CWS_DCMD_Get_dcmd_index_by_Condition_map,
	&__CWS_DCMD_Hello_map,
	NULL
};

template <typename TClient>
inline HRESULT CWS_DCMDT<TClient>::IsReady(
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
	__CWS_DCMD_IsReady_struct __params;
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
inline HRESULT CWS_DCMDT<TClient>::Update_Dcmd_By_XML(
		BSTR xml, 
		Result* Update_Dcmd_By_XMLResult
	)
{
    if ( Update_Dcmd_By_XMLResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWS_DCMD_Update_Dcmd_By_XML_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.xml = xml;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/Update_Dcmd_By_XML\"\r\n"));
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

	*Update_Dcmd_By_XMLResult = __params.Update_Dcmd_By_XMLResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWS_DCMDT<TClient>::Get_dcmd_index_by_Condition(
		BSTR Condition, 
		BSTR* Get_dcmd_index_by_ConditionResult
	)
{
    if ( Get_dcmd_index_by_ConditionResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWS_DCMD_Get_dcmd_index_by_Condition_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Condition = Condition;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/Get_dcmd_index_by_Condition\"\r\n"));
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

	*Get_dcmd_index_by_ConditionResult = __params.Get_dcmd_index_by_ConditionResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWS_DCMDT<TClient>::Hello(
		BSTR* HelloResult
	)
{
    if ( HelloResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWS_DCMD_Hello_struct __params;
	memset(&__params, 0x00, sizeof(__params));

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/Hello\"\r\n"));
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

	*HelloResult = __params.HelloResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
ATL_NOINLINE inline const _soapmap ** CWS_DCMDT<TClient>::GetFunctionMap()
{
	return __CWS_DCMD_funcs;
}

template <typename TClient>
ATL_NOINLINE inline const _soapmap ** CWS_DCMDT<TClient>::GetHeaderMap()
{
	static const _soapmapentry __CWS_DCMD_IsReady_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWS_DCMD_IsReady_atlsoapheader_map = 
	{
		0x34FABC20,
		"IsReady",
		L"IsReadyResponse",
		sizeof("IsReady")-1,
		sizeof("IsReadyResponse")-1,
		SOAPMAP_HEADER,
		__CWS_DCMD_IsReady_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWS_DCMD_Update_Dcmd_By_XML_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWS_DCMD_Update_Dcmd_By_XML_atlsoapheader_map = 
	{
		0xFC06BE73,
		"Update_Dcmd_By_XML",
		L"Update_Dcmd_By_XMLResponse",
		sizeof("Update_Dcmd_By_XML")-1,
		sizeof("Update_Dcmd_By_XMLResponse")-1,
		SOAPMAP_HEADER,
		__CWS_DCMD_Update_Dcmd_By_XML_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWS_DCMD_Get_dcmd_index_by_Condition_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWS_DCMD_Get_dcmd_index_by_Condition_atlsoapheader_map = 
	{
		0x0EA4B81D,
		"Get_dcmd_index_by_Condition",
		L"Get_dcmd_index_by_ConditionResponse",
		sizeof("Get_dcmd_index_by_Condition")-1,
		sizeof("Get_dcmd_index_by_ConditionResponse")-1,
		SOAPMAP_HEADER,
		__CWS_DCMD_Get_dcmd_index_by_Condition_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWS_DCMD_Hello_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWS_DCMD_Hello_atlsoapheader_map = 
	{
		0x3C801683,
		"Hello",
		L"HelloResponse",
		sizeof("Hello")-1,
		sizeof("HelloResponse")-1,
		SOAPMAP_HEADER,
		__CWS_DCMD_Hello_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};


	static const _soapmap * __CWS_DCMD_headers[] =
	{
		&__CWS_DCMD_IsReady_atlsoapheader_map,
		&__CWS_DCMD_Update_Dcmd_By_XML_atlsoapheader_map,
		&__CWS_DCMD_Get_dcmd_index_by_Condition_atlsoapheader_map,
		&__CWS_DCMD_Hello_atlsoapheader_map,
		NULL
	};
	
	return __CWS_DCMD_headers;
}

template <typename TClient>
ATL_NOINLINE inline void * CWS_DCMDT<TClient>::GetHeaderValue()
{
	return this;
}

template <typename TClient>
ATL_NOINLINE inline const wchar_t * CWS_DCMDT<TClient>::GetNamespaceUri()
{
	return L"http://tempuri.org/";
}

template <typename TClient>
ATL_NOINLINE inline const char * CWS_DCMDT<TClient>::GetServiceName()
{
	return NULL;
}

template <typename TClient>
ATL_NOINLINE inline const char * CWS_DCMDT<TClient>::GetNamespaceUriA()
{
	return "http://tempuri.org/";
}

template <typename TClient>
ATL_NOINLINE inline HRESULT CWS_DCMDT<TClient>::CallFunction(
	void *, 
	const wchar_t *, int,
	size_t)
{
	return E_NOTIMPL;
}

template <typename TClient>
ATL_NOINLINE inline HRESULT CWS_DCMDT<TClient>::GetClientReader(ISAXXMLReader **ppReader)
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

} // namespace WS_DCMD

template<>
inline HRESULT AtlCleanupValue<WS_DCMD::Result>(WS_DCMD::Result *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->succeed);
	AtlCleanupValue(&pVal->message);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WS_DCMD::Result>(WS_DCMD::Result *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->succeed, pMemMgr);
	AtlCleanupValueEx(&pVal->message, pMemMgr);
	return S_OK;
}
