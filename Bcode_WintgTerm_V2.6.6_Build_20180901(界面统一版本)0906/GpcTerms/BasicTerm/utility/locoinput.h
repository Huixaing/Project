#pragma once
class CLocoInputCtrl
{
	CComboBox   m_cboType;
	CEdit       m_edNo;
	CString     m_result;
	int m_con_id;
public:
	BOOL  CreateMe(CWnd *parent,const CRect &rect,int con_id);
	const char *GetLocoNo();
	void  SetLocoNo(const char *engineno);
	void  Reset();
public:
};

