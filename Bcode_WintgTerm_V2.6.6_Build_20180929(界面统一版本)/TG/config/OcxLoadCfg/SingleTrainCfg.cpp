#include "StdAfx.h"
#include "TGCtrl.h"
#include ".\SingleTrainCfg.h"
#import  <msxml.dll> named_guids
using namespace MSXML;

// 股道字体大小
int	SingleTrainCfg::m_sideFontSize=145;
// 到发点字体大小
int	SingleTrainCfg::m_timeFontSize=120;
// 区间分析字体大小
int SingleTrainCfg::m_analyFontSize=100;
// 早晚点时分字体
int	SingleTrainCfg::m_lateTimeFontSize=100;
// 秒字体大小
int	SingleTrainCfg::m_timeSecondFontSize=100;


SingleTrainCfg::SingleTrainCfg(void)
{
	this->m_bSchdSameToActual = false;
	this->m_bChangeVertColor=true; // 默认是改变vert颜色
	bLoadThreeLine=false;
	m_nEqualLineCount=0;
}

SingleTrainCfg::~SingleTrainCfg(void)
{
	m_vect_trainid_change.clear();
	m_vect_trainid_show_define.clear();
	m_map_line_type.clear();
	m_map_line_color.clear();
	m_map_special_color.clear();
	m_nEqualLineCount=0;
}
bool SingleTrainCfg::GetTrainLineStyle(const int factor, LINE_TYPE& style) const
{
	std::map<int, LINE_TYPE>::const_iterator it = m_map_line_type.find(factor);
	if(it != m_map_line_type.end())
	{
		style.type = (*it).second.type;
		style.shape = (*it).second.shape;
		style.decorate = (*it).second.decorate;

		return true;
	}

	return false;
}

void SingleTrainCfg::GetTrainLineColor(const int factor, LINE_COLOR& color) const
{
	WORD type = 255;
	std::map<int, LINE_TYPE>::const_iterator it = m_map_line_type.find(factor);
	if(it != m_map_line_type.end())
	{
		type = it->second.type;
	}
	else
	{
		type = this->m_unknownTrainType;
	}

	std::map<WORD, LINE_COLOR>::const_iterator clrit = this->m_map_line_color.find(type);
	if(clrit != this->m_map_line_color.end())
	{
		color.index = clrit->second.index;
		color.line_type = clrit->second.line_type;
		color.real = clrit->second.real;
		color.schd = clrit->second.schd;
		color.vert = clrit->second.vert;
		return;
	}
	else
	{
		for(int i=0; i<m_nEqualLineCount; i++)
		{
			clrit = EqualLine[i].m_map_line_color.find(type);
			if(clrit != EqualLine[i].m_map_line_color.end())
			{
				color.index = clrit->second.index;
				color.line_type = EqualLine[i].equal_line_type;
				color.real = clrit->second.real;
				color.schd = clrit->second.schd;
				color.vert = clrit->second.vert;
				return;
			}
		}
	}

	// 未知显色----配置正确的情况下,下面不会执行
	clrit = this->m_map_line_color.find(255);
	if(clrit != this->m_map_line_color.end())
	{
		color.index = clrit->second.index;
		color.line_type = clrit->second.line_type;
		color.real = clrit->second.real;
		color.schd = clrit->second.schd;
		color.vert = clrit->second.vert;
	}
}

bool SingleTrainCfg::GetTrainSpecialColor(const WORD index, SPECIAL_COLOR& color) const
{
	std::map<WORD, SPECIAL_COLOR>::const_iterator it = m_map_special_color.find(index);
	if(it != m_map_special_color.end())
	{
		color.index = it->second.index;
		color.length_rank = it->second.length_rank;
		color.military_rank = it->second.military_rank;
		color.space_rank = it->second.space_rank;
		color.super_rank = it->second.super_rank;
		return true;
	}

	return false;
}

//<TRAIN_GRAPH>
//  <DECORATE_STYLE index="1" width="1" lcolor="255-255-0" rcolor="255-255-0"/>
//  
//  <SPECIAL_LINE_STYLE>
//     	<SPACE_TRAIN 	desp="超限" decorate="0"/>
//     	<SUPER_TRAIN 	desp="专运" decorate="0"/>
//		<LENGTH_TRAIN 	desp="超长" decorate="0"/>
//		<IMPORT_TRAIN 	desp="重点" decorate="0"/>
//		<MILITARY_TRAIN desp="军用" decorate="0"/>
//		<ELEC_TRAIN 	desp="电力" decorate="1"/>			 
//  </SPECIAL_LINE_STYLE>
//
//  <LINE_TYPE type="2"> 
//     <FACTOR factor="1"/>
//     <FACTOR sfactor="1" efactor="2"/>
//  </LINE_TYPE>
//</TRAIN_GRAPH>
void SingleTrainCfg::LoadTgLineStyleXML(const CString pathname,COleControl* ctrl)
{
	CTGCtrl* pCtrl = (CTGCtrl*)ctrl;

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
		
		pCtrl->FIRE_TG_LOG(prompt);
		return; 
	}
	
	MSXML::IXMLDOMNodePtr pChildNode,pNode,pRootNode;

	pRootNode = pXMLDoc->selectSingleNode( "TRAIN_GRAPH");
	if(pRootNode == NULL)
		return;
    // LINE_SAME desp="配置实际线和计划线画法是否相同" set="0">
	pNode = pRootNode->selectSingleNode("LINE_SAME");
	if(pNode)
	{
		pChildNode = pNode->selectSingleNode("@set");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			int set = atoi(CString(v.bstrVal));	
			this->m_bSchdSameToActual = (set == 0) ? false:true;
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChildNode = pNode->selectSingleNode("@notchangevertcolor");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			int set = atoi(CString(v.bstrVal));	
			this->m_bChangeVertColor = (set >= 1)?false:true;
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}

		
	// 上一班数据修改
	//<CanChangeHistorySchd desp="上一班数据修改配置" ismodify="0" len="0"/>
	this->bCanChangeHistorySchd = false;
	this->nTimeLen = 0;
	pNode = pRootNode->selectSingleNode("CanChangeHistorySchd");
	if(pNode)
	{
		// @ismodify
		pChildNode = pNode->selectSingleNode("@ismodify");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			int set = atoi(CString(v.bstrVal));	
			this->bCanChangeHistorySchd = (set == 0) ? false:true;
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		// @len
		pChildNode = pNode->selectSingleNode("@len");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();	
			this->nTimeLen = (BYTE)atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}

	// DECORATE_STYLE
	m_decorate_style.index = 0;
	m_decorate_style.width = 0;
	m_decorate_style.rcolor = 0;
	m_decorate_style.lcolor = 0;
	pNode = pRootNode->selectSingleNode("DECORATE_STYLE");
	if(pNode)
	{
		// @index
		pChildNode = pNode->selectSingleNode("@index");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			m_decorate_style.index = (BYTE)atoi(CString(v.bstrVal));	
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		// @width
		pChildNode = pNode->selectSingleNode("@width");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			m_decorate_style.width = (BYTE)atoi(CString(v.bstrVal));	
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		// @lcolor
		pChildNode = pNode->selectSingleNode("@lcolor");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			CString rgb(v.bstrVal);
			int R,G,B;
			sscanf(rgb.GetBuffer(), "%d-%d-%d", &R,&G,&B);
			m_decorate_style.lcolor = RGB(R,G,B);	
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		// @rcolor
		pChildNode = pNode->selectSingleNode("@rcolor");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			CString rgb(v.bstrVal);
			int R,G,B;
			sscanf(rgb.GetBuffer(), "%d-%d-%d", &R,&G,&B);
			m_decorate_style.rcolor = RGB(R,G,B);	
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}

	// SPECIAL_LINE_STYLE
	pNode = pRootNode->selectSingleNode("SPECIAL_LINE_STYLE");
	if(pNode)
	{
		MSXML::IXMLDOMNodePtr pTmpNode;
		// SPACE_TRAIN
		pChildNode = pNode->selectSingleNode("SPACE_TRAIN");
		if(pChildNode)
		{
			// @decorate
			pTmpNode = pChildNode->selectSingleNode("@decorate");
			if(pTmpNode)
			{
				_variant_t v;
				v = pTmpNode->GetnodeTypedValue();
				m_special_line_style.space_rank.decorate = (BYTE)atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// @schd_width
			pTmpNode = pChildNode->selectSingleNode("@schd_width");
			if(pTmpNode)
			{
				_variant_t v;
				v = pTmpNode->GetnodeTypedValue();
				m_special_line_style.space_rank.schd_width = (BYTE)atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// @real_width
			pTmpNode = pChildNode->selectSingleNode("@real_width");
			if(pTmpNode)
			{
				_variant_t v;
				v = pTmpNode->GetnodeTypedValue();
				m_special_line_style.space_rank.real_width = (BYTE)atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// @thick_schd_width
			pTmpNode = pChildNode->selectSingleNode("@thick_schd_width");
			if(pTmpNode)
			{
				_variant_t v;
				v = pTmpNode->GetnodeTypedValue();
				m_special_line_style.space_rank.thick_schd_width = (BYTE)atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// @thick_real_width
			pTmpNode = pChildNode->selectSingleNode("@thick_real_width");
			if(pTmpNode)
			{
				_variant_t v;
				v = pTmpNode->GetnodeTypedValue();
				m_special_line_style.space_rank.thick_real_width = (BYTE)atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// @style
			pTmpNode = pChildNode->selectSingleNode("@style");
			if(pTmpNode)
			{
				_variant_t v;
				v = pTmpNode->GetnodeTypedValue();
				m_special_line_style.space_rank.style = (BYTE)atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
		}

		// SUPER_TRAIN
		pChildNode = pNode->selectSingleNode("SUPER_TRAIN");
		if(pChildNode)
		{
			// @decorate
			pTmpNode = pChildNode->selectSingleNode("@decorate");
			if(pTmpNode)
			{
				_variant_t v;
				v = pTmpNode->GetnodeTypedValue();
				m_special_line_style.super_rank.decorate = (BYTE)atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// @schd_width
			pTmpNode = pChildNode->selectSingleNode("@schd_width");
			if(pTmpNode)
			{
				_variant_t v;
				v = pTmpNode->GetnodeTypedValue();
				m_special_line_style.super_rank.schd_width = (BYTE)atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// @real_width
			pTmpNode = pChildNode->selectSingleNode("@real_width");
			if(pTmpNode)
			{
				_variant_t v;
				v = pTmpNode->GetnodeTypedValue();
				m_special_line_style.super_rank.real_width = (BYTE)atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// @style
			pTmpNode = pChildNode->selectSingleNode("@style");
			if(pTmpNode)
			{
				_variant_t v;
				v = pTmpNode->GetnodeTypedValue();
				m_special_line_style.super_rank.style = (BYTE)atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// @thick_schd_width
			pTmpNode = pChildNode->selectSingleNode("@thick_schd_width");
			if(pTmpNode)
			{
				_variant_t v;
				v = pTmpNode->GetnodeTypedValue();
				m_special_line_style.super_rank.thick_schd_width = (BYTE)atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// @thick_real_width
			pTmpNode = pChildNode->selectSingleNode("@thick_real_width");
			if(pTmpNode)
			{
				_variant_t v;
				v = pTmpNode->GetnodeTypedValue();
				m_special_line_style.super_rank.thick_real_width = (BYTE)atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
		}

		// LENGTH_TRAIN
		pChildNode = pNode->selectSingleNode("LENGTH_TRAIN");
		if(pChildNode)
		{
			// @decorate
			pTmpNode = pChildNode->selectSingleNode("@decorate");
			if(pTmpNode)
			{
				_variant_t v;
				v = pTmpNode->GetnodeTypedValue();
				m_special_line_style.length_rank.decorate = (BYTE)atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// @schd_width
			pTmpNode = pChildNode->selectSingleNode("@schd_width");
			if(pTmpNode)
			{
				_variant_t v;
				v = pTmpNode->GetnodeTypedValue();
				m_special_line_style.length_rank.schd_width = (BYTE)atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// @real_width
			pTmpNode = pChildNode->selectSingleNode("@real_width");
			if(pTmpNode)
			{
				_variant_t v;
				v = pTmpNode->GetnodeTypedValue();
				m_special_line_style.length_rank.real_width = (BYTE)atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// @style
			pTmpNode = pChildNode->selectSingleNode("@style");
			if(pTmpNode)
			{
				_variant_t v;
				v = pTmpNode->GetnodeTypedValue();
				m_special_line_style.length_rank.style = (BYTE)atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// @thick_schd_width
			pTmpNode = pChildNode->selectSingleNode("@thick_schd_width");
			if(pTmpNode)
			{
				_variant_t v;
				v = pTmpNode->GetnodeTypedValue();
				m_special_line_style.length_rank.thick_schd_width = (BYTE)atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// @thick_real_width
			pTmpNode = pChildNode->selectSingleNode("@thick_real_width");
			if(pTmpNode)
			{
				_variant_t v;
				v = pTmpNode->GetnodeTypedValue();
				m_special_line_style.length_rank.thick_real_width = (BYTE)atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
		}

		// NORMAL_TRAIN
		pChildNode = pNode->selectSingleNode("NORMAL_TRAIN");
		if(pChildNode)
		{
			// @decorate
			pTmpNode = pChildNode->selectSingleNode("@decorate");
			if(pTmpNode)
			{
				_variant_t v;
				v = pTmpNode->GetnodeTypedValue();
				m_special_line_style.normal_rank.decorate = (BYTE)atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// @schd_width
			pTmpNode = pChildNode->selectSingleNode("@schd_width");
			if(pTmpNode)
			{
				_variant_t v;
				v = pTmpNode->GetnodeTypedValue();
				m_special_line_style.normal_rank.schd_width = (BYTE)atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// @real_width
			pTmpNode = pChildNode->selectSingleNode("@real_width");
			if(pTmpNode)
			{
				_variant_t v;
				v = pTmpNode->GetnodeTypedValue();
				m_special_line_style.normal_rank.real_width = (BYTE)atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// @style
			pTmpNode = pChildNode->selectSingleNode("@style");
			if(pTmpNode)
			{
				_variant_t v;
				v = pTmpNode->GetnodeTypedValue();
				m_special_line_style.normal_rank.style = (BYTE)atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// @thick_schd_width
			pTmpNode = pChildNode->selectSingleNode("@thick_schd_width");
			if(pTmpNode)
			{
				_variant_t v;
				v = pTmpNode->GetnodeTypedValue();
				m_special_line_style.normal_rank.thick_schd_width = (BYTE)atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// @thick_real_width
			pTmpNode = pChildNode->selectSingleNode("@thick_real_width");
			if(pTmpNode)
			{
				_variant_t v;
				v = pTmpNode->GetnodeTypedValue();
				m_special_line_style.normal_rank.thick_real_width = (BYTE)atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
		}
		// MILITARY_TRAIN
		pChildNode = pNode->selectSingleNode("MILITARY_TRAIN");
		if(pChildNode)
		{
			// @decorate
			pTmpNode = pChildNode->selectSingleNode("@decorate");
			if(pTmpNode)
			{
				_variant_t v;
				v = pTmpNode->GetnodeTypedValue();
				m_special_line_style.military_rank.decorate = (BYTE)atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// @schd_width
			pTmpNode = pChildNode->selectSingleNode("@schd_width");
			if(pTmpNode)
			{
				_variant_t v;
				v = pTmpNode->GetnodeTypedValue();
				m_special_line_style.military_rank.schd_width = (BYTE)atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// @real_width
			pTmpNode = pChildNode->selectSingleNode("@real_width");
			if(pTmpNode)
			{
				_variant_t v;
				v = pTmpNode->GetnodeTypedValue();
				m_special_line_style.military_rank.real_width = (BYTE)atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// @style
			pTmpNode = pChildNode->selectSingleNode("@style");
			if(pTmpNode)
			{
				_variant_t v;
				v = pTmpNode->GetnodeTypedValue();
				m_special_line_style.military_rank.style = (BYTE)atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// @thick_schd_width
			pTmpNode = pChildNode->selectSingleNode("@thick_schd_width");
			if(pTmpNode)
			{
				_variant_t v;
				v = pTmpNode->GetnodeTypedValue();
				m_special_line_style.military_rank.thick_schd_width = (BYTE)atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// @thick_real_width
			pTmpNode = pChildNode->selectSingleNode("@thick_real_width");
			if(pTmpNode)
			{
				_variant_t v;
				v = pTmpNode->GetnodeTypedValue();
				m_special_line_style.military_rank.thick_real_width = (BYTE)atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
		}
		// ELEC_TRAIN
		pChildNode = pNode->selectSingleNode("ELEC_TRAIN");
		if(pChildNode)
		{
			// @decorate
			pTmpNode = pChildNode->selectSingleNode("@decorate");
			if(pTmpNode)
			{
				_variant_t v;
				v = pTmpNode->GetnodeTypedValue();
				m_special_line_style.elec_rank.decorate = (BYTE)atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// @schd_width
			pTmpNode = pChildNode->selectSingleNode("@schd_width");
			if(pTmpNode)
			{
				_variant_t v;
				v = pTmpNode->GetnodeTypedValue();
				m_special_line_style.elec_rank.schd_width = (BYTE)atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// @real_width
			pTmpNode = pChildNode->selectSingleNode("@real_width");
			if(pTmpNode)
			{
				_variant_t v;
				v = pTmpNode->GetnodeTypedValue();
				m_special_line_style.elec_rank.real_width = (BYTE)atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// @style
			pTmpNode = pChildNode->selectSingleNode("@style");
			if(pTmpNode)
			{
				_variant_t v;
				v = pTmpNode->GetnodeTypedValue();
				m_special_line_style.elec_rank.style = (BYTE)atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// @thick_schd_width
			pTmpNode = pChildNode->selectSingleNode("@thick_schd_width");
			if(pTmpNode)
			{
				_variant_t v;
				v = pTmpNode->GetnodeTypedValue();
				m_special_line_style.elec_rank.thick_schd_width = (BYTE)atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// @thick_real_width
			pTmpNode = pChildNode->selectSingleNode("@thick_real_width");
			if(pTmpNode)
			{
				_variant_t v;
				v = pTmpNode->GetnodeTypedValue();
				m_special_line_style.elec_rank.thick_real_width = (BYTE)atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
		}
	}

	
	// <LINE_STYLE factor="166" type="16"  shape="1" decorate="0"/> 
	// 未知列车类型
	this->m_unknownTrainType = 21;
    pNode = pRootNode->selectSingleNode( "LINE_STYLE");
	if(pNode)
	{
		pChildNode = pNode->selectSingleNode("@unknowtype");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			this->m_unknownTrainType = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}

	MSXML::IXMLDOMNodeListPtr list = pRootNode->selectNodes( "LINE_STYLE/style");
	if(NULL != list)
	{
		for(int idx = 0; idx < list->Getlength(); idx++)
		{
			pNode = list->Getitem(idx);
			if(pNode)
			{
				LINE_TYPE lineType;

				lineType.factor = -1;
				lineType.type = 9999;
				lineType.shape = 0;
				lineType.decorate = 0;

				// @factor
				pChildNode = pNode->selectSingleNode("@factor");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					lineType.factor = atoi(CString(v.bstrVal));
					SysFreeString(v.bstrVal);
					v.Clear();
				}

				// @type
				pChildNode = pNode->selectSingleNode("@type");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					lineType.type = (WORD)atoi(CString(v.bstrVal));	
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				// @shape
				pChildNode = pNode->selectSingleNode("@shape");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					lineType.shape = (DWORD)atol(CString(v.bstrVal));	
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				// @decorate
				pChildNode = pNode->selectSingleNode("@decorate");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					lineType.decorate = (BYTE)atoi(CString(v.bstrVal));	
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				
				if(lineType.factor != -1 && lineType.type != 9999)
				{
					this->m_map_line_type[lineType.factor] = lineType;
				}
			}
		}
	}
}

//<TRAIN_GRAPH>
//	<SPECIAL_COLOR index="1" space_rank="255-128-0" super_rank="128-0-128" length_rank="128-64-0" import_rank="128-64-0" military_rank="128-64-0"/>
//	<COLOR line_type="255"	index="1"	schd="100-0-0"	real="100-0-0"/>
//</TRAIN_GRAPH>
void SingleTrainCfg::LoadTgLineColorXML(const CString pathname,COleControl* ctrl)
{
	CTGCtrl* pCtrl = (CTGCtrl*)ctrl;

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
		pCtrl->FIRE_TG_LOG(prompt);
		return; 
	}
	
	MSXML::IXMLDOMNodePtr pChildNode,pNode,pRootNode,pNode1;

	pRootNode = pXMLDoc->selectSingleNode( "TRAIN_GRAPH");
	if(pRootNode == NULL)
		return;

	// SPECIAL_COLOR
	MSXML::IXMLDOMNodeListPtr list = pRootNode->selectNodes( "SPECIAL_COLOR");
	if(NULL != list)
	{
		for(int idx = 0; idx < list->Getlength(); idx++)
		{
			pNode = list->Getitem(idx);
			if(pNode)
			{
				SPECIAL_COLOR special;
				// @index
				special.index = 0;
				pChildNode = pNode->selectSingleNode("@index");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					special.index = (WORD)atoi(CString(v.bstrVal));	
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				
				MSXML::IXMLDOMNodePtr pChildTmpNode;
				// SPACE_RANK
				pChildNode = pNode->selectSingleNode("SPACE_RANK");
				if(pChildNode)
				{
					pChildTmpNode = pChildNode->selectSingleNode("@schd_clr");
					if(pChildTmpNode)
					{
						_variant_t v;
						v = pChildTmpNode->GetnodeTypedValue();
						CString rgb(v.bstrVal);
						int R,G,B;
						sscanf(rgb.GetBuffer(), "%d-%d-%d", &R,&G,&B);
				
						special.space_rank.schd_color = RGB(R,G,B);		
						SysFreeString(v.bstrVal);
						v.Clear();
					}
					pChildTmpNode = pChildNode->selectSingleNode("@real_clr");
					if(pChildTmpNode)
					{
						_variant_t v;
						v = pChildTmpNode->GetnodeTypedValue();
						CString rgb(v.bstrVal);
						int R,G,B;
						sscanf(rgb.GetBuffer(), "%d-%d-%d", &R,&G,&B);
				
						special.space_rank.real_color = RGB(R,G,B);		
						SysFreeString(v.bstrVal);
						v.Clear();
					}
				}

				// SUPER_RANK
				pChildNode = pNode->selectSingleNode("SUPER_RANK");
				if(pChildNode)
				{
					pChildTmpNode = pChildNode->selectSingleNode("@schd_clr");
					if(pChildTmpNode)
					{
						_variant_t v;
						v = pChildTmpNode->GetnodeTypedValue();
						CString rgb(v.bstrVal);
						int R,G,B;
						sscanf(rgb.GetBuffer(), "%d-%d-%d", &R,&G,&B);
				
						special.super_rank.schd_color = RGB(R,G,B);		
						SysFreeString(v.bstrVal);
						v.Clear();
					}
					pChildTmpNode = pChildNode->selectSingleNode("@real_clr");
					if(pChildTmpNode)
					{
						_variant_t v;
						v = pChildTmpNode->GetnodeTypedValue();
						CString rgb(v.bstrVal);
						int R,G,B;
						sscanf(rgb.GetBuffer(), "%d-%d-%d", &R,&G,&B);
				
						special.super_rank.real_color = RGB(R,G,B);		
						SysFreeString(v.bstrVal);
						v.Clear();
					}
				}

				// LENGTH_RANK
				pChildNode = pNode->selectSingleNode("LENGTH_RANK");
				if(pChildNode)
				{
					pChildTmpNode = pChildNode->selectSingleNode("@schd_clr");
					if(pChildTmpNode)
					{
						_variant_t v;
						v = pChildTmpNode->GetnodeTypedValue();
						CString rgb(v.bstrVal);
						int R,G,B;
						sscanf(rgb.GetBuffer(), "%d-%d-%d", &R,&G,&B);
				
						special.length_rank.schd_color = RGB(R,G,B);		
						SysFreeString(v.bstrVal);
						v.Clear();
					}
					pChildTmpNode = pChildNode->selectSingleNode("@real_clr");
					if(pChildTmpNode)
					{
						_variant_t v;
						v = pChildTmpNode->GetnodeTypedValue();
						CString rgb(v.bstrVal);
						int R,G,B;
						sscanf(rgb.GetBuffer(), "%d-%d-%d", &R,&G,&B);
				
						special.length_rank.real_color = RGB(R,G,B);		
						SysFreeString(v.bstrVal);
						v.Clear();
					}
				}

				// MILITARY_RANK
				pChildNode = pNode->selectSingleNode("MILITARY_RANK");
				if(pChildNode)
				{
					pChildTmpNode = pChildNode->selectSingleNode("@schd_clr");
					if(pChildTmpNode)
					{
						_variant_t v;
						v = pChildTmpNode->GetnodeTypedValue();
						CString rgb(v.bstrVal);
						int R,G,B;
						sscanf(rgb.GetBuffer(), "%d-%d-%d", &R,&G,&B);
				
						special.military_rank.schd_color = RGB(R,G,B);		
						SysFreeString(v.bstrVal);
						v.Clear();
					}
					pChildTmpNode = pChildNode->selectSingleNode("@real_clr");
					if(pChildTmpNode)
					{
						_variant_t v;
						v = pChildTmpNode->GetnodeTypedValue();
						CString rgb(v.bstrVal);
						int R,G,B;
						sscanf(rgb.GetBuffer(), "%d-%d-%d", &R,&G,&B);
				
						special.military_rank.real_color = RGB(R,G,B);		
						SysFreeString(v.bstrVal);
						v.Clear();
					}
				}

				if(special.index > 0)
				{
					this->m_map_special_color.insert(make_pair(special.index, special));
				}
			}
		}
	}
	
	// COLOR
	list = pRootNode->selectNodes( "COLOR");
	if(NULL != list)
	{
		for(int idx = 0; idx < list->Getlength(); idx++)
		{
			pNode = list->Getitem(idx);
			if(pNode)
			{
				LINE_COLOR color;
				// @line_type
				color.line_type = 0;
				pChildNode = pNode->selectSingleNode("@line_type");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					color.line_type = (WORD)atoi(CString(v.bstrVal));	
					SysFreeString(v.bstrVal);
					v.Clear();
				}

				// @index
				color.index = 0;
				pChildNode = pNode->selectSingleNode("@index");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					color.index = (WORD)atoi(CString(v.bstrVal));	
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				
				// @schd
				color.schd = RGB(255,0,0);
				pChildNode = pNode->selectSingleNode("@schd");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					CString rgb(v.bstrVal);
					int R,G,B;
					sscanf(rgb.GetBuffer(), "%d-%d-%d", &R,&G,&B);
					color.schd = RGB(R,G,B);	
					SysFreeString(v.bstrVal);
					v.Clear();
				}

				// @real
				color.real = RGB(255,0,0);
				pChildNode = pNode->selectSingleNode("@real");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					CString rgb(v.bstrVal);
					int R,G,B;
					sscanf(rgb.GetBuffer(), "%d-%d-%d", &R,&G,&B);
					color.real = RGB(R,G,B);	
					SysFreeString(v.bstrVal);
					v.Clear();
				}

				// @vert
				color.vert = RGB(255,0,0);
				pChildNode = pNode->selectSingleNode("@vert");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					CString rgb(v.bstrVal);
					int R,G,B;
					sscanf(rgb.GetBuffer(), "%d-%d-%d", &R,&G,&B);
					color.vert = RGB(R,G,B);	
					SysFreeString(v.bstrVal);
					v.Clear();
				}

				this->m_map_line_color.insert(make_pair(color.line_type, color));
			}
		}

		// 未知显色
		std::map<WORD, LINE_COLOR>::iterator it = this->m_map_line_color.find(255);
		if(it == this->m_map_line_color.end())
		{
			LINE_COLOR color;
			color.line_type = 255;
			color.index = 1;
			color.schd = RGB(100,0,0);
			color.real = RGB(100,0,0);
			color.vert = RGB(100,0,0);
			this->m_map_line_color.insert(make_pair(color.line_type, color));
		}
	}
	
	// EQUAL
	m_nEqualLineCount=0;
	list = pRootNode->selectNodes( "EQUAL");
	if(NULL == list)
	{
		return;
	}

	for(int idx = 0; (idx < list->Getlength()) && (m_nEqualLineCount<64); idx++)
	{
		pNode = list->Getitem(idx);
		if(pNode==NULL)
		{
			continue;
		}

		int equal_line_type = -1;
		pChildNode = pNode->selectSingleNode("@line_type");
		if(pChildNode==NULL)
		{
			continue;
		}
		_variant_t v;
		v = pChildNode->GetnodeTypedValue();
		equal_line_type = (WORD)atoi(CString(v.bstrVal));	
		SysFreeString(v.bstrVal);
		v.Clear();
						
		MSXML::IXMLDOMNodeListPtr listclr = pNode->selectNodes( "COLOR");
		if(NULL == listclr)
		{
			continue;
		}

		for(int idx1 = 0; idx1 < listclr->Getlength(); idx1++)
		{
			pNode1 = listclr->Getitem(idx1);
			if(pNode1==NULL)
			{
				continue;
			}
		
			LINE_COLOR color;
			// @line_type
			color.line_type = 0;
			pChildNode = pNode1->selectSingleNode("@line_type");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				color.line_type = (WORD)atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}

			// @index
			color.index = 0;
			pChildNode = pNode1->selectSingleNode("@index");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				color.index = (WORD)atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			
			// @schd
			color.schd = RGB(255,0,0);
			pChildNode = pNode1->selectSingleNode("@schd");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				CString rgb(v.bstrVal);
				int R,G,B;
				sscanf(rgb.GetBuffer(), "%d-%d-%d", &R,&G,&B);
				color.schd = RGB(R,G,B);	
				SysFreeString(v.bstrVal);
				v.Clear();
			}

			// @real
			color.real = RGB(255,0,0);
			pChildNode = pNode1->selectSingleNode("@real");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				CString rgb(v.bstrVal);
				int R,G,B;
				sscanf(rgb.GetBuffer(), "%d-%d-%d", &R,&G,&B);
				color.real = RGB(R,G,B);	
				SysFreeString(v.bstrVal);
				v.Clear();
			}

			// @vert
			color.vert = RGB(255,0,0);
			pChildNode = pNode1->selectSingleNode("@vert");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				CString rgb(v.bstrVal);
				int R,G,B;
				sscanf(rgb.GetBuffer(), "%d-%d-%d", &R,&G,&B);
				color.vert = RGB(R,G,B);	
				SysFreeString(v.bstrVal);
				v.Clear();
			}

			EqualLine[m_nEqualLineCount].m_map_line_color.insert(make_pair(color.line_type, color));
		}
		
		if(-1!=equal_line_type && EqualLine[m_nEqualLineCount].m_map_line_color.size()>0 && m_nEqualLineCount<64)
		{
			EqualLine[m_nEqualLineCount].equal_line_type=equal_line_type;
			m_nEqualLineCount++;
		}
	}
	return;
}

void SingleTrainCfg::LoadTgTrainIDXML(const CString pathname, COleControl* ctrl)
{
	CTGCtrl* pCtrl = (CTGCtrl*)ctrl;

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
		pCtrl->FIRE_TG_LOG(prompt);
		return; 
	}
	
	MSXML::IXMLDOMNodePtr pChildNode,pNode,pRootNode;

	pRootNode = pXMLDoc->selectSingleNode( "LINE_TRAINID");
	if(pRootNode == NULL)
		return;

	_variant_t v;
	
	// 配置列车变换车次显示
	MSXML::IXMLDOMNodeListPtr list = pRootNode->selectNodes( "SHOW/STATION");
	if(NULL != list)
	{
		for(int idx = 0; idx < list->Getlength(); idx++)
		{
			pNode = list->Getitem(idx);
			if(pNode == NULL)
				continue;
			
			TRAINID_DEFINE trainIdShowMethod;

			// @id
			trainIdShowMethod.stano = 0;
			pChildNode = pNode->selectSingleNode("@id");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				trainIdShowMethod.stano = atoi(CString(v.bstrVal));			
				SysFreeString(v.bstrVal);
				v.Clear();
			}

			// @index
			trainIdShowMethod.index = 0;
			pChildNode = pNode->selectSingleNode("@index");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				trainIdShowMethod.index = atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			
			// @arrival_method
			trainIdShowMethod.arrival_method = TRAINID_DEFINE::TRAINID::ARRIVAL;
			pChildNode = pNode->selectSingleNode("@arrival_method");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				int tmp = atoi(CString(v.bstrVal));
				if(tmp)
					trainIdShowMethod.arrival_method = TRAINID_DEFINE::TRAINID::DEPART;
				SysFreeString(v.bstrVal);
				v.Clear();
			}

			
			if(trainIdShowMethod.stano > 0)
			{
				this->m_vect_trainid_show_define.push_back(trainIdShowMethod);
			}
		}
	}
	
	// 配置自动变换车次 
	list = pRootNode->selectNodes( "CHANGE/STATION");
	if(NULL != list)
	{
		for(int idx = 0; idx < list->Getlength(); idx++)
		{
			pNode = list->Getitem(idx);
			if(pNode == NULL)
				continue;
			
			AUTO_CHANGE_TRAIN_ID trainIdAutoChg;

			// @id
			trainIdAutoChg.stano = 0;
			pChildNode = pNode->selectSingleNode("@id");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				trainIdAutoChg.stano = atoi(CString(v.bstrVal));			
				SysFreeString(v.bstrVal);
				v.Clear();
			}

			// @index
			trainIdAutoChg.index = 0;
			pChildNode = pNode->selectSingleNode("@index");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				trainIdAutoChg.index = atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
						
			if(trainIdAutoChg.stano > 0)
			{
				this->m_vect_trainid_change.push_back(trainIdAutoChg);
			}
		}
	}

	
	// 配置特别列车车次显示 
	pNode = pRootNode->selectSingleNode("SPECIAL/SPACE_TRAIN");
	if(pNode)
	{
		SetSpecialTrainIdShowStyle(pNode);
	}
	pNode = pRootNode->selectSingleNode("SPECIAL/LENGTH_TRAIN");
	if(pNode)
	{
		SetSpecialTrainIdShowStyle(pNode);
	}
	pNode = pRootNode->selectSingleNode("SPECIAL/SUPER_TRAIN");
	if(pNode)
	{
		SetSpecialTrainIdShowStyle(pNode);
	}
	pNode = pRootNode->selectSingleNode("SPECIAL/MILITARY_TRAIN");
	if(pNode)
	{
		SetSpecialTrainIdShowStyle(pNode);
	}
	pNode = pRootNode->selectSingleNode("SPECIAL/ELEC_TRAIN");
	if(pNode)
	{
		SetSpecialTrainIdShowStyle(pNode);
	}
	pNode = pRootNode->selectSingleNode("SPECIAL/NOMORL_TRAIN");
	if(pNode)
	{
		SetSpecialTrainIdShowStyle(pNode);
	}

	pNode = pRootNode->selectSingleNode("TIME_FONT");
	if(pNode)
	{
		_variant_t v;
		v = pNode->GetnodeTypedValue();
		int fontSize = atoi(CString(v.bstrVal));
		if(fontSize>50 && fontSize<200)
		{
			this->m_timeFontSize=fontSize;
		}
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	pNode = pRootNode->selectSingleNode("SECOND_TIME_FONT");
	if(pNode)
	{
		_variant_t v;
		v = pNode->GetnodeTypedValue();
		int fontSize = atoi(CString(v.bstrVal));
		if(fontSize>50 && fontSize<200)
		{
			this->m_timeSecondFontSize=fontSize;
		}
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	pNode = pRootNode->selectSingleNode("SIDE_FONT");
	if(pNode)
	{
		_variant_t v;
		v = pNode->GetnodeTypedValue();
		int fontSize = atoi(CString(v.bstrVal));
		if(fontSize>50 && fontSize<200)
		{
			this->m_sideFontSize=fontSize;
		}
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	pNode = pRootNode->selectSingleNode("ANALY_TIME_FONT");
	if(pNode)
	{
		_variant_t v;
		v = pNode->GetnodeTypedValue();
		int fontSize = atoi(CString(v.bstrVal));
		if(fontSize>50 && fontSize<200)
		{
			this->m_analyFontSize=fontSize;
		}
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	pNode = pRootNode->selectSingleNode("LATE_TIME_FONT");
	if(pNode)
	{
		_variant_t v;
		v = pNode->GetnodeTypedValue();
		int fontSize = atoi(CString(v.bstrVal));
		if(fontSize>50 && fontSize<200)
		{
			this->m_lateTimeFontSize=fontSize;
		}
		SysFreeString(v.bstrVal);
		v.Clear();
	}
}

void SingleTrainCfg::SetSpecialTrainIdShowStyle(MSXML::IXMLDOMNodePtr pNode)
{
	CString desp;
	CString tail;
	int  weight=400;
	int  height=12;
	int  drawrect = 0;
	MSXML::IXMLDOMNodePtr pChildNode;
	_variant_t v;
	pChildNode = pNode->selectSingleNode("@desp");
	if(pChildNode)
	{
		v = pChildNode->GetnodeTypedValue();
		desp = v.bstrVal;
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	pChildNode = pNode->selectSingleNode("@fixtail");
	if(pChildNode)
	{
		v = pChildNode->GetnodeTypedValue();
		tail = v.bstrVal;
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	pChildNode = pNode->selectSingleNode("@font_weight");
	if(pChildNode)
	{
		v = pChildNode->GetnodeTypedValue();
		weight = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	pChildNode = pNode->selectSingleNode("@font_height");
	if(pChildNode)
	{
		v = pChildNode->GetnodeTypedValue();
		height = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	pChildNode = pNode->selectSingleNode("@drawrect");
	if(pChildNode)
	{
		v = pChildNode->GetnodeTypedValue();
		drawrect = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	if(desp == "超限")
	{
		this->m_special_train_show.space_rank.desp = "超限";
		this->m_special_train_show.space_rank.tail = tail;
		this->m_special_train_show.space_rank.weight = weight;
        this->m_special_train_show.space_rank.height = height;
		this->m_special_train_show.space_rank.drawrect = (drawrect==0)? false:true;
	}
	else if(desp == "超长")
	{
		this->m_special_train_show.length_rank.desp = "超长";
		this->m_special_train_show.length_rank.tail = tail;
		this->m_special_train_show.length_rank.weight = weight;
        this->m_special_train_show.length_rank.height = height;
		this->m_special_train_show.length_rank.drawrect = (drawrect==0)? false:true;
	}
	else if(desp == "重点")
	{
		this->m_special_train_show.super_rank.desp = "重点";
		this->m_special_train_show.super_rank.tail = tail;
		this->m_special_train_show.super_rank.weight = weight;
        this->m_special_train_show.super_rank.height = height;
		this->m_special_train_show.super_rank.drawrect = (drawrect==0)? false:true;
	}
	else if(desp == "军用")
	{
		this->m_special_train_show.military_rank.desp = "军用";
		this->m_special_train_show.military_rank.tail = tail;
		this->m_special_train_show.military_rank.weight = weight;
        this->m_special_train_show.military_rank.height = height;
		this->m_special_train_show.military_rank.drawrect = (drawrect==0)? false:true;
	}
	else if(desp == "电力")
	{
		this->m_special_train_show.elec_rank.desp = "电力";
		this->m_special_train_show.elec_rank.tail = tail;
		this->m_special_train_show.elec_rank.weight = weight;
        this->m_special_train_show.elec_rank.height = height;
		this->m_special_train_show.elec_rank.drawrect = (drawrect==0)? false:true;
	}
	else if(desp == "一般列车")
	{
		this->m_special_train_show.nomorl_rank.desp = "一般列车";
		this->m_special_train_show.nomorl_rank.tail = tail;
		this->m_special_train_show.nomorl_rank.weight = weight;
        this->m_special_train_show.nomorl_rank.height = height;
		this->m_special_train_show.nomorl_rank.drawrect = (drawrect==0)? false:true;
	}
}

const TRAINID_DEFINE* SingleTrainCfg::GetTrainIdDefine(const int stano)
{
	for(UINT idx=0; idx<m_vect_trainid_show_define.size(); ++idx)
	{
		if(stano == this->m_vect_trainid_show_define[idx].stano)
		{
			return &this->m_vect_trainid_show_define[idx];
		}
	}
	
    return NULL;
}

bool SingleTrainCfg::IsAutoChangeTrainId(int station, int index)
{
	for(UINT idx=0; idx<m_vect_trainid_change.size(); ++idx)
	{
		if(station == this->m_vect_trainid_change[idx].stano 
			&& index  == this->m_vect_trainid_change[idx].index-1)
		{
			return true;
		}
	}
	
    return false;
}

void SingleTrainCfg::LoadTgThreeLineXML(const CString pathname)
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
	
	MSXML::IXMLDOMNodePtr pChildNode,pNode,pRootNode, pNodeEle;

	pRootNode = pXMLDoc->selectSingleNode( "THREE_LINE");
	if(pRootNode == NULL)
		return;

	_variant_t v;
	
	int id=0;
	pChildNode = pRootNode->selectSingleNode("@id");
	if(pChildNode)
	{
		_variant_t v;
		v = pChildNode->GetnodeTypedValue();
		id = atoi(CString(v.bstrVal));			
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	
	// 配置三线变换
	MSXML::IXMLDOMNodeListPtr list = pRootNode->selectNodes( "LINE_LIST");
	if(NULL == list)
		return;
	
	for(int idx = 0; idx < list->Getlength(); idx++)
	{
		pNode = list->Getitem(idx);
		if(pNode == NULL)
			continue;
		
		THREE_LINE line_list;
		memset(&line_list, 0, sizeof(THREE_LINE));
		line_list.count1=0;
		line_list.count2=0;

		MSXML::IXMLDOMNodeListPtr list_ele = pNode->selectNodes( "ELE");
		if(NULL == list_ele)
			continue;
		int ele_count=list_ele->Getlength();
		if(ele_count<=0 || ele_count>3)
		{
			continue;
		}
		for(int id = 0; id < list_ele->Getlength(); id++)
		{
			pNodeEle = list_ele->Getitem(id);
			if(pNodeEle == NULL)
				break;

			// @station
			int station=0;
			pChildNode = pNodeEle->selectSingleNode("@station");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				station = atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}

			STA_INFO part1;
			part1.InitFlag();
			part1.station = station;
			// @entry_index1
			pChildNode = pNodeEle->selectSingleNode("@entry_index1");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				part1.arrive_index = atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// @exit_index1
			pChildNode = pNodeEle->selectSingleNode("@exit_index1");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				part1.depart_index = atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// @entry1
			pChildNode = pNodeEle->selectSingleNode("@entry1");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				part1.entry = atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// @exit1
			pChildNode = pNodeEle->selectSingleNode("@exit1");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				part1.exit = atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// @side1
			pChildNode = pNodeEle->selectSingleNode("@side1");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				part1.sideno = atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// @runtime1
			pChildNode = pNodeEle->selectSingleNode("@runtime1");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				part1.run_time = atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			if(part1.entry>=0 && part1.exit>=0 && part1.arrive_index>=0 && part1.depart_index>=0)
			{
				line_list.part1[line_list.count1]=part1;
				line_list.count1++;
			}

			STA_INFO part2;
			part2.InitFlag();
			part2.station = station;
			// @entry_index2
			pChildNode = pNodeEle->selectSingleNode("@entry_index2");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				part2.arrive_index = atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// @exit_index2
			pChildNode = pNodeEle->selectSingleNode("@exit_index2");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				part2.depart_index = atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// @entry2
			pChildNode = pNodeEle->selectSingleNode("@entry2");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				part2.entry = atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// @exit2
			pChildNode = pNodeEle->selectSingleNode("@exit2");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				part2.exit = atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// @side2
			pChildNode = pNodeEle->selectSingleNode("@side2");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				part2.sideno = atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// @runtime2
			pChildNode = pNodeEle->selectSingleNode("@runtime2");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				part2.run_time = atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			if(part2.entry>=0 && part2.exit>=0 && part2.arrive_index>=0 && part2.depart_index>=0)
			{
				line_list.part2[line_list.count2]=part2;
				line_list.count2++;
			}
		}
		if(line_list.count1>=2 && line_list.count2>=2)
		{
			vetThreeLine.push_back(line_list);
		}
	}
}

void SingleTrainCfg::LoadAbsLineDespFile(const CString pathname, COleControl* ctrl)
{
	CTGCtrl* pCtrl = (CTGCtrl*)ctrl;

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
		prompt.Format("配置 %s 不存在或者不符合xml语法(可以用浏览器检查是否能打开)", pathname);
		//AfxMessageBox(prompt);
		pCtrl->FIRE_TG_LOG(prompt);
		return; 
	}
	
	MSXML::IXMLDOMNodePtr pChildNode,pNode,pRootNode;

	pRootNode = pXMLDoc->selectSingleNode( "BUREAU");
	if(pRootNode == NULL)
		return;

	// CON_ID
	MSXML::IXMLDOMNodeListPtr list = pRootNode->selectNodes( "CON_ID");
	if(NULL == list)
		return;

	for(int idx = 0; idx < list->Getlength(); idx++)
	{
		pNode = list->Getitem(idx);
		if(NULL == pNode)
			continue;
			
		ConLineDesp absdesp;
		absdesp.station_num=0;

		// @id
		int con_id=0;
		pChildNode = pNode->selectSingleNode("@id");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			con_id = atoi(CString(v.bstrVal));	
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		// @name
		char name[100];
		memset(name, 0, 100);
		pChildNode = pNode->selectSingleNode("@name");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			CString tmp = CString(v.bstrVal);
			strncpy(name, tmp.GetBuffer(0), 99);
			name[99]=0;
			SysFreeString(v.bstrVal);
			v.Clear();
		}

		MSXML::IXMLDOMNodeListPtr liststa = pNode->selectNodes( "STATION");
		if(NULL == liststa)
			continue;

		for(int idx = 0; (idx < liststa->Getlength()) && (absdesp.station_num < 16); idx++)
		{
			pNode = liststa->Getitem(idx);
			if(NULL == pNode)
				continue;

			// @id
			absdesp.station[absdesp.station_num]=0;
			pChildNode = pNode->selectSingleNode("@id");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				absdesp.station[absdesp.station_num] = atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
				
			// 出入口个数为0
			absdesp.sta_exit_desp[absdesp.station_num].exit_num=0;

			MSXML::IXMLDOMNodeListPtr listexit = pNode->selectNodes( "EXIT");
			if(NULL == listexit)
				continue;

			for(int idx = 0; idx < listexit->Getlength(); idx++)
			{
				pNode = listexit->Getitem(idx);
				if(NULL == pNode)
					continue;

				// @id
				int exit=0;
				pChildNode = pNode->selectSingleNode("@id");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					exit = atoi(CString(v.bstrVal));	
					SysFreeString(v.bstrVal);
					v.Clear();
				}

				// @absname
				CString absname;
				pChildNode = pNode->selectSingleNode("@absname");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					absname = CString(v.bstrVal);
					SysFreeString(v.bstrVal);
					v.Clear();
				}

				if(exit>0 && !absname.IsEmpty())
				{
					int num = absdesp.sta_exit_desp[absdesp.station_num].exit_num;
					absdesp.sta_exit_desp[absdesp.station_num].exit[num]=exit;
					strncpy(absdesp.sta_exit_desp[absdesp.station_num].absname[num], absname.GetBuffer(0), 63);
					absdesp.sta_exit_desp[absdesp.station_num].absname[num][63]=0;
					absdesp.sta_exit_desp[absdesp.station_num].exit_num++;
				}
			}

			if((absdesp.station[absdesp.station_num] == 0) || (absdesp.sta_exit_desp[absdesp.station_num].exit_num<=0))
			{
				continue;
			}
			absdesp.station_num++;

		}

		if(absdesp.station_num>0&&con_id>0)
		{
			m_mapLineDesp.insert(make_pair(con_id, absdesp));
		}
	}	
	
	return;
}

CString SingleTrainCfg::GetAbsLineDesp(const int con_id, const int station, const int exit)
{
	std::map<int, ConLineDesp>::iterator it = m_mapLineDesp.find(con_id);
	if(it != m_mapLineDesp.end())
	{
		for(int i=0; i<it->second.station_num; i++)
		{
			if(station == it->second.station[i])
			{
				for(int k=0; k<it->second.sta_exit_desp[i].exit_num; k++)
				{
					if(exit == it->second.sta_exit_desp[i].exit[k])
					{
						return it->second.sta_exit_desp[i].absname[k];
					}
				}
				break;
			}
		}
	}

	return "";
}

void SingleTrainCfg::AddAbsLineDesp(int conid, int station, int exit, char desp[])
{
	std::map<int, ConLineDesp>::iterator it = m_mapLineDesp.find(conid);
	if(it == m_mapLineDesp.end())
	{
		ConLineDesp ss;
		ss.station_num=1;
		ss.station[0]=station;
		ss.sta_exit_desp[0].exit_num=1;
		ss.sta_exit_desp[0].exit[0]=exit;
		strncpy(ss.sta_exit_desp[0].absname[0], desp, 63);
		ss.sta_exit_desp[0].absname[0][63]=0;

		m_mapLineDesp[conid]=ss;
	}
	else
	{
		for(int i=0; i<it->second.station_num; i++)
		{
			if(it->second.station[i]==station)
			{
				int count=it->second.sta_exit_desp[i].exit_num;
				it->second.sta_exit_desp[i].exit[count]=exit;
				strncpy(it->second.sta_exit_desp[i].absname[count], desp, 63);
				it->second.sta_exit_desp[i].absname[count][63]=0;
				it->second.sta_exit_desp[i].exit_num++;
				break;
			}
		}
		if(i>=it->second.station_num)
		{
			int count=it->second.station_num;
			it->second.station[count]=station;
			it->second.sta_exit_desp[count].exit_num=1;
			it->second.sta_exit_desp[count].exit[0]=exit;
			strncpy(it->second.sta_exit_desp[count].absname[0], desp, 63);
			it->second.sta_exit_desp[count].absname[0][63]=0;
			it->second.station_num++;
		}
	}
}

