/*
 * TCC 实现文件
 *
 * 本文件实现TCC命令与XML文件转换
 */

#include "stdafx.h"
#include "Tcc_Base.h"


//#include <MsXml2.h>
//#include <comutil.h>
//#import <msxml4.dll>
//#import <msxml4.dll> raw_interface_only
//using namespace MSXML2;
//#pragma comment(lib, "msxml2")
//#pragma comment(lib, "comsupp")
namespace TCC {
/*
<RAW_STOCK_COMMAND>
	<COMMAND id="1" raw_sysid="xxxxxxxx-xxxx-xxxx-xxxxxxxxxxxxxx" console="1" isCancelLimit="0"
		BeginStation="1" EndStation="2" AbsLineID="1"
		isFullLimit="0" BeginKmMark="0" EndKmMark="0"
		isStartNow="1" BeginTime="0" isEndless="1" EndTime="0"
		creator="1" checkor="1" confirmor="1">

		<TCC id="1" line="1">
		<TCC id="2" line="1">

		<TRAINCMD sys_id="" speed="" TrainID="">
			<TCC status="" sender="" time="" />
			<TCC status="" sender="" time="" />
		</TRAINCMD>
		<TRAINCMD sys_id="" speed="" TrainID="">
			<TCC status="" sender="" time="" />
			<TCC status="" sender="" time="" />
		</TRAINCMD>
		<TRAINCMD sys_id="" speed="" TrainID="">
			<TCC status="" sender="" time="" />
			<TCC status="" sender="" time="" />
		</TRAINCMD>
		<TRAINCMD sys_id="" speed="" TrainID="">
			<TCC status="" sender="" time="" />
			<TCC status="" sender="" time="" />
		</TRAINCMD>
		<TRAINCMD sys_id="" speed="" TrainID="">
			<TCC status="" sender="" time="" />
			<TCC status="" sender="" time="" />
		</TRAINCMD>
	</COMMAND
</RAW_STOCK_COMMAND>
*/
BOOL Serialize(MSXML2::IXMLDOMNodePtr pCmdNode, RAW_STOCK_COMMAND &cmd, BOOL bLoad)
{
	ASSERT(NULL != pCmdNode);

	do {
		CString str;
		_variant_t varVal;
		
		MSXML2::IXMLDOMNodePtr pNode;

		// 命令号
		if(pNode = pCmdNode->selectSingleNode("@id"))
		{
			if(bLoad)
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				cmd.nCmdID = atoi(str);
			}
			else
			{
				str.Format("%d", cmd.nCmdID);
				varVal = str;
				pNode->PutnodeTypedValue(varVal);
			}
			SysFreeString(varVal.bstrVal);
			varVal.Clear();
		}
		
		if(pNode = pCmdNode->selectSingleNode("@raw_sysid"))
		{
			if(bLoad)
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				cmd.raw_sysid = SysidFromString(str);
			}
			else
			{
				varVal = SysidToString(cmd.raw_sysid);
				pNode->PutnodeTypedValue(varVal);
			}
			SysFreeString(varVal.bstrVal);
			varVal.Clear();
		}

		// 调度台台号
		if(pNode = pCmdNode->selectSingleNode("@console"))
		{
			if(bLoad)
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				cmd.nConsoleID = atoi(str);
			}
			else
			{
				str.Format("%d", cmd.nConsoleID);
				varVal = str;
				pNode->PutnodeTypedValue(varVal);
			}
			SysFreeString(varVal.bstrVal);
			varVal.Clear();
		}

		// 取消限速
		if(pNode = pCmdNode->selectSingleNode("@isCancelLimit"))
		{
			if(bLoad)
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				cmd.bCancelLimit = atoi(str);
			}
			else
			{
				str.Format("%lu", cmd.bCancelLimit);
				varVal = str;
				pNode->PutnodeTypedValue(varVal);
			}
			SysFreeString(varVal.bstrVal);
			varVal.Clear();
		}
		
		// BeginStation
		if(pNode = pCmdNode->selectSingleNode("@BeginStation"))
		{
			if(bLoad)
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				cmd.nStaID[0] = atoi(str);
			}
			else
			{
				str.Format("%lu", cmd.nStaID[0]);
				varVal = str;
				pNode->PutnodeTypedValue(varVal);
			}
			SysFreeString(varVal.bstrVal);
			varVal.Clear();
		}
		
		// EndStation
		if(pNode = pCmdNode->selectSingleNode("@EndStation"))
		{
			if(bLoad)
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				cmd.nStaID[1] = atoi(str);
			}
			else
			{
				str.Format("%lu", cmd.nStaID[1]);
				varVal = str;
				pNode->PutnodeTypedValue(varVal);
			}
			SysFreeString(varVal.bstrVal);
			varVal.Clear();
		}
		
		// AbslineID
		if(pNode = pCmdNode->selectSingleNode("@AbslineID"))
		{
			if(bLoad)
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				cmd.nLineID = atoi(str);
			}
			else
			{
				str.Format("%lu", cmd.nLineID);
				varVal = str;
				pNode->PutnodeTypedValue(varVal);
			}
			SysFreeString(varVal.bstrVal);
			varVal.Clear();
		}

		// 全线限速
		if(pNode = pCmdNode->selectSingleNode("@isFullLimit"))
		{
			if(bLoad)
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				cmd.bFullLimit = atoi(str);
			}
			else
			{
				str.Format("%lu", cmd.bFullLimit);
				varVal = str;
				pNode->PutnodeTypedValue(varVal);
			}
			SysFreeString(varVal.bstrVal);
			varVal.Clear();
		}
		
		// BeginKmMark
		if(pNode = pCmdNode->selectSingleNode("@BeginKmMark"))
		{
			if(bLoad)
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				cmd.nKmMark[0] = atoi(str);
			}
			else
			{
				str.Format("%lu", cmd.nKmMark[0]);
				varVal = str;
				pNode->PutnodeTypedValue(varVal);
			}
			SysFreeString(varVal.bstrVal);
			varVal.Clear();
		}
		
		// EndKmMark
		if(pNode = pCmdNode->selectSingleNode("@EndKmMark"))
		{
			if(bLoad)
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				cmd.nKmMark[1] = atoi(str);
			}
			else
			{
				str.Format("%lu", cmd.nKmMark[1]);
				varVal = str;
				pNode->PutnodeTypedValue(varVal);
			}
			SysFreeString(varVal.bstrVal);
			varVal.Clear();
		}

		// 立即开始
		if(pNode = pCmdNode->selectSingleNode("@isStartNow"))
		{
			if(bLoad)
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				cmd.bStartNow = atoi(str);
			}
			else
			{
				str.Format("%lu", cmd.bStartNow);
				varVal = str;
				pNode->PutnodeTypedValue(varVal);
			}
			SysFreeString(varVal.bstrVal);
			varVal.Clear();
		}
		
		// BeginTime
		if(pNode = pCmdNode->selectSingleNode("@BeginTime"))
		{
			if(bLoad)
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				cmd.nTime[0] = atoi(str);
			}
			else
			{
				str.Format("%lu", cmd.nTime[0]);
				varVal = str;
				pNode->PutnodeTypedValue(varVal);
			}
			SysFreeString(varVal.bstrVal);
			varVal.Clear();
		}

		// 永久有效
		if(pNode = pCmdNode->selectSingleNode("@isEndless"))
		{
			if(bLoad)
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				cmd.bEndless = atoi(str);
			}
			else
			{
				str.Format("%lu", cmd.bEndless);
				varVal = str;
				pNode->PutnodeTypedValue(varVal);
			}
			SysFreeString(varVal.bstrVal);
			varVal.Clear();
		}
		
		// EndTime
		if(pNode = pCmdNode->selectSingleNode("@EndTime"))
		{
			if(bLoad)
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				cmd.nTime[1] = atoi(str);
			}
			else
			{
				str.Format("%lu", cmd.nTime[1]);
				varVal = str;
				pNode->PutnodeTypedValue(varVal);
			}
			SysFreeString(varVal.bstrVal);
			varVal.Clear();
		}
		
		// creator
		if(pNode = pCmdNode->selectSingleNode("@creator"))
		{
			if(bLoad)
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				cmd.nCreatorID = atoi(str);
			}
			else
			{
				str.Format("%lu", cmd.nCreatorID);
				varVal = str;
				pNode->PutnodeTypedValue(varVal);
			}
			SysFreeString(varVal.bstrVal);
			varVal.Clear();
		}
		
		// checkor
		if(pNode = pCmdNode->selectSingleNode("@checkor"))
		{
			if(bLoad)
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				cmd.nCheckorID = atoi(str);
			}
			else
			{
				str.Format("%lu", cmd.nCheckorID);
				varVal = str;
				pNode->PutnodeTypedValue(varVal);
			}
			SysFreeString(varVal.bstrVal);
			varVal.Clear();
		}
		
		// confirmor
		if(pNode = pCmdNode->selectSingleNode("@confirmor"))
		{
			if(bLoad)
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				cmd.nConfirmorID = atoi(str);
			}
			else
			{
				str.Format("%lu", cmd.nConfirmorID);
				varVal = str;
				pNode->PutnodeTypedValue(varVal);
			}
			SysFreeString(varVal.bstrVal);
			varVal.Clear();
		}

		//bGenCanCelCmd
		if(pNode = pCmdNode->selectSingleNode("@bGenCanCelCmd"))
		{
			if(bLoad)
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				cmd.bGenCancelCommand = atoi(str);
			}
			else
			{
				str.Format("%lu", cmd.bGenCancelCommand);
				varVal = str;
				pNode->PutnodeTypedValue(varVal);
			}
			SysFreeString(varVal.bstrVal);
			varVal.Clear();
		}
		
		MSXML2::IXMLDOMNodeListPtr pNodeList;
		// TCC
		pNodeList = pCmdNode->selectNodes( "TCC" );
		cmd.tcc.resize(pNodeList->Getlength());
		for(int i=0; i<pNodeList->Getlength(); i++)
		{
			MSXML2::IXMLDOMNodePtr pNode_TCC = pNodeList->Getitem(i);

			// 受令站 站号
			if(pNode = pNode_TCC->selectSingleNode("@id"))
			{
				if(bLoad)
				{
					varVal = pNode->GetnodeTypedValue();
					str = varVal.bstrVal;
					cmd.tcc[i].nID = atoi(str);
				}
				else
				{
					str.Format("%lu", cmd.tcc[i].nID);
					varVal = str;
					pNode->PutnodeTypedValue(varVal);
				}
				SysFreeString(varVal.bstrVal);
				varVal.Clear();
			}

			// 受令站 线路号line
			if(pNode = pNode_TCC->selectSingleNode("@line"))
			{
				if(bLoad)
				{
					varVal = pNode->GetnodeTypedValue();
					str = varVal.bstrVal;
					cmd.tcc[i].nTccLineID = atoi(str);
				}
				else
				{
					str.Format("%lu", cmd.tcc[i].nTccLineID);
					varVal = str;
					pNode->PutnodeTypedValue(varVal);
				}
				SysFreeString(varVal.bstrVal);
				varVal.Clear();
			}

			// 受令站 需校验
			if(pNode = pNode_TCC->selectSingleNode("@needStationCheck"))
			{
				if(bLoad)
				{
					varVal = pNode->GetnodeTypedValue();
					str = varVal.bstrVal;
					cmd.tcc[i].bTccValidate = atoi(str);
				}
				else
				{
					str.Format("%lu", cmd.tcc[i].bTccValidate);
					varVal = str;
					pNode->PutnodeTypedValue(varVal);
				}
				SysFreeString(varVal.bstrVal);
				varVal.Clear();
			}

			// 受令站 checkor
			if(pNode = pNode_TCC->selectSingleNode("@checkor"))
			{
				if(bLoad)
				{
					varVal = pNode->GetnodeTypedValue();
					str = varVal.bstrVal;
					cmd.tcc[i].nCheckorID = atoi(str);
				}
				else
				{
					str.Format("%lu", cmd.tcc[i].nCheckorID);
					varVal = str;
					pNode->PutnodeTypedValue(varVal);
				}
				SysFreeString(varVal.bstrVal);
				varVal.Clear();
			}
		}
		
		// 每趟车
		pNodeList = pCmdNode->selectNodes( "TRAINCMD" );
		for(int i=0; i<pNodeList->Getlength(); i++)
		{
			MSXML2::IXMLDOMNodePtr pNode_TRAIN = pNodeList->Getitem(i);

			if(pNode = pNode_TRAIN->selectSingleNode("@sys_id"))
			{
				if(bLoad)
				{
					varVal = pNode->GetnodeTypedValue();
					str = varVal.bstrVal;
					cmd.train_cmd[i].sys_id = SysidFromString(str);
				}
				else
				{
					varVal = SysidToString(cmd.train_cmd[i].sys_id);
					pNode->PutnodeTypedValue(varVal);
				}
				SysFreeString(varVal.bstrVal);
				varVal.Clear();
			}

			if(pNode = pNode_TRAIN->selectSingleNode("@speed"))
			{
				if(bLoad)
				{
					varVal = pNode->GetnodeTypedValue();
					str = varVal.bstrVal;
					cmd.train_cmd[i].nSpeedMaxVal = atoi(str);
				}
				else
				{
					str.Format("%lu", cmd.train_cmd[i].nSpeedMaxVal);
					varVal = str;
					pNode->PutnodeTypedValue(varVal);
				}
				SysFreeString(varVal.bstrVal);
				varVal.Clear();
			}

			if(pNode = pNode_TRAIN->selectSingleNode("@TrainID"))
			{
				if(bLoad)
				{
					varVal = pNode->GetnodeTypedValue();
					cmd.train_cmd[i].szTrainID = varVal.bstrVal;
				}
				else
				{
					varVal = cmd.train_cmd[i].szTrainID;
					pNode->PutnodeTypedValue(varVal);
				}
				SysFreeString(varVal.bstrVal);
				varVal.Clear();
			}

			MSXML2::IXMLDOMNodeListPtr pNodeList2 = pNode_TRAIN->selectNodes("TCC");
			cmd.train_cmd[i].tcc.resize(pNodeList2->Getlength());
			for(int j=0; j<pNodeList2->Getlength(); j++)
			{
				MSXML2::IXMLDOMNodePtr pNode_TCC = pNodeList2->Getitem(j);

				if(pNode = pNode_TCC->selectSingleNode("@status"))
				{
					if(bLoad)
					{
						varVal = pNode->GetnodeTypedValue();
						str = varVal.bstrVal;
						cmd.train_cmd[i].tcc[j].nStatus = atoi(str);
					}
					else
					{
						str.Format("%lu", cmd.train_cmd[i].tcc[j].nStatus);
						varVal = str;
						pNode->PutnodeTypedValue(varVal);
					}
					SysFreeString(varVal.bstrVal);
					varVal.Clear();
				}

				if(pNode = pNode_TCC->selectSingleNode("@sender"))
				{
					if(bLoad)
					{
						varVal = pNode->GetnodeTypedValue();
						str = varVal.bstrVal;
						cmd.train_cmd[i].tcc[j].nSenderID = atoi(str);
					}
					else
					{
						str.Format("%lu", cmd.train_cmd[i].tcc[j].nSenderID);
						varVal = str;
						pNode->PutnodeTypedValue(varVal);
					}
					SysFreeString(varVal.bstrVal);
					varVal.Clear();
				}

				if(pNode = pNode_TCC->selectSingleNode("@time"))
				{
					if(bLoad)
					{
						varVal = pNode->GetnodeTypedValue();
						str = varVal.bstrVal;
						cmd.train_cmd[i].tcc[j].nTime = atoi(str);
					}
					else
					{
						str.Format("%lu", cmd.train_cmd[i].tcc[j].nTime);
						varVal = str;
						pNode->PutnodeTypedValue(varVal);
					}
					SysFreeString(varVal.bstrVal);
					varVal.Clear();
				}
			}
		}

		return TRUE;
	} while(0);

	return FALSE;
}

BOOL LoadRawStockCommand(
		MSXML2::IXMLDOMNodePtr pCmdNode, 
		RAW_STOCK_COMMAND &cmd)
{
	return Serialize(pCmdNode, cmd, TRUE);
}

BOOL SaveRawStockCommand(
		MSXML2::IXMLDOMNodePtr pCmdNode, 
		RAW_STOCK_COMMAND &cmd)
{
	return Serialize(pCmdNode, cmd, FALSE);
}

BOOL DeleteCommandNode(
				MSXML2::IXMLDOMDocumentPtr pDOMDoc, 
				sysid_t& sysid)
{
	MSXML2::IXMLDOMNodePtr pNode_root = pDOMDoc->selectSingleNode( "RAW_STOCK_COMMAND" );
	
	CString str;
	str.Format("COMMAND[@raw_sysid='%s']", SysidToString(sysid));
	MSXML2::IXMLDOMNodePtr pNode = pNode_root->selectSingleNode( _bstr_t(str) );
	if(pNode)
	{
		pNode_root->removeChild(pNode);
		return TRUE;
	}

	return FALSE;
}

BOOL AddCommandNode(
				 MSXML2::IXMLDOMDocumentPtr pDOMDoc, 
				 RAW_STOCK_COMMAND& cmd, 
				 BOOL bInsert)
{
	MSXML2::IXMLDOMNodePtr pNode_root = pDOMDoc->selectSingleNode( "RAW_STOCK_COMMAND" );

	CString str;

	str.Format("COMMAND[@raw_sysid='%s']", SysidToString(cmd.raw_sysid));
	MSXML2::IXMLDOMNodePtr pNode = pNode_root->selectSingleNode( _bstr_t(str) );
	if(NULL == pNode && !bInsert)
		return FALSE;

	if(pNode)
	{
		SaveRawStockCommand(pNode, cmd);
	}
	else if(bInsert)
	{
		MSXML2::IXMLDOMNodePtr pNode = pNode_root->GetfirstChild();
		MSXML2::IXMLDOMNodePtr pNewNode = pNode->cloneNode(VARIANT_TRUE);
		
		SaveRawStockCommand(pNewNode, cmd);
		
		pNode = pNode_root->appendChild(pNewNode);
	}

	return TRUE;
}



/*
 * 删除当前命令文件 nCmdID.xml
 * 由于车站PC的同步问题，删除相当于将文件置空
 */
BOOL DeleteCommand(RAW_STOCK_COMMAND& cmd, LPCSTR szFilename)
{
	HRESULT hr;
	MSXML2::IXMLDOMDocumentPtr pDOMDoc;
	hr = pDOMDoc.CreateInstance(
		__uuidof(MSXML2::DOMDocument40), 
		NULL, 
		CLSCTX_INPROC_SERVER);
	
	pDOMDoc->put_async(VARIANT_FALSE);
	pDOMDoc->put_validateOnParse(VARIANT_TRUE);
	pDOMDoc->put_resolveExternals(VARIANT_TRUE);

	_variant_t vXml(szFilename);
	VARIANT_BOOL vBool = pDOMDoc->load(vXml);
	if(VARIANT_TRUE == vBool)
	{
		MSXML2::IXMLDOMNodePtr pRootNode = pDOMDoc->selectSingleNode( "RAW_STOCK_COMMAND" );

		CString strSQL;
		strSQL.Format("COMMAND[@raw_sysid = '%s']", SysidToString(cmd.raw_sysid));
		MSXML2::IXMLDOMNodePtr pNode = pRootNode->selectSingleNode( (LPCSTR)strSQL );
		if(pNode)
			pRootNode->removeChild( pNode );
		if(NULL == pRootNode->GetfirstChild())
		{
			SysFreeString(vXml.bstrVal);
			DumpFile(szFilename);
		}
		else
		{
			pDOMDoc->save(vXml);
			SysFreeString(vXml.bstrVal);
		}
	}

	return TRUE;
}

/*
 * 过期1天半的文件 无用
 */
BOOL IsOldCommandFile(LPCSTR szFilename)
{
	CFileFind fFind;
	if(fFind.FindFile(szFilename))
	{
		fFind.FindNextFile();
		CTime ct;
		fFind.GetLastWriteTime(ct);
		if(CTime(CTime::GetCurrentTime()) - ct > CTimeSpan(1, 12, 0, 0))
		{
			fFind.Close();
			return TRUE;
		}
	}
    fFind.Close();
	return FALSE;
}

/*
 * 清空文件
 */
void DumpFile(LPCSTR szFilename)
{
	CFileFind fFind;
	if( fFind.FindFile(szFilename) )
	{
		CStdioFile file;
		file.Open(
			szFilename, 
			CFile::modeWrite | CFile::modeCreate | CFile::typeText);
	
		file.Close();
	}
}

/*
 * 2天前的空文件删除
 */
void EraseZeroFileInDir(LPCSTR szDirectory)
{
	char buf[1024];
	memset(buf, 0, sizeof(buf));

	GetCurrentDirectory(1024, buf);
	SetCurrentDirectory(szDirectory);

	CFileFind fFind;
	BOOL bFind = fFind.FindFile("*.xml");
	
	SetCurrentDirectory(buf);

	CTime ctNow(CTime::GetCurrentTime());
	while(bFind)
	{
		bFind = fFind.FindNextFile();
		CTime ct;
		fFind.GetLastWriteTime(ct);

		if( ctNow - ct > CTimeSpan(2, 0, 0, 0) && 0 == fFind.GetLength())
		{
			CString sz = szDirectory + fFind.GetFileName();
			::DeleteFile(sz);
		}
	}
	fFind.Close();
}


/*
 * 以命令号作为文件名称, 0.xml保存的都是待发命令
 *
 * 取消命令与其原始命令保存在同一个文件中
 * 
 * 已下发的命令若sys_id号不一致，则命令号已作一大循环，必须清空
 */
BOOL SaveCommand(RAW_STOCK_COMMAND& cmd, LPCSTR szFilename)
{
	CoInitialize(NULL);

	BOOL bRet = FALSE;
	do {
		HRESULT hr;
		MSXML2::IXMLDOMDocumentPtr pDOMDoc;
		hr = pDOMDoc.CreateInstance(
			__uuidof(MSXML2::DOMDocument40), 
			NULL, 
			CLSCTX_INPROC_SERVER);
		
		pDOMDoc->put_async(VARIANT_FALSE);
		pDOMDoc->put_validateOnParse(VARIANT_TRUE);
		pDOMDoc->put_resolveExternals(VARIANT_TRUE);

		_variant_t vXml(szFilename);
		VARIANT_BOOL vBool = pDOMDoc->load(vXml);
		if(VARIANT_TRUE == vBool)
		{
			MSXML2::IXMLDOMNodePtr pRootNode = pDOMDoc->selectSingleNode( "RAW_STOCK_COMMAND" );

			CString str;
			str.Format("COMMAND[@raw_sysid='%s']", SysidToString(cmd.raw_sysid));
			MSXML2::IXMLDOMNodePtr pNode = pRootNode->selectSingleNode((LPCSTR)str);
			if(pNode)
			{
				SaveRawStockCommand(pNode, cmd);
				pDOMDoc->save(vXml);
				SysFreeString(vXml.bstrVal);

				bRet = TRUE;
				break;
			}
			else
			{
				pNode = pRootNode->GetfirstChild();
				if(pNode)
				{
					MSXML2::IXMLDOMNodePtr pNewNode = pNode->cloneNode(VARIANT_TRUE);
					SaveRawStockCommand(pNewNode, cmd);
					pNode = pRootNode->appendChild(pNewNode);
					pDOMDoc->save(vXml);
					SysFreeString(vXml.bstrVal);
		
					bRet = TRUE;
					break;
				}
			}
		}
		SysFreeString(vXml.bstrVal);

		CStdioFile file;
		BOOL b = file.Open(szFilename, CFile::modeWrite | CFile::modeCreate | CFile::typeText);
		if(!b)
		{
			bRet = FALSE;
			break;
		}

		CString str;
		//str.Format("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>");
		str.Format("<?xml version=\"1.0\" encoding=\"GB2312\"?>");
		file.WriteString(str);

		str.Format("\n<RAW_STOCK_COMMAND>");
		file.WriteString(str);

		str.Format("\n\t<COMMAND id=\"%d\" raw_sysid=\"%s\" console=\"%d\" isCancelLimit=\"%d\" "
			"BeginStation=\"%d\" EndStation=\"%d\" AbslineID=\"%d\" "
			"isFullLimit=\"%d\" BeginKmMark=\"%d\" EndKmMark=\"%d\" "
			"isStartNow=\"%d\" BeginTime=\"%lu\" isEndless=\"%d\" EndTime=\"%lu\" "
			"creator=\"%d\" checkor=\"%d\" confirmor=\"%d\" bGenCanCelCmd=\"%d\" >", 
			cmd.nCmdID, SysidToString(cmd.raw_sysid), cmd.nConsoleID, cmd.bCancelLimit, 
			cmd.nStaID[0], cmd.nStaID[1], cmd.nLineID,
			cmd.bFullLimit, cmd.nKmMark[0], cmd.nKmMark[1],
			cmd.bStartNow, cmd.nTime[0], cmd.bEndless, cmd.nTime[1],
			cmd.nCreatorID, cmd.nCheckorID, cmd.nConfirmorID, cmd.bGenCancelCommand);
		file.WriteString(str);

		// 受令站(列控)
		for(int j=0; j<cmd.tcc.size(); j++)
		{
			str.Format("\n\t\t\t<TCC id=\"%lu\" line=\"%lu\" needStationCheck=\"%lu\" checkor=\"%lu\" />",
				cmd.tcc[j].nID,
				cmd.tcc[j].nTccLineID,
				cmd.tcc[j].bTccValidate,
				cmd.tcc[j].nCheckorID);
			file.WriteString(str);
		}

		for(int i=0; i<sizeof(cmd.train_cmd)/sizeof(cmd.train_cmd[0]); i++)
		{
			str.Format("\n\t\t<TRAINCMD sys_id=\"%s\" speed=\"%d\" TrainID=\"%s\">",
				SysidToString(cmd.train_cmd[i].sys_id),
				cmd.train_cmd[i].nSpeedMaxVal,
				cmd.train_cmd[i].szTrainID);
			file.WriteString(str);

			for(int j=0; j<cmd.train_cmd[i].tcc.size(); j++)
			{
				str.Format("\n\t\t\t<TCC status=\"%d\" sender=\"%d\" time=\"%d\"/>",
					cmd.train_cmd[i].tcc[j].nStatus,
					cmd.train_cmd[i].tcc[j].nSenderID,
					cmd.train_cmd[i].tcc[j].nTime);
				file.WriteString(str);
			}

			str.Format("\n\t\t</TRAINCMD>");
			file.WriteString(str);
		}

		str.Format("\n\t</COMMAND>");
		file.WriteString(str);

		str.Format("\n</RAW_STOCK_COMMAND>");
		file.WriteString(str);

		file.Close();

		bRet = TRUE;
	}while(0);

	CoUninitialize();
	return bRet;
}

BOOL LoadCommand(RAW_STOCK_COMMAND& cmd, LPCSTR szFilename)
{
	CoInitialize(NULL);

	BOOL bRet = FALSE;
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
			break;

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
		MSXML2::IXMLDOMNodeListPtr pNodeList = pXMLDOMDoc->selectNodes("RAW_STOCK_COMMAND/COMMAND");

		long length = pNodeList->Getlength();
		for(int i=0; i<length && i<1; i++)
			LoadRawStockCommand( pNodeList->Getitem( i ), cmd);

		bRet = TRUE;
	} while(0);

	CoUninitialize();
	return bRet;
}


};
