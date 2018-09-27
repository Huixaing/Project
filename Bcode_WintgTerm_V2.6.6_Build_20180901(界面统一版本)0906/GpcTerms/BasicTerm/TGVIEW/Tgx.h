#pragma once

// 计算机生成了由 Microsoft Visual C++ 创建的 IDispatch 包装类

// 注意: 不要修改此文件的内容。如果此类由
//  Microsoft Visual C++ 重新生成，您的修改将被改写。

/////////////////////////////////////////////////////////////////////////////
// CTgX 包装类
#include "tg_msgcopy_newctcmsg.h"

class CTgX : public CWnd
{
protected:
	DECLARE_DYNCREATE(CTgX)
public:
	CLSID const& GetClsid()
	{
		static CLSID const clsid
			= { 0xEE53802C, 0x2D94, 0x483C, { 0x8C, 0x4D, 0x91, 0x6B, 0x80, 0x55, 0x28, 0x11 } };
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

// _DTG

// Functions
//

	void TgStart(LPCTSTR filename)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x1, DISPATCH_METHOD, VT_EMPTY, NULL, parms, filename);
	}
	void SetTrainDisplayFlag(short type, short val)
	{
		static BYTE parms[] = VTS_I2 VTS_I2 ;
		InvokeHelper(0x2, DISPATCH_METHOD, VT_EMPTY, NULL, parms, type, val);
	}
	void SetBackGroud(unsigned short flag)
	{
		static BYTE parms[] = VTS_UI2 ;
		InvokeHelper(0x3, DISPATCH_METHOD, VT_EMPTY, NULL, parms, flag);
	}
	void SetTgType(unsigned char type)
	{
		static BYTE parms[] = VTS_UI1 ;
		InvokeHelper(0x4, DISPATCH_METHOD, VT_EMPTY, NULL, parms, type);
	}
	void SetBackgroudBeginHour(unsigned char begin_hour)
	{
		static BYTE parms[] = VTS_UI1 ;
		InvokeHelper(0x5, DISPATCH_METHOD, VT_EMPTY, NULL, parms, begin_hour);
	}
	void SetBackgroudTimeLength(unsigned char nHours)
	{
		static BYTE parms[] = VTS_UI1 ;
		InvokeHelper(0x6, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nHours);
	}
	void SetStationInfo(LPCTSTR xml)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x7, DISPATCH_METHOD, VT_EMPTY, NULL, parms, xml);
	}
	void SetCurrentSectionId(short flag)
	{
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0x8, DISPATCH_METHOD, VT_EMPTY, NULL, parms, flag);
	}
	void SetShiftId(unsigned long nshiftid, short nCurValue)
	{
		static BYTE parms[] = VTS_UI4 VTS_I2 ;
		InvokeHelper(0x9, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nshiftid, nCurValue);
	}
	void DeleteMutiTrain()
	{
		InvokeHelper(0xa, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	short DrawSpecialTrain(short trainType)
	{
		short result;
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0xb, DISPATCH_METHOD, VT_I2, (void*)&result, parms, trainType);
		return result;
	}
	void FindTrain()
	{
		InvokeHelper(0xc, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void TrainsOperator(short oper, short rst)
	{
		static BYTE parms[] = VTS_I2 VTS_I2 ;
		InvokeHelper(0xd, DISPATCH_METHOD, VT_EMPTY, NULL, parms, oper, rst);
	}
	short SelectSection(short nSection, long nFlag)
	{
		short result;
		static BYTE parms[] = VTS_I2 VTS_I4 ;
		InvokeHelper(0xe, DISPATCH_METHOD, VT_I2, (void*)&result, parms, nSection, nFlag);
		return result;
	}
	short ZoomIn(short count)
	{
		short result;
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0xf, DISPATCH_METHOD, VT_I2, (void*)&result, parms, count);
		return result;
	}
	short ZoomOut(short count)
	{
		short result;
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0x10, DISPATCH_METHOD, VT_I2, (void*)&result, parms, count);
		return result;
	}
	short GetZoomStatus()
	{
		short result;
		InvokeHelper(0x11, DISPATCH_METHOD, VT_I2, (void*)&result, NULL);
		return result;
	}
	void SetStationConnectStatus(short nStaId, unsigned char active)
	{
		static BYTE parms[] = VTS_I2 VTS_UI1 ;
		InvokeHelper(0x12, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nStaId, active);
	}
	void SetCtrlMode(short nStaId, short sta_ctrl_mode, short route_ctrl_mode)
	{
		static BYTE parms[] = VTS_I2 VTS_I2 VTS_I2 ;
		InvokeHelper(0x13, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nStaId, sta_ctrl_mode, route_ctrl_mode);
	}
	short GetTrainDisplayFlag(short type, short val)
	{
		short result;
		static BYTE parms[] = VTS_I2 VTS_I2 ;
		InvokeHelper(0x14, DISPATCH_METHOD, VT_I2, (void*)&result, parms, type, val);
		return result;
	}
	void SaveToPicture(LPCTSTR pathname)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x15, DISPATCH_METHOD, VT_EMPTY, NULL, parms, pathname);
	}
	void NewMark(short mark_type)
	{
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0x16, DISPATCH_METHOD, VT_EMPTY, NULL, parms, mark_type);
	}
	void BeginNewSingleTrain(LPCTSTR strTrainId, LPCTSTR strChangeTrainId, unsigned char inMode, unsigned char outMode)
	{
		static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_UI1 VTS_UI1 ;
		InvokeHelper(0x17, DISPATCH_METHOD, VT_EMPTY, NULL, parms, strTrainId, strChangeTrainId, inMode, outMode);
	}
	void EndNewSingleTrain()
	{
		InvokeHelper(0x18, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void DeleteTrainOCX(long nIndex, long nLogic)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x19, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nIndex, nLogic);
	}
	void UpdateTrainOCX(const CTG_SingleTrain& train)
	{
		SendTrainMsgToOCX(train, SEND_FLAG::Batch_Train);
	}
	void AppendTrainOCX(const CTG_SingleTrain& train)
	{
		SendTrainMsgToOCX(train, SEND_FLAG::Batch_Train);
	}
	void DeleteMarkOCX(long nIndex)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x1c, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nIndex);
	}
	void UpdateMarkOCX(const CTG_SingleMark& mark)
	{
		SendBlockMsgToOCX(mark);
	}
	void AppendMarkOCX(const CTG_SingleMark& mark)
	{
		SendBlockMsgToOCX(mark);
	}
	void EndLoadData()
	{
		GpcLogTrainMessage(0, 0, "EndLoadData");

		InvokeHelper(0x1f, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void BeginLoadData()
	{
		GpcLogTrainMessage(0, 0, "BeginLoadData");

		InvokeHelper(0x20, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void TurnReality()
	{
		InvokeHelper(0x21, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void ReceivePoint()
	{
		InvokeHelper(0x22, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void SetAppCtrlWord(unsigned long word)
	{
		static BYTE parms[] = VTS_UI4 ;
		InvokeHelper(0x23, DISPATCH_METHOD, VT_EMPTY, NULL, parms, word);
	}
	void SetLineCtrlWord(unsigned long word)
	{
		static BYTE parms[] = VTS_UI4 ;
		InvokeHelper(0x24, DISPATCH_METHOD, VT_EMPTY, NULL, parms, word);
	}
	void SetMarkCtrlWord(unsigned long word)
	{
		static BYTE parms[] = VTS_UI4 ;
		InvokeHelper(0x25, DISPATCH_METHOD, VT_EMPTY, NULL, parms, word);
	}
	unsigned long GetAppCtrlWord()
	{
		unsigned long result;
		InvokeHelper(0x26, DISPATCH_METHOD, VT_UI4, (void*)&result, NULL);
		return result;
	}
	unsigned long GetLineCtrlWord()
	{
		unsigned long result;
		InvokeHelper(0x27, DISPATCH_METHOD, VT_UI4, (void*)&result, NULL);
		return result;
	}
	unsigned long GetMarkCtrlWord()
	{
		unsigned long result;
		InvokeHelper(0x28, DISPATCH_METHOD, VT_UI4, (void*)&result, NULL);
		return result;
	}
	void SetOperateRight(unsigned char right)
	{
		static BYTE parms[] = VTS_UI1 ;
		InvokeHelper(0x29, DISPATCH_METHOD, VT_EMPTY, NULL, parms, right);
	}
	unsigned char GetOperateRight()
	{
		unsigned char result;
		InvokeHelper(0x2a, DISPATCH_METHOD, VT_UI1, (void*)&result, NULL);
		return result;
	}
	void SetLeapLineFlag(unsigned char flag)
	{
		static BYTE parms[] = VTS_UI1 ;
		InvokeHelper(0x2b, DISPATCH_METHOD, VT_EMPTY, NULL, parms, flag);
	}
	void SetDrawTenicalStopMark(short flag)
	{
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0x2c, DISPATCH_METHOD, VT_EMPTY, NULL, parms, flag);
	}
	void SetUseBmp(unsigned char flag)
	{
		static BYTE parms[] = VTS_UI1 ;
		InvokeHelper(0x2d, DISPATCH_METHOD, VT_EMPTY, NULL, parms, flag);
	}
	void SetAllStationActive()
	{
		InvokeHelper(0x2e, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void SetPlanRecieveFlag(unsigned char nType, unsigned long id, unsigned long color)
	{
		static BYTE parms[] = VTS_UI1 VTS_UI4 VTS_UI4 ;
		InvokeHelper(0x2f, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nType, id, color);
	}
	void SetLeapTrainShow(unsigned char nFlag)
	{
		static BYTE parms[] = VTS_UI1 ;
		InvokeHelper(0x30, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nFlag);
	}
	void SetRouteOkMarkFlag(unsigned char flag)
	{
		static BYTE parms[] = VTS_UI1 ;
		InvokeHelper(0x31, DISPATCH_METHOD, VT_EMPTY, NULL, parms, flag);
	}
	void AliveWindow(unsigned long nFlag)
	{
		static BYTE parms[] = VTS_UI4 ;
		InvokeHelper(0x32, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nFlag);
	}
	unsigned long GetCurrentSectionID()
	{
		unsigned long result;
		InvokeHelper(0x33, DISPATCH_METHOD, VT_UI4, (void*)&result, NULL);
		return result;
	}
	CString GetTgVersion()
	{
		CString result;
		InvokeHelper(0x35, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void ShowWholeScreen(unsigned char nFlag)
	{
		static BYTE parms[] = VTS_UI1 ;
		InvokeHelper(0x37, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nFlag);
	}

	void SetContrlRight(unsigned long nFlag)
	{
		static BYTE parms[] = VTS_UI4 ;
		InvokeHelper(0x38, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nFlag);
	}
	
	void GetPlotParam(unsigned long nFlag)
	{
		static BYTE parms[] = VTS_UI4 ;
		InvokeHelper(0x39, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nFlag);
	}

	void GetTGMemoryData(unsigned long nFlag)
	{
		static BYTE parms[] = VTS_UI4 ;
		InvokeHelper(0x3a, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nFlag);
	}

	void SetBasicTrainWholeMove(unsigned char nFlag)
	{
		static BYTE parms[] = VTS_UI1 ;
		InvokeHelper(0x3b, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nFlag);
	}
	void RefreshTG(unsigned long nFlag)
	{
		static BYTE parms[] = VTS_UI4 ;
		InvokeHelper(0x3c, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nFlag);
	}
	void BeginNewSingleTrainXml(LPCTSTR xml)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x3d, DISPATCH_METHOD, VT_EMPTY, NULL, parms, xml);
	}
	void TGGenTrainNodeIndex(LPCTSTR xml)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x3e, DISPATCH_METHOD, VT_EMPTY, NULL, parms, xml);
	}
	void AppendEarlyLateInfo(LPCTSTR xml, BYTE val, LONG shiftid)
	{
		static BYTE parms[] = VTS_BSTR VTS_UI1 VTS_UI4;
		InvokeHelper(0x3f, DISPATCH_METHOD, VT_EMPTY, NULL, parms, xml, val, shiftid);
	}
	void SetWinTGOperateRight(unsigned long right)
	{
		static BYTE parms[] = VTS_UI4 ;
		InvokeHelper(0x40, DISPATCH_METHOD, VT_EMPTY, NULL, parms, right);
	}
	void SetUseVirtualMemory(short val)
	{
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0x41, DISPATCH_METHOD, VT_EMPTY, NULL, parms, val);
	}

	void BeginBatchDraw(unsigned long flag, CString oper)
	{
		GpcLogTrainMessage(0, 0, "BeginBatchDraw oper:%s flag=%d", oper, flag);

		static BYTE parms[] = VTS_UI4 ;
		InvokeHelper(0x43, DISPATCH_METHOD, VT_EMPTY, NULL, parms, flag);
	}

	void EndBatchDraw(unsigned long flag, CString oper)
	{
		GpcLogTrainMessage(0, 0, "EndBatchDraw oper:%s flag=%d", oper, flag);

		static BYTE parms[] = VTS_UI4 ;
		InvokeHelper(0x44, DISPATCH_METHOD, VT_EMPTY, NULL, parms, flag);
	}

	unsigned char  IsBatchStatus()
	{
		unsigned char result;
		InvokeHelper(0x46, DISPATCH_METHOD, VT_UI1, (void*)&result, NULL);
		return result;
	}

	void SetBackgrandInfo(unsigned long nshiftid, unsigned long nBeginTm, short nAutoMove, short nTotalLen, short nHistoryLen)
	{
		static BYTE parms[] = VTS_UI4 VTS_UI4 VTS_I2 VTS_I2 VTS_I2 ;
		InvokeHelper(0x47, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nshiftid, nBeginTm, nAutoMove, nTotalLen, nHistoryLen);
	}

	void NewShiftBanci()
	{
		InvokeHelper(0x48, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	
	unsigned long GetLoginShiftId()
	{
		unsigned long result;
		InvokeHelper(0x49, DISPATCH_METHOD, VT_UI4, (void*)&result, NULL);
		return result;
	}

	unsigned long GetShiftBeginTime()
	{
		unsigned long result;
		InvokeHelper(0x4a, DISPATCH_METHOD, VT_UI4, (void*)&result, NULL);
		return result;
	}

	unsigned long GetHistoryLen()
	{
		unsigned long result;
		InvokeHelper(0x4b, DISPATCH_METHOD, VT_UI4, (void*)&result, NULL);
		return result;
	}

	unsigned long GetAutoMove()
	{
		unsigned long result;
		InvokeHelper(0x4c, DISPATCH_METHOD, VT_UI4, (void*)&result, NULL);
		return result;
	}
	
	unsigned char DataIsExistInOCX(unsigned long trainindex, unsigned long data_type)
	{
		unsigned char result;
		static BYTE parms[] = VTS_UI4 VTS_UI4 ;
		InvokeHelper(0x4d, DISPATCH_METHOD, VT_UI1, (void*)&result, parms, trainindex, data_type);
		return result;
	}
	
	void SetSectionInfo(LPCTSTR xml)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x4e, DISPATCH_METHOD, VT_EMPTY, NULL, parms, xml);
	}

	void AddBStplanStation(unsigned long station)
	{
		static BYTE parms[] = VTS_UI4 ;
		InvokeHelper(0x4f, DISPATCH_METHOD, VT_EMPTY, NULL, parms, station);
	}

	void UsingWorkGoodMode()
	{
		InvokeHelper(0x50, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}

	void MoveTGOCXOneStep()
	{
		InvokeHelper(0x51, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}

	void SetGpcUserRight(unsigned long right)
	{
		static BYTE parms[] = VTS_UI4 ;
		InvokeHelper(0x52, DISPATCH_METHOD, VT_EMPTY, NULL, parms, right);
	}

	void AddNotMeetInfo(short not_meet_flag, LPCTSTR name, LPCTSTR short_name)
	{
		static BYTE parms[] = VTS_UI2 VTS_BSTR VTS_BSTR ;
		InvokeHelper(0x53, DISPATCH_METHOD, VT_EMPTY, NULL, parms, not_meet_flag, name, short_name);
	}

	void FindTrainByTrainIndex(unsigned long train_index)
	{
		static BYTE parms[] = VTS_UI4 ;
		InvokeHelper(0x54, DISPATCH_METHOD, VT_EMPTY, NULL, parms, train_index);
	}

	void SetFormatView()
	{
		InvokeHelper(0x55, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}

private:
	unsigned long  OnBatchAddSchduleData(VARIANT msg)
	{
		unsigned long result;
		static BYTE parms[] = VTS_VARIANT ;
		InvokeHelper(0x45, DISPATCH_METHOD, VT_UI4, (void*)&result, parms, &msg);
		return result;
	}
	CString OnGenExitEntry(VARIANT msg)
	{
		CString result;
		static BYTE parms[] = VTS_VARIANT ;
		InvokeHelper(0x36, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, &msg);
		return result;
	}
	CString SetTrainDefault(VARIANT msg)
	{
		CString result;
		static BYTE parms[] = VTS_VARIANT ;
		InvokeHelper(0x34, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, &msg);
		return result;
	}

public:
	enum SEND_FLAG
	{
		Batch_Train = 1,
		Set_TrainDefault = 2,
		Gen_ExitEntry = 3,
	};

	CString SendTrainMsgToOCX(const CTG_SingleTrain& train, SEND_FLAG flag)
	{
		static char buffer[40960];
		do
		{
			MsgTrainOCX msg;
			if(TRUE!=TrainCopyForOCX(msg, train))
				break;
			CLBTH::Archive ar(CLBTH::Archive::store, sizeof(buffer), buffer);
			CLBTH::HRCODE hr = msg.serialize(ar);
			if(hr.failed())
			{
				if(hr.sys_code != CLBTH::HRCODE::E_WRITE_OUTOFBUFFER)
				{
					break;
				}
			}
			int buf_len = ar.getCurrentPos();
			if(buf_len<=0)
			{
				break;
			}
			if(buf_len>=40960)
			{
				break;
			}
			BYTE *pdata=NULL;
			COleSafeArray byte_array;
			SAFEARRAYBOUND rgsabounds[1];
			rgsabounds[0].cElements=buf_len; // 元素个数
			rgsabounds[0].lLbound =0;  // 数组从0开始
			byte_array.Create(VT_I1, 1, rgsabounds); // 1维
			byte_array.AccessData((LPVOID*)&pdata);
			if(pdata)
			{
				memcpy(pdata,buffer,buf_len);
				byte_array.UnaccessData();
			}
			else
			{
				byte_array.UnaccessData();
				break;
			}
			
			if(flag==SEND_FLAG::Batch_Train)
				OnBatchAddSchduleData(byte_array);
			else if(flag==SEND_FLAG::Set_TrainDefault)
				return SetTrainDefault(byte_array);
			else if(flag==SEND_FLAG::Gen_ExitEntry)
				return OnGenExitEntry(byte_array);
		}while(0);

		return "";
	}

	void SendBlockMsgToOCX(const CTG_SingleMark& mark)
	{
		static char buffer[8096];

		MsgSchdBlockOCX msg;
		if(TRUE==BlockCopyForOCX(msg, mark))
		{
			CLBTH::Archive ar(CLBTH::Archive::store, sizeof(buffer), buffer);
			CLBTH::HRCODE hr = msg.serialize(ar);
			if(hr.failed())
			{
				if(hr.sys_code != CLBTH::HRCODE::E_WRITE_OUTOFBUFFER)
				{
					return;
				}
			}
			int buf_len = ar.getCurrentPos();
			if(buf_len<=0)
			{
				return;
			}
			if(buf_len>=4096)
			{
				return;
			}
			BYTE *pdata=NULL;
			COleSafeArray byte_array;
			SAFEARRAYBOUND rgsabounds[1];
			rgsabounds[0].cElements=buf_len; // 元素个数
			rgsabounds[0].lLbound =0;  // 数组从0开始
			byte_array.Create(VT_I1, 1, rgsabounds); // 1维
			byte_array.AccessData((LPVOID*)&pdata);
			if(pdata)
			{
				memcpy(pdata,buffer,buf_len);
				byte_array.UnaccessData();
			}
			else
			{
				byte_array.UnaccessData();
				return;
			}
			

			OnBatchAddSchduleData(byte_array);
		}
	}

// Properties
//
};
