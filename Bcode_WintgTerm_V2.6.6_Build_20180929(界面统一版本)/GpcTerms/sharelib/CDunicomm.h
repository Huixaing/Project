#pragma once

// 计算机生成了由 Microsoft Visual C++ 创建的 IDispatch 包装类

// 注意: 不要修改此文件的内容。如果此类由
//  Microsoft Visual C++ 重新生成，您的修改将被改写。

/////////////////////////////////////////////////////////////////////////////
// CUnicomm 包装类

class CDUnicomm : public CWnd
{
protected:
	DECLARE_DYNCREATE(CDUnicomm)
public:
	CLSID const& GetClsid()
	{
		static CLSID const clsid
			= { 0xB5EC8714, 0x9122, 0x44FA, { 0xB5, 0x85, 0xB, 0x2, 0x33, 0xA1, 0x65, 0x22 } };
		return clsid;
	}
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,
						const RECT& rect, CWnd* pParentWnd, UINT nID, 
						CCreateContext* pContext = NULL)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID); 
	}

    BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, 
				UINT nID, CFile* pPersist = NULL, BOOL bStorage = FALSE,
				BSTR bstrLicKey = NULL)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID,
		pPersist, bStorage, bstrLicKey); 
	}

// 属性
public:


// 操作
public:

// _DUnicomm

// Functions
//

	short Start()
	{
		short result;
		InvokeHelper(0x4, DISPATCH_METHOD, VT_I2, (void*)&result, NULL);
		return result;
	}
	short Stop()
	{
		short result;
		InvokeHelper(0x5, DISPATCH_METHOD, VT_I2, (void*)&result, NULL);
		return result;
	}
	short Reconnect()
	{
		short result;
		InvokeHelper(0x6, DISPATCH_METHOD, VT_I2, (void*)&result, NULL);
		return result;
	}
	short PostMessage(long source_id, long dest_id, long label, short flag, VARIANT msg)
	{
		short result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I2 VTS_VARIANT ;
		InvokeHelper(0x7, DISPATCH_METHOD, VT_I2, (void*)&result, parms, source_id, dest_id, label, flag, &msg);
		return result;
	}
	short LogMessage(short type, short pri,unsigned long id, LPCTSTR msg)
	{
		short result;
		static BYTE parms[] = VTS_I2 VTS_I2 VTS_I4 VTS_BSTR ;
		InvokeHelper(0x8, DISPATCH_METHOD, VT_I2, (void*)&result, parms, type, pri,id, msg);
		return result;
	}
	void AboutBox()
	{
		InvokeHelper(DISPID_ABOUTBOX, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	short PostMessageS(long dest_id, VARIANT msg)
	{
		short result;
		static BYTE parms[] = VTS_I4 VTS_VARIANT ;
		InvokeHelper(0x9, DISPATCH_METHOD, VT_I2, (void*)&result, parms, dest_id, &msg);
		return result;
	}
	short SetLocalEntityID(long entity_id)
	{
		short result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xa, DISPATCH_METHOD, VT_I2, (void*)&result, parms, entity_id);
		return result;
	}
	short SetRelateEntityID(long entity_id)
	{
		short result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xb, DISPATCH_METHOD, VT_I2, (void*)&result, parms, entity_id);
		return result;
	}
	short GetRelateEntityStatus(long entity_id)
	{
		short result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xc, DISPATCH_METHOD, VT_I2, (void*)&result, parms, entity_id);
		return result;
	}
	void EnableCommLogging(short flag)
	{
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0xe, DISPATCH_METHOD, VT_EMPTY, NULL, parms, flag);
	}
	short PostXMLMessage(long source_id, long dest_id, long label, short flag, LPCTSTR szMsgString)
	{
		short result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I2 VTS_BSTR ;
		InvokeHelper(0xf, DISPATCH_METHOD, VT_I2, (void*)&result, parms, source_id, dest_id, label, flag, szMsgString);
		return result;
	}
	short SetCommServerEntityID(long entity_id)
	{
		short result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x10, DISPATCH_METHOD, VT_I2, (void*)&result, parms, entity_id);
		return result;
	}
	short RemoveRelateEntityID(long entity_id)
	{
		short result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x11, DISPATCH_METHOD, VT_I2, (void*)&result, parms, entity_id);
		return result;
	}
	short SetSNMPObjectValue_Integer(LPCTSTR obj_name, long val)
	{
		short result;
		static BYTE parms[] = VTS_BSTR VTS_I4 ;
		InvokeHelper(0x12, DISPATCH_METHOD, VT_I2, (void*)&result, parms, obj_name, val);
		return result;
	}
	short SetSNMPObjectValue_String(LPCTSTR obj_name, LPCTSTR valString)
	{
		short result;
		static BYTE parms[] = VTS_BSTR VTS_BSTR ;
		InvokeHelper(0x13, DISPATCH_METHOD, VT_I2, (void*)&result, parms, obj_name, valString);
		return result;
	}
	short SetSNMPInterfaceStatus(LPCTSTR obj_name, long status, long para1, long para2)
	{
		short result;
		static BYTE parms[] = VTS_BSTR VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x14, DISPATCH_METHOD, VT_I2, (void*)&result, parms, obj_name, status, para1, para2);
		return result;
	}
	short StopT(short tRestartDelay)
	{
		short result;
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0x15, DISPATCH_METHOD, VT_I2, (void*)&result, parms, tRestartDelay);
		return result;
	}
	short PostTextMessage(short msg_type, LPCTSTR ip_addr, short port, LPCTSTR msg, long msglen)
	{
		short result;
		static BYTE parms[] = VTS_I2 VTS_BSTR VTS_I2 VTS_BSTR VTS_I4 ;
		InvokeHelper(0x16, DISPATCH_METHOD, VT_I2, (void*)&result, parms, msg_type, ip_addr, port, msg, msglen);
		return result;
	}
	short PostXMLMessage2(long source_id, long dest_id, long label, short flag, LPCTSTR szToken, LPCTSTR szMsgString, unsigned char bOneFrame)
	{
		short result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I2 VTS_BSTR VTS_BSTR VTS_UI1 ;
		InvokeHelper(0x19, DISPATCH_METHOD, VT_I2, (void*)&result, parms, source_id, dest_id, label, flag, szToken, szMsgString, bOneFrame);
		return result;
	}
	short PostXMLMessage3(long source_id, long dest_id, long label, short flag, LPCTSTR szSysID, LPCTSTR szToken, LPCTSTR szMsgString, unsigned long nPackageCount, unsigned long nPackageIndex, unsigned long nTotalLength)
	{
		short result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I2 VTS_BSTR VTS_BSTR VTS_BSTR VTS_UI4 VTS_UI4 VTS_UI4 ;
		InvokeHelper(0x1b, DISPATCH_METHOD, VT_I2, (void*)&result, parms, source_id, dest_id, label, flag, szSysID, szToken, szMsgString, nPackageCount, nPackageIndex, nTotalLength);
		return result;
	}
	short AddExtLocalEntityID(unsigned long entity_id)
	{
		short result;
		static BYTE parms[] = VTS_UI4 ;
		InvokeHelper(0x1c, DISPATCH_METHOD, VT_I2, (void*)&result, parms, entity_id);
		return result;
	}
	short ClearExtLocalEntityID()
	{
		short result;
		InvokeHelper(0x1d, DISPATCH_METHOD, VT_I2, (void*)&result, NULL);
		return result;
	}
	void SetCommServerEntityIDx(short com_id, unsigned long entity_id)
	{
		static BYTE parms[] = VTS_I2 VTS_UI4 ;
		InvokeHelper(0x1e, DISPATCH_METHOD, VT_EMPTY, NULL, parms, com_id, entity_id);
	}
	short SetMSStatus(short nAttribType, unsigned long nAttribID, short nAttribIndex, LPCTSTR szAttribName, unsigned long nValue,unsigned long nValue2,unsigned long nValue3, LPCTSTR szValue, short flag)
	{
		short result;
		static BYTE parms[] = VTS_I2 VTS_UI4 VTS_I2 VTS_BSTR VTS_UI4 VTS_UI4 VTS_UI4 VTS_BSTR VTS_I2 ;
		InvokeHelper(0x1f, DISPATCH_METHOD, VT_I2, (void*)&result, parms, nAttribType, nAttribID, nAttribIndex, szAttribName, nValue,nValue2,nValue3, szValue, flag);
		return result;
	}
	short ReplyMSStatus(short nAttribType, unsigned long nAttribID, short nAttribIndex, LPCTSTR szAttribName, unsigned long nValue,unsigned long nValue2,unsigned long nValue3, LPCTSTR szValue)
	{
		short result;
		static BYTE parms[] = VTS_I2 VTS_UI4 VTS_I2 VTS_BSTR VTS_UI4 VTS_UI4 VTS_UI4 VTS_BSTR ;
		InvokeHelper(0x21, DISPATCH_METHOD, VT_I2, (void*)&result, parms, nAttribType, nAttribID, nAttribIndex, szAttribName, nValue,nValue2,nValue3,szValue);
		return result;
	}
	void SetMSAttribIndexRange(short nAttribType, short nMinIndex, short nMaxIndex)
	{
		static BYTE parms[] = VTS_I2 VTS_I2 VTS_I2 ;
		InvokeHelper(0x22, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nAttribType, nMinIndex, nMaxIndex);
	}
	short PostMessageX(short com_id, long source_id, long dest_id, long label, short flag, VARIANT msg)
	{
		short result;
		static BYTE parms[] = VTS_I2 VTS_I4 VTS_I4 VTS_I4 VTS_I2 VTS_VARIANT ;
		InvokeHelper(0x23, DISPATCH_METHOD, VT_I2, (void*)&result, parms, com_id, source_id, dest_id, label, flag, &msg);
		return result;
	}
	short PostMessageSX(short com_id, long dest_id, VARIANT msg)
	{
		short result;
		static BYTE parms[] = VTS_I2 VTS_I4 VTS_VARIANT ;
		InvokeHelper(0x24, DISPATCH_METHOD, VT_I2, (void*)&result, parms, com_id, dest_id, &msg);
		return result;
	}
	short PostXMLMessageX(short com_id, long source_id, long dest_id, long label, short flag, LPCTSTR szMsgString)
	{
		short result;
		static BYTE parms[] = VTS_I2 VTS_I4 VTS_I4 VTS_I4 VTS_I2 VTS_BSTR ;
		InvokeHelper(0x25, DISPATCH_METHOD, VT_I2, (void*)&result, parms, com_id, source_id, dest_id, label, flag, szMsgString);
		return result;
	}
	short PostXMLMessage2X(short com_id, long source_id, long dest_id, long label, short flag, LPCTSTR szToken, LPCTSTR szMsgString, unsigned char bOneFrame)
	{
		short result;
		static BYTE parms[] = VTS_I2 VTS_I4 VTS_I4 VTS_I4 VTS_I2 VTS_BSTR VTS_BSTR VTS_UI1 ;
		InvokeHelper(0x26, DISPATCH_METHOD, VT_I2, (void*)&result, parms, com_id, source_id, dest_id, label, flag, szToken, szMsgString, bOneFrame);
		return result;
	}
	short PostXMLMessage3X(short com_id, long source_id, long dest_id, long label, short flag, LPCTSTR szSysID, LPCTSTR szToken, LPCTSTR szMsgString, unsigned long nPackageCount, unsigned long nPackageIndex, unsigned long nTotalLength)
	{
		short result;
		static BYTE parms[] = VTS_I2 VTS_I4 VTS_I4 VTS_I4 VTS_I2 VTS_BSTR VTS_BSTR VTS_BSTR VTS_UI4 VTS_UI4 VTS_UI4 ;
		InvokeHelper(0x27, DISPATCH_METHOD, VT_I2, (void*)&result, parms, com_id, source_id, dest_id, label, flag, szSysID, szToken, szMsgString, nPackageCount, nPackageIndex, nTotalLength);
		return result;
	}
	short SetLocalStationID(long station_id)
	{
		short result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x28, DISPATCH_METHOD, VT_I2, (void*)&result, parms, station_id);
		return result;
	}
	short SetRelateEntityIDX(long entity_id, long flag)
	{
		short result;
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x29, DISPATCH_METHOD, VT_I2, (void*)&result, parms, entity_id, flag);
		return result;
	}

// Properties
//

CString GetCommTaskConfigFile()
{
	CString result;
	GetProperty(0x1, VT_BSTR, (void*)&result);
	return result;
}
void SetCommTaskConfigFile(CString propVal)
{
	SetProperty(0x1, VT_BSTR, propVal);
}
CString GetMainConfigFile()
{
	CString result;
	GetProperty(0x2, VT_BSTR, (void*)&result);
	return result;
}
void SetMainConfigFile(CString propVal)
{
	SetProperty(0x2, VT_BSTR, propVal);
}
short GetCommStatus()
{
	short result;
	GetProperty(0x3, VT_I2, (void*)&result);
	return result;
}
void SetCommStatus(short propVal)
{
	SetProperty(0x3, VT_I2, propVal);
}
CString GetCommLogFileName()
{
	CString result;
	GetProperty(0xd, VT_BSTR, (void*)&result);
	return result;
}
void SetCommLogFileName(CString propVal)
{
	SetProperty(0xd, VT_BSTR, propVal);
}
CString GetCommSocketName()
{
	CString result;
	GetProperty(0x17, VT_BSTR, (void*)&result);
	return result;
}
void SetCommSocketName(CString propVal)
{
	SetProperty(0x17, VT_BSTR, propVal);
}
long GetFunctionFlag()
{
	long  result;
	GetProperty(0x18, VT_I4, (void*)&result);
	return result;
}
void SetFunctionFlag(long propVal)
{
	SetProperty(0x18, VT_I4, propVal);
}
short GetMachineType()
{
	short result;
	GetProperty(0x1a, VT_I2, (void*)&result);
	return result;
}
void SetMachineType(short propVal)
{
	SetProperty(0x1a, VT_I2, propVal);
}
short GetnMSPort()
{
	short result;
	GetProperty(0x20, VT_I2, (void*)&result);
	return result;
}
void SetnMSPort(short propVal)
{
	SetProperty(0x20, VT_I2, propVal);
}
};
