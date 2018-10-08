#pragma once

class CMyButton:public CButton{
	CWnd *m_pNotifyWindow;
public:
	void   SetNotifyWindow(CWnd *p){m_pNotifyWindow=p;}
	DECLARE_MESSAGE_MAP()
	afx_msg void   OnLButtonDown(UINT   nFlags,   CPoint   point);
};