#pragma once

// nanchang多级菜单使用 add by qiaoyt

class CComboBoxExt : public CComboBox
{
	DECLARE_DYNAMIC(CComboBoxExt)

public:
	CComboBoxExt();
	virtual ~CComboBoxExt();

protected:
	DECLARE_MESSAGE_MAP()
public:
	void SetRecvMsgWnd(CWnd* pWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	CString GetDCmdFileName(int idx);
private:
	void GenPopContentMenu(CMenu* pMenu,int index);
private:
	CWnd* mpMsgRecvWnd;
	CMenu mMenu;
	int   mIndex;
	CStringArray mMenuCfgInfoAry;
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};


