#include "stdafx.h"
#include "locoinput.h"
#include "tg_basicpara.h"
#define  IDC_CBO_LOCO_TYPE 7001
#define  IDC_ED_LOCO_NO    7002
BOOL  CLocoInputCtrl::CreateMe(CWnd *parent,const CRect &rect, int con_id)
{
	m_con_id = con_id;
	CRect r=rect;
	r.right=r.left+rect.Width()*0.66-1;
	r.bottom=r.top+rect.Height()*4;
	r.OffsetRect(0,1); 
	m_cboType.Create(WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST|WS_VSCROLL,r,parent,IDC_CBO_LOCO_TYPE);
	r=rect;
	r.left+=rect.Width()*0.66+1;
	r.DeflateRect(0,1); 
	r.OffsetRect(0,1); 
	m_edNo.Create(WS_CHILD|WS_VISIBLE,r,parent,IDC_ED_LOCO_NO);
	m_edNo.SetLimitText(5);
	
	for(int i=0;i<256;i++)
	{
		int locoid;
		CString loconame=GetTGBasicParam()->GetLocoName(m_con_id, i, locoid);
		if(!loconame.IsEmpty())
		{
			int sel=m_cboType.AddString(loconame);
			if(sel>=0)
				m_cboType.SetItemData(sel,locoid);
		}
		else
			break;
	}
	m_cboType.SetCurSel(0);
	return TRUE;
}
void CLocoInputCtrl::Reset()
{
	 m_edNo.SetWindowText(""); 
}
void CLocoInputCtrl::SetLocoNo(const char *engineno)
{
	 CString  src=engineno;
	 int id=atoi(src.Mid(0,3));
	 for(int i=0;i<m_cboType.GetCount();i++)
	 {
		 if(m_cboType.GetItemData(i)==id)
		 {
			 m_cboType.SetCurSel(i);
			 break;
		 }
	 }
	 m_edNo.SetWindowText(src.Mid(3));  
}

const char *CLocoInputCtrl::GetLocoNo()
{
	m_result="";
	CString nostr;
	m_edNo.GetWindowText(nostr);
	nostr=nostr.Trim();
	if(nostr.IsEmpty())
		return (const char *)m_result;
	int sel=m_cboType.GetCurSel();
	if(sel>=0)
	{
		m_result.Format("%03d",	m_cboType.GetItemData(sel));
	}
	else
	{
		AfxMessageBox("必须选择机车类型");
		return NULL;
	}
	
	if(nostr.GetLength()>5)
	{
		AfxMessageBox("机车号码长度不能超过5");
		return NULL;
	}
	for(int i=0;i<nostr.GetLength();i++)
	{
		char ch=nostr.GetAt(i);
		if(!('0'<=ch && ch<='9'))
		{
			AfxMessageBox("机车号码必须由数字组成");
			return NULL;
		}
	}
	m_result+=nostr;
	return (const char *)m_result;
}