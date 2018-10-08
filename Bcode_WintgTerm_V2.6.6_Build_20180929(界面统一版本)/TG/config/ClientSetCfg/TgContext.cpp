#include "StdAfx.h"
#include ".\tgcontext.h"
#include ".\tgctrl.h"
#include ".\tgdoc.h"
#import  <msxml.dll> named_guids
using namespace MSXML;

int GetMutiParam(const CString& strentry, int exit_entry[], const int maxlimit)
{
	CString tmp,str=strentry;
	if(str=="")
		return 0;

	int num=0;
	do
	{
		int pos=str.Find('*',0);
		if(pos==-1)
		{
			exit_entry[num]=atoi(str);
			num++;
			break;
		}
		tmp=str.Left(pos);
		exit_entry[num]=atoi(tmp);
		num++;

		str=str.Right(str.GetLength()-pos-1);
	}while(maxlimit>num);

	return num;
}

CTgContext::CTgContext(void)
{
	m_bOnlyShowSelLine=false;
	m_datapath = ".\\data";
	// 0:十分格只显示分,分%10 1:十分格显示分(分%10).秒, 2:小时格显示分.秒
	m_nADStytle=0;
	// 跳跃车站绘制标志 1:无标志 2:接入交出 3:连线"
	m_nLeapLineFlag = 3;
	// 1 ：技术停点绘制‘▽’ 0:非技术停点绘制‘▽’  -1：不绘制
	m_nDrawTenicalStop = -1;

	// 进路准备好标志----0: 不绘制 1: 紫色线 2: 圆圈
	m_nRouteOkFlag = 1;

	// 统计早晚点 1: 统计货车 2: 统计客车 3: 统计客货车
	m_nLateEarlyTime = 0;

	// 运行线粗细
	m_nLineWidth = LINE::THIN;

	// 不显示机车号
	m_bShowTractionEngine = false;

	// 显示列车类型 1: 客车 2: 货车 3: 客货车
	m_nShowTrainFlag = 3;
	// 显示列车方向 1: 上行 2: 下行 3: 上下行
	m_nShowDirectFlag=3;

	// 是否允许修改基本图
	m_nModifyBasicSchd = 1;
	mTrainFuncCtrlWord = 0;   
	mMarkFuncCtrlWord = 0; 
	mAppCtrlWord = 0;
	m_oper_right = OPER_RIGHT::NONE_EDIT;
	m_wintg_oper_right =0x8;
	m_lctrlword = 0;
	mDebugCtrlWord = 0;
	mAppCtrlWordEx=0;
	m_nShowSideEntryNoPass=0;
	m_nMouseMoveLineFlag=1;
	m_nDisplayUnknownText=1;
	m_bVerifyTrainRect=false;
	m_nZoomRangeX=100;
	m_nZoomRangeY=100;

	m_bElecDrawGreenDecorate=true;
	m_bEnableLockSide=false;
	m_bEnableCZH=false;
	m_bEnableNewCZH=false;
	m_bSetReturnTrainID=true;
	m_bIsMoveSameCZTrain=false;

	m_jcolor=RGB(0,0,0);
	m_fcolor=RGB(0,255,0);


	swtichStation.clear();
	notshowsidectcstation.clear();
	mCrhStopStation.clear();
	errorSideStation.clear();
	m_nModifyShiftSchd=0;
	// 限制实际点修改时间
	m_nLimitTimeLen=0;
	m_bLimitModifyActTime=false;

	this->m_bIsShowAcceptChioceDlg=false;

	this->nAutoAddBlockEndTime=0;
	this->nAutoAddBlockTypeCount=0;

	m_bProhibitDeleteRealTime=false;
	
	m_nCurrentPixelPerMinute=4;
	mSupportInterCityStPlan=false;

	m_bModifyRelationSetTrainImport=false;
	m_bDrawSideNamePerHours=false;
	m_bDefaultShowSide=false;
	m_bRelationTrainBold=false;
}

int CTgContext::GetPixelPerMinute(void)
{
	return m_nCurrentPixelPerMinute;
}

void CTgContext::SetPixelPerMinute(int nPixelPerMinute)
{
	if(nPixelPerMinute>=2)
		m_nCurrentPixelPerMinute = nPixelPerMinute;
}

int CTgContext::IncreateCurrentUnit(short count) 
{
	int val = m_nCurrentPixelPerMinute + (m_nCurrentPixelPerMinute*count)/100;
	if(m_nCurrentPixelPerMinute==val)
		val += 1;
	if(val<=24)
	{
		m_nCurrentPixelPerMinute=val;
	}
	else
	{
		m_nCurrentPixelPerMinute=24;
	}
	return m_nCurrentPixelPerMinute;
}

int CTgContext::DecreateCurrentUnit(short count) 
{ 
	int val = m_nCurrentPixelPerMinute - (m_nCurrentPixelPerMinute*count)/100;
	if(val>=3)
	{
		m_nCurrentPixelPerMinute=val;
	}
	else
	{
		m_nCurrentPixelPerMinute=3;
	}
	return m_nCurrentPixelPerMinute;
}

CTgContext::~CTgContext(void)
{
}

bool CTgContext::IsShowSideEntryNoPassFlag(COLORREF& clr)
{
	clr=m_clrNoPassSide;
	if(m_nShowSideEntryNoPass>0)
	{
		return true;
	}
	return false;
}

int GetTokenInt(const CString& instr, int token[], const int maxlimit)
{
	CString tmp,str=instr;
	if(str=="")
		return 0;

	int num=0;
	do
	{
		int pos=str.Find('*',0);
		if(pos==-1)
		{
			token[num]=atoi(str);
			num++;
			break;
		}
		tmp=str.Left(pos);
		token[num]=atoi(tmp);
		num++;

		str=str.Right(str.GetLength()-pos-1);
	}while(maxlimit>num);

	return num;
}

bool CTgContext::LoadTGNoXML(CString pathname)
{
	memset(mAppWordTable, 0, sizeof(mAppWordTable));
	memset(mAppExWordTable, 0, sizeof(mAppExWordTable));
	mAppCtrlWord = 0;
	mAppCtrlWordEx=0;
	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		AfxMessageBox(er.ErrorMessage());
		return false;
	}
	
	if(VARIANT_TRUE != pXMLDoc->load( _bstr_t( pathname ) ) )
	{
		CString  prompt;
		prompt.Format("配置%s不存在或者不符合xml语法(可以用浏览器检查是否能打开)", pathname);
		AfxMessageBox(prompt);
		
		return false; 
	}
	
	MSXML::IXMLDOMNodePtr pChildNode,pNode,pRootNode;

	pRootNode = pXMLDoc->selectSingleNode( "GPC_TG_DEFINE");
	if(pRootNode == NULL)
		return false;
  
	this->m_nStationBlockShowMode=0;
	pNode = pRootNode->selectSingleNode("BlockShow");
	if(pNode)
	{
		// @set
		pChildNode = pNode->selectSingleNode("@set");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			m_nStationBlockShowMode = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}

	// 到发时间显示样式
	m_nADStytle=0;
	pNode = pRootNode->selectSingleNode("ADTimeStytle");
	if(pNode)
	{
		// @set
		pChildNode = pNode->selectSingleNode("@set");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			m_nADStytle = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}

	// 进路已触发标志
	this->m_nRouteOkFlag=0;
	m_jcolor = RGB(0,0,0);
	m_fcolor = RGB(0,255,0);
	pNode = pRootNode->selectSingleNode("RouteOkMark");
	if(pNode)
	{
		// @set
		pChildNode = pNode->selectSingleNode("@set");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			m_nRouteOkFlag = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		// @jcolor 股道颜色不同
		pChildNode = pNode->selectSingleNode("@jcolor");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			CString rgb(v.bstrVal);
			int R,G,B;
			sscanf(rgb.GetBuffer(), "%d-%d-%d", &R,&G,&B);
	
			m_jcolor = RGB(R,G,B);		
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		// @fcolor 股道颜色不同
		pChildNode = pNode->selectSingleNode("@fcolor");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			CString rgb(v.bstrVal);
			int R,G,B;
			sscanf(rgb.GetBuffer(), "%d-%d-%d", &R,&G,&B);
	
			m_fcolor = RGB(R,G,B);		
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}
	
	mCrhStopStation.clear();
    pNode = pRootNode->selectSingleNode("CRHStopStation");
	if(pNode)
	{
		// @set
		pChildNode = pNode->selectSingleNode("@set");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
             
			int token[50];
			int token_num=GetTokenInt(CString(v.bstrVal), token, sizeof(token)/sizeof(token[0]));
			for(int i=0; i<token_num; i++)
				AddCrhStopStation(token[i]);

			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}

	this->m_nRouteTrigerMode=0;
	pNode = pRootNode->selectSingleNode("RouteTrigFlag");
	if(pNode)
	{
		// @set
		pChildNode = pNode->selectSingleNode("@set");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			m_nRouteTrigerMode = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}

	m_bRelationTrainBold=false;
	pNode = pRootNode->selectSingleNode("RelationTrainBold");
	if(pNode)
	{
		// @set
		pChildNode = pNode->selectSingleNode("@set");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			int t = atoi(CString(v.bstrVal));
			if(t==1)
				m_bRelationTrainBold=true;
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}

	m_bDefaultShowSide=false;
	pNode = pRootNode->selectSingleNode("DefaultShowSide");
	if(pNode)
	{
		// @set
		pChildNode = pNode->selectSingleNode("@set");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			int t = atoi(CString(v.bstrVal));
			if(t==1)
				m_bDefaultShowSide=true;
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}

	m_bDrawSideNamePerHours=false;
	pNode = pRootNode->selectSingleNode("DrawSideNamePerHours");
	if(pNode)
	{
		// @set
		pChildNode = pNode->selectSingleNode("@set");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			int t = atoi(CString(v.bstrVal));
			if(t==1)
				m_bDrawSideNamePerHours=true;
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}

    m_bModifyRelationSetTrainImport=false;
	pNode = pRootNode->selectSingleNode("ModifyRelationSetTrainImport");
	if(pNode)
	{
		// @set
		pChildNode = pNode->selectSingleNode("@set");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			int t = atoi(CString(v.bstrVal));
			if(t==1)
				m_bModifyRelationSetTrainImport=true;
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}
	
	this->m_nRouteOkInhibitModifySchd=0; // 允许
	this->m_nMoveAfterTm=1;
	pNode = pRootNode->selectSingleNode("RouteOkInhibitModifySchd");
	if(pNode)
	{
		// @set
		pChildNode = pNode->selectSingleNode("@set");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			m_nRouteOkInhibitModifySchd = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		// @move
		pChildNode = pNode->selectSingleNode("@move");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			m_nMoveAfterTm = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}

	// 车组连接线配置
	this->m_clrCZConnectLine=RGB(255,0,255); 
	this->m_nCZConnectLineSize=2;
	pNode = pRootNode->selectSingleNode("CZHConnectLine");
	if(pNode)
	{
		// @set
		pChildNode = pNode->selectSingleNode("@set");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			m_nCZConnectLineSize = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
			if(m_nCZConnectLineSize<=0)
				m_nCZConnectLineSize=1;
			if(m_nCZConnectLineSize>2)
				m_nCZConnectLineSize=2;
		}
		// @color
		pChildNode = pNode->selectSingleNode("@color");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			CString rgb(v.bstrVal);
			int R,G,B;
			sscanf(rgb.GetBuffer(), "%d-%d-%d", &R,&G,&B);
			m_clrCZConnectLine = RGB(R,G,B);		

			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}

	this->m_nShowSideEntryNoPass=0; // 显示股道出入口不同标志
	m_clrNoPassSide = RGB(0,0,255);
	pNode = pRootNode->selectSingleNode("ShowSideEntryNoPass");
	if(pNode)
	{
		// @set
		pChildNode = pNode->selectSingleNode("@set");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			m_nShowSideEntryNoPass = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		// @color 股道颜色不同
		pChildNode = pNode->selectSingleNode("@color");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			CString rgb(v.bstrVal);
			int R,G,B;
			sscanf(rgb.GetBuffer(), "%d-%d-%d", &R,&G,&B);
	
			m_clrNoPassSide = RGB(R,G,B);		
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}

	pNode = pRootNode->selectSingleNode("MinuteWidth");
	if(pNode)
	{
		int nSet=0;
		// @set
		pChildNode = pNode->selectSingleNode("@set");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			nSet = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}

	pNode = pRootNode->selectSingleNode("OnlyShowSelLine");
	if(pNode)
	{
		int nSet=0;
		// @set
		pChildNode = pNode->selectSingleNode("@set");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			nSet = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		if(nSet>0)
			m_bOnlyShowSelLine=true;
		else
			m_bOnlyShowSelLine=false;
		
	}

	this->m_nColorManualReal = 0;
	pNode = pRootNode->selectSingleNode("ManualBaodianColor");
	if(pNode)
	{
		// @set
		int set=0;
		pChildNode = pNode->selectSingleNode("@set");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			set = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}

		if(set == 1)
		{
			pChildNode = pNode->selectSingleNode("@color");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				CString rgb(v.bstrVal);
				int R,G,B;
				sscanf(rgb.GetBuffer(), "%d-%d-%d", &R,&G,&B);
		
				m_nColorManualReal = RGB(R,G,B);		
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			else
			{
				m_nColorManualReal = RGB(255,128,64);
			}
		}
	}

	this->m_bIsShowAcceptChioceDlg=false;
	pNode = pRootNode->selectSingleNode("ShowAcceptChioceDlg");
	if(pNode)
	{
		// @set
		pChildNode = pNode->selectSingleNode("@set");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			int set = atoi(CString(v.bstrVal));
			if(set>0)
				m_bIsShowAcceptChioceDlg=true;
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}

	this->nAutoAddBlockEndTime=0;
	this->nAutoAddBlockTypeCount=0;
	pNode = pRootNode->selectSingleNode("AutoAddBlockEndTime");
	if(pNode)
	{
		nAutoAddBlockTypeCount=3;
		AutoAddBlockType[0]=SIDE_SLOW_MARK;
		AutoAddBlockType[1]=SIDE_OCCUPY_MARK;
		AutoAddBlockType[2]=ABS_SLOW_MARK;
		
		// @set
		pChildNode = pNode->selectSingleNode("@set");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			int set = atoi(CString(v.bstrVal));
			if(set>=0)
				nAutoAddBlockEndTime=set;
			SysFreeString(v.bstrVal);
			v.Clear();
		}

		if(nAutoAddBlockEndTime<=0)
		{
			nAutoAddBlockTypeCount=0;
			nAutoAddBlockEndTime=0;
		}
	}

	// 未知列车是否显示"未知列车"字样 0:不显示 1:默认值,显示
	this->m_nDisplayUnknownText = 1;
	pNode = pRootNode->selectSingleNode("UnknownText");
	if(pNode)
	{
		// @set
		pChildNode = pNode->selectSingleNode("@set");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			this->m_nDisplayUnknownText = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}

	this->m_nZoomRangeX=100;
	this->m_nZoomRangeY=100;
	this->m_bVerifyTrainRect = false;
	pNode = pRootNode->selectSingleNode("TrainRect");
	if(pNode)
	{
		// @set
		pChildNode = pNode->selectSingleNode("@set");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			int set = atoi(CString(v.bstrVal));
			if(set==1)
			{
				m_bVerifyTrainRect=true;
			}
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		// @range
		pChildNode = pNode->selectSingleNode("@rangeX");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			int set = atoi(CString(v.bstrVal));
			if(set>0 && set<200)
			{
				m_nZoomRangeX+=set;
			}
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		// @range
		pChildNode = pNode->selectSingleNode("@rangeY");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			int set = atoi(CString(v.bstrVal));
			if(set>0 && set<200)
			{
				m_nZoomRangeY+=set;
			}
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}
	
	// 鼠标挪动运行线及显示方式: 0:整分挪线,不显示秒级 1:默认值,秒级挪线,整分显示 2:秒级挪线,秒级显示
	this->m_nMouseMoveLineFlag = 1;
	pNode = pRootNode->selectSingleNode("MouseMoveLine");
	if(pNode)
	{
		// @set
		pChildNode = pNode->selectSingleNode("@set");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			this->m_nMouseMoveLineFlag = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}
	if(m_nMouseMoveLineFlag!=1 && m_nMouseMoveLineFlag!=0 && m_nMouseMoveLineFlag!=2)
		m_nMouseMoveLineFlag=1;

	// TABLE
	MSXML::IXMLDOMNodeListPtr list;
	
	list= pRootNode->selectNodes( "DEBUG/CTRL");
	if(NULL != list)
	{
		for(int idx = 0; idx < list->Getlength(); idx++)
		{
			pNode = list->Getitem(idx);
			if(pNode)
			{
				// @id
				pChildNode = pNode->selectSingleNode("@id");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					int id = atoi(CString(v.bstrVal));
					SysFreeString(v.bstrVal);
					v.Clear();

					mDebugCtrlWord += 1<<(id-1);
				}
			}
		}
	}

	list= pRootNode->selectNodes( "TABLE/APP_CTRL/CTRL");
	if(NULL != list)
	{
		for(int idx = 0; idx < list->Getlength(); idx++)
		{
			pNode = list->Getitem(idx);
			if(pNode)
			{
				int  id = 0;
				long set = 0;
				
				// @id
				pChildNode = pNode->selectSingleNode("@id");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					id = atoi(CString(v.bstrVal));
					SysFreeString(v.bstrVal);
					v.Clear();
				}

				// @set
				pChildNode = pNode->selectSingleNode("@set");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					sscanf(CString(v.bstrVal), "%x", &set);
					SysFreeString(v.bstrVal);
					v.Clear();
				}

				if(0<id && id<sizeof(mAppWordTable)/sizeof(mAppWordTable[0]))
				   mAppWordTable[id] = set;
			}
		}
	}
	list= pRootNode->selectNodes( "TABLE/APP_EX_CTRL/CTRL");
	if(NULL != list)
	{
		for(int idx = 0; idx < list->Getlength(); idx++)
		{
			pNode = list->Getitem(idx);
			if(pNode)
			{
				int  id = 0;
				long set = 0;
				
				// @id
				pChildNode = pNode->selectSingleNode("@id");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					id = atoi(CString(v.bstrVal));
					SysFreeString(v.bstrVal);
					v.Clear();
				}

				// @set
				pChildNode = pNode->selectSingleNode("@set");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					sscanf(CString(v.bstrVal), "%x", &set);
					SysFreeString(v.bstrVal);
					v.Clear();
				}

				if(0<id && id<sizeof(mAppExWordTable)/sizeof(mAppExWordTable[0]))
					mAppExWordTable[id] = set;
			}
		}
	}
	
	// -------------------------- RIGHT ------------------------------
    list= pRootNode->selectNodes( "RIGHT/APP/CTRL");
	if(NULL != list)
	{
		MSXML::IXMLDOMNodePtr pChildNode,pNode;
		for(int idx = 0; idx < list->Getlength(); idx++)
		{
			pNode = list->Getitem(idx);
			if(pNode)
			{
				int id = 0;
				// @id
				pChildNode = pNode->selectSingleNode("@id");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					id = atoi(CString(v.bstrVal));
					SysFreeString(v.bstrVal);
					v.Clear();
	                
					if(id>0 && id < sizeof(mAppWordTable)/sizeof(mAppWordTable[0]))
						mAppCtrlWord += mAppWordTable[id];
				}
				else
				{
					int begin_id = 0, end_id = 0;
					// @begin_id
					pChildNode = pNode->selectSingleNode("@begin_id");
					if(pChildNode)
					{
						_variant_t v;
						v = pChildNode->GetnodeTypedValue();
						begin_id = atoi(CString(v.bstrVal));
						SysFreeString(v.bstrVal);
						v.Clear();
					}
					// @end_id
					pChildNode = pNode->selectSingleNode("@end_id");
					if(pChildNode)
					{
						_variant_t v;
						v = pChildNode->GetnodeTypedValue();
						end_id = atoi(CString(v.bstrVal));
						SysFreeString(v.bstrVal);
						v.Clear();
					}
					
					for(int idx=begin_id; idx<=end_id; idx++)
					{
						if(idx>0 && idx < sizeof(mAppWordTable)/sizeof(mAppWordTable[0]))
							mAppCtrlWord += mAppWordTable[idx];
					}
				}
			}
		}
	}
	list= pRootNode->selectNodes( "RIGHT/APP_EX/CTRL");
	if(NULL != list)
	{
		MSXML::IXMLDOMNodePtr pChildNode,pNode;
		for(int idx = 0; idx < list->Getlength(); idx++)
		{
			pNode = list->Getitem(idx);
			if(pNode)
			{
				int id = 0;
				// @id
				pChildNode = pNode->selectSingleNode("@id");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					id = atoi(CString(v.bstrVal));
					SysFreeString(v.bstrVal);
					v.Clear();
	                
					if(id>0 && id < sizeof(mAppExWordTable)/sizeof(mAppExWordTable[0]))
						mAppCtrlWordEx += mAppExWordTable[id];
				}
				else
				{
					int begin_id = 0, end_id = 0;
					// @begin_id
					pChildNode = pNode->selectSingleNode("@begin_id");
					if(pChildNode)
					{
						_variant_t v;
						v = pChildNode->GetnodeTypedValue();
						begin_id = atoi(CString(v.bstrVal));
						SysFreeString(v.bstrVal);
						v.Clear();
					}
					// @end_id
					pChildNode = pNode->selectSingleNode("@end_id");
					if(pChildNode)
					{
						_variant_t v;
						v = pChildNode->GetnodeTypedValue();
						end_id = atoi(CString(v.bstrVal));
						SysFreeString(v.bstrVal);
						v.Clear();
					}
					
					for(int idx=begin_id; idx<=end_id; idx++)
					{
						if(idx>0 && idx < sizeof(mAppExWordTable)/sizeof(mAppExWordTable[0]))
							mAppCtrlWordEx += mAppExWordTable[idx];
					}
				}
			}
		}
	}
	m_genUpdateNodeIndex = false;
	pNode = pRootNode->selectSingleNode( "GenNodeIndex");
	if(pNode)
	{
		pChildNode = pNode->selectSingleNode( "@set");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			if(atoi(CString(v.bstrVal)) > 0)
			    m_genUpdateNodeIndex = true;
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}
	return true;
}

void CTgContext::SetEnableLockSide(int enable)
{
	if(enable>0)
		m_bEnableLockSide=true;
	else
		m_bEnableLockSide=false;
}

void CTgContext::SetElecDrawGreenDecorate(int enable)
{
	if(enable>0)
		m_bElecDrawGreenDecorate=true;
	else
		m_bElecDrawGreenDecorate=false;
}

void CTgContext::EnableCZH(int enable)
{
	if(enable>0)
		m_bEnableCZH=true;
	else
		m_bEnableCZH=false;
}

void CTgContext::EnableNewCZH(int enable)
{
	if(enable>0)
		m_bEnableNewCZH=true;
	else
		m_bEnableNewCZH=false;
}

void CTgContext::EnableSetCZHReturnTrainID(int enable)
{
	if(enable>0)
		m_bSetReturnTrainID=true;
	else
		m_bSetReturnTrainID=false;
}

void CTgContext::EnableInterCityStplan(int enable)
{
	if(enable>0)
		mSupportInterCityStPlan=true;
	else
		mSupportInterCityStPlan=false;
}

void CTgContext::SetMoveSameCZTrain(bool b)
{
	m_bIsMoveSameCZTrain=b;
}

bool CTgContext::IsMoveSameCZTrain() 
{
	return m_bIsMoveSameCZTrain;
}

void CTgContext::LockSideNotTechStop(int enable)
{
	if (enable>0)
		m_nLockSideNotTechStop = true;
	else
		m_nLockSideNotTechStop = false;
}

bool CTgContext::IsSwtichStation(int station)
{
	for(int i=0; i<swtichStation.size(); i++)
	{
		if(swtichStation[i] == station)
			return true;
	}

	return false;
}
	
void CTgContext::AddSwtichStation(int station)
{
	int i=0;
	for(i=0; i<swtichStation.size(); i++)
	{
		if(swtichStation[i] == station)
			break;
	}

	if(i>=swtichStation.size())
		swtichStation.push_back(station);

	return;
}

bool CTgContext::IsNotShowSideCtcStation(int station)
{
	for(int i=0; i<notshowsidectcstation.size(); i++)
	{
		if(notshowsidectcstation[i] == station)
			return true;
	}

	return false;
}
	
void CTgContext::AddNotShowSideCtcStation(int station)
{
	int i=0;
	for(i=0; i<notshowsidectcstation.size(); i++)
	{
		if(notshowsidectcstation[i] == station)
			break;
	}

	if(i>=notshowsidectcstation.size())
		notshowsidectcstation.push_back(station);

	return;
}

bool CTgContext::IsErrorSideStation(int station)
{
	for(int i=0; i<errorSideStation.size(); i++)
	{
		if(errorSideStation[i] == station)
			return true;
	}

	return false;
}

void CTgContext::AddErrorSideStation(int station)
{
	int i=0;
	for(i=0; i<errorSideStation.size(); i++)
	{
		if(errorSideStation[i] == station)
			break;
	}

	if(i>=errorSideStation.size())
		errorSideStation.push_back(station);

	return;
}

int  CTgContext::GetAutoAddBlockEndTime()
{
	if(nAutoAddBlockEndTime<=0 || nAutoAddBlockTypeCount<=0)
		return 0;

	return nAutoAddBlockEndTime;
}

bool CTgContext::IsAutoAddBlockEndTime(int type)
{
	if(nAutoAddBlockEndTime<=0 || nAutoAddBlockTypeCount<=0)
		return false;

	for(int i=0; i<nAutoAddBlockTypeCount; i++)
	{
		if(AutoAddBlockType[i]==type)
			return true;
	}

	return false;
}

void CTgContext::AddCrhStopStation(int station)
{
	for(int i=0; i<mCrhStopStation.size(); i++)
	{
		if(mCrhStopStation[i]==station)
			break;
	}

	if(i>=mCrhStopStation.size())
	{
		mCrhStopStation.push_back(station);
	}
}

bool CTgContext::IsCrhStopStation(int station)
{
	for(int i=0; i<mCrhStopStation.size(); i++)
	{
		if(mCrhStopStation[i]==station)
			return true;
	}

	return false;
}

void CTgContext::SetProhibitDeleteRealTimeFlag(short val)
{ 
	if (val>0)
		m_bProhibitDeleteRealTime = true;
	else
		m_bProhibitDeleteRealTime = false;
}


