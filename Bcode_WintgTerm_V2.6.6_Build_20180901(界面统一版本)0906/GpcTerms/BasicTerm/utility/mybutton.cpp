#include "stdafx.h"
#include "mybutton.h"

BEGIN_MESSAGE_MAP(CMyButton, CButton)
	//{{AFX_MSG_MAP(CMyButton)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void   CMyButton::OnLButtonDown(UINT   nFlags,   CPoint   point)   
  {   
    m_pNotifyWindow->SendMessage(WM_COMMAND,GetDlgCtrlID(),0);
  }   
    


