#pragma once

#include <map>
#include <vector>
#include <afxtempl.h>
using namespace std;
#import  <msxml.dll> named_guids
using namespace MSXML;
struct DECORATE_STYLE
{
	BYTE index;
	BYTE width;
	COLORREF lcolor;
	COLORREF rcolor;
};

struct SLS
{
	BYTE style;
	BYTE decorate;  
	BYTE schd_width;
	BYTE real_width;
	BYTE thick_schd_width;
	BYTE thick_real_width;
	SLS()
	{
		style = 0;
		decorate = 0;
		schd_width = 1;
		real_width = 2;

		thick_schd_width=2;
		thick_real_width=3;
	}
};

struct SPECIAL_LINE_STYLE
{
	SLS space_rank;
	SLS super_rank;
	SLS length_rank;
	SLS military_rank;
	SLS elec_rank;
	SLS normal_rank;
};

struct LINE_TYPE
{
	WORD  type;
	DWORD shape;
	BYTE  decorate;
	int   factor;
};

struct SLCLR
{
	COLORREF schd_color;
	COLORREF real_color;
	SLCLR()
	{
		schd_color = RGB(255,255,0);
		real_color = RGB(255,255,0);
	}
};

struct SPECIAL_COLOR
{
	WORD index;
	SLCLR space_rank;
	SLCLR super_rank;
	SLCLR length_rank;
	SLCLR military_rank;
};

struct LINE_COLOR
{
	WORD line_type;
	WORD index;
	COLORREF schd;
	COLORREF real;
	COLORREF vert;
	LINE_COLOR()
	{
		this->line_type = 0;
		this->index = 0;
		this->schd = RGB(255,0,0);
		this->real = RGB(255,0,0);
		this->vert = RGB(255,0,0);
	}
	LINE_COLOR(const LINE_COLOR& rhs)
	{
		this->line_type = rhs.line_type;
		this->index = rhs.index;
		this->schd = rhs.schd;
		this->real = rhs.real;
		this->vert = rhs.vert;
	}
};

struct SPECIAL_TRAIN_ID_SHOW
{
	struct tag_desp
	{
		CString desp;
		CString tail;
		int     weight;
		int     height;
		bool    drawrect;

		tag_desp()
		{
			weight = 700;
			height = 12;
			drawrect = false;
		}
	};

	tag_desp space_rank;
	tag_desp length_rank;
	tag_desp super_rank;
	tag_desp military_rank;
	tag_desp elec_rank;
	tag_desp nomorl_rank;
};

struct TRAINID_DEFINE
{
	int  index; // 索引号---index为TG中[SECTION_NODE]的车站序号
	int  stano; // 站号
	int  arrival_method;// 显示方法 0: 默认值,显示接入车次 1: 显示出发车次号
	enum TRAINID
	{
		ARRIVAL = 0,
		DEPART  = 1,
	};
};

struct AUTO_CHANGE_TRAIN_ID
{
	int index;  // 索引号---index为TG中[SECTION_NODE]的车站序号
	int stano;  // 开始站号
};
class SingleTrainCfg
{
public:
	SingleTrainCfg(void);
	~SingleTrainCfg(void);
public:
	void LoadTgLineStyleXML(const CString pathname);
	void LoadTgLineColorXML(const CString pathname);
	void LoadTgTrainIDXML(const CString pathname);
private:
	void SetSpecialTrainIdShowStyle(MSXML::IXMLDOMNodePtr pNode);
public:
	DECORATE_STYLE& GetDecorateStyle(){ return m_decorate_style; }
	bool IsSchdLineSameToActualLine(){return m_bSchdSameToActual;}
	bool IsChangeVertColor(){return m_bChangeVertColor;}
	SPECIAL_TRAIN_ID_SHOW& GetSpecialTrainIdShowStyle() { return m_special_train_show;}
    const TRAINID_DEFINE* GetTrainIdDefine(const int staid);
	void SetAutoChangeTrainId(int index, int stano);
	bool IsAutoChangeTrainId(int station, int index);
public:
	bool CanChangeHistorySchd(BYTE& hour){ hour = nTimeLen; return bCanChangeHistorySchd;}
	
	bool GetTrainLineStyle(const int type, LINE_TYPE& style) const;
	bool GetTrainLineColor(const int factor, LINE_COLOR& color) const;
	bool GetTrainSpecialColor(const WORD index, SPECIAL_COLOR& color) const;
	SPECIAL_LINE_STYLE& GetSpecialLineStyle(){ return m_special_line_style;}
private:
	int m_nEqualLineCount;
	struct EqualLineColor
	{
		WORD equal_line_type;
		std::map<WORD, LINE_COLOR> m_map_line_color;
	}EqualLine[64];

	BYTE m_unknownTrainType;
	bool m_bSchdSameToActual;
	bool m_bChangeVertColor;
	std::map<int,  LINE_TYPE> m_map_line_type;//m_vector_line_type;
	std::map<WORD, LINE_COLOR> m_map_line_color;
	std::map<WORD, SPECIAL_COLOR> m_map_special_color;
	DECORATE_STYLE m_decorate_style;
	SPECIAL_LINE_STYLE m_special_line_style;

	// 运行线显示车次定义
	std::vector<TRAINID_DEFINE> m_vect_trainid_show_define;
    // 运行线变更车次定义
	std::vector<AUTO_CHANGE_TRAIN_ID> m_vect_trainid_change;
	// 运行线车次显示定义---大小 字体
	SPECIAL_TRAIN_ID_SHOW m_special_train_show;

	// 是否可以修改上一班历史图
	bool bCanChangeHistorySchd;
	BYTE nTimeLen;
};
