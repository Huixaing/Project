//
// sproxy.exe generated file
// do not modify this file
//
// Created: 05/25/2009@09:01:14
//

#pragma once


#ifndef _WIN32_WINDOWS
#pragma message("warning: defining _WIN32_WINDOWS = 0x0410")
#define _WIN32_WINDOWS 0x0410
#endif

#include <atlsoap.h>

namespace WS_DCMD
{

struct Recv_Dcmd_Xml_Info
{
	int Recv_Send_Entity;
	BSTR Recv_Creat_Time;
	BSTR Train_ID;
	BSTR Locomotive;
	int Station_ID;
	BSTR Recv_Time;
	int Recv_entity;
	int Recv_Status;
	BSTR Sign_Name;
	BSTR Cmd_no;
	bool parse_succeed;
};

struct DutyChief_Info
{
	int DutyChief_EntityID;
	BSTR DutyChief_DisplayName;
	int DutyChief_ID;
	int DutyChief_GPCID;
};

struct Result
{
	bool succeed;
	BSTR message;
};

struct Console_Info
{
	int Console_EntityID;
	BSTR Console_DisplayName;
	int Console_ID;
	int Console_GPCID;
};

struct Station_Info
{
	int Station_EntityID;
	BSTR Station_DisplayName;
	int Station_ID;
	int Lirc_EntityID;
};

struct DCMD_Setting
{
	int Send_Entity;
	int Sender_Type;
	int Sender_ID;
	int Tcc_Version;
	int OPMS_Interface;
	BSTR Console_DisplayName;
	int Dcmd_num_Start;
	int Dcmd_num_End;
	int GSM_R_entity;
	int BaseLevel_entity;
	int TSR_entity;
	Station_Info *Recv_Station_List;
	int __Recv_Station_List_nSizeIs;
	Console_Info *Recv_Console_List;
	int __Recv_Console_List_nSizeIs;
	DutyChief_Info *Recv_DutyChief_List;
	int __Recv_DutyChief_List_nSizeIs;
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

	CWS_DCMDT(ISAXXMLReader *pReader = NULL)
		:TClient(_T("http://192.100.88.226:1111/ws_dcmd.asmx"))
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

	HRESULT Reset_DcmdNumber(
		int Sender_id, 
		int cmd_Oper, 
		Result* Reset_DcmdNumberResult
	);

	HRESULT Update_Dcmd_Number_Setting(
		BSTR ds, 
		Result* Update_Dcmd_Number_SettingResult
	);

	HRESULT Get_dcmd_template_all(
		BSTR* Get_dcmd_template_allResult
	);

	HRESULT Get_dcmd_By_dcmdid(
		BSTR dcmd_id, 
		BSTR* Get_dcmd_By_dcmdidResult
	);

	HRESULT Get_SignRecord_By_DCMDID(
		BSTR dcmdid, 
		BSTR* Get_SignRecord_By_DCMDIDResult
	);

	HRESULT Get_SignRecord_data(
		BSTR* Get_SignRecord_dataResult
	);

	HRESULT Update_Signal_Status_Local(
		int sendentity, 
		BSTR creattime, 
		BSTR dcmdnum, 
		int status, 
		DCMD_Setting current_Setting, 
		Result* Update_Signal_Status_LocalResult
	);

	HRESULT Get_Dcmd_Number_Setting_All(
		BSTR* Get_Dcmd_Number_Setting_AllResult
	);

	HRESULT Update_Dcmd_and_DcmdNum(
		BSTR ds, 
		BSTR str_number, 
		int theSender_ID, 
		int cmd_Oper, 
		Result* Update_Dcmd_and_DcmdNumResult
	);

	HRESULT Update_Dcmd(
		BSTR ds, 
		Result* Update_DcmdResult
	);

	HRESULT Update_Recv_Status(
		Recv_Dcmd_Xml_Info recv_info, 
		int senderid, 
		bool is_dutychief, 
		Result* Update_Recv_StatusResult
	);

	HRESULT Delete_Dcmd_By_Dcmdid(
		BSTR dcmd_id, 
		Result* Delete_Dcmd_By_DcmdidResult
	);

	HRESULT Is_All_Signaled(
		BSTR ds, 
		bool* Is_All_SignaledResult
	);

	HRESULT Is_DB_Connected(
		Result* Is_DB_ConnectedResult
	);

	HRESULT Update_SignRecord(
		BSTR ds, 
		Result* Update_SignRecordResult
	);

	HRESULT Is_recv_info_HasSaved(
		int sendentity, 
		BSTR creattime, 
		BSTR dcmdnum, 
		int senderid, 
		bool* Is_recv_info_HasSavedResult
	);

	HRESULT Get_DCMD_ID_By_recv_info2(
		int sendentity, 
		BSTR creattime, 
		BSTR dcmdnum, 
		int senderid, 
		Result* Get_DCMD_ID_By_recv_info2Result
	);

	HRESULT Get_DCMD_ID_By_recv_info(
		Recv_Dcmd_Xml_Info recv_info, 
		Result* Get_DCMD_ID_By_recv_infoResult
	);

	HRESULT IS_DcmdNumber_Validated(
		BSTR theNumber, 
		bool* IS_DcmdNumber_ValidatedResult
	);

	HRESULT Get_dcmd_index_by_Condition(
		BSTR Condition, 
		BSTR* Get_dcmd_index_by_ConditionResult
	);

	HRESULT Insert_Dcmd_main(
		BSTR xml, 
		Result* Insert_Dcmd_mainResult
	);

	HRESULT Update_Dcmd_Template(
		BSTR ds, 
		Result* Update_Dcmd_TemplateResult
	);

	HRESULT Hello(
		BSTR* HelloResult
	);

	HRESULT Reset_DcmdNumber_By_DCMD_DATE(
		int Sender_id, 
		BSTR current_dcmd_date, 
		Result* Reset_DcmdNumber_By_DCMD_DATEResult
	);

	HRESULT Get_Next_DCMD_Number(
		int Sender_id, 
		int command_Oper, 
		Result* Get_Next_DCMD_NumberResult
	);
};

typedef CWS_DCMDT<> CWS_DCMD;

__if_not_exists(__Recv_Dcmd_Xml_Info_entries)
{
extern __declspec(selectany) const _soapmapentry __Recv_Dcmd_Xml_Info_entries[] =
{
	{ 
		0x646AEDD5, 
		"Recv_Send_Entity", 
		L"Recv_Send_Entity", 
		sizeof("Recv_Send_Entity")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(Recv_Dcmd_Xml_Info, Recv_Send_Entity),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xD1028CCC, 
		"Recv_Creat_Time", 
		L"Recv_Creat_Time", 
		sizeof("Recv_Creat_Time")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD, 
		offsetof(Recv_Dcmd_Xml_Info, Recv_Creat_Time),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xB6F7492A, 
		"Train_ID", 
		L"Train_ID", 
		sizeof("Train_ID")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(Recv_Dcmd_Xml_Info, Train_ID),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x796A16A1, 
		"Locomotive", 
		L"Locomotive", 
		sizeof("Locomotive")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(Recv_Dcmd_Xml_Info, Locomotive),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x7B29AC4E, 
		"Station_ID", 
		L"Station_ID", 
		sizeof("Station_ID")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(Recv_Dcmd_Xml_Info, Station_ID),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x64C25CBE, 
		"Recv_Time", 
		L"Recv_Time", 
		sizeof("Recv_Time")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD, 
		offsetof(Recv_Dcmd_Xml_Info, Recv_Time),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xC6D2A88C, 
		"Recv_entity", 
		L"Recv_entity", 
		sizeof("Recv_entity")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(Recv_Dcmd_Xml_Info, Recv_entity),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x9D3823F3, 
		"Recv_Status", 
		L"Recv_Status", 
		sizeof("Recv_Status")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(Recv_Dcmd_Xml_Info, Recv_Status),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xAB8262F1, 
		"Sign_Name", 
		L"Sign_Name", 
		sizeof("Sign_Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(Recv_Dcmd_Xml_Info, Sign_Name),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xA43689B0, 
		"Cmd_no", 
		L"Cmd_no", 
		sizeof("Cmd_no")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(Recv_Dcmd_Xml_Info, Cmd_no),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xC7E56296, 
		"parse_succeed", 
		L"parse_succeed", 
		sizeof("parse_succeed")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD, 
		offsetof(Recv_Dcmd_Xml_Info, parse_succeed),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __Recv_Dcmd_Xml_Info_map =
{
	0xC57B58E2,
	"Recv_Dcmd_Xml_Info",
	L"Recv_Dcmd_Xml_Info",
	sizeof("Recv_Dcmd_Xml_Info")-1,
	sizeof("Recv_Dcmd_Xml_Info")-1,
	SOAPMAP_STRUCT,
	__Recv_Dcmd_Xml_Info_entries,
	sizeof(Recv_Dcmd_Xml_Info),
	11,
	-1,
	SOAPFLAG_NONE,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};
}

__if_not_exists(__DutyChief_Info_entries)
{
extern __declspec(selectany) const _soapmapentry __DutyChief_Info_entries[] =
{
	{ 
		0x300982AE, 
		"DutyChief_EntityID", 
		L"DutyChief_EntityID", 
		sizeof("DutyChief_EntityID")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(DutyChief_Info, DutyChief_EntityID),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x326E8C1B, 
		"DutyChief_DisplayName", 
		L"DutyChief_DisplayName", 
		sizeof("DutyChief_DisplayName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DutyChief_Info, DutyChief_DisplayName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xEECD1791, 
		"DutyChief_ID", 
		L"DutyChief_ID", 
		sizeof("DutyChief_ID")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(DutyChief_Info, DutyChief_ID),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xA47CC42B, 
		"DutyChief_GPCID", 
		L"DutyChief_GPCID", 
		sizeof("DutyChief_GPCID")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(DutyChief_Info, DutyChief_GPCID),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __DutyChief_Info_map =
{
	0xD6720010,
	"DutyChief_Info",
	L"DutyChief_Info",
	sizeof("DutyChief_Info")-1,
	sizeof("DutyChief_Info")-1,
	SOAPMAP_STRUCT,
	__DutyChief_Info_entries,
	sizeof(DutyChief_Info),
	4,
	-1,
	SOAPFLAG_NONE,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};
}

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

__if_not_exists(__Console_Info_entries)
{
extern __declspec(selectany) const _soapmapentry __Console_Info_entries[] =
{
	{ 
		0x41AF485C, 
		"Console_EntityID", 
		L"Console_EntityID", 
		sizeof("Console_EntityID")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(Console_Info, Console_EntityID),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x8A599B09, 
		"Console_DisplayName", 
		L"Console_DisplayName", 
		sizeof("Console_DisplayName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(Console_Info, Console_DisplayName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xB433B2BF, 
		"Console_ID", 
		L"Console_ID", 
		sizeof("Console_ID")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(Console_Info, Console_ID),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x8D3AB899, 
		"Console_GPCID", 
		L"Console_GPCID", 
		sizeof("Console_GPCID")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(Console_Info, Console_GPCID),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __Console_Info_map =
{
	0x8FEC1EBE,
	"Console_Info",
	L"Console_Info",
	sizeof("Console_Info")-1,
	sizeof("Console_Info")-1,
	SOAPMAP_STRUCT,
	__Console_Info_entries,
	sizeof(Console_Info),
	4,
	-1,
	SOAPFLAG_NONE,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};
}

__if_not_exists(__Station_Info_entries)
{
extern __declspec(selectany) const _soapmapentry __Station_Info_entries[] =
{
	{ 
		0x1143F12B, 
		"Station_EntityID", 
		L"Station_EntityID", 
		sizeof("Station_EntityID")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(Station_Info, Station_EntityID),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x7DFEC578, 
		"Station_DisplayName", 
		L"Station_DisplayName", 
		sizeof("Station_DisplayName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(Station_Info, Station_DisplayName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x7B29AC4E, 
		"Station_ID", 
		L"Station_ID", 
		sizeof("Station_ID")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(Station_Info, Station_ID),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xA91D7EF3, 
		"Lirc_EntityID", 
		L"Lirc_EntityID", 
		sizeof("Lirc_EntityID")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(Station_Info, Lirc_EntityID),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __Station_Info_map =
{
	0xEC46B80D,
	"Station_Info",
	L"Station_Info",
	sizeof("Station_Info")-1,
	sizeof("Station_Info")-1,
	SOAPMAP_STRUCT,
	__Station_Info_entries,
	sizeof(Station_Info),
	4,
	-1,
	SOAPFLAG_NONE,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};
}

__if_not_exists(__DCMD_Setting_entries)
{
extern __declspec(selectany) const _soapmapentry __DCMD_Setting_entries[] =
{
	{ 
		0x66422CE6, 
		"Send_Entity", 
		L"Send_Entity", 
		sizeof("Send_Entity")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(DCMD_Setting, Send_Entity),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x9BF70582, 
		"Sender_Type", 
		L"Sender_Type", 
		sizeof("Sender_Type")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(DCMD_Setting, Sender_Type),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xFE7F65CD, 
		"Sender_ID", 
		L"Sender_ID", 
		sizeof("Sender_ID")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(DCMD_Setting, Sender_ID),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xDB0882BF, 
		"Tcc_Version", 
		L"Tcc_Version", 
		sizeof("Tcc_Version")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(DCMD_Setting, Tcc_Version),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x1DBD394F, 
		"OPMS_Interface", 
		L"OPMS_Interface", 
		sizeof("OPMS_Interface")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(DCMD_Setting, OPMS_Interface),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x8A599B09, 
		"Console_DisplayName", 
		L"Console_DisplayName", 
		sizeof("Console_DisplayName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DCMD_Setting, Console_DisplayName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x90656B54, 
		"Dcmd_num_Start", 
		L"Dcmd_num_Start", 
		sizeof("Dcmd_num_Start")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(DCMD_Setting, Dcmd_num_Start),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xA5643C5D, 
		"Dcmd_num_End", 
		L"Dcmd_num_End", 
		sizeof("Dcmd_num_End")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(DCMD_Setting, Dcmd_num_End),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x737CF4F4, 
		"GSM_R_entity", 
		L"GSM_R_entity", 
		sizeof("GSM_R_entity")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(DCMD_Setting, GSM_R_entity),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x1A2B06EF, 
		"BaseLevel_entity", 
		L"BaseLevel_entity", 
		sizeof("BaseLevel_entity")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(DCMD_Setting, BaseLevel_entity),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x4963F6D5, 
		"TSR_entity", 
		L"TSR_entity", 
		sizeof("TSR_entity")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(DCMD_Setting, TSR_entity),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xAF89286C, 
		"Recv_Station_List", 
		L"Recv_Station_List", 
		sizeof("Recv_Station_List")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(DCMD_Setting, Recv_Station_List),
		NULL, 
		&__Station_Info_map, 
		11+1 
	},
	{ 
		0xAF89286C, 
		"Recv_Station_List", 
		L"Recv_Station_List", 
		sizeof("Recv_Station_List")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(DCMD_Setting, __Recv_Station_List_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 
		0x532E8F1D, 
		"Recv_Console_List", 
		L"Recv_Console_List", 
		sizeof("Recv_Console_List")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(DCMD_Setting, Recv_Console_List),
		NULL, 
		&__Console_Info_map, 
		13+1 
	},
	{ 
		0x532E8F1D, 
		"Recv_Console_List", 
		L"Recv_Console_List", 
		sizeof("Recv_Console_List")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(DCMD_Setting, __Recv_Console_List_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 
		0x6D67602F, 
		"Recv_DutyChief_List", 
		L"Recv_DutyChief_List", 
		sizeof("Recv_DutyChief_List")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(DCMD_Setting, Recv_DutyChief_List),
		NULL, 
		&__DutyChief_Info_map, 
		15+1 
	},
	{ 
		0x6D67602F, 
		"Recv_DutyChief_List", 
		L"Recv_DutyChief_List", 
		sizeof("Recv_DutyChief_List")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(DCMD_Setting, __Recv_DutyChief_List_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __DCMD_Setting_map =
{
	0xA5C12635,
	"DCMD_Setting",
	L"DCMD_Setting",
	sizeof("DCMD_Setting")-1,
	sizeof("DCMD_Setting")-1,
	SOAPMAP_STRUCT,
	__DCMD_Setting_entries,
	sizeof(DCMD_Setting),
	14,
	-1,
	SOAPFLAG_NONE,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};
}

struct __CWS_DCMD_Reset_DcmdNumber_struct
{
	int Sender_id;
	int cmd_Oper;
	Result Reset_DcmdNumberResult;
};

extern __declspec(selectany) const _soapmapentry __CWS_DCMD_Reset_DcmdNumber_entries[] =
{

	{
		0xFE7F6A0D, 
		"Sender_id", 
		L"Sender_id", 
		sizeof("Sender_id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_DCMD_Reset_DcmdNumber_struct, Sender_id),
		NULL,
		NULL,
		-1,
	},
	{
		0x142963C9, 
		"cmd_Oper", 
		L"cmd_Oper", 
		sizeof("cmd_Oper")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_DCMD_Reset_DcmdNumber_struct, cmd_Oper),
		NULL,
		NULL,
		-1,
	},
	{
		0xA1E720E2, 
		"Reset_DcmdNumberResult", 
		L"Reset_DcmdNumberResult", 
		sizeof("Reset_DcmdNumberResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_DCMD_Reset_DcmdNumber_struct, Reset_DcmdNumberResult),
		NULL,
		&__Result_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWS_DCMD_Reset_DcmdNumber_map =
{
	0xB7DA01B2,
	"Reset_DcmdNumber",
	L"Reset_DcmdNumberResponse",
	sizeof("Reset_DcmdNumber")-1,
	sizeof("Reset_DcmdNumberResponse")-1,
	SOAPMAP_FUNC,
	__CWS_DCMD_Reset_DcmdNumber_entries,
	sizeof(__CWS_DCMD_Reset_DcmdNumber_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWS_DCMD_Update_Dcmd_Number_Setting_struct
{
	BSTR ds;
	Result Update_Dcmd_Number_SettingResult;
};

extern __declspec(selectany) const _soapmapentry __CWS_DCMD_Update_Dcmd_Number_Setting_entries[] =
{

	{
		0x00000D57, 
		"ds", 
		L"ds", 
		sizeof("ds")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWS_DCMD_Update_Dcmd_Number_Setting_struct, ds),
		NULL,
		NULL,
		-1,
	},
	{
		0x0E97517E, 
		"Update_Dcmd_Number_SettingResult", 
		L"Update_Dcmd_Number_SettingResult", 
		sizeof("Update_Dcmd_Number_SettingResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_DCMD_Update_Dcmd_Number_Setting_struct, Update_Dcmd_Number_SettingResult),
		NULL,
		&__Result_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWS_DCMD_Update_Dcmd_Number_Setting_map =
{
	0x1158C94E,
	"Update_Dcmd_Number_Setting",
	L"Update_Dcmd_Number_SettingResponse",
	sizeof("Update_Dcmd_Number_Setting")-1,
	sizeof("Update_Dcmd_Number_SettingResponse")-1,
	SOAPMAP_FUNC,
	__CWS_DCMD_Update_Dcmd_Number_Setting_entries,
	sizeof(__CWS_DCMD_Update_Dcmd_Number_Setting_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWS_DCMD_Get_dcmd_template_all_struct
{
	BSTR Get_dcmd_template_allResult;
};

extern __declspec(selectany) const _soapmapentry __CWS_DCMD_Get_dcmd_template_all_entries[] =
{

	{
		0x144BCFA9, 
		"Get_dcmd_template_allResult", 
		L"Get_dcmd_template_allResult", 
		sizeof("Get_dcmd_template_allResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWS_DCMD_Get_dcmd_template_all_struct, Get_dcmd_template_allResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWS_DCMD_Get_dcmd_template_all_map =
{
	0x56257E39,
	"Get_dcmd_template_all",
	L"Get_dcmd_template_allResponse",
	sizeof("Get_dcmd_template_all")-1,
	sizeof("Get_dcmd_template_allResponse")-1,
	SOAPMAP_FUNC,
	__CWS_DCMD_Get_dcmd_template_all_entries,
	sizeof(__CWS_DCMD_Get_dcmd_template_all_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWS_DCMD_Get_dcmd_By_dcmdid_struct
{
	BSTR dcmd_id;
	BSTR Get_dcmd_By_dcmdidResult;
};

extern __declspec(selectany) const _soapmapentry __CWS_DCMD_Get_dcmd_By_dcmdid_entries[] =
{

	{
		0x009B5884, 
		"dcmd_id", 
		L"dcmd_id", 
		sizeof("dcmd_id")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWS_DCMD_Get_dcmd_By_dcmdid_struct, dcmd_id),
		NULL,
		NULL,
		-1,
	},
	{
		0x0A4C4454, 
		"Get_dcmd_By_dcmdidResult", 
		L"Get_dcmd_By_dcmdidResult", 
		sizeof("Get_dcmd_By_dcmdidResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWS_DCMD_Get_dcmd_By_dcmdid_struct, Get_dcmd_By_dcmdidResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWS_DCMD_Get_dcmd_By_dcmdid_map =
{
	0xCE15C9A4,
	"Get_dcmd_By_dcmdid",
	L"Get_dcmd_By_dcmdidResponse",
	sizeof("Get_dcmd_By_dcmdid")-1,
	sizeof("Get_dcmd_By_dcmdidResponse")-1,
	SOAPMAP_FUNC,
	__CWS_DCMD_Get_dcmd_By_dcmdid_entries,
	sizeof(__CWS_DCMD_Get_dcmd_By_dcmdid_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWS_DCMD_Get_SignRecord_By_DCMDID_struct
{
	BSTR dcmdid;
	BSTR Get_SignRecord_By_DCMDIDResult;
};

extern __declspec(selectany) const _soapmapentry __CWS_DCMD_Get_SignRecord_By_DCMDID_entries[] =
{

	{
		0xF080D565, 
		"dcmdid", 
		L"dcmdid", 
		sizeof("dcmdid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWS_DCMD_Get_SignRecord_By_DCMDID_struct, dcmdid),
		NULL,
		NULL,
		-1,
	},
	{
		0x184EF12C, 
		"Get_SignRecord_By_DCMDIDResult", 
		L"Get_SignRecord_By_DCMDIDResult", 
		sizeof("Get_SignRecord_By_DCMDIDResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWS_DCMD_Get_SignRecord_By_DCMDID_struct, Get_SignRecord_By_DCMDIDResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWS_DCMD_Get_SignRecord_By_DCMDID_map =
{
	0x67770C7C,
	"Get_SignRecord_By_DCMDID",
	L"Get_SignRecord_By_DCMDIDResponse",
	sizeof("Get_SignRecord_By_DCMDID")-1,
	sizeof("Get_SignRecord_By_DCMDIDResponse")-1,
	SOAPMAP_FUNC,
	__CWS_DCMD_Get_SignRecord_By_DCMDID_entries,
	sizeof(__CWS_DCMD_Get_SignRecord_By_DCMDID_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWS_DCMD_Get_SignRecord_data_struct
{
	BSTR Get_SignRecord_dataResult;
};

extern __declspec(selectany) const _soapmapentry __CWS_DCMD_Get_SignRecord_data_entries[] =
{

	{
		0x775D11A7, 
		"Get_SignRecord_dataResult", 
		L"Get_SignRecord_dataResult", 
		sizeof("Get_SignRecord_dataResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWS_DCMD_Get_SignRecord_data_struct, Get_SignRecord_dataResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWS_DCMD_Get_SignRecord_data_map =
{
	0xC28F37B7,
	"Get_SignRecord_data",
	L"Get_SignRecord_dataResponse",
	sizeof("Get_SignRecord_data")-1,
	sizeof("Get_SignRecord_dataResponse")-1,
	SOAPMAP_FUNC,
	__CWS_DCMD_Get_SignRecord_data_entries,
	sizeof(__CWS_DCMD_Get_SignRecord_data_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWS_DCMD_Update_Signal_Status_Local_struct
{
	int sendentity;
	BSTR creattime;
	BSTR dcmdnum;
	int status;
	DCMD_Setting current_Setting;
	Result Update_Signal_Status_LocalResult;
};

extern __declspec(selectany) const _soapmapentry __CWS_DCMD_Update_Signal_Status_Local_entries[] =
{

	{
		0x4F81F447, 
		"sendentity", 
		L"sendentity", 
		sizeof("sendentity")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_DCMD_Update_Signal_Status_Local_struct, sendentity),
		NULL,
		NULL,
		-1,
	},
	{
		0x8AE3603E, 
		"creattime", 
		L"creattime", 
		sizeof("creattime")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_DCMD_Update_Signal_Status_Local_struct, creattime),
		NULL,
		NULL,
		-1,
	},
	{
		0x009B99E8, 
		"dcmdnum", 
		L"dcmdnum", 
		sizeof("dcmdnum")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWS_DCMD_Update_Signal_Status_Local_struct, dcmdnum),
		NULL,
		NULL,
		-1,
	},
	{
		0x14AB8884, 
		"status", 
		L"status", 
		sizeof("status")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_DCMD_Update_Signal_Status_Local_struct, status),
		NULL,
		NULL,
		-1,
	},
	{
		0x15C9E960, 
		"current_Setting", 
		L"current_Setting", 
		sizeof("current_Setting")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_DCMD_Update_Signal_Status_Local_struct, current_Setting),
		NULL,
		&__DCMD_Setting_map,
		-1,
	},
	{
		0x1750FB4C, 
		"Update_Signal_Status_LocalResult", 
		L"Update_Signal_Status_LocalResult", 
		sizeof("Update_Signal_Status_LocalResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_DCMD_Update_Signal_Status_Local_struct, Update_Signal_Status_LocalResult),
		NULL,
		&__Result_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWS_DCMD_Update_Signal_Status_Local_map =
{
	0x2F241E9C,
	"Update_Signal_Status_Local",
	L"Update_Signal_Status_LocalResponse",
	sizeof("Update_Signal_Status_Local")-1,
	sizeof("Update_Signal_Status_LocalResponse")-1,
	SOAPMAP_FUNC,
	__CWS_DCMD_Update_Signal_Status_Local_entries,
	sizeof(__CWS_DCMD_Update_Signal_Status_Local_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWS_DCMD_Get_Dcmd_Number_Setting_All_struct
{
	BSTR Get_Dcmd_Number_Setting_AllResult;
};

extern __declspec(selectany) const _soapmapentry __CWS_DCMD_Get_Dcmd_Number_Setting_All_entries[] =
{

	{
		0x0F300A53, 
		"Get_Dcmd_Number_Setting_AllResult", 
		L"Get_Dcmd_Number_Setting_AllResult", 
		sizeof("Get_Dcmd_Number_Setting_AllResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWS_DCMD_Get_Dcmd_Number_Setting_All_struct, Get_Dcmd_Number_Setting_AllResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWS_DCMD_Get_Dcmd_Number_Setting_All_map =
{
	0x9B030B63,
	"Get_Dcmd_Number_Setting_All",
	L"Get_Dcmd_Number_Setting_AllResponse",
	sizeof("Get_Dcmd_Number_Setting_All")-1,
	sizeof("Get_Dcmd_Number_Setting_AllResponse")-1,
	SOAPMAP_FUNC,
	__CWS_DCMD_Get_Dcmd_Number_Setting_All_entries,
	sizeof(__CWS_DCMD_Get_Dcmd_Number_Setting_All_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWS_DCMD_Update_Dcmd_and_DcmdNum_struct
{
	BSTR ds;
	BSTR str_number;
	int theSender_ID;
	int cmd_Oper;
	Result Update_Dcmd_and_DcmdNumResult;
};

extern __declspec(selectany) const _soapmapentry __CWS_DCMD_Update_Dcmd_and_DcmdNum_entries[] =
{

	{
		0x00000D57, 
		"ds", 
		L"ds", 
		sizeof("ds")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWS_DCMD_Update_Dcmd_and_DcmdNum_struct, ds),
		NULL,
		NULL,
		-1,
	},
	{
		0x6869D9E1, 
		"str_number", 
		L"str_number", 
		sizeof("str_number")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWS_DCMD_Update_Dcmd_and_DcmdNum_struct, str_number),
		NULL,
		NULL,
		-1,
	},
	{
		0xF29A9A2E, 
		"theSender_ID", 
		L"theSender_ID", 
		sizeof("theSender_ID")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_DCMD_Update_Dcmd_and_DcmdNum_struct, theSender_ID),
		NULL,
		NULL,
		-1,
	},
	{
		0x142963C9, 
		"cmd_Oper", 
		L"cmd_Oper", 
		sizeof("cmd_Oper")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_DCMD_Update_Dcmd_and_DcmdNum_struct, cmd_Oper),
		NULL,
		NULL,
		-1,
	},
	{
		0xD34C5C92, 
		"Update_Dcmd_and_DcmdNumResult", 
		L"Update_Dcmd_and_DcmdNumResult", 
		sizeof("Update_Dcmd_and_DcmdNumResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_DCMD_Update_Dcmd_and_DcmdNum_struct, Update_Dcmd_and_DcmdNumResult),
		NULL,
		&__Result_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWS_DCMD_Update_Dcmd_and_DcmdNum_map =
{
	0xD77CE962,
	"Update_Dcmd_and_DcmdNum",
	L"Update_Dcmd_and_DcmdNumResponse",
	sizeof("Update_Dcmd_and_DcmdNum")-1,
	sizeof("Update_Dcmd_and_DcmdNumResponse")-1,
	SOAPMAP_FUNC,
	__CWS_DCMD_Update_Dcmd_and_DcmdNum_entries,
	sizeof(__CWS_DCMD_Update_Dcmd_and_DcmdNum_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWS_DCMD_Update_Dcmd_struct
{
	BSTR ds;
	Result Update_DcmdResult;
};

extern __declspec(selectany) const _soapmapentry __CWS_DCMD_Update_Dcmd_entries[] =
{

	{
		0x00000D57, 
		"ds", 
		L"ds", 
		sizeof("ds")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWS_DCMD_Update_Dcmd_struct, ds),
		NULL,
		NULL,
		-1,
	},
	{
		0xA8B21879, 
		"Update_DcmdResult", 
		L"Update_DcmdResult", 
		sizeof("Update_DcmdResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_DCMD_Update_Dcmd_struct, Update_DcmdResult),
		NULL,
		&__Result_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWS_DCMD_Update_Dcmd_map =
{
	0x9D413B09,
	"Update_Dcmd",
	L"Update_DcmdResponse",
	sizeof("Update_Dcmd")-1,
	sizeof("Update_DcmdResponse")-1,
	SOAPMAP_FUNC,
	__CWS_DCMD_Update_Dcmd_entries,
	sizeof(__CWS_DCMD_Update_Dcmd_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWS_DCMD_Update_Recv_Status_struct
{
	Recv_Dcmd_Xml_Info recv_info;
	int senderid;
	bool is_dutychief;
	Result Update_Recv_StatusResult;
};

extern __declspec(selectany) const _soapmapentry __CWS_DCMD_Update_Recv_Status_entries[] =
{

	{
		0xF45C153B, 
		"recv_info", 
		L"recv_info", 
		sizeof("recv_info")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_DCMD_Update_Recv_Status_struct, recv_info),
		NULL,
		&__Recv_Dcmd_Xml_Info_map,
		-1,
	},
	{
		0x882EF5EE, 
		"senderid", 
		L"senderid", 
		sizeof("senderid")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_DCMD_Update_Recv_Status_struct, senderid),
		NULL,
		NULL,
		-1,
	},
	{
		0xF9F30C60, 
		"is_dutychief", 
		L"is_dutychief", 
		sizeof("is_dutychief")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_DCMD_Update_Recv_Status_struct, is_dutychief),
		NULL,
		NULL,
		-1,
	},
	{
		0x240DD114, 
		"Update_Recv_StatusResult", 
		L"Update_Recv_StatusResult", 
		sizeof("Update_Recv_StatusResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_DCMD_Update_Recv_Status_struct, Update_Recv_StatusResult),
		NULL,
		&__Result_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWS_DCMD_Update_Recv_Status_map =
{
	0x5E6D8664,
	"Update_Recv_Status",
	L"Update_Recv_StatusResponse",
	sizeof("Update_Recv_Status")-1,
	sizeof("Update_Recv_StatusResponse")-1,
	SOAPMAP_FUNC,
	__CWS_DCMD_Update_Recv_Status_entries,
	sizeof(__CWS_DCMD_Update_Recv_Status_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWS_DCMD_Delete_Dcmd_By_Dcmdid_struct
{
	BSTR dcmd_id;
	Result Delete_Dcmd_By_DcmdidResult;
};

extern __declspec(selectany) const _soapmapentry __CWS_DCMD_Delete_Dcmd_By_Dcmdid_entries[] =
{

	{
		0x009B5884, 
		"dcmd_id", 
		L"dcmd_id", 
		sizeof("dcmd_id")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWS_DCMD_Delete_Dcmd_By_Dcmdid_struct, dcmd_id),
		NULL,
		NULL,
		-1,
	},
	{
		0x08808607, 
		"Delete_Dcmd_By_DcmdidResult", 
		L"Delete_Dcmd_By_DcmdidResult", 
		sizeof("Delete_Dcmd_By_DcmdidResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_DCMD_Delete_Dcmd_By_Dcmdid_struct, Delete_Dcmd_By_DcmdidResult),
		NULL,
		&__Result_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWS_DCMD_Delete_Dcmd_By_Dcmdid_map =
{
	0x2A614417,
	"Delete_Dcmd_By_Dcmdid",
	L"Delete_Dcmd_By_DcmdidResponse",
	sizeof("Delete_Dcmd_By_Dcmdid")-1,
	sizeof("Delete_Dcmd_By_DcmdidResponse")-1,
	SOAPMAP_FUNC,
	__CWS_DCMD_Delete_Dcmd_By_Dcmdid_entries,
	sizeof(__CWS_DCMD_Delete_Dcmd_By_Dcmdid_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWS_DCMD_Is_All_Signaled_struct
{
	BSTR ds;
	bool Is_All_SignaledResult;
};

extern __declspec(selectany) const _soapmapentry __CWS_DCMD_Is_All_Signaled_entries[] =
{

	{
		0x00000D57, 
		"ds", 
		L"ds", 
		sizeof("ds")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWS_DCMD_Is_All_Signaled_struct, ds),
		NULL,
		NULL,
		-1,
	},
	{
		0x17441819, 
		"Is_All_SignaledResult", 
		L"Is_All_SignaledResult", 
		sizeof("Is_All_SignaledResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_DCMD_Is_All_Signaled_struct, Is_All_SignaledResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWS_DCMD_Is_All_Signaled_map =
{
	0xF851A2A9,
	"Is_All_Signaled",
	L"Is_All_SignaledResponse",
	sizeof("Is_All_Signaled")-1,
	sizeof("Is_All_SignaledResponse")-1,
	SOAPMAP_FUNC,
	__CWS_DCMD_Is_All_Signaled_entries,
	sizeof(__CWS_DCMD_Is_All_Signaled_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWS_DCMD_Is_DB_Connected_struct
{
	Result Is_DB_ConnectedResult;
};

extern __declspec(selectany) const _soapmapentry __CWS_DCMD_Is_DB_Connected_entries[] =
{

	{
		0xA983D292, 
		"Is_DB_ConnectedResult", 
		L"Is_DB_ConnectedResult", 
		sizeof("Is_DB_ConnectedResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_DCMD_Is_DB_Connected_struct, Is_DB_ConnectedResult),
		NULL,
		&__Result_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWS_DCMD_Is_DB_Connected_map =
{
	0x1969DF62,
	"Is_DB_Connected",
	L"Is_DB_ConnectedResponse",
	sizeof("Is_DB_Connected")-1,
	sizeof("Is_DB_ConnectedResponse")-1,
	SOAPMAP_FUNC,
	__CWS_DCMD_Is_DB_Connected_entries,
	sizeof(__CWS_DCMD_Is_DB_Connected_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWS_DCMD_Update_SignRecord_struct
{
	BSTR ds;
	Result Update_SignRecordResult;
};

extern __declspec(selectany) const _soapmapentry __CWS_DCMD_Update_SignRecord_entries[] =
{

	{
		0x00000D57, 
		"ds", 
		L"ds", 
		sizeof("ds")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWS_DCMD_Update_SignRecord_struct, ds),
		NULL,
		NULL,
		-1,
	},
	{
		0x8428D671, 
		"Update_SignRecordResult", 
		L"Update_SignRecordResult", 
		sizeof("Update_SignRecordResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_DCMD_Update_SignRecord_struct, Update_SignRecordResult),
		NULL,
		&__Result_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWS_DCMD_Update_SignRecord_map =
{
	0x315F5701,
	"Update_SignRecord",
	L"Update_SignRecordResponse",
	sizeof("Update_SignRecord")-1,
	sizeof("Update_SignRecordResponse")-1,
	SOAPMAP_FUNC,
	__CWS_DCMD_Update_SignRecord_entries,
	sizeof(__CWS_DCMD_Update_SignRecord_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWS_DCMD_Is_recv_info_HasSaved_struct
{
	int sendentity;
	BSTR creattime;
	BSTR dcmdnum;
	int senderid;
	bool Is_recv_info_HasSavedResult;
};

extern __declspec(selectany) const _soapmapentry __CWS_DCMD_Is_recv_info_HasSaved_entries[] =
{

	{
		0x4F81F447, 
		"sendentity", 
		L"sendentity", 
		sizeof("sendentity")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_DCMD_Is_recv_info_HasSaved_struct, sendentity),
		NULL,
		NULL,
		-1,
	},
	{
		0x8AE3603E, 
		"creattime", 
		L"creattime", 
		sizeof("creattime")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_DCMD_Is_recv_info_HasSaved_struct, creattime),
		NULL,
		NULL,
		-1,
	},
	{
		0x009B99E8, 
		"dcmdnum", 
		L"dcmdnum", 
		sizeof("dcmdnum")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWS_DCMD_Is_recv_info_HasSaved_struct, dcmdnum),
		NULL,
		NULL,
		-1,
	},
	{
		0x882EF5EE, 
		"senderid", 
		L"senderid", 
		sizeof("senderid")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_DCMD_Is_recv_info_HasSaved_struct, senderid),
		NULL,
		NULL,
		-1,
	},
	{
		0xAB13E143, 
		"Is_recv_info_HasSavedResult", 
		L"Is_recv_info_HasSavedResult", 
		sizeof("Is_recv_info_HasSavedResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_DCMD_Is_recv_info_HasSaved_struct, Is_recv_info_HasSavedResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWS_DCMD_Is_recv_info_HasSaved_map =
{
	0xBF385E53,
	"Is_recv_info_HasSaved",
	L"Is_recv_info_HasSavedResponse",
	sizeof("Is_recv_info_HasSaved")-1,
	sizeof("Is_recv_info_HasSavedResponse")-1,
	SOAPMAP_FUNC,
	__CWS_DCMD_Is_recv_info_HasSaved_entries,
	sizeof(__CWS_DCMD_Is_recv_info_HasSaved_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWS_DCMD_Get_DCMD_ID_By_recv_info2_struct
{
	int sendentity;
	BSTR creattime;
	BSTR dcmdnum;
	int senderid;
	Result Get_DCMD_ID_By_recv_info2Result;
};

extern __declspec(selectany) const _soapmapentry __CWS_DCMD_Get_DCMD_ID_By_recv_info2_entries[] =
{

	{
		0x4F81F447, 
		"sendentity", 
		L"sendentity", 
		sizeof("sendentity")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_DCMD_Get_DCMD_ID_By_recv_info2_struct, sendentity),
		NULL,
		NULL,
		-1,
	},
	{
		0x8AE3603E, 
		"creattime", 
		L"creattime", 
		sizeof("creattime")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_DCMD_Get_DCMD_ID_By_recv_info2_struct, creattime),
		NULL,
		NULL,
		-1,
	},
	{
		0x009B99E8, 
		"dcmdnum", 
		L"dcmdnum", 
		sizeof("dcmdnum")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWS_DCMD_Get_DCMD_ID_By_recv_info2_struct, dcmdnum),
		NULL,
		NULL,
		-1,
	},
	{
		0x882EF5EE, 
		"senderid", 
		L"senderid", 
		sizeof("senderid")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_DCMD_Get_DCMD_ID_By_recv_info2_struct, senderid),
		NULL,
		NULL,
		-1,
	},
	{
		0xA2E249A8, 
		"Get_DCMD_ID_By_recv_info2Result", 
		L"Get_DCMD_ID_By_recv_info2Result", 
		sizeof("Get_DCMD_ID_By_recv_info2Result")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_DCMD_Get_DCMD_ID_By_recv_info2_struct, Get_DCMD_ID_By_recv_info2Result),
		NULL,
		&__Result_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWS_DCMD_Get_DCMD_ID_By_recv_info2_map =
{
	0xE44273F8,
	"Get_DCMD_ID_By_recv_info2",
	L"Get_DCMD_ID_By_recv_info2Response",
	sizeof("Get_DCMD_ID_By_recv_info2")-1,
	sizeof("Get_DCMD_ID_By_recv_info2Response")-1,
	SOAPMAP_FUNC,
	__CWS_DCMD_Get_DCMD_ID_By_recv_info2_entries,
	sizeof(__CWS_DCMD_Get_DCMD_ID_By_recv_info2_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWS_DCMD_Get_DCMD_ID_By_recv_info_struct
{
	Recv_Dcmd_Xml_Info recv_info;
	Result Get_DCMD_ID_By_recv_infoResult;
};

extern __declspec(selectany) const _soapmapentry __CWS_DCMD_Get_DCMD_ID_By_recv_info_entries[] =
{

	{
		0xF45C153B, 
		"recv_info", 
		L"recv_info", 
		sizeof("recv_info")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_DCMD_Get_DCMD_ID_By_recv_info_struct, recv_info),
		NULL,
		&__Recv_Dcmd_Xml_Info_map,
		-1,
	},
	{
		0x22688D16, 
		"Get_DCMD_ID_By_recv_infoResult", 
		L"Get_DCMD_ID_By_recv_infoResult", 
		sizeof("Get_DCMD_ID_By_recv_infoResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_DCMD_Get_DCMD_ID_By_recv_info_struct, Get_DCMD_ID_By_recv_infoResult),
		NULL,
		&__Result_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWS_DCMD_Get_DCMD_ID_By_recv_info_map =
{
	0x5E674AE6,
	"Get_DCMD_ID_By_recv_info",
	L"Get_DCMD_ID_By_recv_infoResponse",
	sizeof("Get_DCMD_ID_By_recv_info")-1,
	sizeof("Get_DCMD_ID_By_recv_infoResponse")-1,
	SOAPMAP_FUNC,
	__CWS_DCMD_Get_DCMD_ID_By_recv_info_entries,
	sizeof(__CWS_DCMD_Get_DCMD_ID_By_recv_info_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWS_DCMD_IS_DcmdNumber_Validated_struct
{
	BSTR theNumber;
	bool IS_DcmdNumber_ValidatedResult;
};

extern __declspec(selectany) const _soapmapentry __CWS_DCMD_IS_DcmdNumber_Validated_entries[] =
{

	{
		0xE050AFAA, 
		"theNumber", 
		L"theNumber", 
		sizeof("theNumber")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWS_DCMD_IS_DcmdNumber_Validated_struct, theNumber),
		NULL,
		NULL,
		-1,
	},
	{
		0xCCE2A688, 
		"IS_DcmdNumber_ValidatedResult", 
		L"IS_DcmdNumber_ValidatedResult", 
		sizeof("IS_DcmdNumber_ValidatedResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_DCMD_IS_DcmdNumber_Validated_struct, IS_DcmdNumber_ValidatedResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWS_DCMD_IS_DcmdNumber_Validated_map =
{
	0x8FCD88D8,
	"IS_DcmdNumber_Validated",
	L"IS_DcmdNumber_ValidatedResponse",
	sizeof("IS_DcmdNumber_Validated")-1,
	sizeof("IS_DcmdNumber_ValidatedResponse")-1,
	SOAPMAP_FUNC,
	__CWS_DCMD_IS_DcmdNumber_Validated_entries,
	sizeof(__CWS_DCMD_IS_DcmdNumber_Validated_struct),
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


struct __CWS_DCMD_Insert_Dcmd_main_struct
{
	BSTR xml;
	Result Insert_Dcmd_mainResult;
};

extern __declspec(selectany) const _soapmapentry __CWS_DCMD_Insert_Dcmd_main_entries[] =
{

	{
		0x00020CF1, 
		"xml", 
		L"xml", 
		sizeof("xml")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWS_DCMD_Insert_Dcmd_main_struct, xml),
		NULL,
		NULL,
		-1,
	},
	{
		0x685EC16F, 
		"Insert_Dcmd_mainResult", 
		L"Insert_Dcmd_mainResult", 
		sizeof("Insert_Dcmd_mainResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_DCMD_Insert_Dcmd_main_struct, Insert_Dcmd_mainResult),
		NULL,
		&__Result_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWS_DCMD_Insert_Dcmd_main_map =
{
	0xFABBF97F,
	"Insert_Dcmd_main",
	L"Insert_Dcmd_mainResponse",
	sizeof("Insert_Dcmd_main")-1,
	sizeof("Insert_Dcmd_mainResponse")-1,
	SOAPMAP_FUNC,
	__CWS_DCMD_Insert_Dcmd_main_entries,
	sizeof(__CWS_DCMD_Insert_Dcmd_main_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWS_DCMD_Update_Dcmd_Template_struct
{
	BSTR ds;
	Result Update_Dcmd_TemplateResult;
};

extern __declspec(selectany) const _soapmapentry __CWS_DCMD_Update_Dcmd_Template_entries[] =
{

	{
		0x00000D57, 
		"ds", 
		L"ds", 
		sizeof("ds")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWS_DCMD_Update_Dcmd_Template_struct, ds),
		NULL,
		NULL,
		-1,
	},
	{
		0x3F9A4934, 
		"Update_Dcmd_TemplateResult", 
		L"Update_Dcmd_TemplateResult", 
		sizeof("Update_Dcmd_TemplateResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_DCMD_Update_Dcmd_Template_struct, Update_Dcmd_TemplateResult),
		NULL,
		&__Result_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWS_DCMD_Update_Dcmd_Template_map =
{
	0x8EF88684,
	"Update_Dcmd_Template",
	L"Update_Dcmd_TemplateResponse",
	sizeof("Update_Dcmd_Template")-1,
	sizeof("Update_Dcmd_TemplateResponse")-1,
	SOAPMAP_FUNC,
	__CWS_DCMD_Update_Dcmd_Template_entries,
	sizeof(__CWS_DCMD_Update_Dcmd_Template_struct),
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


struct __CWS_DCMD_Reset_DcmdNumber_By_DCMD_DATE_struct
{
	int Sender_id;
	BSTR current_dcmd_date;
	Result Reset_DcmdNumber_By_DCMD_DATEResult;
};

extern __declspec(selectany) const _soapmapentry __CWS_DCMD_Reset_DcmdNumber_By_DCMD_DATE_entries[] =
{

	{
		0xFE7F6A0D, 
		"Sender_id", 
		L"Sender_id", 
		sizeof("Sender_id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_DCMD_Reset_DcmdNumber_By_DCMD_DATE_struct, Sender_id),
		NULL,
		NULL,
		-1,
	},
	{
		0x52C4EA57, 
		"current_dcmd_date", 
		L"current_dcmd_date", 
		sizeof("current_dcmd_date")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_DCMD_Reset_DcmdNumber_By_DCMD_DATE_struct, current_dcmd_date),
		NULL,
		NULL,
		-1,
	},
	{
		0x9F158650, 
		"Reset_DcmdNumber_By_DCMD_DATEResult", 
		L"Reset_DcmdNumber_By_DCMD_DATEResult", 
		sizeof("Reset_DcmdNumber_By_DCMD_DATEResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_DCMD_Reset_DcmdNumber_By_DCMD_DATE_struct, Reset_DcmdNumber_By_DCMD_DATEResult),
		NULL,
		&__Result_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWS_DCMD_Reset_DcmdNumber_By_DCMD_DATE_map =
{
	0xBA377AA0,
	"Reset_DcmdNumber_By_DCMD_DATE",
	L"Reset_DcmdNumber_By_DCMD_DATEResponse",
	sizeof("Reset_DcmdNumber_By_DCMD_DATE")-1,
	sizeof("Reset_DcmdNumber_By_DCMD_DATEResponse")-1,
	SOAPMAP_FUNC,
	__CWS_DCMD_Reset_DcmdNumber_By_DCMD_DATE_entries,
	sizeof(__CWS_DCMD_Reset_DcmdNumber_By_DCMD_DATE_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWS_DCMD_Get_Next_DCMD_Number_struct
{
	int Sender_id;
	int command_Oper;
	Result Get_Next_DCMD_NumberResult;
};

extern __declspec(selectany) const _soapmapentry __CWS_DCMD_Get_Next_DCMD_Number_entries[] =
{

	{
		0xFE7F6A0D, 
		"Sender_id", 
		L"Sender_id", 
		sizeof("Sender_id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_DCMD_Get_Next_DCMD_Number_struct, Sender_id),
		NULL,
		NULL,
		-1,
	},
	{
		0x9B6B92F4, 
		"command_Oper", 
		L"command_Oper", 
		sizeof("command_Oper")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_DCMD_Get_Next_DCMD_Number_struct, command_Oper),
		NULL,
		NULL,
		-1,
	},
	{
		0x13656F9C, 
		"Get_Next_DCMD_NumberResult", 
		L"Get_Next_DCMD_NumberResult", 
		sizeof("Get_Next_DCMD_NumberResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWS_DCMD_Get_Next_DCMD_Number_struct, Get_Next_DCMD_NumberResult),
		NULL,
		&__Result_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWS_DCMD_Get_Next_DCMD_Number_map =
{
	0x8226E6EC,
	"Get_Next_DCMD_Number",
	L"Get_Next_DCMD_NumberResponse",
	sizeof("Get_Next_DCMD_Number")-1,
	sizeof("Get_Next_DCMD_NumberResponse")-1,
	SOAPMAP_FUNC,
	__CWS_DCMD_Get_Next_DCMD_Number_entries,
	sizeof(__CWS_DCMD_Get_Next_DCMD_Number_struct),
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
	&__CWS_DCMD_Reset_DcmdNumber_map,
	&__CWS_DCMD_Update_Dcmd_Number_Setting_map,
	&__CWS_DCMD_Get_dcmd_template_all_map,
	&__CWS_DCMD_Get_dcmd_By_dcmdid_map,
	&__CWS_DCMD_Get_SignRecord_By_DCMDID_map,
	&__CWS_DCMD_Get_SignRecord_data_map,
	&__CWS_DCMD_Update_Signal_Status_Local_map,
	&__CWS_DCMD_Get_Dcmd_Number_Setting_All_map,
	&__CWS_DCMD_Update_Dcmd_and_DcmdNum_map,
	&__CWS_DCMD_Update_Dcmd_map,
	&__CWS_DCMD_Update_Recv_Status_map,
	&__CWS_DCMD_Delete_Dcmd_By_Dcmdid_map,
	&__CWS_DCMD_Is_All_Signaled_map,
	&__CWS_DCMD_Is_DB_Connected_map,
	&__CWS_DCMD_Update_SignRecord_map,
	&__CWS_DCMD_Is_recv_info_HasSaved_map,
	&__CWS_DCMD_Get_DCMD_ID_By_recv_info2_map,
	&__CWS_DCMD_Get_DCMD_ID_By_recv_info_map,
	&__CWS_DCMD_IS_DcmdNumber_Validated_map,
	&__CWS_DCMD_Get_dcmd_index_by_Condition_map,
	&__CWS_DCMD_Insert_Dcmd_main_map,
	&__CWS_DCMD_Update_Dcmd_Template_map,
	&__CWS_DCMD_Hello_map,
	&__CWS_DCMD_Reset_DcmdNumber_By_DCMD_DATE_map,
	&__CWS_DCMD_Get_Next_DCMD_Number_map,
	NULL
};

template <typename TClient>
inline HRESULT CWS_DCMDT<TClient>::Reset_DcmdNumber(
		int Sender_id, 
		int cmd_Oper, 
		Result* Reset_DcmdNumberResult
	)
{
    if ( Reset_DcmdNumberResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWS_DCMD_Reset_DcmdNumber_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Sender_id = Sender_id;
	__params.cmd_Oper = cmd_Oper;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/Reset_DcmdNumber\"\r\n"));
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

	*Reset_DcmdNumberResult = __params.Reset_DcmdNumberResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWS_DCMDT<TClient>::Update_Dcmd_Number_Setting(
		BSTR ds, 
		Result* Update_Dcmd_Number_SettingResult
	)
{
    if ( Update_Dcmd_Number_SettingResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWS_DCMD_Update_Dcmd_Number_Setting_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.ds = ds;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/Update_Dcmd_Number_Setting\"\r\n"));
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

	*Update_Dcmd_Number_SettingResult = __params.Update_Dcmd_Number_SettingResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWS_DCMDT<TClient>::Get_dcmd_template_all(
		BSTR* Get_dcmd_template_allResult
	)
{
    if ( Get_dcmd_template_allResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWS_DCMD_Get_dcmd_template_all_struct __params;
	memset(&__params, 0x00, sizeof(__params));

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/Get_dcmd_template_all\"\r\n"));
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

	*Get_dcmd_template_allResult = __params.Get_dcmd_template_allResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWS_DCMDT<TClient>::Get_dcmd_By_dcmdid(
		BSTR dcmd_id, 
		BSTR* Get_dcmd_By_dcmdidResult
	)
{
    if ( Get_dcmd_By_dcmdidResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWS_DCMD_Get_dcmd_By_dcmdid_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.dcmd_id = dcmd_id;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/Get_dcmd_By_dcmdid\"\r\n"));
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

	*Get_dcmd_By_dcmdidResult = __params.Get_dcmd_By_dcmdidResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWS_DCMDT<TClient>::Get_SignRecord_By_DCMDID(
		BSTR dcmdid, 
		BSTR* Get_SignRecord_By_DCMDIDResult
	)
{
    if ( Get_SignRecord_By_DCMDIDResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWS_DCMD_Get_SignRecord_By_DCMDID_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.dcmdid = dcmdid;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/Get_SignRecord_By_DCMDID\"\r\n"));
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

	*Get_SignRecord_By_DCMDIDResult = __params.Get_SignRecord_By_DCMDIDResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWS_DCMDT<TClient>::Get_SignRecord_data(
		BSTR* Get_SignRecord_dataResult
	)
{
    if ( Get_SignRecord_dataResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWS_DCMD_Get_SignRecord_data_struct __params;
	memset(&__params, 0x00, sizeof(__params));

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/Get_SignRecord_data\"\r\n"));
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

	*Get_SignRecord_dataResult = __params.Get_SignRecord_dataResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWS_DCMDT<TClient>::Update_Signal_Status_Local(
		int sendentity, 
		BSTR creattime, 
		BSTR dcmdnum, 
		int status, 
		DCMD_Setting current_Setting, 
		Result* Update_Signal_Status_LocalResult
	)
{
    if ( Update_Signal_Status_LocalResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWS_DCMD_Update_Signal_Status_Local_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sendentity = sendentity;
	__params.creattime = creattime;
	__params.dcmdnum = dcmdnum;
	__params.status = status;
	__params.current_Setting = current_Setting;

	__atlsoap_hr = SetClientStruct(&__params, 6);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/Update_Signal_Status_Local\"\r\n"));
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

	*Update_Signal_Status_LocalResult = __params.Update_Signal_Status_LocalResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWS_DCMDT<TClient>::Get_Dcmd_Number_Setting_All(
		BSTR* Get_Dcmd_Number_Setting_AllResult
	)
{
    if ( Get_Dcmd_Number_Setting_AllResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWS_DCMD_Get_Dcmd_Number_Setting_All_struct __params;
	memset(&__params, 0x00, sizeof(__params));

	__atlsoap_hr = SetClientStruct(&__params, 7);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/Get_Dcmd_Number_Setting_All\"\r\n"));
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

	*Get_Dcmd_Number_Setting_AllResult = __params.Get_Dcmd_Number_Setting_AllResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWS_DCMDT<TClient>::Update_Dcmd_and_DcmdNum(
		BSTR ds, 
		BSTR str_number, 
		int theSender_ID, 
		int cmd_Oper, 
		Result* Update_Dcmd_and_DcmdNumResult
	)
{
    if ( Update_Dcmd_and_DcmdNumResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWS_DCMD_Update_Dcmd_and_DcmdNum_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.ds = ds;
	__params.str_number = str_number;
	__params.theSender_ID = theSender_ID;
	__params.cmd_Oper = cmd_Oper;

	__atlsoap_hr = SetClientStruct(&__params, 8);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/Update_Dcmd_and_DcmdNum\"\r\n"));
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

	*Update_Dcmd_and_DcmdNumResult = __params.Update_Dcmd_and_DcmdNumResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWS_DCMDT<TClient>::Update_Dcmd(
		BSTR ds, 
		Result* Update_DcmdResult
	)
{
    if ( Update_DcmdResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWS_DCMD_Update_Dcmd_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.ds = ds;

	__atlsoap_hr = SetClientStruct(&__params, 9);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/Update_Dcmd\"\r\n"));
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

	*Update_DcmdResult = __params.Update_DcmdResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWS_DCMDT<TClient>::Update_Recv_Status(
		Recv_Dcmd_Xml_Info recv_info, 
		int senderid, 
		bool is_dutychief, 
		Result* Update_Recv_StatusResult
	)
{
    if ( Update_Recv_StatusResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWS_DCMD_Update_Recv_Status_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.recv_info = recv_info;
	__params.senderid = senderid;
	__params.is_dutychief = is_dutychief;

	__atlsoap_hr = SetClientStruct(&__params, 10);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/Update_Recv_Status\"\r\n"));
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

	*Update_Recv_StatusResult = __params.Update_Recv_StatusResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWS_DCMDT<TClient>::Delete_Dcmd_By_Dcmdid(
		BSTR dcmd_id, 
		Result* Delete_Dcmd_By_DcmdidResult
	)
{
    if ( Delete_Dcmd_By_DcmdidResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWS_DCMD_Delete_Dcmd_By_Dcmdid_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.dcmd_id = dcmd_id;

	__atlsoap_hr = SetClientStruct(&__params, 11);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/Delete_Dcmd_By_Dcmdid\"\r\n"));
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

	*Delete_Dcmd_By_DcmdidResult = __params.Delete_Dcmd_By_DcmdidResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWS_DCMDT<TClient>::Is_All_Signaled(
		BSTR ds, 
		bool* Is_All_SignaledResult
	)
{
    if ( Is_All_SignaledResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWS_DCMD_Is_All_Signaled_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.ds = ds;

	__atlsoap_hr = SetClientStruct(&__params, 12);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/Is_All_Signaled\"\r\n"));
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

	*Is_All_SignaledResult = __params.Is_All_SignaledResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWS_DCMDT<TClient>::Is_DB_Connected(
		Result* Is_DB_ConnectedResult
	)
{
    if ( Is_DB_ConnectedResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWS_DCMD_Is_DB_Connected_struct __params;
	memset(&__params, 0x00, sizeof(__params));

	__atlsoap_hr = SetClientStruct(&__params, 13);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/Is_DB_Connected\"\r\n"));
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

	*Is_DB_ConnectedResult = __params.Is_DB_ConnectedResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWS_DCMDT<TClient>::Update_SignRecord(
		BSTR ds, 
		Result* Update_SignRecordResult
	)
{
    if ( Update_SignRecordResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWS_DCMD_Update_SignRecord_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.ds = ds;

	__atlsoap_hr = SetClientStruct(&__params, 14);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/Update_SignRecord\"\r\n"));
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

	*Update_SignRecordResult = __params.Update_SignRecordResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWS_DCMDT<TClient>::Is_recv_info_HasSaved(
		int sendentity, 
		BSTR creattime, 
		BSTR dcmdnum, 
		int senderid, 
		bool* Is_recv_info_HasSavedResult
	)
{
    if ( Is_recv_info_HasSavedResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWS_DCMD_Is_recv_info_HasSaved_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sendentity = sendentity;
	__params.creattime = creattime;
	__params.dcmdnum = dcmdnum;
	__params.senderid = senderid;

	__atlsoap_hr = SetClientStruct(&__params, 15);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/Is_recv_info_HasSaved\"\r\n"));
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

	*Is_recv_info_HasSavedResult = __params.Is_recv_info_HasSavedResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWS_DCMDT<TClient>::Get_DCMD_ID_By_recv_info2(
		int sendentity, 
		BSTR creattime, 
		BSTR dcmdnum, 
		int senderid, 
		Result* Get_DCMD_ID_By_recv_info2Result
	)
{
    if ( Get_DCMD_ID_By_recv_info2Result == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWS_DCMD_Get_DCMD_ID_By_recv_info2_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sendentity = sendentity;
	__params.creattime = creattime;
	__params.dcmdnum = dcmdnum;
	__params.senderid = senderid;

	__atlsoap_hr = SetClientStruct(&__params, 16);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/Get_DCMD_ID_By_recv_info2\"\r\n"));
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

	*Get_DCMD_ID_By_recv_info2Result = __params.Get_DCMD_ID_By_recv_info2Result;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWS_DCMDT<TClient>::Get_DCMD_ID_By_recv_info(
		Recv_Dcmd_Xml_Info recv_info, 
		Result* Get_DCMD_ID_By_recv_infoResult
	)
{
    if ( Get_DCMD_ID_By_recv_infoResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWS_DCMD_Get_DCMD_ID_By_recv_info_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.recv_info = recv_info;

	__atlsoap_hr = SetClientStruct(&__params, 17);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/Get_DCMD_ID_By_recv_info\"\r\n"));
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

	*Get_DCMD_ID_By_recv_infoResult = __params.Get_DCMD_ID_By_recv_infoResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWS_DCMDT<TClient>::IS_DcmdNumber_Validated(
		BSTR theNumber, 
		bool* IS_DcmdNumber_ValidatedResult
	)
{
    if ( IS_DcmdNumber_ValidatedResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWS_DCMD_IS_DcmdNumber_Validated_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.theNumber = theNumber;

	__atlsoap_hr = SetClientStruct(&__params, 18);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/IS_DcmdNumber_Validated\"\r\n"));
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

	*IS_DcmdNumber_ValidatedResult = __params.IS_DcmdNumber_ValidatedResult;
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

	__atlsoap_hr = SetClientStruct(&__params, 19);
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
inline HRESULT CWS_DCMDT<TClient>::Insert_Dcmd_main(
		BSTR xml, 
		Result* Insert_Dcmd_mainResult
	)
{
    if ( Insert_Dcmd_mainResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWS_DCMD_Insert_Dcmd_main_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.xml = xml;

	__atlsoap_hr = SetClientStruct(&__params, 20);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/Insert_Dcmd_main\"\r\n"));
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

	*Insert_Dcmd_mainResult = __params.Insert_Dcmd_mainResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWS_DCMDT<TClient>::Update_Dcmd_Template(
		BSTR ds, 
		Result* Update_Dcmd_TemplateResult
	)
{
    if ( Update_Dcmd_TemplateResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWS_DCMD_Update_Dcmd_Template_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.ds = ds;

	__atlsoap_hr = SetClientStruct(&__params, 21);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/Update_Dcmd_Template\"\r\n"));
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

	*Update_Dcmd_TemplateResult = __params.Update_Dcmd_TemplateResult;
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

	__atlsoap_hr = SetClientStruct(&__params, 22);
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
inline HRESULT CWS_DCMDT<TClient>::Reset_DcmdNumber_By_DCMD_DATE(
		int Sender_id, 
		BSTR current_dcmd_date, 
		Result* Reset_DcmdNumber_By_DCMD_DATEResult
	)
{
    if ( Reset_DcmdNumber_By_DCMD_DATEResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWS_DCMD_Reset_DcmdNumber_By_DCMD_DATE_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Sender_id = Sender_id;
	__params.current_dcmd_date = current_dcmd_date;

	__atlsoap_hr = SetClientStruct(&__params, 23);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/Reset_DcmdNumber_By_DCMD_DATE\"\r\n"));
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

	*Reset_DcmdNumber_By_DCMD_DATEResult = __params.Reset_DcmdNumber_By_DCMD_DATEResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWS_DCMDT<TClient>::Get_Next_DCMD_Number(
		int Sender_id, 
		int command_Oper, 
		Result* Get_Next_DCMD_NumberResult
	)
{
    if ( Get_Next_DCMD_NumberResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWS_DCMD_Get_Next_DCMD_Number_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Sender_id = Sender_id;
	__params.command_Oper = command_Oper;

	__atlsoap_hr = SetClientStruct(&__params, 24);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/Get_Next_DCMD_Number\"\r\n"));
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

	*Get_Next_DCMD_NumberResult = __params.Get_Next_DCMD_NumberResult;
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
	static const _soapmapentry __CWS_DCMD_Reset_DcmdNumber_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWS_DCMD_Reset_DcmdNumber_atlsoapheader_map = 
	{
		0xB7DA01B2,
		"Reset_DcmdNumber",
		L"Reset_DcmdNumberResponse",
		sizeof("Reset_DcmdNumber")-1,
		sizeof("Reset_DcmdNumberResponse")-1,
		SOAPMAP_HEADER,
		__CWS_DCMD_Reset_DcmdNumber_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWS_DCMD_Update_Dcmd_Number_Setting_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWS_DCMD_Update_Dcmd_Number_Setting_atlsoapheader_map = 
	{
		0x1158C94E,
		"Update_Dcmd_Number_Setting",
		L"Update_Dcmd_Number_SettingResponse",
		sizeof("Update_Dcmd_Number_Setting")-1,
		sizeof("Update_Dcmd_Number_SettingResponse")-1,
		SOAPMAP_HEADER,
		__CWS_DCMD_Update_Dcmd_Number_Setting_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWS_DCMD_Get_dcmd_template_all_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWS_DCMD_Get_dcmd_template_all_atlsoapheader_map = 
	{
		0x56257E39,
		"Get_dcmd_template_all",
		L"Get_dcmd_template_allResponse",
		sizeof("Get_dcmd_template_all")-1,
		sizeof("Get_dcmd_template_allResponse")-1,
		SOAPMAP_HEADER,
		__CWS_DCMD_Get_dcmd_template_all_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWS_DCMD_Get_dcmd_By_dcmdid_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWS_DCMD_Get_dcmd_By_dcmdid_atlsoapheader_map = 
	{
		0xCE15C9A4,
		"Get_dcmd_By_dcmdid",
		L"Get_dcmd_By_dcmdidResponse",
		sizeof("Get_dcmd_By_dcmdid")-1,
		sizeof("Get_dcmd_By_dcmdidResponse")-1,
		SOAPMAP_HEADER,
		__CWS_DCMD_Get_dcmd_By_dcmdid_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWS_DCMD_Get_SignRecord_By_DCMDID_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWS_DCMD_Get_SignRecord_By_DCMDID_atlsoapheader_map = 
	{
		0x67770C7C,
		"Get_SignRecord_By_DCMDID",
		L"Get_SignRecord_By_DCMDIDResponse",
		sizeof("Get_SignRecord_By_DCMDID")-1,
		sizeof("Get_SignRecord_By_DCMDIDResponse")-1,
		SOAPMAP_HEADER,
		__CWS_DCMD_Get_SignRecord_By_DCMDID_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWS_DCMD_Get_SignRecord_data_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWS_DCMD_Get_SignRecord_data_atlsoapheader_map = 
	{
		0xC28F37B7,
		"Get_SignRecord_data",
		L"Get_SignRecord_dataResponse",
		sizeof("Get_SignRecord_data")-1,
		sizeof("Get_SignRecord_dataResponse")-1,
		SOAPMAP_HEADER,
		__CWS_DCMD_Get_SignRecord_data_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWS_DCMD_Update_Signal_Status_Local_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWS_DCMD_Update_Signal_Status_Local_atlsoapheader_map = 
	{
		0x2F241E9C,
		"Update_Signal_Status_Local",
		L"Update_Signal_Status_LocalResponse",
		sizeof("Update_Signal_Status_Local")-1,
		sizeof("Update_Signal_Status_LocalResponse")-1,
		SOAPMAP_HEADER,
		__CWS_DCMD_Update_Signal_Status_Local_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWS_DCMD_Get_Dcmd_Number_Setting_All_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWS_DCMD_Get_Dcmd_Number_Setting_All_atlsoapheader_map = 
	{
		0x9B030B63,
		"Get_Dcmd_Number_Setting_All",
		L"Get_Dcmd_Number_Setting_AllResponse",
		sizeof("Get_Dcmd_Number_Setting_All")-1,
		sizeof("Get_Dcmd_Number_Setting_AllResponse")-1,
		SOAPMAP_HEADER,
		__CWS_DCMD_Get_Dcmd_Number_Setting_All_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWS_DCMD_Update_Dcmd_and_DcmdNum_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWS_DCMD_Update_Dcmd_and_DcmdNum_atlsoapheader_map = 
	{
		0xD77CE962,
		"Update_Dcmd_and_DcmdNum",
		L"Update_Dcmd_and_DcmdNumResponse",
		sizeof("Update_Dcmd_and_DcmdNum")-1,
		sizeof("Update_Dcmd_and_DcmdNumResponse")-1,
		SOAPMAP_HEADER,
		__CWS_DCMD_Update_Dcmd_and_DcmdNum_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWS_DCMD_Update_Dcmd_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWS_DCMD_Update_Dcmd_atlsoapheader_map = 
	{
		0x9D413B09,
		"Update_Dcmd",
		L"Update_DcmdResponse",
		sizeof("Update_Dcmd")-1,
		sizeof("Update_DcmdResponse")-1,
		SOAPMAP_HEADER,
		__CWS_DCMD_Update_Dcmd_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWS_DCMD_Update_Recv_Status_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWS_DCMD_Update_Recv_Status_atlsoapheader_map = 
	{
		0x5E6D8664,
		"Update_Recv_Status",
		L"Update_Recv_StatusResponse",
		sizeof("Update_Recv_Status")-1,
		sizeof("Update_Recv_StatusResponse")-1,
		SOAPMAP_HEADER,
		__CWS_DCMD_Update_Recv_Status_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWS_DCMD_Delete_Dcmd_By_Dcmdid_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWS_DCMD_Delete_Dcmd_By_Dcmdid_atlsoapheader_map = 
	{
		0x2A614417,
		"Delete_Dcmd_By_Dcmdid",
		L"Delete_Dcmd_By_DcmdidResponse",
		sizeof("Delete_Dcmd_By_Dcmdid")-1,
		sizeof("Delete_Dcmd_By_DcmdidResponse")-1,
		SOAPMAP_HEADER,
		__CWS_DCMD_Delete_Dcmd_By_Dcmdid_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWS_DCMD_Is_All_Signaled_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWS_DCMD_Is_All_Signaled_atlsoapheader_map = 
	{
		0xF851A2A9,
		"Is_All_Signaled",
		L"Is_All_SignaledResponse",
		sizeof("Is_All_Signaled")-1,
		sizeof("Is_All_SignaledResponse")-1,
		SOAPMAP_HEADER,
		__CWS_DCMD_Is_All_Signaled_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWS_DCMD_Is_DB_Connected_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWS_DCMD_Is_DB_Connected_atlsoapheader_map = 
	{
		0x1969DF62,
		"Is_DB_Connected",
		L"Is_DB_ConnectedResponse",
		sizeof("Is_DB_Connected")-1,
		sizeof("Is_DB_ConnectedResponse")-1,
		SOAPMAP_HEADER,
		__CWS_DCMD_Is_DB_Connected_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWS_DCMD_Update_SignRecord_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWS_DCMD_Update_SignRecord_atlsoapheader_map = 
	{
		0x315F5701,
		"Update_SignRecord",
		L"Update_SignRecordResponse",
		sizeof("Update_SignRecord")-1,
		sizeof("Update_SignRecordResponse")-1,
		SOAPMAP_HEADER,
		__CWS_DCMD_Update_SignRecord_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWS_DCMD_Is_recv_info_HasSaved_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWS_DCMD_Is_recv_info_HasSaved_atlsoapheader_map = 
	{
		0xBF385E53,
		"Is_recv_info_HasSaved",
		L"Is_recv_info_HasSavedResponse",
		sizeof("Is_recv_info_HasSaved")-1,
		sizeof("Is_recv_info_HasSavedResponse")-1,
		SOAPMAP_HEADER,
		__CWS_DCMD_Is_recv_info_HasSaved_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWS_DCMD_Get_DCMD_ID_By_recv_info2_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWS_DCMD_Get_DCMD_ID_By_recv_info2_atlsoapheader_map = 
	{
		0xE44273F8,
		"Get_DCMD_ID_By_recv_info2",
		L"Get_DCMD_ID_By_recv_info2Response",
		sizeof("Get_DCMD_ID_By_recv_info2")-1,
		sizeof("Get_DCMD_ID_By_recv_info2Response")-1,
		SOAPMAP_HEADER,
		__CWS_DCMD_Get_DCMD_ID_By_recv_info2_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWS_DCMD_Get_DCMD_ID_By_recv_info_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWS_DCMD_Get_DCMD_ID_By_recv_info_atlsoapheader_map = 
	{
		0x5E674AE6,
		"Get_DCMD_ID_By_recv_info",
		L"Get_DCMD_ID_By_recv_infoResponse",
		sizeof("Get_DCMD_ID_By_recv_info")-1,
		sizeof("Get_DCMD_ID_By_recv_infoResponse")-1,
		SOAPMAP_HEADER,
		__CWS_DCMD_Get_DCMD_ID_By_recv_info_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWS_DCMD_IS_DcmdNumber_Validated_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWS_DCMD_IS_DcmdNumber_Validated_atlsoapheader_map = 
	{
		0x8FCD88D8,
		"IS_DcmdNumber_Validated",
		L"IS_DcmdNumber_ValidatedResponse",
		sizeof("IS_DcmdNumber_Validated")-1,
		sizeof("IS_DcmdNumber_ValidatedResponse")-1,
		SOAPMAP_HEADER,
		__CWS_DCMD_IS_DcmdNumber_Validated_atlsoapheader_entries,
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

	static const _soapmapentry __CWS_DCMD_Insert_Dcmd_main_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWS_DCMD_Insert_Dcmd_main_atlsoapheader_map = 
	{
		0xFABBF97F,
		"Insert_Dcmd_main",
		L"Insert_Dcmd_mainResponse",
		sizeof("Insert_Dcmd_main")-1,
		sizeof("Insert_Dcmd_mainResponse")-1,
		SOAPMAP_HEADER,
		__CWS_DCMD_Insert_Dcmd_main_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWS_DCMD_Update_Dcmd_Template_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWS_DCMD_Update_Dcmd_Template_atlsoapheader_map = 
	{
		0x8EF88684,
		"Update_Dcmd_Template",
		L"Update_Dcmd_TemplateResponse",
		sizeof("Update_Dcmd_Template")-1,
		sizeof("Update_Dcmd_TemplateResponse")-1,
		SOAPMAP_HEADER,
		__CWS_DCMD_Update_Dcmd_Template_atlsoapheader_entries,
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

	static const _soapmapentry __CWS_DCMD_Reset_DcmdNumber_By_DCMD_DATE_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWS_DCMD_Reset_DcmdNumber_By_DCMD_DATE_atlsoapheader_map = 
	{
		0xBA377AA0,
		"Reset_DcmdNumber_By_DCMD_DATE",
		L"Reset_DcmdNumber_By_DCMD_DATEResponse",
		sizeof("Reset_DcmdNumber_By_DCMD_DATE")-1,
		sizeof("Reset_DcmdNumber_By_DCMD_DATEResponse")-1,
		SOAPMAP_HEADER,
		__CWS_DCMD_Reset_DcmdNumber_By_DCMD_DATE_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWS_DCMD_Get_Next_DCMD_Number_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWS_DCMD_Get_Next_DCMD_Number_atlsoapheader_map = 
	{
		0x8226E6EC,
		"Get_Next_DCMD_Number",
		L"Get_Next_DCMD_NumberResponse",
		sizeof("Get_Next_DCMD_Number")-1,
		sizeof("Get_Next_DCMD_NumberResponse")-1,
		SOAPMAP_HEADER,
		__CWS_DCMD_Get_Next_DCMD_Number_atlsoapheader_entries,
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
		&__CWS_DCMD_Reset_DcmdNumber_atlsoapheader_map,
		&__CWS_DCMD_Update_Dcmd_Number_Setting_atlsoapheader_map,
		&__CWS_DCMD_Get_dcmd_template_all_atlsoapheader_map,
		&__CWS_DCMD_Get_dcmd_By_dcmdid_atlsoapheader_map,
		&__CWS_DCMD_Get_SignRecord_By_DCMDID_atlsoapheader_map,
		&__CWS_DCMD_Get_SignRecord_data_atlsoapheader_map,
		&__CWS_DCMD_Update_Signal_Status_Local_atlsoapheader_map,
		&__CWS_DCMD_Get_Dcmd_Number_Setting_All_atlsoapheader_map,
		&__CWS_DCMD_Update_Dcmd_and_DcmdNum_atlsoapheader_map,
		&__CWS_DCMD_Update_Dcmd_atlsoapheader_map,
		&__CWS_DCMD_Update_Recv_Status_atlsoapheader_map,
		&__CWS_DCMD_Delete_Dcmd_By_Dcmdid_atlsoapheader_map,
		&__CWS_DCMD_Is_All_Signaled_atlsoapheader_map,
		&__CWS_DCMD_Is_DB_Connected_atlsoapheader_map,
		&__CWS_DCMD_Update_SignRecord_atlsoapheader_map,
		&__CWS_DCMD_Is_recv_info_HasSaved_atlsoapheader_map,
		&__CWS_DCMD_Get_DCMD_ID_By_recv_info2_atlsoapheader_map,
		&__CWS_DCMD_Get_DCMD_ID_By_recv_info_atlsoapheader_map,
		&__CWS_DCMD_IS_DcmdNumber_Validated_atlsoapheader_map,
		&__CWS_DCMD_Get_dcmd_index_by_Condition_atlsoapheader_map,
		&__CWS_DCMD_Insert_Dcmd_main_atlsoapheader_map,
		&__CWS_DCMD_Update_Dcmd_Template_atlsoapheader_map,
		&__CWS_DCMD_Hello_atlsoapheader_map,
		&__CWS_DCMD_Reset_DcmdNumber_By_DCMD_DATE_atlsoapheader_map,
		&__CWS_DCMD_Get_Next_DCMD_Number_atlsoapheader_map,
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
inline HRESULT AtlCleanupValue<WS_DCMD::Recv_Dcmd_Xml_Info>(WS_DCMD::Recv_Dcmd_Xml_Info *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Recv_Send_Entity);
	AtlCleanupValue(&pVal->Recv_Creat_Time);
	AtlCleanupValue(&pVal->Train_ID);
	AtlCleanupValue(&pVal->Locomotive);
	AtlCleanupValue(&pVal->Station_ID);
	AtlCleanupValue(&pVal->Recv_Time);
	AtlCleanupValue(&pVal->Recv_entity);
	AtlCleanupValue(&pVal->Recv_Status);
	AtlCleanupValue(&pVal->Sign_Name);
	AtlCleanupValue(&pVal->Cmd_no);
	AtlCleanupValue(&pVal->parse_succeed);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WS_DCMD::Recv_Dcmd_Xml_Info>(WS_DCMD::Recv_Dcmd_Xml_Info *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Recv_Send_Entity, pMemMgr);
	AtlCleanupValueEx(&pVal->Recv_Creat_Time, pMemMgr);
	AtlCleanupValueEx(&pVal->Train_ID, pMemMgr);
	AtlCleanupValueEx(&pVal->Locomotive, pMemMgr);
	AtlCleanupValueEx(&pVal->Station_ID, pMemMgr);
	AtlCleanupValueEx(&pVal->Recv_Time, pMemMgr);
	AtlCleanupValueEx(&pVal->Recv_entity, pMemMgr);
	AtlCleanupValueEx(&pVal->Recv_Status, pMemMgr);
	AtlCleanupValueEx(&pVal->Sign_Name, pMemMgr);
	AtlCleanupValueEx(&pVal->Cmd_no, pMemMgr);
	AtlCleanupValueEx(&pVal->parse_succeed, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WS_DCMD::DutyChief_Info>(WS_DCMD::DutyChief_Info *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->DutyChief_EntityID);
	AtlCleanupValue(&pVal->DutyChief_DisplayName);
	AtlCleanupValue(&pVal->DutyChief_ID);
	AtlCleanupValue(&pVal->DutyChief_GPCID);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WS_DCMD::DutyChief_Info>(WS_DCMD::DutyChief_Info *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->DutyChief_EntityID, pMemMgr);
	AtlCleanupValueEx(&pVal->DutyChief_DisplayName, pMemMgr);
	AtlCleanupValueEx(&pVal->DutyChief_ID, pMemMgr);
	AtlCleanupValueEx(&pVal->DutyChief_GPCID, pMemMgr);
	return S_OK;
}

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

template<>
inline HRESULT AtlCleanupValue<WS_DCMD::Console_Info>(WS_DCMD::Console_Info *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Console_EntityID);
	AtlCleanupValue(&pVal->Console_DisplayName);
	AtlCleanupValue(&pVal->Console_ID);
	AtlCleanupValue(&pVal->Console_GPCID);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WS_DCMD::Console_Info>(WS_DCMD::Console_Info *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Console_EntityID, pMemMgr);
	AtlCleanupValueEx(&pVal->Console_DisplayName, pMemMgr);
	AtlCleanupValueEx(&pVal->Console_ID, pMemMgr);
	AtlCleanupValueEx(&pVal->Console_GPCID, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WS_DCMD::Station_Info>(WS_DCMD::Station_Info *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Station_EntityID);
	AtlCleanupValue(&pVal->Station_DisplayName);
	AtlCleanupValue(&pVal->Station_ID);
	AtlCleanupValue(&pVal->Lirc_EntityID);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WS_DCMD::Station_Info>(WS_DCMD::Station_Info *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Station_EntityID, pMemMgr);
	AtlCleanupValueEx(&pVal->Station_DisplayName, pMemMgr);
	AtlCleanupValueEx(&pVal->Station_ID, pMemMgr);
	AtlCleanupValueEx(&pVal->Lirc_EntityID, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WS_DCMD::DCMD_Setting>(WS_DCMD::DCMD_Setting *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Send_Entity);
	AtlCleanupValue(&pVal->Sender_Type);
	AtlCleanupValue(&pVal->Sender_ID);
	AtlCleanupValue(&pVal->Tcc_Version);
	AtlCleanupValue(&pVal->OPMS_Interface);
	AtlCleanupValue(&pVal->Console_DisplayName);
	AtlCleanupValue(&pVal->Dcmd_num_Start);
	AtlCleanupValue(&pVal->Dcmd_num_End);
	AtlCleanupValue(&pVal->GSM_R_entity);
	AtlCleanupValue(&pVal->BaseLevel_entity);
	AtlCleanupValue(&pVal->TSR_entity);
	AtlCleanupArray(pVal->Recv_Station_List, pVal->__Recv_Station_List_nSizeIs);
	AtlCleanupArray(pVal->Recv_Console_List, pVal->__Recv_Console_List_nSizeIs);
	AtlCleanupArray(pVal->Recv_DutyChief_List, pVal->__Recv_DutyChief_List_nSizeIs);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WS_DCMD::DCMD_Setting>(WS_DCMD::DCMD_Setting *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Send_Entity, pMemMgr);
	AtlCleanupValueEx(&pVal->Sender_Type, pMemMgr);
	AtlCleanupValueEx(&pVal->Sender_ID, pMemMgr);
	AtlCleanupValueEx(&pVal->Tcc_Version, pMemMgr);
	AtlCleanupValueEx(&pVal->OPMS_Interface, pMemMgr);
	AtlCleanupValueEx(&pVal->Console_DisplayName, pMemMgr);
	AtlCleanupValueEx(&pVal->Dcmd_num_Start, pMemMgr);
	AtlCleanupValueEx(&pVal->Dcmd_num_End, pMemMgr);
	AtlCleanupValueEx(&pVal->GSM_R_entity, pMemMgr);
	AtlCleanupValueEx(&pVal->BaseLevel_entity, pMemMgr);
	AtlCleanupValueEx(&pVal->TSR_entity, pMemMgr);
	if (pVal->Recv_Station_List != NULL)
	{
		AtlCleanupArrayEx(pVal->Recv_Station_List, pVal->__Recv_Station_List_nSizeIs, pMemMgr);
		pMemMgr->Free(pVal->Recv_Station_List);
	}
	if (pVal->Recv_Console_List != NULL)
	{
		AtlCleanupArrayEx(pVal->Recv_Console_List, pVal->__Recv_Console_List_nSizeIs, pMemMgr);
		pMemMgr->Free(pVal->Recv_Console_List);
	}
	if (pVal->Recv_DutyChief_List != NULL)
	{
		AtlCleanupArrayEx(pVal->Recv_DutyChief_List, pVal->__Recv_DutyChief_List_nSizeIs, pMemMgr);
		pMemMgr->Free(pVal->Recv_DutyChief_List);
	}
	return S_OK;
}
