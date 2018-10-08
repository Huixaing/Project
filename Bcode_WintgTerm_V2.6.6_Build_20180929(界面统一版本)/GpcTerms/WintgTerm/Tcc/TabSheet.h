
#pragma once

#define MAXPAGE 100
class CTabSheet : public CTabCtrl
{
	DECLARE_DYNAMIC(CTabSheet)
public:
	CTabSheet();
	virtual ~CTabSheet();

private:
	INT m_nNumOfPages;
	INT m_nCurrentPage;
	CDialog* m_pPages[MAXPAGE];
	UINT     m_IDD[MAXPAGE];
	CString  m_Title[MAXPAGE];

public:
	BOOL AddPage(LPCTSTR title, CDialog *pDialog,UINT ID);
	void SelectPage(int nSel);
	void Show();
	void SetRect();

	DECLARE_MESSAGE_MAP();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point) ;
};
