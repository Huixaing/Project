// TCC.cpp : 实现文件
//

#include "stdafx.h"
#include <map>
#include <list>
#include "sysshare.h"
#include "tcc_base.h"
#include "GpcInfoProvider.h"

//////////////////////////////////////////////////////
namespace TCC
{
//增加各站公里标范围
struct TCC_STRUCT 
{
	TCC::stano_t nTccID;

	struct _LINE {
		int builtin_id;
		TCC::lineid_t tccline_id;
		TCC::kmmark_t nKmBegin, nKmEnd;
	};
	std::map<int, _LINE> line_map;

	//侧线列表
	struct _BLOCK {
		TCC::lineid_t tccline_id;
		CString szName;
		_BLOCK() : tccline_id(0) { }
	};
	vector<_BLOCK> blocks;
};
std::map<TCC::stano_t, TCC_STRUCT> tcc_map;
///////////////////////////////////////////////////////


struct _ABSLINE_STRUCT {
	DWORD nAbsLineID;
	CString szLineName;
	TCC::dir_t  dir;
	struct {
		TCC::stano_t nStaID;
		TCC::lineid_t nTccLineID;
		TCC::dir_t   dir;
	} sta[2];
	_ABSLINE_STRUCT();
};

_ABSLINE_STRUCT::_ABSLINE_STRUCT()
{
	nAbsLineID = 0;
	dir = TCC::DIR_UNKNOWN;
	sta[0].nStaID = 0;
	sta[0].nTccLineID = 0;
	sta[0].dir = TCC::DIR_UNKNOWN;
	sta[1] = sta[0];
}

std::list<_ABSLINE_STRUCT> abslines;


struct _TCCLINE_STRUCT {
	TCC::stano_t nStaID;

	struct _LINE {
		TCC::lineid_t nTccLineID;
		TCC::dir_t line_dir;
		CString szLineName;
		TCC::dir_t dir[2];
		TCC::stano_t nLinkStaID[2];
		TCC::lineid_t nLinkTccLineID[2];
		_LINE()
		{
			nTccLineID = 0;
			line_dir = TCC::DIR_UNKNOWN;
			dir[0] = dir[1] = TCC::DIR_UNKNOWN;
			nLinkStaID[0] = nLinkStaID[1] = 0;
			nLinkTccLineID[0] = nLinkTccLineID[1] = 0;
		}
	};
	std::vector<_LINE> tcclines;

	TCC::dir_t GetStationDirection(TCC::stano_t nStaID2);
	TCC::stano_t GetLinkedStation(TCC::lineid_t nTccLineID, TCC::dir_t dir);
	LPCSTR GetTccLineName(TCC::lineid_t nTccLineID);
	void GetTccLineList(std::vector<TCC::lineid_t>& lines);
	void GetTccLineList(TCC::stano_t nStaID2, std::vector<TCC::lineid_t>& lines);
	TCC::lineid_t GetTccLine(TCC::stano_t nStaID2, TCC::lineid_t nTccLineID2);
	TCC::dir_t GetTccLineAbsNum(TCC::lineid_t nTccLineID);
	TCC::dir_t GetTccLineDirection(TCC::lineid_t nTccLineID);

	void AddTccLine(
		TCC::lineid_t nTccLineID,
		TCC::dir_t line_dir,
		CString szLineName,
		TCC::dir_t dir,
		TCC::stano_t nLinkStaID,
		TCC::lineid_t nLinkTccLineID);
};

TCC::dir_t _TCCLINE_STRUCT::GetStationDirection(TCC::stano_t nStaID2)
{
	TCC::dir_t dir = TCC::DIR_UNKNOWN;

	for(int i=0; i<tcclines.size(); i++)
	{
		_LINE& line = tcclines[i];
		if(line.nLinkStaID[0] == nStaID2)
		{
			dir = line.dir[0];
			break;
		}
		if(line.nLinkStaID[1] == nStaID2)
		{
			dir = line.dir[1];
			break;
		}
	}
	ASSERT(TCC::DIR_DUAL != dir);

	return dir;
}

TCC::stano_t _TCCLINE_STRUCT::GetLinkedStation(TCC::lineid_t nTccLineID, TCC::dir_t dir)
{
	TCC::stano_t nLinkStaID = 0;

	for(int i=0; i<tcclines.size(); i++)
	{
		_LINE& line = tcclines[i];
		if(line.nTccLineID == nTccLineID)
		{
			if(line.dir[0] == dir)
			{
				nLinkStaID = line.nLinkStaID[0];
			}
			else if(line.dir[1] == dir)
			{
				nLinkStaID = line.nLinkStaID[1];
			}

			break;
		}
	}

	return nLinkStaID;
}

TCC::dir_t _TCCLINE_STRUCT::GetTccLineAbsNum(TCC::lineid_t nTccLineID)
{
	TCC::dir_t num = TCC::DIR_UNKNOWN;

	for(int i=0; i<tcclines.size(); i++)
	{
		_LINE& line = tcclines[i];
		if(line.nTccLineID == nTccLineID)
		{
			if(line.dir[0] == TCC::DIR_UNKNOWN)
			{
				num = line.dir[1];
			}
			else if(line.dir[1] == TCC::DIR_UNKNOWN)
			{
				num = line.dir[0];
			}
			else
			{
				num = TCC::DIR_DUAL;
			}

			break;
		}
	}
	return num;
}

TCC::dir_t _TCCLINE_STRUCT::GetTccLineDirection(TCC::lineid_t nTccLineID)
{
	TCC::dir_t dir = TCC::DIR_UNKNOWN;

	for(int i=0; i<tcclines.size(); i++)
	{
		_LINE& line = tcclines[i];
		if(line.nTccLineID == nTccLineID)
		{
			dir = line.line_dir;
			break;
		}
	}
	return dir;
}

LPCSTR _TCCLINE_STRUCT::GetTccLineName(TCC::lineid_t nTccLineID)
{
	for(int i=0; i<tcclines.size(); i++)
	{
		_LINE& line = tcclines[i];
		if(line.nTccLineID == nTccLineID)
			return line.szLineName;
	}
	return "";
}

void _TCCLINE_STRUCT::GetTccLineList(std::vector<TCC::lineid_t>& lines)
{
	lines.clear();
	for(int i=0; i<tcclines.size(); i++)
	{
		_LINE& line = tcclines[i];
		lines.push_back(line.nTccLineID);
	}
}

void _TCCLINE_STRUCT::GetTccLineList(TCC::stano_t nStaID2, std::vector<TCC::lineid_t>& lines)
{
	lines.clear();
	for(int i=0; i<tcclines.size(); i++)
	{
		_LINE& line = tcclines[i];
		if(line.nLinkStaID[0] == nStaID2 || line.nLinkStaID[1] == nStaID2)
		{
			lines.push_back(line.nTccLineID);
		}
	}
}

TCC::lineid_t _TCCLINE_STRUCT::GetTccLine(TCC::stano_t nStaID2, TCC::lineid_t nTccLineID2)
{
	TCC::lineid_t nLinkTccLineID = 0;

	for(int i=0; i<tcclines.size(); i++)
	{
		_LINE& line = tcclines[i];
		if(line.nTccLineID == nTccLineID2)
		{
			if(line.nLinkStaID[0] == nStaID2)
			{
				nLinkTccLineID = line.nLinkTccLineID[0];
			}
			else if(line.nLinkStaID[1] == nStaID2)
			{
				nLinkTccLineID = line.nLinkTccLineID[1];
			}
			break;
		}
	}
	return nLinkTccLineID;
}

void _TCCLINE_STRUCT::AddTccLine(
	TCC::lineid_t nTccLineID,
	TCC::dir_t line_dir,
	CString szLineName,
	TCC::dir_t dir,
	TCC::stano_t nLinkStaID,
	TCC::lineid_t nLinkTccLineID)
{
	ASSERT(TCC::DIR_DOWN == dir || TCC::DIR_UP == dir);
	ASSERT(TCC::DIR_DOWN == line_dir
		|| TCC::DIR_UP == line_dir
		|| TCC::DIR_DUAL== line_dir);

	for(int i=0;i<tcclines.size(); i++)
	{
		_LINE& line = tcclines[i];
		if(line.nTccLineID == nTccLineID)
		{
			if(line.szLineName != szLineName)
			{
				CString prompt;
				prompt.Format("%d站 TccLine=%d有两个名称\n\r%s\n\r%s", 
					nStaID, nTccLineID,
					line.szLineName,
					szLineName);
				AfxMessageBox(prompt);
			}
			if(line.line_dir != line_dir)
			{
				CString prompt;
				prompt.Format("%d站 TccLine=%d有两个方向", 
					nStaID, nTccLineID);
				AfxMessageBox(prompt);
			}

			if(line.dir[0] == TCC::DIR_UNKNOWN)
			{
				line.dir[0] = dir;
				line.nLinkStaID[0] = nLinkStaID;
				line.nLinkTccLineID[0] = nLinkTccLineID;
			}
			else if(line.dir[1] == TCC::DIR_UNKNOWN)
			{
				line.dir[1] = dir;
				line.nLinkStaID[1] = nLinkStaID;
				line.nLinkTccLineID[1] = nLinkTccLineID;
			}

			return;
		}
	}

	_LINE line;
	line.nTccLineID = nTccLineID;
	line.line_dir = line_dir;
	line.szLineName = szLineName;
	line.dir[0] = dir;
	line.nLinkStaID[0] = nLinkStaID;
	line.nLinkTccLineID[0] = nLinkTccLineID;
	tcclines.push_back(line);
}

std::map<TCC::stano_t, _TCCLINE_STRUCT> sta_tccline_map;


CTccGpcInfoProvider::CTccGpcInfoProvider()
	: m_nSecNo(0)
{
}

CTccGpcInfoProvider::~CTccGpcInfoProvider()
{
}

// 得到TCC数据根目录, 如.../data/tcc
CString CTccGpcInfoProvider::GetTccDataDirectory()
{
	CString path = gpcdef.GetDDMLDataDir();
	path.TrimRight("\\");
	path.TrimRight("/");
	path += "\\DDML\\TCC";

	CString sz;
	//sz.Format("\\q%03d", pDoc->m_nCurrentSectionNo);
	return path + sz;
}

CString CTccGpcInfoProvider::GetTccConfigDirectory()
{
	CString path = gpcdef.GetDataDir();
	CString dir = gpcdef.GetTccCfgDir(2);
	if(dir.IsEmpty())
	{
		;
	}
	else
		path += "\\" + dir;
	
	return path;
}


// 得到车站列表，返回值为实际取得的列表数目
void CTccGpcInfoProvider::GetStationList(std::vector<TCC::stano_t>& staList)
{
	staList.clear();

	int num=500;
	int stano[500];
	if(gpcdef.GetStaList(2, num, stano))
	{
		for(int i=0; i<num; i++)
		{
			WORD station=stano[i];
			int k=0;
			for(k=0; k<staList.size(); k++)
			{
				if(station == staList[k])
					break;
			}
			if(k>=staList.size())
			{
				staList.push_back(station);
			}
		}
	}
	
	return;
}
// 根据站号得到站名
CString CTccGpcInfoProvider::GetStationName(TCC::stano_t nStaID)
{
	CString szStaName = gpcdef.GetStationName(nStaID);
	if(szStaName=="管外")
		return "";
	return szStaName;
}

// 得到发令单位和发令人
CString CTccGpcInfoProvider::GetOrgSender()
{
	return CString("Casco Ltd.");
}
CString CTccGpcInfoProvider::GetPersonSender()
{
	return CString("SCX");
}

WORD CTccGpcInfoProvider::GetConsoleID()
{
	return m_nCurrentSectionNo;//gpcdef.GetDispatchNoFromDefaultUser();//这里是指哪个调度台
}

// 判断nStaID与nStaID2 的连接状态
BOOL CTccGpcInfoProvider::IsLinkedStation(TCC::stano_t nStaID, TCC::stano_t nStaID2)
{
	TCC::dir_t dir = TCC::DIR_UNKNOWN;

	std::map<TCC::stano_t, _TCCLINE_STRUCT>::iterator iter = sta_tccline_map.find(nStaID);
	if(iter != sta_tccline_map.end())
	{
		dir = (*iter).second.GetStationDirection(nStaID2);
	}
	ASSERT(TCC::DIR_DUAL != dir);

	return TCC::DIR_UNKNOWN != dir;
}

// 站nStaID2相对于站nStaID的区间方向性(上行方向, 下行方向)
TCC::dir_t CTccGpcInfoProvider::GetStationDirection(
	TCC::stano_t nStaID, 
	TCC::stano_t nStaID2)
{
	TCC::dir_t dir = TCC::DIR_UNKNOWN;

	std::map<TCC::stano_t, _TCCLINE_STRUCT>::iterator iter = sta_tccline_map.find(nStaID);
	if(iter != sta_tccline_map.end())
	{
		dir = (*iter).second.GetStationDirection(nStaID2);
	}

	return dir;
}

// 得到nStaID站的连接站列表
void CTccGpcInfoProvider::GetLinkedStationList(TCC::stano_t nStaID, std::vector<TCC::stano_t>& staList)
{
	staList.clear();

/*	
	for(int i=0; i<pDoc->m_nAllStationNum; i++)
	{
		if(!IsLinkedStation(nStaID, pDoc->m_nAllStationList[i]))
			continue;
		staList.push_back(pDoc->m_nAllStationList[i]);
	}
*/
	ushort RcvSTCount;
	int nDispatchNo = gpcdef.GetDispatchNoFromDefaultUser();
	RcvSTCount = gpcdef.GetRcvDCStationNum(nDispatchNo);

	for(int i=0; i<RcvSTCount; i++)
	{
		ushort stno = gpcdef.GetRcvDCStationList(nDispatchNo, i);

		if(!IsLinkedStation(nStaID, stno))
			continue;

		staList.push_back(stno);
	}



	//for(int i=0; i<pDoc->m_nStationNum; i++)
	//{
	//	if(!IsLinkedStation(nStaID, pDoc->m_nCurrentStationList[i]))
	//		continue;

	//	staList.push_back(pDoc->m_nCurrentStationList[i]);
	//}
}

// 根据站号和Absline编号得到相连的另一个站
TCC::stano_t CTccGpcInfoProvider::GetLinkedStation(
	TCC::stano_t nStaID, 
	TCC::lineid_t nTccLineID,
	TCC::dir_t dir)
{
	TCC::stano_t nTempStaID = 0;

	//for(int i=0; i<pDoc->m_nAbsLineCount; i++)
	//{
	//	if(pDoc->abs_line_info[i].nLineNo != nAbslineID)
	//		continue;

	//	if(pDoc->abs_line_info[i].nStartStation == nStaID)
	//	{
	//		nTempStaID = pDoc->abs_line_info[i].nEndStation;
	//		break;
	//	}

	//	if(pDoc->abs_line_info[i].nEndStation == nStaID)
	//	{
	//		nTempStaID = pDoc->abs_line_info[i].nStartStation;
	//		break;
	//	}
	//}
	std::map<TCC::stano_t, _TCCLINE_STRUCT>::iterator iter = sta_tccline_map.find(nStaID);
	if(iter != sta_tccline_map.end())
		nTempStaID = (*iter).second.GetLinkedStation(nTccLineID, dir);

	return nTempStaID;
}

void CTccGpcInfoProvider::GetTccLineList(TCC::stano_t nStaID, std::vector<TCC::lineid_t>& lines)
{
	//lines.clear();
	//lines.push_back(1);
	//lines.push_back(2);
	std::map<TCC::stano_t, _TCCLINE_STRUCT>::iterator iter = sta_tccline_map.find(nStaID);
	if(iter != sta_tccline_map.end())
	{
		(*iter).second.GetTccLineList(lines);
	}
}

void CTccGpcInfoProvider::GetTccLineList(
		TCC::stano_t nStaID,
		TCC::stano_t nStaID2,
		std::vector<TCC::lineid_t>& lines)
{
	//lines.clear();
	//lines.push_back(1);
	//lines.push_back(2);
	std::map<TCC::stano_t, _TCCLINE_STRUCT>::iterator iter = sta_tccline_map.find(nStaID);
	if(iter != sta_tccline_map.end())
	{
		(*iter).second.GetTccLineList(nStaID2, lines);
	}
}

void CTccGpcInfoProvider::GetTccBlockLineList(TCC::stano_t nStaID, std::vector<TCC::lineid_t>& lines)
{	
	TCC_STRUCT &tcc = tcc_map[nStaID];
	if(tcc.nTccID != nStaID)
		return;

	for(int i=0; i<tcc.blocks.size(); i++)
	{
		lines.push_back(tcc.blocks[i].tccline_id);
	}
}

CString CTccGpcInfoProvider::GetTccBlockLineName(TCC::stano_t nStaID, TCC::lineid_t nTccLineID)
{
	CString str;
	TCC_STRUCT &tcc = tcc_map[nStaID];
	if(tcc.nTccID != nStaID)
		return str;

	for(int i=0; i<tcc.blocks.size(); i++)
	{
		if(tcc.blocks[i].tccline_id == nTccLineID)
		{
			str = tcc.blocks[i].szName;
			break;
		}
	}
	return str;
}

BOOL CTccGpcInfoProvider::IsTccBlockLine(TCC::stano_t nStaID, TCC::lineid_t nTccLineID)
{
	TCC_STRUCT &tcc = tcc_map[nStaID];
	if(tcc.nTccID != nStaID)
		return FALSE;

	BOOL bRet = FALSE;

	for(int i=0; i<tcc.blocks.size(); i++)
	{
		if(tcc.blocks[i].tccline_id == nTccLineID)
		{
			bRet = TRUE;
			break;
		}
	}
	return bRet;
}

TCC::lineid_t CTccGpcInfoProvider::GetTccLine(
	TCC::stano_t nStaID,
	TCC::stano_t nStaID2,
	TCC::lineid_t nTccLineID2)
{
	std::map<TCC::stano_t, _TCCLINE_STRUCT>::iterator iter = sta_tccline_map.find(nStaID2);
	if(iter != sta_tccline_map.end())
	{
		return (*iter).second.GetTccLine(nStaID, nTccLineID2);
	}
	return 0;
}

TCC::dir_t CTccGpcInfoProvider::GetTccLineAbsNum(TCC::stano_t nStaID, TCC::lineid_t nTccLineID)
{
	std::map<TCC::stano_t, _TCCLINE_STRUCT>::iterator iter = sta_tccline_map.find(nStaID);
	if(iter != sta_tccline_map.end())
	{
		return (*iter).second.GetTccLineAbsNum(nTccLineID);
	}
	return TCC::DIR_UNKNOWN;
}

TCC::dir_t CTccGpcInfoProvider::GetTccLineDirection(TCC::stano_t nStaID, TCC::lineid_t nTccLineID)
{
	std::map<TCC::stano_t, _TCCLINE_STRUCT>::iterator iter = sta_tccline_map.find(nStaID);
	if(iter != sta_tccline_map.end())
	{
		return (*iter).second.GetTccLineDirection(nTccLineID);
	}
	return TCC::DIR_UNKNOWN;
}

CString CTccGpcInfoProvider::GetTccLineName(TCC::stano_t nStaID, TCC::lineid_t nTccLineID)
{
	if( IsTccBlockLine(nStaID, nTccLineID) )
		return GetTccBlockLineName(nStaID, nTccLineID);
	else
	{
		CString str;
		std::map<TCC::stano_t, _TCCLINE_STRUCT>::iterator iter = sta_tccline_map.find(nStaID);
		if(iter != sta_tccline_map.end())
			return str = (*iter).second.GetTccLineName(nTccLineID);

		return str;
	}
}

// 校验站号的合法性
BOOL CTccGpcInfoProvider::ValidateStationID(TCC::stano_t nStaID)
{
/*
	for(int i=0; i<pDoc->m_nStationNum; i++)
	{
		if(pDoc->m_nCurrentStationList[i] == nStaID)
			break;
	}
	return i < pDoc->m_nStationNum;
*/
	ushort RcvSTCount;
	int nDispatchNo = gpcdef.GetDispatchNoFromDefaultUser();
	RcvSTCount = gpcdef.GetRcvDCStationNum(nDispatchNo);

	for(int i=0; i<RcvSTCount; i++)
	{
		if(gpcdef.GetRcvDCStationList(nDispatchNo, i) == nStaID)
			break;
	}

	return i < RcvSTCount;

}

BOOL CTccGpcInfoProvider::IsStationListChanged()
{
	BOOL bRet = FALSE;
	if(m_nSecNo != m_nCurrentSectionNo)
		bRet = TRUE;
	m_nSecNo = m_nCurrentSectionNo;
	return bRet;
}

// 根据公里标得到相关站
int CTccGpcInfoProvider::GetLineID(TCC::stano_t nTccID, TCC::lineid_t nTccLineID)
{
	TCC_STRUCT& tcc = tcc_map[nTccID];
	if(tcc.nTccID != nTccID)
		return -1;

	std::map<int, TCC_STRUCT::_LINE>::iterator iter_line = tcc.line_map.begin();
	for(; iter_line != tcc.line_map.end(); iter_line++)
	{
		TCC_STRUCT::_LINE& line = (*iter_line).second;
		if(nTccLineID == line.tccline_id)
			return line.builtin_id;
	}

	return -1;
}

std::vector<TCC::stano_t> CTccGpcInfoProvider::GetTccList(int builtin_id, TCC::kmmark_t nKmBegin, TCC::kmmark_t nKmEnd)
{
	std::vector<TCC::stano_t> tcclist;

	if(nKmBegin == nKmEnd)
		return tcclist;

	TCC::kmmark_t nMax = nKmEnd,	nMin = nKmBegin;
	if(nMin > nMax)
	{
		nMin = nKmEnd;
		nMax = nKmBegin;
	}

	std::map<TCC::stano_t, TCC_STRUCT>::iterator iter;
	for(iter = tcc_map.begin(); iter != tcc_map.end(); iter++)
	{
		TCC_STRUCT& tcc = (*iter).second;

		std::map<int, TCC_STRUCT::_LINE>::iterator iter_line = tcc.line_map.begin();
		for(; iter_line != tcc.line_map.end(); iter_line++)
		{
			TCC_STRUCT::_LINE& line = (*iter_line).second;

			if(line.builtin_id != builtin_id)
				continue;

			TCC::kmmark_t nTccMax = line.nKmEnd;
			TCC::kmmark_t nTccMin = line.nKmBegin;
			if(nTccMax == nTccMin)
				continue;
			if(nTccMin > nTccMax)
			{
				nTccMin = line.nKmEnd;
				nTccMax = line.nKmBegin;
			}

			//计算交叉点
			if(nTccMin < nMin)
				nTccMin = nMin;
			if(nTccMax > nMax)
				nTccMax = nMax;
			if(nTccMin < nTccMax)
			{
				tcclist.push_back(tcc.nTccID);
			}
		}
	}

	return tcclist;
}

void CTccGpcInfoProvider::SetTCCRight(int SectionNo, bool bReadOnly, bool bExecute, bool bCTCGpc)
{
	m_nCurrentSectionNo=SectionNo;
	m_bReadOnly = bReadOnly;
	m_bExecute  = bExecute;
	m_bCTCGpc   = bCTCGpc;
}

void CTccGpcInfoProvider::LoadTccLineNameConfig(CString strFilename)
{
	gpcdef.ClearC2TccLineName();
	LPCSTR szFilename = strFilename;

	CoInitialize(NULL);

	MSXML2::IXMLDOMDocumentPtr pXMLDOMDoc;
	HRESULT hr;
	hr = pXMLDOMDoc.CreateInstance(
		__uuidof(MSXML2::DOMDocument40), 
		NULL, 
		CLSCTX_INPROC_SERVER);

	pXMLDOMDoc->put_async(VARIANT_FALSE);
	pXMLDOMDoc->put_validateOnParse(VARIANT_TRUE);
	pXMLDOMDoc->put_resolveExternals(VARIANT_TRUE);

	_variant_t vXml(szFilename);
	VARIANT_BOOL vBool = pXMLDOMDoc->load(vXml);
	SysFreeString(vXml.bstrVal);
	if(VARIANT_FALSE == vBool)
		return;

	MSXML2::IXMLDOMParseErrorPtr pError = pXMLDOMDoc->GetparseError();
	if(S_OK != pError->GeterrorCode())
	{
		CString str; 
		str.Format("\nLine: %d", pError->Getline());

		_bstr_t strRes = _bstr_t("Validation failed on ") + szFilename
			+ _bstr_t("\n=============")
			+ _bstr_t("\nReason: ") + pError->Getreason()
			+ _bstr_t("\nText: ") + pError->GetsrcText()
			+ _bstr_t(str);

		AfxMessageBox(strRes);
		return;
	}

	// 取数据
	MSXML2::IXMLDOMNodeListPtr pNodeList = pXMLDOMDoc->selectNodes("root/absline");
	long length = pNodeList->Getlength();
	for(int i=0; i<length; i++)
	{
		_ABSLINE_STRUCT absline;

		MSXML2::IXMLDOMNodePtr pNode_absline = pNodeList->Getitem( i );

		CString str;
		_variant_t varVal;
		MSXML2::IXMLDOMNodePtr pNode;
		int nAbsLineID=0;
		if(pNode = pNode_absline->selectSingleNode("@id"))
		{
			varVal = pNode->GetnodeTypedValue();
			str = varVal.bstrVal;
			nAbsLineID = atoi(str);

			SysFreeString(varVal.bstrVal);
			varVal.Clear();
		}
		if(pNode = pNode_absline->selectSingleNode("@name"))
		{
			varVal = pNode->GetnodeTypedValue();
			str = varVal.bstrVal;

			SysFreeString(varVal.bstrVal);
			varVal.Clear();

			if(nAbsLineID>0)
				gpcdef.AddC2TccLineName(nAbsLineID, str);
		}
	}

	CoUninitialize();
}

BOOL CTccGpcInfoProvider::LoadTccLineConfig()
{
	CString strFilename;
	strFilename.Format("%s\\tccline.cfg", GetTccConfigDirectory());
	LPCSTR szFilename = strFilename;

	abslines.clear();
	tcc_map.clear();
	sta_tccline_map.clear();

	BOOL bRet = FALSE;

	CoInitialize(NULL);

	do {
		MSXML2::IXMLDOMDocumentPtr pXMLDOMDoc;
		HRESULT hr;
		hr = pXMLDOMDoc.CreateInstance(
			__uuidof(MSXML2::DOMDocument40), 
			NULL, 
			CLSCTX_INPROC_SERVER);

		pXMLDOMDoc->put_async(VARIANT_FALSE);
		pXMLDOMDoc->put_validateOnParse(VARIANT_TRUE);
		pXMLDOMDoc->put_resolveExternals(VARIANT_TRUE);

		_variant_t vXml(szFilename);
		VARIANT_BOOL vBool = pXMLDOMDoc->load(vXml);
		SysFreeString(vXml.bstrVal);
		if(VARIANT_FALSE == vBool)
			return FALSE;

		MSXML2::IXMLDOMParseErrorPtr pError = pXMLDOMDoc->GetparseError();
		if(S_OK != pError->GeterrorCode())
		{
			CString str; 
			str.Format("\nLine: %d", pError->Getline());

			_bstr_t strRes = _bstr_t("Validation failed on ") + szFilename
				+ _bstr_t("\n=============")
				+ _bstr_t("\nReason: ") + pError->Getreason()
				+ _bstr_t("\nText: ") + pError->GetsrcText()
				+ _bstr_t(str);

			AfxMessageBox(strRes);
			break;
		}

		// 取数据
		MSXML2::IXMLDOMNodeListPtr pNodeList = pXMLDOMDoc->selectNodes("root/absline");
		long length = pNodeList->Getlength();
		for(int i=0; i<length; i++)
		{
			_ABSLINE_STRUCT absline;

			MSXML2::IXMLDOMNodePtr pNode_absline = pNodeList->Getitem( i );

			CString str;
			_variant_t varVal;
			MSXML2::IXMLDOMNodePtr pNode;

			if(pNode = pNode_absline->selectSingleNode("@id"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				absline.nAbsLineID = atoi(str);

				SysFreeString(varVal.bstrVal);
				varVal.Clear();
			}
			if(pNode = pNode_absline->selectSingleNode("@name"))
			{
				varVal = pNode->GetnodeTypedValue();
				absline.szLineName = varVal.bstrVal;

				SysFreeString(varVal.bstrVal);
				varVal.Clear();
			}
			if(pNode = pNode_absline->selectSingleNode("@direction"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				switch(atoi(str))
				{
				case 0:
					absline.dir = TCC::DIR_UP;
					break;
				case 1:
					absline.dir = TCC::DIR_DOWN;
					break;
				case 3:
				default:
					absline.dir = TCC::DIR_DUAL;
					break;
					break;
				}

				SysFreeString(varVal.bstrVal);
				varVal.Clear();
			}

			if(pNode = pNode_absline->selectSingleNode("upstation/@id"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				absline.sta[0].nStaID = atoi(str);

				SysFreeString(varVal.bstrVal);
				varVal.Clear();
			}
			if(pNode = pNode_absline->selectSingleNode("upstation/@tccline"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				absline.sta[0].nTccLineID = atoi(str);
				absline.sta[0].dir = TCC::DIR_UP;

				SysFreeString(varVal.bstrVal);
				varVal.Clear();
			}

			if(pNode = pNode_absline->selectSingleNode("downstation/@id"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				absline.sta[1].nStaID = atoi(str);

				SysFreeString(varVal.bstrVal);
				varVal.Clear();
			}
			if(pNode = pNode_absline->selectSingleNode("downstation/@tccline"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				absline.sta[1].nTccLineID = atoi(str);
				absline.sta[1].dir = TCC::DIR_DOWN;

				SysFreeString(varVal.bstrVal);
				varVal.Clear();
			}

			abslines.push_back(absline);

			TCC::stano_t nStaID = absline.sta[0].nStaID;
			TCC::stano_t nStaID2= absline.sta[1].nStaID;

			if(nStaID)
			{
				if(sta_tccline_map.find(nStaID) == sta_tccline_map.end())
				{
					_TCCLINE_STRUCT tccline;
					tccline.nStaID = nStaID;
					tccline.AddTccLine(absline.sta[0].nTccLineID,
						absline.dir,
						absline.szLineName,
						absline.sta[1].dir,
						nStaID2,
						absline.sta[1].nTccLineID);
					sta_tccline_map[nStaID] = tccline;
				}
				else
				{
					_TCCLINE_STRUCT& tccline = sta_tccline_map[nStaID];
					tccline.AddTccLine(absline.sta[0].nTccLineID,
						absline.dir,
						absline.szLineName,
						absline.sta[1].dir,
						nStaID2,
						absline.sta[1].nTccLineID);
				}
			}

			if(nStaID2)
			{
				if(sta_tccline_map.find(nStaID2) == sta_tccline_map.end())
				{
					_TCCLINE_STRUCT tccline;
					tccline.nStaID = nStaID2;
					tccline.AddTccLine(absline.sta[1].nTccLineID,
						absline.dir,
						absline.szLineName,
						absline.sta[0].dir,
						nStaID,
						absline.sta[0].nTccLineID);
					sta_tccline_map[nStaID2] = tccline;
				}
				else
				{
					_TCCLINE_STRUCT& tccline = sta_tccline_map[nStaID2];
					tccline.AddTccLine(absline.sta[1].nTccLineID,
						absline.dir,
						absline.szLineName,
						absline.sta[0].dir,
						nStaID,
						absline.sta[0].nTccLineID);
				}
			}
		}


		variant_t varVal;
		CString str, prompt, szPromptPrex;

		//列控管辖的线路范围
		pNodeList = pXMLDOMDoc->selectNodes("root/tcclist/tcc");
		length = pNodeList->Getlength();
		for(i=0; i<length; i++)
		{
			MSXML2::IXMLDOMNodePtr pNode_tcc = pNodeList->Getitem(i);
			MSXML2::IXMLDOMNodePtr pNode;

			TCC_STRUCT tcc;
			CString szPromptPrex;

			if(pNode = pNode_tcc->selectSingleNode("@id"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;

				SysFreeString(varVal.bstrVal);
				varVal.Clear();

				tcc.nTccID = atoi(str);
				if(tcc.nTccID <= 0)
				{
					prompt.Format("root/tcclist/tcc/@id=\"%s\" 列控编号非法, 跳过!", szPromptPrex, str);
					AfxMessageBox(prompt);
					continue;
				}
				szPromptPrex.Format("root/tcclist/tcc[@id=\"%s\"]", str);
			}
			else
			{
				prompt.Format("root/tcclist/tcc/@id 没有配, 跳过!", str);
				AfxMessageBox(prompt);
				continue;
			}


			MSXML2::IXMLDOMNodeListPtr pNodeList_line = pNode_tcc->selectNodes("linelist/line");
			// 最多8条线
			for(int j=0; j<pNodeList_line->Getlength(); j++)
			{
				TCC_STRUCT::_LINE tcc_line;
				CString szTccLinePromptPrex;
			
				MSXML2::IXMLDOMNodePtr pNode_line = pNodeList_line->Getitem(j);

				if(pNode = pNode_line->selectSingleNode("@builtin_id"))
				{
					varVal = pNode->GetnodeTypedValue();
					str = varVal.bstrVal;

					SysFreeString(varVal.bstrVal);
					varVal.Clear();

					tcc_line.builtin_id = atoi(str);
					if(tcc_line.builtin_id <= 0)
					{
						prompt.Format("%s/linelist/line/@builtin_id=\"%s\" 不合法, 跳过", 
							szPromptPrex, str);
						AfxMessageBox(prompt);
						continue;
					}
					szTccLinePromptPrex.Format("%s/linelist/line[@builtin_id=\"%s\"]", szPromptPrex, str);
				}
				else
				{
					prompt.Format("%s/linelist/line/@builtin_id 没有配, 跳过", str);
					AfxMessageBox(prompt);
					continue;
				}

				//if(line_map.end() == line_map.find(tcc_line.builtin_id))
				//{
				//	prompt.Format("%s, 线路属性列表中没有这条线路，跳过", szTccLinePromptPrex);
				//	AfxMessageBox(prompt);
				//	continue;
				//}

				if(tcc.line_map.end() != tcc.line_map.find(tcc_line.builtin_id))
				{
					prompt.Format("%s 线路已存在，跳过", szTccLinePromptPrex);
					AfxMessageBox(prompt);
					continue;
				}

				if(pNode = pNode_line->selectSingleNode("@tccline_id"))
				{
					varVal = pNode->GetnodeTypedValue();
					str = varVal.bstrVal;

					SysFreeString(varVal.bstrVal);
					varVal.Clear();

					tcc_line.tccline_id = atoi(str);
					if(tcc_line.tccline_id <= 0)
					{
						prompt.Format("%s/@tccline_id=\"%s\" 不合法, 跳过", 
							szTccLinePromptPrex, str);
						AfxMessageBox(prompt);
						continue;
					}
					szTccLinePromptPrex += "[@tccline_id=" + str + "\"]";
					if(tcc_line.tccline_id == tcc.line_map[tcc_line.builtin_id].tccline_id)
					{
						prompt.Format("%s 已存在，跳过", szTccLinePromptPrex);
						AfxMessageBox(prompt);
						continue;
					}
				}
				else
				{
					prompt.Format("%s/@tccline_id 没有配, 跳过", szTccLinePromptPrex);
					AfxMessageBox(prompt);
					continue;
				}

				if(pNode = pNode_line->selectSingleNode("@begin"))
				{
					varVal = pNode->GetnodeTypedValue();
					str = varVal.bstrVal;

					SysFreeString(varVal.bstrVal);
					varVal.Clear();

					tcc_line.nKmBegin = atoi(str);
					if(tcc_line.nKmBegin <= 0)
					{
						prompt.Format("%s/@begin=\"%s\" 非法, 跳过",
							szTccLinePromptPrex, str);
						AfxMessageBox(prompt);
						continue;
					}
					szTccLinePromptPrex += "[@begin=" + str + "\"]";
				}
				else
				{
					prompt.Format("%s/@begin 没有配, 跳过", szTccLinePromptPrex);
					AfxMessageBox(prompt);
					continue;
				}

				if(pNode = pNode_line->selectSingleNode("@end"))
				{
					varVal = pNode->GetnodeTypedValue();
					str = varVal.bstrVal;

					SysFreeString(varVal.bstrVal);
					varVal.Clear();

					tcc_line.nKmEnd = atoi(str);
					if(tcc_line.nKmEnd <= 0)
					{
						prompt.Format("%s/@end=\"%s\" 非法, 跳过",
							szTccLinePromptPrex, str);
						AfxMessageBox(prompt);
						continue;
					}
					szTccLinePromptPrex += "[@end=" + str + "\"]";
				}
				else
				{
					prompt.Format("%s/@end 没有配, 跳过", szTccLinePromptPrex);
					AfxMessageBox(prompt);
					continue;
				}

				if(tcc_line.nKmBegin == tcc_line.nKmEnd)
				{
					prompt.Format("%s 的@begin==@end, 不合法，跳过", szTccLinePromptPrex);
					AfxMessageBox(prompt);
					continue;
				}
				else
				{
					if(tcc_line.nKmBegin > tcc_line.nKmEnd)
					{
						TCC::kmmark_t temp;
						temp = tcc_line.nKmBegin;
						tcc_line.nKmBegin = tcc_line.nKmEnd;
						tcc_line.nKmEnd = temp;
					}
				}

				tcc.line_map[tcc_line.builtin_id] = tcc_line;
			}

			//侧线
			MSXML2::IXMLDOMNodeListPtr pNodeList_block = pNode_tcc->selectNodes("blocklist/block");
			for(int j=0; j<pNodeList_block->Getlength(); j++)
			{
				TCC_STRUCT::_BLOCK tcc_block;
			
				MSXML2::IXMLDOMNodePtr pNode_block = pNodeList_block->Getitem(j);

				if(pNode = pNode_block->selectSingleNode("@tccline_id"))
				{
					varVal = pNode->GetnodeTypedValue();
					str = varVal.bstrVal;

					SysFreeString(varVal.bstrVal);
					varVal.Clear();

					tcc_block.tccline_id = atoi(str);
					if(tcc_block.tccline_id <= 0)
					{
						prompt.Format("%s/blocklist/block/@tccline_id=\"%s\" 不合法, 跳过", szPromptPrex, str);
						AfxMessageBox(prompt);
						continue;
					}
					for(int iblock=0; iblock<tcc.blocks.size(); iblock++)
					{
						if(tcc.blocks[iblock].tccline_id == tcc_block.tccline_id)
							break;
					}
					if(iblock < tcc.blocks.size())
					{
						prompt.Format("%s/blocklist/block[@tccline_id=\"%d\"] 线路已存在，跳过", 
							szPromptPrex, tcc_block.tccline_id);
						AfxMessageBox(prompt);
						continue;
					}
					std::map<int, TCC_STRUCT::_LINE>::const_iterator iter;
					for(iter = tcc.line_map.begin();
						iter != tcc.line_map.end();
						iter ++)
					{
						if(iter->second.tccline_id == tcc_block.tccline_id)
						{
							break;
						}
					}
					if(iter != tcc.line_map.end())
					{
						prompt.Format("%s/blocklist/block[@tccline_id=\"%d\"] 线路已存在，跳过", 
							szPromptPrex, tcc_block.tccline_id);
						AfxMessageBox(prompt);
						continue;
					}
				}
				else
				{
					prompt.Format("%s/blocklist/block/@tccline_id 没有配，跳过", szPromptPrex);
					AfxMessageBox(prompt);
					continue;
				}

				if(pNode = pNode_block->selectSingleNode("@name"))
				{
					varVal = pNode->GetnodeTypedValue();
					tcc_block.szName = varVal.bstrVal;

					SysFreeString(varVal.bstrVal);
					varVal.Clear();
				}

				/*if(pNode = pNode_block->selectSingleNode("@speed"))
				{
					varVal = pNode->GetnodeTypedValue();
					str = varVal.bstrVal;

					SysFreeString(varVal.bstrVal);
					varVal.Clear();

					parseNumberList(tcc_block.speeds, str);
				}
				else
				{
					tcc_block.speeds = blockspeeds;
				}*/

				tcc.blocks.push_back(tcc_block);
			}

			tcc_map[tcc.nTccID] = tcc;
		}
	} while(0);

	CoUninitialize();

	return bRet;
}

}