// AbsTimeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AbsTimeDlg.h"
#include "function.h"
#include "tgmsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern WriteMessage(PROCESSID usProcessId,ushort usQueueHandle,
		ushort count,char *buffer);

/////////////////////////////////////////////////////////////////////////////
// CAbsTimeDlg dialog


CAbsTimeDlg::CAbsTimeDlg(/*CWintgDoc *pDoc,*/CWnd* pParent /*=NULL*/)
	: CDialog(CAbsTimeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAbsTimeDlg)
	m_edit1 = _T("");
	//}}AFX_DATA_INIT
//\	m_pDoc = pDoc;
	m_nCurrentSel = -1;
//	m_pListEdit = 0;

}
CAbsTimeDlg::~CAbsTimeDlg()
{
 //	delete m_pListEdit;
}


void CAbsTimeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAbsTimeDlg)
	DDX_Text(pDX, IDC_EDIT1, m_edit1);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAbsTimeDlg, CDialog)
	//{{AFX_MSG_MAP(CAbsTimeDlg)
	ON_MESSAGE(UM_ABS_TIME_MSG, OnAbsTimeMsg)
 	ON_NOTIFY(NM_CLICK, IDC_LIST, OnClickList)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAbsTimeDlg message handlers
/*
void CAbsTimeDlg::OnBeginlabeleditList(NMHDR* pNMHDR, LRESULT* pResult) 
{
 	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
 
	*pResult = 0;	
}

void CAbsTimeDlg::OnEndlabeleditList(NMHDR* pNMHDR, LRESULT* pResult) 
{
 	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	int item = pDispInfo->item.iItem;
	int subitem = pDispInfo->item.iSubItem;
	// This is coming from the grid list control notification.
	if( m_pListEdit )
		{
		CString str;
		if( pDispInfo->item.pszText )
  			m_GridListCtrl.SetItemText( item, subitem, pDispInfo->item.pszText );
		delete m_pListEdit;
		m_pListEdit = 0;
		}
	*pResult = 0;	
}*/
#define ITEMNUM  9
BOOL CAbsTimeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  
    ushort usCommQueueHandle;
	while(OsGetQueueHandle(PMTG.ThreadId,&usCommQueueHandle,
		"QUEUES.GPC.COMM"))
	{
		OsSleep(1000L,PMTG.ThreadId);
		sysprint(0,PMTG.ThreadId,"[PMTG]:get comm queue handle failed");
	}

	char buff[128], *ptr;
 	ptr = buff;
	*ptr++ = PMTG_MESSAGE;
	ptr += 2;

 	*(ushort *)ptr = NettoHostUShort(LOAD_ABS_RUN_TIME);
	ptr += 2;

	////////////////////////////////////////////////
	//add by cf after 7-18

	///modiby cf after 9-9
	/* modi by cf after 9-9
	int sectionNum;
	sectionNum = gpcdef.GetSectionNum();
	int *sectionlist;
	sectionlist = new int[sectionNum];
	gpcdef.GetSectionList(sectionNum,sectionlist);

	*(byte*)ptr = (byte)(sectionlist[0]);
	ptr += 1;
	delete []sectionlist;
	*/
    if(m_pDoc)
	{
		if(schd_Type == HISTORY_SCHEDULE)
		{
			*(byte*) = (byte)(m_pDoc->m_nCurrentSectionHistNo);
			ptr+=1;
		}
		else
		{
			if(schd_Type == BASIC_SCHEDULE)
			{
				*(byte*) = (byte)(m_pDoc->m_nCurrentSectionBasicNo);
				ptr+=1;
			}
			else
			{
				*(byte*) = (byte)(m_pDoc->m_nCurrentSectionNo);
				ptr+=1;
			}
		}
	};//add by cf after 9-9

	*(ushort *)ptr = NettoHostUShort((ushort)gpcdef.GetDispatchNoFromDefaultUser());
	ptr += 2;

	//////////end of cf ///////////////////////////////
	//////////end of cf ///////////////////////////////

 	ushort count = ptr -buff;
	*(ushort *)(buff + 1) = NettoHostUShort(count - 1);
	WriteMessage(PMTG.ThreadId,	usCommQueueHandle,count,buff);	

 	TCHAR rgtsz[ITEMNUM][20] = {_T("区间开始站"), _T("区间结束站"), _T("出口"), _T("进口"),
		_T("方向"),_T("列车类型"),_T("停车附加时分"),_T("起车附加时分"),_T("区间运行时分")};

    int cxcolumn[ITEMNUM] = {40,40,20,20,30,60,50,50,50};
	LV_ITEM			lvitem;
	LV_COLUMN		lvcolumn;
	CRect rect;
 /*
	// Insert Image Lists
	m_pImageList = new CImageList();
	m_pImageList->Create( IDB_BITMAP1, 16, 1, RGB(255,255,255) );
 	m_GridListCtrl.SetImageList( m_pImageList, LVSIL_SMALL );
*/
	// Insert columns using the order field
	int i;
	for(i = 0; i < ITEMNUM; i++)  
		{
		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH | LVCF_ORDER;
		lvcolumn.fmt = LVCFMT_LEFT;
		lvcolumn.pszText = rgtsz[i];
		lvcolumn.iSubItem = i;
		lvcolumn.iOrder = i;
		lvcolumn.cx = cxcolumn[i];//rect.Width() / ITEMNUM;  
		m_GridListCtrl.InsertColumn(i, &lvcolumn);  
        m_GridListCtrl.SetColumnWidth(i, cxcolumn[i]);
		}

/*
	int iItem, iSubItem, iActualItem;
	TCHAR buffer[30];
	
	for (iItem = 0; iItem < 50; iItem++)  // will now insert the items and subitems into the list view.
		for (iSubItem = 0; iSubItem < ITEMNUM; iSubItem++)
		{
			lvitem.mask = LVIF_TEXT | (iSubItem == 0? LVIF_IMAGE : 0);
			lvitem.iItem = (iSubItem == 0)? iItem : iActualItem;
			lvitem.iSubItem = iSubItem;
			lvitem.iImage = (iItem%2)?0:2;
			sprintf( buffer, "( %d, %d )", iItem+1, iSubItem+1);
			lvitem.pszText = buffer;

			if (iSubItem == 0)
				iActualItem = m_ListCtrl.InsertItem(&lvitem);
			else
				m_ListCtrl.SetItem(&lvitem);
		}
*/
	return TRUE;  // return TRUE  unless you set the focus to a control	
 
}
void CAbsTimeDlg::OnAbsTimeMsg(WPARAM wParam, LPARAM lParam)
{
	return;

  	char* ptr = (char *)lParam;
 	static int nReceiveFlag = 0;
 	int nLinkCount,theStatus = 1;
    ushort station_no1,station_no2,exit,entry,direction,train_type,stop_time,
          start_time,run_time; 
	ushort usSubMessageType = *ptr++;//sub message code

	switch(usSubMessageType)
	{
	case MESSAGE_BEGIN:
		if(nReceiveFlag)
		{
			sysprint(0,TGMSG.ThreadId,"[TGMSG]:invalid receive status");
		}
		nReceiveFlag = 1;
//        nStationNo= NettoHostUShort(*(ushort *)ptr);
//		ptr +=2;
		numUnit = 0;
 		theStatus = 0;
 		break;
	case MESSAGE_DATA:
		ushort usDataSize;
//        nSideNo= NettoHostUShort(*ptr);
//		ptr +=1;
        nLinkCount= NettoHostUShort(*(ushort *)ptr);
		ptr +=2;
		while(nLinkCount--){
            unit[numUnit].station_no1 = NettoHostUShort(*(ushort *)ptr);
            ptr += 2;
            unit[numUnit].station_no2 = NettoHostUShort(*(ushort *)ptr);
            ptr += 2;
			unit[numUnit].exit = *ptr;
			ptr ++;
			unit[numUnit].entry = *ptr;
			ptr ++;
			unit[numUnit].direction = *ptr;
			ptr ++;
			unit[numUnit].train_type = *ptr;
			ptr ++;
			unit[numUnit].stop_time = NettoHostUShort(*(ushort *)ptr);
			ptr +=2;
			unit[numUnit].start_time = NettoHostUShort(*(ushort *)ptr);
			ptr +=2;
			unit[numUnit].run_time = NettoHostUShort(*(ushort *)ptr);
			ptr +=2;
            
			numUnit ++;
		}
 
		if(nReceiveFlag == 0)
		{
			sysprint(0,TGMSG.ThreadId,"[TGMSG]:invalid receive status");
			break;
		}
		theStatus = 0;
		break;
	case MESSAGE_END:
		if(nReceiveFlag == 0)
		{
			sysprint(0,TGMSG.ThreadId,"[TGMSG]:invalid receive status");
			break;
		}
 		nReceiveFlag = 0;
		break;
	default:
		sysprint(0,TGMSG.ThreadId,"[TGMSG]:invalid sub message");
		break;
	}
	if(nReceiveFlag == 0){


        m_GridListCtrl.DeleteAllItems ();
		int iItem, iSubItem, iActualItem;
		TCHAR buffer[10];
     	//CString buffer;
		LV_ITEM			lvitem;
		
		for (iItem = 0; iItem < numUnit; iItem++)  // will now insert the items and subitems into the list view.
			for (iSubItem = 0; iSubItem < ITEMNUM; iSubItem++)
			{
				lvitem.mask = LVIF_TEXT;// | (iSubItem == 0? LVIF_IMAGE : 0);
				lvitem.iItem = (iSubItem == 0)? iItem : iActualItem;
				lvitem.iSubItem = iSubItem;
				//lvitem.iImage = (iItem%2)?0:2;
				switch(iSubItem){
				case 0:
//				    buffer.Format ( "%d", unit[iItem].station_no1 );
				    sprintf(buffer,"%s", gpcdef.GetStationName(unit[iItem].station_no1 ));
					break;
				case 1:
				    sprintf(buffer,"%s", gpcdef.GetStationName(unit[iItem].station_no2 ));
					break;
				case 2:
				    sprintf(buffer,"%d", unit[iItem].exit );
					break;
				case 3:
				    sprintf(buffer,"%d", unit[iItem].entry );
					break;
				case 4:
				    sprintf(buffer,"%s",(unit[iItem].direction ?"上行":"下行") );
					break;
				case 5:
				    sprintf(buffer,"%s", GetTrainTypeStr(unit[iItem].train_type));//货车 客车快车
					break;
				case 6:
				    sprintf(buffer,"%d:%d", unit[iItem].start_time/60,unit[iItem].start_time%60 );
					break;
				case 7:
				    sprintf(buffer,"%d:%d", unit[iItem].stop_time/60,unit[iItem].stop_time%60 );
					break;
				case 8:
				    sprintf(buffer,"%d:%d", unit[iItem].run_time/60, unit[iItem].run_time%60 );
					break;
                default:
					break;
				}
 				
				lvitem.pszText = buffer;

				if (iSubItem == 0)
					iActualItem = m_GridListCtrl.InsertItem(&lvitem);
				else
  			    	m_GridListCtrl.SetItem(&lvitem);
			}	  
		}
}

CString CAbsTimeDlg::GetTrainTypeStr(int nType)
{
// 客车快车
    CString str;
    switch(nType){
	case  0:
	    str = "货车";
		break;
	case 1:
		str = "客车";
		break;
	case 2:
		str = "快车";
		break;
	default:
		str = "未知列车"; 
		break;
	}
    return str;   
}
/*
BOOL CAbsTimeDlg::PreTranslateMessage(MSG* pMsg) 
{

    if(pMsg->message == WM_KEYDOWN )  {
		do{
			if(pMsg->hwnd != m_ListCtrl.GetSafeHwnd( ))// m_list.GetSafeHwnd()
				break;
 			if(pMsg->wParam == VK_DOWN){
	//			if(m_nCurrentSel < numUnit-1) m_nCurrentSel++;
       //            SetListText();
			}
 			if(pMsg->wParam == VK_UP){
	//			if(m_nCurrentSel >= 1) m_nCurrentSel--;
      //             SetListText();
			}
		}while(0);
	}

    if(pMsg->message == WM_LBUTTONDOWN )  {
 	do {
			if(pMsg->hwnd != m_ListCtrl.GetSafeHwnd( ))// m_list.GetSafeHwnd()
				break;
			CPoint point;
			point.x = (pMsg->lParam) & 0x0000ffff;
			point.y = ((pMsg->lParam) & 0xffff0000) >> 16;
			UINT uFlags = 0;
			int nHitItem = m_ListCtrl.HitTest(point, &uFlags);

			// we need additional checking in owner-draw mode
			// because we only get LVHT_ONITEM
			BOOL bHit = FALSE;
			if ((uFlags & LVHT_ONITEM) && (m_ListCtrl.GetStyle() & LVS_OWNERDRAWFIXED))
			{
				CRect rect;
				m_ListCtrl.GetItemRect(nHitItem, rect, LVIR_BOUNDS);

				// check if hit was on a state icon
				if (point.x >= rect.left && point.x <= rect.right 
					&& point.y >= rect.top && point.y <= rect.bottom)
					bHit = TRUE;
			} 

			if (!bHit)
				break;
			if(nHitItem == m_nCurrentSel)
				break;
 			m_nCurrentSel = nHitItem;
		}while(0);	
        SetListText();
 	 }

	return CDialog::PreTranslateMessage(pMsg);
}
*/
void CAbsTimeDlg::OnClickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
    POSITION pos = m_GridListCtrl.GetFirstSelectedItemPosition();
    if (pos == NULL)
       TRACE0("No items were selected!\n");
    else 
	{
        while (pos)
		{
          m_nCurrentSel = m_GridListCtrl.GetNextSelectedItem(pos);
          TRACE1("Item %d was selected!\n", m_nCurrentSel);
           // you could do your own processing on nItem here
		}
	}

  	for(int i= 0;i< ITEMNUM;i++){
	   CString str = m_GridListCtrl.GetItemText(m_nCurrentSel,i);
	   SetDlgItemText(IDC_EDIT1 + i,str);
	}

	*pResult = 0;
}

void CAbsTimeDlg::SetListText()
{
  //  SetDlgItemText(IDC_EDIT1,"lyc"); 
    m_edit1 = "lyc";
    UpdateData(FALSE);
	/*
 	for(int i= 0;i< ITEMNUM;i++){
	   CString str = m_ListCtrl.GetItemText(m_nCurrentSel,i);
	   SetDlgItemText(IDC_EDIT1 + i,str);
	}*/
}

void CAbsTimeDlg::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
 	*pResult = 0;
}
