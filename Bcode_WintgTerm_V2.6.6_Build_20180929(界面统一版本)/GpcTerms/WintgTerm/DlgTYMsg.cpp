// DlgWait.cpp : implementation file
//

#include "stdafx.h"

#include "DlgTYMsg.h"
#include ".\dlgtymsg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgTYMsg dialog

CDlgTYMsg::CDlgTYMsg(CTgDocument *pdoc,CWnd* parent)
: CDialog(CDlgTYMsg::IDD, parent)
{
	Create(CDlgTYMsg::IDD, parent);
	ShowWindow(SW_HIDE);
	m_pDoc = pdoc;
}

CDlgTYMsg::~CDlgTYMsg()
{
	DestroyWindow();
}

void CDlgTYMsg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTYMsg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgTYMsg, CDialog)
	//{{AFX_MSG_MAP(CDlgTYMsg)
	// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_TYMSG_FINISH, OnBnClickedTymsgFinish)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTYMsg message handlers

void  CDlgTYMsg::UpdateDisplay(const char *str)
{
	this->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_TYMSG_TEXT)->SetWindowText(str);
	UpdateWindow();
}

void CDlgTYMsg::SetNotReady()
{
	m_bReady=false;
}

void CDlgTYMsg::SetDlgWindowFocus()
{
	CWnd* parent=GetParent();
	CRect  rect_scr,rect_w;
	parent->GetClientRect(&rect_scr); 
	GetWindowRect(&rect_w);
	int x=rect_scr.Width()/2-rect_w.Width()/2;
	int y=rect_scr.Height()/2-rect_w.Height()/2;
	if(x<=0 || y<=0)
	{
		x=500;
		y=300;
	}
	SetWindowPos(parent, x, y, rect_w.Width(), rect_w.Height(),0);
}

void CDlgTYMsg::SetThisPacketInfo(int src, CString msgid, int count, CString jht)
{
	m_nSRCCODE=src;
	m_nMSGID=msgid;
	m_nJHTNM=jht;
	m_nCount=count;
	m_SeqList.clear();
	m_bReady=true;
}

bool CDlgTYMsg::AddSeq(int src, CString msgid, int count, int nSeq)
{
	if(m_nSRCCODE!=src)
		return false;
	if(m_nMSGID!=msgid)
		return false;
	if(m_nCount!=count)
		return false;
    
	for(int i=0; i<m_SeqList.size(); i++)
	{
		if(m_SeqList[i]==nSeq)
			break;
	}
	if(i>=m_SeqList.size())
	   m_SeqList.push_back(nSeq);

	return true;
}

void CDlgTYMsg::OnBnClickedTymsgFinish()
{
	m_bReady=false;

	if (m_SeqList.size() == m_nCount)
		m_pDoc->SendGpcAckMsg(TD_GPC_ACK_SPEC, true, m_nSRCCODE, m_nMSGID, m_nJHTNM);
	else 
		m_pDoc->SendGpcAckMsg(TD_GPC_ACK_SPEC, false, m_nSRCCODE, m_nMSGID, m_nJHTNM);

	ShowWindow(SW_HIDE);
	//CDialog::OnOK();
}
