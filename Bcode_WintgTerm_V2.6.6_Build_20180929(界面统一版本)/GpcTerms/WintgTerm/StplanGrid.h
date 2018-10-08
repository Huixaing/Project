#pragma once
#include <vector>
#include "GridCtrl_src/GridCtrl.h"

class CStplanGrid : public CGridCtrl
{
	DECLARE_DYNAMIC(CStplanGrid)
	DECLARE_MESSAGE_MAP()

public:
	CStplanGrid(void);
	virtual ~CStplanGrid(void);

public:
	BOOL  CreateGrid(CWnd *parent, const RECT &rect, UINT ctrlid);
	
	void  ClearView();
	void  AddWarnInfo(int nLevel, CString& text);
	void  DisplayWarnInfo();
    bool  HaveStplanWarnInfo();
private:
	void  AutoWidth();

private:
	class WarnType
	{
	public:
		COLORREF clr;
		CStringArray list;
		std::vector<int> id_range;
	};
	// 报警级别分为三级：一级，二级和其他
	WarnType m_class1,m_class2,m_class;
	CWnd* m_parent;
	BOOL  m_bSingleLine;
};
