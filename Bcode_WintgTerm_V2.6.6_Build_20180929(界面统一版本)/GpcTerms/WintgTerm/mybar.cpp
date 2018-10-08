// mybar.cpp : implementation file
//

#include "stdafx.h"
#include "mybar.h"
#include "resource.h"
#include "tgmsg.h"
#include "alarm.h"
#include "sysshare.h"
#include "messages.h"
#include "widgets.h"
#include "function.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int nGlobalConnectStatus;
Pstruct _tagConflictInformNode *selectconflict = NULL;
static Pstruct _tagConflictInformNode *sConflictInformBase = NULL;
int nMaxConflictMessageLineNo = 1;

static char  *OsShowConflict(PROCESSID ProcessId,CONFLICTINFORM *ciCurr);

#define INVALID_MSGID_CONJOIN		2

/////////////////////////////////////////////////////////////////////////////
// CMyBar
extern int WriteMessage(PROCESSID usProcessId,ushort usQueueHandle,
		ushort count,char *buffer);

CMyBar::CMyBar()
{
	m_nListItemNum = 0;
    m_nCurSel = -1;
	curr = NULL;


}

CMyBar::~CMyBar()
{
}


BEGIN_MESSAGE_MAP(CMyBar, baseCMyBar)
    //{{AFX_MSG_MAP(CMyBar)
    ON_WM_CREATE()
    ON_WM_SIZE()
	ON_LBN_DBLCLK(IDC_COFLICT_BOX, OnDblclkList1)
	ON_LBN_SELCHANGE(IDC_LIST1, OnSelchangeList1)
// 	ON_MESSAGE(SYSTEM_ALARM_LOG_APPEND, OnAlarmMessage)
	ON_MESSAGE(UM_TGMSG_CONFLICT_PURGE, OnConflictPurge)
 	ON_MESSAGE(UM_TGMSG_CONFLICT_INSERT, OnConflictInsert)
	ON_MESSAGE(UM_TGMSG_CONFLICT_DELETE, OnConflictDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMyBar message handlers

int CMyBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (baseCMyBar::OnCreate(lpCreateStruct) == -1)
        return -1;
/*
    if (!m_wndChild.Create(_T("liuyichun"),
		WS_CHILD|WS_VISIBLE,
		CRect(0,0,0,0), this, ID_EDIT_PASTE))
        return -1;
*/
	// create a list box to fill the client area with. Use CreateEx to add the
	// WS_EX_CLIENTEDGE style. 
	CRect rect;
//	GetClientRect(&rect);
	
	rect = CRect(0,0,100,100);
	// Register a class for the list box without CS_VREDRAW, CS_HREDRAW
	WNDCLASS wc;
	VERIFY(GetClassInfo(AfxGetInstanceHandle(), _T("LISTBOX"), &wc)); 	
	wc.style &= ~(CS_VREDRAW | CS_HREDRAW);
	wc.lpszClassName = "_MF_LISTBOX";
	VERIFY(AfxRegisterClass(&wc));

  
/*
	if (!m_ListBox.CreateEx(WS_EX_CLIENTEDGE,
                 			_T("_MF_LISTBOX"),
                 			NULL,
     						WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL
	 								| LBS_HASSTRINGS | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT, 
	             			rect.left,
                 			rect.top,
      						rect.Width(),
							rect.Height(),
                 			GetSafeHwnd(),
                 			0,
                 			NULL))//IDC_COFLICT_BOX*/
    if(!strcmp(strTitle,"人工报点窗"))
	{
 
  		if (!m_ComboStation.Create( WS_CHILD | WS_VISIBLE /*| WS_HSCROLL | WS_VSCROLL
	 							| LBS_HASSTRINGS | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT */, 
                            CRect(0,0,100,20),
							this,
                  			IDC_COMBO_STATION))//IDC_COFLICT_BOX
		{
			TRACE("Failed to create list box\n");
			return -1;
		} 
  		if (!m_ComboTrain.Create( WS_CHILD | WS_VISIBLE /*| WS_HSCROLL | WS_VSCROLL
	 							| LBS_HASSTRINGS | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT */, 
                            CRect(0,40,100,60),
							this,
                  			IDC_COMBO_STATION))//IDC_COFLICT_BOX
		{
			TRACE("Failed to create list box\n");
			return -1;
		} 
//		m_ComboStation.SetHorizontalExtent(200);
/*
		if (!m_font.CreateStockObject(DEFAULT_GUI_FONT))
			if (!m_font.CreatePointFont(80, "MS Sans Serif"))
				return -1;

		m_ComboStation.SetFont(&m_font);

		for (int i = 0; i < 10; i++)
		{
			char msg[100];
			sprintf(msg, "Line %d", i);
			m_ComboStation.AddString("CHINA");
		}
*/
	}else{
		if (!m_ListBox.Create( WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL
	 							| LBS_HASSTRINGS | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT , 
                            rect,
							this,
                  			IDC_COFLICT_BOX))//IDC_COFLICT_BOX
		{
			TRACE("Failed to create list box\n");
			return -1;
		}

		m_ListBox.SetHorizontalExtent(200);

		if (!m_font.CreateStockObject(DEFAULT_GUI_FONT))
			if (!m_font.CreatePointFont(80, "MS Sans Serif"))
				return -1;

		m_ListBox.SetFont(&m_font);

	//  m_wndChild.SetFont(&m_font);
	/*
		m_hPrintSemaphore = OpenSemaphore(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE,
			TRUE,
			"system_print_sem");
		if(m_hPrintSemaphore == NULL)
			TRACE1("open semaphore failed %d",GetLastError( ));
		SetPrintParameter(GetSafeHwnd( ),1);
	*/	}
/*	
	// add some strings to the list box - makes it a bit more interesting
	for (int i = 0; i < 10; i++)
	{
		char msg[100];
		sprintf(msg, "Line %d", i);
		m_ListBox.AddString(msg);
	}*/
    // older versions of Windows* (NT 3.51 for instance)
    // fail with DEFAULT_GUI_FONT

    return 0;
}

void CMyBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
	baseCMyBar::OnUpdateCmdUI(pTarget, bDisableIfNoHndler);

	UpdateDialogControls(pTarget, bDisableIfNoHndler);
}

void CMyBar::OnSize(UINT nType, int cx, int cy) 
{
    baseCMyBar::OnSize(nType, cx, cy);
    if(!strcmp(strTitle,"人工报点窗") ||!strcmp(strTitle,"") )
       return;

    // TODO: Add your message handler code here
    CRect rc;
    GetClientRect(rc);
	if(m_ListBox.GetSafeHwnd() != NULL)
        m_ListBox.MoveWindow(rc);
}

void CMyBar::OnDblclkList1() 
{
    if(!strcmp(strTitle,"人工报点窗"))
       return;

	int iSel = m_ListBox.GetCurSel( );
	char  text[512];
// 	Pstruct _tagConflictInformNode *curr;
	int theStatus = 1,nFoundFlag = 0;
 	char abBuffer[64],*ptr;
	ushort count;
 
	do { 
		if(iSel == LB_ERR)
			break;
        //added by lyc 2001/3/4
        if(curr != NULL && m_nCurSel != -1  && m_nCurSel != iSel ){
			ptr = abBuffer;
			ptr += 2;// message length
 
			// send unselect message
			*(ushort *)ptr = NettoHostUShort(UNSELECT_CONFLICT_TRAIN);
			ptr += 2;
 			curr->flag = 0;
 
			strcpy(ptr,curr->data.ele1.trainid);
			ptr += 12;
			strcpy(ptr,curr->data.ele2.trainid);
			ptr += 12;
			count = (ushort)(ptr - abBuffer);
			*(ushort *)abBuffer = NettoHostUShort(count);
			LocalTGRequestMessageProcess((unsigned char *)abBuffer,count);
		}
       
		m_nCurSel = iSel;

		m_ListBox.GetText(iSel,text);
		
		if(sConflictInformBase == NULL)
			break;
		
		curr = sConflictInformBase->next;
		while(curr != sConflictInformBase)
		{
			if(curr->usLineNo == iSel + 1)
			{
				nFoundFlag = 1;
				break;
			} else 
				curr = curr->next;
		};
		if(nFoundFlag == 0)
		{
			if(curr->usLineNo == iSel + 1)
				nFoundFlag = 1;
		}
		if(nFoundFlag == 0)
		{
			sysprint(0,PMTG.ThreadId,"[PMTG]: found conflict ele failed");
			break;
		}
 
		if(selectconflict != NULL && selectconflict != curr && curr->flag == 0)
		{
			sysprint(0,PMTG.ThreadId,"[PMTG]: delete select conflict train");			
 			OsDeselectConflict(selectconflict);
		}

		ptr = abBuffer;
		ptr += 2;// message length
        
		if(curr->flag == 0)
		{// send select message
			*(ushort *)ptr = NettoHostUShort(CONFLICT_RECORD_SELECT);
			ptr += 2;
			selectconflict = curr;
			curr->flag = 1;
		} else { // send unselect message
			*(ushort *)ptr = NettoHostUShort(UNSELECT_CONFLICT_TRAIN);
			ptr += 2;
			selectconflict = NULL;
			curr->flag = 0;
		}

		strcpy(ptr,curr->data.ele1.trainid);
		ptr += 12;
		strcpy(ptr,curr->data.ele2.trainid);
		ptr += 12;
		count = (ushort)(ptr - abBuffer);
		*(ushort *)abBuffer = NettoHostUShort(count);
		LocalTGRequestMessageProcess((unsigned char *)abBuffer,count);
		theStatus = 0;
	}while(0);
}

void CMyBar::OnSelchangeList1() 
{
}


int CMyBar::OsDeselectConflict(Pstruct _tagConflictInformNode *curr)
{
	int theStatus = 1;
 	
	do {
		if(selectconflict == NULL)
			break;
		if(curr == NULL || curr != selectconflict)
			break;
		char abBuffer[64],*ptr;
		ushort count;

		ptr = abBuffer;
		ptr += 2;// message length
		
		 // send unselect message
		*(ushort *)ptr = NettoHostUShort(UNSELECT_CONFLICT_TRAIN);
		ptr += 2;
		
		strcpy(ptr,selectconflict->data.ele1.trainid);
		ptr += 12;
		strcpy(ptr,selectconflict->data.ele2.trainid);
		ptr += 12;
		count = (ushort)(ptr - abBuffer);
		*(ushort *)abBuffer = NettoHostUShort(count);
		LocalTGRequestMessageProcess((unsigned char *)abBuffer,count);

		selectconflict = NULL;
		theStatus = 0;
	}while(0);
	if(theStatus)
		return -1;
	return 0;
}

LRESULT CMyBar::OnConflictDelete(WPARAM wParam, LPARAM lParam)
{
	int nDisappearConflictID = (int)wParam;
	Pstruct _tagConflictInformNode *curr,*prev;
	int nFoundFlag = 0;
	int theStatus = 1;

	do {
		if(sConflictInformBase == NULL)
		{
			sysprint(0,PMTG.ThreadId,"[PMTG]: error conflict disappear");
			break;
		}
		curr = sConflictInformBase->next;

		while(curr != sConflictInformBase){
			if(curr->data.ele1.usConflictid == nDisappearConflictID)
			{// this record need be deleted;
				prev = curr;
				curr = curr->next;
				prev->prev->next = curr;
				prev->next->prev = prev->prev;
				nFoundFlag = prev->usLineNo;

				if(prev->flag == 1)
				{
					sysprint(0,PMTG.ThreadId,"[PMTG]: delete select conflict train");
					OsDeselectConflict(prev);
				}
				free(prev);
				break;
			} else 
				curr = curr->next;
		}
		if(nFoundFlag == 0)
		{
			curr = sConflictInformBase;
			if(curr->data.ele1.usConflictid == nDisappearConflictID)
			{// DETERMINE IF FIRST RECORD NEED BE deleteD;
				prev = curr;
				nFoundFlag = prev->usLineNo;
				if(curr == curr->next)
				{
					if(prev->flag == 1)
					{
						sysprint(0,PMTG.ThreadId,"[PMTG]: delete select conflict train");
						OsDeselectConflict(curr);
					}
					free(curr);
					sConflictInformBase = NULL;
				}else {
					curr = curr->next;
					prev->prev->next = curr;
					prev->next->prev = prev->prev;
					sConflictInformBase = curr;
					if(prev->flag == 1)
					{
						sysprint(0,PMTG.ThreadId,"[PMTG]: delete select conflict train");
						OsDeselectConflict(prev);
					}
					free(prev);
				}
			}
		} 
		if(nFoundFlag)
		{
 			if(nFoundFlag >= nMaxConflictMessageLineNo)
 				sysprint(0,PMTG.ThreadId,"[PMTG]: error line no");
			if(curr = sConflictInformBase)
			{
				if(curr->usLineNo > nFoundFlag)
					curr->usLineNo -= 1;
				else if(curr->usLineNo == nFoundFlag)
					sysprint(0,PMTG.ThreadId,"[PMTG]: error line no");
				curr = curr->next;
				while(curr != sConflictInformBase)
				{
					if(curr->usLineNo > nFoundFlag)
						curr->usLineNo -= 1;
					else if(curr->usLineNo == nFoundFlag)
						sysprint(0,PMTG.ThreadId,"[PMTG]: error line no");
					curr = curr->next;
				}
			}
			m_ListBox.DeleteString(nFoundFlag - 1);
			nMaxConflictMessageLineNo -= 1;
			if(sConflictInformBase == NULL && 
				nMaxConflictMessageLineNo != 1)
				sysprint(0,PMTG.ThreadId,"[PMTG]: error line no");
		} else 
			sysprint(0,PMTG.ThreadId,"[PMTG]: havn't found conflict ele");
		theStatus = 0;
	}while(0);
	if(theStatus)
		MessageBeep(MB_ICONASTERISK);

	return TRUE;
}

LRESULT CMyBar::OnConflictPurge(WPARAM wParam, LPARAM lParam)
{
	Pstruct _tagConflictInformNode *curr,*prev;

	if(sConflictInformBase)
	{
		curr = sConflictInformBase->next;
		while(curr != sConflictInformBase)
		{
			prev = curr;
			curr = curr->next;
			prev->prev->next = curr;
			curr->prev = prev->prev;
			if(prev->flag == 1)
			{
				sysprint(0,PMTG.ThreadId,"[PMTG]: delete select conflict train");
				OsDeselectConflict(prev);
			}
			free(prev);
		};
		if(sConflictInformBase->flag == 1)
		{
			sysprint(0,PMTG.ThreadId,"[PMTG]: delete select conflict train");
			OsDeselectConflict(curr);
		}
		
		free(sConflictInformBase);
		sConflictInformBase = NULL;
		m_ListBox.ResetContent( );
	}
 	nMaxConflictMessageLineNo = 1;
	return TRUE;
}

LRESULT CMyBar::OnConflictInsert(WPARAM wParam, LPARAM lParam)
{
    if(!strcmp(strTitle,"人工报点窗"))
       return FALSE;

	Pstruct _tagConflictInfo *conflict_info = (Pstruct _tagConflictInfo *)wParam;
	CONFLICTINFORM *rec;
	Pstruct _tagConflictInformNode *curr,*prev;
	char *ptr;
	int theStatus = 1,i;

	do {
		if(conflict_info == NULL || conflict_info->count == 0)
		{
			sysprint(0,PMTG.ThreadId,"[PMTG]:invalid parameter");
			break;
		}

		if(sConflictInformBase == NULL)
		{
			rec = (CONFLICTINFORM *)&conflict_info->rec[0];

			if((sConflictInformBase = (Pstruct _tagConflictInformNode *)malloc(sizeof(Pstruct _tagConflictInformNode))) == NULL)
			{
				sysprint(0,PMTG.ThreadId,"[PMTG]: malloc failed in file %s at line %d",
					__FILE__,__LINE__);
				break;
			}
			curr = sConflictInformBase->next = sConflictInformBase->prev = sConflictInformBase;
			curr->data = *rec;
			curr->usLineNo = 1;
			nMaxConflictMessageLineNo += 1;
			curr->flag = 0;
			if(ptr = OsShowConflict(PMTG.ThreadId,rec))
				m_ListBox.AddString(ptr);
			for(i=1;i < conflict_info->count;i++)
			{
				rec = (CONFLICTINFORM *)&conflict_info->rec[i];

				prev = curr;
				curr = (Pstruct _tagConflictInformNode *)malloc(sizeof(Pstruct _tagConflictInformNode));
				if(curr == NULL)
				{
					sysprint(0,PMTG.ThreadId,"[PMTG]: malloc failed in file %s at line %d",
						__FILE__,__LINE__);
					break;
				}
				// insert after
				curr->next = prev->next;
				curr->prev = prev;
				prev->next->prev = curr;
				prev->next = curr;

				curr->data = *rec;
				curr->flag = 0;
				curr->usLineNo = nMaxConflictMessageLineNo;
				nMaxConflictMessageLineNo += 1;
				if(ptr = OsShowConflict(PMTG.ThreadId,rec))
					m_ListBox.AddString(ptr);
			}
		} else {
			for(i=0;i < conflict_info->count;i++)
			{
				rec = (CONFLICTINFORM *)&conflict_info->rec[i];

				curr = sConflictInformBase;
				// this is a new record
				prev = curr;
				curr = (Pstruct _tagConflictInformNode *)malloc(sizeof(Pstruct _tagConflictInformNode));
				if(curr == NULL)
				{
					sysprint(0,PMTG.ThreadId,"[PMTG]: malloc failed in file %s at line %d",
						__FILE__,__LINE__);
					break;
				}
				// insert before first 
				curr->next = prev;
				curr->prev = prev->prev;
				prev->prev->next = curr;
				prev->prev = curr;
				curr->flag = 0;
				curr->usLineNo = nMaxConflictMessageLineNo;
				nMaxConflictMessageLineNo += 1;
				curr->data = *rec;
				if(ptr = OsShowConflict(PMTG.ThreadId,rec))
					m_ListBox.AddString(ptr);
			}
		}
		theStatus = 0;
	}while(0);
	if(theStatus)
		MessageBeep(MB_ICONASTERISK);
	return FALSE;
}
	
static char  *OsShowConflict(PROCESSID ProcessId,CONFLICTINFORM *ciCurr)
{
	static char pbShowMsgBuffer[256];
	Pstruct _tagConflictElement *tceInfo1,*tceInfo2;
	short rs = 0;
	char bLastByte;
	short i;

	pbShowMsgBuffer[0] = pbShowMsgBuffer[1] = 0x20;
	if(ciCurr->ele1.usElementType <= ciCurr->ele2.usElementType)
	{
		tceInfo1 = (Pstruct _tagConflictElement *)&(ciCurr->ele1);
		tceInfo2 = (Pstruct _tagConflictElement *)&(ciCurr->ele2);
	}else {
		tceInfo2 = (Pstruct _tagConflictElement *)&(ciCurr->ele1);
		tceInfo1 = (Pstruct _tagConflictElement *)&(ciCurr->ele2);
	}
	/******
	ptr = (char *)ciCurr;
	for(i=0;i < sizeof(Pstruct TRAIN_CONFLICT_ELE) * 2;i++)
		sysprint(0,ProcessId,"[%d ,%x]",i,ptr[i]);
	******/
	if(tceInfo1->trainid[2] == 0x00)		
	{
		tceInfo1->trainid[2] = 0x20;
		tceInfo1->trainid[3] = 0x00;
	}
	if(tceInfo1->trainid[3] == 0x00)		
	{
		tceInfo1->trainid[3] = 0x20;
		tceInfo1->trainid[4] = 0x00;
	}
	if(tceInfo2->trainid[2] == 0x00)		
	{
		tceInfo2->trainid[2] = 0x20;
		tceInfo2->trainid[3] = 0x00;
	}
	if(tceInfo2->trainid[3] == 0x00)		
	{
		tceInfo2->trainid[3] = 0x20;
		tceInfo2->trainid[4] = 0x00;
	}
	switch(tceInfo1->usElementType)
	{
		case CAD_STA_ABS:
			i = strlen(tceInfo1->trainid);
			while((tceInfo1->trainid[i] > 0x39 || 
				tceInfo1->trainid[i] < 0x30) && i >= 0)
				i--;
			bLastByte = tceInfo1->trainid[i] - 0x30;
 			i = strlen(tceInfo2->trainid);
			while((tceInfo2->trainid[i] > 0x39 || 
				tceInfo2->trainid[i] < 0x30) && i >= 0)
				i--;
			if(bLastByte % 2 == 0 && tceInfo2->trainid[i] % 2 == 0)
			{
				tceInfo2->usStationNo--;
				/*
				if(tceInfo2->usStationNo >= station_count)
					tceInfo2->usStationNo--;
			*/
			}else if(bLastByte % 2 == 1 && 
				tceInfo2->trainid[i] %2 == 1) {
				tceInfo2->usStationNo++;
				if(tceInfo2->usStationNo <= 0)
					tceInfo2->usStationNo--;
			}
			switch(tceInfo2->usElementType)
			{
				case CAD_STA_ABS:

					sprintf(pbShowMsgBuffer + 2,
						"列车 %s / %s 在 %s -- %s 区间间隔时间不足",
						tceInfo1->trainid,tceInfo2->trainid,
						gpcdef.GetStationName(tceInfo1->usStationNo),
						gpcdef.GetStationName(tceInfo2->usStationNo));
					break;
				case CAD_STA_ABS_PROHIBIT:
					if(tceInfo1->usStationNo == tceInfo2->usStationNo){
		 				i = strlen(tceInfo1->trainid);
						while((tceInfo1->trainid[i] > 0x39 || 
							tceInfo1->trainid[i] < 0x30) && i >= 0)
							i--;
						bLastByte = tceInfo1->trainid[i] - 0x30;
						if(bLastByte % 2 == 0 )
						{
							tceInfo2->usStationNo--;
							/*
							if(tceInfo2->usStationNo >= station_count)
								tceInfo2->usStationNo--;
						*/
						}else if(bLastByte % 2 == 1) {
							tceInfo2->usStationNo++;
							if(tceInfo2->usStationNo <= 0)
								tceInfo2->usStationNo--;
						}
					}
					sprintf(pbShowMsgBuffer + 2,
						"列车 %s 在 %s -- %s 区间进入封锁区间",
						tceInfo1->trainid,
						gpcdef.GetStationName(tceInfo1->usStationNo),
						gpcdef.GetStationName(tceInfo2->usStationNo));
					break;
				case CAD_STA_ABS_SUPER_WIDTH:

					sprintf(pbShowMsgBuffer + 2,
						"列车 %s / %s 在 %s -- %s 区间因超限互相影响",
						tceInfo1->trainid,tceInfo2->trainid,
						gpcdef.GetStationName(tceInfo1->usStationNo),
						gpcdef.GetStationName(tceInfo2->usStationNo));
					break;
				case CAD_STA_ABS_MEET_PROHIBIT:

					sprintf(pbShowMsgBuffer + 2,
						"列车 %s / %s 在 %s -- %s 区间因禁会互相影响",
						tceInfo1->trainid,tceInfo2->trainid,
						gpcdef.GetStationName(tceInfo1->usStationNo),
						gpcdef.GetStationName(tceInfo2->usStationNo));
					break;
				default:
					rs = INVALID_MSGID_CONJOIN;
					break;
			}
			break;
		case CAD_STA_SIDE:
			switch(tceInfo2->usElementType)
			{
				case CAD_STA_SIDE:

					sprintf(pbShowMsgBuffer + 2,
						"列车 %s / %s 在 %s 站 %2d 股道间隔时间不足",
						tceInfo1->trainid,tceInfo2->trainid,
						gpcdef.GetStationName(tceInfo1->usStationNo),
						tceInfo2->usElementId);
					break;
				case CAD_STA_SIDE_PROHIBIT:

					sprintf(pbShowMsgBuffer + 2,
						"列车 %s 在 %s 站 %2d 股道 进入封锁的股道",
						tceInfo1->trainid,
						gpcdef.GetStationName(tceInfo1->usStationNo),
						tceInfo2->usElementId);
					break;
				case CAD_STA_SIDE_SUPER_WIDTH:

					sprintf(pbShowMsgBuffer + 2,
						"列车 %s / %s 在 %s 站 %2d 股道因超限互相影响",
						tceInfo1->trainid,tceInfo2->trainid,
						gpcdef.GetStationName(tceInfo1->usStationNo),
						tceInfo2->usElementId);
					break;
				case CAD_STA_SIDE_START_DIFFICULT:

					sprintf(pbShowMsgBuffer + 2,
						"列车 %s 在 %s 站 %2d 股道停在启动困难站",
						tceInfo1->trainid,
						gpcdef.GetStationName(tceInfo1->usStationNo),
						tceInfo2->usElementId);
					break;
				case CAD_STA_SIDE_LENGTH:

					sprintf(pbShowMsgBuffer + 2,
						"列车 %s / %s 在 %s 站 %2d 股道因超长互相影响",
						tceInfo1->trainid,tceInfo2->trainid,
						gpcdef.GetStationName(tceInfo1->usStationNo),
						tceInfo2->usElementId);
					break;
				default:
					rs = INVALID_MSGID_CONJOIN;
					break;
			}
			break;
		case CAD_STA_ENTRY:

			sprintf(pbShowMsgBuffer + 2,
				"列车 %s / %s 在 %s 站咽喉间隔时间不足",
				tceInfo1->trainid,tceInfo2->trainid,
				gpcdef.GetStationName(tceInfo1->usStationNo));
			break;
		case CAD_STA_THROAT:

			sprintf(pbShowMsgBuffer + 2,
				"列车 %s / %s 在 %s 站咽喉间隔时间不足",
				tceInfo1->trainid,tceInfo2->trainid,
				gpcdef.GetStationName(tceInfo1->usStationNo));
			break;
		case CAD_STA_SPECIAL_ENTRY:

			sprintf(pbShowMsgBuffer + 2,
				"列车 %s / %s 在 %s 站咽喉间隔时间不足",
				tceInfo1->trainid,tceInfo2->trainid,
				gpcdef.GetStationName(tceInfo1->usStationNo));
			break;
		default:
			rs = INVALID_MSGID_CONJOIN;
			break;
	}
	if(rs == INVALID_MSGID_CONJOIN)
	{
		sysprint(0,ProcessId,
			"[CONFLICT]:invalid conjoin usElementType (%d %d) [in %s at line %d]",
			tceInfo1->usElementType,tceInfo2->usElementType,__FILE__,__LINE__);
		return NULL;
	}
	return pbShowMsgBuffer;
}