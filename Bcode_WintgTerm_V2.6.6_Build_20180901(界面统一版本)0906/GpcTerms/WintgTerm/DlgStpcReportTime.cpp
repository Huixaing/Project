// DlgStpcReportTime.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgStpcReportTime.h"
#include "function.h"
//#include "widgets.h"
 #include <baseMsg\msgid.h>
#include <baseMsg\clbth.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern WriteMessage(PROCESSID usProcessId,ushort usQueueHandle,
		ushort count,char *buffer);

//#define SELECTSCHDTRAINCOLUMNS		4
#define ITEMNUM  6
_TCHAR *_gszStpcReportTimeLabel[ITEMNUM] =
{
	_T("序号"),_T("车站"),_T("车次号"),_T("股道"),_T("到/发"),_T("时      间")
};
int _gnStpcReportTimeFmt[ITEMNUM] = 
{
	LVCFMT_LEFT,LVCFMT_LEFT, LVCFMT_LEFT,LVCFMT_LEFT, LVCFMT_CENTER,LVCFMT_CENTER//,LVCFMT_CENTER,LVCFMT_CENTER
};

int _gnStpcReportTimeWidth[ITEMNUM] = 
{
	40,40,60,40,60,100//,90,90
};
//TCHAR rgtsz[ITEMNUM][20] = {_T("序号"),_T("车站"),_T("车次号"),_T("股道"),_T("到/发"),_T("时      间")};

/////////////////////////////////////////////////////////////////////////////
// CDlgStpcReportTime dialog


CDlgStpcReportTime::CDlgStpcReportTime(CWintgDoc *pDoc,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgStpcReportTime::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgStpcReportTime)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pDoc = pDoc;
	m_bFirstClick = m_bSecondClick = FALSE;
	
}


void CDlgStpcReportTime::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgStpcReportTime)
	DDX_Control(pDX, IDC_REPORT_RECORD, m_list);
	DDX_Control(pDX, IDC_CMB_STATION, m_cmbStation);
	//}}AFX_DATA_MAP
//	DDX_GridControl(pDX, IDC_CUST_GRID, m_Grid);
}


BEGIN_MESSAGE_MAP(CDlgStpcReportTime, CDialog)
	//{{AFX_MSG_MAP(CDlgStpcReportTime)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDOK, OnOk)
	ON_NOTIFY(NM_DBLCLK, IDC_REPORT_RECORD, OnDblclkReportRecord)
	ON_NOTIFY(HDN_ITEMDBLCLICK, IDC_REPORT_RECORD, OnItemdblclickReportRecord)
	ON_CBN_SELCHANGE(IDC_CMB_STATION, OnSelchangeCmbStation)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_REPORT_RECORD, OnItemchangedReportRecord)
	ON_NOTIFY(NM_RDBLCLK, IDC_REPORT_RECORD, OnRdblclkReportRecord)
	//}}AFX_MSG_MAP
	ON_MESSAGE(UM_STPC_REPORT_TIME_MSG,OnStpcReportTime)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgStpcReportTime message handlers

void CDlgStpcReportTime::OnDestroy() 
{
	CDialog::OnDestroy();
 	
	// TODO: Add your message handler code here
	
}

BOOL CDlgStpcReportTime::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if(!gpcdef.GetOperatorRight(WINTG_EDIT))
	{
		CWnd *pWnd;
		pWnd = GetDlgItem(IDOK);
		pWnd->EnableWindow(FALSE);
		pWnd = GetDlgItem(IDC_DELETE);
		pWnd->EnableWindow(FALSE);
	}
//	VERIFY(m_list.SubclassDlgItem(IDC_REPORT_RECORD,this));
	m_list.SetHeaderLabel(ITEMNUM,_gszStpcReportTimeLabel,
		_gnStpcReportTimeWidth,_gnStpcReportTimeFmt);
//	STPC_REPORT_RECORD rec;
//	m_pDoc->m_vectorReportTime.resize( 20 );
/* 	for(int k=0;k<100;k++){
       rec.station = k;
	   strcpy(rec.train_id,"ss");
	   rec.side = 0;
	   rec.a_d_flag = 1;
	   rec.usDate = 0;
	   rec.usTime = 0;
	   m_pDoc->m_vectorReportTime.insert(m_pDoc->m_vectorReportTime.begin(),rec);
	}
*/ 
//    m_pDoc->m_vectorReportTime.pop_back();
	for(int i=0;i < m_pDoc->m_nStationNum;i++){
     	m_cmbStation.InsertString(i,gpcdef.GetStationName(m_pDoc->m_nCurrentStationList[i]));
	}
   	m_cmbStation.InsertString(m_pDoc->m_nStationNum, "<所有>");
	
    m_cmbStation.SetCurSel(0);
    m_nCurStation = m_pDoc->m_nCurrentStationList[0];	

    ShowReportTimeRecord();
	InitDlgPosition(this);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgStpcReportTime::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
//	m_pWintgForm->m_dlgFocusDialog = NULL;
//	m_pWintgForm->m_pStpcReportTimeDlg = NULL;
	m_vectorStpcReportTime.clear();
 	delete this;
	//CDialog::PostNcDestroy();
}

bool CDlgStpcReportTime::ShowReportTimeRecord() 
{
	STPC_REPORT_RECORD rec;
	//test
    m_list.DeleteAllItems();

	REPORT_TIME_VECTOR::iterator theIterator;
//    REPORT_TIME_VECTOR::size_type total_count;
 
	LV_ITEM lvi;
	lvi.mask = LVIF_TEXT ;
	lvi.iSubItem = 0;
	CString strText;
	int row;
	row = 0;
 	// fill rows/cols with text
	int index = 0;
	m_vectorStpcReportTime.clear();
	for (theIterator = m_pDoc->m_vectorReportTime.begin();
		 theIterator != m_pDoc->m_vectorReportTime.end();
		 theIterator++)
	{
//	    if(m_nCurStation != -1 && m_nCurStation != theIterator->station)
		rec = m_pDoc->m_vectorReportTime.at(index);
	    if(m_nCurStation != -1 && m_nCurrentStationSel != theIterator->station)
            continue;
//        m_vectorStpcReportTime.insert(m_vectorStpcReportTime.begin(),rec);
        m_vectorStpcReportTime.push_back(rec);

 		lvi.iItem = row;
        strText.Format(_T("%d"),row);
 		lvi.pszText = (char *)(LPCTSTR)strText;
		m_list.InsertItem(&lvi);
   	    strText.Format(_T("%s"),gpcdef.GetStationName(theIterator->station));
     	m_list.SetItemText(row,1,strText);
 	 	strText.Format(_T("%s"),theIterator->train_id);
      	m_list.SetItemText(row,2,strText);
    	strText.Format(_T("%d"),theIterator->side);
      	m_list.SetItemText(row,3,strText);
		if(theIterator->a_d_flag == 'A' ||
			theIterator->a_d_flag == 'a')
			strText = "到";
		else
			strText = "发";
      	m_list.SetItemText(row,4,strText);
	 	strText.Format(_T("%s"),OsConvertTimetoString(theIterator->usTime));
     	m_list.SetItemText(row,5,strText);
	    row ++;
		index ++;
	}
	m_nCurrentRecSel = 0;
	m_list.SetCurSel(m_nCurrentRecSel);
	Invalidate(FALSE);

    return TRUE;
}

void CDlgStpcReportTime::OnOk() 
{
	STPC_REPORT_RECORD rec;
	UINT i, uSelectedCount = m_list.GetSelectedCount();
	int  nItem = -1;
	// Update all of the selected items.
	if (uSelectedCount > 0)
	{
/*		REPORT_TIME_VECTOR::iterator theIterator;
		REPORT_TIME_VECTOR::size_type total_count;
 		for (theIterator = m_pDoc->m_vectorReportTime.begin();
			 theIterator != m_pDoc->m_vectorReportTime.end();
			 theIterator++)
		{
		}
*/

		char buff[128], *ptr;
 		ptr = buff;
		ushort count;
 
		ptr += 2;
 		*(ushort *)ptr =  (STPC_REPORT_MANUAL_PROC);
		ptr += 2;
		*(ushort*)ptr = NettoHostUShort(gpcdef.GetUserGpcId());
		ptr += 2;
   		*ptr = (byte)m_pDoc->m_nCurrentSectionNo;
		ptr += 1;
		*(ushort*)ptr = NettoHostUShort(uSelectedCount);
		ptr += 2;
      	for (i=0;i < uSelectedCount;i++)
		{
			*ptr = (byte)1;//operator type 0:delete ,1:recept
			ptr ++;
		    nItem = m_list.GetNextItem(nItem, LVNI_SELECTED);
		    ASSERT(nItem != -1);
		    rec = m_vectorStpcReportTime.at(nItem);
            strncpy(ptr,(char*)rec.train_id,10);
			ptr += 10;
            *ptr = rec.flag;
			ptr ++;
            *(USHORT*)ptr= NettoHostUShort(rec.station);
			ptr += 2;
            *ptr = rec.a_d_flag;
			ptr ++;
            *ptr = rec.side;
			ptr ++;
            *(time_t*)ptr = NettoHostUInteger(long_time(rec.usDate,rec.usTime ));
			ptr += 4;
		}

 		count = ptr - buff;
		*(ushort *)(buff  ) =  (count  );
		SendTGCommMessage(count,buff);
	}
    m_list.SetCurSel(m_nCurrentRecSel);

//	CDialog::DestroyWindow();
}

void CDlgStpcReportTime::OnCancel() 
{
	// TODO: Add extra cleanup here
	CDialog::DestroyWindow();
//	CDialog::OnCancel();
}

BOOL CDlgStpcReportTime::PreTranslateMessage(MSG* pMsg) 
{
 	if(pMsg->message == WM_LBUTTONDOWN ||
		pMsg->message == WM_RBUTTONDOWN ){
		do {
			if(pMsg->hwnd != m_list.GetSafeHwnd( ))
				break;
			CPoint point;
			point.x = (pMsg->lParam) & 0x0000ffff;
			point.y = ((pMsg->lParam) & 0xffff0000) >> 16;
			UINT uFlags = 0;
			int nHitItem = m_list.HitTest(point, &uFlags);

			// we need additional checking in owner-draw mode
			// because we only get LVHT_ONITEM
			BOOL bHit = FALSE;
			if ((uFlags & LVHT_ONITEM) && (m_list.GetStyle() & LVS_OWNERDRAWFIXED))
			{
				CRect rect;
				m_list.GetItemRect(nHitItem, rect, LVIR_BOUNDS);

				// check if hit was on a state icon
// 				if(rect.PtInRect(point))
// 					bHit = TRUE;
 
 				if (point.x >= rect.left && point.x <= rect.right 
 					&& point.y >= rect.top && point.y <= rect.bottom)
     				bHit = TRUE;
             } 

			if (!bHit)
				break;
//			if(nHitItem == m_nCurrentRecSel)
//				break;
/*			if(pMsg->message == WM_LBUTTONDOWN && !m_bFirstClick){
           	    m_bFirstClick = TRUE;
				nPrevTime = GetTickCount( );
			}else{
				int delta = GetTickCount( ) - nPrevTime;
				if(delta < 500){// && m_bFirstClick){
            		
					OperateRecord(TRUE);
 					m_bFirstClick = FALSE;
				}else{
		    		nPrevTime = GetTickCount( );
 					m_bFirstClick = TRUE;
				}
			}
*/
			m_nCurrentRecSel = nHitItem;
 
			if(pMsg->message == WM_RBUTTONDOWN  && gpcdef.GetOperatorRight(WINTG_EDIT))
            	OperateRecord(TRUE);
		}while(0);
	}
 	
	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgStpcReportTime::OnDblclkReportRecord(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
  
	*pResult = 0;
}

void CDlgStpcReportTime::OnItemdblclickReportRecord(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

LRESULT CDlgStpcReportTime::OnStpcReportTime(WPARAM wParam,LPARAM lParam)
{
    ShowReportTimeRecord();
	return TRUE;
}

void CDlgStpcReportTime::OperateRecord(int flag)
{
	STPC_REPORT_RECORD rec;
	UINT i, uSelectedCount = m_list.GetSelectedCount();
	int  nItem = -1;
	// Update all of the selected items.
	if (uSelectedCount > 0)
	{
		char buff[2048], *ptr;
 		ptr = buff;
		ushort count;
	 
		ptr += 2;
 		*(ushort *)ptr =  (STPC_REPORT_MANUAL_PROC);
		ptr += 2;
		*(ushort*)ptr = NettoHostUShort(gpcdef.GetUserGpcId());
		ptr += 2;
   		*ptr = m_pDoc->m_nCurrentSectionNo;
		ptr += 1;
		*(ushort*)ptr = NettoHostUShort(uSelectedCount);
		ptr += 2;
      	for (i=0;i < uSelectedCount;i++)
		{
			*ptr = flag;//operator type 0:delete ,1:recept
			ptr ++;
		    nItem = m_list.GetNextItem(nItem, LVNI_SELECTED);
		    ASSERT(nItem != -1);
		    rec = m_vectorStpcReportTime.at(nItem);
            strncpy(ptr,(char*)rec.train_id,10);
			ptr += 10;
            *ptr = rec.flag;
			ptr ++;
            *(USHORT*)ptr= NettoHostUShort(rec.station);
			ptr += 2;
            *ptr = rec.a_d_flag;
			ptr ++;
            *ptr = rec.side;
			ptr ++;
            *(time_t*)ptr = NettoHostUInteger(long_time(rec.usDate,rec.usTime));
			ptr += 4;
			if(i>100) break;
		}

 		count = ptr - buff;
		*(ushort *)(buff  ) =  (count  );
		SendTGCommMessage(count,buff);
	}
    m_list.SetCurSel(m_nCurrentRecSel);

}

void CDlgStpcReportTime::OnSelchangeCmbStation() 
{
	// TODO: Add your control notification handler code here
	m_nCurrentStationSel = m_cmbStation.GetCurSel();
	if(m_nCurrentStationSel == m_pDoc->m_nStationNum)
		m_nCurStation = -1;
	else
        m_nCurStation = m_pDoc->m_nCurrentStationList[m_nCurrentStationSel];	
    ShowReportTimeRecord();
    m_list.SetCurSel(m_nCurrentRecSel);

}

void CDlgStpcReportTime::OnDelete() 
{
	STPC_REPORT_RECORD rec;
	UINT i, uSelectedCount = m_list.GetSelectedCount();
	int  nItem = -1;
	// Update all of the selected items.
	if (uSelectedCount > 0)
	{
 

		char buff[2048], *ptr;
 		ptr = buff;
		ushort count;
	 
		ptr += 2;
 		*(ushort *)ptr =  (STPC_REPORT_MANUAL_PROC);
		ptr += 2;
		*(ushort*)ptr = NettoHostUShort(gpcdef.GetUserGpcId());
		ptr += 2;
   		*ptr = m_pDoc->m_nCurrentSectionNo;
		ptr += 1;
		*(ushort*)ptr = NettoHostUShort(uSelectedCount);
		ptr += 2;

      	for (i=0;i < uSelectedCount;i++)
		{
			*ptr = 0;//operator type 0:delete ,1:recept
			ptr ++;
		    nItem = m_list.GetNextItem(nItem, LVNI_SELECTED);
		    ASSERT(nItem != -1);
		    rec = m_vectorStpcReportTime.at(nItem);
            strncpy(ptr,(char*)rec.train_id,10);
			ptr += 10;
            *ptr = rec.flag;
			ptr ++;
            *(USHORT*)ptr= NettoHostUShort(rec.station);
			ptr += 2;
            *ptr = rec.a_d_flag;
			ptr ++;
            *ptr = rec.side;
			ptr ++;
            *(time_t*)ptr = long_time(rec.usDate,rec.usTime );
			ptr += 4;
			if(i>100) break;
		}

 		count = ptr - buff;
		*(ushort *)(buff  ) =  (count  );
		SendTGCommMessage(count,buff);
	}
    m_list.SetCurSel(m_nCurrentRecSel);
}

void CDlgStpcReportTime::OnItemchangedReportRecord(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CDlgStpcReportTime::OnRdblclkReportRecord(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}
