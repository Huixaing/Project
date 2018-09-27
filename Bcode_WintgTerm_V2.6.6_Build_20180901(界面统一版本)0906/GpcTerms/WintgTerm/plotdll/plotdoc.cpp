// WintgDoc.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "plotDoc.h"
#include "widgets.h"
#include "function.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#include "tg_basicpara.h"

#include <stdlib.h>
#include <crtdbg.h>
#include ".\plotdoc.h"

#import  <msxml.dll> named_guids
using namespace MSXML;
char* g_Jinan_Bianzu_Title[] ={"车       次","机 车 型 号", "司       机","车       长",
"辆       数","总       重","换       长","蓬       布"};

/////////////////////////////////////////////////////////////////////////////
// CPlotDoc

extern CView *clsPlotView;
IMPLEMENT_DYNCREATE(CPlotDoc, CDocument)

int  ConvertAbsnumStationToEntry(ushort station,ushort abs_line)
{
	ushort st1=0,etr1=255,st2=0,etr2=255;
	GetTGBasicParam()->ConvertAbsnumToStationEntry(&st1,&etr1,&st2,&etr2,abs_line);
	if(st1==station)
	{
		return etr1;
	}
	else if(st2==station)
	{
		return etr2;
	}
	return -1;
}

COLORREF CPlotDoc::GetSchdColor(int factor)
{
	LINE_COLOR clr;
	if(m_lineCfg.GetTrainLineColor(factor, clr))
		return clr.schd;
	else
	{
		if(m_lineCfg.GetTrainLineColor(255, clr))
			return clr.schd;
		else
			return RGB(100,0,0);
	}
}

COLORREF CPlotDoc::GetActualLineColor(int factor)
{
	LINE_COLOR cr;
	if(m_lineCfg.GetTrainLineColor(factor, cr))
		return cr.real;
	else
		return RGB(255,0,0); 
}

int CPlotDoc::GetLineType(int factor)
{
	LINE_TYPE style;
	if(m_lineCfg.GetTrainLineStyle(factor, style))
		return style.type;
	else
		return 21;
}

void CPlotDoc::LoadPlotXML(CString fileName, int printType, int nShiftNo)
{
	int nosame=0;
	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		AfxMessageBox(er.ErrorMessage());
		return;
	}

	if(VARIANT_TRUE != pXMLDoc->load( _bstr_t( fileName ) ) )
	{
		CString  prompt;
		prompt.Format("配置%s不存在或者不符合xml语法(可以用浏览器检查是否能打开)", fileName);

		return; 
	}
    
	MSXML::IXMLDOMNodePtr pChildNode,pNode,pRootNode;
	
	pRootNode = pXMLDoc->selectSingleNode( "plot/drawtrain");
	if(pRootNode)
	{
		_variant_t v;
		v = pRootNode->GetnodeTypedValue();
		m_nDrawTrainConsiderStationSep = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();

		if(m_nDrawTrainConsiderStationSep!=0 && m_nDrawTrainConsiderStationSep!=1)
		{
			m_nDrawTrainConsiderStationSep=0;
		}
	}

	GpcLogTrainMessage(0, 0, "打印运行图时 画车变量 %d.", m_nDrawTrainConsiderStationSep);
	
	pRootNode = pXMLDoc->selectSingleNode( "plot/font");
	if(pRootNode)
	{
		pNode = pRootNode->selectSingleNode("@time_char");
		if(pNode)
		{
			_variant_t v;
			v = pNode->GetnodeTypedValue();
			m_font.time_char = atof(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();

			if(m_font.time_char <= 4)
				m_font.time_char=8;
		}
		pNode = pRootNode->selectSingleNode("@train_id");
		if(pNode)
		{
			_variant_t v;
			v = pNode->GetnodeTypedValue();
			m_font.train_id = atof(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();

			if(m_font.train_id <= 4)
				m_font.train_id=8;
		}
		pNode = pRootNode->selectSingleNode("@engine_id");
		if(pNode)
		{
			_variant_t v;
			v = pNode->GetnodeTypedValue();
			m_font.engine_id = atof(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();

			if(m_font.engine_id <= 4)
				m_font.engine_id=6;
		}
		pNode = pRootNode->selectSingleNode("@late_time");
		if(pNode)
		{
			_variant_t v;
			v = pNode->GetnodeTypedValue();
			m_font.late_time = atof(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();

			if(m_font.late_time <= 4)
				m_font.late_time=8;
		}
		pNode = pRootNode->selectSingleNode("@station_text");
		if(pNode)
		{
			_variant_t v;
			v = pNode->GetnodeTypedValue();
			m_font.station_text = atof(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();

			if(m_font.station_text <= 4)
				m_font.station_text=8;
		}
	}

	m_nSubaoRowNum=80;
	pNode = pXMLDoc->selectSingleNode( "plot/subaorow");
	if(pNode)
	{
		_variant_t v;
		v = pNode->GetnodeTypedValue();
		m_nSubaoRowNum= atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();

		if(m_nSubaoRowNum <= 80)
			m_nSubaoRowNum=80;
		else if(m_nSubaoRowNum > 512)
			m_nSubaoRowNum=512;
	}
	pRootNode = pXMLDoc->selectSingleNode( "plot/hegiht");
	if(pRootNode)
	{
		pNode = pRootNode->selectSingleNode("@vertical_route_space");
		if(pNode)
		{
			_variant_t v;
			v = pNode->GetnodeTypedValue();
			m_space.vertical_route_space = atof(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		
		pNode = pRootNode->selectSingleNode("@step_of_side");
		if(pNode)
		{
			_variant_t v;
			v = pNode->GetnodeTypedValue();
			m_space.step_of_side = atof(CString(v.bstrVal));
			step_of_side = m_space.step_of_side;
			SysFreeString(v.bstrVal);
			v.Clear();

			if(m_space.step_of_side < 4)
				m_space.step_of_side=4;
		}
	}
	if(m_space.vertical_route_space < 20)
	{
		m_space.vertical_route_space=20;
	}
	vertical_route_space=m_space.vertical_route_space*2;

	if(printType == 0)
	{
		pRootNode = pXMLDoc->selectSingleNode( "plot/plot24");
	}
	else if(printType == 1)
	{
		pRootNode = pXMLDoc->selectSingleNode( "plot/plot12");
		pChildNode = pRootNode->selectSingleNode("@nosame");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			nosame = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}
	else
	{
		pRootNode = pXMLDoc->selectSingleNode( "plot/basic");
	}

	if(pRootNode == NULL)
		return;

	// margin
	pNode = pRootNode->selectSingleNode("margin");
	if(pNode)
	{
		pChildNode = pNode->selectSingleNode("@left");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			mPlotStyle.left = atof(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChildNode = pNode->selectSingleNode("@right");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			mPlotStyle.right = atof(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChildNode = pNode->selectSingleNode("@top");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			mPlotStyle.top = atof(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChildNode = pNode->selectSingleNode("@bottom");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			mPlotStyle.bottom = atof(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}
	else
	{
		mPlotStyle.left = 1;
		mPlotStyle.right = 1;
		mPlotStyle.top = 1;
		mPlotStyle.bottom = 1;
	}

	// stytle
	pNode = pRootNode->selectSingleNode("stytle");
	if(pNode)
	{
		_variant_t v;
		v = pNode->GetnodeTypedValue();
		mPlotStyle.id = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	else
	{
		mPlotStyle.id = 2;
	}

	// side_sep
	pNode = pRootNode->selectSingleNode("side_sep");
	if(pNode)
	{
		_variant_t v;
		v = pNode->GetnodeTypedValue();
		mPlotStyle.side_sep= atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	else
	{
		mPlotStyle.side_sep = 1;
	}

	// train_id
	pNode = pRootNode->selectSingleNode("train_id");
	if(pNode)
	{
		_variant_t v;
		v = pNode->GetnodeTypedValue();
		mPlotStyle.train_id= atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	else
	{
		mPlotStyle.train_id = 1;
	}
	

	mPlotStyle.con_num=0;
	memset(mPlotStyle.con_id, 0, sizeof(mPlotStyle.con_id));
	MSXML::IXMLDOMNodeListPtr pNodeList = pRootNode->selectNodes( "no_car_area/con_id" );
	int count = pNodeList->Getlength();
	for(int idx = 0; idx < count&&idx<64; idx++)
	{
		pChildNode = pNodeList->Getitem(idx)->selectSingleNode( "@id" );
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			mPlotStyle.con_id[idx] = atol(CString((v.bstrVal)));
			mPlotStyle.con_num++;
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}
	
	// name_area
	pNode = pRootNode->selectSingleNode("name_area");
	if(pNode)
	{
		pChildNode = pNode->selectSingleNode("@x_offset");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			mPlotStyle.name_area.x_offset = atof(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChildNode = pNode->selectSingleNode("@y_offset");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			mPlotStyle.name_area.y_offset = atof(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChildNode = pNode->selectSingleNode("@height");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			mPlotStyle.name_area.height = atof(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChildNode = pNode->selectSingleNode("@width");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			mPlotStyle.name_area.width = atof(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}

	// train_area
	pNode = pRootNode->selectSingleNode("train_area");
	if(pNode)
	{
		pChildNode = pNode->selectSingleNode("@x_offset");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			mPlotStyle.train_area.x_offset = atof(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChildNode = pNode->selectSingleNode("@y_offset");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			mPlotStyle.train_area.y_offset = atof(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChildNode = pNode->selectSingleNode("@height");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			mPlotStyle.train_area.height = atof(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChildNode = pNode->selectSingleNode("@width");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			mPlotStyle.train_area.width = atof(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChildNode = pNode->selectSingleNode("@timedlt");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			mPlotStyle.timedlt = atof(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		else
		{
			mPlotStyle.timedlt = 2;
		}


	}

	// phase_area
	pNode = pRootNode->selectSingleNode("phase_area");
	if(pNode)
	{
		pChildNode = pNode->selectSingleNode("@x_offset");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			mPlotStyle.phase_area.x_offset = atof(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChildNode = pNode->selectSingleNode("@y_offset");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			mPlotStyle.phase_area.y_offset = atof(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChildNode = pNode->selectSingleNode("@height");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			mPlotStyle.phase_area.height = atof(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChildNode = pNode->selectSingleNode("@width");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			mPlotStyle.phase_area.width = atof(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}

	// note_area
	pNode = pRootNode->selectSingleNode("note_area");
	if(pNode)
	{
		pChildNode = pNode->selectSingleNode("@x_offset");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			mPlotStyle.note_area.x_offset = atof(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChildNode = pNode->selectSingleNode("@y_offset");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			mPlotStyle.note_area.y_offset = atof(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChildNode = pNode->selectSingleNode("@height");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			mPlotStyle.note_area.height = atof(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChildNode = pNode->selectSingleNode("@width");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			mPlotStyle.note_area.width = atof(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}

	// qb_area
	pNode = pRootNode->selectSingleNode("qb_area");
	if(pNode)
	{
		pChildNode = pNode->selectSingleNode("@x_offset");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			mPlotStyle.qb_area.x_offset = atof(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChildNode = pNode->selectSingleNode("@y_offset");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			mPlotStyle.qb_area.y_offset = atof(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChildNode = pNode->selectSingleNode("@height");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			mPlotStyle.qb_area.height = atof(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChildNode = pNode->selectSingleNode("@width");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			mPlotStyle.qb_area.width = atof(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}

	// qb_area_left
	pNode = pRootNode->selectSingleNode("qb_area_left");
	if(pNode)
	{
		pChildNode = pNode->selectSingleNode("@x_offset");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			mPlotStyle.qb_area_left.x_offset = atof(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChildNode = pNode->selectSingleNode("@y_offset");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			mPlotStyle.qb_area_left.y_offset = atof(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChildNode = pNode->selectSingleNode("@height");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			mPlotStyle.qb_area_left.height = atof(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChildNode = pNode->selectSingleNode("@width");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			mPlotStyle.qb_area_left.width = atof(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}

	// qb_area_right
	pNode = pRootNode->selectSingleNode("qb_area_right");
	if(pNode)
	{
		pChildNode = pNode->selectSingleNode("@x_offset");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			mPlotStyle.qb_area_right.x_offset = atof(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChildNode = pNode->selectSingleNode("@y_offset");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			mPlotStyle.qb_area_right.y_offset = atof(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChildNode = pNode->selectSingleNode("@height");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			mPlotStyle.qb_area_right.height = atof(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChildNode = pNode->selectSingleNode("@width");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			mPlotStyle.qb_area_right.width = atof(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}

	// car_area
	pNode = pRootNode->selectSingleNode("car_area");
	if(pNode)
	{
		pChildNode = pNode->selectSingleNode("@x_offset");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			mPlotStyle.car_area.x_offset = atof(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChildNode = pNode->selectSingleNode("@y_offset");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			mPlotStyle.car_area.y_offset = atof(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChildNode = pNode->selectSingleNode("@height");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			mPlotStyle.car_area.height = atof(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChildNode = pNode->selectSingleNode("@width");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			mPlotStyle.car_area.width = atof(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}

	if(nShiftNo==1 && nosame == 1 && printType == 1)
	{
		mPlotStyle.note_area.x_offset = mPlotStyle.name_area.x_offset;
		mPlotStyle.qb_area.x_offset   = mPlotStyle.name_area.x_offset;

		mPlotStyle.phase_area.x_offset = mPlotStyle.note_area.x_offset+mPlotStyle.note_area.width+1;
		mPlotStyle.train_area.x_offset = mPlotStyle.phase_area.x_offset;
		mPlotStyle.qb_area.width -= 1;
		mPlotStyle.car_area.x_offset   = mPlotStyle.qb_area.x_offset+mPlotStyle.qb_area.width+1;
	}
	if(!IsPrintPhaseCar(m_sPrintInform->usSectionNo) && mPlotStyle.id == 4)
	{
		mPlotStyle.note_area.width -=10;
		mPlotStyle.qb_area.width = mPlotStyle.note_area.width;
		mPlotStyle.train_area.width += 10;
		mPlotStyle.phase_area.width += 10;

		mPlotStyle.qb_area.x_offset = mPlotStyle.train_area.x_offset + mPlotStyle.train_area.width + 2;
		mPlotStyle.note_area.x_offset = mPlotStyle.qb_area.x_offset;
	}
}

void CPlotDoc::ParsePlotParam(const CString& plot_param)
{
	CString xml = plot_param;
	
	for(int i=0; i<sizeof(m_nCRHStopStation)/sizeof(m_nCRHStopStation[0]); i++)
	{
		m_nCRHStopStation[i]=0;
	}
	
	for(int i=0; i<sizeof(m_nCRHReturnStation)/sizeof(m_nCRHReturnStation[0]); i++)
	{
		m_nCRHReturnStation[i].x=0;
		m_nCRHReturnStation[i].y=-1;
	}
	
	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		AfxMessageBox(er.ErrorMessage());
		return;
	}

	if(VARIANT_TRUE != pXMLDoc->loadXML( _bstr_t( xml ) ) )
	{
		return;
	}
	CStdioFile file;
	if(file.Open("c:\\plot_param.xml", CFile::modeCreate|CFile::modeReadWrite))
	{
		file.WriteString(xml);
		file.Close();
	}
	MSXML::IXMLDOMNodePtr pRootNode = pXMLDoc->selectSingleNode( "PLOT_PARAM");
	if(pRootNode == NULL)
	{
		return;
	}

	MSXML::IXMLDOMNodePtr pNode, pChild, pSide;
	_variant_t v;

	m_bShowdistanceStone = FALSE;
	m_bShowFastOrSlowTime = FALSE;
	m_bShowCzhConnectLineOnPlot = false;
	m_bShowNotTechStopFlag = true;

	pNode = pRootNode->selectSingleNode("MODE");
	if(pNode)
	{
		pChild = pNode->selectSingleNode( "@leap" );
		if(pChild)
		{
			v = pChild->GetnodeTypedValue();
			this->m_nTrainLeapLineFlag = atoi(CString((v.bstrVal)));

			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChild = pNode->selectSingleNode( "@electric" );
		if(pChild)
		{
			v = pChild->GetnodeTypedValue();
			this->m_nShowElecLocoFlag = atoi(CString((v.bstrVal)));

			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChild = pNode->selectSingleNode( "@fast_slow" );
		if(pChild)
		{
			v = pChild->GetnodeTypedValue();
			this->m_bShowFastOrSlowTime = atoi(CString(v.bstrVal));

			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChild = pNode->selectSingleNode( "@distance_stone" );
		if(pChild)
		{
			v = pChild->GetnodeTypedValue();
			this->m_bShowdistanceStone = atoi(CString(v.bstrVal));

			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChild = pNode->selectSingleNode( "@wave_line" );
		if(pChild)
		{
			v = pChild->GetnodeTypedValue();
			this->m_bStopStationWaveLine = atoi(CString(v.bstrVal));

			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChild = pNode->selectSingleNode( "@hrb" );
		if(pChild)
		{
			v = pChild->GetnodeTypedValue();
			this->m_hrbLateTimeMode = atoi(CString(v.bstrVal));

			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChild = pNode->selectSingleNode( "@manualRealColor" );
		if(pChild)
		{
			v = pChild->GetnodeTypedValue();
			this->manualRealColor = (DWORD)atol(CString(v.bstrVal));

			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChild = pNode->selectSingleNode( "@draw_side_point" );
		if(pChild)
		{
			v = pChild->GetnodeTypedValue();
			this->m_nDrawSidePoint = atoi(CString(v.bstrVal));

			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}

	MSXML::IXMLDOMNodeListPtr pNodeList = pRootNode->selectNodes( "NODES/NODE" ), pSideList;
	int count = pNodeList->Getlength();
	for(int idx = 0; idx < count&&idx<128; idx++)
	{
		pNode = pNodeList->Getitem(idx);
		if(pNode)
		{
			pChild = pNode->selectSingleNode( "@station_no" );
			if(pChild)
			{
				v = pChild->GetnodeTypedValue();
				m_tgNode[idx].sta_no = atol(CString((v.bstrVal)));

				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pChild = pNode->selectSingleNode( "@sep_station" );
			if(pChild)
			{
				v = pChild->GetnodeTypedValue();
				m_tgNode[idx].sta_sep = atol(CString((v.bstrVal)));

				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pChild = pNode->selectSingleNode( "@show_side" );
			if(pChild)
			{
				v = pChild->GetnodeTypedValue();
				m_tgNode[idx].show_side = atol(CString((v.bstrVal)));

				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pChild = pNode->selectSingleNode( "@czh_printflag" );
			if(pChild)
			{
				v = pChild->GetnodeTypedValue();
				int czh_printflag = atol(CString((v.bstrVal)));
				if(czh_printflag==1) // 车次折返站
				{
					for(int i=0; i<sizeof(m_nCRHReturnStation)/sizeof(m_nCRHReturnStation[0]); i++)
					{
						m_nCRHReturnStation[i].x=m_tgNode[idx].sta_no;
						m_nCRHReturnStation[i].y=idx;
					}
				}
				if(czh_printflag==2) // 动车段或动车所
				{
					for(int i=0; i<sizeof(m_nCRHStopStation)/sizeof(m_nCRHStopStation[0]); i++)
					{
						if(m_nCRHStopStation[i]==0)
						{
							m_nCRHStopStation[i]=m_tgNode[idx].sta_no;
						}
					}
				}
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			

			// <SIDES>
			pSideList = pNode->selectNodes( "SIDES/SIDE" );

			m_tgNode[idx].side_num = 0;
			memset(m_tgNode[idx].SIDES, 0, sizeof(m_tgNode[idx].SIDES));
			int nMaxSides=sizeof(m_tgNode[idx].SIDES)/sizeof(m_tgNode[idx].SIDES[0])-1;
			for(int i = 0; ((i<pSideList->Getlength()) && (i<nMaxSides)); i++)   // 2016.10.10 打印数组越界
			{
				pSide = pSideList->Getitem(i);
				if(pSide)
				{
					m_tgNode[idx].side_num++;
					pChild = pSide->selectSingleNode( "@side_no" );
					if(pChild)
					{
						v = pChild->GetnodeTypedValue();
						m_tgNode[idx].SIDES[i].side_no = atoi(CString((v.bstrVal)));

						SysFreeString(v.bstrVal);
						v.Clear();
					}

					pChild = pSide->selectSingleNode( "@side_name" );
					if(pChild)
					{
						v = pChild->GetnodeTypedValue();
						CString sideName = v.bstrVal;
						strncpy(m_tgNode[idx].SIDES[i].side_name, sideName.GetBuffer(), 9);
						m_tgNode[idx].SIDES[i].side_name[9] = 0;
						SysFreeString(v.bstrVal);
						v.Clear();
					}

					pChild = pSide->selectSingleNode( "@side_property" );
					if(pChild)
					{
						v = pChild->GetnodeTypedValue();
						m_tgNode[idx].SIDES[i].side_property = atoi(CString(v.bstrVal));
						
						SysFreeString(v.bstrVal);
						v.Clear();
					}
				}
			}
		}
		else
		{
			m_tgNode[idx].sta_sep = 0;
			m_tgNode[idx].show_side = 0;
			m_tgNode[idx].side_num = 0;
			memset(m_tgNode[idx].SIDES, 0, sizeof(m_tgNode[idx].SIDES));
		}

		m_tgNodeNum++;
	}
	
	// STATICS_TIME_COMPLETE_BY_TGDEF
	m_bStaticsTimeCompleteByTGDef=false;
	pNode = pRootNode->selectSingleNode("STATICS_TIME_COMPLETE_BY_TGDEF");
	if(pNode)
	{
		pChild = pNode->selectSingleNode( "@method" );
		if(pChild)
		{
			v = pChild->GetnodeTypedValue();
			int tmp = atoi(CString((v.bstrVal)));
			if(tmp>0)
				m_bStaticsTimeCompleteByTGDef=true;
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}

	// STATICS_TIME
	m_nStaticMethod=0;
	pNode = pRootNode->selectSingleNode("STATICS_TIME");
	if(pNode)
	{
		pChild = pNode->selectSingleNode( "@method" );
		if(pChild)
		{
			v = pChild->GetnodeTypedValue();
			m_nStaticMethod = atoi(CString((v.bstrVal)));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}

	// NOLATETRAIN
	m_bDrawBlackArc=false; // 默认不画黑圈
	pNode = pRootNode->selectSingleNode("NOLATETRAIN");
	if(pNode)
	{
		pChild = pNode->selectSingleNode( "@draw_black_arc" );
		if(pChild)
		{
			v = pChild->GetnodeTypedValue();
			int nFlag = atoi(CString((v.bstrVal)));
			if(nFlag>0)
				m_bDrawBlackArc=true;
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}

	m_noearlylatetime.clear();
	MSXML::IXMLDOMNodeListPtr pTrainIndexList = pRootNode->selectNodes( "NOLATETRAIN/TRAININDEX" );
	count = pTrainIndexList->Getlength();
	for(int idx = 0; idx < count; idx++)
	{
		pNode = pTrainIndexList->Getitem(idx);
		if(pNode)
		{
			pChild = pNode->selectSingleNode( "@id" );
			if(pChild)
			{
				v = pChild->GetnodeTypedValue();
				ULONG index= atol(CString((v.bstrVal)));;
				m_noearlylatetime.insert(make_pair(index,index));
				
				SysFreeString(v.bstrVal);
				v.Clear();
			}
		}

		m_tgNodeNum++;
	}

	m_virtualStation.clear();
	int ishow=0;
	MSXML::IXMLDOMNodePtr pVirNode = pRootNode->selectSingleNode("VIRTUAL_STATION");
	if(pVirNode)
	{
			pChild = pVirNode->selectSingleNode( "@show" );
			if(pChild)
			{
				v = pChild->GetnodeTypedValue();
				ishow= atoi(CString((v.bstrVal)));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
	}
	if(ishow>0)
		m_bShowVirtualStationOnPlot=true;
	else
		m_bShowVirtualStationOnPlot=false;

	if(pVirNode)
	{
		MSXML::IXMLDOMNodeListPtr pVritualList = pVirNode->selectNodes( "STATION" );
		int count = pVritualList->Getlength();
		for(int idx = 0; idx < count; idx++)
		{
			pNode = pVritualList->Getitem(idx);
			if(pNode)
			{
				TGVirtualStation node;
				pChild = pNode->selectSingleNode( "@up" );
				if(pChild)
				{
					v = pChild->GetnodeTypedValue();
					node.perStationIndex= atoi(CString((v.bstrVal)));
					SysFreeString(v.bstrVal);
					v.Clear();
				}

				pChild = pNode->selectSingleNode( "@down" );
				if(pChild)
				{
					v = pChild->GetnodeTypedValue();
					node.nextStationIndex= atoi(CString((v.bstrVal)));
					SysFreeString(v.bstrVal);
					v.Clear();
				}

				pChild = pNode->selectSingleNode( "@id" );
				if(pChild)
				{
					v = pChild->GetnodeTypedValue();
					node.nStation= atoi(CString((v.bstrVal)));
					SysFreeString(v.bstrVal);
					v.Clear();
				}

				pChild = pNode->selectSingleNode( "@name" );
				if(pChild)
				{
					v = pChild->GetnodeTypedValue();
					CString name=v.bstrVal;
					strncpy(node.strStatinName, name.GetBuffer(0), 63);
					node.strStatinName[63]=0;
					SysFreeString(v.bstrVal);
					v.Clear();
				}

				pChild = pNode->selectSingleNode( "@percent" );
				if(pChild)
				{
					v = pChild->GetnodeTypedValue();
					node.nPercent= atoi(CString((v.bstrVal)));
					SysFreeString(v.bstrVal);
					v.Clear();
				}

				m_virtualStation.push_back(node);
			}
		}
	}
}
CPlotDoc::CPlotDoc()
{
	m_tgNodeNum = 0;
	memset(&NOTES,0,sizeof(NOTEBOOK));
	memset(&NOTES2,0,sizeof(NOTEBOOK));
	memset(&train_additional_info,0,sizeof(TRAIN_ADDITIONAL_INFO)*1024);
	m_nPenWidth = 3;
	m_nActualPenWidth = 4;
	m_nLineWidthRate = 1;

	m_EnvironInitFlag = 0;

	m_brushBackground.CreateSolidBrush(CLR_WHITE);

	for(int j= 0;j<19;j++)
	{
		parsecontent[j].Empty();
	}
	m_nCurrentRouteNum = 0;
	m_nAllRouteNum = 0;
	m_nDispDistanceNum = 0;


	station_axis_width = STATION_AXIS_WIDTH;// 站名宽度
	header_title_height = HEADER_TITLE_HEIGHT;// 头标题高度
	draw_mark_height = DRAW_MARK_HEIGHT;// 车次号高度
	distance_text_width = DISTANCE_TEXT_WIDTH;// 公里标宽度
	footer_title_height = FOOTER_TITLE_HEIGHT;// 尾标题高度
	train_inform_height = TRAIN_INFORM_HEIGHT;//列车编组情况高度

	//DMIS CAPTURE
	x_offset_print = 50;
	y_offset_print = 50;

	step_of_side = m_space.step_of_side;
	vertical_route_space = m_space.vertical_route_space;
	train_mark_height = m_space.train_mark_height;

	single_train_inform_width = SINGLE_TRAIN_INFORM_WIDTH;//单个列车编组情况宽度
	single_train_height = SINGLE_TRAIN_HEIGHT;                                         //单个列车编组的高度
	xMultiple=yMultiple = 1;   
	fHeightMulti = fWidthMulti = 1;//横向,纵向放大(打印)
	time_axis_height = TIME_AXIS_HEIGHT;//时间轴高度

	m_nStaticMethod=0;
	m_bStaticsTimeCompleteByTGDef=false;

	nColumnWidth[0]=m_nSubaoWidth[0]=50;    
	nColumnWidth[1]=m_nSubaoWidth[1]=120; 
	nColumnWidth[2]=m_nSubaoWidth[2]=120; 
	nColumnWidth[3]=m_nSubaoWidth[3]=120; 
	nColumnWidth[4]=m_nSubaoWidth[4]=120; 
	nColumnWidth[5]=m_nSubaoWidth[5]=120; 
	nColumnWidth[6]=m_nSubaoWidth[6]=120; 
	nColumnWidth[7]=m_nSubaoWidth[7]=120;

	m_nMaxPageHeight = 2*m_nPageHeight;

	m_nMinPageHeight = 200;

	m_nShowDirectFlag = 2;
	m_nShowTrainFlag = 2;
	
	m_nDrawSidePoint=0;
	m_nTrainLeapLineFlag = 2;
	m_nDrawTrainConsiderStationSep = 0;
	m_pTgDoc = NULL;
	m_bShowReverse = TRUE;
	m_bStopStationWaveLine = FALSE;
	memset(m_sDispDistance, 0, sizeof(m_sDispDistance));

	m_bShowVirtualStationOnPlot =false;

	m_nCZCount=0;
	for(int i=0; i<max_czh_count; i++)
		m_sCZRelation[i].Init();
	for(int i=0; i<sizeof(m_nCRHStopStation)/sizeof(m_nCRHStopStation[0]); i++)
	{
		m_nCRHStopStation[i]=0;
	}
	
	for(int i=0; i<sizeof(m_nCRHReturnStation)/sizeof(m_nCRHReturnStation[0]); i++)
	{
		m_nCRHReturnStation[i].x=0;
		m_nCRHReturnStation[i].y=-1;
	}
}

bool CPlotDoc::IsBlockAllSide(int stano, int sideNum)
{
	for(int i=0; i<m_tgNodeNum; i++)
	{
		if(m_tgNode[i].sta_no == stano)
		{
			if(m_tgNode[i].side_num == sideNum)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}

	return false;
}

CString CPlotDoc::GetSideName(int sta_no, int side_no)
{
	CString sideName="";
	for(int i=0; i<m_tgNodeNum; i++)
	{
		if(m_tgNode[i].sta_no == sta_no)
		{
			for(int k=0;k <m_tgNode[i].side_num; k++)
			{
				if(m_tgNode[i].SIDES[k].side_no == side_no)
				{
					sideName = m_tgNode[i].SIDES[k].side_name;
					return sideName;
				}
			}
			break;
		}
	}
	
	return sideName;
}

void CPlotDoc::ParseMarkReasonText(const CString& text, CString& dcmdno, CString& reason)
{
	dcmdno = "";
	reason = text;

	int pos = text.Find("^",0);
	if(pos <= 0)
		return;

	if(text.GetLength() > pos+1)
	{
		if(text.GetAt(pos+1) == '^')
		{
			dcmdno = text.Left(pos);
			reason = text.Right(text.GetLength() - pos - 2);
		}
	}
}

CPlotDoc::~CPlotDoc()
{
	m_brushBackground.DeleteObject();
}

BEGIN_MESSAGE_MAP(CPlotDoc, CDocument)
	//{{AFX_MSG_MAP(CPlotDoc)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlotDoc diagnostics
void CPlotDoc::TimeToDataAndMinute(TIME long_time, long *date, long *minute)
{
	if(long_time == -1)
	{
		*date   = -1;
		*minute = -1;
	} 
	else if(long_time < 86400 * 3) 
	{
		*date   =  long_time / 86400;
		*minute =  long_time % 86400;
	} 
	else 
	{
		struct tm *tm_;
		time_t t = time_t(long_time);
		tm_ = localtime(&t);

		*date= (tm_->tm_year - 90)*380+tm_->tm_mon*31+tm_->tm_mday-1;
		*minute = tm_->tm_hour*3600+tm_->tm_min*60+tm_->tm_sec;
	}
	return;
}

#ifdef _DEBUG
void CPlotDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPlotDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPlotDoc serialization

void CPlotDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPlotDoc commands
void CPlotDoc::HideStationString(CDC * pCDC)
{
	CRect  rclHere;

	rclHere.left =  5;
	rclHere.bottom = m_nPageHeight;
	rclHere.top = 1;
	rclHere.right = station_axis_width;
	pCDC->FillRect(&rclHere, &m_brushBackground);
}

int CPlotDoc::ConvertYPostoStationNo(int yPos,int &nRouteSetNo)
{
	int i;

	for(i=0;i < m_nDispDistanceNum;i++)
	{
		if(m_sDispDistance[i].distance.node_info.show_sides)
		{
			int y1,y2;
			y1 = m_sDispDistance[i].nStationYPos;
			y2 = m_sDispDistance[i].nSecondStationYPos;
			if( yPos > y1 - 8 && yPos < y2 + 8)
				break;
		}
		if(abs(yPos - m_sDispDistance[i].nStationYPos) < 10)
			break;
	}
	if(i == m_nDispDistanceNum)
		return -1;
	nRouteSetNo = m_sDispDistance[i].nRouteSetNo;
	return m_sDispDistance[i].nStationNo;
}

void CPlotDoc::ConvertXPostoTime(int xPos, long * pusDate, long * pusTime)
{// convert screen coordinate to time
	int timevalue=0;

	if(m_nSchdStatus == WORK_SCHEDULE || m_nSchdStatus == SHIFT_SCHEDULE)
	{
		int nHistoryPixelWidth = (int)(m_nHistoryGraphWidth * m_fPixelPerMinute);
		if(xPos < nHistoryPixelWidth)
			timevalue = (int)(xPos / m_fPixelPerMinute)- m_nHistoryGraphWidth;
		else if(xPos >= nHistoryPixelWidth && 
			xPos <= nHistoryPixelWidth + SEPERATORBARWIDTH)
			timevalue = 0;
		else if(xPos > nHistoryPixelWidth + SEPERATORBARWIDTH)
			timevalue = (int)((xPos/* - SEPERATORBARWIDTH */) / m_fPixelPerMinute)- m_nHistoryGraphWidth;
		timevalue *= 60;
		timevalue += m_usCurrentTime;
		if(timevalue > WHOLE_DAY_SECOND)
		{
			*pusTime = timevalue - WHOLE_DAY_SECOND;
			*pusDate = OsGetNextDate(PLOT.ThreadId,m_usCurrentDate);
		}
		else 
		{
			*pusTime = timevalue;
			*pusDate = m_usCurrentDate;
		}
	}
	else if(m_nSchdStatus == HISTORY_SCHEDULE) 
	{
		timevalue = (int)(xPos / m_fPixelPerMinute);
		timevalue += m_usHistGraphStartTime;
		timevalue *= 60;
		if(timevalue >= WHOLE_DAY_SECOND)
		{
			*pusTime = timevalue - WHOLE_DAY_SECOND;
			*pusDate = OsGetNextDate(PLOT.ThreadId,m_sTGGraphStartTime.day);
		}
		else 
		{
			*pusTime = timevalue;
			*pusDate = m_sTGGraphStartTime.day;
		}
	}
	else
	{
		timevalue = (int)(xPos / m_fPixelPerMinute);
		timevalue += TGtimeConverttoMinute(m_sTGGraphStartTime);
		timevalue *= 60;
		if(m_nSchdStatus == BASIC_SCHEDULE)
		{
			*pusDate = 0;
			*pusTime = timevalue;
		}
		else if(m_nShiftType == LAST_SHIFT) 
		{
			if(m_usCurrentTime >= 21600 && m_usCurrentTime <= 64800)
			{ // last shift is night shift
				if(timevalue >= WHOLE_DAY_SECOND)
				{
					*pusTime = timevalue - WHOLE_DAY_SECOND;
					*pusDate = m_usCurrentDate;
				} else {
					*pusTime = timevalue;
					*pusDate = OsGetPrevDate(PLOT.ThreadId,m_usCurrentDate);
				}
			} 
			else if(m_usCurrentTime < 21600 ) 
			{
				*pusTime = timevalue;
				*pusDate = OsGetPrevDate(PLOT.ThreadId,m_usCurrentDate);
			}
			else if(m_usCurrentTime > 64800) 
			{
				*pusTime = timevalue;
				*pusDate = m_usCurrentDate;
			}
		} 
		else if(m_nShiftType == CURR_SHIFT ) 
		{
			if(m_usCurrentTime >= 21600 && m_usCurrentTime <= 64800)
			{ // curr shift is day shift
				*pusTime = timevalue;
				*pusDate = m_usCurrentDate;
			} 
			else if(m_usCurrentTime < 21600) 
			{
				if(timevalue >= WHOLE_DAY_SECOND)
				{
					*pusTime = timevalue;
					*pusDate = m_usCurrentDate;
				} else {
					*pusTime = timevalue;
					*pusDate = OsGetPrevDate(PLOT.ThreadId,m_usCurrentDate);
				}
			}
			else if(m_usCurrentTime > 64800) 
			{
				if(timevalue >= WHOLE_DAY_SECOND)
				{
					*pusTime = timevalue - WHOLE_DAY_SECOND;
					*pusDate = OsGetNextDate(PLOT.ThreadId,m_usCurrentDate);
				}
				else 
				{
					*pusTime = timevalue;
					*pusDate = m_usCurrentDate;
				}
			}
		}
		else if(m_nShiftType == NEXT_SHIFT) 
		{
			if(m_usCurrentTime > 21600 && m_usCurrentTime < 64800)
			{ // next shift is night shift
				if(timevalue >= WHOLE_DAY_SECOND)
				{
					*pusTime = timevalue - WHOLE_DAY_SECOND;
					*pusDate = OsGetNextDate(PLOT.ThreadId,m_usCurrentDate);
				} 
				else 
				{
					*pusTime = timevalue;
					*pusDate = m_usCurrentDate;
				}
			} 
			else if(m_usCurrentTime < 21600) 
			{
				*pusTime = timevalue;
				*pusDate = m_usCurrentDate;
			}
			else if(m_usCurrentTime > 64800) 
			{
				*pusTime = timevalue;
				*pusDate = OsGetNextDate(PLOT.ThreadId,m_usCurrentDate);
			}
		}
	}
}

int CPlotDoc::ConvertTimetoXPos(long usTime, long usDate,int schd_type)
{
	// convert time to screen coordinate 
	int nOffset;
	nOffset = GetMinuteWidthToPlotStartTime(usTime/60,usDate,schd_type);
	nOffset = (int)(nOffset * m_fPixelPerMinute);
	return nOffset ;
}

// Draw schdule train function 
void CPlotDoc::DrawSchdTrain(CDC *pCDC,int nFlag)
{
	int nSchdTrainNum = m_vecSchdTrains.size();
	if(nSchdTrainNum <= 0)
		return;
	int i;
	int nDrawMode = 0x00000000;// redraw all schedule train, so copypen mode

	if(m_nSchdStatus == BASIC_SCHEDULE)
	{  	 
		nDrawMode = DRAW_NORMAL;// redraw all schedule train, so copypen mode
		if(nFlag == 0)
		{
			for(i=0;i < nSchdTrainNum;i++)
				DrawSingleTrain(pCDC, &m_vecSchdTrains[i],nDrawMode,0,0,TRUE,BASIC_SCHEDULE); 
		}
		else 
		{ 
			for(i=0;i < nSchdTrainNum;i++)
				DrawSingleTrain(pCDC, &m_vecSchdTrains[i],nDrawMode,0,0,TRUE,BASIC_SCHEDULE);
		}
	}
	else if(m_nSchdStatus == WORK_SCHEDULE || m_nSchdStatus == SHIFT_SCHEDULE)
	{
		if(nFlag == 0)
		{
			for(i=0;i < nSchdTrainNum;i++)
				DrawSingleTrain(pCDC, &m_vecSchdTrains[i],nDrawMode,0,-1,TRUE,WORK_SCHEDULE);
		}
		else 
		{ 
			for(i=0;i < nSchdTrainNum;i++)
				DrawSingleTrain(pCDC, &m_vecSchdTrains[i],nDrawMode,0,-1,TRUE,WORK_SCHEDULE);
		}
	}
	else if(m_nSchdStatus == HISTORY_SCHEDULE)
	{
		if(nFlag == 0)
		{
			for(i=0;i < nSchdTrainNum;i++)
				DrawSingleTrain(pCDC, &m_vecSchdTrains[i],nDrawMode,0,-1,TRUE,HISTORY_SCHEDULE);
		}
		else 
		{ 
			for(i=0;i < nSchdTrainNum;i++)
				DrawSingleTrain(pCDC, &m_vecSchdTrains[i],nDrawMode,0,-1,TRUE,HISTORY_SCHEDULE);
		}
	}
	else
		;

}

void CPlotDoc::DrawBackground(CDC * pDC, LONG lColor, ushort usStationNo, 
							  int usStartDate,int usStartTime, int usEndDate,int usEndTime,int nFlag)
{
	int i,nEnd,nStationOffset;
	UINT nOldAlign,nOldBackMode;
	CPen mThickPen,*pOldPen;

	usStartTime -= 1800;
	if(usStartTime < 0 )
	{
		if(usStartDate == 0)
		{
			usStartTime = 0;
		}else {
			usStartDate = OsGetPrevDate(PLOT.ThreadId,usStartDate);
			usStartTime += WHOLE_DAY_SECOND;
		}
	}

	usEndTime += 1800;
	if(usEndTime >= WHOLE_DAY_SECOND)
	{
		usEndDate = OsGetNextDate(PLOT.ThreadId,usEndDate);
		usEndTime -= WHOLE_DAY_SECOND;
	}
	if(nFlag)
	{// draw screen

	}else {// draw memory

	}

	nOldAlign = pDC->SetTextAlign(TA_LEFT|TA_BASELINE);
	nOldBackMode = pDC->SetBkMode(TRANSPARENT);
	mThickPen.CreatePen(PS_SOLID,2,lColor);
	pOldPen = pDC->SelectObject(&mThickPen);
	// function need modify
	nStationOffset = GetDispIndex(usStationNo,(ushort)1,0);

	if(m_nSchdStatus == WORK_SCHEDULE || m_nSchdStatus == HISTORY_SCHEDULE || m_nSchdStatus == SHIFT_SCHEDULE)
		nEnd = (int)(24 * m_fPixelPerHour);
	else nEnd = m_nPageWidth;
	// draw the text on the memory device context(the bitmap)
	// station axis line
	pDC->MoveTo(0,m_sDispDistance[nStationOffset].nStationYPos);
	pDC->LineTo(nEnd,m_sDispDistance[nStationOffset].nStationYPos);



	POINT ptlStart, ptlEnd;
	double lPosX;
	CPen mThinPen;
	// select pen/brush;
	mThinPen.CreatePen(PS_SOLID,1,lColor);
	pDC->SelectObject(&mThinPen);

	if(m_nViewFormat == TWO_MINUTE_GRID)
	{// draw two minute aix line
		ptlStart.y = m_sDispDistance[nStationOffset].nStationYPos - 1L;
		ptlEnd.y   = m_sDispDistance[nStationOffset].nStationYPos - 4L;
		ptlStart.x = ptlEnd.x = 0;
		lPosX = 0;
		for (i=1; i < m_usTwoMinuteMaxValue + 1; i++) {
			lPosX  += m_fPixelPerTwoMinute;
			ptlStart.x = (long)lPosX;
			ptlEnd.x   = (long)lPosX;
			pDC->MoveTo(ptlStart);
			pDC->LineTo(ptlEnd);
		}
	}
	pDC->SelectObject(pOldPen);	
	pDC->SetTextAlign(nOldAlign);
	pDC->SetBkMode(nOldBackMode);

	mThinPen.DeleteObject( );
	mThickPen.DeleteObject();
}




int CPlotDoc::Resize(int nDirect, int nFlag)
{
	int ChangeFlag = 0;
	double fTotalDistance;

	if(nDirect) // bigger
	{
		if(m_nPageHeight < m_nMaxPageHeight)
		{
			fTotalDistance = m_nPageHeight * 1.25;
			if(fTotalDistance > m_nMaxPageHeight)
				fTotalDistance = (double)m_nMaxPageHeight;
			ChangeFlag |= 1;
		}
		if(nFlag && m_fPixelPerMinute < m_fMaxPixelPerMinute)
		{
			m_fPixelPerMinute += 0.5;
			ChangeFlag |= 2;
		}
	}else {
		if(m_nPageHeight > m_nMinPageHeight)
		{
			fTotalDistance = m_nPageHeight * 0.8;
			if(fTotalDistance < m_nMinPageHeight)
				fTotalDistance = (double)m_nMinPageHeight;
			ChangeFlag |= 1;
		}
		if(nFlag && m_fPixelPerMinute > 1)
		{
			m_fPixelPerMinute -= 0.5;
			ChangeFlag |= 2;
		}
	}
	if(ChangeFlag == 0)
		return -1;
	if(ChangeFlag & 1)
	{
		m_nPageHeight = (int)fTotalDistance;
		// initialize station vertical position inform
	}
	// initialize time inform
	if(ChangeFlag & 2)
	{
		m_fPixelPerHour = m_fPixelPerMinute * 60;
		m_nPageWidth = (int)(m_nViewTimeRange * m_fPixelPerHour);

		m_fPixelPerHalfHour = m_fPixelPerHour / 2;
		m_fPixelPerTenMinute = m_fPixelPerHour / 6;
		m_fPixelPerTwoMinute = m_fPixelPerHour / 30;
		if(m_nSchdStatus == WORK_SCHEDULE || m_nSchdStatus == HISTORY_SCHEDULE || m_nSchdStatus == SHIFT_SCHEDULE)
		{
			m_usThreeHourMaxValue = 8;
			m_usOneHourMaxValue = 24;
			m_usHalfHourMaxValue = 48;
			m_usTenMinuteMaxValue = 144;
			m_usTwoMinuteMaxValue = 720;
			m_usMemoryPageWidth = (int)(24 * m_fPixelPerHour);
		} else {
			m_usThreeHourMaxValue = m_nViewTimeRange / 3;
			m_usOneHourMaxValue = m_nViewTimeRange;
			m_usHalfHourMaxValue = m_nViewTimeRange * 2;
			m_usTenMinuteMaxValue = m_nViewTimeRange * 6;
			m_usTwoMinuteMaxValue = m_usTenMinuteMaxValue * 5;
			m_usMemoryPageWidth = m_nPageWidth;
		}
	}
	return 0;
}

void CPlotDoc::DrawFrameBorder(CDC * pCDC, COLORREF clrColor)
{
	CPen mThickPen,*pOldPen;
	int y = m_nPartialHeight + draw_mark_height + time_axis_height + 10; 
	mThickPen.CreatePen(PS_SOLID,2,clrColor);
	pOldPen = pCDC->SelectObject(&mThickPen);
	pCDC->MoveTo(5,3);
	pCDC->LineTo(m_nPageWidth - 5,3);
	pCDC->LineTo(m_nPageWidth - 5,y - 60);  
	pCDC->LineTo(5,y - 60);                 
	pCDC->LineTo(5,3);
	pCDC->SelectObject(pOldPen);
	mThickPen.DeleteObject();
}

//绘制列车标志
void CPlotDoc::DrawTrainMark(CDC * pCDC)
{
	for(int i = 0;i< this->m_nAbsList.GetNumFigures();i++)
	{
		CPlotAbsMark* pBlock = this->m_nAbsList.GetFigure(i);
		if(NULL != pBlock)
			pBlock->Draw(pCDC,m_nPageWidth);
	}
}

void CPlotDoc::DrawMarkOfStart(CDC *pCDC,CTG_SingleTrain * single,CString strTrainId, CPoint ptlStart, int nLength, int nDirect, COLORREF Color,bool bActual,int schd_type)
{
	int scaleRate = min(yMultiple,5);

	if(ptlStart.y < y_offset_print)
		return;

	for(int id_count = 0; id_count < single->GetTrainRecordCount();id_count++)
	{
		if(single->GetTrainRecord(id_count)->m_data.arrive_train_id != single->GetTrainRecord(id_count)->m_data.depart_train_id
			&& strlen(single->GetTrainRecord(id_count)->m_data.arrive_train_id)>0 && strlen(single->GetTrainRecord(id_count)->m_data.depart_train_id)>0)
		{
			CString str = CString(single->GetTrainRecord(id_count)->m_data.depart_train_id);
			CString strTrainId;
			strTrainId.Format("%s",single->GetTrainID());
			int len = strTrainId.GetLength();
			CString str0,str1,str_def;
			bool flag = FALSE;
			for(int i=1;i<=len;i++)
			{
				str0 = strTrainId.Left(i);
				str1 = str.Left(i);
				if(str0 == str1) continue;
				str_def = str.Right(str.GetLength() - i + 1);
				flag = TRUE;
				break;
			}
			if(flag)
				strTrainId = strTrainId + "/" + str_def;
			break;
		}
	}

	//create text font
	int m_nLogPixelSy = pCDC->GetDeviceCaps(LOGPIXELSY);

	CFont font;
	LOGFONT stFont;
	CFont * pOldFont;
	memset(&stFont,0,sizeof(stFont));

	stFont.lfHeight = MulDiv(m_font.train_id,m_nLogPixelSy,72);
	stFont.lfWeight = FW_NORMAL;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	strcpy(stFont.lfFaceName,"Times New Roman");
	font.CreateFontIndirect(&stFont);
	pOldFont = pCDC->SelectObject(&font);
	CSize sTextExtent = pCDC->GetTextExtent(strTrainId);

	int uStartX,uStartY,uEndX,uEndY;
	CPoint ptlEnd;

	nLength = sTextExtent.cy*1.2; //始发默认高度为两个字体高
	int tem_index = GetAdditionalTrainInfoIndex(single->GetTrainIndex());
	if(tem_index == -1)
		return;
	if(nDirect == UP_MARK)
	{
		ptlEnd.x = ptlStart.x;
		ptlEnd.y = ptlStart.y - nLength;

		uStartX = ptlStart.x - sTextExtent.cx/2;// text width is 60,30
		uEndX = ptlStart.x + sTextExtent.cx/2;
		uStartY = ptlEnd.y - train_mark_height;// text height is 30
		uEndY = ptlEnd.y;
	} 
	else 
	{
		ptlEnd.x = ptlStart.x;
		ptlEnd.y = ptlStart.y + nLength; 
		uStartX = ptlStart.x - sTextExtent.cx/2;
		uEndX = ptlStart.x + sTextExtent.cx/2;
		uStartY = ptlEnd.y;
		uEndY = ptlEnd.y + train_mark_height;
	}

	//to create temp rect;
	CRect RectTemp(uStartX, uStartY,  uEndX, uEndY);
	CPoint pt1(RectTemp.left,RectTemp.top);
	CPoint pt2(RectTemp.left,RectTemp.bottom);
	CPoint pt3(RectTemp.right,RectTemp.top);
	CPoint pt4(RectTemp.right,RectTemp.bottom);
	CPoint pt( (RectTemp.left+RectTemp.right)/2, (RectTemp.top/ + RectTemp.bottom)/2);

	//to judge if new rect collide with previous ones;
	int Flag=0;   //if collidding 

	int nSchdTrainNum = m_vecSchdTrains.size();
	for(int i = 0;i < nSchdTrainNum;i++)
	{
		for(int j=0;j <= train_additional_info[i].m_nNumberOfMarkedRect; j++)
		{	 
			if( (PtInRects(train_additional_info[i].m_nRect[j], pt1) ||
				PtInRects(train_additional_info[i].m_nRect[j], pt) ||
				PtInRects(train_additional_info[i].m_nRect[j], pt2) ||
				PtInRects(train_additional_info[i].m_nRect[j], pt3) ||
				PtInRects(train_additional_info[i].m_nRect[j], pt4) )  && 
				m_vecSchdTrains[i].GetTrainIndex() != single->GetTrainIndex())
			{ 
				Flag=1;
				break;	
			}
		}
		if( Flag==1)
			break;
	}

	int increaeelayer;
	if(gpcdef.GetTrainIDFormat()==2)
	{
		increaeelayer =1;
		int stationno,i;
		stationno=this->ConvertYPostoStationNo(ptlStart.y-y_offset_print,i);
		for(i=0;i<train_additional_info[tem_index].ptoftrainid_count;i++)
		{
			if(stationno==train_additional_info[tem_index].stPttoTrainid[i].stationno)
			{
				train_additional_info[tem_index].stPttoTrainid[i].layer = GetIncreaseLayer(single,CPoint(uStartX,uStartY),nDirect,stationno,pCDC,schd_type);
				increaeelayer =train_additional_info[tem_index].stPttoTrainid[i].layer;
				break;
			}
		}

		if(train_additional_info[tem_index].ptoftrainid_count == 0)
		{
			train_additional_info[tem_index].stPttoTrainid[0].layer = GetIncreaseLayer(single,CPoint(ptlEnd.x - sTextExtent.cx,ptlEnd.y),nDirect,stationno,pCDC,schd_type);
			train_additional_info[tem_index].ptoftrainid_count++;
		}
	}
	else
	{
		train_additional_info[tem_index].m_nIncreaseLayer = GetIncreaseLayer(single,pCDC,schd_type);
		increaeelayer = train_additional_info[tem_index].m_nIncreaseLayer;
	}

	int iDelta = sTextExtent.cy*(increaeelayer-1);

	if(nDirect == UP_MARK)//上标注
	{ 
		uStartY += -iDelta;
		uEndY += -iDelta;  
		ptlEnd.y += -iDelta;

	}
	else
	{
		uStartY += iDelta;
		uEndY += iDelta;  
		ptlEnd.y += iDelta;
	}

	CPen mPen,*pOldPen;
	UINT nOldAlign;
	UINT nPreviousMode = pCDC->SetROP2(R2_COPYPEN); 
	UINT nOldMode = pCDC->SetBkMode(TRANSPARENT);

	mPen.CreatePen(PS_INSIDEFRAME, scaleRate*m_nLineWidthRate, Color); //2
	pOldPen = pCDC->SelectObject(&mPen);

	if(m_nShowElecLocoFlag)
	{
		if(IsElectricTrain(single))
		{
			//绘制电力机车的方框
			CRect m_rect;	CPoint cPoint; CSize rectSize;
			cPoint.x = uStartX-2 ;
			CSize trainidSize = pCDC->GetTextExtent(strTrainId);
			rectSize = trainidSize;
			rectSize.cx +=2;

			if(nDirect == UP_MARK)
			{
				cPoint.y = uEndY  - rectSize.cy ;
			}
			else
			{ 
				cPoint.y = uStartY +2 ;

			}
			rectSize.cy -=2;

			m_rect = CRect(cPoint,rectSize);
			pCDC->MoveTo(m_rect.TopLeft());
			pCDC->LineTo(m_rect.BottomRight().x, m_rect.TopLeft().y);
			pCDC->LineTo(m_rect.BottomRight());
			pCDC->LineTo(m_rect.TopLeft().x, m_rect.BottomRight().y );
			pCDC->LineTo(m_rect.TopLeft() );
		}
	}
	// to create new rect
	CRect rects(uStartX, uStartY,  uEndX, uEndY);

	if(train_additional_info[tem_index].m_nNumberOfMarkedRect<=31)
		train_additional_info[tem_index].m_nNumberOfMarkedRect += 1;
	else
		train_additional_info[tem_index].m_nNumberOfMarkedRect =1;

	train_additional_info[tem_index].m_nRect[train_additional_info[tem_index].m_nNumberOfMarkedRect-1]=rects;

	pCDC->MoveTo(ptlStart.x,ptlStart.y);
	pCDC->LineTo(ptlEnd.x,ptlEnd.y);

	if(nDirect == UP_MARK)
	{
		pCDC->MoveTo(uStartX,uEndY);
		pCDC->LineTo(uEndX ,uEndY);

		if(uStartX != uEndX)
		{
			BOOL bShowAcceptTrainLate = FALSE;
			BOOL bAcceptTruckLate = FALSE;
			if(schd_type == WORK_SCHEDULE || schd_type == SHIFT_SCHEDULE)
			{
				bShowAcceptTrainLate = FALSE;//暂时简单赋值 m_pDoc->m_bShowAcceptTrainLate;
				bAcceptTruckLate = FALSE;//暂时简单赋值 m_pDoc->m_bAcceptTruckLate;
			}
			else if(schd_type == HISTORY_SCHEDULE)
			{
				bShowAcceptTrainLate = FALSE;//暂时简单赋值 m_pDoc->m_bHistShowAcceptTrainLate;
				bAcceptTruckLate = FALSE;//暂时简单赋值 m_pDoc->m_bHistAcceptTruckLate;
			}
			else
			{
				bShowAcceptTrainLate = FALSE;
				bAcceptTruckLate = FALSE;
			}
		}
		nOldAlign = pCDC->SetTextAlign(TA_LEFT|TA_BOTTOM);
	} 
	else 
	{
		pCDC->MoveTo(uStartX,uStartY);
		pCDC->LineTo(uEndX,uStartY);

		BOOL bShowAcceptTrainLate = FALSE;
		BOOL bAcceptTruckLate = FALSE;
		if(schd_type == WORK_SCHEDULE || schd_type == SHIFT_SCHEDULE)
		{
			bShowAcceptTrainLate = FALSE;//暂时简单赋值 m_pDoc->m_bShowAcceptTrainLate;
			bAcceptTruckLate = FALSE;//暂时简单赋值 m_pDoc->m_bAcceptTruckLate;
		}
		else if(schd_type == BASIC_SCHEDULE)
		{
			bShowAcceptTrainLate = FALSE;
			bAcceptTruckLate = FALSE;
		}
		else if(schd_type == HISTORY_SCHEDULE)
		{
			bShowAcceptTrainLate = FALSE;//暂时简单赋值 m_pDoc->m_bHistShowAcceptTrainLate;
			bAcceptTruckLate = FALSE;//暂时简单赋值 m_pDoc->m_bHistAcceptTruckLate;
		}
		else
		{
			bShowAcceptTrainLate = FALSE;
			bAcceptTruckLate = FALSE;
		}

		nOldAlign = pCDC->SetTextAlign(TA_LEFT|TA_TOP);
	}

	UINT nOldColor = pCDC->SetTextColor(Color);

	CPoint basic;
	if(nDirect == UP_MARK)
	{
		pCDC->TextOut(uStartX  ,uEndY,strTrainId);
		basic = CPoint(uStartX,uEndY);
	}

	else
	{
		pCDC->TextOut(uStartX  ,uStartY,strTrainId);
		basic = CPoint(uStartX,uStartY);
	}

	//绘制机车号
	m_bShowTractionEngine=TRUE;
	if(GetTGFunctionConfig()->IsEnableLanzhouEnginePlot())
		DrawLanzhouMarkOfTractionEngine(pCDC,basic,sTextExtent,single,nDirect,schd_type,1);
	else
		DrawMarkOfTractionEngine(pCDC,basic,sTextExtent,single,nDirect,schd_type,0);
	
	// remove pen / brush
	pCDC->SelectObject(pOldFont);   
	pCDC->SetTextAlign(nOldAlign);
	pCDC->SetBkMode(nOldMode);
	pCDC->SetTextColor(nOldColor);
	pCDC->SelectObject(pOldPen);
	pCDC->SetROP2(nPreviousMode); 

	font.DeleteObject();            
	mPen.DeleteObject();
}

void CPlotDoc::DrawMarkOfCritical(CDC *pCDC,CTG_SingleTrain * single,CString strTrainId, CPoint ptlStart, int nLength, int nDirect, COLORREF Color)
{
	int scaleRate = min(yMultiple,5);
	if(ptlStart.y < y_offset_print)
		return;

	CPen mPen,*pOldPen;
	UINT nOldAlign;
	UINT nPreviousMode = pCDC->SetROP2(R2_COPYPEN); 
	UINT nOldMode = pCDC->SetBkMode(TRANSPARENT);


	mPen.CreatePen(PS_INSIDEFRAME, scaleRate*m_nLineWidthRate,Color); 
	pOldPen = pCDC->SelectObject(&mPen);

	int m_nDirectFlag_prefix = GetTrainRecDirect(single, 0);
	
	int nDir = m_nDirectFlag_prefix;
	int uStartX,uStartY,uEndX,uEndY;
	CPoint ptlEnd;

	int m_nLogPixelSy = pCDC->GetDeviceCaps(LOGPIXELSY);

	CFont font;
	LOGFONT stFont;
	CFont * pOldFont;
	memset(&stFont,0,sizeof(stFont));

	stFont.lfHeight = MulDiv(m_font.train_id,m_nLogPixelSy,72);
	stFont.lfWeight = FW_NORMAL;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	strcpy(stFont.lfFaceName,"Times New Roman");
	font.CreateFontIndirect(&stFont);
	pOldFont = pCDC->SelectObject(&font);
	int tem_index = GetAdditionalTrainInfoIndex(single->GetTrainIndex());
	if(tem_index == -1)
		return;

	nLength = 46;
	nLength = nLength*yMultiple;

	if(nDirect == LEFT_MARK)
	{
		ptlEnd.x = ptlStart.x - nLength;
		ptlEnd.y = ptlStart.y;
		uStartX = ptlStart.x;// - strTrainId.GetLength()*3*xMultiple;// text width is 60,30
		uEndX = ptlEnd.x;//ptlStart.x + strTrainId.GetLength()*3*xMultiple;
		uStartY = ptlEnd.y;// text height is 30
		uEndY = ptlEnd.y;
	} 
	else 
	{//RIGHT MARK 
		ptlEnd.y = ptlStart.y;//;strTrainId.GetLength()
		ptlEnd.x = ptlStart.x + nLength;		
		uStartX = ptlStart.x;// - strTrainId.GetLength()*3*xMultiple;
		uEndX = ptlEnd.x;//ptlStart.x + strTrainId.GetLength()*3*xMultiple;
		uStartY = ptlEnd.y;
		uEndY = ptlEnd.y;
	}
	//**********************************************
	//to create temp rect;
	CRect RectTemp,rectInter;
	if(uEndX > uStartX)
	{
		if(nDir == DOWN_DIRECT)
			RectTemp = CRect(uStartX,uStartY-abs(stFont.lfHeight),uEndX,uStartY);
		else
			RectTemp = CRect(uStartX,uStartY,uEndX,uStartY+abs(stFont.lfHeight));
	}
	else
	{
		if(nDir == DOWN_DIRECT)
			RectTemp = CRect(uEndX,uStartY-abs(stFont.lfHeight),uStartX,uStartY);
		else
			RectTemp = CRect(uEndX,uStartY,uStartX,uStartY+abs(stFont.lfHeight));
	}

	int Flag=0;   //if collidding 

	int nSchdTrainNum = m_vecSchdTrains.size();
	for(int i = 0;i < nSchdTrainNum;i++)
	{
		for(int j=0;j <= train_additional_info[i].m_nNumberOfMarkedRect; j++)
		{	   
			if(rectInter.IntersectRect(&train_additional_info[i].m_nRect[j],RectTemp) && (m_vecSchdTrains[i].GetTrainIndex() != single->GetTrainIndex()))
			{  
				Flag=1;
				break;	
			}
		}
		if( Flag==1)
			break;
	}

	//have found colliding  rect ;
	int offset=0; 
	int yoffset=0;

	int iDelta = 46*fHeightMulti;

	if(i < m_vecSchdTrains.size())
	{ 	
		if(nDirect == LEFT_MARK)//左标注
		{ 
			uStartX += -iDelta;
			uEndX +=-iDelta;
			offset = -iDelta; 
			yoffset = abs(stFont.lfHeight);//+8*yMultiple;
			RectTemp.OffsetRect(CPoint(-iDelta,yoffset));
		}
		else  //右标注
		{
			uStartX += iDelta;
			uEndX  += iDelta;
			offset = iDelta; 
			yoffset = abs(stFont.lfHeight);//+8*yMultiple;
			RectTemp.OffsetRect(CPoint(iDelta,yoffset));
		}
	}

	//2004-03-31 设置早晚点的圈的大小为实际尺寸大小
	//设置早晚点的圈直径为0.5英寸
	//1cm = 0.3937inch
	//1inch = 2.54cm
	int timeRectWidth = pCDC->GetDeviceCaps(LOGPIXELSX)*0.3937;//定义实际直径为1cm
	int timeRectHeight = pCDC->GetDeviceCaps(LOGPIXELSY);

	// to create new rect
	if(train_additional_info[tem_index].m_nNumberOfMarkedRect<=31)
		train_additional_info[tem_index].m_nNumberOfMarkedRect += 1;
	else
		train_additional_info[tem_index].m_nNumberOfMarkedRect = 1;

	train_additional_info[tem_index].m_nRect[train_additional_info[tem_index].m_nNumberOfMarkedRect-1]=RectTemp;

	pCDC->MoveTo(ptlStart.x,ptlStart.y);
	pCDC->LineTo(ptlEnd.x,ptlEnd.y + yoffset);
	pCDC->LineTo(ptlEnd.x+offset,ptlEnd.y + yoffset); ////

	UINT nOldColor = pCDC->SetTextColor(Color);
	if(nDirect == LEFT_MARK)
	{
		if(nDir == DOWN_DIRECT)
			nOldAlign = pCDC->SetTextAlign(TA_LEFT|TA_BOTTOM);
		else
			nOldAlign = pCDC->SetTextAlign(TA_LEFT|TA_TOP);

		pCDC->TextOut(ptlEnd.x+offset,ptlEnd.y + yoffset,strTrainId);
	} 
	else 
	{//RIGHT MARK
		if(nDir == DOWN_DIRECT)
			nOldAlign = pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
		else
			nOldAlign = pCDC->SetTextAlign(TA_RIGHT|TA_BOTTOM);
		pCDC->TextOut(ptlEnd.x+offset,ptlEnd.y + yoffset,strTrainId);
	}


	int delta = nLength;
	CSize size(delta,delta);
	CPoint  myPoint,tmpPoint;
	myPoint = CPoint(ptlEnd.x+offset,ptlEnd.y + yoffset);
	if(nDir == DOWN_DIRECT)
	{
		if(nDirect == LEFT_MARK)
		{
			tmpPoint.x = myPoint.x - delta/2;
			tmpPoint.y = myPoint.y - delta/2;
		}else{
			tmpPoint.x = myPoint.x + delta/2;
			tmpPoint.y = myPoint.y + delta/2;
		}
	}
	else
	{
		if(nDirect == LEFT_MARK)
		{		
			tmpPoint.x = myPoint.x - delta/2;
			tmpPoint.y = myPoint.y + delta/2;
		}else{
			tmpPoint.x = myPoint.x + delta/2;
			tmpPoint.y = myPoint.y - delta/2;
		}
	}


	//wangjinhai is here
	CSize trainidSize = pCDC->GetTextExtent(strTrainId);
	////CPoint basic = CPoint(ptlEnd.x -  trainidSize.cx,ptlEnd.y);
	CPoint basic;
	if(nDirect == LEFT_MARK)
		basic = CPoint(ptlEnd.x-  trainidSize.cx,ptlEnd.y-  trainidSize.cy);
	else
		basic = CPoint(ptlEnd.x ,ptlEnd.y -  trainidSize.cy);
	if(GetTGFunctionConfig()->IsEnableLanzhouEnginePlot())
		DrawLanzhouMarkOfTractionEngine(pCDC,basic,trainidSize,single,nDir,WORK_SCHEDULE,1);
	else
		DrawMarkOfTractionEngine(pCDC,basic,trainidSize,single,nDirect,WORK_SCHEDULE,0);
	

	// remove pen / brush
	pCDC->SelectObject(pOldFont);   
	pCDC->SetTextAlign(nOldAlign);
	pCDC->SetBkMode(nOldMode);
	pCDC->SetTextColor(nOldColor);
	pCDC->SelectObject(pOldPen);
	pCDC->SetROP2(nPreviousMode); 

	mPen.DeleteObject();
	font.DeleteObject();
}
void CPlotDoc::DrawMarkOfArrive(CDC *pCDC,CTG_SingleTrain * single,CString strTrainId, CPoint ptlStart, int nLength, int nDirect, COLORREF Color, bool bActual, int schd_type, int rec_index)
{
	int scaleRate = min(yMultiple,5);

	if(ptlStart.y < y_offset_print)
		return;

	for(int id_count = 0; id_count < single->GetTrainRecordCount();id_count++)
	{
		if(single->GetTrainRecord(id_count)->m_data.arrive_train_id != single->GetTrainRecord(id_count)->m_data.depart_train_id
			&& strlen(single->GetTrainRecord(id_count)->m_data.arrive_train_id)>0 && strlen(single->GetTrainRecord(id_count)->m_data.depart_train_id)>0)
		{
			CString str = CString(single->GetTrainRecord(id_count)->m_data.depart_train_id);
			CString strTrainId;
			strTrainId.Format("%s",single->GetTrainID());
			int len = strTrainId.GetLength();
			CString str0,str1,str_def;
			bool flag = FALSE;
			for(int i=1;i<=len;i++)
			{
				str0 = strTrainId.Left(i);
				str1 = str.Left(i);
				if(str0 == str1) continue;
				str_def = str.Right(str.GetLength() - i + 1);
				flag = TRUE;
				break;
			}
			if(flag)
				strTrainId = strTrainId + "/" + str_def;
			break;
		}
	}

	CPen mPen,*pOldPen;
	UINT nOldAlign;
	UINT nPreviousMode = pCDC->SetROP2(R2_COPYPEN); 
	UINT nOldMode = pCDC->SetBkMode(TRANSPARENT);
	UINT nOldColor = pCDC->SetTextColor(Color);
	mPen.CreatePen(PS_INSIDEFRAME,m_nLineWidthRate*scaleRate,Color);
	pOldPen = pCDC->SelectObject(&mPen);

	//车次号字体
	CFont font;
	LOGFONT stFont;
	CFont * pOldFont;
	int m_nLogPixelSy = pCDC->GetDeviceCaps(LOGPIXELSY);

	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(m_font.train_id,m_nLogPixelSy,72);
	stFont.lfWeight = FW_NORMAL;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	strcpy(stFont.lfFaceName,"Times New Roman");
	font.CreateFontIndirect(&stFont);
	pOldFont = pCDC->SelectObject(&font);

	int uStartX,uStartY,uEndX,uEndY;
	CPoint ptlEnd;

	int trainidHeight;
	CSize trainidSize = pCDC->GetTextExtent(strTrainId);
	nLength =  pCDC->GetTextExtent("K12345").cx*0.6;
	trainidHeight = trainidSize.cy;


#ifdef ACT_AS_JI_GUI
	if(nDirect == UP_MARK)
	{
		ptlEnd.x = ptlStart.x;
		ptlEnd.y = ptlStart.y - trainidHeight;
		uStartX = ptlStart.x - strTrainId.GetLength()*8*xMultiple;// the text width is 20,50
		uEndX = ptlStart.x;
		uStartY = ptlEnd.y - train_mark_height/2;// the text height is 20
		uEndY = ptlEnd.y;
	} else {
		ptlEnd.y = ptlStart.y + trainidHeight;
		ptlEnd.x = ptlStart.x;
		uStartX = ptlStart.x - strTrainId.GetLength()*8*xMultiple;
		uEndX = ptlStart.x;
		uStartY = ptlEnd.y;
		uEndY = ptlEnd.y  - train_mark_height/2;
	}
#else
	if(nDirect == UP_MARK)
	{
		ptlEnd.x = ptlStart.x - trainidHeight;
		ptlEnd.y = ptlStart.y - trainidHeight;
		uStartX = ptlEnd.x - strTrainId.GetLength()*8*xMultiple;// the text width is 20,50
		uEndX = ptlEnd.x;
		uStartY = ptlEnd.y - train_mark_height/2;// the text height is 20
		uEndY = ptlEnd.y;
	} 
	else 
	{
		ptlEnd.y = ptlStart.y + trainidHeight;
		ptlEnd.x = ptlStart.x - trainidHeight;
		uStartX = ptlEnd.x - strTrainId.GetLength()*8*xMultiple;
		uEndX = ptlEnd.x;
		uStartY = ptlEnd.y;
		uEndY = ptlEnd.y  - train_mark_height/2;
	}
#endif

	if(uStartY < 20)
	{
		sysprint(0,PLOT.ThreadId,"uStartY<20 %d ",uStartY);
		return;
	}

	//2004-03-31 设置早晚点的圈的大小为实际尺寸大小
	//设置早晚点的圈直径为0.5英寸
	//1cm = 0.3937inch
	//1inch = 2.54cm
	int timeRectWidth = pCDC->GetDeviceCaps(LOGPIXELSX)*0.3937;//定义实际直径为1cm
	int timeRectHeight = pCDC->GetDeviceCaps(LOGPIXELSY);

	//2004-12-14
	//int increaeelayer = single->m_nIncreaseLayer;
	//加入新的打印方式
	int increaeelayer;
	int tem_index = GetAdditionalTrainInfoIndex(single->GetTrainIndex());
	if(tem_index == -1)
		return;
	if(gpcdef.GetTrainIDFormat()==2)
	{
		increaeelayer =1;
		int stationno,i;
		//bool bFind = false;
		stationno=this->ConvertYPostoStationNo(ptlStart.y-y_offset_print,i);
		for(i=0;i<train_additional_info[tem_index].ptoftrainid_count;i++)
		{
			if(stationno==train_additional_info[tem_index].stPttoTrainid[i].stationno)
			{
				train_additional_info[tem_index].stPttoTrainid[i].layer = GetIncreaseLayer(single,CPoint(ptlEnd.x - trainidSize.cx,ptlEnd.y),nDirect,stationno,pCDC,schd_type);
				increaeelayer =train_additional_info[tem_index].stPttoTrainid[i].layer;
				break;
			}
		}

		if(train_additional_info[tem_index].ptoftrainid_count == 0)
		{
			train_additional_info[tem_index].stPttoTrainid[0].layer = GetIncreaseLayer(single,CPoint(ptlEnd.x - trainidSize.cx,ptlEnd.y),nDirect,stationno,pCDC,schd_type);
			train_additional_info[tem_index].ptoftrainid_count++;
		}
	}
	else
	{
		train_additional_info[tem_index].m_nIncreaseLayer= GetIncreaseLayer(single,pCDC,schd_type);
		increaeelayer = train_additional_info[tem_index].m_nIncreaseLayer;
	}

	int iDelta = trainidSize.cy*(increaeelayer-1);

	if(nDirect == UP_MARK)//上标注
	{   
		ptlEnd.y += -iDelta;
	}
	else  //下标注
	{		
		ptlEnd.y += iDelta;
	}

	if(m_nShowElecLocoFlag)
	{
		if(IsElectricTrain(single))
		{
			//绘制电力机车的方框
			CRect m_rect;	CPoint cPoint; CSize rectSize;
			cPoint.x = ptlEnd.x - trainidSize.cx-2 ;
			rectSize = trainidSize;
			rectSize.cx +=2;

			if(nDirect == UP_MARK)
			{
				cPoint.y = ptlEnd.y  - rectSize.cy ;
			}
			else
			{ 
				cPoint.y = ptlEnd.y +2 ;

			}
			rectSize.cy -=2;

			m_rect = CRect(cPoint,rectSize);
			pCDC->MoveTo(m_rect.TopLeft());
			pCDC->LineTo(m_rect.BottomRight().x, m_rect.TopLeft().y);
			pCDC->LineTo(m_rect.BottomRight());
			pCDC->LineTo(m_rect.TopLeft().x, m_rect.BottomRight().y );
			pCDC->LineTo(m_rect.TopLeft() );
		}
	}

	pCDC->MoveTo(ptlStart.x,ptlStart.y);
	pCDC->LineTo(ptlEnd.x,ptlEnd.y);

	CTG_TrainRecord *first,*second;
	int rec_count = single->GetTrainRecordCount();
	first = single->GetTrainRecord(0);
	if(first ==  NULL) return;
	second = NULL;

	if(rec_count >1)
		second = single->GetTrainRecord(1);

	bool bIsFenJieStation = FALSE;
	vector<int>::iterator theIterator;  
	for (theIterator = fenjie_station_list.begin();
		theIterator != fenjie_station_list.end();
		theIterator++)
	{
		if(*theIterator == first->m_data.station){
			bIsFenJieStation = TRUE;
			break;
		}
	}

	int train_type = single->GetTrainInfo()->m_data.type;
	vector<int>::iterator arrayIterator;
	for(arrayIterator = train_type_inhibit_array[m_nCurrentSectionNo].begin();
		arrayIterator != train_type_inhibit_array[m_nCurrentSectionNo].end();arrayIterator ++)
	{
		if(arrayIterator != NULL)
		{
			if(train_type == *arrayIterator)
			{
				bIsFenJieStation = TRUE;
				break;
			}
		}
	}

	bool bIsBianZhuStation = FALSE;
	vector<int>::iterator theBianZhuIterator;  
	for (theBianZhuIterator = bianzhu_station_list.begin();
		theBianZhuIterator != bianzhu_station_list.end();
		theBianZhuIterator++)
	{
		if(*theBianZhuIterator == first->m_data.station){
			bIsBianZhuStation = TRUE;
			break;
		}
		if(second != NULL && *theBianZhuIterator == second->m_data.station ){
			bIsBianZhuStation = TRUE;
			break;		    
		}
	}	



	CPoint endPoint;
	if(nDirect == UP_MARK)
	{
		pCDC->MoveTo(ptlEnd.x,ptlEnd.y);
		pCDC->LineTo(ptlEnd.x - trainidSize.cx, ptlEnd.y);

		endPoint.x = ptlEnd.x - trainidSize.cx - trainidSize.cy;
		endPoint.y = ptlEnd.y - trainidSize.cy;

		BOOL bShowAcceptTrainLate = FALSE;
		BOOL bAcceptTruckLate = FALSE;
		if(schd_type == WORK_SCHEDULE || schd_type == SHIFT_SCHEDULE)
		{
			bShowAcceptTrainLate = FALSE;//暂时简单赋值 m_pDoc->m_bShowAcceptTrainLate;
			bAcceptTruckLate = FALSE;//暂时简单赋值 m_pDoc->m_bAcceptTruckLate;
		}
		else if(schd_type == BASIC_SCHEDULE)
		{
			bShowAcceptTrainLate = FALSE;
			bAcceptTruckLate = FALSE;
		}
		else if(schd_type == HISTORY_SCHEDULE)
		{
			bShowAcceptTrainLate = FALSE;//暂时简单赋值 m_pDoc->m_bHistShowAcceptTrainLate;
			bAcceptTruckLate = FALSE;//暂时简单赋值 m_pDoc->m_bHistAcceptTruckLate;
		}
		else
		{
			bShowAcceptTrainLate = FALSE;
			bAcceptTruckLate = FALSE;
		}

		if(schd_type == BASIC_SCHEDULE) //基本图不画分界口，区段站晚点时分，别的都要
		{
			bShowAcceptTrainLate = FALSE;
			bAcceptTruckLate = FALSE;
		}

		if(  ((IsTrain_Plot(single->GetTrainInfo()) && bShowAcceptTrainLate)
			|| (IsTruck_Plot(single->GetTrainInfo()) && bAcceptTruckLate))&& !bIsFenJieStation)
		{
			do
			{
				if(IsTruck_Plot(single->GetTrainInfo())&& bIsBianZhuStation)
					break;			
				int  timeValue;
				CString str_text;
				CPen nPen;
				CPen *ppoldPen;

				CFont nfont;			
				CFont * ppOldFont;

				memset(&stFont,0,sizeof(stFont));
				stFont.lfHeight = MulDiv(m_font.late_time,m_nLogPixelSy,72);
				stFont.lfWeight = FW_THIN;
				stFont.lfClipPrecision = CLIP_LH_ANGLES;
				strcpy(stFont.lfFaceName,"Times New Roman");
				nfont.CreateFontIndirect(&stFont);

				ppOldFont = pCDC->SelectObject(&nfont);


				timeValue = single->GetTrainInfo()->m_data.sborder_actual_time - single->GetTrainInfo()->m_data.sborder_plan_time;
				timeValue = timeValue/60;
				if(timeValue>0)
				{
					nPen.CreatePen (PS_JOIN_ROUND,(int)1*scaleRate,CLR_BLUE);  //PS_SOLID
				}
				else
				{
					nPen.CreatePen (PS_JOIN_ROUND,(int)1*scaleRate,CLR_RED);  //PS_SOLID
				}


				CRect tempRect(CPoint(endPoint.x - timeRectWidth/2, endPoint.y - timeRectWidth)
					,CSize(timeRectWidth*2/3,timeRectWidth*2/3));


				if(abs(timeValue/60) < 1)
					if(timeValue==0) str_text.Format("");
					else str_text.Format("%d",abs(timeValue%60));
				else
					str_text.Format("%d.%02d",abs(timeValue/60),abs(timeValue%60)); 


				ppoldPen = pCDC->SelectObject(&nPen);
				UINT nOldColor = pCDC->SetTextColor(CLR_BLACK);


				pCDC->Arc(tempRect,CPoint(tempRect.left ,tempRect.CenterPoint().y),CPoint(tempRect.left,tempRect.CenterPoint().y));
				CSize tx_size;
				tx_size = pCDC->GetTextExtent(str_text);
				pCDC->SetTextAlign(TA_CENTER);
				pCDC->TextOut (tempRect.CenterPoint().x, tempRect.CenterPoint().y - tx_size.cy/2,str_text);//added by Qian Guorong 


				pCDC->SetTextColor(nOldColor);
				pCDC->SelectObject(ppoldPen);
				pCDC->SelectObject(ppOldFont);

				nPen.DeleteObject();
				nfont.DeleteObject();
			}while(0);
		}

		nOldAlign = pCDC->SetTextAlign(TA_LEFT|TA_BOTTOM);
		pCDC->TextOut(ptlEnd.x - trainidSize.cx,ptlEnd.y,strTrainId);

		sysprint(0,PLOT.ThreadId,"textout %d %d",ptlEnd.x-trainidSize.cx,ptlEnd.y);
	}
	else 
	{
		pCDC->MoveTo(ptlEnd.x,ptlEnd.y);
		pCDC->LineTo(ptlEnd.x - trainidSize.cx, ptlEnd.y );

		//郑州不需要斜线
		endPoint.x = ptlEnd.x - trainidSize.cx - trainidSize.cy;
		endPoint.y = ptlEnd.y + trainidSize.cy;

		BOOL bShowAcceptTrainLate = FALSE;
		BOOL bAcceptTruckLate = FALSE;
		if(schd_type == WORK_SCHEDULE || schd_type == SHIFT_SCHEDULE)
		{
			bShowAcceptTrainLate = FALSE;//暂时简单赋值 m_pDoc->m_bShowAcceptTrainLate;
			bAcceptTruckLate = FALSE;//暂时简单赋值 m_pDoc->m_bAcceptTruckLate;
		}
		else if(schd_type == BASIC_SCHEDULE) //基本图不画分界口，区段站晚点时分，别的都要
		{
			bShowAcceptTrainLate = FALSE;
			bAcceptTruckLate = FALSE;
		}
		else if(schd_type == HISTORY_SCHEDULE)
		{
			bShowAcceptTrainLate = FALSE;//暂时简单赋值 m_pDoc->m_bHistShowAcceptTrainLate;
			bAcceptTruckLate = FALSE;//暂时简单赋值 m_pDoc->m_bHistAcceptTruckLate;
		}
		else
		{
			bShowAcceptTrainLate = FALSE;
			bAcceptTruckLate = FALSE;
		}

		if(schd_type == BASIC_SCHEDULE) //基本图不画分界口，区段站晚点时分，别的都要
		{
			bShowAcceptTrainLate = FALSE;
			bAcceptTruckLate = FALSE;
		}

		if( ( (IsTrain_Plot(single->GetTrainInfo()) && bShowAcceptTrainLate)
			|| (IsTruck_Plot(single->GetTrainInfo()) && bAcceptTruckLate))&& !bIsFenJieStation)
		{
			do
			{
				if(IsTruck_Plot(single->GetTrainInfo())&& bIsBianZhuStation)
					break;

				int  timeValue;
				CPen nPen;
				CPen *ppoldPen;

				CFont nfont;			
				CFont * ppOldFont;

				memset(&stFont,0,sizeof(stFont));
				
				stFont.lfHeight = MulDiv(m_font.late_time,m_nLogPixelSy,72);
				stFont.lfWeight = FW_THIN;
				stFont.lfClipPrecision = CLIP_LH_ANGLES;
				strcpy(stFont.lfFaceName,"Times New Roman");
				nfont.CreateFontIndirect(&stFont);
				ppOldFont = pCDC->SelectObject(&nfont);

				CString str_text;
				timeValue = single->GetTrainInfo()->m_data.sborder_actual_time - single->GetTrainInfo()->m_data.sborder_plan_time; 
				
				timeValue = timeValue/60;
				if(timeValue>0)
				{
					nPen.CreatePen (PS_JOIN_ROUND,(int)1*scaleRate,CLR_BLUE);  //PS_SOLID
				}
				else
				{
					nPen.CreatePen (PS_JOIN_ROUND,(int)1*scaleRate,CLR_RED);  //PS_SOLID
				}

				CRect tempRect(CPoint(endPoint.x - timeRectWidth/2, endPoint.y)
					,CSize(timeRectWidth*2/3,timeRectWidth*2/3));


				ppoldPen = pCDC->SelectObject(&nPen);

				if(abs(timeValue/60) < 1)
					if(timeValue==0) str_text.Format("");
					else str_text.Format("%d",abs(timeValue%60));
				else
					str_text.Format("%d.%02d",abs(timeValue/60),abs(timeValue%60));  


				UINT nOldColor = pCDC->SetTextColor(CLR_BLACK);

				pCDC->Arc(tempRect,CPoint(tempRect.left ,tempRect.CenterPoint().y),CPoint(tempRect.left,tempRect.CenterPoint().y));
				CSize tx_size;
				tx_size = pCDC->GetTextExtent(str_text);
				pCDC->SetTextAlign(TA_CENTER);
				pCDC->TextOut (tempRect.CenterPoint().x, tempRect.CenterPoint().y - tx_size.cy/2,str_text); 


				pCDC->SetTextColor(nOldColor);
				pCDC->SelectObject(ppoldPen);	
				pCDC->SelectObject(ppOldFont);

				nPen.DeleteObject();
				nfont.DeleteObject();
			}while(0);

		}

		nOldAlign = pCDC->SetTextAlign(TA_LEFT|TA_TOP);
		pCDC->TextOut(ptlEnd.x -  trainidSize.cx,ptlEnd.y,strTrainId);
	}

	CPoint basic = CPoint(ptlEnd.x -  trainidSize.cx,ptlEnd.y);

	m_bShowTractionEngine=TRUE;
	
	if(GetTGFunctionConfig()->IsEnableLanzhouEnginePlot())
		DrawLanzhouMarkOfTractionEngine(pCDC,basic,trainidSize,single,nDirect,schd_type,1);
	else
		DrawMarkOfTractionEngine(pCDC,basic,trainidSize,single,nDirect,schd_type, rec_index);


	pCDC->SelectObject(pOldFont);  
	pCDC->SetTextAlign(nOldAlign);
	pCDC->SetBkMode(nOldMode);
	pCDC->SetTextColor(nOldColor);
	pCDC->SelectObject(pOldPen);
	pCDC->SetROP2(nPreviousMode); 

	font.DeleteObject();
	mPen.DeleteObject();
}

//绘制固定大小的交出符号
void CPlotDoc::DrawMarkOfDepartFixedSize(CDC *pCDC,CTG_SingleTrain * single,CString strTrainId, 
										 CPoint ptlStart, int nLength, int nDirect, COLORREF Color, bool bActual)
{
	if(ptlStart.y <= 0)
		return;

	int scaleRate = min(yMultiple,5);
	CPen mPen,*pOldPen;
	POINT ptl[4];
	CRect rect;
	UINT nPreviousMode = pCDC->SetROP2(R2_COPYPEN); 
	UINT nOldMode = pCDC->SetBkMode(TRANSPARENT);
	int penWidth = scaleRate*m_nLineWidthRate;
	mPen.CreatePen(PS_INSIDEFRAME, scaleRate*m_nLineWidthRate, Color);
	pOldPen = pCDC->SelectObject(&mPen);

	if(nDirect == UP_MARK)
	{
		ptl[0].x = ptlStart.x;
		ptl[0].y = ptlStart.y;
		ptl[1].x = ptl[0].x;
		ptl[1].y = ptl[0].y - 6*scaleRate;
		ptl[2].x = ptl[1].x + 10*scaleRate;
		ptl[2].y = ptl[1].y;
		ptl[3].x = ptl[2].x - 3*scaleRate;
		ptl[3].y = ptl[2].y - 5*scaleRate;

		rect = CRect(CPoint(ptl[1].x - penWidth/2,ptl[1].y - penWidth/2),CSize(penWidth,penWidth));
	} 
	else 
	{
		ptl[0].x = ptlStart.x;
		ptl[0].y = ptlStart.y;
		ptl[1].x = ptl[0].x;
		ptl[1].y = ptl[0].y + 6*scaleRate;
		ptl[2].x = ptl[1].x + 10*scaleRate;
		ptl[2].y = ptl[1].y;
		ptl[3].x = ptl[2].x - 3*scaleRate;
		ptl[3].y = ptl[2].y + 5*scaleRate;

		rect = CRect(0,0,0,0);
	}
	pCDC->Polyline(ptl,4);

	pCDC->FillRect(&rect,&CBrush(Color));

	// remove pen / brush
	pCDC->SelectObject(pOldPen);
	pCDC->SetROP2(nPreviousMode); 
	pCDC->SetBkMode(nOldMode);
	BOOL ret = mPen.DeleteObject();
}
void CPlotDoc::DrawMarkOfDepart(CDC *pCDC,CTG_SingleTrain * single,CString strTrainId, CPoint ptlStart, int nLength, int nDirect, COLORREF Color, bool bActual)
{
	DrawMarkOfDepartFixedSize(pCDC,single, strTrainId,ptlStart,nLength,nDirect,Color, bActual); // 2008 qiaoyt
	return;
}

void CPlotDoc::DrawMarkOfEnd(CDC *pCDC,CTG_SingleTrain * single,CString strTrainId, CPoint ptlStart, 
							 int nLength, int nDirect, COLORREF Color,USHORT flag, bool bActual)
{
	int scaleRate = min(yMultiple,5);
	int xscaleRate = min(xMultiple,5);
	if(ptlStart.y < y_offset_print)
		return;

	CPen mPen,*pOldPen;
	UINT nPreviousMode = pCDC->SetROP2(R2_COPYPEN); 
	UINT nOldMode = pCDC->SetBkMode(TRANSPARENT);
	mPen.CreatePen(PS_INSIDEFRAME, scaleRate*m_nLineWidthRate, Color);
	pOldPen = pCDC->SelectObject(&mPen);

	int uStartX,uStartY,uEndX,uEndY;
	CPoint ptlEnd;
	nLength = 8*xscaleRate;

	if(nDirect == UP_MARK)
	{
		ptlEnd.x = ptlStart.x;
		ptlEnd.y = ptlStart.y - nLength;
		uStartX = ptlStart.x - 4*xscaleRate;
		uEndX = ptlStart.x + 4*xscaleRate;
		uStartY = ptlEnd.y - 8*xscaleRate;
		uEndY = ptlEnd.y;
	} 
	else 
	{
		ptlEnd.y = ptlStart.y + nLength;
		ptlEnd.x = ptlStart.x;
		uStartX = ptlStart.x - 4*xscaleRate;
		uEndX = ptlStart.x + 4*xscaleRate;
		uStartY = ptlEnd.y;
		uEndY = ptlEnd.y + 8*xscaleRate;
	}

	POINT ptl[4];
	pCDC->MoveTo(ptlStart.x,ptlStart.y);
	pCDC->LineTo(ptlEnd.x,ptlEnd.y);
	
	if(nDirect == UP_MARK)
	{
		ptl[3].x = ptl[0].x = uStartX;
		ptl[3].y = ptl[0].y = uEndY;
		ptl[1].x = uEndX;
		ptl[1].y = uEndY;
		ptl[2].x = ptlEnd.x;
		ptl[2].y = uStartY;
	} 
	else 
	{
		ptl[3].x = ptl[0].x = uStartX;
		ptl[3].y = ptl[0].y = uStartY;
		ptl[1].x = uEndX;
		ptl[1].y = uStartY;
		ptl[2].x = ptlEnd.x;
		ptl[2].y = uEndY;
	}

	//added by qianguorong 2003-07-09
	if(IsFlagEnd(flag))//临时停运
	{
		CPoint topleft,botright;
		CPoint start;
		CRect tempRect;
		if(nDirect == UP_MARK)
		{
			topleft.x = ptlEnd.x - 4*scaleRate;
			topleft.y = ptlEnd.y - 8*scaleRate;

			botright.x = ptlEnd.x + 4*scaleRate;
			botright.y = ptlEnd.y;			
		}
		else
		{
			topleft.x = ptlEnd.x - 4*scaleRate;
			topleft.y = ptlEnd.y;

			botright.x = ptlEnd.x + 4*scaleRate;
			botright.y = ptlEnd.y + 8*scaleRate;
		}
		tempRect.SetRect(topleft,botright);

		start.x = (tempRect.left + tempRect.right) /2;
		start.y = tempRect.top;
		pCDC->Arc(tempRect,start,start);
	}
	else
	{
		pCDC->Polyline(ptl,4);
	}
 
	pCDC->SelectObject(pOldPen);
	pCDC->SetROP2(nPreviousMode); 
	pCDC->SetBkMode(nOldMode);
          
	mPen.DeleteObject();
}
void CPlotDoc::DrawActualLine(CDC* pCDC, CPoint& ptlStart, CPoint& ptlEnd,int factor,bool bAbsLine,
							  int nEngine,int nCarriageCount, bool bIsActual)
{
	int scaleRate = min(yMultiple,5);
	int nLineWidth = scaleRate*m_nLineWidthRate;

	if(ptlStart.y < 0 || ptlEnd.y < 0)
		return;
	int line_type = 21;
	LINE_TYPE style;
	if(m_lineCfg.GetTrainLineStyle(factor, style))
		line_type = style.type;

	COLORREF clrActual = RGB(255,0,0);
	LINE_COLOR color;
	color.schd = RGB(255,0,0);
	color.real = RGB(255,0,0);

	if(m_lineCfg.GetTrainLineColor(factor, color))
	{
		if (bIsActual)
			clrActual = color.real;
		else
			clrActual = color.schd;
		line_type=color.line_type;
	}
	else
	{
		clrActual = RGB(255,0,0);
		color.vert = RGB(255,0,0);
	}

	if((bIsActual==false) && (m_lineCfg.IsSchdLineSameToActualLine() == true))
	{
		if(m_lineCfg.IsChangeVertColor())
			color.vert = clrActual;
	}

	BOOL bExist_Engine = FALSE;
	if(factor >= 143 && nEngine > 0)
	{
		bExist_Engine = TRUE;
	}
	if(13 == line_type)
	{
		bExist_Engine = FALSE;
	}

	CPoint m_ptStart = ptlStart;
	CPoint m_ptEnd = ptlEnd;

	//define line corlor
	//draw line type 
	switch(line_type)
	{
	case 0: // 一般客车─────
	case 5:
	case 6:
	case 20:
	case 31:// 太原局扩充用两万吨
	case 32:// 太原局扩充用一万吨
	case 40:// 太原局扩充用救援列车
		{
			CPen mPen,*pOldPen,mRPen;
			mPen.CreatePen(PS_SOLID, nLineWidth, clrActual);
			pOldPen = pCDC->SelectObject(&mPen);

			pCDC->MoveTo(m_ptStart.x,m_ptStart.y);
			pCDC->LineTo(m_ptEnd.x,m_ptEnd.y);

			pCDC->SelectObject (pOldPen);
			mPen.DeleteObject();
		}
		break;
	case 18:// —→—→—
		{
			CPen mPen,*pOldPen,mRPen;
			CBrush mBrush(clrActual),*pOldBrush;
			mPen.CreatePen(PS_SOLID, nLineWidth, clrActual);
			pOldPen = pCDC->SelectObject(&mPen);


			CPoint ptMiddle,ptRect[4],ptTemp;
			int nSize = 4,nSize2=5,nSize3 = 13;
			double fDelta;
			ptMiddle.x = m_ptStart.x *1/2 + m_ptEnd.x / 2;
			ptMiddle.y = m_ptStart.y *1/2 + m_ptEnd.y / 2;
			fDelta = (double)(m_ptEnd.y - m_ptStart.y) / (double)(m_ptEnd.x - m_ptStart.x);
			ptRect[0].y = (int)(nSize * fDelta / sqrt(fDelta * fDelta + 1));
			ptRect[0].x = (int)(nSize / sqrt(fDelta * fDelta + 1));
			ptRect[0].Offset(ptMiddle.x,ptMiddle.y);

			ptTemp.y = (int)(-7 * fDelta / sqrt(fDelta * fDelta + 1));
			ptTemp.x = (int)(-7 / sqrt(fDelta * fDelta + 1));
			ptTemp.Offset(ptMiddle.x,ptMiddle.y);
			pCDC->MoveTo(m_ptStart.x,m_ptStart.y);
			pCDC->LineTo(ptTemp.x,ptTemp.y);

			ptTemp.y = (int)(-5 * fDelta / sqrt(fDelta * fDelta + 1));
			ptTemp.x = (int)(-5 / sqrt(fDelta * fDelta + 1));
			ptTemp.Offset(ptMiddle.x,ptMiddle.y);

			ptRect[1].y = (int)(nSize2 / sqrt(fDelta * fDelta + 1));
			ptRect[1].x = (int)((-nSize2) * fDelta / sqrt(fDelta * fDelta + 1));
			ptRect[1].Offset(ptMiddle.x,ptMiddle.y);

			ptRect[3].y = (int)(nSize3 * fDelta / sqrt(fDelta * fDelta + 1));
			ptRect[3].x = (int)(nSize3 / sqrt(fDelta * fDelta + 1));
			ptRect[3].Offset(ptTemp.x,ptTemp.y);
			pCDC->MoveTo(ptTemp.x,ptTemp.y);
			pCDC->LineTo(ptRect[3].x,ptRect[3].y);

			ptTemp.y = (int)((nSize3+2) * fDelta / sqrt(fDelta * fDelta + 1));
			ptTemp.x = (int)((nSize3+2) / sqrt(fDelta * fDelta + 1));
			ptTemp.Offset(ptMiddle.x,ptMiddle.y);
			pCDC->MoveTo(ptTemp.x,ptTemp.y);
			pCDC->LineTo(m_ptEnd.x,m_ptEnd.y);

			ptRect[2].y = (int)((-nSize2) / sqrt(fDelta * fDelta + 1));
			ptRect[2].x = (int)(nSize2 * fDelta / sqrt(fDelta * fDelta + 1));
			ptRect[2].Offset(ptMiddle.x,ptMiddle.y);

			// draw start line
			pOldBrush=pCDC->SelectObject(&mBrush);
			pCDC->BeginPath();
			pCDC->MoveTo(ptRect[0].x,ptRect[0].y);
			pCDC->LineTo(ptRect[1].x,ptRect[1].y);
			pCDC->LineTo(ptRect[3].x,ptRect[3].y);
			pCDC->LineTo(ptRect[2].x,ptRect[2].y);

			pCDC->EndPath();
			pCDC->FillPath();
			pCDC->SelectObject (pOldPen);
			mPen.DeleteObject();
			pCDC->SelectObject (pOldBrush);
			mBrush.DeleteObject();
		}

		break;

	case 1: // —‖—‖—
	case 16:
		{
			CPen mPen,*pOldPen,mRPen;
			mPen.CreatePen(PS_SOLID, nLineWidth, clrActual);
			mRPen.CreatePen(PS_SOLID, nLineWidth, color.vert);
			pOldPen = pCDC->SelectObject(&mPen);

			if(bAbsLine)
			{
				//middle rect size is 4 * 4 pixel
				CPoint ptMiddle,ptRect[4],ptTemp;
				int x,y,flag;
				double nSize = 3*scaleRate,nSize2 = 8*scaleRate;
				double fDelta,fThreeSqrt = sqrt((double)3);

				if(TRUE)
				{
					fDelta = (double)(m_ptEnd.y - m_ptStart.y) / (double)(m_ptEnd.x - m_ptStart.x);
					ptRect[0].y = (int)(nSize * (fDelta + fThreeSqrt) / sqrt(fDelta * fDelta + 1));
					ptRect[0].x = (int)(nSize * (1 - fThreeSqrt * fDelta) / sqrt(fDelta * fDelta + 1));

					ptRect[1].y = (int)(nSize * (fThreeSqrt - fDelta) / sqrt(fDelta * fDelta + 1));
					ptRect[1].x = (int)((-nSize) * (1 + fThreeSqrt * fDelta) / sqrt(fDelta * fDelta + 1));

					ptRect[2].y = (int)((-nSize) * (fDelta + fThreeSqrt) / sqrt(fDelta * fDelta + 1));
					ptRect[2].x = (int)((-nSize) * (1 - fThreeSqrt * fDelta) / sqrt(fDelta * fDelta + 1));

					ptRect[3].y = (int)(nSize * (fDelta - fThreeSqrt) / sqrt(fDelta * fDelta + 1));
					ptRect[3].x = (int)(nSize * (1 + fThreeSqrt * fDelta) / sqrt(fDelta * fDelta + 1));
					y = (int)(nSize2 * fDelta / sqrt(fDelta * fDelta + 1));
					x = (int)(nSize2 / sqrt(fDelta * fDelta + 1));
					flag = 0;
				} 
				else 
				{
					fDelta = (double)(m_ptStart.x - m_ptEnd.x) / (double)(m_ptStart.y - m_ptEnd.y);
					ptRect[0].y = (int)(nSize * (1 + fDelta * fThreeSqrt) / sqrt(fDelta * fDelta + 1));
					ptRect[0].x = (int)(nSize * (fDelta - fThreeSqrt) / sqrt(fDelta * fDelta + 1));

					ptRect[1].y = (int)(nSize * (fThreeSqrt * fDelta - 1) / sqrt(fDelta * fDelta + 1));
					ptRect[1].x = (int)((-nSize) * (fDelta + fThreeSqrt) / sqrt(fDelta * fDelta + 1));

					ptRect[2].y = (int)((-nSize) * (1 + fDelta * fThreeSqrt) / sqrt(fDelta * fDelta + 1));
					ptRect[2].x = (int)((-nSize) * (fDelta - fThreeSqrt) / sqrt(fDelta * fDelta + 1));

					ptRect[3].y = (int)(nSize * (1 - fDelta * fThreeSqrt) / sqrt(fDelta * fDelta + 1));
					ptRect[3].x = (int)(nSize * (fThreeSqrt + fDelta) / sqrt(fDelta * fDelta + 1));
					y = (int)(nSize2 / sqrt(fDelta * fDelta + 1));
					x = (int)(nSize2 * fDelta  / sqrt(fDelta * fDelta + 1));
					flag = 1;
				} 

				// draw start line
				ptMiddle.x = m_ptStart.x /2 + m_ptEnd.x / 2;
				ptMiddle.y = m_ptStart.y /2 + m_ptEnd.y / 2;
				ptTemp.x = ptMiddle.x - x;
				ptTemp.y = ptMiddle.y - y;
				if(flag == 0)
				{
					pCDC->MoveTo(m_ptStart.x,m_ptStart.y - 1);
					pCDC->LineTo(ptTemp.x,ptTemp.y - 1);
				} 
				else 
				{
					pCDC->MoveTo(m_ptStart.x - 1,m_ptStart.y);
					pCDC->LineTo(ptTemp.x - 1,ptTemp.y);
				}
				pCDC->SelectObject(pOldPen);
				pOldPen = pCDC->SelectObject(&mRPen);


				pCDC->MoveTo(ptRect[0].x + ptMiddle.x,ptRect[0].y + ptMiddle.y);
				pCDC->LineTo(ptRect[3].x + ptMiddle.x,ptRect[3].y + ptMiddle.y);
				pCDC->MoveTo(ptRect[1].x + ptMiddle.x,ptRect[1].y + ptMiddle.y);
				pCDC->LineTo(ptRect[2].x + ptMiddle.x,ptRect[2].y + ptMiddle.y);

				pCDC->SelectObject(pOldPen);
				pOldPen = pCDC->SelectObject(&mPen);
				ptTemp.x = ptMiddle.x + x;
				ptTemp.y = ptMiddle.y + y;
				if(flag == 0)
				{
					pCDC->MoveTo(ptTemp.x,ptTemp.y - 1);
					pCDC->LineTo(m_ptEnd.x,m_ptEnd.y - 1);
				} 
				else
				{
					pCDC->MoveTo(ptTemp.x - 1,ptTemp.y);
					pCDC->LineTo(m_ptEnd.x - 1,m_ptEnd.y);
				}
			}
			else
			{
				pCDC->MoveTo(m_ptStart.x,m_ptStart.y);
				pCDC->LineTo(m_ptEnd.x,m_ptEnd.y);
			}
			pCDC->SelectObject(pOldPen);

			mPen.DeleteObject();
			mRPen.DeleteObject();
		}
		break;
	case 3: // —○—○—
	case 4:
	case 7:
	case 12:
		{
			CPen mPen,*pOldPen,mRPen;
			mPen.CreatePen(PS_SOLID, nLineWidth, clrActual);
			mRPen.CreatePen(PS_SOLID, nLineWidth, color.vert);
			pOldPen = pCDC->SelectObject(&mPen);

			if(bAbsLine )
			{
				//middle rect size is 4 * 4 pixel
				CPoint ptMiddle,ptTemp;
				int x,y,nSize = 10*scaleRate;
				double fDelta;

				if(TRUE)
				{
					fDelta = (double)(m_ptStart.y - m_ptEnd.y) / (double)(m_ptStart.x - m_ptEnd.x);
					y = (int)(nSize * fDelta / sqrt(fDelta * fDelta + 1));
					x = (int)(nSize / sqrt(fDelta * fDelta + 1));
				} 
				else 
				{
					fDelta = (double)(m_ptStart.x - m_ptEnd.x) / (double)(m_ptStart.y - m_ptEnd.y);
					y = (int)(nSize / sqrt(fDelta * fDelta + 1));
					x = (int)(nSize * fDelta  / sqrt(fDelta * fDelta + 1));
				} 

				// draw start line
				pCDC->MoveTo(m_ptStart);
				ptMiddle.x = m_ptStart.x /2 + m_ptEnd.x / 2;
				ptMiddle.y = m_ptStart.y /2 + m_ptEnd.y / 2;
				ptTemp.x = ptMiddle.x - x;
				ptTemp.y = ptMiddle.y - y;
				pCDC->LineTo(ptTemp);

				pCDC->SelectObject(pOldPen);
				pOldPen = pCDC->SelectObject(&mRPen);
				pCDC->Ellipse(ptMiddle.x - nSize/2,ptMiddle.y - nSize/2,
					ptMiddle.x + nSize/2,ptMiddle.y + nSize/2);

				pCDC->SelectObject(pOldPen);
				pOldPen = pCDC->SelectObject(&mPen);
				ptTemp.x = ptMiddle.x + x;
				ptTemp.y = ptMiddle.y + y;
				pCDC->MoveTo(ptTemp);
				pCDC->LineTo(m_ptEnd);
			}
			else
			{
				pCDC->MoveTo(m_ptStart.x,m_ptStart.y);
				pCDC->LineTo(m_ptEnd.x,m_ptEnd.y);
			}

			pCDC->SelectObject(pOldPen);
			mPen.DeleteObject();
			mRPen.DeleteObject();
		}
		break;
	case 8: // 军用红断线 ----------
	case 41:// 兰断线--万吨
		{
			CPen mPen,*pOldPen;

			int nStyleCount = 2;
			DWORD StyleArray[2];
			StyleArray[0] = 5*scaleRate;
			StyleArray[1] = 5*scaleRate;
			LOGBRUSH logBrush;
			logBrush.lbStyle = BS_SOLID;
			logBrush.lbColor = clrActual;
			logBrush.lbHatch = 0;

			mPen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE, nLineWidth, &logBrush, nStyleCount, StyleArray);
			pOldPen = pCDC->SelectObject(&mPen);

			pCDC->MoveTo(m_ptStart.x,m_ptStart.y);
			pCDC->LineTo(m_ptEnd.x,m_ptEnd.y);

			pCDC->SelectObject (pOldPen);
			mPen.DeleteObject();
		}
		break;
	case 2:
	case 9:
	case 10:// 线加方框 --□--□--
		{
			CPen mPen,*pOldPen,mRPen;
			COLORREF m_clrLine,m_clrVert;

			m_clrLine = clrActual;
			m_clrVert = color.vert;

			if(line_type == 9)
			{
				int nStyleCount = 2;
				DWORD StyleArray[2];
				StyleArray[0] = 5*scaleRate;
				StyleArray[1] = 5*scaleRate;
				LOGBRUSH logBrush;
				logBrush.lbStyle = BS_SOLID;
				logBrush.lbColor = m_clrVert;
				logBrush.lbHatch = 0;

				mPen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE, nLineWidth, &logBrush, nStyleCount, StyleArray);
			}
			else
			{
				mPen.CreatePen(PS_SOLID, nLineWidth, m_clrLine);
			}

			mRPen.CreatePen(PS_SOLID, nLineWidth, m_clrVert);
			pOldPen = pCDC->SelectObject(&mPen);

			if(bAbsLine )
			{
				//middle rect size is 4 * 4 pixel
				CPoint ptMiddle,ptRect[4],ptTemp;
				int x,y;
				int nSize = 4*scaleRate;
				double fDelta;

				if(TRUE)
				{
					fDelta = (double)(m_ptEnd.y - m_ptStart.y) / (double)(m_ptEnd.x - m_ptStart.x);
					ptRect[0].y = (int)(nSize * (fDelta + 1) / sqrt(fDelta * fDelta + 1));
					ptRect[0].x = (int)(nSize * (1 - fDelta) / sqrt(fDelta * fDelta + 1));

					ptRect[1].y = (int)(nSize * (1 - fDelta) / sqrt(fDelta * fDelta + 1));
					ptRect[1].x = (int)((-nSize) * (1 + fDelta) / sqrt(fDelta * fDelta + 1));

					ptRect[2].y = (int)((-nSize) * (fDelta + 1) / sqrt(fDelta * fDelta + 1));
					ptRect[2].x = (int)((-nSize) * (1 - fDelta) / sqrt(fDelta * fDelta + 1));

					ptRect[3].y = (int)(nSize * (fDelta - 1) / sqrt(fDelta * fDelta + 1));
					ptRect[3].x = (int)(nSize * (1 + fDelta) / sqrt(fDelta * fDelta + 1));
					y = (int)(2 * nSize * fDelta / sqrt(fDelta * fDelta + 1));
					x = (int)(2 * nSize / sqrt(fDelta * fDelta + 1));
				} 
				else 
				{
					fDelta = (double)(m_ptStart.x - m_ptEnd.x) / (double)(m_ptStart.y - m_ptEnd.y);
					ptRect[0].y = (int)(nSize * (fDelta + 1) / sqrt(fDelta * fDelta + 1));
					ptRect[0].x = (int)(nSize * (fDelta - 1) / sqrt(fDelta * fDelta + 1));

					ptRect[1].y = (int)(nSize * (fDelta - 1) / sqrt(fDelta * fDelta + 1));
					ptRect[1].x = (int)((-nSize) * (1 + fDelta) / sqrt(fDelta * fDelta + 1));

					ptRect[2].y = (int)((-nSize) * (fDelta + 1) / sqrt(fDelta * fDelta + 1));
					ptRect[2].x = (int)((-nSize) * (fDelta - 1) / sqrt(fDelta * fDelta + 1));

					ptRect[3].y = (int)(nSize * (1 - fDelta) / sqrt(fDelta * fDelta + 1));
					ptRect[3].x = (int)(nSize * (1 + fDelta) / sqrt(fDelta * fDelta + 1));
					y = (int)(2 * nSize / sqrt(fDelta * fDelta + 1));
					x = (int)(2 * nSize * fDelta  / sqrt(fDelta * fDelta + 1));
				} 

				// draw start line
				pCDC->MoveTo(m_ptStart);
				ptMiddle.x = m_ptStart.x /2 + m_ptEnd.x / 2;
				ptMiddle.y = m_ptStart.y /2 + m_ptEnd.y / 2;
				ptTemp.x = ptMiddle.x - x;
				ptTemp.y = ptMiddle.y - y;
				pCDC->LineTo(ptTemp);
				pCDC->SelectObject(pOldPen);
				pOldPen = pCDC->SelectObject(&mRPen);
				pCDC->MoveTo(ptRect[0].x + ptMiddle.x,ptRect[0].y + ptMiddle.y);
				pCDC->LineTo(ptRect[1].x + ptMiddle.x,ptRect[1].y + ptMiddle.y);
				pCDC->LineTo(ptRect[2].x + ptMiddle.x,ptRect[2].y + ptMiddle.y);
				pCDC->LineTo(ptRect[3].x + ptMiddle.x,ptRect[3].y + ptMiddle.y);
				pCDC->LineTo(ptRect[0].x + ptMiddle.x,ptRect[0].y + ptMiddle.y);

				pCDC->SelectObject(pOldPen);
				pOldPen = pCDC->SelectObject(&mPen);
				ptTemp.x = ptMiddle.x + x;
				ptTemp.y = ptMiddle.y + y;
				pCDC->MoveTo(ptTemp);
				pCDC->LineTo(m_ptEnd);
			}
			else
			{
				pCDC->MoveTo(m_ptStart.x,m_ptStart.y);
				pCDC->LineTo(m_ptEnd.x,m_ptEnd.y);
			}
			pCDC->SelectObject(pOldPen);	
			mPen.DeleteObject();
			mRPen.DeleteObject();
		}
		break;
	case 11:// －＋－｜－
		{
			CPen mPen,*pOldPen,mRPen;
			COLORREF m_clrLine,m_clrVert;
			m_clrLine = RGB(0,0,0);
			m_clrVert = RGB(0,0,0);

			mPen.CreatePen(PS_SOLID, nLineWidth, m_clrLine);
			mRPen.CreatePen(PS_SOLID, nLineWidth, m_clrVert);
			pOldPen = pCDC->SelectObject(&mPen);

			if(bAbsLine)
			{
				//middle rect size is 4 * 4 pixel
				CPoint ptMiddle,ptRect[4],ptTemp;
				int x,y,nSize = 6*scaleRate,nSize2 = 8*scaleRate;
				double fDelta;

				if(TRUE)
				{
					fDelta = (double)(m_ptEnd.y - m_ptStart.y) / (double)(m_ptEnd.x - m_ptStart.x);
					ptRect[0].y = (int)(nSize * fDelta / sqrt(fDelta * fDelta + 1));
					ptRect[0].x = (int)(nSize / sqrt(fDelta * fDelta + 1));

					ptRect[1].y = (int)(nSize / sqrt(fDelta * fDelta + 1));
					ptRect[1].x = (int)((-nSize) * fDelta / sqrt(fDelta * fDelta + 1));

					ptRect[2].y = (int)((-nSize) * fDelta / sqrt(fDelta * fDelta + 1));
					ptRect[2].x = (int)((-nSize) / sqrt(fDelta * fDelta + 1));

					ptRect[3].y = (int)((-nSize) / sqrt(fDelta * fDelta + 1));
					ptRect[3].x = (int)(nSize * fDelta / sqrt(fDelta * fDelta + 1));
					y = (int)(nSize2 * fDelta / sqrt(fDelta * fDelta + 1));
					x = (int)(nSize2 / sqrt(fDelta * fDelta + 1));
				} 
				else 
				{
					fDelta = (double)(m_ptStart.x - m_ptEnd.x) / (double)(m_ptStart.y - m_ptEnd.y);

					ptRect[0].y = (int)(nSize / sqrt(fDelta * fDelta + 1));
					ptRect[0].x = (int)(nSize * fDelta / sqrt(fDelta * fDelta + 1));

					ptRect[1].y = (int)(nSize * fDelta / sqrt(fDelta * fDelta + 1));
					ptRect[1].x = (int)((-nSize) / sqrt(fDelta * fDelta + 1));

					ptRect[2].y = (int)((-nSize) / sqrt(fDelta * fDelta + 1));
					ptRect[2].x = (int)((-nSize) * fDelta / sqrt(fDelta * fDelta + 1));

					ptRect[3].y = (int)((-nSize) * fDelta / sqrt(fDelta * fDelta + 1));
					ptRect[3].x = (int)(nSize / sqrt(fDelta * fDelta + 1));

					y = (int)(nSize2 / sqrt(fDelta * fDelta + 1));
					x = (int)(nSize2 * fDelta  / sqrt(fDelta * fDelta + 1));
				} 

				// draw start line
				pCDC->MoveTo(m_ptStart);
				ptMiddle.x = m_ptStart.x *2/3 + m_ptEnd.x / 3;
				ptMiddle.y = m_ptStart.y *2/3 + m_ptEnd.y / 3;
				ptTemp.x = ptMiddle.x - x;
				ptTemp.y = ptMiddle.y - y;
				pCDC->LineTo(ptTemp);

				pCDC->SelectObject(pOldPen);
				pOldPen = pCDC->SelectObject(&mRPen);
				pCDC->MoveTo(ptRect[0].x + ptMiddle.x,ptRect[0].y + ptMiddle.y);
				pCDC->LineTo(ptRect[2].x + ptMiddle.x,ptRect[2].y + ptMiddle.y);
				pCDC->MoveTo(ptRect[1].x + ptMiddle.x,ptRect[1].y + ptMiddle.y);
				pCDC->LineTo(ptRect[3].x + ptMiddle.x,ptRect[3].y + ptMiddle.y);
				// draw middle line
				pCDC->SelectObject(pOldPen);
				pOldPen = pCDC->SelectObject(&mPen);
				ptTemp.x = ptMiddle.x + x;
				ptTemp.y = ptMiddle.y + y;
				pCDC->MoveTo(ptTemp);
				ptMiddle.x = m_ptStart.x / 3 + m_ptEnd.x * 2 / 3;
				ptMiddle.y = m_ptStart.y / 3 + m_ptEnd.y * 2 / 3;
				ptTemp.x = ptMiddle.x - x;
				ptTemp.y = ptMiddle.y - y;
				pCDC->LineTo(ptTemp);

				pCDC->SelectObject(pOldPen);
				pOldPen = pCDC->SelectObject(&mRPen);
				pCDC->MoveTo(ptRect[0].x + ptMiddle.x,ptRect[0].y + ptMiddle.y);
				pCDC->LineTo(ptRect[2].x + ptMiddle.x,ptRect[2].y + ptMiddle.y);
				//draw end line
				pCDC->SelectObject(pOldPen);
				pOldPen = pCDC->SelectObject(&mPen);
				ptTemp.x = ptMiddle.x + x;
				ptTemp.y = ptMiddle.y + y;
				pCDC->MoveTo(ptTemp);
				pCDC->LineTo(m_ptEnd);
			}
			else
			{
				pCDC->MoveTo(m_ptStart.x,m_ptStart.y);
				pCDC->LineTo(m_ptEnd.x,m_ptEnd.y);
			}
			pCDC->SelectObject(pOldPen);
			mPen.DeleteObject();
			mRPen.DeleteObject();
		}
		break;
	case 13://单机类别
		{
			CPen mPen,*pOldPen,mRPen;
			
			mPen.CreatePen(PS_SOLID, nLineWidth, clrActual);
			mRPen.CreatePen(PS_SOLID, nLineWidth, color.vert);
			pOldPen = pCDC->SelectObject(&mPen);

			if(bAbsLine)
			{
				if(nEngine == 0)
				{
					//middle rect size is 4 * 4 pixel
					CPoint ptMiddle,ptRect[3],ptTemp;
					int x,y;
					double nSize = 6.5*scaleRate,nSize2 = 12*scaleRate;
					double fDelta,fThreeSqrt = sqrt((double)3);
					// compute 8 * 8 triangle after rotate some angle
					nSize = 4*scaleRate;

					if(TRUE)
					{
						fDelta = (double)(m_ptStart.y - m_ptEnd.y) / (double)(m_ptStart.x - m_ptEnd.x);
						ptRect[0].y = (int)(2 * nSize * fDelta / sqrt(fDelta * fDelta + 1));
						ptRect[0].x = (int)(2 * nSize / sqrt(fDelta * fDelta + 1));

						ptRect[1].y = (int)(nSize * (fThreeSqrt - fDelta) / sqrt(fDelta * fDelta + 1));
						ptRect[1].x = (int)((-nSize) * (1 + fThreeSqrt * fDelta) / sqrt(fDelta * fDelta + 1));

						ptRect[2].y = (int)((-nSize) * (fDelta + fThreeSqrt) / sqrt(fDelta * fDelta + 1));
						ptRect[2].x = (int)((-nSize) * (1 - fThreeSqrt * fDelta) / sqrt(fDelta * fDelta + 1));

						y = (int)(nSize2 * fDelta / sqrt(fDelta * fDelta + 1));//fDelta *
						x = (int)(nSize2 / sqrt(fDelta * fDelta + 1));
					} 
					else 
					{
						fDelta = (double)(m_ptStart.x - m_ptEnd.x) / (double)(m_ptStart.y - m_ptEnd.y);
						ptRect[0].y = (int)(2 * nSize / sqrt(fDelta * fDelta + 1));
						ptRect[0].x = (int)(2 * nSize * fDelta / sqrt(fDelta * fDelta + 1));

						ptRect[1].y = (int)(nSize * (fThreeSqrt * fDelta - 1) / sqrt(fDelta * fDelta + 1));
						ptRect[1].x = (int)((-nSize) * (fDelta + fThreeSqrt) / sqrt(fDelta * fDelta + 1));

						ptRect[2].y = (int)((-nSize) * (1 + fDelta * fThreeSqrt) / sqrt(fDelta * fDelta + 1));
						ptRect[2].x = (int)((-nSize) * (fDelta - fThreeSqrt) / sqrt(fDelta * fDelta + 1));

						y = (int)(nSize2 / sqrt(fDelta * fDelta + 1));
						x = (int)(nSize2 * fDelta  / sqrt(fDelta * fDelta + 1));
					} 

					// draw start line
					pCDC->MoveTo(m_ptStart);
					ptMiddle.x = m_ptStart.x /2 + m_ptEnd.x /2;
					ptMiddle.y = m_ptStart.y /2 + m_ptEnd.y /2;
					if(m_ptStart.y > m_ptEnd.y && m_ptStart.x > m_ptEnd.x)
					{
						ptTemp.x = ptMiddle.x + x;
						ptTemp.y = ptMiddle.y + y;
					}
					else
					{
						ptTemp.x = ptMiddle.x - x;
						ptTemp.y = ptMiddle.y - y;
					}
					pCDC->LineTo(ptTemp);

					pCDC->SelectObject(pOldPen);
					pOldPen = pCDC->SelectObject(&mRPen);
					pCDC->MoveTo(ptRect[0].x + ptMiddle.x,ptRect[0].y + ptMiddle.y);
					pCDC->LineTo(ptRect[1].x + ptMiddle.x,ptRect[1].y + ptMiddle.y);
					pCDC->LineTo(ptRect[2].x + ptMiddle.x,ptRect[2].y + ptMiddle.y);
					pCDC->LineTo(ptRect[0].x + ptMiddle.x,ptRect[0].y + ptMiddle.y);


					pCDC->SelectObject(pOldPen);
					pOldPen = pCDC->SelectObject(&mPen);
					if(m_ptStart.y > m_ptEnd.y && m_ptStart.x > m_ptEnd.x)
					{
						ptTemp.x = ptMiddle.x - x;
						ptTemp.y = ptMiddle.y - y;
					}
					else
					{
						ptTemp.x = ptMiddle.x + x;
						ptTemp.y = ptMiddle.y + y;
					}
					pCDC->MoveTo(ptTemp);
					pCDC->LineTo(m_ptEnd);
					if(nCarriageCount >=1&&nCarriageCount <=10)
					{
						CFont * pOldFont;
						COLORREF  pOldColor;
						CFont font;
						LOGFONT stFont;
						UINT pOldAlign;

						memset(&stFont,0,sizeof(stFont));
						stFont.lfHeight = MulDiv(9,-pCDC->GetDeviceCaps(LOGPIXELSY),72);
						stFont.lfWeight = FW_NORMAL;
						stFont.lfClipPrecision = CLIP_LH_ANGLES;
						strcpy(stFont.lfFaceName,"Times New Roman");

						stFont.lfEscapement=(fDelta==0?0:10*atan(1/fDelta)*180/3.14);
						if(m_ptStart.y > m_ptEnd.y)
							pOldAlign=pCDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
						else
							pOldAlign=pCDC->SetTextAlign(TA_CENTER|TA_TOP);

						font.CreateFontIndirect(&stFont);
						pOldFont = pCDC->SelectObject(&font);
						CString tempstr;
						tempstr.Format("%d",nCarriageCount);
						
						pOldColor=pCDC->SetTextColor(COLORREF(RGB(0,0,0)));
						
						pCDC->SetTextAlign(pOldAlign);
						pCDC->SetTextColor(pOldColor);
						pCDC->SelectObject(pOldFont);
						font.DeleteObject();
					}
				}
				else
				{//挂机车的单车
					//middle rect size is 4 * 4 pixel
					CPoint ptMiddle,ptRect[3],ptTemp;
					int x,y;
					double nSize = 6.5*scaleRate,nSize2 = 12*scaleRate;
					double fDelta,fThreeSqrt = sqrt((double)3);
					nSize = 4*scaleRate;

					if(m_ptStart.y > m_ptEnd.y )
					{
						fDelta = (double)(m_ptStart.y - m_ptEnd.y) / (double)(m_ptStart.x - m_ptEnd.x);
						ptRect[0].y = (int)(2 * nSize * fDelta / sqrt(fDelta * fDelta + 1));
						ptRect[0].x = (int)(2 * nSize / sqrt(fDelta * fDelta + 1));

						ptRect[1].y = (int)(nSize * (fThreeSqrt - fDelta) / sqrt(fDelta * fDelta + 1));
						ptRect[1].x = (int)((-nSize) * (1 + fThreeSqrt * fDelta) / sqrt(fDelta * fDelta + 1));

						ptRect[2].y = (int)((-nSize) * (fDelta + fThreeSqrt) / sqrt(fDelta * fDelta + 1));
						ptRect[2].x = (int)((-nSize) * (1 - fThreeSqrt * fDelta) / sqrt(fDelta * fDelta + 1));

						y = (int)(nSize2 * fDelta / sqrt(fDelta * fDelta + 1));//fDelta *
						x = (int)(nSize2 / sqrt(fDelta * fDelta + 1));
					} 
					else 
					{
						fDelta = (double)(m_ptStart.x - m_ptEnd.x) / (double)(m_ptStart.y - m_ptEnd.y);
						ptRect[0].y = (int)(2 * nSize / sqrt(fDelta * fDelta + 1));
						ptRect[0].x = (int)(2 * nSize * fDelta / sqrt(fDelta * fDelta + 1));

						ptRect[1].y = (int)(nSize * (fThreeSqrt * fDelta - 1) / sqrt(fDelta * fDelta + 1));
						ptRect[1].x = (int)((-nSize) * (fDelta + fThreeSqrt) / sqrt(fDelta * fDelta + 1));

						ptRect[2].y = (int)((-nSize) * (1 + fDelta * fThreeSqrt) / sqrt(fDelta * fDelta + 1));
						ptRect[2].x = (int)((-nSize) * (fDelta - fThreeSqrt) / sqrt(fDelta * fDelta + 1));

						y = (int)(nSize2 / sqrt(fDelta * fDelta + 1));
						x = (int)(nSize2 * fDelta  / sqrt(fDelta * fDelta + 1));
					} 

					// draw start line
					pCDC->MoveTo(m_ptStart);
					ptMiddle.x = m_ptStart.x * 2/ 3 + m_ptEnd.x / 3;
					ptMiddle.y = m_ptStart.y * 2/ 3 + m_ptEnd.y / 3;
					if(m_ptStart.y > m_ptEnd.y && m_ptStart.x > m_ptEnd.x)
					{
						ptTemp.x = ptMiddle.x + x;
						ptTemp.y = ptMiddle.y + y;
					}
					else
					{
						ptTemp.x = ptMiddle.x - x;
						ptTemp.y = ptMiddle.y - y;
					}
					pCDC->LineTo(ptTemp);

					pCDC->SelectObject(pOldPen);
					pOldPen = pCDC->SelectObject(&mRPen);
					pCDC->MoveTo(ptRect[0].x + ptMiddle.x,ptRect[0].y + ptMiddle.y);
					pCDC->LineTo(ptRect[1].x + ptMiddle.x,ptRect[1].y + ptMiddle.y);
					pCDC->LineTo(ptRect[2].x + ptMiddle.x,ptRect[2].y + ptMiddle.y);
					pCDC->LineTo(ptRect[0].x + ptMiddle.x,ptRect[0].y + ptMiddle.y);

					ptMiddle.x = ptMiddle.x + ptRect[0].x;
					ptMiddle.y = ptMiddle.y + ptRect[0].y;
					if(m_ptStart.y > m_ptEnd.y && m_ptStart.x > m_ptEnd.x)
					{
						ptMiddle.x = ptMiddle.x - x;
						ptMiddle.y = ptMiddle.y - y;
					}
					else
					{
						ptMiddle.x = ptMiddle.x + x;
						ptMiddle.y = ptMiddle.y + y;
					}


					pCDC->SelectObject(pOldPen);
					pOldPen = pCDC->SelectObject(&mRPen);
					pCDC->MoveTo(ptRect[0].x + ptMiddle.x,ptRect[0].y + ptMiddle.y);
					pCDC->LineTo(ptRect[1].x + ptMiddle.x,ptRect[1].y + ptMiddle.y);
					pCDC->LineTo(ptRect[2].x + ptMiddle.x,ptRect[2].y + ptMiddle.y);
					pCDC->LineTo(ptRect[0].x + ptMiddle.x,ptRect[0].y + ptMiddle.y);

					//draw end line
					pCDC->SelectObject(pOldPen);
					pOldPen = pCDC->SelectObject(&mPen);
					if(m_ptStart.y > m_ptEnd.y && m_ptStart.x > m_ptEnd.x)
					{
						ptTemp.x = ptMiddle.x - x;
						ptTemp.y = ptMiddle.y - y;
					}
					else
					{
						ptTemp.x = ptMiddle.x + x;
						ptTemp.y = ptMiddle.y + y;
					}
					pCDC->MoveTo(ptTemp);
					pCDC->LineTo(m_ptEnd);
				}
			}
			else
			{
				pCDC->MoveTo(m_ptStart.x,m_ptStart.y);
				pCDC->LineTo(m_ptEnd.x,m_ptEnd.y);
			}
			pCDC->SelectObject(pOldPen);

			mPen.DeleteObject();
			mRPen.DeleteObject();
		}
		break;
	case 14:
		{
			CPen mPen,*pOldPen,mRPen;
			COLORREF m_clrLine,m_clrTriangle;
			m_clrLine = RGB(255,0,0);
			m_clrTriangle = RGB(255,0,0);

			mPen.CreatePen(PS_SOLID, nLineWidth, clrActual);
			mRPen.CreatePen(PS_SOLID, nLineWidth, color.vert);
			pOldPen = pCDC->SelectObject(&mPen);

			if(bAbsLine)
			{
				//middle rect size is 4 * 4 pixel
				CPoint ptMiddle,ptRect[3],ptTemp;
				int x,y;
				double nSize = 6.5*scaleRate,nSize2 = 12*scaleRate;
				double fDelta,fThreeSqrt = sqrt((double)3);
				// compute 8 * 8 triangle after rotate some angle
				nSize = 3*scaleRate;

				if(TRUE)
				{
					fDelta = (double)(m_ptStart.y - m_ptEnd.y) / (double)(m_ptStart.x - m_ptEnd.x);
					ptRect[0].y = (int)(2 * nSize * fDelta / sqrt(fDelta * fDelta + 1));
					ptRect[0].x = (int)(2 * nSize / sqrt(fDelta * fDelta + 1));

					ptRect[1].y = (int)(nSize * (fThreeSqrt - fDelta) / sqrt(fDelta * fDelta + 1));
					ptRect[1].x = (int)((-nSize) * (1 + fThreeSqrt * fDelta) / sqrt(fDelta * fDelta + 1));

					ptRect[2].y = (int)((-nSize) * (fDelta + fThreeSqrt) / sqrt(fDelta * fDelta + 1));
					ptRect[2].x = (int)((-nSize) * (1 - fThreeSqrt * fDelta) / sqrt(fDelta * fDelta + 1));

					y = (int)(nSize2 * fDelta / sqrt(fDelta * fDelta + 1));//fDelta *
					x = (int)(nSize2 / sqrt(fDelta * fDelta + 1));
				} 
				else 
				{
					fDelta = (double)(m_ptStart.x - m_ptEnd.x) / (double)(m_ptStart.y - m_ptEnd.y);
					ptRect[0].y = (int)(2 * nSize / sqrt(fDelta * fDelta + 1));
					ptRect[0].x = (int)(2 * nSize * fDelta / sqrt(fDelta * fDelta + 1));

					ptRect[1].y = (int)(nSize * (fThreeSqrt * fDelta - 1) / sqrt(fDelta * fDelta + 1));
					ptRect[1].x = (int)((-nSize) * (fDelta + fThreeSqrt) / sqrt(fDelta * fDelta + 1));

					ptRect[2].y = (int)((-nSize) * (1 + fDelta * fThreeSqrt) / sqrt(fDelta * fDelta + 1));
					ptRect[2].x = (int)((-nSize) * (fDelta - fThreeSqrt) / sqrt(fDelta * fDelta + 1));

					y = (int)(nSize2 / sqrt(fDelta * fDelta + 1));
					x = (int)(nSize2 * fDelta  / sqrt(fDelta * fDelta + 1));
				} 

				// draw start line
				pCDC->MoveTo(m_ptStart);
				ptMiddle.x = m_ptStart.x /2 + m_ptEnd.x /2;
				ptMiddle.y = m_ptStart.y /2 + m_ptEnd.y /2;
				ptTemp.x = ptMiddle.x - x;
				ptTemp.y = ptMiddle.y - y;
				pCDC->LineTo(ptTemp);

				pCDC->SelectObject(pOldPen);
				pOldPen = pCDC->SelectObject(&mRPen);
				pCDC->MoveTo(ptRect[0].x + ptMiddle.x,ptRect[0].y + ptMiddle.y);
				pCDC->LineTo(ptRect[1].x + ptMiddle.x,ptRect[1].y + ptMiddle.y);
				pCDC->MoveTo(ptRect[2].x + ptMiddle.x,ptRect[2].y + ptMiddle.y);
				pCDC->LineTo(ptRect[0].x + ptMiddle.x,ptRect[0].y + ptMiddle.y);

				pCDC->SelectObject(pOldPen);
				pOldPen = pCDC->SelectObject(&mPen);
				ptTemp.x = ptMiddle.x + x;
				ptTemp.y = ptMiddle.y + y;
				pCDC->MoveTo(ptTemp);
				pCDC->LineTo(m_ptEnd);
				//add 
				pCDC->MoveTo(m_ptStart);
				pCDC->LineTo(m_ptEnd);
			}
			else
			{
				pCDC->MoveTo(m_ptStart.x,m_ptStart.y);
				pCDC->LineTo(m_ptEnd.x,m_ptEnd.y);
			}

			pCDC->SelectObject(pOldPen);
			mPen.DeleteObject();
			mRPen.DeleteObject();
		}
		break;
	case 15:// —×—×—
		{
			CPen mPen,*pOldPen,mRPen;
			COLORREF m_clrLine ;
			m_clrLine = RGB(255,0,0);

			mPen.CreatePen(PS_SOLID, nLineWidth, clrActual);
			mRPen.CreatePen(PS_SOLID, nLineWidth, color.vert);
			pOldPen = pCDC->SelectObject(&mPen);
			
			if(bAbsLine)
			{
				//middle rect size is 4 * 4 pixel
				CPoint ptMiddle,ptRect[4],ptTemp;
				int x,y;
				double nSize = 5*scaleRate,nSize2 = 8*scaleRate;
				double fDelta;

				if(TRUE)
				{
					fDelta = (double)(m_ptEnd.y - m_ptStart.y) / (double)(m_ptEnd.x - m_ptStart.x);
					ptRect[0].y = (int)(nSize * (fDelta + 1) / sqrt(fDelta * fDelta + 1));
					ptRect[0].x = (int)(nSize * (1 - fDelta) / sqrt(fDelta * fDelta + 1));

					ptRect[1].y = (int)(nSize * (1 - fDelta) / sqrt(fDelta * fDelta + 1));
					ptRect[1].x = (int)((-nSize) * (1 + fDelta) / sqrt(fDelta * fDelta + 1));

					ptRect[2].y = (int)((-nSize) * (fDelta + 1) / sqrt(fDelta * fDelta + 1));
					ptRect[2].x = (int)((-nSize) * (1 - fDelta) / sqrt(fDelta * fDelta + 1));

					ptRect[3].y = (int)(nSize * (fDelta - 1) / sqrt(fDelta * fDelta + 1));
					ptRect[3].x = (int)(nSize * (1 + fDelta) / sqrt(fDelta * fDelta + 1));
					y = (int)(nSize2 * fDelta / sqrt(fDelta * fDelta + 1));
					x = (int)(nSize2 / sqrt(fDelta * fDelta + 1));
				} 
				else 
				{
					fDelta = (double)(m_ptStart.x - m_ptEnd.x) / (double)(m_ptStart.y - m_ptEnd.y);
					ptRect[0].y = (int)(nSize * (fDelta + 1) / sqrt(fDelta * fDelta + 1));
					ptRect[0].x = (int)(nSize * (fDelta - 1) / sqrt(fDelta * fDelta + 1));

					ptRect[1].y = (int)(nSize * (fDelta - 1) / sqrt(fDelta * fDelta + 1));
					ptRect[1].x = (int)((-nSize) * (1 + fDelta) / sqrt(fDelta * fDelta + 1));

					ptRect[2].y = (int)((-nSize) * (fDelta + 1) / sqrt(fDelta * fDelta + 1));
					ptRect[2].x = (int)((-nSize) * (fDelta - 1) / sqrt(fDelta * fDelta + 1));

					ptRect[3].y = (int)(nSize * (1 - fDelta) / sqrt(fDelta * fDelta + 1));
					ptRect[3].x = (int)(nSize * (1 + fDelta) / sqrt(fDelta * fDelta + 1));
					y = (int)(nSize2 / sqrt(fDelta * fDelta + 1));
					x = (int)(nSize2 * fDelta  / sqrt(fDelta * fDelta + 1));
				} 

				// draw start line
				pCDC->MoveTo(m_ptStart);
				ptMiddle.x = m_ptStart.x /2 + m_ptEnd.x / 2;
				ptMiddle.y = m_ptStart.y /2 + m_ptEnd.y / 2;
				ptTemp.x = ptMiddle.x - x;
				ptTemp.y = ptMiddle.y - y;
				pCDC->LineTo(ptTemp);

				pCDC->SelectObject(pOldPen);
				pOldPen = pCDC->SelectObject(&mRPen);
				pCDC->MoveTo(ptRect[0].x + ptMiddle.x,ptRect[0].y + ptMiddle.y);
				pCDC->LineTo(ptRect[2].x + ptMiddle.x,ptRect[2].y + ptMiddle.y);
				pCDC->MoveTo(ptRect[1].x + ptMiddle.x,ptRect[1].y + ptMiddle.y);
				pCDC->LineTo(ptRect[3].x + ptMiddle.x,ptRect[3].y + ptMiddle.y);

				pCDC->SelectObject(pOldPen);
				pOldPen = pCDC->SelectObject(&mPen);
				ptTemp.x = ptMiddle.x + x;
				ptTemp.y = ptMiddle.y + y;
				pCDC->MoveTo(ptTemp);
				pCDC->LineTo(m_ptEnd);
			}
			else
			{
				pCDC->MoveTo(m_ptStart.x,m_ptStart.y);
				pCDC->LineTo(m_ptEnd.x,m_ptEnd.y);
			}

			pCDC->SelectObject(pOldPen); 
			mPen.DeleteObject();
			mRPen.DeleteObject();
		}
		break;
	case 42:// 兰断线--万吨
	case 43:// 兰断线--万吨
		{
			int nStyleCount = 2;
			DWORD StyleArray[2];
			if(42 == line_type)
			{
				StyleArray[0] = 15;
				StyleArray[1] = 6;
			}
			else
			{
				StyleArray[0] = 36;
				StyleArray[1] = 6;
				nLineWidth+=1;
			}
			LOGBRUSH logBrush;
			logBrush.lbStyle = BS_SOLID;
			logBrush.lbColor = clrActual;
			logBrush.lbHatch = 0;

			CPen mPen,*pOldPen;
			mPen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE, nLineWidth,&logBrush,nStyleCount,StyleArray);

			pOldPen = pCDC->SelectObject(&mPen);

			pCDC->MoveTo(m_ptStart.x,m_ptStart.y);
			pCDC->LineTo(m_ptEnd.x,m_ptEnd.y);

			pCDC->SelectObject (pOldPen);
			mPen.DeleteObject();
		}
		break;
	case 44:// 双单机
		{
			CPen mPen,*pOldPen,mRPen;

			mPen.CreatePen(PS_SOLID,nLineWidth,clrActual);
			mRPen.CreatePen(PS_SOLID,nLineWidth,color.vert);

			pOldPen = pCDC->SelectObject(&mPen);

			if(bAbsLine)
			{
				//middle rect size is 4 * 4 pixel
				CPoint ptMiddle,ptRect[3],ptTemp;
				int x,y;
				double nSize = 6.5,nSize2 = 12;
				double fDelta,fThreeSqrt = sqrt((double)3);
				nSize = 4;

				if(m_ptStart.y > m_ptEnd.y )
				{
					fDelta = (double)(m_ptStart.y - m_ptEnd.y) / (double)(m_ptStart.x - m_ptEnd.x);
					ptRect[0].y = (int)(2 * nSize * fDelta / sqrt(fDelta * fDelta + 1));
					ptRect[0].x = (int)(2 * nSize / sqrt(fDelta * fDelta + 1));

					ptRect[1].y = (int)(nSize * (fThreeSqrt - fDelta) / sqrt(fDelta * fDelta + 1));
					ptRect[1].x = (int)((-nSize) * (1 + fThreeSqrt * fDelta) / sqrt(fDelta * fDelta + 1));

					ptRect[2].y = (int)((-nSize) * (fDelta + fThreeSqrt) / sqrt(fDelta * fDelta + 1));
					ptRect[2].x = (int)((-nSize) * (1 - fThreeSqrt * fDelta) / sqrt(fDelta * fDelta + 1));

					y = (int)(nSize2 * fDelta / sqrt(fDelta * fDelta + 1));//fDelta *
					x = (int)(nSize2 / sqrt(fDelta * fDelta + 1));
				} 
				else 
				{
					fDelta = (double)(m_ptStart.x - m_ptEnd.x) / (double)(m_ptStart.y - m_ptEnd.y);
					ptRect[0].y = (int)(2 * nSize / sqrt(fDelta * fDelta + 1));
					ptRect[0].x = (int)(2 * nSize * fDelta / sqrt(fDelta * fDelta + 1));

					ptRect[1].y = (int)(nSize * (fThreeSqrt * fDelta - 1) / sqrt(fDelta * fDelta + 1));
					ptRect[1].x = (int)((-nSize) * (fDelta + fThreeSqrt) / sqrt(fDelta * fDelta + 1));

					ptRect[2].y = (int)((-nSize) * (1 + fDelta * fThreeSqrt) / sqrt(fDelta * fDelta + 1));
					ptRect[2].x = (int)((-nSize) * (fDelta - fThreeSqrt) / sqrt(fDelta * fDelta + 1));

					y = (int)(nSize2 / sqrt(fDelta * fDelta + 1));
					x = (int)(nSize2 * fDelta  / sqrt(fDelta * fDelta + 1));
				} 

				// draw start line
				pCDC->MoveTo(m_ptStart);
				ptMiddle.x = m_ptStart.x * 2 / 3 + m_ptEnd.x / 3;
				ptMiddle.y = m_ptStart.y * 2 / 3 + m_ptEnd.y / 3;
				if(m_ptStart.y > m_ptEnd.y && m_ptStart.x > m_ptEnd.x)
				{
					ptTemp.x = ptMiddle.x + x;
					ptTemp.y = ptMiddle.y + y;
				}
				else
				{
					ptTemp.x = ptMiddle.x - x;
					ptTemp.y = ptMiddle.y - y;
				}
				pCDC->LineTo(ptTemp);

				pCDC->SelectObject(pOldPen);
				pOldPen = pCDC->SelectObject(&mRPen);
				pCDC->MoveTo(ptRect[0].x + ptMiddle.x,ptRect[0].y + ptMiddle.y);
				pCDC->LineTo(ptRect[1].x + ptMiddle.x,ptRect[1].y + ptMiddle.y);
				pCDC->LineTo(ptRect[2].x + ptMiddle.x,ptRect[2].y + ptMiddle.y);
				pCDC->LineTo(ptRect[0].x + ptMiddle.x,ptRect[0].y + ptMiddle.y);


				ptMiddle.x = ptMiddle.x + ptRect[0].x;
				ptMiddle.y = ptMiddle.y + ptRect[0].y;
				if(m_ptStart.y > m_ptEnd.y && m_ptStart.x > m_ptEnd.x)
				{
					ptMiddle.x = ptMiddle.x - x;
					ptMiddle.y = ptMiddle.y - y;
				}
				else
				{
					ptMiddle.x = ptMiddle.x + x;
					ptMiddle.y = ptMiddle.y + y;
				}


				pCDC->SelectObject(pOldPen);
				pOldPen = pCDC->SelectObject(&mRPen);
				pCDC->MoveTo(ptRect[0].x + ptMiddle.x,ptRect[0].y + ptMiddle.y);
				pCDC->LineTo(ptRect[1].x + ptMiddle.x,ptRect[1].y + ptMiddle.y);
				pCDC->LineTo(ptRect[2].x + ptMiddle.x,ptRect[2].y + ptMiddle.y);
				pCDC->LineTo(ptRect[0].x + ptMiddle.x,ptRect[0].y + ptMiddle.y);

				//draw end line
				pCDC->SelectObject(pOldPen);
				pOldPen = pCDC->SelectObject(&mPen);
				if(m_ptStart.y > m_ptEnd.y && m_ptStart.x > m_ptEnd.x)
				{
					ptTemp.x = ptMiddle.x - x;
					ptTemp.y = ptMiddle.y - y;
				}
				else
				{
					ptTemp.x = ptMiddle.x + x;
					ptTemp.y = ptMiddle.y + y;
				}
				pCDC->MoveTo(ptTemp);
				pCDC->LineTo(m_ptEnd);
			}
			else
			{
				pCDC->MoveTo(m_ptStart.x,m_ptStart.y);
				pCDC->LineTo(m_ptEnd.x,m_ptEnd.y);
			}
			pCDC->SelectObject(pOldPen);
			mPen.DeleteObject();
			mRPen.DeleteObject();
		}
		break;
	case 21://未知列车类型
	default:
		{
			COLORREF  m_clrUp = RGB(0,255,0);
			COLORREF  m_clrDown = RGB(100,0,0);

			CPen mPen,*pOldPen;

			mPen.CreatePen(PS_SOLID, nLineWidth, m_clrDown);
			pOldPen = pCDC->SelectObject(&mPen);
			pCDC->MoveTo(ptlStart);
			pCDC->LineTo(ptlEnd);

			pCDC->SelectObject(pOldPen);
			mPen.DeleteObject();

		}

		break;
	}
	if(bExist_Engine)
	{
		if(m_ptEnd.y != m_ptStart.y)
		{
			int minor;
			if(m_ptEnd.y >= m_ptStart.y)
			{
				minor = -5;
			}
			else
			{
				minor = 5;
			}

			COLORREF m_clrLine = RGB(0,0,0);
			CPen mMyPen,*pMyOldPen;
			int nStyleCount = 2;
			DWORD StyleArray[2];
			StyleArray[0] = 10* scaleRate;
			StyleArray[1] = 10* scaleRate;
			LOGBRUSH logBrush;
			logBrush.lbStyle = BS_SOLID;
			logBrush.lbColor = m_clrLine;
			logBrush.lbHatch = 0;
			mMyPen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE, (int)(1*scaleRate) ,&logBrush,nStyleCount,StyleArray);

			pMyOldPen = pCDC->SelectObject(&mMyPen);
			pCDC->MoveTo(m_ptStart.x +minor,m_ptStart.y);
			pCDC->LineTo(m_ptEnd.x +minor,m_ptEnd.y);
			pCDC->SelectObject (pMyOldPen);
			mMyPen.DeleteObject();
		}
	}
}

void CPlotDoc::DrawTrainText(CDC * pCDC,CTG_SingleTrain *single,COLORREF color,int schd_type)
{
	int i=0;
	CTG_TrainRecord *rec=NULL,*next_rec=NULL,*prev_rec=NULL;
	CPoint point;
	int minute_width=0;
	CString str="";
	int dirFlag=UP_DIRECT;
	int nRecNum = single->GetTrainRecordCount();
	if(nRecNum <= 0) return;
	int m_nLogPixelSy = pCDC->GetDeviceCaps(LOGPIXELSY);
	if(m_nLogPixelSy >300)
		m_nLogPixelSy = 400;
	// text attribute
	CFont mFont,*pOldFont;
	LOGFONT stFont;

	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(m_font.station_text,-m_nLogPixelSy,72);
	stFont.lfWeight = FW_THIN;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	strcpy(stFont.lfFaceName,"Arial");
	mFont.CreateFontIndirect(&stFont);	
	pOldFont = pCDC->SelectObject(&mFont);

	int nOldColor = pCDC->SetTextColor(color);
	int nOldBackMode = pCDC->SetBkMode(TRANSPARENT);
	int nOldAlign = pCDC->SetTextAlign(TA_LEFT | TA_TOP);

	int m_nDirectFlag_prefix = GetTrainRecDirect(single, 0);
	
	//station text
	for (i=0;i< single->GetTrainRecordCount();i++)
	{
		prev_rec = single->GetTrainRecord(i-1);
		rec = single->GetTrainRecord(i);
		next_rec = single->GetTrainRecord(i+1);
		if(rec == NULL) continue;
		if(!rec->GetStationText() || (strlen(rec->GetStationText()) == 0))
			continue;
		if(rec == NULL) continue;
		long time_arrival_rec,date_arrival_rec;
		time_arrival_rec = minute_time(rec->m_data.arrive,&date_arrival_rec);
		minute_width = GetMinuteWidthToGraphStartTime(time_arrival_rec/60,date_arrival_rec,schd_type);
		point.x = minute_width * m_fPixelPerMinute;

		if(point.x < 0 || point.x >m_fPixelPerMinute * m_nViewTimeRange*60)
			continue;

		str = CString(rec->GetStationText());
		//DIRECT
		if(nRecNum == 1)
			dirFlag = m_nDirectFlag_prefix;
		else 
		{
			if(i < nRecNum -1)
			{
				next_rec = single->GetTrainRecord(i + 1);
				if(GetStationVPos(single,rec,PRIMARY_TRACK,EXIT_POSITION) > GetStationVPos(single,next_rec,PRIMARY_TRACK,ENTRY_POSITION))
					dirFlag = UP_DIRECT;
				else
					dirFlag = DOWN_DIRECT;
			}
			else if(i > 0)
			{
				prev_rec = single->GetTrainRecord(i - 1);
				if(GetStationVPos(single,rec,PRIMARY_TRACK,EXIT_POSITION) > GetStationVPos(single,prev_rec,PRIMARY_TRACK,ENTRY_POSITION))
					dirFlag = DOWN_DIRECT;
				else
					dirFlag = UP_DIRECT;
			}
		}

		if(dirFlag == UP_DIRECT)
		{
			point.y = GetStationVPos(single,rec,PRIMARY_TRACK,ENTRY_POSITION,schd_type) ;
			point.Offset (x_offset_print,y_offset_print);
			pCDC->TextOut(point.x,point.y - 20*yMultiple,str);
		}
		else
		{
			point.y = GetStationVPos(single,rec,PRIMARY_TRACK,ENTRY_POSITION,schd_type) ;
			point.Offset (x_offset_print,y_offset_print);
			pCDC->TextOut(point.x,point.y,str);
		}
	}
	
	pCDC->SelectObject(pOldFont);
	pCDC->SetTextColor(nOldColor);
	pCDC->SetTextAlign(nOldAlign);
	pCDC->SetBkMode(nOldBackMode);
	mFont.DeleteObject();
}

void CPlotDoc::GetGraphStartTime(ulong nShiftId, int schd_type)
{

	long nShiftDate;
	int  nShiftNo;

	nShiftDate = (nShiftId/1000000 - 1990) * 380 +
		(nShiftId/10000-(nShiftId/1000000)*100 - 1) * 31 + 
		nShiftId/100 - (nShiftId/10000)*100  - 1 ;
	nShiftNo = nShiftId - nShiftId/100*100;

	switch(schd_type){
	case WORK_SCHEDULE:
	case SHIFT_SCHEDULE:
		if(nShiftNo == 2){
			m_sTGGraphStartTime.day = nShiftDate;
			m_sTGGraphStartTime.hour = 6;   //0
			m_sTGGraphStartTime.minute = 0;
		}else{
			m_sTGGraphStartTime.day = OsGetPrevDate(PMTG.ThreadId,nShiftDate);
			m_sTGGraphStartTime.hour = 18;  //12
			m_sTGGraphStartTime.minute = 0;
		}
		break;
	case HISTORY_SCHEDULE:
		if(nShiftNo == 2){

			m_sTGGraphStartTime.day = nShiftDate;
			m_sTGGraphStartTime.hour = 6;  //0
			m_sTGGraphStartTime.minute = 0;

			m_sTGHistGraphStartTime.day = nShiftDate;
			m_sTGHistGraphStartTime.hour = 6;   //0
			m_sTGHistGraphStartTime.minute = 0;
		}else{
			m_sTGGraphStartTime.day = OsGetPrevDate(PMTG.ThreadId,nShiftDate);
			m_sTGGraphStartTime.hour = 18;  //12
			m_sTGGraphStartTime.minute = 0;

			m_sTGHistGraphStartTime.day = OsGetPrevDate(PMTG.ThreadId,nShiftDate);
			m_sTGHistGraphStartTime.hour = 18;  //12
			m_sTGHistGraphStartTime.minute = 0;
		}
		break;
	case BASIC_SCHEDULE:
		m_sTGGraphStartTime.day = nShiftDate;
		m_sTGGraphStartTime.hour =  0;//暂时简单赋值 m_pDoc->m_sTGBasicGraphStartTime.hour;   //0
		m_sTGGraphStartTime.minute = 0;
		break;
	default:
		break;
	}
}

int CPlotDoc::GetStationVPosByIndex(CTG_SingleTrain *single,CTG_TrainRecord* rec,int usSideFlag,int usFlag)
{
	if(NULL == rec)
		return -1;
	// m_sDispDistance
	int i = 0;
	if(EXIT_POSITION == usFlag)
	{
		i = rec->GetDepartTGIndex();
		if(0 > i || i >= this->m_nDispDistanceNum)
		{
			i = rec->GetArriveTGIndex();
		}
	}
	else
	{
		i = rec->GetArriveTGIndex();
		if(0 > i || i >= this->m_nDispDistanceNum)
		{
			i = rec->GetDepartTGIndex();
		}
	}

	if(0 > i || i >= this->m_nDispDistanceNum)
	{
		if(GetNodeIndex(single,rec,i,(ushort)usFlag) < 0)
			return -1;
	}

	if(0 > i || i >= this->m_nDispDistanceNum)
		return -1;

	if(m_sDispDistance[i].distance.node_info.show_sides)
	{
		switch(usSideFlag)
		{
		case TOP_TRACK:
			return m_sDispDistance[i].nStationYPos;
			break;
		case PRIMARY_TRACK:
			{
				for(int k = 1;k <= m_sDispDistance[i].distance.number_of_side;k++)
				{
					if(rec->m_data.arrive_side == m_sDispDistance[i].distance.side[k].side_number )
						break;
				}
				if(k > m_sDispDistance[i].distance.number_of_side)
				{
					int DirectFlag = JudeTrainDirectByRecord(single, rec,ENTRY_POSITION);
					if(IsFlagStart(rec->m_data.flag)  &&  GetModeOfStationShowSide(i) ==1 )
					{
						if(DirectFlag ==DOWN_DIRECT)
							return m_sDispDistance[i].nSecondStationYPos;
						else if( DirectFlag ==UP_DIRECT)
							return m_sDispDistance[i].nStationYPos;
					}

					if( IsFlagTerminal(rec->m_data.flag) &&  GetModeOfStationShowSide(i) ==1)
					{
						if(DirectFlag ==UP_DIRECT)
							return m_sDispDistance[i].nSecondStationYPos;
						else if( DirectFlag ==DOWN_DIRECT)
							return m_sDispDistance[i].nStationYPos;
					}

					return m_sDispDistance[i].nStationYPos;
				}

				return m_sDispDistance[i].nSideYPos[rec->m_data.arrive_side];
			}
			break;
		case BOTTOM_TRACK:
			return m_sDispDistance[i].nSecondStationYPos;
			break;
		default:
			return m_sDispDistance[i].nStationYPos;
			break;
		} 				
	}

	return m_sDispDistance[i].nStationYPos;
}

int CPlotDoc::GetStationVPos(CTG_SingleTrain *single,CTG_TrainRecord *rec, int usSideFlag, int usFlag, int schd_type)
{
	if(NULL == rec)
		return -1;

	int i = 0;
	if(EXIT_POSITION == usFlag)
	{
		i = rec->GetDepartTGIndex();
		if(0 > i || i >= this->m_nDispDistanceNum)
		{
			i = rec->GetArriveTGIndex();
		}
	}
	else
	{
		i = rec->GetArriveTGIndex();
		if(0 > i || i >= this->m_nDispDistanceNum)
		{
			i = rec->GetDepartTGIndex();
		}
	}

	if(0 > i || i >= this->m_nDispDistanceNum)
	{
		if(GetNodeIndex(single,rec,i,(ushort)usFlag) < 0)
			return -1;
	}

	if(0 > i || i >= this->m_nDispDistanceNum)
		return -1;

	if(m_sDispDistance[i].distance.node_info.show_sides)
	{
		switch(usSideFlag)
		{
		case TOP_TRACK://first side 
			return m_sDispDistance[i].nStationYPos;
			break;
		case PRIMARY_TRACK://prime side
			{
				for(int k = 1;k <= m_sDispDistance[i].distance.number_of_side;k++)
				{
					if(rec->m_data.arrive_side == m_sDispDistance[i].distance.side[k].side_number )
						break;
				}
				if(k > m_sDispDistance[i].distance.number_of_side)
				{
					sysprint(0,PMTG.ThreadId,"record arrive_side error!--station%d,side%d ",rec->m_data.station,rec->m_data.arrive_side);
				
					int DirectFlag = JudeTrainDirectByRecord(single, rec,ENTRY_POSITION);
					if(IsFlagStart(rec->m_data.flag)  &&  GetModeOfStationShowSide(i) ==1 )
					{
						if( DirectFlag ==DOWN_DIRECT)
							return m_sDispDistance[i].nSecondStationYPos;
						else if( DirectFlag ==UP_DIRECT)
							return m_sDispDistance[i].nStationYPos;
					}

					if( IsFlagTerminal(rec->m_data.flag) &&  GetModeOfStationShowSide(i) ==1 )
					{
						if(DirectFlag ==UP_DIRECT)
							return m_sDispDistance[i].nSecondStationYPos;
						else if( DirectFlag ==DOWN_DIRECT)
							return m_sDispDistance[i].nStationYPos;
					}

					return m_sDispDistance[i].nStationYPos;
				}
				return m_sDispDistance[i].nSideYPos[rec->m_data.arrive_side];
			}
			break;
		case BOTTOM_TRACK://last side 
			return m_sDispDistance[i].nSecondStationYPos;
			break;
		default:
			return m_sDispDistance[i].nStationYPos;
			break;
		} 				
	}
	else
		return m_sDispDistance[i].nStationYPos;

}

int CPlotDoc::GetMinuteWidthToPlotStartTime(long nTime, long nDate, BOOL isPrint, int schd_type)
{
	int width = 0;
	int offse_day;
	CString as;

	switch(schd_type)
	{
	case WORK_SCHEDULE:
	case SHIFT_SCHEDULE:
		offse_day = OsGetTwoDateOffset(PMTG.ThreadId,nDate,m_sTGGraphStartTime.day);
		width = nTime  + WHOLE_DAY_MINUTE*offse_day - m_sTGGraphStartTime.hour*60;
		break;
	case BASIC_SCHEDULE:
		if(nTime >= m_sTGGraphStartTime.hour*60)
			width = nTime - m_sTGGraphStartTime.hour*60;
		else
			width = nTime + WHOLE_DAY_SECOND/60 - m_sTGGraphStartTime.hour*60;
		break;
	case HISTORY_SCHEDULE:
		offse_day = OsGetTwoDateOffset(PMTG.ThreadId,nDate,m_sTGGraphStartTime.day);
		width = nTime  + WHOLE_DAY_MINUTE*offse_day - m_sTGGraphStartTime.hour*60;
		break;
	default:
		break;

	}

	return width;
}

int CPlotDoc::GetAbsVPos(CTG_SingleTrain *single,CTG_TrainRecord *rec, ushort position, int usFlag, ushort dir, int schd_type)
{
	//注意，在修改计划参数时，如果发现是区间作业折返车次，显示下行车全部的下方连通线，上行车显示全部上方连通线
	//对于区间作业折返车次，利用出入口号来判断下一区间位置
	//对于区间作业前行车次，处理同普通的区间停车相同，利用下一记录来取得下一区间位置
	schd_type = WORK_SCHEDULE;
	int i,j;

	if(GetNodeIndex(single,rec,i,(ushort)usFlag) < 0)
		return -1;


	//判断传入的记录在列车记录中的索引
	int usCurIndex = -1;
	for(int recindex=0; recindex<single->GetTrainRecordCount(); recindex++)
	{
		if(rec == single->GetTrainRecord(recindex))
		{
			usCurIndex = recindex;
			break;
		}
	}

	if(usCurIndex<0)
		return -1;

	//判断是否存在下一记录，如果存在，这两个记录的站号是否相同，并且前一站是否有区间停车标志
	CTG_TrainRecord* next_rec = NULL;
	next_rec = single->GetTrainRecord(usCurIndex+1);

	if(next_rec == NULL)//自由画线，绘制区间停车
	{

		if(i >= m_nDispDistanceNum || i<0)
			return -1;
		if(dir == DOWN_DIRECT)
		{		
			j = i+1;
			while(j<m_nDispDistanceNum)
			{
				if(JudeNodesConnected(i,j,WORK_SCHEDULE))
					break;
				j++;
			}
			if(j>=m_nDispDistanceNum)
				return -1;

		}
		else
		{
			j = i-1;

			while(j>=0)
			{
				if(JudeNodesConnected(i,j,WORK_SCHEDULE))
					break;
				j--;
			}
			if(j<0)
				return -1;
		}
	}
	else
	{
		//如果是区间作业折返列车，取得该车在该站的出口号，根据出口号得到线号
		//根据线号查找得到下方站，并得到序号
		if(IsFlagAbsStart(rec->m_data.flag) && position != 2)
		{
			ushort abs_num,dir;
			GetTGBasicParam()->ConvertStationEntryToAbsnum(rec->m_data.station,rec->m_data.entry,&abs_num,&dir);
			if(IsAbsStartStopUp(single->GetTrainInfo()->m_data))//上行车
			{			
				j = GetNodeIndexByAbs(i,abs_num,UP_DIRECT,WORK_SCHEDULE);
			}
			else
			{			
				j = GetNodeIndexByAbs(i,abs_num,DOWN_DIRECT,WORK_SCHEDULE);
			}

			if(j<0 || j>=m_nDispDistanceNum)
				return -1;
		}
		else if(IsFlagStopAbs(rec->m_data.flag) && rec->m_data.station == next_rec->m_data.station)
		{
			ushort abs_num,dir;
			GetTGBasicParam()->ConvertStationEntryToAbsnum(rec->m_data.station,rec->m_data.exit,&abs_num,&dir);
			if(IsFlagStopAbsUp(rec->m_data.flag))//上行车
			{			
				j = GetNodeIndexByAbs(i,abs_num,UP_DIRECT,WORK_SCHEDULE);

			}
			else
			{			
				j = GetNodeIndexByAbs(i,abs_num,DOWN_DIRECT,WORK_SCHEDULE);
			}

			if(j<0 || j>=m_nDispDistanceNum)
				return -1;

		}
		//对于区间作业前行车次，处理同普通的区间停车相同，利用下一记录来取得下一区间位置
		else
		{
			if(GetNodeIndex(single,next_rec,j,ENTRY_POSITION) < 0)
				return -1;
		}   
	}

	int y1,y2;
	if(m_sDispDistance[i].distance.node_info.show_sides)
	{
		if(i < j)
			y1 = m_sDispDistance[i].nSecondStationYPos;
		else
			y1 = m_sDispDistance[i].nStationYPos;
	}
	else
		y1 = m_sDispDistance[i].nStationYPos;

	if(m_sDispDistance[j].distance.node_info.show_sides)
	{
		if(j < i)
			y2 = m_sDispDistance[j].nSecondStationYPos;
		else
			y2 = m_sDispDistance[j].nStationYPos;
	}
	else
		y2 = m_sDispDistance[j].nStationYPos;


	switch(position)
	{
	case 0:
		return y1 + (y2-y1)/2;
		break;
	case 1:// for draw stop signal
		if(dir == DOWN_DIRECT)
			return y2 -10;
		else
			return y2 + 10;
		break;
	case 2://last side
		return (y1 + y2)/2;
		break;
	default:
		return (y1 + y2)/2;
		break;
	} 	

}

//#define PI 3.1415926
void CPlotDoc::DrawFastOrSlowTime(CDC *pCDC, CPoint &ptlStart, CPoint &ptlEnd, BOOL isFast, int minute,int dir)
{
	CPoint m_ptStart = ptlStart;
	CPoint m_ptEnd = ptlEnd;
	CPen *pOldPen,mRPen;
	COLORREF m_clrLine ;
	int nOldTextColor;
	int nOldAlign ;
	int nOldMode ;
	CFont mFont,*pOldFont;

	if(minute == 0)
	{
		return;
	}

	//middle rect size is 4 * 4 pixel
	CPoint ptMiddle,ptRect[4],ptTemp;
	double nSize = 5;
	double fDelta=0;


	// compute 8 * 8 rectangle after rotate some angle
	if(abs(m_ptStart.y - m_ptEnd.y) < abs(m_ptStart.x - m_ptEnd.x))
	{
		fDelta = (double)(m_ptEnd.y - m_ptStart.y) / (double)(m_ptEnd.x - m_ptStart.x);
		ptRect[0].y = (int)(nSize * (fDelta + 1) / sqrt(fDelta * fDelta + 1));
		ptRect[0].x = (int)(nSize * (1 - fDelta) / sqrt(fDelta * fDelta + 1));

		ptRect[1].y = (int)(nSize * (1 - fDelta) / sqrt(fDelta * fDelta + 1));
		ptRect[1].x = (int)((-nSize) * (1 + fDelta) / sqrt(fDelta * fDelta + 1));

		ptRect[2].y = (int)((-nSize) * (fDelta + 1) / sqrt(fDelta * fDelta + 1));
		ptRect[2].x = (int)((-nSize) * (1 - fDelta) / sqrt(fDelta * fDelta + 1));

		ptRect[3].y = (int)(nSize * (fDelta - 1) / sqrt(fDelta * fDelta + 1));
		ptRect[3].x = (int)(nSize * (1 + fDelta) / sqrt(fDelta * fDelta + 1));
	} 
	else 
	{
		fDelta = (double)(m_ptStart.x - m_ptEnd.x) / (double)(m_ptStart.y - m_ptEnd.y);
		ptRect[0].y = (int)(nSize * (fDelta + 1) / sqrt(fDelta * fDelta + 1));
		ptRect[0].x = (int)(nSize * (fDelta - 1) / sqrt(fDelta * fDelta + 1));

		ptRect[1].y = (int)(nSize * (fDelta - 1) / sqrt(fDelta * fDelta + 1));
		ptRect[1].x = (int)((-nSize) * (1 + fDelta) / sqrt(fDelta * fDelta + 1));

		ptRect[2].y = (int)((-nSize) * (fDelta + 1) / sqrt(fDelta * fDelta + 1));
		ptRect[2].x = (int)((-nSize) * (fDelta - 1) / sqrt(fDelta * fDelta + 1));

		ptRect[3].y = (int)(nSize * (1 - fDelta) / sqrt(fDelta * fDelta + 1));
		ptRect[3].x = (int)(nSize * (1 + fDelta) / sqrt(fDelta * fDelta + 1));
	} 


	ptMiddle.x = m_ptStart.x /2 + m_ptEnd.x / 2;
	ptMiddle.y = m_ptStart.y /2 + m_ptEnd.y / 2;

	int m_nLogPixelSy = pCDC->GetDeviceCaps(LOGPIXELSY);
	if(m_nLogPixelSy >300)
		m_nLogPixelSy = 400;

	m_clrLine = CLR_GREEN;
	mRPen.CreatePen(PS_SOLID,m_nActualPenWidth,m_clrLine);
	pOldPen = pCDC->SelectObject(&mRPen);
	nOldMode = pCDC->SetBkMode(TRANSPARENT);

	LOGFONT stFont;

	memset(&stFont,0,sizeof(stFont));
	
	stFont.lfHeight = MulDiv(7,-m_nLogPixelSy,72);	 
	stFont.lfWeight = FW_NORMAL;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	mFont.CreateFontIndirect(&stFont);
	pOldFont = pCDC->SelectObject(&mFont);


	if(dir == 2)//DOWN_DIRECT
		nOldAlign = pCDC->SetTextAlign(TA_LEFT|TA_TOP);
	else
		nOldAlign = pCDC->SetTextAlign(TA_RIGHT|TA_BOTTOM);	

	CString str;
	str.Empty();
	if(isFast)
	{
		nOldTextColor = pCDC->SetTextColor(CLR_RED);
		str.Format("%d",minute);
	}
	else
	{
		nOldTextColor = pCDC->SetTextColor(CLR_BLUE);
		str.Format("%d",minute);
	}

	if(((m_ptStart.y - m_ptEnd.y) > 0) &&(m_ptStart.x <= m_ptEnd.x))
	{
		pCDC->TextOut(ptMiddle.x, ptMiddle.y,str);
	}
	else if(((m_ptStart.y - m_ptEnd.y)> 0 ) &&(m_ptStart.x > m_ptEnd.x))
	{
		pCDC->TextOut(ptMiddle.x, ptMiddle.y,str);
	}
	else if((m_ptStart.y <= m_ptEnd.y)&&(m_ptStart.x <= m_ptEnd.x))
	{
		pCDC->TextOut(ptMiddle.x, ptMiddle.y + stFont.lfHeight-2 ,str);
	}
	else
	{
		pCDC->TextOut(ptMiddle.x , ptMiddle.y ,str);
	}


	pCDC->SelectObject(pOldFont);
	pCDC->SetTextColor(nOldTextColor);
	pCDC->SetTextAlign(nOldAlign);
	pCDC->SetBkMode(nOldMode);
	pCDC->SelectObject(pOldPen);

	mRPen.DeleteObject();
	mFont.DeleteObject();
}

void CPlotDoc::parseNotes()
{
	CString notes = NOTES.note;
	int pos = 0;


	int numofparse = 0;
	CString tempStr;
	while(pos != -1)
	{
		tempStr = notes;
		pos = notes.Find(_T("||"));
		if(pos != -1)
		{
			notes = notes.Left(pos);
			//支持两个班次的内容
			if(parsecontent[numofparse].IsEmpty())
			{
				parsecontent[numofparse] += notes;
			}
			else
			{
				parsecontent[numofparse] += "$$";
				parsecontent[numofparse] += notes;
			}
			notes = tempStr;
			notes = notes.Right(notes.GetLength() - pos -2);
		}
		else
		{
			//支持两个班次的内容
			if(parsecontent[numofparse].IsEmpty())
			{
				parsecontent[numofparse] += notes;
			}
			else
			{
				parsecontent[numofparse] += "$$";
				parsecontent[numofparse] += notes;
			}		
		}
		numofparse ++;
	}

	numofparse = 0;
}
void CPlotDoc::DrawPhaseStoreTrainFrame(CDC *pCDC, COLORREF clr)
{
	CRect drawRect;
	CString jishi_text;
	int i;

	UINT nOldAlign;
	CPen mThickPen;
	CPen mBlockPen,*pOldPen;
	UINT OldTextColor;
	CRect contentRect;
	int m_nLogPixelSy = pCDC->GetDeviceCaps(LOGPIXELSY);
	if(m_nLogPixelSy >300)
		m_nLogPixelSy = 400;

	LOGFONT stFont;
	CFont mFont,mFont2,mFont3;
	CFont *pOldFont;
	CSize strExtent;

	//设置各种属性
	mThickPen.CreatePen(PS_SOLID,(int)(1*yMultiple),clr);
	nOldAlign = pCDC->SetTextAlign(TA_LEFT|TA_TOP);
	//	nOldBackMode = pCDC->SetBkMode(TRANSPARENT);
	pOldPen = pCDC->SelectObject(&mThickPen);
	OldTextColor = pCDC->SetTextColor(clr);

	//2004-02-06
	if(m_nShiftId %2 == 0){		
		drawRect.left = x_offset_print + m_fPixelPerHour* m_nViewTimeRange + (2*JINAN_MARGIN+JINAN_STATION_WIDTH)*fWidthMulti;
		drawRect.right = drawRect.left + JINAN_PHASE_STORE_WIDTH * fWidthMulti;
	}else{		
		drawRect.left = JINAN_MARGIN*xMultiple;
		drawRect.right = drawRect.left + JINAN_PHASE_STORE_WIDTH * fWidthMulti;
	}

	drawRect.top = (JINAN_BUREAU_HEIGHT+JINAN_BIANZU_HEIGHT)* fHeightMulti;
	drawRect.bottom = drawRect.top + JINAN_TIME_STR_HEIGHT*fHeightMulti ;  


	//设置字体
	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(28,-m_nLogPixelSy,72);	 
	stFont.lfWeight = FW_NORMAL;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	mFont.CreateFontIndirect(&stFont);
	pOldFont = pCDC->SelectObject(&mFont);

	CRect titleRect = drawRect;
	pCDC->DrawText("各站阶段停留车辆",titleRect,DT_VCENTER|DT_CENTER|DT_SINGLELINE|DT_NOCLIP);
	/*
	strExtent = pCDC->GetTextExtent("各站阶段停留车辆");	
	title_Pos_y = drawRect.top - 45 * fHeightMulti;
	title_Pos_x = drawRect.left + drawRect.Width()/2 - strExtent.cx/2;

	pCDC->TextOut(title_Pos_x,title_Pos_y,"各站阶段停留车辆");
	*/
	pCDC->SelectObject(pOldFont);

	//画出框架
	drawRect.top = drawRect.bottom;
	drawRect.bottom = drawRect.top + (JINAN_MAIN_GRAPH_HEIGHT
		+JINAN_TIME_STR_HEIGHT+JINAN_BIANZU_HEIGHT)*fHeightMulti;
	pCDC->Rectangle(&drawRect);

	pCDC->MoveTo(drawRect.left  , drawRect.top +80*fHeightMulti);
	pCDC->LineTo(drawRect.right , drawRect.top +80*fHeightMulti);

	int x_first = 0.2*drawRect.Width(); //车站名称占宽度的1/5
	int y_first =  80*fHeightMulti;

	int x_step = (drawRect.Width() -x_first )/3 ;
	//draw first column(for station name)
	pCDC->MoveTo( drawRect.left + x_first, drawRect.top);
	pCDC->LineTo( drawRect.left + x_first, drawRect.bottom);
	//draw remains columns 
	for(i=0;i<2;i++)
	{
		pCDC->MoveTo(drawRect.left +x_first +x_step *(i+1),drawRect.top);
		pCDC->LineTo(drawRect.left +x_first +x_step *(i+1),drawRect.bottom);
	}

	int nStationNum;
	int nStationList[64];
	nStationNum=sizeof(nStationList)/sizeof(nStationList[0]);
	gpcdef.GetSectionPhaseStoreStationList(m_nCurrentSectionNo,nStationNum,nStationList);
	//缺省的车站个数
	int nRowCount = nStationNum;
	int y_step = (drawRect.Height() -y_first)/nRowCount;
	for(i=0;i<nRowCount-1;i++)
	{
		pCDC->MoveTo(drawRect.left , drawRect.top +y_first +(i+1)*y_step);
		pCDC->LineTo(drawRect.right, drawRect.top +y_first +(i+1)*y_step);
	}
	//draw bias line;
	pCDC->MoveTo( drawRect.left, drawRect.top);
	pCDC->LineTo( drawRect.left + x_first, drawRect.top +y_first);

	//fill the fix content of the frame;
	CString str_text;
	pCDC->SelectObject(pOldFont);
	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(12,m_nLogPixelSy,72);	 
	stFont.lfWeight = FW_BOLD;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	mFont2.CreateFontIndirect(&stFont);
	pOldFont = pCDC->SelectObject(&mFont2);
	str_text = "站名";
	strExtent = pCDC->GetTextExtent(str_text);

	contentRect = drawRect;
	contentRect.left = contentRect.left + 10*fWidthMulti;
	contentRect.top = contentRect.top + 10 * fHeightMulti;
	contentRect.bottom = drawRect.top + (y_first-10*fHeightMulti);
	contentRect.right = drawRect.left + (x_first-10*fWidthMulti);

	pCDC->DrawText("站 名",&contentRect,DT_BOTTOM|DT_LEFT|DT_SINGLELINE|DT_WORDBREAK|DT_EXTERNALLEADING|DT_NOCLIP);
	pCDC->DrawText("时 间",&contentRect,DT_TOP|DT_RIGHT|DT_SINGLELINE|DT_WORDBREAK|DT_EXTERNALLEADING|DT_NOCLIP);
	int nYear=m_sPrintInform->usShiftid/1000000;
	int nMonth=(m_sPrintInform->usShiftid-nYear*1000000 )/10000;
	int nDay=(m_sPrintInform->usShiftid-nYear*1000000-nMonth*10000 )/100;
	unsigned long banbie=m_sPrintInform->usShiftid-nYear*1000000-nMonth*10000-nDay*100;
	CString str,str_1,str_2,str_3;
	if(banbie == 1)
	{
		str_1.Format("十八点"); str_2.Format("零 点"); str_3.Format("六 点");
	}
	else
	{
		str_1.Format("六点"); str_2.Format("十二点"); str_3.Format("十八点");
	}


	pCDC->SelectObject(pOldFont);
	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(20,m_nLogPixelSy,72);	 
	stFont.lfWeight = FW_BOLD;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	mFont3.CreateFontIndirect(&stFont);
	pOldFont = pCDC->SelectObject(&mFont3);

	contentRect = drawRect;
	contentRect.bottom = drawRect.top + y_first;

	for(i=0;i<3;i++)
	{
		contentRect.left = drawRect.left + x_first + i*x_step;	 
		contentRect.right =  contentRect.left  + x_step;

		if(i==0)
			str = str_1;
		if(i==1)
			str = str_2;
		if(i==2)
			str= str_3;
		pCDC->DrawText(str,&contentRect,DT_VCENTER|DT_CENTER|DT_SINGLELINE|DT_WORDBREAK|DT_EXTERNALLEADING|DT_NOCLIP);

	}
	//show staton name;

	contentRect = drawRect;
	contentRect.left = drawRect.left;	 
	contentRect.right =  contentRect.left  + x_first;
	/*****commented by qianguorong 2004-02-05 
	contentRect.right =  contentRect.left  + x_first*yMultiple;
	*/
	for(i=0;i<nStationNum;i++)
	{	
		contentRect.top    = drawRect.top + y_first + i*y_step;
		contentRect.bottom = contentRect.top  + y_step;
		/*****commented by qianguorong 2004-02-05 
		contentRect.top    = drawRect.top + y_first*yMultiple + i*y_step*yMultiple;
		contentRect.bottom = contentRect.top  + y_step*yMultiple;
		*/

		str = gpcdef.GetStationName(nStationList[i]);
		pCDC->DrawText(str,&contentRect,DT_VCENTER|DT_CENTER|DT_SINGLELINE|DT_WORDBREAK|DT_EXTERNALLEADING|DT_NOCLIP);

	}

	pCDC->SelectObject(pOldFont);
	pCDC->SetTextAlign(nOldAlign);
	//	pCDC->SetBkMode(nOldBackMode);
	pCDC->SelectObject(pOldPen);
	pCDC->SetTextColor(OldTextColor);

	mFont.DeleteObject();	
	mFont2.DeleteObject();
	mFont3.DeleteObject();
	mThickPen.DeleteObject();
}

void CPlotDoc::ShowPhaseStoreTrain(CDC *pCDC, COLORREF clr, int nStationNo, int phase,CString& stext)
{
	CRect drawRect;

	UINT nOldAlign;
	UINT OldTextColor;

	CRect contentRect;
	int m_nLogPixelSy = pCDC->GetDeviceCaps(LOGPIXELSY);
	if(m_nLogPixelSy >300)
		m_nLogPixelSy = 400;

	//设置各种属性	
	nOldAlign = pCDC->SetTextAlign(TA_LEFT|TA_TOP);
	OldTextColor = pCDC->SetTextColor(clr);

	LOGFONT stFont;
	CFont mFont;
	CFont *pOldFont;
	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(12,-m_nLogPixelSy,72);	 
	stFont.lfWeight = FW_THIN;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	mFont.CreateFontIndirect(&stFont);
	pOldFont = pCDC->SelectObject(&mFont);


	//2004-02-06
	if(m_nShiftId %2 == 0){		
		drawRect.left = x_offset_print + m_fPixelPerHour* m_nViewTimeRange + (2*JINAN_MARGIN+JINAN_STATION_WIDTH)*fWidthMulti;
		drawRect.right = drawRect.left + JINAN_PHASE_STORE_WIDTH * fWidthMulti;
	}else{		
		drawRect.left = JINAN_MARGIN*fWidthMulti;
		drawRect.right = drawRect.left + JINAN_PHASE_STORE_WIDTH * fWidthMulti;
	}
	drawRect.top = (JINAN_BUREAU_HEIGHT+JINAN_BIANZU_HEIGHT)* fHeightMulti;
	drawRect.bottom = drawRect.top + JINAN_TIME_STR_HEIGHT*fHeightMulti ;  

	//画出框架
	drawRect.top = drawRect.bottom;
	drawRect.bottom = drawRect.top + (JINAN_MAIN_GRAPH_HEIGHT
		+JINAN_TIME_STR_HEIGHT+JINAN_BIANZU_HEIGHT)*fHeightMulti;

	int x_first = 0.2*drawRect.Width();
	int y_first =  80*fHeightMulti;

	int x_step = (drawRect.Width() -x_first )/3 ;
	int nStationNum;
	int nStationList[64];
	nStationNum=sizeof(nStationList)/sizeof(nStationList[0]);
	gpcdef.GetSectionPhaseStoreStationList(m_nCurrentSectionNo,nStationNum,nStationList);


	CRect temp = drawRect;
	temp.right = temp.left + x_first;
	temp.bottom = temp.top + y_first;

	//缺省的车站个数
	int nRowCount = nStationNum;
	int y_step = (drawRect.Height() -y_first)/nRowCount;


	int nYear=m_sPrintInform->usShiftid/1000000;
	int nMonth=(m_sPrintInform->usShiftid-nYear*1000000 )/10000;
	int nDay=(m_sPrintInform->usShiftid-nYear*1000000-nMonth*10000 )/100;
	unsigned long banbie=m_sPrintInform->usShiftid-nYear*1000000-nMonth*10000-nDay*100;
	CString str,str_1,str_2,str_3;
	int j;
	if(banbie == 1)
	{
		str_1.Format("十八点"); str_2.Format("零 点"); str_3.Format("六 点");
		if(phase ==18)
			j=0;
		else if(phase ==0)
			j=1;
		else
			j=2;
	}
	else
	{
		str_1.Format("六点"); str_2.Format("十二点"); str_3.Format("十八点");
		if(phase ==6)
			j=0;
		else if(phase ==12)
			j=1;
		else
			j=2;
	}


	//show staton name;
	contentRect = drawRect;
	contentRect.left = drawRect.left + x_first + x_step*j;
	contentRect.right =  contentRect.left  +  x_step;

	for(int i=0;i<nStationNum;i++)
	{
		if(nStationList[i] != nStationNo)
			continue;
		contentRect.top    = drawRect.top + y_first + i*y_step;
		contentRect.bottom = contentRect.top  + y_step;	

		str =  stext;
		contentRect.InflateRect(-10*fWidthMulti,-10*fHeightMulti);
		pCDC->DrawText(str,&contentRect, DT_VCENTER|DT_CENTER|DT_WORDBREAK|DT_EXTERNALLEADING|DT_NOCLIP);

	}


	pCDC->SelectObject(pOldFont);
	pCDC->SetTextAlign(nOldAlign);
	pCDC->SetTextColor(OldTextColor);
	mFont.DeleteObject();

}

//绘制波浪线的判断
/* 待避波浪线的判断原则(哈尔滨，05年6月)
同方向的列车，先到却后开，则此车绘制待避波浪线
*/
bool CPlotDoc::JudgeNeedWaveLine(CTG_TrainRecord *rec,int schd_type, int nDirectFlag)
{
	CTG_SingleTrain *train;
	int startX,endX,tempStartX,tempEndX;
	long time_depart_rec,date_depart_rec,time_arrival_rec,date_arrival_rec;
	time_arrival_rec = minute_time(rec->m_data.arrive,&date_arrival_rec);
	if(rec->m_data.depart == NOTIME || rec->m_data.depart == 0)
	{
		date_depart_rec = date_arrival_rec;
		time_depart_rec = time_arrival_rec;
	}
	else
	{
		time_depart_rec = minute_time(rec->m_data.depart,&date_depart_rec);
	}
	startX = GetMinuteWidthToPlotStartTime(time_arrival_rec/60,date_arrival_rec,schd_type);
	endX = GetMinuteWidthToPlotStartTime(time_depart_rec/60,date_depart_rec,schd_type);
	bool flag = FALSE;

	int nSchdTrainNum = m_vecSchdTrains.size();
	for(int i=0;i < nSchdTrainNum;i++)
	{
		CTG_TrainRecord *temp;
		train = &m_vecSchdTrains[i];
		//非同方向车，不绘制
		int m_nDirectFlag_prefix = GetTrainRecDirect(train, 0);
		
		if(m_nDirectFlag_prefix != nDirectFlag)
			continue;

		temp = train->GetTrainRecordByStation(rec->m_data.station); 
		if(temp == NULL)  continue;
		if(IsFlagStart(temp->m_data.flag) || IsFlagTerminal(temp->m_data.flag))
			continue;
		if(!(temp->m_data.entry == rec->m_data.entry && temp->m_data.exit == rec->m_data.exit))
			continue;
		//到点，发点位置
		long time_depart_rec,date_depart_rec,time_arrival_rec,date_arrival_rec;
		time_arrival_rec = minute_time(temp->m_data.arrive,&date_arrival_rec);
		if(temp->m_data.depart == NOTIME || temp->m_data.depart == 0)
		{
			date_depart_rec = date_arrival_rec;
			time_depart_rec = time_arrival_rec;
		}
		else
		{
			time_depart_rec = minute_time(temp->m_data.depart,&date_depart_rec);
		}
		tempStartX = GetMinuteWidthToPlotStartTime(time_arrival_rec/60,date_arrival_rec,schd_type);
		tempEndX = GetMinuteWidthToPlotStartTime(time_depart_rec/60,date_depart_rec,schd_type);
		//其他的运行线的到发点有一个落在其中
		////if((tempStartX > startX && tempStartX < endX)  	||	(tempEndX > startX && tempEndX < endX) )
		if( (tempStartX > startX && tempStartX < endX) &&
			(tempEndX < endX) )
		{
			flag = TRUE;
			break;  
		}
	}

	return flag;
}
void CPlotDoc::DrawActualStopStationLine(CDC* pCDC, CPoint& ptlStart, CPoint& ptlEnd,int factor,int dir)
{
	int scaleRate = min(yMultiple,5);
	int nLineWidth = scaleRate*m_nLineWidthRate;
	
	if(ptlStart.y < 0 || ptlEnd.y < 0)
		return;

	COLORREF clrLine = RGB(255, 255, 0);
	LINE_COLOR color;
	if(m_lineCfg.GetTrainLineColor(factor, color))
		clrLine = color.real;


	CPoint m_ptStart = ptlStart;
	CPoint m_ptEnd = ptlEnd;

	CPen mPen,*pOldPen;
	mPen.CreatePen(PS_SOLID, nLineWidth, clrLine);
	pOldPen = pCDC->SelectObject(&mPen);

	DrawStopStationLine(pCDC,ptlStart,ptlEnd,dir);

	pCDC->SelectObject (pOldPen);
	mPen.DeleteObject();
}

void CPlotDoc::DrawStopStationLine(CDC* pCDC, CPoint& ptlStart, CPoint& ptlEnd,int dir)
{
	int scaleRate = min(yMultiple,5);
	DrawChordLine(pCDC,ptlStart,ptlEnd,dir,scaleRate*m_nLineWidthRate);
	return;
}

void CPlotDoc::DrawBadMark(CDC *pCDC, CPoint &ptlStart, CPoint &ptlEnd,int line_type,CString strTrainID)
{
	int scaleRate = min(yMultiple,5);
	CPoint m_ptStart = ptlStart;
	CPoint m_ptEnd = ptlEnd;
	CPen *pOldPen,mRPen;
	COLORREF m_clrLine ;
	int nOldTextColor;
	int nOldAlign ;
	int nOldMode ;
	CFont mFont,*pOldFont;


	//if switch is close then return; //to be add 

	switch(line_type){
	case EXCEPTION_MARK:
		{
			m_clrLine = CLR_GREEN;
			mRPen.CreatePen(PS_SOLID, scaleRate*m_nLineWidthRate,m_clrLine);
			pOldPen = pCDC->SelectObject(&mRPen);
		}
		break;
	case BAD_MARK:
		{
			m_clrLine = CLR_RED;
			mRPen.CreatePen(PS_SOLID, scaleRate*m_nLineWidthRate,m_clrLine);
			pOldPen = pCDC->SelectObject(&mRPen);
		}
		break;
	case REVERSE_MARK:
		{
			m_clrLine = CLR_RED;
			mRPen.CreatePen(PS_SOLID, scaleRate*m_nLineWidthRate,m_clrLine);
			pOldPen = pCDC->SelectObject(&mRPen);

		}
		break;
	case STOPREASTON_MARK:
		{
			m_clrLine = CLR_GREEN;
			mRPen.CreatePen(PS_SOLID, scaleRate*m_nLineWidthRate,m_clrLine);
			pOldPen = pCDC->SelectObject(&mRPen);
		};
		break;
	default:
		{
			m_clrLine = CLR_RED;
			mRPen.CreatePen(PS_SOLID, scaleRate*m_nLineWidthRate,m_clrLine);
			pOldPen = pCDC->SelectObject(&mRPen);
		}
		break;
	}
	//middle rect size is 4 * 4 pixel
	CPoint ptMiddle,ptRect[4],ptTemp;
	double nSize = 5;
	double fDelta=0;


	// compute 8 * 8 rectangle after rotate some angle
	//	if(abs(m_ptStart.y - m_ptEnd.y) < abs(m_ptStart.x - m_ptEnd.x))
	if(TRUE)
	{
		fDelta = (double)(m_ptEnd.y - m_ptStart.y) / (double)(m_ptEnd.x - m_ptStart.x);
		ptRect[0].y = (int)(nSize * (fDelta + 1) / sqrt(fDelta * fDelta + 1));
		ptRect[0].x = (int)(nSize * (1 - fDelta) / sqrt(fDelta * fDelta + 1));

		ptRect[1].y = (int)(nSize * (1 - fDelta) / sqrt(fDelta * fDelta + 1));
		ptRect[1].x = (int)((-nSize) * (1 + fDelta) / sqrt(fDelta * fDelta + 1));

		ptRect[2].y = (int)((-nSize) * (fDelta + 1) / sqrt(fDelta * fDelta + 1));
		ptRect[2].x = (int)((-nSize) * (1 - fDelta) / sqrt(fDelta * fDelta + 1));

		ptRect[3].y = (int)(nSize * (fDelta - 1) / sqrt(fDelta * fDelta + 1));
		ptRect[3].x = (int)(nSize * (1 + fDelta) / sqrt(fDelta * fDelta + 1));
	} 
	else 
	{
		fDelta = (double)(m_ptStart.x - m_ptEnd.x) / (double)(m_ptStart.y - m_ptEnd.y);
		ptRect[0].y = (int)(nSize * (fDelta + 1) / sqrt(fDelta * fDelta + 1));
		ptRect[0].x = (int)(nSize * (fDelta - 1) / sqrt(fDelta * fDelta + 1));

		ptRect[1].y = (int)(nSize * (fDelta - 1) / sqrt(fDelta * fDelta + 1));
		ptRect[1].x = (int)((-nSize) * (1 + fDelta) / sqrt(fDelta * fDelta + 1));

		ptRect[2].y = (int)((-nSize) * (fDelta + 1) / sqrt(fDelta * fDelta + 1));
		ptRect[2].x = (int)((-nSize) * (fDelta - 1) / sqrt(fDelta * fDelta + 1));

		ptRect[3].y = (int)(nSize * (1 - fDelta) / sqrt(fDelta * fDelta + 1));
		ptRect[3].x = (int)(nSize * (1 + fDelta) / sqrt(fDelta * fDelta + 1));
	} 


	ptMiddle.x = m_ptStart.x /2 + m_ptEnd.x / 2;
	ptMiddle.y = m_ptStart.y /2 + m_ptEnd.y / 2;

	switch(line_type){
	case EXCEPTION_MARK:
	case BAD_MARK:
		{
			pCDC->MoveTo(ptRect[0].x + ptMiddle.x,ptRect[0].y + ptMiddle.y);
			pCDC->LineTo(ptRect[2].x + ptMiddle.x,ptRect[2].y + ptMiddle.y);
			pCDC->MoveTo(ptRect[1].x + ptMiddle.x,ptRect[1].y + ptMiddle.y);
			pCDC->LineTo(ptRect[3].x + ptMiddle.x,ptRect[3].y + ptMiddle.y); 		
		}
		break;
	case REVERSE_MARK:
		{
			LOGFONT stFont;

			memset(&stFont,0,sizeof(stFont));
			int temp = -pCDC->GetDeviceCaps(LOGPIXELSY);
			stFont.lfHeight = MulDiv(8,-pCDC->GetDeviceCaps(LOGPIXELSY),72);	 
			stFont.lfWeight = FW_NORMAL;
			stFont.lfClipPrecision = CLIP_LH_ANGLES;
			nOldTextColor = pCDC->SetTextColor(CLR_BLUE);
			nOldAlign = pCDC->SetTextAlign(TA_LEFT|TA_TOP);
			nOldMode = pCDC->SetBkMode(TRANSPARENT);
			
			strTrainID += "反";
			
			if(((m_ptStart.y - m_ptEnd.y) > 0) &&(m_ptStart.x <= m_ptEnd.x))
			{
				stFont.lfEscapement = atan2(double(m_ptStart.y- m_ptEnd.y), double(m_ptEnd.x-m_ptStart.x)) * 180 / PI * 10;
				mFont.CreateFontIndirect(&stFont);
				pOldFont = pCDC->SelectObject(&mFont);

				pCDC->TextOut(ptMiddle.x-2 , ptMiddle.y ,strTrainID);
			}
			else if(((m_ptStart.y - m_ptEnd.y)> 0 ) &&(m_ptStart.x > m_ptEnd.x))
			{
				stFont.lfEscapement = - atan2(double(m_ptStart.y- m_ptEnd.y), double(m_ptStart.x-m_ptEnd.x)) * 180 / PI * 10;
				mFont.CreateFontIndirect(&stFont);
				pOldFont = pCDC->SelectObject(&mFont);

				pCDC->TextOut(ptMiddle.x+2 , ptMiddle.y ,strTrainID);
			}
			else if((m_ptStart.y <= m_ptEnd.y)&&(m_ptStart.x <= m_ptEnd.x))
			{
				stFont.lfEscapement = - atan2(double( -m_ptStart.y + m_ptEnd.y), double(-m_ptStart.x+m_ptEnd.x)) * 180 / PI * 10;
				mFont.CreateFontIndirect(&stFont);
				pOldFont = pCDC->SelectObject(&mFont);

				pCDC->TextOut(ptMiddle.x+2 , ptMiddle.y ,strTrainID);

			}
			else
			{
				stFont.lfEscapement = atan2(double( -m_ptStart.y + m_ptEnd.y), double(m_ptStart.x-m_ptEnd.x)) * 180 / PI * 10;
				mFont.CreateFontIndirect(&stFont);
				pOldFont = pCDC->SelectObject(&mFont);

				pCDC->TextOut(ptMiddle.x-2 , ptMiddle.y ,strTrainID);

			}


			pCDC->SelectObject(pOldFont);
			pCDC->SetTextColor(nOldTextColor);
			pCDC->SetTextAlign(nOldAlign);
			pCDC->SetBkMode(nOldMode);
			mFont.DeleteObject();
		}
		break;
	case STOPREASTON_MARK:
		{
			CRect tempRect;
			tempRect.bottom = ptMiddle.y - 4;
			tempRect.left = ptMiddle.x - 4;
			tempRect.right = ptMiddle.x + 4;
			tempRect.top = ptMiddle.y + 4;
			CBrush tempBrush(CLR_GREEN);
			CBrush *pOldBrush;
			pOldBrush = pCDC->SelectObject(&tempBrush);
			pCDC->Ellipse(&tempRect);
			pCDC->SelectObject(pOldBrush);
			tempBrush.DeleteObject();
		};
		break;

	default:		
		break;
	}

	pCDC->SelectObject(pOldPen); 		
	mRPen.DeleteObject();
}

int CPlotDoc::GetNodeIndex(CTG_SingleTrain * single,CTG_TrainRecord* rec,int& index,ushort usFlag)
{
	index = -1;

	if(usFlag == EXIT_POSITION)
		index = rec->GetDepartTGIndex();
	else if(usFlag == ENTRY_POSITION)
		index = rec->GetArriveTGIndex();

	return index; 
}

void CPlotDoc::DrawChordLine(CDC* pDC,CPoint pt1,CPoint pt2,int dir,int rate)
{
	//距离太近，直接连接
	if(abs(pt1.x-pt2.x)<10)
	{
		pDC->MoveTo(pt1);
		pDC->LineTo(pt2);
		return;
	}
	const int A = 20*rate;  
	CPoint pt_start = pt1;
	CPoint pt_end   = pt2;  
	int X_WIDTH = pt_end.x - pt_start.x;
	CPoint pt = pt_start;
	double step = 2*PI/(X_WIDTH);
	for(double i=0;i<=2*PI; i += step)
	{
		pDC->MoveTo(pt);
		pt.x ++;	 
		if(dir == DOWN_DIRECT)
			pt.y = pt_start.y+A*sin(i);
		else
			pt.y = pt_start.y-A*sin(i);
		pDC->LineTo(pt);    
		//避免断线，进行最后的绘制
		if(i+step >=2*PI)
		{
			pDC->LineTo(pt2);
			break;
		}
	}
}


int CPlotDoc::GetEntryExitDirection(CTG_SingleTrain * single,CTG_TrainRecord* rec,ushort usFlag,int section,int schd_type,short dFlag)
{
	int node_index;
	bool flag = FALSE;
	int direction = -1;

	Pstruct _node_info *node_info;
	do
	{
		if(single == NULL) break;
		GetNodeIndex(single,rec,node_index,usFlag);

		if(node_index <0 || node_index >= m_nDispDistanceNum) break;
		node_info = &m_sDispDistance[node_index].distance.node_info;

		int j=0;
	
		ushort line=0,dir=0;

		if(usFlag == ENTRY_POSITION)
		{
			if(rec->m_data.entry == (ushort) -1) break;
			if(GetTGBasicParam()->ConvertStationEntryToAbsnum(rec->m_data.station,
				rec->m_data.entry,
				&line,
				&dir) == RET_FAILED) 
				break;

		}
		else
		{
			if(rec->m_data.exit == (ushort) -1) break;
			if(GetTGBasicParam()->ConvertStationEntryToAbsnum(rec->m_data.station,
				rec->m_data.exit,
				&line,
				&dir) == RET_FAILED)
				break;

		}
		for(j= 0;j <  node_info->up_line_count;j++)
		{
			if(line ==  node_info->up_line_list[j])
				direction = UP_MARK;                 					 
		}
		if(dFlag != -1 && dFlag == direction){//yuzh 20110529 处理西安局包西一台张桥打印问题
			flag = TRUE;
			break;
		}
		//looking for down line
		for(j= 0;j <  node_info->down_line_count;j++)
		{
			if(line ==  node_info->down_line_list[j])
				direction = DOWN_MARK;                        					 
		}

		flag = TRUE;
	}while(0);

	if(flag && direction != -1 )
	{
		return direction;        
	}
	else
	{
		return -1;
	}
}

int CPlotDoc::GetMinuteWidthToGraphStartTime(long nTime,long nDate,int schd_type)
{
	int width = 0;
	int offse_day=0;

	switch(schd_type)
	{
	case WORK_SCHEDULE:
	case SHIFT_SCHEDULE:
	case HISTORY_SCHEDULE:
		offse_day = OsGetTwoDateOffset(PMTG.ThreadId,nDate,m_sTGGraphStartTime.day);
		width = nTime  + WHOLE_DAY_MINUTE*offse_day - m_sTGGraphStartTime.hour*60;
		break;

	case BASIC_SCHEDULE:
		if(nTime >= m_sTGBasicGraphStartTime.hour*60)
			width = nTime - m_sTGBasicGraphStartTime.hour*60;
		else
			width = nTime + WHOLE_DAY_SECOND/60 - m_sTGBasicGraphStartTime.hour*60;
		break;
	default:
		break;
	}
	return width;
}

BOOL CPlotDoc::LoadTGProfile()
{
	memset(&m_InhibitStatTrain, 0, sizeof(INHIBIT_TRAIN));
	
	m_nCfgUnknownTrainColor= RGB(184,174,7);
	m_nCfgUpAbsEleColor = m_nCfgDownAbsEleColor = CLR_RED;

	// 读取配置文件
	char *token;
	char seps[]=" ,\t\\/=\n";
	//for read line info
	int  nSectionNo = -1;
	int  nNodeCount = -1;
	
	int nRouteCount = -1;

	int nStationCount =-1;

	//for read route info
	int nRouteIndex = 0;

	FILE *fp=NULL;
	//	int result;
	char *ptr, szFileName[128], strLine[1024];
	sprintf(szFileName,"%s\\%s",gpcdef.GetDataDir(),TGDEF);
	if((fp=fopen(szFileName, "r"))==NULL)
		return 0;
	memset(&section_node_info, 0x00, sizeof(section_node_info));
	section_node_info.sub_section_count = 0;
	section_node_info.node_count = 0;
	section_node_info.node_info[nNodeCount].sub_section = 0;

	// read file
	m_nTgErrorline = 0;
	while (!feof(fp))	
	{
		ptr = fgets(strLine, 1024, fp);
		m_nTgErrorline++;
		if (ptr == NULL) 
			continue;
		while (*ptr == ' ' || *ptr == '\t')
			ptr ++;
		if (*ptr == ';')	
			continue;
		if (*ptr == '\n')	
			continue;
		if (*ptr == '[' && !strncmp(ptr, GPC_SECTION_NODE, strlen(GPC_SECTION_NODE)) ) 
		{
			ptr = ptr + strlen(GPC_SECTION_NODE);
			
			// section no 
			token = strtok(ptr, seps);
			if(token==NULL) 
			{
				fclose(fp);
				return 0;
			}
			int nTmpSectionNo = atoi(token);
			if(nTmpSectionNo != m_nCurrentSectionNo)
				continue;

			if(nSectionNo != nTmpSectionNo)
			{
				nSectionNo = (int)atoi(token);
				nNodeCount = 0;
			}
			if(nNodeCount>=MAX_STATIONS_PER_ROUTE){
				AfxMessageBox("系统节点号已经溢出!");
			}
			nNodeCount = section_node_info.node_count;

			// sub_num
			token = strtok(NULL, seps);
			if(token==NULL)
			{
				fclose(fp);
				return 0;
			}
			if(section_node_info.sub_section_count != (int)atoi(token))
				section_node_info.sub_section_count++; 
			section_node_info.node_info[nNodeCount].sub_section = (int)atoi(token);

			// index
			token = strtok(NULL, seps);
			if(token==NULL)
			{
				fclose(fp);
				return 0;
			}
			section_node_info.node_info[nNodeCount].index =  (int)atoi(token);
			
			// name
			token = strtok(NULL, seps);
			if(token==NULL)
			{
				fclose(fp);
				return 0;
			}
			strncpy(section_node_info.node_info[nNodeCount].text, (const char*)token,49);
			
			//staton number
			token = strtok(NULL, seps);
			if(token==NULL)
			{
				fclose(fp);
				return 0;
			}
			section_node_info.node_info[nNodeCount].station_number =  (int)atoi(token);
			
			// down line number
			token = strtok(NULL, seps);
			if(token==NULL)
			{
				fclose(fp);
				return 0;
			}
			section_node_info.node_info[nNodeCount].down_line_count = (int)atoi(token);
			// check downline list
			if(section_node_info.node_info[nNodeCount].down_line_count>=MAX_ENTRY)
			{
				AfxMessageBox("系统下方线号已经溢出!");
				section_node_info.node_info[nNodeCount].down_line_count=50;
			}

			// down abs line list
			for(int i=0; i< section_node_info.node_info[nNodeCount].down_line_count;i++)
			{
				token = strtok(NULL, seps);
				if(token==NULL)
				{
					fclose(fp);
					return 0;
				}
				section_node_info.node_info[nNodeCount].down_line_list[i] = (int)atoi(token);
			}

			
			// up line number
			token = strtok(NULL, seps);
			if(token==NULL)
			{
				fclose(fp);
				return 0;
			}
			section_node_info.node_info[nNodeCount].up_line_count =  (int)atoi(token);
			// check downline list
			if(section_node_info.node_info[nNodeCount].up_line_count>=MAX_ENTRY)
			{
				AfxMessageBox("系统上方线号已经溢出!");
				section_node_info.node_info[nNodeCount].up_line_count=50;
			}

			// up abs line list
			for(i=0; i< section_node_info.node_info[nNodeCount].up_line_count;i++)
			{
				token = strtok(NULL, seps);
				if(token==NULL)
				{
					fclose(fp);
					return 0;
				}
				section_node_info.node_info[nNodeCount].up_line_list[i] = (int)atoi(token);
			}
			
			// staton space
			token = strtok(NULL, seps);
			if(token==NULL)
			{
				fclose(fp);
				return 0;
			}
			section_node_info.node_info[nNodeCount].space =  (int)atoi(token);

			// 是否显示股道，0不显示，1股道展开画股道线，2股道展开空白
			int nStationSideMode=0;
			token = strtok(NULL, seps);
			if(token==NULL)
				nStationSideMode = 0;
			else
				nStationSideMode =  (int)atoi(token);

			//mode为1表示空白，0表示正常显示股道线
			if(nStationSideMode == 0)      //车站不展开,双击时候为有股道线的形式
			{
				section_node_info.node_info[nNodeCount].show_sides=0;
				section_node_info.node_info[nNodeCount].show_sides_mode = 1;
			}
			else if(nStationSideMode == 1) //车站展开,双击时候为有空白股道线的形式
			{
				section_node_info.node_info[nNodeCount].show_sides=1;
				section_node_info.node_info[nNodeCount].show_sides_mode = 0;
			}
			else if(nStationSideMode == 2) //车站展开,双击时候为有股道线的形式
			{
				section_node_info.node_info[nNodeCount].show_sides=1;
				section_node_info.node_info[nNodeCount].show_sides_mode = 1;
			}
			else if(nStationSideMode == 3) //朔黄 车站不展开 -- 禁止股道空白展开
			{
				section_node_info.node_info[nNodeCount].show_sides=0;
				section_node_info.node_info[nNodeCount].show_sides_mode = 0;
			}
			else if(nStationSideMode == 4) //朔黄 车站展开 -- 禁止股道空白展开
			{
				section_node_info.node_info[nNodeCount].show_sides=1;
				section_node_info.node_info[nNodeCount].show_sides_mode = 0;
			}
		
			// 站平线是否加粗
			token = strtok(NULL, seps);
			if(token==NULL)
			{
				fclose(fp);
				return 0;
			}
			section_node_info.node_info[nNodeCount].isNeedThick = (int)atoi(token);

			// show statistic flag
			token = strtok(NULL, seps);
			if(token==NULL)
			{
				fclose(fp);
				return 0;
			}
			section_node_info.node_info[nNodeCount].show_statistic = (int)atoi(token);		 
			
			// 上行客车
			token = strtok(NULL, seps);
			if(token==NULL)
			{
				fclose(fp);
				return 0;
			}
			section_node_info.node_info[nNodeCount].up_pessenger_train = (int)atoi(token);		 
			// 上行货车  
			token = strtok(NULL, seps);
			if(token==NULL)	
			{
				fclose(fp);
				return 0;
			}
			section_node_info.node_info[nNodeCount].up_goods_train = (int)atoi(token);		 			     
			// 下行客车
			token = strtok(NULL,seps);
			if(token == NULL)
			{
				fclose(fp);
				return 0;
			}
			section_node_info.node_info[nNodeCount].down_pessenger_train = (int)atoi(token);		 			     
			// 下行货车
			token = strtok(NULL,seps);
			if(token == NULL) 
			{
				fclose(fp);
				return 0;
			}
			section_node_info.node_info[nNodeCount].down_goods_train = (int)atoi(token);		 			     
			
			// 禁止同时接车方向类型
			token = strtok(NULL,seps);
			if(token == NULL)
				section_node_info.node_info[nNodeCount].inhibit_sametime_dir = (int)3;		 			     
			else
				section_node_info.node_info[nNodeCount].inhibit_sametime_dir = (int)atoi(token);		 			     

			// 电网分相   0:不绘制(默认) 1:上 2:下 3:上下
			token = strtok(NULL,seps);
			if(token == NULL)
				section_node_info.node_info[nNodeCount].show_sides_TanChuang_UpDn = 0;
			else
			{
				section_node_info.node_info[nNodeCount].show_sides_TanChuang_UpDn = (int)atoi(token);
				if(section_node_info.node_info[nNodeCount].show_sides_TanChuang_UpDn > 2)
					section_node_info.node_info[nNodeCount].show_sides_TanChuang_UpDn = 0;				
			}
			// 压前压后
			token = strtok(NULL,seps);
			
			// 公里标
			token = strtok(NULL,seps);			
			if(token==NULL || (int)atoi(token) == 0)
			{
				int station_no = section_node_info.node_info[nNodeCount].station_number;
				strncpy(section_node_info.node_info[nNodeCount].kilo_mark, gpcdef.GetStationKiloMeter(station_no),sizeof(section_node_info.node_info[nNodeCount].kilo_mark));
			}
			else
			{
				strncpy(section_node_info.node_info[nNodeCount].kilo_mark, token, sizeof(section_node_info.node_info[nNodeCount].kilo_mark));
			}
			section_node_info.node_count += 1;
		}
		else if (*ptr == '[' && !strncmp(ptr, BIANZHU_STATION, strlen(BIANZHU_STATION)) ) 
		{
			ptr = ptr + strlen(BIANZHU_STATION);

			token = strtok(ptr, seps);
			if(token==NULL) 
			{
				fclose(fp);
				return 0;
			}
			int count = (int)atoi(token);
			int station = 0;
			for(int j= 0;j<count;j++)
			{
				token = strtok(NULL, seps);
				if(token==NULL)
				{
					fclose(fp);
					return 0;
				}
				station = (int)atoi(token);
				bianzhu_station_list.push_back(station);
			}
		}

		else if (*ptr == '[' && !strncmp(ptr, FENJIE_STATION, strlen(FENJIE_STATION)) ) 
		{
			ptr = ptr + strlen(FENJIE_STATION);

			token = strtok(ptr, seps);
			if(token==NULL) return 0;
			int count = (int)atoi(token);
			int station = 0;
			for(int j= 0;j<count;j++)
			{
				token = strtok(NULL, seps);
				if(token==NULL)
				{
					fclose(fp);
					return 0;
				}
				station = (int)atoi(token);
				fenjie_station_list.push_back(station);
			}
		}

		else if (*ptr == '[' && !strncmp(ptr, DISPLAY_TYPE_INHIBIT, strlen(DISPLAY_TYPE_INHIBIT)) ) 
		{	
			ptr = ptr + strlen(DISPLAY_TYPE_INHIBIT);

			token = strtok(ptr, seps);
			if(token==NULL) 
			{
				fclose(fp);
				return 0;
			}
			int con_id = (int)atoi(token);
			if(con_id != m_nCurrentSectionNo)
				continue;

			token = strtok(NULL, seps);
			if(token==NULL) 
			{
				fclose(fp);
				return 0;
			}
			int numofTypes = (int)atoi(token);

			m_InhibitStatTrain.nTypeCount = 0;
			for(int j= 0;j<numofTypes&&j<sizeof(m_InhibitStatTrain.trainType)/sizeof(m_InhibitStatTrain.trainType[0]);j++)
			{
				token = strtok(NULL, seps);
				if(token==NULL) 
				{
					fclose(fp);
					return 0;
				}
				m_InhibitStatTrain.trainType[m_InhibitStatTrain.nTypeCount] = (int)atoi(token);
				m_InhibitStatTrain.nTypeCount++;
			}
		}
	}

	fclose(fp);
	return TRUE;
}

int CPlotDoc::GetDispIndex(ushort nStationNo, ushort nRouteSetNo,ushort *rcCode)
{
	for(int i=0;i < m_nDispDistanceNum;i++)
	{
		if(m_sDispDistance[i].nStationNo == nStationNo &&
			m_sDispDistance[i].nRouteSetNo == nRouteSetNo)
		{
			if(rcCode)
				*rcCode = 1;
			return i;
		}
		if(m_sDispDistance[i].nStationNo == nStationNo &&
			m_sDispDistance[i].nSecondRouteSetNo == nRouteSetNo)
		{
			if(rcCode)
				*rcCode = 2;
			return i;
		}
	}
	return -1;
}

BOOL CPlotDoc::PtInRects(RECT rects, POINT pt)
{
	CPoint point1(rects.left,rects.top);
	CPoint point2(rects.right,rects.bottom);

	//for test Rect;
	CRect Rects(point1,point2); 
	if(Rects.PtInRect(pt))
	{    
		return TRUE; 
	}

	//compare with point2;
	if( (pt.x == point2.x && pt.y >=point1.y && pt.y <= point2.y ) || 
		(pt.y == point2.y && pt.x >= point1.x && pt.x <= point2.x )    )
	{   
		return TRUE;
	}
	//no 
	return FALSE;
}


BOOL CPlotDoc::IsElectricTrain( CTG_SingleTrain *single)
{
	BOOL bValue = FALSE;
	if(single == NULL)
		return bValue;


	BYTE engine_type;
	for(int i=0; i<single->GetEngineList()->GetRecordCount(); i++)
	{
		engine_type  = single->GetEngineList()->m_listRecords[i].GetEngineType();
		if(IsAttachEngineExt(engine_type))
			continue;  //附挂机车
		///"牵引机车";			
		if(IsDieselEngineExt(engine_type))
			bValue = FALSE;  //内燃机车
		else 
		{
			bValue = TRUE;  //电力机车
			break;
		}

	}
	return bValue;
}
// 兰州模式机车画法 G01
void CPlotDoc::DrawLanzhouMarkOfTractionEngine(CDC *pCDC, CPoint pBasic, CSize trainidSize, CTG_SingleTrain *single,int nDirect,int schd_type,int workthread)
{
	//是否显示机车
	if(!m_bShowTractionEngine)
		return;

	CPoint pStart1; //机车号的起始点，文字绘制为TA_BOTTOM|TA_CENTER
	CPoint pStart2 ; //第二个机车号的位置
	int bNeedOutputEngine;//是否有甩挂作业 0：无 1：有  added by zhaoliang for lanzou Engine attach_unattach style
	int minute_width=0;
	CPoint EnginePos;
	CPoint EngineLocation;
	nDirect = GetTrainRecDirect(single, 0);//added by zl 20130206

	//最多显示两个牵引机车
	int tractionCount = 0;
	CTG_TrainEngineList *train_enginelist;
	train_enginelist = single->GetEngineList();

	if( train_enginelist->GetRecordCount() > MAX_ENGINEER_NUM)
		return;
	
	//added by zhaoliang for lanzou Engine attach_unattach style
	//////////////////////////////////////////////////////////////////////////
	for (int sta=0;sta<=single->GetTrainRecordCount()-1;sta++)
	{
		bNeedOutputEngine=0;
		CString str_engineIds,str_attach,str_unstatch,str_engine;
		for (int i = 0; i < train_enginelist->GetRecordCount(); i++)//对机车进行遍历
		{
			if (train_enginelist->m_listRecords[i].m_data.attach_sta == sta||train_enginelist->m_listRecords[i].m_data.unattach_sta == sta)//该站有甩挂机车作业
			{
				bNeedOutputEngine = TRUE;
				break;
			}
		}
		if (bNeedOutputEngine)//如果该站有甩挂
		{

			for (int i = 0; i < train_enginelist->GetRecordCount(); i++)//对机车进行遍历
			{
				CString tmp  = train_enginelist->m_listRecords[i].m_data.engine_type;
				CString name = train_enginelist->m_listRecords[i].GetEngineName();
				/*if(name.Find("SS7E") >= 0)
					Sleep(0);*/
				if ((sta>=train_enginelist->m_listRecords[i].m_data.attach_sta && sta<train_enginelist->m_listRecords[i].m_data.unattach_sta)||
					(sta==train_enginelist->m_listRecords[i].m_data.attach_sta&&sta==train_enginelist->m_listRecords[i].m_data.unattach_sta&&sta==0))
					//如果该站存在机车,绘制贴线机车号
				{
					if(name.IsEmpty())
					{
						if (str_engineIds.IsEmpty())
							str_engineIds =  tmp;  
						else
							str_engineIds = str_engineIds + "/" + tmp;  
					}
					else
					{
						if (str_engineIds.IsEmpty())
							//str_engineIds =  name + tmp.Right(tmp.GetLength()-3);   
							str_engineIds =  name;  //20100619：改正机车类型为四位时，将类型的最后一位赋给机车号最后一位的错误，如SS7E-00018E,应该为SS7E-00018才对，下面的同样处理。
						else
							str_engineIds = str_engineIds + "/" + name ;   
					}
				}
				if (train_enginelist->m_listRecords[i].m_data.attach_sta == sta&&sta!=0)//挂车 绘制△+机车号
				{
					if (name.IsEmpty())
					{
						if (str_attach.IsEmpty())
							str_attach = "△+" + tmp;  
						else
							str_attach = str_attach + "/" + tmp;  
					}
					else
					{
						if (str_attach.IsEmpty())
							str_attach = "△+" + name ;   
						else
							str_attach = str_attach + "/" + name ;
					}
				}
				if(train_enginelist->m_listRecords[i].m_data.unattach_sta == sta&&sta!=0)//甩车 绘制△-机车号
				{
					if (name.IsEmpty())
					{
						if (str_unstatch.IsEmpty())
							str_unstatch = "△-" + tmp;  
						else
							str_unstatch = str_unstatch + "/" + tmp;  
					}
					else
					{
						if (str_unstatch.IsEmpty())
							str_unstatch = "△-" + name ;   
						else
							str_unstatch = str_unstatch + "/" + name ;   
					}
				}
				str_engine=str_attach+str_unstatch;
			}//end 对机车进行遍历
			//绘制标识
			CFont font,font1;
			LOGFONT stFont,stFont1;
			CFont * pOldFont;
			int oldColor = CLR_RED;
			memset(&stFont,0,sizeof(stFont));
			stFont.lfHeight = MulDiv(m_font.engine_id,-pCDC->GetDeviceCaps(LOGPIXELSY),72);//10
			stFont.lfWeight = FW_THIN; 
			stFont.lfClipPrecision = CLIP_LH_ANGLES;
			long time_arrival_rec,date_arrival_rec;
            long time_depart_rec,date_depart_rec;
			strcpy(stFont.lfFaceName,"Arial Narrow");

			memset(&stFont1,0,sizeof(stFont1));
			stFont1.lfHeight = MulDiv(m_font.engine_id,-pCDC->GetDeviceCaps(LOGPIXELSY),72);//10
			stFont1.lfWeight = FW_THIN; 
			stFont1.lfClipPrecision = CLIP_LH_ANGLES;
			strcpy(stFont1.lfFaceName,"Arial Narrow");


			CSize sz = pCDC->GetTextExtent("11");
			int oldAligh = 0;
			CTG_TrainRecord *rec=NULL;
			rec = single->GetTrainRecord(sta);
			if(rec == NULL)
				return ;
			int if_need = 1;
			int y1_depart,x1_depart,y1_arrival,x1_arrival;

			int nPageWidth =m_fPixelPerMinute * m_nViewTimeRange*60;  
			if (rec->GetDepartTGIndex() != rec->GetArriveTGIndex())
			{
 				//第一个节点 开点
				time_depart_rec = minute_time(rec->m_data.depart,&date_depart_rec);
				minute_width = GetMinuteWidthToGraphStartTime(time_depart_rec/60,date_depart_rec,schd_type);
				y1_depart = GetStationVPos(single,rec,PRIMARY_TRACK,EXIT_POSITION);
				x1_depart = minute_width * m_fPixelPerMinute;
 				//第一个节点 到点
				time_arrival_rec = minute_time(rec->m_data.arrive,&date_arrival_rec);
				minute_width = GetMinuteWidthToGraphStartTime(time_arrival_rec/60,date_arrival_rec,schd_type);
				y1_arrival = GetStationVPos(single,rec,PRIMARY_TRACK,ENTRY_POSITION);
				x1_arrival = minute_width * m_fPixelPerMinute;
				if_need = -1;
			}
			else
			{
 				//第一个节点 开点
				time_depart_rec = minute_time(rec->m_data.depart,&date_depart_rec);
				minute_width = GetMinuteWidthToGraphStartTime(time_depart_rec/60,date_depart_rec,schd_type);
				y1_depart = GetStationVPos(single,rec,PRIMARY_TRACK,ENTRY_POSITION);
				x1_depart = minute_width * m_fPixelPerMinute;
 				//第一个节点 到点
				time_arrival_rec = minute_time(rec->m_data.arrive,&date_arrival_rec);
				minute_width = GetMinuteWidthToGraphStartTime(time_arrival_rec/60,date_arrival_rec,schd_type);
				y1_arrival = GetStationVPos(single,rec,PRIMARY_TRACK,ENTRY_POSITION);
				x1_arrival = minute_width * m_fPixelPerMinute;
			}

            
			double fDelta;
			double fDelcon = 0;
			double fDelsin = 1;
            //added by zhaoliang
			int offset_x, offset_y, height, width;
			if(!GetOffsetAndSize(AREA_TYPE::TRAIN, offset_x, offset_y, height, width))
				return;

            if (x1_depart>=offset_x-150 && x1_depart < nPageWidth)
            {
				if (single->GetRecordList()->GetRecordCount()>1)
				{
					rec = single->GetTrainRecord(sta+1);
					if(rec == NULL)
						return ;
					//获取到点
					time_arrival_rec = minute_time(rec->m_data.arrive,&date_arrival_rec);
					minute_width = GetMinuteWidthToGraphStartTime(time_arrival_rec/60,date_arrival_rec,schd_type);
					int y2_arrival = GetStationVPos(single,rec,PRIMARY_TRACK,ENTRY_POSITION);
					int x2_arrival  = minute_width * m_fPixelPerMinute;

					fDelta = (double)(y2_arrival - y1_depart) / (double)(x2_arrival -x1_depart);
					fDelcon = sqrt(1/(1+fDelta*fDelta));
					fDelsin = sqrt(1-fDelcon);

					if(nDirect == DOWN_DIRECT)//计算斜率
					{
						if(x1_depart != x2_arrival)
						{

							if(y1_depart > y2_arrival )
							{
								stFont.lfEscapement=(fDelta==0?0:(-10*atan(1/fDelta)*180/3.14));
							}
							else
							{
								stFont.lfEscapement=-10*atan(fDelta)*180/3.14;
							}
						}
					}
					else
					{
						if(x1_depart != x2_arrival)
						{

							if(y1_depart > y2_arrival )
							{
								stFont.lfEscapement=(fDelta==0?0:(900+10*atan(1/fDelta)*180/3.14));
							}
							else
							{
								stFont.lfEscapement=900+10*atan(fDelta)*180/3.14;
							}
						}
					}
				} 
				oldColor = pCDC->SetTextColor(CLR_RED);
				EnginePos.x=x1_depart;
				EnginePos.y=y1_depart;
				EnginePos.Offset (x_offset_print,y_offset_print);

				EngineLocation.x = x1_arrival;
				EngineLocation.y = y1_arrival;
				EngineLocation.Offset (x_offset_print,y_offset_print);//带三角的

				CTG_TrainRecord *curr_rec = single->GetTrainRecord(sta);
				if (nDirect == DOWN_DIRECT )
				{
					oldAligh = pCDC->SetTextAlign(TA_LEFT|TA_BASELINE);
					font.CreateFontIndirect(&stFont);
					pOldFont = pCDC->SelectObject(&font);
					pCDC->TextOut(EnginePos.x+8*fDelsin, EnginePos.y-8*fDelcon,str_engineIds);

					font1.CreateFontIndirect(&stFont1);
					pCDC->SetTextAlign(TA_LEFT|TA_BOTTOM);
					pCDC->SelectObject(&font1);
					if (if_need < 0)
					{
						if (gpcdef.IsLocoNoDisplay(m_nCurrentSectionNo,	curr_rec->m_data.station, curr_rec->GetArriveTGIndex(), curr_rec->GetDepartTGIndex(), nDirect, 1))
							pCDC->TextOut(EngineLocation.x, EngineLocation.y-8,str_unstatch);//三角标记
						if (gpcdef.IsLocoNoDisplay(m_nCurrentSectionNo, curr_rec->m_data.station, curr_rec->GetArriveTGIndex(), curr_rec->GetDepartTGIndex(), nDirect, 0))
							pCDC->TextOut(EngineLocation.x, EnginePos.y-8,str_attach);//三角标记
					}
					else
						pCDC->TextOut(EngineLocation.x, EngineLocation.y-8,str_engine);//三角标记
				} 
				else
				{
					oldAligh = pCDC->SetTextAlign(TA_LEFT|TA_BASELINE);
					font.CreateFontIndirect(&stFont);
					pOldFont = pCDC->SelectObject(&font);
					pCDC->TextOut(EnginePos.x-8*fDelsin, EnginePos.y-8*fDelcon,str_engineIds);
					font.DeleteObject();

					stFont.lfEscapement=0;
					font.CreateFontIndirect(&stFont);
					pOldFont = pCDC->SelectObject(&font);
					
					font1.CreateFontIndirect(&stFont1);
					pCDC->SetTextAlign(TA_LEFT|TA_BOTTOM);
					pCDC->SelectObject(&font1);
					if (if_need < 0)
					{
						if (gpcdef.IsLocoNoDisplay(m_nCurrentSectionNo,curr_rec->m_data.station, curr_rec->GetArriveTGIndex(), curr_rec->GetDepartTGIndex(), nDirect, 1))
							pCDC->TextOut(EngineLocation.x, EngineLocation.y+8,str_unstatch);//三角标记
						if (gpcdef.IsLocoNoDisplay(m_nCurrentSectionNo,curr_rec->m_data.station, curr_rec->GetArriveTGIndex(), curr_rec->GetDepartTGIndex(), nDirect, 0))
							pCDC->TextOut(EngineLocation.x, EnginePos.y+8,str_attach);//三角标记
					}
					else
						pCDC->TextOut(EngineLocation.x, EngineLocation.y+8,str_engine);//三角标记
				}
				pCDC->SelectObject(pOldFont);
				pCDC->SetTextAlign(oldAligh);
				pCDC->SetTextColor(oldColor);
				font.DeleteObject();
            }
			else if( x1_depart < offset_x-150 )//暂时将此边界作为判断过表车的依据

			{
				oldColor = pCDC->SetTextColor(CLR_RED);
				EnginePos.x=pBasic.x;
				EnginePos.y=pBasic.y;
                CSize EngineidSize = pCDC->GetTextExtent(str_engineIds);
				EnginePos.x = EnginePos.x - EngineidSize.cx/2;
				if (EnginePos.x < 0)
					EnginePos.x = 0;
				
				if (nDirect == DOWN_DIRECT )
				{
					oldAligh = pCDC->SetTextAlign(TA_LEFT|TA_BASELINE);
					font.CreateFontIndirect(&stFont);
					pOldFont = pCDC->SelectObject(&font);
					pCDC->TextOut(EnginePos.x, EnginePos.y+EngineidSize.cy*2,str_engineIds);
					font.DeleteObject();
				} 
				else
				{
					oldAligh = pCDC->SetTextAlign(TA_LEFT|TA_BASELINE);
					font.CreateFontIndirect(&stFont);
					pOldFont = pCDC->SelectObject(&font);
					pCDC->TextOut(EnginePos.x, EnginePos.y+EngineidSize.cy,str_engineIds);
					font.DeleteObject();
				}
				pCDC->SelectObject(pOldFont);
				pCDC->SetTextAlign(oldAligh);
				pCDC->SetTextColor(oldColor);
				font.DeleteObject();

            }
			
		}

	}
}

void CPlotDoc::DrawMarkOfTractionEngine(CDC *pCDC, CPoint pBasic, CSize trainidSize, CTG_SingleTrain *single,int nDirect,int schd_type,int rec_index)
{
	//是否显示机车
	if(!m_bShowTractionEngine)
		return;

	CPoint pStart1; //机车号的起始点，文字绘制为TA_BOTTOM|TA_CENTER
	CPoint pStart2 ; //第二个机车号的位置

	//原来的显示牵引机车号的标志该为显示所有机车号
	POINT pPos[MAX_ENGINEER_NUM];//每个机车号的位置	
	CString engineIds[MAX_ENGINEER_NUM];//机车号

	//最多显示两个牵引机车
	int tractionCount = 0;
	CTG_TrainEngineList *train_enginelist;
	train_enginelist = single->GetEngineList();

	if( train_enginelist->GetRecordCount() > MAX_ENGINEER_NUM)
		return;

	for(int i=0; i<train_enginelist->GetRecordCount(); i++)
	{
		CString trainEngine = train_enginelist->m_listRecords[i].m_data.engine_type;
		CString engineId= train_enginelist->m_listRecords[i].m_data.engine_id;

		if(engineId.GetLength()>3)
		{
			if(engineId.IsEmpty())
				engineIds[tractionCount] = engineId;
			else
				engineIds[tractionCount] = trainEngine +"/"+engineId.Right(engineId.GetLength()-3);
		}
		else
		{
			if(engineId.IsEmpty())
				continue;
			else
				engineIds[tractionCount] = engineId;
		}

		tractionCount++;
		if (tractionCount>=8)
			break;
	}

	//没有机车
	if(tractionCount<=0)
		return;

	CFont font;
	LOGFONT stFont;
	CFont * pOldFont;

	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(m_font.engine_id,-pCDC->GetDeviceCaps(LOGPIXELSY),72);

	stFont.lfWeight = FW_NORMAL;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	strcpy(stFont.lfFaceName,"Times New Roman");
	font.CreateFontIndirect(&stFont);
	pOldFont = pCDC->SelectObject(&font);
	//换了字体，重新设置trainidSize
	CSize newSize = trainidSize;
	trainidSize = pCDC->GetTextExtent(single->GetTrainID());
	trainidSize.cx = newSize.cx;

	pStart1 = pBasic;
	pStart2 = pBasic;

	pStart1.x = pStart1.x + trainidSize.cx/2;
	if(nDirect == UP_MARK)
	{			
		pStart1.y -= trainidSize.cy;
	}
	else //DOWN_MARK
	{
		pStart1.y += trainidSize.cy;
	}
	pStart2 = pStart1;

	//计算每个机车号的位置，如果多个机车，那么不考虑选择了中间位置这个选项，中间位置作用同上方位置
	//机车号的默认位置是中间，必须考虑
	pPos[0] = pStart1;

	for(int j=1; j<tractionCount; j++)
	{
		pPos[j] = pPos[j-1];

		if(nDirect == UP_MARK)
		{
			pPos[j].y = pPos[j-1].y - trainidSize.cy;
		}
		else
		{
			pPos[j].y = pPos[j-1].y + trainidSize.cy;

		}
	}

	int oldAligh = 0;
	int oldColor = CLR_RED;
	if(nDirect == UP_MARK)
		oldAligh = pCDC->SetTextAlign(TA_BOTTOM|TA_CENTER);
	else
		oldAligh = pCDC->SetTextAlign(TA_TOP|TA_CENTER);
	oldColor = pCDC->SetTextColor(CLR_RED);

	for(j=0; j<tractionCount; j++)
	{
		pCDC->TextOut(pPos[j].x,pPos[j].y,engineIds[j]);

	}

	pCDC->SelectObject(pOldFont);
	pCDC->SetTextAlign(oldAligh);
	pCDC->SetTextColor(oldColor);
	font.DeleteObject();
	return;
}

int CPlotDoc::JudeTrainDirectByRecord(CTG_SingleTrain *single, CTG_TrainRecord *rec,int flag)
{
	CTG_TrainRecord* next = single->GetTrainRecord(rec->rec_index+1);
	if(NULL != next)
	{
		int dir = UP_DIRECT; 
		if(rec->GetDepartTGIndex() > next->GetArriveTGIndex())
		{
			dir = UP_DIRECT;
		}
		else
		{
			dir = DOWN_DIRECT;
		}

		return dir;
	}

	if( rec->m_data.entry == (ushort) -1 &&  rec->m_data.exit ==(ushort)-1 ){
		sysprint(0,PMTG.ThreadId,"judge train dir error!, station %d,entry %d, exit %d",rec->m_data.station,rec->m_data.entry,rec->m_data.exit);
		return -1;
	}

	if(rec->m_data.entry == (ushort)-1)
		flag = EXIT_POSITION;
	if(rec->m_data.exit == (short)-1)
		flag = ENTRY_POSITION;

	int entry = rec->m_data.entry;
	int exit = rec->m_data.exit;

	ushort line,dir;
	if(flag == ENTRY_POSITION){
		GetTGBasicParam()->ConvertStationEntryToAbsnum(rec->m_data.station,
			entry,
			&line,
			&dir); 
	}
	else{
		GetTGBasicParam()->ConvertStationEntryToAbsnum(rec->m_data.station,
			exit,
			&line,
			&dir);
	}

	int i,j;
	bool bFoundLine = FALSE;
	Pstruct _node_info *node_info;

	dir = DOWN_DIRECT;
	for(i=0;i < m_nDispDistanceNum;i++)
	{
		node_info = &m_sDispDistance[i].distance.node_info;
		if(rec->m_data.station != node_info->station_number)
			continue;
		//looking for up line 
		for(j= 0;j <  node_info->up_line_count;j++)
		{
			if(line ==  node_info->up_line_list[j])
			{
				if(flag == ENTRY_POSITION)
					dir = DOWN_DIRECT; 
				else
					dir = UP_DIRECT;
				bFoundLine = TRUE;                 					 
			}
		}
		//looking for down line
		for(j= 0;j <  node_info->down_line_count;j++)
		{
			if(line ==  node_info->down_line_list[j])
			{
				if(flag == ENTRY_POSITION)
					dir = UP_DIRECT; 
				else
					dir = DOWN_DIRECT;
				bFoundLine = TRUE;                  
			}
		}
		if(bFoundLine)
			break;
	}

	if(bFoundLine == FALSE)
	{
		sysprint(0,PMTG.ThreadId,"judge train dir error!, station %d,entry %d, exit %d",rec->m_data.station,rec->m_data.entry,rec->m_data.exit);
		return -1;
	}

	return (int)dir;
}



int CPlotDoc::GetModeOfStationShowSide(int DistanceIndex, int schd_type)
{
	if(DistanceIndex<0 || DistanceIndex>=m_nDispDistanceNum)
		return FALSE;

	if(m_sDispDistance[DistanceIndex].distance.node_info.show_sides)
		return TRUE;
	else
		return FALSE;
}


BOOL CPlotDoc::JudeNodesConnected(ushort cur_node_index,ushort pre_node_index,int schd_type)
{
	if(cur_node_index >= m_nDispDistanceNum || pre_node_index>= m_nDispDistanceNum)
		return FALSE;

	int i,j;
	Pstruct _node_info *cur_node_info,*pre_node_info;    
	cur_node_info = &m_sDispDistance[cur_node_index].distance.node_info;
	pre_node_info = &m_sDispDistance[pre_node_index].distance.node_info;
	if(cur_node_info->station_number == pre_node_info->station_number )
		return TRUE;

	for(i=0;i<cur_node_info->up_line_count;i++)
	{
		for(j= 0;j<pre_node_info->down_line_count ;j++)
		{
			if(cur_node_info->up_line_list [i] == pre_node_info->down_line_list [j])
				return TRUE;
		}
	}
	for(i=0;i<cur_node_info->down_line_count;i++)
	{
		for(j= 0;j<pre_node_info->up_line_count ;j++)
		{
			if(cur_node_info->down_line_list [i] == pre_node_info->up_line_list [j])
				return TRUE;
		}
	}
	return FALSE;	
}

int CPlotDoc::GetNodeIndexByAbs(int curNodeIndex, int abs_num, int dir, int schd_type)
{
	int ret_node_index = -1;
	Pstruct _node_info *node_info_current,*node_info_next,*node_info_pre;    

	node_info_current = &m_sDispDistance[curNodeIndex].distance.node_info;

	if(dir == UP_DIRECT)//上行车
	{
		int pre_node_index = curNodeIndex - 1;
		while(pre_node_index >= 0)
		{
			node_info_pre = &m_sDispDistance[pre_node_index].distance.node_info;
			for(int i=0; i<node_info_pre->down_line_count; i++)
			{
				if(node_info_pre->down_line_list[i] == abs_num)
					break;
			}

			if(i<node_info_pre->down_line_count)
			{
				ret_node_index = pre_node_index;
				break;
			}
			pre_node_index--;
		}

	}
	else //下行车
	{
		int next_node_index = curNodeIndex + 1;
		while(next_node_index < m_nDispDistanceNum)
		{
			node_info_next = &m_sDispDistance[next_node_index].distance.node_info;
			for(int i=0; i<node_info_next->up_line_count; i++)
			{
				if(node_info_next->up_line_list[i] == abs_num)
					break;
			}

			if(i<node_info_next->up_line_count)
			{
				ret_node_index = next_node_index;
				break;
			}
			next_node_index++;
		}

	}

	return ret_node_index;

}

bool CPlotDoc::IsSameStation(CTG_SingleTrain * single,int rec_index,int station)
{
	if(rec_index <0 || single == NULL || station <=0 )
		return FALSE;	
	int usRecordCount = single->GetTrainRecordCount();       //记录总数
	if(rec_index > usRecordCount-1)
		return FALSE;
	CTG_TrainRecord *rec;
	//查看配置中的某条径路是否和此车径路相符合
	rec = single->GetTrainRecord(rec_index);
	if(rec->m_data.station == station)
		return TRUE;
	else
		return FALSE;
}

int CPlotDoc::JudgeMultiNodeIndex(CTG_SingleTrain * single,CTG_TrainRecord* rec,int& index,ushort usFlag,int section,int shcd_type)
{
	index = -1;
	if(rec->m_data.entry==0)rec->m_data.entry=-1;
	if(rec->m_data.exit==0)rec->m_data.exit=-1;
	if( rec->m_data.entry == (ushort) -1 &&  rec->m_data.exit ==(ushort)-1 ){
		return ERR_NOT_FIND;
	}
	int i,j;
	bool bFoundLine = FALSE;
	Pstruct _node_info *node_info;

	if( rec->m_data.entry != (ushort) -1&&rec->m_data.exit !=(ushort)-1 ){
		ushort lineEntry,dirEntry;
		ushort lineExit,dirExit;
		bool bEntryFound = FALSE;
		bool bExitFound = FALSE;

		GetTGBasicParam()->ConvertStationEntryToAbsnum(rec->m_data.station,
			rec->m_data.entry,
			&lineEntry,
			&dirEntry); 
		GetTGBasicParam()->ConvertStationEntryToAbsnum(rec->m_data.station,
			rec->m_data.exit,
			&lineExit,
			&dirExit);

		bool myFlag = FALSE;
		//从前往后找
		for(i=0;i < m_nDispDistanceNum;i++)
		{
			node_info = &m_sDispDistance[i].distance.node_info;
			if(rec->m_data.station != node_info->station_number)
				continue;
			bEntryFound = FALSE;                 					 
			bExitFound = FALSE;                 					 

			//looking for up line 
			for(j= 0;j <  node_info->up_line_count;j++)
			{
				if(lineEntry ==  node_info->up_line_list[j])
				{
					bEntryFound = TRUE;
					break;
				}

			}
			//looking for down line
			for(j= 0;j <  node_info->down_line_count;j++){
				if(lineEntry ==  node_info->down_line_list[j])
				{
					bEntryFound = TRUE;                        					 
					break;
				}
			}
			//looking for up line 
			for(j= 0;j <  node_info->up_line_count;j++){
				if(lineExit ==  node_info->up_line_list[j])
				{
					bExitFound = TRUE;                 					 
					break;
				}

			}
			//looking for down line
			for(j= 0;j <  node_info->down_line_count;j++){
				if(lineExit ==  node_info->down_line_list[j])
				{
					bExitFound = TRUE;                        					 
					break;
				}
			}

			if(bEntryFound && bExitFound){
				index = i;
				myFlag = TRUE;
			}
		}

		///从后往前找
		BOOL myFlag1=FALSE;
		int index1=-1;
		for(i=m_nDispDistanceNum-1;i >=0 ;i--)
		{
			node_info = &m_sDispDistance[i].distance.node_info;
			if(rec->m_data.station != node_info->station_number)
				continue;
			bEntryFound = FALSE;                 					 
			bExitFound = FALSE;                 					 

			//looking for up line 
			for(j= 0;j <  node_info->up_line_count;j++)
			{
				if(lineEntry ==  node_info->up_line_list[j])
				{
					bEntryFound = TRUE;
					break;
				}

			}
			//looking for down line
			for(j= 0;j <  node_info->down_line_count;j++)
			{
				if(lineEntry ==  node_info->down_line_list[j])
				{
					bEntryFound = TRUE;                        					 
					break;
				}
			}
			//looking for up line 
			for(j= 0;j <  node_info->up_line_count;j++){
				if(lineExit ==  node_info->up_line_list[j])
				{
					bExitFound = TRUE;                 					 
					break;
				}

			}
			//looking for down line
			for(j= 0;j <  node_info->down_line_count;j++){
				if(lineExit ==  node_info->down_line_list[j])
				{
					bExitFound = TRUE;                        					 
					break;
				}
			}

			if(bEntryFound && bExitFound){
				index1 = i;
				myFlag1 = TRUE;
			}
		}
		if((index!=index1)&&(myFlag==TRUE)&&(myFlag1==TRUE)){
			return FIND_TWICE;	
		}else if((index==index1)&&(myFlag==TRUE)&&(myFlag1==TRUE)){
			return FIND_ONCE;
		}
	}

	int entry = (rec->m_data.entry!=(ushort)-1) ? rec->m_data.entry : rec->m_data.exit;
	int exit = (rec->m_data.exit !=(ushort)-1) ? rec->m_data.exit : rec->m_data.entry;


	ushort line,dir;
	if(usFlag == ENTRY_POSITION){
		GetTGBasicParam()->ConvertStationEntryToAbsnum(rec->m_data.station,
			entry,
			&line,
			&dir); 
	}
	else{
		GetTGBasicParam()->ConvertStationEntryToAbsnum(rec->m_data.station,
			exit,
			&line,
			&dir);
	}

	int myIndex = -1;
	bFoundLine = FALSE;
	for(i=0;i < m_nDispDistanceNum;i++)
	{
		node_info = &m_sDispDistance[i].distance.node_info;
		if(rec->m_data.station != node_info->station_number)
			continue;

		//looking for up line 
		for(j= 0;j <  node_info->up_line_count;j++){
			if(line ==  node_info->up_line_list[j])
				bFoundLine = TRUE;                 					 
		}
		//looking for down line
		for(j= 0;j <  node_info->down_line_count;j++){
			if(line ==  node_info->down_line_list[j])
				bFoundLine = TRUE;                        					 
		}
		if(bFoundLine){
			myIndex = i;//break;
			bFoundLine = FALSE;
		}
	}

	int myIndex1 = -1;
	BOOL bFoundLine1 = FALSE;
	for(i=m_nDispDistanceNum-1;i >=0;i--){
		node_info = &m_sDispDistance[i].distance.node_info;
		if(rec->m_data.station != node_info->station_number)
			continue;

		//looking for up line 
		for(j= 0;j <  node_info->up_line_count;j++){
			if(line ==  node_info->up_line_list[j])
				bFoundLine1 = TRUE;                 					 
		}
		//looking for down line
		for(j= 0;j <  node_info->down_line_count;j++){
			if(line ==  node_info->down_line_list[j])
				bFoundLine1 = TRUE;                        					 
		}
		if(bFoundLine1){
			myIndex1 = i;//break;
			bFoundLine1 = FALSE;
		}
	}
	if((myIndex>=0)&&(myIndex1>=0)&&(myIndex1!=myIndex)){
		return FIND_TWICE;	
	}else{
		if((myIndex>=0)&&(myIndex1>=0)&&(myIndex1==myIndex)){
			index = myIndex;
			return FIND_ONCE;	
		}
	}
	return ERR_NOT_FIND;      
}

int CPlotDoc::GetNodeSubSectionByIndex(int index,int schd_type)
{
	if(index>=0&&index<m_nDispDistanceNum)
		return m_sDispDistance[index].nRouteSetNo;
	else
		return -1;
}

int CPlotDoc::GetNodeIndexBySubSection(int station,int subsection,int schd_type)
{
	for(int i=0;i<m_nDispDistanceNum;i++)
	{
		if(m_sDispDistance[i].nRouteSetNo==subsection&&m_sDispDistance[i].nStationNo==station)
			return i;
	}
	return -1;
}

int CPlotDoc::JudgeMultiNodeIndexbySeq(CTG_SingleTrain * single,CTG_TrainRecord* rec,CList<int,int>& list,CTG_TrainRecord* _start_rec,CTG_TrainRecord* _end_rec,int shcd_type)
{
	if(_start_rec==NULL||_end_rec==NULL){
		return ERR_NOT_FIND;
	}
	
	//forward sequence search
	CTG_TrainRecord *temp=_start_rec;
	int cur_node_sub_index=-1;
	for(int i=0;i<m_nDispDistanceNum;i++){
		if(temp==NULL)
			return ERR_NOT_FIND;
		if(cur_node_sub_index!=m_sDispDistance[i].nRouteSetNo){
			cur_node_sub_index=m_sDispDistance[i].nRouteSetNo;
			temp=_start_rec;
		}
		if(temp->m_data.station == m_sDispDistance[i].distance.node_info.station_number){
			if((temp->rec_index==_end_rec->rec_index)&&(cur_node_sub_index!=-1)&&(list.Find(cur_node_sub_index)==NULL))
				list.AddTail(cur_node_sub_index);
			if(temp->rec_index!=_end_rec->rec_index)
			{
				if(temp->GetIndex() < single->GetTrainRecordCount()-1)
					temp=single->GetTrainRecord(temp->GetIndex()+1);
			}
		}
	}

	//reverse sequence search;
	temp=_start_rec;
	cur_node_sub_index=-1;
	for(i=m_nDispDistanceNum-1;i>=0;i--){
		if(temp==NULL)
			return ERR_NOT_FIND;
		if(cur_node_sub_index!=m_sDispDistance[i].nRouteSetNo){
			cur_node_sub_index=m_sDispDistance[i].nRouteSetNo;
			temp=_start_rec;
		}
		if(temp->m_data.station == m_sDispDistance[i].distance.node_info.station_number){
			if((temp->rec_index==_end_rec->rec_index)&&(cur_node_sub_index!=-1)&&(list.Find(cur_node_sub_index)==NULL))
				list.AddTail(cur_node_sub_index);
			if(temp->rec_index!=_end_rec->rec_index)
			{
				if(temp->GetIndex() < single->GetTrainRecordCount()-1)
					temp=single->GetTrainRecord(temp->GetIndex()+1);
			}
		}
	}
	if(list.IsEmpty())
		return ERR_NOT_FIND;
	else{
		return FIND_TWICE;
	}
}

int CPlotDoc::GetAdditionalTrainInfoIndex(TRAIN_INDEX trainindex)
{
	int nSchdTrainNum = m_vecSchdTrains.size();
	for(int i = 0; i<nSchdTrainNum; i++)
	{
		if(train_additional_info[i].m_nTrainIndexforCompare == trainindex)
		{
			return i;
		}
	}
	return -1;
}

int CPlotDoc::GetIncreaseLayer(CTG_SingleTrain* single,CDC* pCDC, int schd_type)
{	
	static CTG_SingleTrain* orderedList[1024];
	int orderedCount = 0;
	int trainCount = m_vecSchdTrains.size();
	int cursection = 0;

	switch(schd_type)
	{
	case WORK_SCHEDULE:	
	case SHIFT_SCHEDULE:
		cursection = m_nCurrentSectionNo;
		break;
	default:
		cursection = m_nCurrentSectionNo;
		break;

	}
	int tem_index = GetAdditionalTrainInfoIndex(single->GetTrainIndex());
	if(tem_index == -1)
		return 1;
	int retlayer = train_additional_info[tem_index].m_nIncreaseLayer;

	char otherDigit[2],selfDigit[2];
	int otherInt,selfInt;
	CTG_TrainRecord*  firstrec = NULL;
	CTG_TrainRecord*  rec = NULL;
	int i,j;

	//被判断的列车有问题
	firstrec = single->GetTrainRecord(0);
	if(firstrec == NULL)
		return retlayer;

	memset(otherDigit,0x00,sizeof(otherDigit));
	memset(selfDigit,0x00,sizeof(selfDigit));

	//获得当前车次的最后一个数字，根据数字来判断上下行
	int m = 0;	
	int realNum = 0;
	j = strlen(single->GetTrainID());
	for(m=0; m<j; m++)
	{	
		realNum = j - m -1;
		selfDigit[0] = single->GetTrainID()[realNum];
		
		if(selfDigit[0] >= 0x30 && selfDigit[0] <= 0x39)	
			break;
	}
	if(m>=j)
		return retlayer;
	selfDigit[1] = '\0';

	//获得当前列车的nodeindex
	int otherindex,selfindex;
	ushort usflag = EXIT_POSITION;

	if(!GetNodeIndex(single,firstrec,selfindex,usflag))
		return retlayer;

	CTG_SingleTrain* curtrain = NULL;	

	for(i=0; i<trainCount; i++)
	{		
		curtrain = &m_vecSchdTrains[i];
		if(curtrain == NULL)
			continue;			

		//判断的是第一个点, 
		rec = curtrain->GetTrainRecord(0);
		if(rec == NULL)
			continue;

		//站号不一样，肯定序号不可能一样
		if(firstrec->m_data.station != rec->m_data.station)
			continue;

		//判断序号是否相同
		if(!GetNodeIndex(single,rec,otherindex,usflag))
			continue;

		//这种情况出现于一个区段有多个相同站的情况
		if(otherindex != selfindex)
			continue;

		//判断方向是否相同
		if(strlen(curtrain->GetTrainID()) == 0)
			continue;

		otherDigit[0] = '\0';

		//根据车次号判断上下行
		j = strlen(curtrain->GetTrainID());
		for(m=0; m<j; m++)
		{	
			realNum = j - m -1;
			otherDigit[0] = curtrain->GetTrainID()[realNum];
			
			if(otherDigit[0] >= 0x30 && otherDigit[0] <= 0x39)	
				break;
		}
		if(m>=j) continue;
		otherDigit[1] = '\0';
		if((otherDigit[0]>= 0x30 && otherDigit[0] <= 0x39) && ( selfDigit[0]>=0x30 && selfDigit[0]<= 0x39 ))
		{
			otherInt = atoi(otherDigit);
			selfInt = atoi(selfDigit);

			if(otherInt%2 != selfInt%2)//0上行，1下行，相同方向的车才要
				continue;			
		}
		else
			continue;

		orderedList[orderedCount]=curtrain;
		orderedCount++;		
	}


	//根据起始点时间从小到大排序
	long timeMin,timeMax;
	CTG_TrainRecord* rec1, *rec2;
	CTG_SingleTrain* tempTrain;
	for(i=0; i<orderedCount-1; i++)
	{
		rec1 = orderedList[i]->GetTrainRecord(0);
		if(rec1 == NULL)
			continue;
		timeMin = rec1->m_data.arrive;
		for(j=i+1; j<orderedCount; j++)
		{
			rec2 = orderedList[j]->GetTrainRecord(0);
			if(rec2 == NULL)
				continue;
			timeMax = rec2->m_data.arrive;
			if(timeMin > timeMax)
			{
				timeMin = timeMax;
				tempTrain = orderedList[i];
				orderedList[i] = orderedList[j];
				orderedList[j] = tempTrain;
			}

		}
	}

	//排序完成，查找要绘制的车属于上行还是下行，判断左右车是否和他相交

	int findIndex = -1;
	for(i=0; i<orderedCount; i++)
	{
		if(orderedList[i] == single)
		{
			findIndex = i;
			break;
		}
	}

	CTG_SingleTrain* left1 = NULL;//左边最靠近当前车的
	CTG_SingleTrain* left2 = NULL;//左边次靠近当前车的
	CTG_SingleTrain* right1 = NULL;//右边最靠近当前车的

	if(findIndex-1>=0)
	{
		left1 = orderedList[findIndex-1];
	}

	if(findIndex-2>=0)
	{
		left2 = orderedList[findIndex-2];
	}

	if(findIndex+1<orderedCount)
	{
		right1 = orderedList[findIndex+1];
	}

	CTG_TrainRecord* recleft1,*recleft2, *recright1;

	recleft1 = (left1 != NULL)?left1->GetTrainRecord(0):NULL;
	recleft2 = (left2 != NULL)?left2->GetTrainRecord(0):NULL;
	recright1 = (right1 != NULL)?right1->GetTrainRecord(0):NULL;


	//找到了左边的和右边的列车之后，判断是否相交来进行是否避让的调整
	int leftCollision1,leftCollision2,rightCollision1;
	//	leftCollision1 = leftCollision2 = rightCollision1 = 0;	

	int nodeDis = 0;

	if(recleft1 != NULL)
	{
		CSize sExtent = GetNodesExtent(recleft1,firstrec,left1,single,pCDC);
		long time_arrival1,date_arrival1;
		time_arrival1 = minute_time(firstrec->m_data.arrive,&date_arrival1);
		long time_arrival2,date_arrival2;
		time_arrival2 = minute_time(recleft1->m_data.arrive,&date_arrival2);

		nodeDis = ConvertTimetoXPos(time_arrival1,date_arrival1)
			- ConvertTimetoXPos(time_arrival2,date_arrival2);

		if(nodeDis < sExtent.cx)
			leftCollision1 = 1;
		else
			leftCollision1 = 0;
	}
	else 
	{
		leftCollision1 = 0;		
	}

	if(recleft2 != NULL)
	{
		CSize sExtent = GetNodesExtent(recleft2,firstrec,left2,single,pCDC);

		long time_arrival1,date_arrival1;
		time_arrival1 = minute_time(firstrec->m_data.arrive,&date_arrival1);
		long time_arrival2,date_arrival2;
		time_arrival2 = minute_time(recleft2->m_data.arrive,&date_arrival2);

		nodeDis = ConvertTimetoXPos(time_arrival1,date_arrival1)
			- ConvertTimetoXPos(time_arrival2,date_arrival2);

		if(nodeDis < sExtent.cx)
			leftCollision2 = 1;
		else
			leftCollision2 = 0;
	}
	else 
	{
		leftCollision2 = 0;		
	}

	if(recright1 != NULL)
	{
		CSize sExtent = GetNodesExtent(firstrec,recright1,single,right1,pCDC);

		long time_arrival1,date_arrival1;
		time_arrival1 = minute_time(recright1->m_data.arrive,&date_arrival1);
		long time_arrival2,date_arrival2;
		time_arrival2 = minute_time(firstrec->m_data.arrive,&date_arrival2);

		nodeDis = ConvertTimetoXPos(time_arrival1,date_arrival1)
			- ConvertTimetoXPos(time_arrival2,date_arrival2);

		if(nodeDis < sExtent.cx)
			rightCollision1 = 1;
		else
			rightCollision1 = 0;
	}
	else 
	{
		rightCollision1 = 0;		
	}


	//我们的判断机制为假设要考虑当前列车的前面两趟车和后面一趟车
	//在不影响后面的列车的情况下考虑三层避让

	int leftlayer1,leftlayer2,rightlayer1;
	int tem_index1, tem_index2, tem_index3;
	if(NULL != left1)
		tem_index1 = GetAdditionalTrainInfoIndex(left1->GetTrainIndex());
	else
		tem_index1 = -1;

	if(NULL != left2)
		tem_index2 = GetAdditionalTrainInfoIndex(left2->GetTrainIndex());
	else
		tem_index2 = -1;

	if(NULL != right1)
		tem_index3 = GetAdditionalTrainInfoIndex(right1->GetTrainIndex());
	else
		tem_index3 = -1;

	//int tem_index2 = GetAdditionalTrainInfoIndex(left2->GetTrainIndex());
	//int tem_index3 = GetAdditionalTrainInfoIndex(right1->GetTrainIndex());

	/*	leftlayer1 = (left1 != NULL)?left1->m_nIncreaseLayer:0;
	leftlayer2 = (left2 != NULL)?left2->m_nIncreaseLayer:0;
	rightlayer1 = (right1 != NULL)?right1->m_nIncreaseLayer:0;*/
	leftlayer1 = (left1 != NULL && tem_index1 != -1)?train_additional_info[tem_index1].m_nIncreaseLayer:0;
	leftlayer2 = (left2 != NULL && tem_index2 != -1)?train_additional_info[tem_index2].m_nIncreaseLayer:0;
	rightlayer1 = (right1 != NULL && tem_index3 != -1)?train_additional_info[tem_index3].m_nIncreaseLayer:0;
	//不影响右边的列车，最理想的情况
	if(!rightCollision1 || (rightCollision1 && leftCollision1 && leftlayer1 == rightlayer1))
	{
		if(leftCollision1)
		{
			if(leftCollision2 && leftlayer1!=leftlayer2)//和左边两个均相交
			{
				retlayer = (1+2+3) - leftlayer1 - leftlayer2;
			}
			else//只和最左边的相交
			{
				/*
				if(leftlayer1 == retlayer)
				{

				retlayer = (leftlayer1+1)%4;

				if(retlayer == 0)
				retlayer = 1;											

				}
				*/
				if(leftlayer1 >= 2)
				{
					retlayer = 1;
				}
				else
				{
					retlayer = 2;
				}
			}
		}
		else//和左右两边均不相交
		{
			//2005-01-23
			//以下处理为济南局的特殊情况
			/*
			if(leftlayer1==1)
			retlayer=2;
			else
			retlayer=1;
			*/

			//以下处理为正常处理方式
			retlayer = 1;
		}
	}
	else
	{
		if(leftCollision1)//和左边第一个相交并且左边第一个和右边第一个的避让层次不一样
		{			
			retlayer = (1+2+3) - leftlayer1 - rightlayer1;
		}
		else //只和最右边的相交
		{
			/*
			if(rightlayer1 == retlayer)
			{
			retlayer = (rightlayer1+1)%4;

			if(retlayer == 0)
			retlayer = 1;						
			}
			*/

			if(rightlayer1 >= 2)
			{
				retlayer = 1;
			}
			else
			{
				retlayer = 2;
			}
		}

	}
	train_additional_info[tem_index].m_nIncreaseLayer = retlayer;
	return retlayer;
}


int CPlotDoc::GetIncreaseLayer(CTG_SingleTrain* single,CPoint ptlOrig,int nDirect,int stationno,CDC* pCDC, int schd_type)
{
	static CTG_SingleTrain* orderedList[1024];
	int orderedCount = 0;

	int trainCount = m_vecSchdTrains.size();
	int cursection = m_nCurrentSectionNo;

	int m_cur_index=-1;int i=0;
	int tem_index = GetAdditionalTrainInfoIndex(single->GetTrainIndex());
	if(tem_index == -1)
		return 1;
	if(train_additional_info[tem_index].ptoftrainid_count<0||train_additional_info[tem_index].ptoftrainid_count>8)
		train_additional_info[tem_index].ptoftrainid_count=0;
	//首先选出存放记录的地方
	for(i=0;i<train_additional_info[tem_index].ptoftrainid_count;i++)
	{
		if(stationno==train_additional_info[tem_index].stPttoTrainid[i].stationno)
		{
			m_cur_index=i;
			break;
		}
	}
	if(m_cur_index==-1)
	{
		if(train_additional_info[tem_index].ptoftrainid_count<8)
		{
			m_cur_index=train_additional_info[tem_index].ptoftrainid_count;
			train_additional_info[tem_index].ptoftrainid_count++;
		}
		else
			return 1;
	}
	if(m_cur_index<0||m_cur_index>=8)return 1;
	train_additional_info[tem_index].stPttoTrainid[m_cur_index].stationno=stationno;
	CPoint ptlEnd=ptlOrig;
	//选出可能存在冲突的点
	CString strTrainId=CString(single->GetTrainID());
	int k = 0;
	for(; k < single->GetTrainRecordCount(); k++)
	{
		if(single->GetTrainRecord(k)->m_data.arrive_train_id != single->GetTrainRecord(k)->m_data.depart_train_id
			&& strlen(single->GetTrainRecord(k)->m_data.arrive_train_id)>0 && strlen(single->GetTrainRecord(k)->m_data.depart_train_id)>0)
		{
			strTrainId = strTrainId + "//";
			break;
		}
	}
	CString temp;
	for(k = 0; k < single->GetTrainRecordCount(); k++)
	{
		if(strlen(single->GetTrainRecord(k)->m_data.original_arrive_train_id)>0)
		{
			temp = CString(single->GetTrainRecord(k)->m_data.original_arrive_train_id);
			break;
		}
		else if(strlen(single->GetTrainRecord(k)->m_data.original_depart_train_id)>0)
		{
			temp = CString(single->GetTrainRecord(k)->m_data.original_depart_train_id);
			break;
		}
	}
	if(!temp.IsEmpty()){
		strTrainId=strTrainId+"()"+temp;
	}

	CSize sExtent = pCDC->GetTextExtent(strTrainId);
	CTG_SingleTrain* curtrain = NULL;
	int layer=1,iDelta=0;
	CRect rect;
	rect.left=ptlEnd.x-sExtent.cx;
	rect.right=ptlEnd.x+sExtent.cx;
	rect.top=ptlEnd.y-sExtent.cy*3;
	rect.bottom=ptlEnd.y+sExtent.cy*3;

	for(i=0;i<trainCount;i++)
	{
		curtrain = &m_vecSchdTrains[i];
		if(curtrain==NULL)continue;
		if(curtrain==single)continue;
		int tem_index_cur = GetAdditionalTrainInfoIndex(curtrain->GetTrainIndex());
		if(tem_index_cur == -1)
			return 1;

		for(int j=0;j<train_additional_info[tem_index_cur].ptoftrainid_count;j++)
		{
			if(rect.PtInRect(train_additional_info[tem_index_cur].stPttoTrainid[j].ptActual))
			{
				orderedList[orderedCount++]=curtrain;
				break;
			}
		}
	}
	if(orderedCount==0)
		layer=1;
	else
	{
		rect.top=ptlEnd.y-sExtent.cy/2;
		rect.bottom=ptlEnd.y+sExtent.cy/2;
		i=0;
		bool find;
		while(i<orderedCount)
		{
			curtrain=orderedList[i];find=false;
			int tem_index_cur = GetAdditionalTrainInfoIndex(curtrain->GetTrainIndex());
			if(tem_index_cur == -1)
				return 1;
			for(int j=0;j<train_additional_info[tem_index_cur].ptoftrainid_count;j++)
			{
				if(rect.PtInRect((train_additional_info[tem_index_cur].stPttoTrainid[j]).ptActual))
				{
					find=true;
					break;
				}
			}
			if(find)
			{
				layer++;
				iDelta = sExtent.cy*(layer-1);
				if(nDirect == UP_MARK)
					ptlEnd.y += -iDelta;
				else  ptlEnd.y += iDelta;
				rect.top=ptlEnd.y-sExtent.cy/2;
				rect.bottom=ptlEnd.y+sExtent.cy/2;
				i=0;
				continue;
			}
			if(layer>8)
				break;
			i++;
		}
	}
	//layer=layer%5
	//返回正确的层次
	train_additional_info[tem_index].stPttoTrainid[m_cur_index].ptActual=ptlEnd;
	train_additional_info[tem_index].stPttoTrainid[m_cur_index].layer=layer;
	return layer;
}

CSize CPlotDoc::GetNodesExtent(CTG_TrainRecord *rec1, CTG_TrainRecord *rec2, CTG_SingleTrain* train1,CTG_SingleTrain* train2,CDC *pCDC)
{
	CSize sExtent;
	sExtent.cx = 0;
	sExtent.cy = 0;
	if(IsFlagStart(rec2->m_data.flag))
	{
		if(IsFlagStart(rec1->m_data.flag))
		{
			CString str1,str2;
			str1.Format("%s",train1->GetTrainID());
			str2.Format("%s",train2->GetTrainID());
			sExtent = pCDC->GetTextExtent(str1+str2);
			sExtent.cx /= 2;
		}
		else
		{
			sExtent = pCDC->GetTextExtent(train2->GetTrainID());
			sExtent.cx /= 2;
		}
	}
	else
	{
		if(IsFlagStart(rec1->m_data.flag))
		{
			CSize tempExtent;
			sExtent = pCDC->GetTextExtent(train2->GetTrainID());
			tempExtent = pCDC->GetTextExtent(train1->GetTrainID());
			sExtent.cx += tempExtent.cx/2;
		}
		else
		{
			sExtent = pCDC->GetTextExtent(train2->GetTrainID());			
		}
	}
	return sExtent;
}

IMPLEMENT_DYNCREATE(CPlotAbsMark,CObject)

CPlotAbsMark::CPlotAbsMark()
{
}

void CPlotAbsMark::SetColor(COLORREF newColor)
{
	mColor = newColor; 
	m_nDrawFlag = mColor;
}

void CPlotAbsMark::SetDrawFlag(int nDrawFlag, BOOL isChangeOldDrawFlag)
{
	if(isChangeOldDrawFlag == TRUE)
		m_nOldDrawFlag = nDrawFlag;
	m_nDrawFlag = nDrawFlag;
}

void CPlotAbsMark::RestoreDrawFlag()
{
	m_nDrawFlag = m_nOldDrawFlag;
}

void CPlotAbsMark::FrameRectExtend(CDC *pDC, CRect &rc)
{ 	
	pDC->MoveTo(rc.TopLeft());
	pDC->LineTo(rc.right,rc.top);
	pDC->LineTo(rc.right,rc.bottom);
	pDC->LineTo(rc.left,rc.bottom);
	pDC->LineTo(rc.TopLeft());

}

//-----------------------------------------------------
CMultiMark::CMultiMark()
{
	DeleteContents();
}
CMultiMark::~CMultiMark()
{
	DeleteContents();
}

void CMultiMark::AddFigure(CPlotAbsMark *pFigure)
{
	mObArray.Add(pFigure);
}

CPlotAbsMark *CMultiMark::GetFigure(int uIndex)
{
	if(uIndex < 0 || uIndex > mObArray.GetUpperBound( ))
		return 0;
	return (CPlotAbsMark *)mObArray.GetAt(uIndex);
}

int CMultiMark:: GetNumFigures()
{
	return mObArray.GetSize();
}

void CMultiMark::DeleteContents()
{
	int uIndex = mObArray.GetSize();
	while(uIndex--)
		delete (CPlotAbsMark*)mObArray.GetAt(uIndex);
	mObArray.RemoveAll();
}

bool CPlotDoc::IsInhibitStatTrain(int con_id, int type)
{
	for(int i=0; i<m_InhibitStatTrain.nTypeCount; i++)
	{
		if(m_InhibitStatTrain.trainType[i] == type)
			return true;
	}

	return false;
}

_node_info* CPlotDoc::GetNodeByIndex(int con_id, int node_index)
{
	if(con_id >= MAX_SECTION_NUM || con_id < 0)
		return NULL;
	if(node_index >= 128 || node_index < 0)
		return NULL;

	return &section_node_info.node_info[node_index];
}

bool CPlotDoc::GetOffsetAndSize(int type, int& x_offset, int& y_offset, int& height, int& width)
{
	double offset_x = 1, offset_y = 1, height_r = 1, width_r = 1;
	switch(type)
	{
	case AREA_TYPE::NAME:
		{
			offset_x = mPlotStyle.name_area.x_offset;
			offset_y = mPlotStyle.name_area.y_offset;
			height_r = mPlotStyle.name_area.height;
			width_r  = mPlotStyle.name_area.width;
		}
		break;
	case AREA_TYPE::TOP_TIME:
		{
			offset_x = mPlotStyle.train_area.x_offset;
			offset_y = mPlotStyle.train_area.y_offset;
			width_r  = mPlotStyle.train_area.width;
		}
		break;
	case AREA_TYPE::TRAIN:
		{
			offset_x = mPlotStyle.train_area.x_offset;
			offset_y = mPlotStyle.train_area.y_offset+mPlotStyle.timedlt;
			height_r = mPlotStyle.train_area.height;
			width_r  = mPlotStyle.train_area.width;
		}
		break;
	case AREA_TYPE::BOTTOM_TIME:
		{
			offset_x = mPlotStyle.train_area.x_offset;
			offset_y = mPlotStyle.train_area.y_offset+mPlotStyle.train_area.height+2*mPlotStyle.timedlt;
			width_r  = mPlotStyle.train_area.width;
		}
		break;
	case AREA_TYPE::PHASE:
		{
			offset_x = mPlotStyle.phase_area.x_offset;
			offset_y = mPlotStyle.phase_area.y_offset;
			height_r = mPlotStyle.phase_area.height;
			width_r  = mPlotStyle.phase_area.width;
		}
		break;
	case AREA_TYPE::NOTE:
		{
			offset_x = mPlotStyle.note_area.x_offset;
			offset_y = mPlotStyle.note_area.y_offset;
			height_r = mPlotStyle.note_area.height;
			width_r  = mPlotStyle.note_area.width;
		}
		break;
	case AREA_TYPE::CAR:
		{
			offset_x = mPlotStyle.car_area.x_offset;
			offset_y = mPlotStyle.car_area.y_offset;
			height_r = mPlotStyle.car_area.height;
			width_r  = mPlotStyle.car_area.width;
		}
		break;
	case AREA_TYPE::QB:
		{
			offset_x = mPlotStyle.qb_area.x_offset;
			offset_y = mPlotStyle.qb_area.y_offset;
			height_r = mPlotStyle.qb_area.height;
			width_r  = mPlotStyle.qb_area.width;
		}
		break;
	case AREA_TYPE::QB_LEFT:
		{
			offset_x = mPlotStyle.qb_area_left.x_offset;
			offset_y = mPlotStyle.qb_area_left.y_offset;
			height_r = mPlotStyle.qb_area_left.height;
			width_r  = mPlotStyle.qb_area_left.width;
		}
		break;
	case AREA_TYPE::QB_RIGHT:
		{
			offset_x = mPlotStyle.qb_area_right.x_offset;
			offset_y = mPlotStyle.qb_area_right.y_offset;
			height_r = mPlotStyle.qb_area_right.height;
			width_r  = mPlotStyle.qb_area_right.width;
		}
		break;
	case AREA_TYPE::TITLE:
		{
			offset_x = 50;
			offset_y = 1.5;
			height_r = 1;
		}
		break;
	case AREA_TYPE::FOOT:
		{
			offset_x = 50;
			offset_y = 99.7;
			height_r = 1;
		}
		break;
	default:
		return false;
	}

	x_offset = (int)((offset_x/100.0)*m_nPageWidth);
	y_offset = (int)((offset_y/100.0)*m_nPageHeight);

	height   = (height_r/100.0)*m_nPageHeight;
	width    = (width_r/100.0)*m_nPageWidth;
	return true;
}

double nRadius = 14.0;
void GetRrcArlRect(CPoint pt, CRect& rect, int nFlagX, int nFlagY)
{
	rect.left = pt.x - nRadius;
	rect.top  = pt.y - nRadius;

	rect.right   = pt.x + nRadius;
	rect.bottom  = pt.y + nRadius;
	
	rect.OffsetRect(nFlagX*18, 9*nFlagY);
	return;
}

void GetRrcRect(CPoint pt, double dltX, double dltY, CRect& rect, int nFlag)
{
	double l = sqrt(dltX*dltX + dltY*dltY);
		
	double H = nRadius;
	double L = (nRadius*l)/dltY + H*dltX/dltY;

	rect.left = pt.x + L - nRadius;
	rect.top  = pt.y + nFlag*H - nRadius;

	rect.right   = pt.x + L + nRadius;;
	rect.bottom  = pt.y + nFlag*H + nRadius;

	return;
}

void GetRrcRect2(CPoint pt, double dltX, double dltY, CRect& rect, int nFlag)
{
	double l = sqrt(dltX*dltX + dltY*dltY);

	double H = nRadius;
	double L = (nRadius*l)/dltY + H*dltX/dltY;

	rect.left = pt.x + L - nRadius;
	rect.top  = pt.y + nFlag*H - nRadius;

	rect.right   = pt.x + L + nRadius;;
	rect.bottom  = pt.y + nFlag*H + nRadius;

	rect.OffsetRect(10,0);
	return;
}

void CPlotDoc::DrawTrainEarlyLastTime(CDC * pCDC,CTG_SingleTrain *single,COLORREF color,bool isPrint,int schd_type)
{
	int scaleRate = min(yMultiple,5);

	if(schd_type == BASIC_SCHEDULE)
		return;

	int nRecNum = single->GetTrainRecordCount();  
	if(0 == nRecNum)
		return;

	int nTrainType  = single->GetTrainType();
	bool bInhibit = this->IsInhibitStatTrain(m_nCurrentSectionNo, nTrainType);

	// 禁止统计早晚点列车, 不画黑圈
	if(bInhibit && !m_bDrawBlackArc)
		return; 

	InitEarlyLastTime(single);
	
	int historyWidth = (int)(m_nHistoryGraphWidth*m_fPixelPerMinute);

	CRect resultRect, rangeRect(0, 0, m_nPageWidth, m_nPageHeight);
	if(m_nSchdStatus == WORK_SCHEDULE || m_nSchdStatus == SHIFT_SCHEDULE)
	{
		rangeRect.SetRect(0, 0, m_nPageWidth, m_nPageHeight);
	}
	else if(m_nSchdStatus==HISTORY_SCHEDULE)
		rangeRect.SetRect(0, 0, historyWidth, m_nPageHeight);
	else if(m_nSchdStatus==BASIC_SCHEDULE)
		rangeRect.SetRect(0, 0, m_nPageWidth, m_nPageHeight);

	rangeRect.SetRect(0,0,m_fPixelPerHour* m_nViewTimeRange,m_nPageHeight);
	int m_nLogPixelSy = pCDC->GetDeviceCaps(LOGPIXELSY);
	if(m_nLogPixelSy >300)
		m_nLogPixelSy = 400;

	CFont font;
	LOGFONT stFont;
	CFont * pOldFont;
	memset(&stFont,0,sizeof(stFont));

	stFont.lfHeight = MulDiv(7,-m_nLogPixelSy,72);  
	stFont.lfWeight = FW_NORMAL;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	strcpy(stFont.lfFaceName, "Times New Roman");
	font.CreateFontIndirect(&stFont);
	pOldFont = pCDC->SelectObject(&font);

	CPen nPen;
	CPen nPen_red;
	CPen nPen_black;
	nPen.CreatePen(PS_JOIN_ROUND,1 * scaleRate,CLR_BLUE);
	nPen_red.CreatePen(PS_JOIN_ROUND,1 *scaleRate,CLR_RED);
	nPen_black.CreatePen(PS_JOIN_ROUND,1 *scaleRate,CLR_BLACK);
	
	CPen* nOldPen = pCDC->SelectObject(&nPen);
	int nOldColor = pCDC->SetTextColor(CLR_BLUE); 
	UINT nOldBackMode = pCDC->SetBkMode(TRANSPARENT);
	UINT nOldAlign = pCDC->SetTextAlign(TA_CENTER);

	int timeRectWidth = pCDC->GetDeviceCaps(LOGPIXELSX)*0.3937;
	
	//定义半径
	nRadius = timeRectWidth/4; //5
	
	int m_nDirectFlag_prefix = GetTrainRecDirect(single, 0);
	int nDirect_flag = m_nDirectFlag_prefix;

	bool bNoEarlylatetime=false;
	std::map<ULONG, ULONG>::iterator it = m_noearlylatetime.find(single->GetTrainIndex());
	if(it != m_noearlylatetime.end())
	{
		bNoEarlylatetime=true;
	}
	
	//列车节点的记录个数
	for(UINT i = 0; i< nRecNum; i++)
	{
		CTG_TrainRecord* rec = single->GetTrainRecord(i);
		if(NULL == rec)
			continue;

		// 到点之差
		int nDeltaArrival = rec->m_data.arrive - rec->m_data.plan_arrive;
		// 发点之差
		int nDeltaDepart = rec->m_data.depart - rec->m_data.plan_depart;

		if(nDeltaArrival%60 > 0) 
			nDeltaArrival = nDeltaArrival - nDeltaArrival%60 ;
		else if(nDeltaArrival%60 < 0)
			nDeltaArrival = nDeltaArrival + nDeltaArrival%60 ;
		if(nDeltaDepart%60 > 0) 
			nDeltaDepart = nDeltaDepart - nDeltaDepart%60 ;
		else if(nDeltaArrival%60 < 0)
			nDeltaDepart = nDeltaDepart + nDeltaDepart%60 ;

		nDeltaArrival = nDeltaArrival/60;
		nDeltaDepart = nDeltaDepart/60;

		if(bNoEarlylatetime || bInhibit)
		{
			nDeltaArrival=0;
			nDeltaDepart=0;
		}
		
		int nDeltaArrival_h = nDeltaArrival/60;
		int nDeltaArrival_m = nDeltaArrival%60;
		int nDeltaDepart_h = nDeltaDepart/60;
		int nDeltaDepart_m = nDeltaDepart%60;

		int y = GetStationVPosByIndex(single,rec,PRIMARY_TRACK,ENTRY_POSITION);
		if(y < 0) 
			continue;

		int local_stat_arrival = 0;
		int local_stat_depart = 0;
		if(IsStatArrival(rec->m_data.stop_reason))
			local_stat_arrival = 4;
		if(IsStatDepart(rec->m_data.stop_reason))
			local_stat_depart = 4;

		if(IsFlagStart(rec->m_data.flag))
			local_stat_arrival = 0;
		else if(IsFlagTerminal(rec->m_data.flag))
			local_stat_depart = 0;
		
		//判断完方向
		nDirect_flag = GetTrainRecDirect(single, i);
		
		// 显示到点
		if(local_stat_arrival > 0)
		{		
			CPoint point;
			long time_arrival,date_arrival;
			time_arrival = minute_time(rec->m_data.arrive,&date_arrival);
			long arrival_minute_width = GetMinuteWidthToPlotStartTime(time_arrival/60,date_arrival,schd_type);
			point.x = arrival_minute_width * m_fPixelPerMinute;

			if(nDirect_flag == DOWN_DIRECT)
				point.y = GetStationVPosByIndex(single,rec,TOP_TRACK,ENTRY_POSITION) ;
			else
				point.y = GetStationVPosByIndex(single,rec,BOTTOM_TRACK,ENTRY_POSITION);

			CRect rect;
			CPoint ptPreDept(0,0);
			CTG_TrainRecord *pre = single->GetTrainRecord(i-1);
			if(NULL != pre)
			{
				if (TG_IsFlagStopAbs(pre->m_data.flag))
				{
					long time_depart=0,date_depart=0;
					//ptPreDept.x = m_fPixelPerMinute*GetOffsetMinuteFromGraphBegin(pre->depart+pre->stop_abs_e_time*60);

					time_depart = minute_time(pre->m_data.depart+pre->m_data.special_time_2*60,&date_depart);
					long depart_minute_width = GetMinuteWidthToPlotStartTime(time_depart/60, date_depart, schd_type);
					ptPreDept.x = m_fPixelPerMinute*depart_minute_width;

					if (rec->m_data.station == pre->m_data.station)
					{
						if(TG_IsFlagStopAbsDown(pre->m_data.flag))
							ptPreDept.y = this->m_sDispDistance[pre->GetDepartTGIndex()+1].nStationYPos;
						else
						{
							if(pre->GetDepartTGIndex()>=1)  // 2016.10.10 打印数组越界
								ptPreDept.y = this->m_sDispDistance[pre->GetDepartTGIndex()-1].nStationYPos;
							else
								ptPreDept.y = 0;
						}
						if(TG_IsFlagStopAbsDown(pre->m_data.flag))
							GetRrcRect(point,  point.x - ptPreDept.x, point.y - ptPreDept.y, rect, 1);
						else
							GetRrcRect(point,  point.x - ptPreDept.x, ptPreDept.y - point.y, rect, -1);
					}
					else
					{
						if(TG_IsFlagStopAbsDown(pre->m_data.flag))
							ptPreDept.y = this->m_sDispDistance[pre->GetDepartTGIndex()].nSecondStationYPos;
						else
							ptPreDept.y = this->m_sDispDistance[pre->GetDepartTGIndex()].nStationYPos;

						if(TG_IsFlagStopAbsUp(pre->m_data.flag))
							GetRrcRect(point,  point.x - ptPreDept.x, point.y - ptPreDept.y, rect, 1);
						else
							GetRrcRect(point,  point.x - ptPreDept.x, ptPreDept.y - point.y, rect, -1);
					}
				}
				else
				{
					long time_depart=0,date_depart=0;
					time_depart = minute_time(pre->m_data.depart,&date_depart);
					long depart_minute_width = GetMinuteWidthToPlotStartTime(time_depart/60, date_depart, schd_type);
					ptPreDept.x = m_fPixelPerMinute*depart_minute_width;

					if(nDirect_flag == DOWN_DIRECT)
						ptPreDept.y = GetStationVPosByIndex(single,pre,BOTTOM_TRACK,EXIT_POSITION) ;
					else
						ptPreDept.y = GetStationVPosByIndex(single,pre,TOP_TRACK,EXIT_POSITION);

					if(nDirect_flag == UP_DIRECT)
						GetRrcRect(point,  point.x - ptPreDept.x, point.y - ptPreDept.y, rect, 1);
					else
						GetRrcRect(point,  point.x - ptPreDept.x, ptPreDept.y - point.y, rect, -1);
				}
			}
			else
			{
				if(nRecNum == 1)
				{
					if(nDirect_flag == UP_DIRECT)
						GetRrcArlRect(point,  rect, -1, -1);
					else
						GetRrcArlRect(point,  rect, -1, 1);
				}
				else
				{
					if(nDirect_flag == UP_DIRECT)
						GetRrcArlRect(point,  rect, -1, 1);
					else
						GetRrcArlRect(point,  rect, -1, -1);
				}
			}

			
			if(bNoEarlylatetime)
			{
				pCDC->SelectObject(nOldPen);
				nOldPen = pCDC->SelectObject(&nPen_red);
				nOldColor = pCDC->SetTextColor(CLR_RED);
			}
			else if(bInhibit)
			{
				pCDC->SelectObject(nOldPen);
				nOldPen = pCDC->SelectObject(&nPen_black);
				nOldColor = pCDC->SetTextColor(CLR_BLACK);

			}
			else
			{
				if(nDeltaArrival<= 0)
				{
					pCDC->SelectObject(nOldPen);
					nOldPen = pCDC->SelectObject(&nPen_red);
					nOldColor = pCDC->SetTextColor(CLR_RED);
				}
				else
				{
					pCDC->SelectObject(nOldPen);
					nOldPen = pCDC->SelectObject(&nPen);
					nOldColor = pCDC->SetTextColor(CLR_BLUE);
				}

				// 哈尔滨调度员反映，按照技规列车晚点5分钟（含5分钟）以内，应显示红圈蓝字，5分钟以外显示为蓝圈蓝字
				if(m_hrbLateTimeMode)
				{
					if(nDeltaArrival > 0 && nDeltaArrival <= 5)
					{
						pCDC->SelectObject(&nPen_red);
						pCDC->SetTextColor(CLR_BLUE);
					}
				}
			}
			

			CString str="";
			if(abs(nDeltaArrival_h) < 1)
				str.Format("%d",abs(nDeltaArrival_m));            
			else
			{
				if(abs(nDeltaArrival_m) < 10)
					str.Format("%d.0%d",abs(nDeltaArrival_h),abs(nDeltaArrival_m));            
				else
					str.Format("%d.%d",abs(nDeltaArrival_h),abs(nDeltaArrival_m));            
			}

			rect.OffsetRect(x_offset_print+1,y_offset_print);
			point.Offset(x_offset_print+1,y_offset_print);

			if(point.x >= x_offset_print && point.x <= (rangeRect.right + x_offset_print))
			{
				pCDC->Arc(&rect,CPoint(rect.left ,(rect.top + rect.bottom )/2 ),CPoint(rect.left ,(rect.top + rect.bottom )/2 ));
				if(nDeltaArrival != 0)
				{
					CSize tx_size;
					tx_size = pCDC->GetTextExtent(str);
					pCDC->TextOut (rect.CenterPoint().x, rect.CenterPoint().y - tx_size.cy/2,str);
				}
			}

			pCDC->SelectObject(nOldPen);
			pCDC->SetTextColor(nOldColor);
		}
				
		// 显示发点
		if(local_stat_depart > 0)
		{ 
			CPoint point;
			long time_depart=0,date_depart=0;
			time_depart = minute_time(rec->m_data.depart,&date_depart);
			long depart_minute_width = GetMinuteWidthToPlotStartTime(time_depart/60, date_depart, schd_type);
			point.x = depart_minute_width * m_fPixelPerMinute;

			if(nDirect_flag == DOWN_DIRECT)
				point.y = GetStationVPosByIndex(single,rec,BOTTOM_TRACK,EXIT_POSITION) ;
			else
				point.y = GetStationVPosByIndex(single,rec,TOP_TRACK,EXIT_POSITION);

			CRect rect;
			CPoint ptNextArl(0,0);
			CTG_TrainRecord *next = single->GetTrainRecord(i+1);
			if(NULL != next)
			{
				if (TG_IsFlagStopAbs(rec->m_data.flag))
				{
					long time_arrival,date_arrival;
					//ptNextArl.x = m_fPixelPerMinute*GetOffsetMinuteFromGraphBegin(first->depart+first->stop_abs_s_time*60);	

					time_arrival = minute_time(rec->m_data.depart+rec->m_data.special_time_1*60,&date_arrival);
					long arrival_minute_width = GetMinuteWidthToPlotStartTime(time_arrival/60,date_arrival,schd_type);

					if (rec->m_data.station == next->m_data.station)
					{
						if(nDirect_flag == DOWN_DIRECT)
							ptNextArl.y = this->m_sDispDistance[next->GetArriveTGIndex()+1].nStationYPos;
						else
						{
							if(next->GetArriveTGIndex()>=1)  // 2016.10.10 打印数组越界
								ptNextArl.y = this->m_sDispDistance[next->GetArriveTGIndex()-1].nStationYPos;
							else
								ptNextArl.y = 0;
						}
					}
					else
					{
						if(nDirect_flag == DOWN_DIRECT)
							ptNextArl.y = this->m_sDispDistance[next->GetArriveTGIndex()].nStationYPos;
						else
							ptNextArl.y = this->m_sDispDistance[next->GetArriveTGIndex()].nSecondStationYPos;
					}

					if(TG_IsFlagStopAbsUp(rec->m_data.flag))
						GetRrcRect2(point, ptNextArl.x - point.x, point.y - ptNextArl.y, rect, -1);
					else
						GetRrcRect2(point, ptNextArl.x - point.x, ptNextArl.y - point.y, rect, 1);
				}
				else
				{
					long time_arrival,date_arrival;
					time_arrival = minute_time(next->m_data.arrive,&date_arrival);
					long arrival_minute_width = GetMinuteWidthToPlotStartTime(time_arrival/60,date_arrival,schd_type);

					ptNextArl.x = m_fPixelPerMinute*arrival_minute_width;

					if(nDirect_flag == DOWN_DIRECT)
						ptNextArl.y = GetStationVPosByIndex(single,next,TOP_TRACK,ENTRY_POSITION) ;
					else
						ptNextArl.y = GetStationVPosByIndex(single,next,BOTTOM_TRACK,ENTRY_POSITION);

					if(nDirect_flag == UP_DIRECT)
						GetRrcRect(point, ptNextArl.x - point.x, point.y - ptNextArl.y, rect, -1);
					else
						GetRrcRect(point, ptNextArl.x - point.x, ptNextArl.y - point.y, rect, 1);
				}
			}
			else
			{
				if(nRecNum == 1)
				{
					if(nDirect_flag == UP_DIRECT)
						GetRrcArlRect(point,  rect, 1, 1);
					else
						GetRrcArlRect(point,  rect, 1, -1);
				}
				else
				{
					if(nDirect_flag == UP_DIRECT)
						GetRrcArlRect(point,  rect, 1, -1);
					else
						GetRrcArlRect(point,  rect, 1, 1);
				}
			}
		
			rect.OffsetRect(x_offset_print+1,y_offset_print);
			point.Offset(x_offset_print+1,y_offset_print);
				
			if(bNoEarlylatetime)
			{
				pCDC->SelectObject(nOldPen);
				nOldPen = pCDC->SelectObject(&nPen_red);
				nOldColor = pCDC->SetTextColor(CLR_RED);
			}
			else if(bInhibit)
			{
				pCDC->SelectObject(nOldPen);
				nOldPen = pCDC->SelectObject(&nPen_black);
				nOldColor = pCDC->SetTextColor(CLR_BLACK);

			}
			else
			{
				if(nDeltaDepart<= 0)
				{
					pCDC->SelectObject(nOldPen);
					nOldPen = pCDC->SelectObject(&nPen_red);
					nOldColor = pCDC->SetTextColor(CLR_RED);
				}
				else
				{
					pCDC->SelectObject(nOldPen);
					nOldPen = pCDC->SelectObject(&nPen);
					nOldColor = pCDC->SetTextColor(CLR_BLUE);
				}

				// 哈尔滨调度员反映，按照技规列车晚点5分钟（含5分钟）以内，应显示红圈蓝字，5分钟以外显示为蓝圈蓝字
				if(m_hrbLateTimeMode)
				{
					if(nDeltaDepart > 0 && nDeltaDepart <= 5)
					{
						pCDC->SelectObject(&nPen_red);
						pCDC->SetTextColor(CLR_BLUE);
					}
				}
			}
			
			CString str="";
			if(abs(nDeltaDepart_h) < 1)
				str.Format("%d",abs(nDeltaDepart_m));  
			else
			{
				if(abs(nDeltaDepart_m) < 10)
					str.Format("%d.0%d",abs(nDeltaArrival_h),abs(nDeltaDepart_m));//modified by zl 20120418 兰州西宁反应早晚点打印错误            
				else        
					str.Format("%d.%d",abs(nDeltaArrival_h),abs(nDeltaDepart_m));
			}

			if(point.x >= x_offset_print && point.x <= rangeRect.right + x_offset_print)
			{
				pCDC->Arc(&rect, CPoint(rect.left, (rect.top + rect.bottom )/2), CPoint(rect.left, (rect.top + rect.bottom )/2));

				if(nDeltaDepart != 0)
				{
					CSize size;
					size = pCDC->GetTextExtent(str);
					pCDC->TextOut (rect.CenterPoint().x, rect.CenterPoint().y - size.cy/2, str);
				}
			}

			pCDC->SelectObject(nOldPen);
			pCDC->SetTextColor(nOldColor);
		}		
	}

	pCDC->SelectObject(pOldFont);   
	pCDC->SetTextColor(nOldColor);
	pCDC->SetBkColor (nOldBackMode);
	pCDC->SetTextAlign(nOldAlign);

	font.DeleteObject(); 
	nPen_red.DeleteObject();
	nPen.DeleteObject();
	nPen_black.DeleteObject();
}

void CPlotDoc::InitEarlyLastTime(CTG_SingleTrain* single)
{
	if(single == NULL)  return;

	CTG_TrainRecord *rec = NULL;
	int nRecNum = single->GetTrainRecordCount();
	int nTrainType = single->GetTrainInfo()->m_data.type;
	for(int i = 0; i< nRecNum; i++)
	{		
		rec = single->GetTrainRecord(i);
		if(rec==NULL) continue;
		
		ResetStatBoth(rec->m_data.stop_reason);

		int node_index = 0;
		if(i==0) 
		{
			if(IsFlagStart(rec->m_data.flag))
				node_index = rec->GetDepartTGIndex();
			else
				node_index = rec->GetArriveTGIndex();
		}
		else if(i==nRecNum-1)
		{
			if(IsFlagTerminal(rec->m_data.flag))
				node_index = rec->GetArriveTGIndex();
			else
				node_index = rec->GetDepartTGIndex();
		}
		else
		{
			node_index = rec->GetArriveTGIndex();
			if(node_index == -1)
				node_index = rec->GetDepartTGIndex();
		}

		if(node_index == -1 || node_index > 128)
			continue;

		const _node_info *pNodeInfo = GetNodeByIndex(m_nCurrentSectionNo, node_index);

		//在当前所在在的区段中查找是否有该车站
		if(pNodeInfo == NULL)
		{  
			ResetStatBoth(rec->m_data.stop_reason);
			continue;
		}		

		// 此车站不允许显示统计(始发站和终到站除外）
		if(pNodeInfo->show_statistic)
		{
			int nSel = 3;
			int nDirect = GetTrainRecDirect(single, i);
			// 上行
			if(nDirect == UP_DIRECT)
			{
				// 客车
				if(single->IsPassengerTrain())
					nSel = pNodeInfo->up_pessenger_train;
				else
					nSel = pNodeInfo->up_goods_train;
			}

			// 下行
			if(nDirect == DOWN_DIRECT)
			{
				// 客车
				if(single->IsPassengerTrain())
					nSel = pNodeInfo->down_pessenger_train;
				else
					nSel = pNodeInfo->down_goods_train;
			}

			if(nSel == 0)
			{
				SetStatArrival(rec->m_data.stop_reason);
			}
			else if(nSel == 1)
			{
				SetStatDepart(rec->m_data.stop_reason);
			}
			else if(nSel == 2)
			{
				SetStatBoth(rec->m_data.stop_reason);
			}

			// 通过车统计定义
			if(!m_bStaticsTimeCompleteByTGDef)
			{
				if(rec->m_data.arrive == rec->m_data.depart && nSel == 2)
				{
					//  客车 图定到开 但是实际通过 统计发点
					if(rec->m_data.plan_arrive != rec->m_data.plan_depart) 
					{
						if(m_nStaticMethod)
						{
							if(single->IsPassengerTrain())
							{
								ResetStatBoth(rec->m_data.stop_reason);
								SetStatDepart(rec->m_data.stop_reason);
							}
						}
					}
					else
					{
						ResetStatBoth(rec->m_data.stop_reason);
						SetStatDepart(rec->m_data.stop_reason);
					}
				}
			}
		}


		// 始发 终到 情况
		if(i==0) 
		{
			if(IsFlagStart(rec->m_data.flag))
			{
				ResetStatArrival(rec->m_data.stop_reason);
				SetStatDepart(rec->m_data.stop_reason);
			}
		}
		else if(i==nRecNum-1)
		{
			if(IsFlagTerminal(rec->m_data.flag))
			{
				SetStatArrival(rec->m_data.stop_reason);
				ResetStatDepart(rec->m_data.stop_reason);
			}
		}
	}
}

UINT CPlotDoc::GetTrainRecDirect(CTG_SingleTrain* single, int index)
{
	UINT nDirect_flag = DOWN_DIRECT;
	if(single == NULL)  
		return nDirect_flag;

	CString strTrainId = single->GetTrainID();
	CTG_TrainRecord* first = single->GetTrainRecord(0);
	if(NULL != first)
	{
		if(strTrainId.IsEmpty())
			strTrainId = first->m_data.arrive_train_id;
		if(strTrainId.IsEmpty())
			strTrainId = first->m_data.depart_train_id;
	}

	// 去掉非数字
	CString strLastNumTrainId="0";
	int len = strTrainId.GetLength();
	for(int i= len-1;i>=0;i--)
	{
		char lastchar = strTrainId.GetAt(i);
		if(0x30 <= lastchar && lastchar <= 0x39)
		{
			strLastNumTrainId = lastchar;
			break;
		}
	}

	len = atoi(strLastNumTrainId);
	if(len%2 == 0)
		nDirect_flag = UP_DIRECT;
	else
		nDirect_flag = DOWN_DIRECT;
	

	CTG_TrainRecord* rec = single->GetTrainRecord(index);
	if(NULL == rec)
		return nDirect_flag;
	
	int nRecNum = single->GetTrainRecordCount();
	//判断完方向
	if(nRecNum > 1)
	{
		if(index == 0)
		{
			CTG_TrainRecord *next_rec = single->GetTrainRecord(index+1);
			if(next_rec != NULL)
			{
				if(rec->GetDepartTGIndex() > next_rec->GetArriveTGIndex())
				{
					nDirect_flag = UP_DIRECT;
				}
				else
				{
					nDirect_flag = DOWN_DIRECT;
				}
			}
		}
		else if(index == nRecNum -1)
		{
			CTG_TrainRecord* pre_rec = single->GetTrainRecord(index-1);
			if(NULL != pre_rec)
			{
				if(pre_rec->GetDepartTGIndex() > rec->GetArriveTGIndex())
				{
					nDirect_flag = UP_DIRECT;
				}
				else
				{
					nDirect_flag = DOWN_DIRECT;
				}
			}
		}
		else if(index > 0 && index < nRecNum-1)
		{
			CTG_TrainRecord* pre_rec = single->GetTrainRecord(index-1);
			if(NULL != pre_rec)
			{
				if(pre_rec->GetDepartTGIndex() > rec->GetArriveTGIndex())
				{
					nDirect_flag = UP_DIRECT;
				}
				else
				{
					nDirect_flag = DOWN_DIRECT;
				}
			}
		}
	}

	return nDirect_flag;
}

// 不打印站存车
bool CPlotDoc::IsPrintPhaseCar(int con_id)
{
	for(int i=0; i<mPlotStyle.con_num; i++)
	{
		if(mPlotStyle.con_id[i] == con_id)
			return false;
	}
	return true;
}

void InitBlockCfg(BlockCfg& blockcfg, MSXML::IXMLDOMNodePtr pNode)
{
	if(NULL == pNode)
		return;
	MSXML::IXMLDOMNodePtr pChildNode;
	// @draw
	pChildNode = pNode->selectSingleNode("@draw");
	if(pChildNode)
	{
		_variant_t v;
		v = pChildNode->GetnodeTypedValue();
		blockcfg.draw = atoi(CString(v.bstrVal));			
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	// @time
	pChildNode = pNode->selectSingleNode("@time");
	if(pChildNode)
	{
		_variant_t v;
		v = pChildNode->GetnodeTypedValue();
		blockcfg.time_pos = atoi(CString(v.bstrVal));	
		SysFreeString(v.bstrVal);
		v.Clear();
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
		blockcfg.color1 = RGB(R,G,B);	

		SysFreeString(v.bstrVal);
		v.Clear();
	}

	// @color1
	pChildNode = pNode->selectSingleNode("@color1");
	if(pChildNode)
	{
		_variant_t v;
		v = pChildNode->GetnodeTypedValue();

		CString rgb(v.bstrVal);
		int R,G,B;
		sscanf(rgb.GetBuffer(), "%d-%d-%d", &R,&G,&B);
		blockcfg.color2 = RGB(R,G,B);	

		SysFreeString(v.bstrVal);
		v.Clear();
	}
	// @color2
	pChildNode = pNode->selectSingleNode("@color2");
	if(pChildNode)
	{
		_variant_t v;
		v = pChildNode->GetnodeTypedValue();

		CString rgb(v.bstrVal);
		int R,G,B;
		sscanf(rgb.GetBuffer(), "%d-%d-%d", &R,&G,&B);
		blockcfg.color3 = RGB(R,G,B);	

		SysFreeString(v.bstrVal);
		v.Clear();
	}
	else
	{
		blockcfg.color3 = blockcfg.color1;
	}
	// @dlt
	pChildNode = pNode->selectSingleNode("@dlt");
	if(pChildNode)
	{
		_variant_t v;
		v = pChildNode->GetnodeTypedValue();
		blockcfg.dlt = atoi(CString(v.bstrVal));	
		SysFreeString(v.bstrVal);
		v.Clear();
	}
}

void CPlotDoc::LoadMarkCfgXML(CString pathname)
{
	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		AfxMessageBox(er.ErrorMessage());
		return;
	}
	
	if(VARIANT_TRUE != pXMLDoc->load( _bstr_t( pathname ) ) )
	{
		CString  prompt;
		prompt.Format("配置%s不存在或者不符合xml语法(可以用浏览器检查是否能打开)", pathname);
		AfxMessageBox(prompt);
		return; 
	}
	
	MSXML::IXMLDOMNodePtr pChildNode,pNode,pRootNode;

	pRootNode = pXMLDoc->selectSingleNode( "MARK_GRAPH");
	if(pRootNode == NULL)
		return;

	_variant_t v;
	
	// -----------------------区间---------------------------
	// 区间封锁
	pNode = pRootNode->selectSingleNode( "ABS_INHIBIT_MARK");
	InitBlockCfg(m_AbsInhibitMarkCfg, pNode);
	// 区间慢行
	pNode = pRootNode->selectSingleNode( "ABS_SLOW_MARK");
	InitBlockCfg(m_AbsSlowMarkCfg, pNode);
	// 电网检修
	pNode = pRootNode->selectSingleNode( "ABS_ELE_FIX_MARK");
	InitBlockCfg(m_AbsEleFixMarkCfg, pNode);
	// 综合天窗修
	pNode = pRootNode->selectSingleNode( "ABS_ZH_FIX_MARK");
	InitBlockCfg(m_AbsZhFixMarkCfg, pNode);
	
	// 区间变更闭塞方式
	pNode = pRootNode->selectSingleNode( "SELF_CLOSE_MARK");
	InitBlockCfg(m_SelfCloseMarkCfg, pNode);

	// -----------------------站内---------------------------
	// 站内股道封锁
	pNode = pRootNode->selectSingleNode( "STATION_SIDE_INHIBIT");
	InitBlockCfg(m_SideInhibitMarkCfg, pNode);
	// 站内股道慢行
	pNode = pRootNode->selectSingleNode( "STATION_SIDE_SLOW");
	InitBlockCfg(m_SideSlowMarkCfg, pNode);
	// 站内股道占用
	pNode = pRootNode->selectSingleNode( "STATION_SIDE_TIANCHUANG");
	InitBlockCfg(m_SideTianchuangMarkCfg, pNode);
	// 站内股道天窗
	pNode = pRootNode->selectSingleNode( "STATION_SIDE_OCCUPY");
	InitBlockCfg(m_SideOccupyMarkCfg, pNode);
}

void CPlotDoc::LoadTrainCZHRelationXML()
{
	m_nCZCount=0;
	for(int i=0; i<255; i++)
		m_sCZRelation[i].Init();
	
	CString strFileName;
	strFileName.Format("%s\\czh_relation.xml", gpcdef.GetDataDir());
	
	GpcLogTrainMessage(0, 0, "开始车组关系文件:%s", strFileName);

	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		AfxMessageBox(er.ErrorMessage());
		return;
	}

	if(VARIANT_TRUE != pXMLDoc->load( _bstr_t( strFileName ) ) )
	{
		CString  prompt;
		prompt.Format("车组关系%s不存在或者不符合xml语法", strFileName);

		return; 
	}

	MSXML::IXMLDOMNodePtr pRoot = pXMLDoc->selectSingleNode( "root");
	if(pRoot == NULL)
	{
		return;
	}
	_variant_t v;
	MSXML::IXMLDOMNodePtr pNode, pChild;
	CTime curtm=CTime::GetCurrentTime();
	long nowtm=curtm.GetTime();
	pChild = pRoot->selectSingleNode( "@tm" );
	if(pChild)
	{
		v = pChild->GetnodeTypedValue();
		CString ss=v.bstrVal;
		long tm=atol(ss);
		if(abs(nowtm-tm)>=60)
		{
			return;
		}
		GpcLogTrainMessage(0, 0, "车组:生成时间%d, 当前时间%d", tm, nowtm);
	}

	MSXML::IXMLDOMNodeListPtr pCZHList = pRoot->selectNodes("CZH");
	for(int m = 0; m <pCZHList->Getlength(); m++)
	{
		CZH_Relation czh;
		czh.nTrainCount=0;
		MSXML::IXMLDOMNodePtr pCZHNode = pCZHList->Getitem(m);
		pChild = pCZHNode->selectSingleNode( "@name" );
		if(pChild)
		{
			v = pChild->GetnodeTypedValue();
			CString ss=v.bstrVal;
			if(ss.GetLength()>0)
			{
				int size=sizeof(czh.cCzh)/sizeof(czh.cCzh[0]);
				if(ss.GetLength()>=size)
				{
					strncpy(czh.cCzh, ss, size-1);
					czh.cCzh[size-1]=0;
				}
				else
				{
					strcpy(czh.cCzh, ss);
				}
			}
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		GpcLogTrainMessage(0, 0, "车组:%s", czh.cCzh);

		MSXML::IXMLDOMNodeListPtr pTrainList = pCZHNode->selectNodes("TRAIN");
		for(int n = 0; n <pTrainList->Getlength(); n++)
		{
			MSXML::IXMLDOMNodePtr pNode = pTrainList->Getitem(n);
			CString strTrainId;
			long nTrainIndex=0;
			pChild = pNode->selectSingleNode( "@name" );
			if(pChild)
			{
				v = pChild->GetnodeTypedValue();
				strTrainId=v.bstrVal;
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pChild = pNode->selectSingleNode( "@index" );
			if(pChild)
			{
				v = pChild->GetnodeTypedValue();
				nTrainIndex = atol(CString((v.bstrVal)));
				SysFreeString(v.bstrVal);
				v.Clear();
			}

			if(nTrainIndex<=0)
			{
				GpcLogTrainMessage(0, 0, "%s 车组关系非法丢弃", czh.cCzh);
				break;
			}			
			int i=czh.nTrainCount;
			czh.Trains[i].nTrainIndex=nTrainIndex;
			czh.nTrainCount++;
			if(strTrainId.GetLength()>0)
			{
				int size=sizeof(czh.Trains[i].strTrainId)/sizeof(czh.Trains[i].strTrainId[0]);
				if(strTrainId.GetLength()>=size)
				{
					strncpy(czh.Trains[i].strTrainId, strTrainId, size-1);
					czh.cCzh[size-1]=0;
				}
				else
				{
					strcpy(czh.Trains[i].strTrainId, strTrainId);
				}
			}
			GpcLogTrainMessage(0, 0, "Train:%s Index:%d", czh.Trains[i].strTrainId, czh.Trains[i].nTrainIndex);
		}
		if(n>=pTrainList->Getlength())
		{
			if(m_nCZCount<max_czh_count)
			{
				m_sCZRelation[m_nCZCount]=czh;
				m_nCZCount++;
			}
			else
			{
				GpcLogTrainMessage(0, 0, "车组关系太多丢弃");
			}
		}
	}
	
	GpcLogTrainMessage(0, 0, "完成车组关系文件读取");
	return;
}

void CPlotDoc::SetTrainBeforeAfterIndex()
{
	for(int i=0; i<m_vecSchdTrains.size(); i++)
	{
		CTG_SingleTrain& train=m_vecSchdTrains[i];
		train.m_nBeforeTrainIndex=0;
		train.m_nNextTrainIndex=0;
		
		for(int n=0; n<m_nCZCount; n++)
		{
			for(int m=0; m<m_sCZRelation[n].nTrainCount; m++)
			{
				if(m_sCZRelation[n].Trains[m].nTrainIndex==train.GetTrainIndex())
				{
					if(m>0)
					{
						train.m_nBeforeTrainIndex=m_sCZRelation[n].Trains[m-1].nTrainIndex;
						strcpy(train.m_cBeforeTrainName,m_sCZRelation[n].Trains[m-1].strTrainId);
						train.m_cBeforeTrainName[10]='\0';
					}
					if(m<m_sCZRelation[n].nTrainCount-1)
					{ 
						train.m_nNextTrainIndex=m_sCZRelation[n].Trains[m+1].nTrainIndex;
						strcpy(train.m_cNextTrainName,m_sCZRelation[n].Trains[m+1].strTrainId);
						train.m_cNextTrainName[10]='\0';
					}

					if(BASIC_SCHEDULE == m_nSchdStatus)
					{
						if(m_sCZRelation[n].nTrainCount>1)
						{
							if(m+1==m_sCZRelation[n].nTrainCount)
							{
								train.m_nNextTrainIndex=m_sCZRelation[n].Trains[0].nTrainIndex;
								strcpy(train.m_cNextTrainName,m_sCZRelation[n].Trains[0].strTrainId);
								train.m_cNextTrainName[10]='\0';
							}
							if(m==0)
							{
								int z=m_sCZRelation[n].nTrainCount;
								train.m_nBeforeTrainIndex=m_sCZRelation[n].Trains[z-1].nTrainIndex;
								strcpy(train.m_cBeforeTrainName,m_sCZRelation[n].Trains[z-1].strTrainId);
								train.m_cBeforeTrainName[10]='\0';
							}
						}
					}
					break;
				}
			}
			if(m<m_sCZRelation[n].nTrainCount)
			{
				break;
			}
		}
	}
}

CTG_SingleTrain* CPlotDoc::GetTrain(TRAIN_INDEX index)
{
	for(int i=0; i<m_vecSchdTrains.size(); i++)
	{
		CTG_SingleTrain& train=m_vecSchdTrains[i];
		if(train.GetTrainIndex()==index)
		{
			return &train;
		}
	}

	return NULL;
}

bool CPlotDoc::IsCRHStopStation(int station)
{
	for(int i=0; i<sizeof(m_nCRHStopStation)/sizeof(m_nCRHStopStation[0]); i++)
	{
		if(m_nCRHStopStation[i]==0)
			break;
		if(m_nCRHStopStation[i]==station)
			return true;
	}

	return false;
}

bool CPlotDoc::IsCRHReturnStation(int station, int index)
{
	for(int i=0; i<sizeof(m_nCRHReturnStation)/sizeof(m_nCRHReturnStation[0]); i++)
	{
		if(m_nCRHReturnStation[i].x==0 || m_nCRHReturnStation[i].y<0)
			break;
		if(m_nCRHReturnStation[i].x==station && m_nCRHReturnStation[i].y==index)
			return true;
	}

	return false;
}
