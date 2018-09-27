/*
 * TCC实现文件
 */

#include "stdafx.h"

//#include <MsXml.h>
//#include <comutil.h>
//#import <msxml4.dll>
//#import <msxml4.dll> raw_interface_only
//using namespace MSXML2;
//#pragma comment(lib, "msxml2")
//#pragma comment(lib, "comsupp")

#include "Tcc_base.h"
#include "resource.h"
#include "Tcc_Dlg.h"

#include <map>
using namespace std;

#pragma comment(lib, "Rpcrt4")

namespace TCC_C3 {

	//-------------------------------------------------------------------------
	//
	// Check if MSXML 4.0 is installed
	//
	//-------------------------------------------------------------------------
	bool isMSXMLInstalled()
	{
		char szTemp[256];
		DWORD dwLen;

		try
		{
			HKEY hKey;
			DWORD retCode;

			retCode = RegOpenKeyEx(HKEY_CLASSES_ROOT,
				_T("CLSID\\{88d969c0-f192-11d4-a65f-0040963251e5}\\InProcServer32"),
				0,
				KEY_QUERY_VALUE,
				&hKey);

			if(retCode != ERROR_SUCCESS) return false;

			retCode = RegQueryValueEx(hKey, 
				_T(""),
				NULL,
				NULL,
				(LPBYTE)szTemp,
				&(dwLen = sizeof(szTemp)));

			if(retCode != ERROR_SUCCESS) return false;

			RegCloseKey(hKey);

			double dVer;
			int i;

			for(i = _tcslen(szTemp); i>=0; --i)
			{
				if (szTemp[i] == _T('\\'))
					break;
			}

			if (_stscanf(szTemp + i + 1, _T("msxml%lf"), &dVer) == 0 || dVer < 4.0)
			{
				_ftprintf(stderr, _T("\nError: MSXML 4.0 is not installed. Exiting.\n"));
				return false;
			}
			else
			{
				return true;
			}
		}
		catch(...)
		{
		}

		return false;
	}

WORD GetDefaultLimitSpeed()
{
	WORD speed = 0;
	//if(AvailableLimitSpeedList.size() > 0)
	//	speed = AvailableLimitSpeedList[0];
	return speed;
}


// 列控命令管理界面启动
HWND TccDialogOpen()
{
	HWND hWnd = GetTccDialogWndHandle();
	if(::IsWindow(hWnd))
	{
		if(!::IsWindowVisible(hWnd))
			::ShowWindow(hWnd, SW_SHOW);
	}
	else
		hWnd = NULL;

	return hWnd;
}

void TccDialogClose()
{
	HWND hWnd = GetTccDialogWndHandle();
	if(::IsWindow(hWnd))
		::SendMessage(hWnd, WM_CLOSE, 0, 0);
}

void TccDialogHide()
{
	HWND hWnd = GetTccDialogWndHandle();
	if(::IsWindow(hWnd))
		::ShowWindow(hWnd, SW_HIDE);
}

HWND TccDialog(
		ITccClientInfoProvider *pInfoProvider, 
		ITccAdviseSink *pAdviseSink, 
		CWnd *pParent, 
		LPCSTR szCaption)
{
	if (!isMSXMLInstalled())
	{
		AfxMessageBox(" MSXML 4.0 is not installed.");
		return NULL;
	}

	HWND hWnd = TccDialogOpen();
	if(hWnd)
		return hWnd;

	if(NULL == pInfoProvider || NULL == pAdviseSink)
		return FALSE;

	CTccDlg *pdlg = new CTccDlg();
	pdlg->SetInterface(pAdviseSink, pInfoProvider);
	if( pdlg->Create(pParent) )
		pdlg->ShowWindow(SW_SHOW);

	return GetTccDialogWndHandle();
}

KCL_MAP kcl_map[] =
{
	{"",   0},
	{"A",  1},
	{"B",  2},
	{"C",  3},
	{"D",  4},
	{"E",  5},
	{"F",  6},
	{"G",  7},
	{"H",  8},
	{"I",  9},
	{"J",  10},
	{"K",  11},
	{"L",  12},
	{"M",  13},
	{"N",  14},
	{"O",  15},
	{"P",  16},
	{"Q",  17},
	{"R",  18},
	{"S",  19},
	{"T",  20},
	{"U",  21},
	{"V",  22},
	{"W",  23},
	{"X",  24},
	{"Y",  25},
	{"Z",  26},
	{"AA", 27},
	{"AB", 28},
	{"AC", 29},
	{"AD", 30},
	{"AE", 31},
	{"AF", 32},
	{"AG", 33},
	{"AH", 34},
	{"AI", 35},
	{"AJ", 36},
	{"AK", 37},
	{"AL", 38},
	{"AM", 39},
	{"AN", 40},
	{"AO", 41},
	{"AP", 42},
	{"AQ", 43},
	{"AR", 44},
	{"AS", 45},
	{"AT", 46},
	{"AU", 47},
	{"AV", 48},
	{"AW", 49},
	{"AX", 50},
	{"AY", 51},
	{"AZ", 52},
	{NULL, 0},
};
KCL_MAP* GetKclMap()
{
	return kcl_map;
}

char StringtoKcl(CString str)
{
	char kcl = 0;
	for(KCL_MAP* p = GetKclMap(); p && p->szKcl != NULL; p++)
	{
		if(str == p->szKcl)
		{
			kcl = p->n;
			break;
		}
	}
	return kcl;
}

CString KcltoString(char kcl)
{
	CString szKcl;
	for(KCL_MAP* p = GetKclMap(); p && p->szKcl != NULL; p++)
	{
		if(kcl == p->n)
		{
			szKcl = p->szKcl;
			break;
		}
	}
	return szKcl;
}

CString kmmark_t::GetKcl() const
{
	return KcltoString(info.km_cl);
}

CString kmmark_t::GetMcl() const
{
	CString szMcl;
	if(info.m_cl)
		szMcl = "a";
	return szMcl;
}

CString kmmark_t::toString(ITccClientInfoProvider *pInfoProvider, int builtin_id) const
{
	CString str, szPrex;
	CString szKcl = GetKcl();
	CString szMcl = GetMcl();

	if(0 >= builtin_id)
		str.Format("K%d%s+%d%s", info.val /1000, szKcl, info.val %1000, szMcl);
	else if(pInfoProvider->GetKmsysPrex(builtin_id, *this, szPrex))
	{
		str.Format("%s%d%s+%d%s", szPrex, info.val /1000, szKcl, info.val %1000, szMcl);
	}
	else if(info.val == 0 || info.val == 9999999)
	{
		str.Format("K%d+%d", info.val /1000, info.val %1000);
	}
	else
		str.Format("E(%d)%d%s+%d%s", flag, info.val /1000, szKcl, info.val %1000, szMcl);
	
	return str;
}

bool operator==(sysid_t& lhs, sysid_t& rhs)
{
	RPC_STATUS status;
	return 0 == UuidCompare(&lhs, &rhs, &status);
}
bool operator!=(sysid_t& lhs, sysid_t& rhs)
{
	return !operator==(lhs, rhs);
}
void SysidCreate(sysid_t *sysid)
{
	UuidCreate(sysid);
}
void SysidCreateNil(sysid_t *sysid)
{
	UuidCreateNil(sysid);
}

INT SysidToByteArray(const sysid_t& sysid, BYTE arr[], INT num)
{
	ASSERT(num >= 16);
	if(num < 16)
		return 0;
	BYTE *p = (BYTE*)arr;
	memcpy(p, &sysid.Data1, sizeof(sysid.Data1));
	p += sizeof(sysid.Data1);
	
	memcpy(p, &sysid.Data2, sizeof(sysid.Data2));
	p += sizeof(sysid.Data2);

	memcpy(p, &sysid.Data3, sizeof(sysid.Data3));
	p += sizeof(sysid.Data3);

	memcpy(p, &sysid.Data4, sizeof(sysid.Data4));
	return 16;
}

INT SysidFromByteArray(sysid_t& sysid,const BYTE arr[], INT num)
{
	ASSERT(num >= 16);
	if(num < 16)
		return 0;
	BYTE *p = (BYTE*)arr;
	memcpy(&sysid.Data1, p, sizeof(sysid.Data1));
	p += sizeof(sysid.Data1);
	
	memcpy(&sysid.Data2, p, sizeof(sysid.Data2));
	p += sizeof(sysid.Data2);

	memcpy(&sysid.Data3, p, sizeof(sysid.Data3));
	p += sizeof(sysid.Data3);

	memcpy(&sysid.Data4, p, sizeof(sysid.Data4));
	return 16;
}

CString SysidToString(sysid_t sysid)
{
	unsigned char* StringUuid;
	CString str;
	if(RPC_S_OK == UuidToStringA(&sysid,&StringUuid))
		str = StringUuid;
	RpcStringFreeA(&StringUuid);
	return str;
}

sysid_t SysidFromString(CString str)
{
	sysid_t sysid;
	unsigned char* p = (unsigned char*)(LPCSTR)str;
	UuidFromStringA(p, &sysid);
	return sysid;
}

TCC_COMMAND::TCC_COMMAND()
{
	nConsoleID = 0;				//调度台 台号
	SysidCreateNil(&sys_id);	//内部标识
	nCmdID = 0;					//命令号
	nStaID[0] = nStaID[1] = 0;	//开始、结束站
	nLineID = 0;				//线路
	nSpeedMaxVal = GetDefaultLimitSpeed();			//限速值

	bFullLimit = FALSE;			//是否全线限速
	nKmMark[0] = nKmMark[1] = 0;//始末公里标(以米为单位)
	bStartNow = TRUE;			//是否立即开始
	bEndless = TRUE;			//是否永久有效
	nTime[0] = nTime[1] = 0;	//开始、结束时间

	bCancelLimit = FALSE;		//是否取消命令

	nTccID = 0;			//受令站
	nTccLineID = 0;
	nTccSenderID = 0;	//受令站发令人
	//nTccKmMark[0] = nTccKmMark[1] = 0;//受令站公里标范围
	nTccKmMarkFlag[0] = nTccKmMarkFlag[1] = 0;
}

bool TCC_COMMAND::operator==(const TCC_COMMAND& rhs)
{
	if(nCmdID != rhs.nCmdID
		|| nTccID != rhs.nTccID
		|| nTccLineID != rhs.nTccLineID
		|| nSpeedMaxVal != rhs.nSpeedMaxVal
		|| nTccKmMarkFlag[0] != rhs.nTccKmMarkFlag[0]
		|| nTccKmMarkFlag[1] != rhs.nTccKmMarkFlag[1]
		|| bStartNow != rhs.bStartNow
		|| bEndless != rhs.bEndless
		|| nTime[0] != rhs.nTime[0]
		|| nTime[1] != rhs.nTime[1])
		return FALSE;

	if(nConsoleID != rhs.nConsoleID
		|| sys_id != rhs.sys_id
		|| nStaID[0] != rhs.nStaID[0]
		|| nStaID[1] != rhs.nStaID[1]
		|| nLineID != rhs.nLineID
		|| nKmMark[0].Unequal(rhs.nKmMark[0])
		|| nKmMark[1].Unequal(rhs.nKmMark[1])
		|| bCancelLimit != rhs.bCancelLimit
		|| bFullLimit != rhs.bFullLimit
		//|| nTccSenderID != rhs.nTccSenderID
		)
		return FALSE;

	return TRUE;
}

bool TCC_COMMAND::operator!=(const TCC_COMMAND& rhs)
{
	return !operator==(rhs);
}

void TCC_COMMAND::ComposeRawStcokCommand(RAW_STOCK_COMMAND &raw_cmd)
{
	raw_cmd.nConsoleID = nConsoleID;				//调度台 台号
	raw_cmd.raw_sysid = sys_id;					//命令标识
	raw_cmd.nCmdID = nCmdID;			//命令号
	raw_cmd.nStaID[0] = nStaID[0];		//开始、结束站
	raw_cmd.nStaID[1] = nStaID[1];
	raw_cmd.nLineID = nLineID;			//线路

	raw_cmd.bFullLimit = bFullLimit;	//是否全线限速
	raw_cmd.nKmMark[0] = nKmMark[0];		//始末公里标(以米为单位)
	raw_cmd.nKmMark[1] = nKmMark[1];

	raw_cmd.bStartNow = bStartNow;		//是否立即开始
	raw_cmd.bEndless = bEndless;		//是否永久有效
	raw_cmd.nTime[0] = nTime[0];		//开始、结束时间
	raw_cmd.nTime[1] = nTime[1];

	raw_cmd.bCancelLimit = bCancelLimit;	//是否取消命令

	raw_cmd.tcc.resize(1);//受令站数目
	raw_cmd.tcc[0].nID = nTccID;
	raw_cmd.tcc[0].nTccLineID = nTccLineID;
	raw_cmd.tcc[0].nKmMarkFlag[0] = nTccKmMarkFlag[0];
	raw_cmd.tcc[0].nKmMarkFlag[1] = nTccKmMarkFlag[1];

	raw_cmd.train_cmd[0].nSpeedMaxVal = nSpeedMaxVal;
	raw_cmd.train_cmd[0].nSpeed = nSpeedMaxVal;
	for(int i=0; i<sizeof(raw_cmd.train_cmd)/sizeof(raw_cmd.train_cmd[0]); i++)
		raw_cmd.train_cmd[i].tcc.resize(1);
}

CString TCC_COMMAND::toString(ITccClientInfoProvider *pInfoProvider)
{
	TCC_COMMAND& cmd = *this;

		CString str, sz;
		
		sz.Format("调度台台号: %d", cmd.nConsoleID);
		str += sz;
		
		sz.Format("\r\n系统标识: %s", SysidToString(cmd.sys_id));
		str += sz;

		sz.Format("\r\n命令号: %06d", cmd.nCmdID);
		str += sz;

		sz.Format("\r\n开始站: %s(%d)",
			pInfoProvider->GetStationName(cmd.nStaID[0]), cmd.nStaID[0]);
		str += sz;
		sz.Format("\r\n结束站: %s(%d)",
			pInfoProvider->GetStationName(cmd.nStaID[1]), cmd.nStaID[1]);
		str += sz;

		if(cmd.nLineID)
		{
			sz.Format("\r\n线路: %s(builtin_id=%d)",
				pInfoProvider->GetLineName(cmd.nLineID), cmd.nLineID);
		}
		else
		{
			sz.Format("\r\n线路: %s(侧线)(tccline_id=%d)",
				pInfoProvider->GetTccLineName(cmd.nTccID, cmd.nTccLineID), cmd.nTccLineID);
		}
		str += sz;
		
		sz.Format("\r\n公里标: %s----%s", 
			cmd.nKmMark[0].toString(pInfoProvider, cmd.nLineID), 
			cmd.nKmMark[1].toString(pInfoProvider, cmd.nLineID));
		str += sz;
		if(cmd.bFullLimit)
			str += "(全限速)";

		if(cmd.bCancelLimit)
		{
			sz.Format("\r\n限速值: 取消限速");
			str += sz;
		}
		else
		{
			sz.Format("\r\n限速值: %dKm/h", cmd.nSpeedMaxVal);
			str += sz;
		}

		str += "\r\n时间: ";
		if(cmd.bStartNow || cmd.nTime[0] <= 0)
			str += "立即开始 ";
		else
		{
			CTime ct(cmd.nTime[0]);
			sz.Format("%04d年%02d月%02d日 %02d:%02d:%02d", 
				ct.GetYear(), ct.GetMonth(), ct.GetDay(),
				ct.GetHour(), ct.GetMinute(),ct.GetSecond());
			str += sz;
		}
		str += "---- ";
		if(cmd.bEndless || cmd.nTime[1] <= 0)
			str += "永久有效";
		else
		{
			CTime ct(cmd.nTime[1]);
			sz.Format("%04d年%02d月%02d日 %02d:%02d:%02d", 
				ct.GetYear(), ct.GetMonth(), ct.GetDay(),
				ct.GetHour(), ct.GetMinute(),ct.GetSecond());
			str += sz;
		}

		sz.Format("\r\n受令站: %s(%d)"
			"\r\n受令站公里标: %d 至 %d"
			"\r\n受令站线路号: %s(tccline_id=%d)", 
			pInfoProvider->GetStationName(cmd.nTccID), cmd.nTccID,
			cmd.nTccKmMarkFlag[0], cmd.nTccKmMarkFlag[1],
			pInfoProvider->GetTccLineName(cmd.nTccID, cmd.nTccLineID), cmd.nTccLineID);
		str += sz;

	return str;
}



STOCK_COMMAND::STOCK_COMMAND()
{
	dcmd_send_entity = 0;	//调度命令索引
	dcmd_create_time = 0;

	nConsoleID = 0;				//调度台 台号
	SysidCreateNil(&sys_id);	//内部标识
	nCmdID = 0;					//命令号
	nStaID[0] = nStaID[1] = 0;	//开始、结束站
	nLineID = 0;				//线路
	nSpeedMaxVal = GetDefaultLimitSpeed();			//限速值

	bFullLimit = FALSE;			//是否全线限速
	nKmMark[0] = nKmMark[1] = 0;//始末公里标(以米为单位)
	bStartNow = TRUE;			//是否立即开始
	bEndless  = TRUE;			//是否永久有效
	nTime[0]  = nTime[1] = 0;	//开始、结束时间

	bCancelLimit = FALSE;		//是否取消命令

	nCreatorID = 0;				//草拟者
	nCheckorID = 0;				//中心校验者
	nConfirmorID = 0;			//确认分解者
	SysidCreateNil(&raw_sysid);	//原始命令标识
	nCreateTime = 0;	//命令创建时间
}

bool STOCK_COMMAND::operator==(const STOCK_COMMAND& rhs)
{
	if(dcmd_id != rhs.dcmd_id)
		return FALSE;

	if(nConsoleID != rhs.nConsoleID
		|| sys_id != rhs.sys_id
		|| nCmdID != rhs.nCmdID
		|| nStaID[0] != rhs.nStaID[0]
		|| nStaID[1] != rhs.nStaID[1]
		|| nLineID != rhs.nLineID
		|| nSpeedMaxVal != rhs.nSpeedMaxVal
		|| bCancelLimit != rhs.bCancelLimit)
		return FALSE;

	if(bFullLimit != rhs.bFullLimit
		|| nKmMark[0].Unequal(rhs.nKmMark[0])
		|| nKmMark[1].Unequal(rhs.nKmMark[1])
		|| bStartNow != rhs.bStartNow
		|| bEndless  != rhs.bEndless
		|| nTime[0]  != rhs.nTime[0]
		|| nTime[1] != rhs.nTime[1])
		return FALSE;

	if(nCreatorID != rhs.nCreatorID
		|| nCheckorID != rhs.nCheckorID
		|| nConfirmorID != rhs.nConfirmorID
		|| raw_sysid != rhs.raw_sysid)
		return FALSE;

	if(tcc.size() != rhs.tcc.size())
		return FALSE;
	for(int i=0; i<tcc.size(); i++)
	{
		const _TCC_STRUCT& tcc_lhs = tcc[i];
		for(int j=0; j<rhs.tcc.size() && tcc_lhs.nID != rhs.tcc[j].nID; j++);
		if(j>=tcc.size())
			break;
		const  _TCC_STRUCT& tcc_rhs = rhs.tcc[j];
		if(tcc_lhs.nTccLineID != tcc_rhs.nTccLineID
			|| tcc_lhs.nCheckorID != tcc_rhs.nCheckorID
			|| tcc_lhs.nSenderID != tcc_rhs.nSenderID
			|| tcc_lhs.nStatus != tcc_rhs.nStatus
			|| tcc_lhs.nTime != tcc_rhs.nTime
			|| tcc_lhs.nKmMarkFlag[0] != tcc_rhs.nKmMarkFlag[0]
			|| tcc_lhs.nKmMarkFlag[1] != tcc_rhs.nKmMarkFlag[1])
			break;
	}
	if(i != tcc.size())
		return FALSE;

	return TRUE;
}

bool STOCK_COMMAND::operator!=(const STOCK_COMMAND& rhs)
{
	return !operator==(rhs);
}

WORD STOCK_COMMAND::GetTccStatus(stano_t nTccID)
{
	for(int i=0; i<tcc.size(); i++)
	{
		if(tcc[i].nID == nTccID)
			return tcc[i].nStatus;
	}
	return 0;
}

DWORD STOCK_COMMAND::GetTccSender(stano_t nTccID)
{
	for(int i=0; i<tcc.size(); i++)
	{
		if(tcc[i].nID == nTccID)
			return tcc[i].nSenderID;
	}
	return 0;
}

BOOL STOCK_COMMAND::GetTccCommand(TCC_COMMAND *pcmd, stano_t nTccID)
{
	ASSERT(pcmd);

	for(int i=0; i<tcc.size(); i++)
	{
		if(tcc[i].nID == nTccID)
			break;
	}
	if(i>=tcc.size())
		return FALSE;

	pcmd->nConsoleID = nConsoleID;
	pcmd->sys_id = sys_id;
	pcmd->nCmdID = nCmdID;
	pcmd->nStaID[0] = nStaID[0];
	pcmd->nStaID[1] = nStaID[1];
	pcmd->nLineID= nLineID;
	pcmd->nSpeedMaxVal = nSpeedMaxVal;
	pcmd->bFullLimit = bFullLimit;
	pcmd->nKmMark[0] = nKmMark[0];
	pcmd->nKmMark[1] = nKmMark[1];
	pcmd->bStartNow = bStartNow;
	pcmd->bEndless = bEndless;
	pcmd->nTime[0] = nTime[0];
	pcmd->nTime[1] = nTime[1];
	pcmd->bCancelLimit = bCancelLimit;
	
	pcmd->nTccID = tcc[i].nID;
	pcmd->nTccLineID = tcc[i].nTccLineID;
	pcmd->nTccSenderID = tcc[i].nSenderID;
	pcmd->nTccKmMarkFlag[0] = tcc[i].nKmMarkFlag[0];
	pcmd->nTccKmMarkFlag[1] = tcc[i].nKmMarkFlag[1];

	return TRUE;
}

INT  STOCK_COMMAND::Decompose(TCC_COMMAND cmd[], INT num)
{
	INT nCmdNum = 0;
	for(int i=0; i<tcc.size() && i<num; i++)
	{
		TCC_COMMAND *pcmd = &cmd[nCmdNum++];

		pcmd->nConsoleID = nConsoleID;
		pcmd->sys_id = sys_id;
		pcmd->nCmdID = nCmdID;
		pcmd->nStaID[0] = nStaID[0];
		pcmd->nStaID[1] = nStaID[1];
		pcmd->nLineID= nLineID;
		pcmd->nSpeedMaxVal = nSpeedMaxVal;
		pcmd->bFullLimit = bFullLimit;
		pcmd->nKmMark[0] = nKmMark[0];
		pcmd->nKmMark[1] = nKmMark[1];
		pcmd->bStartNow = bStartNow;
		pcmd->bEndless = bEndless;
		pcmd->nTime[0] = nTime[0];
		pcmd->nTime[1] = nTime[1];
		pcmd->bCancelLimit = bCancelLimit;
		
		pcmd->nTccID = tcc[i].nID;
		pcmd->nTccLineID = tcc[i].nTccLineID;
		pcmd->nTccSenderID = tcc[i].nSenderID;
		pcmd->nTccKmMarkFlag[0] = tcc[i].nKmMarkFlag[0];
		pcmd->nTccKmMarkFlag[1] = tcc[i].nKmMarkFlag[1];
	}

	return nCmdNum;
}

void STOCK_COMMAND::ComposeRawStcokCommand(RAW_STOCK_COMMAND &raw_cmd)
{
	raw_cmd.dcmd_id = dcmd_id;

	raw_cmd.nConsoleID = nConsoleID;				//调度台 台号
	raw_cmd.raw_sysid = raw_sysid;					//原始命令标识
	raw_cmd.nCmdID = nCmdID;			//命令号
	raw_cmd.nStaID[0] = nStaID[0];		//开始、结束站
	raw_cmd.nStaID[1] = nStaID[1];
	raw_cmd.nLineID = nLineID;			//线路

	raw_cmd.bFullLimit = bFullLimit;	//是否全线限速
	raw_cmd.nKmMark[0] = nKmMark[0];		//始末公里标(以米为单位)
	raw_cmd.nKmMark[1] = nKmMark[1];

	raw_cmd.bStartNow = bStartNow;		//是否立即开始
	raw_cmd.bEndless = bEndless;		//是否永久有效
	raw_cmd.nTime[0] = nTime[0];		//开始、结束时间
	raw_cmd.nTime[1] = nTime[1];

	raw_cmd.bCancelLimit = bCancelLimit;	//是否取消命令

	raw_cmd.tcc.resize(tcc.size());//受令站数目
	for(int i=0; i<tcc.size(); i++)
	{
		raw_cmd.tcc[i].nID =  tcc[i].nID;
		raw_cmd.tcc[i].nTccLineID = tcc[i].nTccLineID;
//		raw_cmd.tcc[i].nCheckorID = tcc[i].nCheckorID;
		raw_cmd.tcc[i].nKmMarkFlag[0] = tcc[i].nKmMarkFlag[0];
		raw_cmd.tcc[i].nKmMarkFlag[1] = tcc[i].nKmMarkFlag[1];
	}

	raw_cmd.train_cmd[0].nSpeedMaxVal = nSpeedMaxVal;
	raw_cmd.train_cmd[0].nSpeed = nSpeedMaxVal;
	for(i=0; i<sizeof(raw_cmd.train_cmd)/sizeof(raw_cmd.train_cmd[0]); i++)
		raw_cmd.train_cmd[i].tcc.resize(tcc.size());
}

CString STOCK_COMMAND::toString(ITccClientInfoProvider *pInfoProvider)
{
	STOCK_COMMAND& cmd = *this;

		CString str, sz;
		
		sz.Format("调度命令号: %s", (const char*)cmd.dcmd_id);
		str += sz;

		sz.Format("\r\n命令号: %d", cmd.nCmdID);
		str += sz;

		sz.Format("\r\n开始站: %s(%d)",
			pInfoProvider->GetStationName(cmd.nStaID[0]), cmd.nStaID[0]);
		str += sz;
		sz.Format("\r\n结束站: %s(%d) ",
			pInfoProvider->GetStationName(cmd.nStaID[1]), cmd.nStaID[1]);
		str += sz;

		if(cmd.nLineID)
		{
			sz.Format("\r\n线路: %s(builtin_id=%d)",
				pInfoProvider->GetLineName(cmd.nLineID), cmd.nLineID);
		}
		else
		{
			sz.Format("\r\n线路: %s(侧线)(tccline_id=%d)",
				pInfoProvider->GetTccLineName(cmd.tcc[0].nID, cmd.tcc[0].nTccLineID), cmd.tcc[0].nTccLineID);
		}
		str += sz;
		
		sz.Format("\r\n公里标: %s----%s", 
			cmd.nKmMark[0].toString(pInfoProvider, cmd.nLineID),
			cmd.nKmMark[1].toString(pInfoProvider, cmd.nLineID));
		str += sz;
		if(cmd.bFullLimit)
			str += "(全限速)";

		if(cmd.bCancelLimit)
		{
			sz.Format("\r\n限速值: 取消限速(原限速%dKm/h)", cmd.nSpeedMaxVal);
			str += sz;
		}
		else
		{
			sz.Format("\r\n限速值: %dKm/h", cmd.nSpeedMaxVal);
			str += sz;
		}

		CTime ct;
		str += "\r\n时间: ";
		if(cmd.bStartNow || cmd.nTime[0] <= 0)
			str += "立即开始 ";
		else
		{
			ct = CTime(cmd.nTime[0]);
			str += ct.Format("%Y年%m月%d日 %H:%M:%S");
		}
		str += "---- ";
		if(cmd.bEndless || cmd.nTime[1] <= 0)
			str += "永久有效";
		else
		{
			ct = CTime(cmd.nTime[1]);
			str += ct.Format("%Y年%m月%d日 %H:%M:%S");
		}

		str += "\r\n受令站列表: ";
		for(int i=0; i<cmd.tcc.size(); i++)
		{
			sz.Format("\r\n    %s(%d), %d 至 %d, 线路=%s(tccline_id=%d), 状态=%s(%d)", 
				pInfoProvider->GetStationName(cmd.tcc[i].nID), cmd.tcc[i].nID,
				cmd.tcc[i].nKmMarkFlag[0], cmd.tcc[i].nKmMarkFlag[1],
				pInfoProvider->GetTccLineName(cmd.tcc[i].nID, cmd.tcc[i].nTccLineID), cmd.tcc[i].nTccLineID,
				GetStockCommandStatusString(cmd.tcc[i].nStatus), cmd.tcc[i].nStatus);
			str += sz;

			if(cmd.tcc[i].nTime <= 0)
				str += "(执行时间 0)";
			else
			{
				ct = CTime(cmd.tcc[i].nTime);
				str += ct.Format("(执行时间 %Y年%m月%d日 %H:%M:%S)");
			}
		}

		str += "\r\n";
		
		sz.Format("\r\n调度台台号: %d", cmd.nConsoleID);
		str += sz;

		// 生成时间
		if(cmd.nCreateTime <= 0)
			sz.Format("\r\n命令生成时间: 0");
		else
		{
			ct = cmd.nCreateTime;
			sz = ct.Format("\r\n命令生成时间: %Y年%m月%d日 %H:%M");
		}
		str += sz;
		
		sz.Format("\r\n系统标识: %s", SysidToString(cmd.sys_id));
		str += sz;

	return str;
}

RAW_STOCK_COMMAND::RAW_STOCK_COMMAND()
{
	nCmdType = 0;
	nDirection = 0;

	nConsoleID = 0;				//调度台 台号
	SysidCreateNil(&raw_sysid);	//原始命令标识
	nCmdID = 0;					//命令号
	nStaID[0] = nStaID[1] = 0;	//开始、结束站
	nLineID= 0;				//线路

	bFullLimit= FALSE;			//是否全线限速
	nKmMark[0]= nKmMark[1] = 0;	//始末公里标(以米为单位)
	bStartNow = TRUE;			//是否立即开始
	bEndless  = TRUE;			//是否永久有效
	nTime[0]  = nTime[1] = 0;	//开始、结束时间

	bCancelLimit = FALSE;		//是否取消命令

	nCreatorID = 0;			//草拟者
	nCheckorID = 0;			//中心校验者
	nConfirmorID = 0;		//确认分解者
	nCreateTime = 0;	//命令创建时间

	for(int i=0; i<sizeof(train_cmd)/sizeof(train_cmd[0]); i++)
	{
		SysidCreateNil(&train_cmd[i].sys_id);		//每趟车一个命令，一个系统标识
		train_cmd[i].szTrainID;	//车次号
		train_cmd[i].nSpeedMaxVal = (i==0 ? GetDefaultLimitSpeed() : 0);		//限速值
		train_cmd[i].nSpeed = 0;
	}

	bGenCancelCommand = FALSE;
}
RAW_STOCK_COMMAND::_TRAINCMD::_TRAINCMD()
{
	SysidCreateNil(&sys_id);
	nSpeedMaxVal = nSpeed = 0;
}

RAW_STOCK_COMMAND& RAW_STOCK_COMMAND::operator =(const RAW_STOCK_COMMAND& rhs)
{
	dcmd_id = rhs.dcmd_id;

	nCmdType = rhs.nCmdType;
	nDirection = rhs.nDirection;

	nConsoleID = rhs.nConsoleID;				//调度台 台号
	raw_sysid = rhs.raw_sysid;	//原始命令标识
	nCmdID = rhs.nCmdID;					//命令号
	nStaID[0] = rhs.nStaID[0];
	nStaID[1] = rhs.nStaID[1];	//开始、结束站
	nLineID = rhs.nLineID;				//线路

	bFullLimit = rhs.bFullLimit;			//是否全线限速
	nKmMark[0] = rhs.nKmMark[0];
	nKmMark[1] = rhs.nKmMark[1];	//始末公里标(以米为单位)
	bStartNow  = rhs.bStartNow;			//是否立即开始
	bEndless   = rhs.bEndless;			//是否永久有效
	nTime[0]   = rhs.nTime[0];
	nTime[1]   = rhs.nTime[1];	//开始、结束时间

	bCancelLimit = rhs.bCancelLimit;		//是否取消命令

	nCreatorID = rhs.nCreatorID;			//草拟者
	nCheckorID = rhs.nCheckorID;			//中心校验者
	nConfirmorID = rhs.nConfirmorID;		//确认分解者
	nCreateTime = rhs.nCreateTime;

	tcc = rhs.tcc;	//受令站列表

	for(int i=0; i<sizeof(train_cmd)/sizeof(train_cmd[0]); i++)
	{
		train_cmd[i].sys_id = rhs.train_cmd[i].sys_id;		//每趟车一个命令，一个系统标识
		train_cmd[i].szTrainID = rhs.train_cmd[i].szTrainID;	//车次号
		train_cmd[i].nSpeedMaxVal = rhs.train_cmd[i].nSpeedMaxVal;		//限速值
		train_cmd[i].nSpeed = rhs.train_cmd[i].nSpeed;

		train_cmd[i].tcc = rhs.train_cmd[i].tcc;	//受令站命令状态列表
	}

	bGenCancelCommand = rhs.bGenCancelCommand;

	return *this;
}

bool RAW_STOCK_COMMAND::operator ==(const RAW_STOCK_COMMAND& rhs)
{
	if(this == &rhs)
		return true;

	bool bRet = false;
	do {
		if(dcmd_id != rhs.dcmd_id)
			break;

		if(nConsoleID != rhs.nConsoleID
			|| nCmdID != rhs.nCmdID
			|| bFullLimit != rhs.bFullLimit
			|| nStaID[0] != rhs.nStaID[0]
			|| nStaID[1] != rhs.nStaID[1]
			|| nLineID != rhs.nLineID
			|| nKmMark[0].Unequal(rhs.nKmMark[0])
			|| nKmMark[1].Unequal(rhs.nKmMark[1])
			|| bCancelLimit != rhs.bCancelLimit
			)
			break;

		if(bStartNow ^ rhs.bStartNow)
			break;
		else if(nTime[0] != rhs.nTime[0])
			break;

		if(bEndless ^ rhs.bEndless)
			break;
		else if(nTime[1] != rhs.nTime[1])
			break;

		if(tcc.size() != rhs.tcc.size())
			break;
		for(int i=0; i<tcc.size(); i++)
		{
			const _TCC_STRUCT& tcc_lhs = tcc[i];
			for(int j=0; j<rhs.tcc.size() && tcc_lhs.nID != rhs.tcc[j].nID; j++);
			if(j>=tcc.size())
				break;
			const  _TCC_STRUCT& tcc_rhs = rhs.tcc[j];
			if(tcc_lhs.nTccLineID != tcc_rhs.nTccLineID
				|| tcc_lhs.nKmMarkFlag[0] != tcc_rhs.nKmMarkFlag[0]
				|| tcc_lhs.nKmMarkFlag[1] != tcc_rhs.nKmMarkFlag[1]
				|| tcc_lhs.bTccValidate != tcc_rhs.bTccValidate)
				break;
		}
		if(i != tcc.size())
			break;

		//取消限速不比较每趟限速
		if(!bCancelLimit)
		{
			for(i=0; i<sizeof(train_cmd)/sizeof(train_cmd[0]); i++)
			{
				if(train_cmd[i].nSpeedMaxVal != rhs.train_cmd[i].nSpeedMaxVal
					|| train_cmd[i].nSpeed != rhs.train_cmd[i].nSpeed
					|| train_cmd[i].szTrainID != rhs.train_cmd[i].szTrainID)
					break;
			}
			if(i != sizeof(train_cmd)/sizeof(train_cmd[0]))
				break;
		}

		if(bGenCancelCommand != rhs.bGenCancelCommand)
			break;

		bRet = true;
	} while(0);

	return bRet;
}

CString GetDifferenceReason(RAW_STOCK_COMMAND& lhs, RAW_STOCK_COMMAND& rhs, ITccClientInfoProvider *pInfoProvider)
{
	CString szReason;
	CString str;

	do {
		if (lhs.dcmd_id != rhs.dcmd_id)
			szReason += "调度命令号不一致\n";

		if (lhs.nConsoleID != rhs.nConsoleID)
			szReason += "调度台号不一致\n";

		if (lhs.nCmdID != rhs.nCmdID)
			szReason += "限速命令号不一致\n";
		
		if (lhs.bCancelLimit != rhs.bCancelLimit)
			szReason += "取消标志不一致\n";

		if (lhs.bFullLimit != rhs.bFullLimit)
			szReason += "全限速标志不一致\n";

		if (lhs.nStaID[0] != rhs.nStaID[0])
			szReason += "开始站不一致\n";

		if (lhs.nStaID[1] != rhs.nStaID[1])
			szReason += "结束不一致\n";

		if (lhs.nLineID != rhs.nLineID)
			szReason += "线路号不一致\n";

		if (lhs.nKmMark[0].Unequal(rhs.nKmMark[0]))
			szReason += "开始公里标不一致\n";

		if (lhs.nKmMark[1].Unequal(rhs.nKmMark[1]))
			szReason += "结束公里标不一致\n";
			//|| bCancelLimit != rhs.bCancelLimit

		if (lhs.bStartNow ^ rhs.bStartNow)
			szReason += "立即开始标志不一致\n";
		else if(lhs.nTime[0] != rhs.nTime[0])
			szReason += "开始时间不一致\n";

		if (lhs.bEndless ^ rhs.bEndless)
			szReason += "永久有效标志不一致\n";
		else if(lhs.nTime[1] != rhs.nTime[1])
			szReason += "结束时间不一致\n";

		if (lhs.tcc.size() != rhs.tcc.size())
			szReason += "受令站列表数目不一致\n";
		else
		{
			for(int i=0; i<lhs.tcc.size(); i++)
			{
				const RAW_STOCK_COMMAND::_TCC_STRUCT& tcc_lhs = lhs.tcc[i];
				for(int j=0; j<rhs.tcc.size() && tcc_lhs.nID != rhs.tcc[j].nID; j++);
				if(j>=rhs.tcc.size())
				{
					str.Format("受令站%s(%d)的站号不一致\r\n", 
						pInfoProvider->GetStationName(tcc_lhs.nID), tcc_lhs.nID);
					szReason += str;
					break;
				}
				const RAW_STOCK_COMMAND::_TCC_STRUCT& tcc_rhs = rhs.tcc[j];
				if(tcc_lhs.nTccLineID != tcc_rhs.nTccLineID)
				{
					str.Format("受令站%s(%d)的线路号不一致\r\n", 
						pInfoProvider->GetStationName(tcc_lhs.nID), tcc_lhs.nID);
					szReason += str;
					break;
				}
				if(tcc_lhs.nKmMarkFlag[0] != tcc_rhs.nKmMarkFlag[0])
				{
					str.Format("受令站%s(%d)的开始公里标不一致",
						pInfoProvider->GetStationName(tcc_lhs.nID), tcc_lhs.nID);
					szReason += str;
					break;
				}
				if(tcc_lhs.nKmMarkFlag[1] != tcc_rhs.nKmMarkFlag[1])
				{
					str.Format("受令站%s(%d)的结束公里标不一致", 
						pInfoProvider->GetStationName(tcc_lhs.nID), tcc_lhs.nID);
					szReason += str;
					break;
				}
				if(tcc_lhs.bTccValidate != tcc_rhs.bTccValidate)
				{
					str.Format("受令站%s(%d)的校验标志不一致\r\n", 
						pInfoProvider->GetStationName(tcc_lhs.nID), tcc_lhs.nID);
					szReason += str;
					break;
				}
			}
		}
		//非取消命令比较每趟车次
		if(!lhs.bCancelLimit || !rhs.bCancelLimit )
		{
			for(int i=0; i<sizeof(lhs.train_cmd)/sizeof(lhs.train_cmd[0]); i++)
			{
				if(lhs.train_cmd[i].nSpeedMaxVal != rhs.train_cmd[i].nSpeedMaxVal
					|| lhs.train_cmd[i].nSpeed != rhs.train_cmd[i].nSpeed
					|| lhs.train_cmd[i].szTrainID != rhs.train_cmd[i].szTrainID)
					break;
			}
			if(i != sizeof(lhs.train_cmd)/sizeof(lhs.train_cmd[0]))
			{
				CString prompt;
				prompt.Format("第%d趟限速不一致!\n", i+1);
				szReason += prompt;
			}
		}

		if(lhs.bGenCancelCommand != rhs.bGenCancelCommand)
			szReason += "是否取消命令标志不一致\n";
	} while(0);

	return szReason;
}

bool RAW_STOCK_COMMAND::operator !=(const RAW_STOCK_COMMAND& rhs)
{
	return !operator==(rhs);
}

BOOL RAW_STOCK_COMMAND::SetStatus(sysid_t& sysid, stano_t nTccID, WORD nStatus)
{
	BOOL bRet = FALSE;
	for(int i=0; i<sizeof(train_cmd)/sizeof(train_cmd[0]); i++)
	{
		if(train_cmd[i].sys_id != sysid)
			continue;

		for(int j=0; j<tcc.size(); j++)
		{
			if(tcc[j].nID != nTccID)
				continue;

			if(train_cmd[i].tcc[j].nStatus != nStatus)
			{
				train_cmd[i].tcc[j].nStatus = nStatus;
				bRet = TRUE;
			}
			break;
		}
		break;
	}
	return bRet;
}

BOOL RAW_STOCK_COMMAND::SetSender(sysid_t& sysid, stano_t nTccID, DWORD nSenderID)
{
	BOOL bRet = FALSE;
	for(int i=0; i<sizeof(train_cmd)/sizeof(train_cmd[0]); i++)
	{
		if(train_cmd[i].sys_id != sysid)
			continue;

		for(int j=0; j<tcc.size(); j++)
		{
			if(tcc[j].nID != nTccID)
				continue;

			if(train_cmd[i].tcc[j].nSenderID != nSenderID)
			{
				train_cmd[i].tcc[j].nSenderID = nSenderID;
				bRet = TRUE;
			}
			break;
		}
		break;
	}
	return bRet;
}

BOOL RAW_STOCK_COMMAND::SetTccCheckor(stano_t nTccID, DWORD nCheckor)
{
	BOOL bRet = FALSE;

	for(int j=0; j<tcc.size(); j++)
	{
		if(tcc[j].nID != nTccID)
			continue;

		if(tcc[j].nCheckorID != nCheckorID)
		{
			tcc[j].nCheckorID = nCheckorID;
			bRet = TRUE;
		}
		break;
	}

	return bRet;
}

BOOL RAW_STOCK_COMMAND::SetTime(sysid_t& sysid, stano_t nTccID, DWORD nTime)
{
	BOOL bRet = FALSE;
	for(int i=0; i<sizeof(train_cmd)/sizeof(train_cmd[0]); i++)
	{
		if(train_cmd[i].sys_id != sysid)
			continue;

		for(int j=0; j<tcc.size(); j++)
		{
			if(tcc[j].nID != nTccID)
				continue;

			if(train_cmd[i].tcc[j].nTime != nTime)
			{
				train_cmd[i].tcc[j].nTime = nTime;
				bRet = TRUE;
			}
			break;
		}
		break;
	}
	return bRet;
}


INT RAW_STOCK_COMMAND::Decompose(STOCK_COMMAND cmd[], INT num)
{
	sysid_t null_sysid;
	SysidCreateNil(&null_sysid);
	INT nCmdNum = 0;
	for(int i=0; i<sizeof(train_cmd)/sizeof(train_cmd[0]) && nCmdNum<num; i++)
	{
		if(0 == train_cmd[i].nSpeedMaxVal && !bCancelLimit)
			continue;

		STOCK_COMMAND& stock_cmd = cmd[nCmdNum++];

		stock_cmd.dcmd_id = dcmd_id;

		stock_cmd.nSpeedMaxVal = train_cmd[i].nSpeedMaxVal;
		stock_cmd.szTrainID = train_cmd[i].szTrainID;
		if(train_cmd[i].sys_id == null_sysid)
			SysidCreate(&train_cmd[i].sys_id);

		stock_cmd.nConsoleID = nConsoleID;
		stock_cmd.sys_id = train_cmd[i].sys_id;
		stock_cmd.nCmdID = nCmdID;
		
		stock_cmd.nStaID[0]  = nStaID[0];
		stock_cmd.nStaID[1]  = nStaID[1];
		stock_cmd.nLineID    = nLineID;
		stock_cmd.bFullLimit = bFullLimit;

		stock_cmd.nKmMark[0] = nKmMark[0];
		stock_cmd.nKmMark[1] = nKmMark[1];

		stock_cmd.bStartNow = bStartNow;
		stock_cmd.bEndless  = bEndless;

		stock_cmd.nTime[0] = nTime[0];
		stock_cmd.nTime[1] = nTime[1];

		stock_cmd.bCancelLimit = bCancelLimit;

		stock_cmd.nCreatorID = nCreatorID;
		stock_cmd.nCheckorID = nCheckorID;
		stock_cmd.nConfirmorID = nConfirmorID;
		stock_cmd.nCreateTime = nCreateTime;

		stock_cmd.raw_sysid = raw_sysid;
		
		// 受令站及其线路
		stock_cmd.tcc.resize(tcc.size());
		for(int j=0; j<stock_cmd.tcc.size(); j++)
		{
			stock_cmd.tcc[j].nID = tcc[j].nID;
			stock_cmd.tcc[j].nTccLineID = tcc[j].nTccLineID;
			stock_cmd.tcc[j].nCheckorID = tcc[j].nCheckorID;
			stock_cmd.tcc[j].nKmMarkFlag[0] = tcc[j].nKmMarkFlag[0];
			stock_cmd.tcc[j].nKmMarkFlag[1] = tcc[j].nKmMarkFlag[1];

			stock_cmd.tcc[j].nStatus = train_cmd[i].tcc[j].nStatus;
			stock_cmd.tcc[j].nSenderID = train_cmd[i].tcc[j].nSenderID;
			stock_cmd.tcc[j].nTime = train_cmd[i].tcc[j].nTime;
		}

		//取消命令只生成一条
		if(bCancelLimit)
			break;
	}

	// 正常限速命令 是否 自动生成取消限速命令
	if(!bCancelLimit && bGenCancelCommand && nCmdNum>0 && nCmdNum<num)
	{
		STOCK_COMMAND& stock_cmd = cmd[nCmdNum++];
		stock_cmd = cmd[0];
		stock_cmd.bCancelLimit = TRUE;
		SysidCreate(&stock_cmd.sys_id);
	}

	return nCmdNum;
}

CString RAW_STOCK_COMMAND::toString(ITccClientInfoProvider *pInfoProvider)
{
	RAW_STOCK_COMMAND& cmd = *this;
		CString str, sz;
		
		sz.Format("调度命令号: %s", (const char*)cmd.dcmd_id);
		str += sz;

		sz.Format("\r\n命令类型: %s", cmd.bCancelLimit ? "取消限速" : "正常限速");
		str += sz;

		sz.Format("\r\n命令号: %d", cmd.nCmdID);
		str += sz;

		sz.Format("\r\n开始站: %s(%d)",
			pInfoProvider->GetStationName(cmd.nStaID[0]), cmd.nStaID[0]);
		str += sz;
		sz.Format("\r\n结束站: %s(%d) ",
			pInfoProvider->GetStationName(cmd.nStaID[1]), cmd.nStaID[1]);
		str += sz;

		if(cmd.nLineID)
		{
			sz.Format("\r\n线路: %s(builtin_id=%d)",
				pInfoProvider->GetLineName(cmd.nLineID), cmd.nLineID);
		}
		else
		{
			sz.Format("\r\n线路: %s(侧线)(tccline_id=%d)",
				pInfoProvider->GetTccLineName(cmd.tcc[0].nID, cmd.tcc[0].nTccLineID), cmd.tcc[0].nTccLineID);
		}
		str += sz;
		
		sz.Format("\r\n公里标: %s----%s", 
			cmd.nKmMark[0].toString(pInfoProvider, cmd.nLineID),
			cmd.nKmMark[1].toString(pInfoProvider, cmd.nLineID));
		str += sz;
		if(cmd.bFullLimit)
			str += "(全限速)";

		CTime ct;
		str += "\r\n开始时间: ";
		if(cmd.bStartNow || cmd.nTime[0] <= 0)
			str += "立即开始 ";
		else
		{
			ct = CTime(cmd.nTime[0]);
			str += ct.Format("%Y年%m月%d日 %H:%M:%S");
		}
		str += "\r\n结束时间: ";
		if(cmd.bEndless || cmd.nTime[1] <= 0)
			str += "永久有效";
		else
		{
			ct = CTime(cmd.nTime[1]);
			str += ct.Format("%Y年%m月%d日 %H:%M:%S");
		}

		str += "\r\n受令站列表: ";
		for(int i=0; i<cmd.tcc.size(); i++)
		{
			sz.Format("\r\n    %s(%d), %d 至 %d, 线路=%s(tccline_id=%d), %s", 
				pInfoProvider->GetStationName(cmd.tcc[i].nID), cmd.tcc[i].nID,
				cmd.tcc[i].nKmMarkFlag[0], cmd.tcc[i].nKmMarkFlag[1],
				pInfoProvider->GetTccLineName(cmd.tcc[i].nID, cmd.tcc[i].nTccLineID), cmd.tcc[i].nTccLineID,
				(cmd.tcc[i].bTccValidate ? "需校验" : "不需校验")
				);
			str += sz;
		}		
		
		if(cmd.bCancelLimit)
		{
			sz.Format("\r\n限速值: 取消限速");
			str += sz;
		}
		else
		{
			for(i=0; i<sizeof(cmd.train_cmd)/sizeof(cmd.train_cmd[0]); i++)
			{
				if(cmd.train_cmd[i].nSpeedMaxVal <= 0)
					continue;

				sz.Format("\r\n第%d趟限速 系统标识%s", 
					i+1, 
					SysidToString(cmd.train_cmd[i].sys_id));
				str += sz;

				if(cmd.train_cmd[i].nSpeed != cmd.train_cmd[i].nSpeedMaxVal)
					sz.Format("\r\n    限速值=%dKm/h (%3dKm/h)", cmd.train_cmd[i].nSpeed, cmd.train_cmd[i].nSpeedMaxVal);
				else
					sz.Format("\r\n    限速值=%dKm/h", cmd.train_cmd[i].nSpeedMaxVal);
				str += sz;
				if(!cmd.train_cmd[i].szTrainID.IsEmpty())
				{
					sz.Format("    车次号=%s", cmd.train_cmd[i].szTrainID);
					str += sz;
				}

			}

			if(bGenCancelCommand)
			{
				sz.Format("\r\n最后恢复常速");
				str += sz;
			}
		}

		str += "\r\n";
		
		sz.Format("\r\n调度台台号: %d", cmd.nConsoleID);
		str += sz;

		// 生成时间
		if(cmd.nCreateTime <= 0)
			sz.Format("\r\n命令生成时间: 0");
		else
		{
			ct = cmd.nCreateTime;
			sz = ct.Format("\r\n命令生成时间: %Y年%m月%d日 %H:%M");
		}
		str += sz;
		
		sz.Format("\r\n系统标识: %s", SysidToString(cmd.raw_sysid));
		str += sz;

	return str;
}


ITccInterface::ITccInterface() : nRefCount(1)
{
}

ITccInterface::~ITccInterface()
{
}

void ITccInterface::AddRef()
{
	::InterlockedIncrement(&nRefCount);
}

void ITccInterface::Release()
{
	if(0 >= ::InterlockedDecrement(&nRefCount))
	{
		delete this;
	}
}


cmdid_t dwMaxID = 9999;
cmdid_t dwMinID = 1;
cmdid_t dwCommandID = 1;
BOOL  ITccClientInfoProvider::IsValidCommandID(WORD nCmdID)
{
	return nCmdID>=dwMinID && nCmdID<=dwMaxID;
}
cmdid_t ITccClientInfoProvider::GetInvalidCommandID()
{
	return 0;
}
cmdid_t ITccClientInfoProvider::GetMinCommandID()
{
	return dwMinID;
}
cmdid_t ITccClientInfoProvider::GetMaxCommandID()
{
	return dwMaxID;
}
cmdid_t ITccClientInfoProvider::GetCommandID()
{
	return dwCommandID;
}

void ITccClientInfoProvider::NextCommandID()
{
	dwCommandID ++;

	if( !IsValidCommandID(dwCommandID) )
		dwCommandID = GetMinCommandID();
}

cmdid_t ITccClientInfoProvider::SaveCommandID(cmdid_t nCommandID)
{
	dwCommandID = nCommandID + 1;

	if( !IsValidCommandID(dwCommandID) )
		dwCommandID = GetMinCommandID();

	CString szFilename;
	szFilename.Format("%s\\command-id.xml", GetTccConfigDirectory());
	SaveCommandIDInfo(szFilename);

	return dwCommandID;
}

BOOL ITccAdviseSink::ConfirmCommandID(int nConsoleID, cmdid_t nCommandID, cmdid_t nMinCommandID, cmdid_t nMaxCommandID, int& who_confirm_cmdid, DWORD dwCookie, HWND hTccDlg)
{
	who_confirm_cmdid = SELF_CONFIRM_COMMAND_ID;

	bool bValid = false;
	if(nCommandID <= nMaxCommandID && nCommandID >= nMinCommandID)
		bValid = true;
	ReceiveCommandIDConfirmation(bValid, nConsoleID, nCommandID, dwCookie);

	return TRUE;
}

BOOL SaveCommandIDInfo(LPCSTR szFilename)
{
	CStdioFile fFile;

	CFileException e = NULL;
	if( fFile.Open(szFilename, CFile::modeWrite | CFile::modeCreate | CFile::typeText, &e) )
	{
		CString str;
		str.Format("<?xml version=\"1.0\"?>\n");
		fFile.WriteString(str);
		str.Format("<COMMAND-ID MaxID=\"%lu\" MinID=\"%lu\" NextID=\"%lu\"/>\n", dwMaxID, dwMinID, dwCommandID);
		fFile.WriteString(str);
	
		fFile.Close();
	}

	return TRUE;
}

BOOL LoadCommandIDInfo(LPCSTR szFilename)
{
	BOOL bRet = FALSE;

	CoInitialize(NULL);

	do {
		MSXML2::IXMLDOMDocumentPtr pXMLDOMDoc;
		HRESULT hr;
		hr = pXMLDOMDoc.CreateInstance(__uuidof(MSXML2::DOMDocument40), NULL, CLSCTX_INPROC_SERVER);

		//pXMLDOMDoc->put_async(VARIANT_FALSE);
		//pXMLDOMDoc->put_validateOnParse(VARIANT_TRUE);
		//pXMLDOMDoc->put_resolveExternals(VARIANT_TRUE);

		_variant_t vXml(szFilename);
		VARIANT_BOOL vBool = pXMLDOMDoc->load(vXml);
		SysFreeString(vXml.bstrVal);
		vXml.Clear();

		MSXML2::IXMLDOMParseErrorPtr pError = pXMLDOMDoc->GetparseError();
		if(S_OK != pError->GeterrorCode())
		{
			CString str;
			str.Format("\nLine: %d", pError->Getline());
			_bstr_t strRes;
			strRes = _bstr_t("Validation failed on ") + szFilename
				+ _bstr_t("\n=============")
				+ _bstr_t("\nReason: ") + pError->Getreason();
				+ (LPCSTR)str
				+ _bstr_t("\nText: ") + pError->GetsrcText();
			
			//MessageBox("error");
			break;
		}

		MSXML2::IXMLDOMNodePtr pRootNode = pXMLDOMDoc->selectSingleNode("COMMAND-ID");

		// 取数据
		MSXML2::IXMLDOMNamedNodeMapPtr pattrMap = pRootNode->Getattributes();

		MSXML2::IXMLDOMNodePtr pNode;
		_variant_t v;
		CString str;

		pNode = pattrMap->getNamedItem( _bstr_t(_T("MaxID")) );
		v = pNode->GetnodeTypedValue();
		str = v.bstrVal;
		SysFreeString(v.bstrVal);
		v.Clear();
		dwMaxID = atoi(str);

		pNode = pattrMap->getNamedItem( _bstr_t(_T("MinID")) );
		v = pNode->GetnodeTypedValue();
		str = v.bstrVal;
		SysFreeString(v.bstrVal);
		v.Clear();
		dwMinID = atoi(str);

		pNode = pattrMap->getNamedItem( _bstr_t(_T("NextID")) );
		v = pNode->GetnodeTypedValue();
		str = v.bstrVal;
		SysFreeString(v.bstrVal);
		v.Clear();
		dwCommandID = atoi(str);

		bRet = TRUE;
	} while(0);

	::CoUninitialize();

	return bRet;
}

BOOL LoadTccConfig(LPCSTR szFilename)
{
	BOOL bRet = FALSE;

	CoInitialize(NULL);

	do {
		MSXML2::IXMLDOMDocumentPtr pXMLDOMDoc;
		HRESULT hr;
		hr = pXMLDOMDoc.CreateInstance(__uuidof(MSXML2::DOMDocument40), NULL, CLSCTX_INPROC_SERVER);

		//pXMLDOMDoc->put_async(VARIANT_FALSE);
		//pXMLDOMDoc->put_validateOnParse(VARIANT_TRUE);
		//pXMLDOMDoc->put_resolveExternals(VARIANT_TRUE);

		_variant_t vXml(szFilename);
		VARIANT_BOOL vBool = pXMLDOMDoc->load(vXml);
		SysFreeString(vXml.bstrVal);
		vXml.Clear();

		MSXML2::IXMLDOMParseErrorPtr pError = pXMLDOMDoc->GetparseError();
		if(S_OK != pError->GeterrorCode())
		{
			CString str;
			str.Format("\nLine: %d", pError->Getline());
			_bstr_t strRes;
			strRes = _bstr_t("Validation failed on ") + szFilename
				+ _bstr_t("\n=============")
				+ _bstr_t("\nReason: ") + pError->Getreason();
				+ (LPCSTR)str
				+ _bstr_t("\nText: ") + pError->GetsrcText();
			
			//MessageBox("error");
			break;
		}

		_variant_t v;
		CString str;

		//MSXML2::IXMLDOMNodePtr pNode = pXMLDOMDoc->selectSingleNode("root/available_speed");

		bRet = TRUE;
	} while(0);

	::CoUninitialize();

	return bRet;
}

struct TCCENV {
	CString szCommandIDFile;
	TCCENV() : szCommandIDFile("command-id.xml") {}
} tccenv;

BOOL LoadTccEnv(LPCSTR szFilename)
{
	CoInitialize(NULL);

	do {
		MSXML2::IXMLDOMDocumentPtr pXMLDOMDoc;
		HRESULT hr;
		hr = pXMLDOMDoc.CreateInstance(__uuidof(MSXML2::DOMDocument40), NULL, CLSCTX_INPROC_SERVER);

		pXMLDOMDoc->put_async(VARIANT_FALSE);
		pXMLDOMDoc->put_validateOnParse(VARIANT_TRUE);
		pXMLDOMDoc->put_resolveExternals(VARIANT_TRUE);

		_variant_t vXml(szFilename);
		VARIANT_BOOL vBool = pXMLDOMDoc->load(vXml);
		SysFreeString(vXml.bstrVal);
		vXml.Clear();

		MSXML2::IXMLDOMParseErrorPtr pError = pXMLDOMDoc->GetparseError();
		if(S_OK != pError->GeterrorCode())
		{
			CString str;
			str.Format("\nLine: %d", pError->Getline());
			_bstr_t strRes;
			strRes = _bstr_t("Validation failed on ") + szFilename
				+ _bstr_t("\n=============")
				+ _bstr_t("\nReason: ") + pError->Getreason();
			+ (LPCSTR)str
				+ _bstr_t("\nText: ") + pError->GetsrcText();
			
			//MessageBox("error");
			break;
		}

		MSXML2::IXMLDOMNodePtr pRootNode = pXMLDOMDoc->selectSingleNode("TCC-ENV");

		// 取数据
		MSXML2::IXMLDOMNodeListPtr pNodeList = pRootNode->GetchildNodes();

		MSXML2::IXMLDOMNodePtr pNode;
		long length = pNodeList->Getlength();
		for(int i=0; i<length; i++)
		{
			pNode = pNodeList->Getitem(i);
			_variant_t v;
			pNode->get_nodeTypedValue(&v);
			tccenv.szCommandIDFile = v.bstrVal;
			SysFreeString(v.bstrVal);
		}

		return TRUE;

	} while(0);

	::CoUninitialize();

	return FALSE;
}


BOOL TccInitialize(ITccClientInfoProvider *pInfoProvider)
{
	static BOOL bResult = TRUE;
	static int nTccInitialize = 0;
	if(nTccInitialize)
		return bResult;

	nTccInitialize ++;

	//LoadTccEnv(path);

	// 读取限速命令号范围，文件TCC\command-id.xml
	CString str;
	CString szCommandIDInfoFile;
	szCommandIDInfoFile.Format("%s\\command-id.xml", pInfoProvider->GetTccConfigDirectory());
	CFileFind fFind;
	if(!fFind.FindFile(szCommandIDInfoFile))
	{
		str.Format("%s 不存在!", szCommandIDInfoFile);
		AfxMessageBox(str);

		bResult = FALSE;
	}
	else if(!LoadCommandIDInfo(szCommandIDInfoFile))
	{
		str.Format("%s 内容错误!", szCommandIDInfoFile);
		AfxMessageBox(str);

		bResult = FALSE;
	}

	// 读Tcc.xml
	CString szTccFilename;
	szTccFilename.Format("%s\\tcc.cfg", pInfoProvider->GetTccConfigDirectory());
	if(!fFind.FindFile(szTccFilename))
	{
		str.Format("%s 不存在!", szTccFilename);
		AfxMessageBox(str);

		bResult = FALSE;
	}
	else if(!LoadTccConfig(szTccFilename))
	{
		str.Format("%s 内容错误!", szTccFilename);
		AfxMessageBox(str);

		bResult = FALSE;
	}
		
	return bResult;
}


map<DWORD, TCC_STATUS> mapTccStatus;
BOOL GetTccStatus(DWORD nTccID, TCC_STATUS *pStatus)
{
	if(NULL == pStatus)
		return FALSE;

	map<DWORD, TCC_STATUS>::iterator iter = mapTccStatus.find(nTccID);
	if(mapTccStatus.end() == iter)
		return FALSE;

	*pStatus = iter->second;

	return TRUE;
}

void SetTccStatus(DWORD nTccID, TCC_STATUS& status)
{
	mapTccStatus[nTccID] = status;
}

void ResetTccStatus()
{
	mapTccStatus.clear();
}

CString GetStockCommandStatusString(DWORD nStatus)
{
	static struct {
		DWORD status;
		LPCSTR str;
	} arr[] = {
		{CONFIRM, "已确认"},
		{EXECUTING, "已执行"},
		{CANCELED, "已取消"},
	}; 
	CString str = "未知状态";
	for(int i=0; i<sizeof(arr)/sizeof(arr[0]); i++)
	{
		if(arr[i].status == nStatus)
		{
			str = arr[i].str;
			break;
		}
	}
	return str;
}

CString GetCommandExtFileName()
{
	return "dat";
}

CString GetCommandExtraFileName(sysid_t& sysid, cmdid_t& nCmdID)
{
	CString str;
	str.Format("%d_%s", nCmdID, SysidToString(sysid));
	return str;
}

//static struct _LIMITSPEEDCMDSTATUS {
//	union {
//		DWORD cap;
//		struct {
//			DWORD bCanEdit : 1;
//			DWORD bCanReSend : 1;
//			DWORD bCanDel : 1;
//		} capbit;
//	};
//	DWORD nStatus;
//	LPCSTR str;
//} arr[] = {
//	{ 0x00000007, TCC_UNKNOWN,		"未知" },
//
//	{ 0x00000002, LIRC_RECEIVED,	"自律机收到" },
//	{ 0x00000003, LIRC_SEND_FAIL,	"自律机发送失败(协议帧错误)" },
//	{ 0x00000000, TCC_RECEIVED,		"列控收到" },
//
//	{ 0x00000007, TCC_ERR_KM,		"公里标非法" },
//	{ 0x00000007, TCC_ERR_SPEED,	"限速非法" },
//	{ 0x00000007, TCC_ERR_REGION_LONG,	"区域太长" },
//	{ 0x00000007, TCC_ERR_REGION_EXIST,	"区域已存在" },
//	{ 0x00000007, TCC_ERR_REGION_INVALID,"区域非法" },
//	{ 0x00000005, TCC_ERR_DUP_CMDNO,	"命令号重复" },
//	{ 0x00000007, TCC_ERR_TIME_FAIL,	"时间校验失败" },
//
//	{ 0x00000000, TCC_ACCEPT,       "列控接受" },
//	{ 0x00000000, TCC_WAITFOR_EXEC,	"排队，等待执行" },
//	{ 0x00000000, TCC_EXECUTING,	"正在执行" },
//	{ 0x00000000, TCC_ZOMBIE,		"执行结束(结束时间到)" },
//	{ 0x00000000, TCC_CANCELED,		"已取消(人工, 正常消亡)" },
//
//	{ 0x00000000, LOC_WAITFOR_CANCEL,	"等待取消" },
//	{ 0x00000007, LOC_EDIT,			"编辑" },
//	{ 0x00000003, LOC_SENDED,		"已下发" },
//	{ 0x00000003, LOC_SEND_FAIL,	"下发出错" },
//	
//	{ 0x00000000, TCC_CONFIRM_OK,      "被确认" },
//	{ 0x00000000, TCC_CONFIRM_CANCEL,  "被取消" },
//	{ 0x00000000, TCC_CONFIRM_TIMEOUT, "超时，未确认" },
//};
//static _LIMITSPEEDCMDSTATUS* Get(DWORD nStatus)
//{
//	_LIMITSPEEDCMDSTATUS *p = NULL;
//	for(int i=0; i<sizeof(arr)/sizeof(arr[0]); i++)
//	{
//		if(arr[i].nStatus == nStatus)
//		{
//			p = &arr[i];
//			break;
//		}
//	}
//	return p;
//}
//LPCSTR GetTccCmdStatusString(DWORD nStatus)
//{
//	_LIMITSPEEDCMDSTATUS *p = Get(nStatus);
//	if(p)
//		return p->str;
//	else
//		return NULL;
//}
//BOOL CanEditLimitSpeedCommand(DWORD nStatus)
//{
//	_LIMITSPEEDCMDSTATUS *p = Get(nStatus);
//	if(p)
//		return p->capbit.bCanEdit;
//	else
//		return FALSE;
//}
//BOOL CanReSendLimitSpeedCommand(DWORD nStatus)
//{
//	_LIMITSPEEDCMDSTATUS *p = Get(nStatus);
//	if(p)
//		return p->capbit.bCanReSend;
//	else
//		return FALSE;
//}
//BOOL CanDeleteLimitSpeedCommand(DWORD nStatus)
//{
//	_LIMITSPEEDCMDSTATUS *p = Get(nStatus);
//	if(p)
//		return p->capbit.bCanDel;
//	else
//		return FALSE;
//}


HWND hWndConfirmCommandIDReceiver = NULL;


BOOL ReceiveCommandIDConfirmation(bool bValid, int nConsoleID, cmdid_t nCmdID, DWORD dwCookie)
{
	SERVER_CONFIRM_COMMAND_ID_STRUCT ds;
	ds.bValid = bValid;
	ds.nConsoleID = nConsoleID;
	ds.nCmdID = nCmdID;
	ds.dwCookie = dwCookie;

	if(::IsWindow(hWndConfirmCommandIDReceiver))
		return ::SendMessage(hWndConfirmCommandIDReceiver, UM_SERVER_CONFIRM_COMMAND_ID, bValid, (LPARAM)&ds);

	return FALSE;
}

void RegisterConfirmCommandIDReceiver(HWND hWnd)
{
	hWndConfirmCommandIDReceiver = hWnd;
}

void UnRegisterConfirmCommandIDReceiver(HWND hWnd)
{
	hWndConfirmCommandIDReceiver = NULL;
}


};
