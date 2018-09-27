// ChangeTrainID.cpp : implementation file
//

#include "stdafx.h"
#include "bureau.h"
#include "resource.h"
#include "InsertTrainDlg.h"
#include "oscalls.h"
#include "sysshare.H"
#include "mainframe.h"
#include "function.h"
#include <BaseMsg/clbth.h>
#include <BaseMsg/gpc_msgid.h>
#include "DlgMessageBox.h"
#include "db_msgproc.h"
#include "tg_basicpara.h"
#include "tgmsg.h"
#include "utility\mmi_function_config.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChangeTrainID dialog
extern void ShowErrorMsg();

/////////////////////////////////////////////////////////////////////////////
// CInsertTrain dialog

CInsertTrain::CInsertTrain(int con_id,LPCTSTR text,CWnd* pParent /*=NULL*/)
	: CDialog(CInsertTrain::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInsertTrain)
	m_strInsertTrainId = _T("");
	m_strArrivalTrainId = _T("");
	
 	//}}AFX_DATA_INIT
	if(text != NULL)
	{
		m_nInsertMethod = 1;
		m_strInsertTrainId = text;
	} else 
		m_nInsertMethod = 0;
	rec_flag = TRUE;

	m_nSectionID=con_id;
	this->passway_count = 0;
	this->Flag_moredata = FALSE;
	// this->LoadPasswayInfo(); // 转移到OnInitDialog中,因为考虑本地数据可直接读出

	m_nKBStyle = KB_BOTTOM | KB_CAPS;
	m_pKbDlg = NULL;
	m_bAutoSelectShiftAddTrain=FALSE;
}

void CInsertTrain::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInsertTrain)
	DDX_Control(pDX, IDC_ROUTE, m_strRoute);
	DDX_Text(pDX, IDC_INSERT_TRAINID, m_strInsertTrainId);
	DDV_MaxChars(pDX, m_strInsertTrainId, 9);
	DDX_Text(pDX, IDC_ARRIVAL_TRAINID, m_strArrivalTrainId);
	DDV_MaxChars(pDX, m_strArrivalTrainId, 9);
	DDX_Control(pDX, IDC_RUN_TYPE, m_strAbsRunType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInsertTrain, CDialog)
	//{{AFX_MSG_MAP(CInsertTrain)
 	ON_BN_CLICKED(IDC_INSERT_GRAPHIC, OnInsertGraphic)
	ON_BN_CLICKED(IDC_INSERT_LIST, OnInsertList)
 	ON_BN_CLICKED(IDC_BUTTON_OK, OnButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnButtonCancel)
	ON_BN_CLICKED(IDC_GRAPHIC, OnGraphic)
	ON_BN_CLICKED(IDC_BY_SHIFT, OnShift)
	ON_CBN_EDITCHANGE(IDC_ROUTE, OnEditchangeRoute)
	ON_CBN_SELCHANGE(IDC_ROUTE, OnSelchangeRoute)
	ON_WM_DESTROY()
	ON_WM_MOVING()
	ON_WM_WINDOWPOSCHANGED()
	ON_EN_CHANGE(IDC_INSERT_TRAINID,OnEditchangeTrainID)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInsertTrain message handlers

char * CInsertTrain::GetTrainId(char *pbInputBuffer)
{
	if(pbInputBuffer != NULL)
		strcpy(pbInputBuffer,(LPCTSTR)m_strInsertTrainId);
	return pbInputBuffer;
}
 
void CInsertTrain::OnInsertGraphic() 
{
	// TODO: Add your control notification handler code here
	m_nInsertMethod = 0;	
}

void CInsertTrain::OnInsertList() 
{
	// TODO: Add your control notification handler code here
	m_nInsertMethod = 1;	
}

BOOL CInsertTrain::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_nRouteNo = -1;
	m_nCurrFirstRouteNo = 0;

	CButton *pSButton = (CButton *)GetDlgItem(IDC_START);
	CButton *pAButton = (CButton *)GetDlgItem(IDC_FETCH);
	CButton *pTButton = (CButton *)GetDlgItem(IDC_TERMINAL);
	CButton *pGButton = (CButton *)GetDlgItem(IDC_GIVEOUT);


	pSButton->EnableWindow(TRUE);
	pAButton->EnableWindow(TRUE);

	if(GetMMIFunctionConfig()->GetConfigIntegerItem(CONFIG_DEFAULT_START)==1)
	{
		pSButton->SetCheck(1);
		pAButton->SetCheck(0);
	}
	else
	{
		pSButton->SetCheck(0);
		pAButton->SetCheck(1);
	}

	if(GetMMIFunctionConfig()->GetConfigIntegerItem(CONFIG_DEFAULT_TERMINAL)==1)
	{
		pTButton->SetCheck(1);
		pGButton->SetCheck(0);
	}
	else
	{
		pTButton->SetCheck(0);
		pGButton->SetCheck(1);
	}

	int nScreenNum = gpcdef.GetScreenNum();
	if(nScreenNum != 1)
	{
		int x_pos = GetSystemMetrics(SM_CXSCREEN); 
		x_pos = x_pos/(nScreenNum * 4) ;
		int y_pos = GetSystemMetrics(SM_CYSCREEN);
		y_pos = y_pos/4;
		SetWindowPos(&CWnd::wndTop,x_pos,y_pos,0,0,SWP_NOSIZE);
	}

	// TODO: Add extra initialization here
	if(m_pKbDlg == NULL)
	{	
		m_pKbDlg = new CSoftKeyBoardDlg(this, m_nKBStyle);
		m_pKbDlg->Create(IDD_KEYBOARD);
		m_pKbDlg->ShowWindow(SW_SHOWNORMAL);
	}

	if(m_pKbDlg != NULL)
	{	
		SHORT	rs = GetKeyState( VK_CAPITAL );
		switch(m_nKBStyle&0xf0)//keyboard caps 
		{
		case KB_CAPS:
			{
				if(rs != 1)
				{
					keybd_event(VK_CAPITAL, 0, 0, 0);
					keybd_event(VK_CAPITAL, 0, KEYEVENTF_KEYUP, 0);
				}
			}
			break;
		case KB_LOWER:
			{
				if(rs == 1)
				{
					keybd_event(VK_CAPITAL, 0, 0, 0);
					keybd_event(VK_CAPITAL, 0, KEYEVENTF_KEYUP, 0);
				}
				
			}
			break;
		default:
			break;
		}
	}

	DWORD stytle = m_strRoute.GetStyle();
	stytle &= ~CBS_SORT;
	  
	m_strRoute.AddString("自由画线");
	m_strRoute.SetItemData (0,0);
 	m_strRoute.SetCurSel(0);
	LoadPasswayInfo();  // 考虑本地数据处理后,移至此处!
	             
	if(GetTGFunctionConfig()->IsFreedrawAddTrainShiftAndBasic())
	{
		CButton *pBtn = (CButton *)GetDlgItem(IDC_BY_SHIFT);
		if(pBtn)
		{
			pBtn->SetWindowText("使用日班计划没有再图定");
		}
	}

	if(!GetTGFunctionConfig()->IsShowArriveTrainid())
	{
		CWnd *pBtn = GetDlgItem(IDC_INSERT_STATIC_BIANGENG);
		if(pBtn)
			pBtn->ShowWindow(SW_HIDE);

		pBtn = GetDlgItem(IDC_ARRIVAL_TRAINID);
		if(pBtn)
			pBtn->ShowWindow(SW_HIDE);

		CRect rt1, rt;
		pBtn = GetDlgItem(IDC_ARRIVAL_STATIC2);
		if(pBtn)
		{
			pBtn->GetWindowRect(&rt1);
			pBtn->MoveWindow(15,30, rt1.Width(), rt1.Height());
		}

		pBtn = GetDlgItem(IDC_INSERT_TRAINID);
		if(pBtn)
		{
			pBtn->GetWindowRect(&rt);
			pBtn->MoveWindow(15+rt1.Width()+15, 30, rt.Width(), rt.Height());
		}
	}

	CStringArray name_list;
	CWordArray run_type_list;
	int count=GetTGBasicParam()->GetAllAbsTimeTypeName(name_list, run_type_list);
	
	int nItem=m_strAbsRunType.AddString("默认区间时分");
	m_strAbsRunType.SetItemData(nItem, 255);
	for(int i=0; i<count; i++)
	{
		nItem=m_strAbsRunType.AddString(name_list.GetAt(i));
		m_strAbsRunType.SetItemData(nItem, run_type_list.GetAt(i));
	}
	m_strAbsRunType.SetCurSel(0);
	if(count<=0)
	{
		CWnd* pWnd=GetDlgItem(IDC_INSERT_STATIC23);
		if(pWnd)
			pWnd->ShowWindow(SW_HIDE);
		m_strAbsRunType.ShowWindow(SW_HIDE);
		m_strAbsRunType.EnableWindow(FALSE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CInsertTrain::GetInsertTrainMethod(int &nRouteNo,int &nMode)
{
    return TRUE;
}

void CInsertTrain::OnButtonOk() 
{
 	UpdateData(TRUE);
 
    //车次中不允许有空格
	if(m_strInsertTrainId.Find(" ")>=0)
	{
		MessageBox("车次中有空格,请新输入!");
		return;
	}

	if(m_strArrivalTrainId.GetLength() > 0 && m_strArrivalTrainId.Find(" ")>=0)
	{
		MessageBox("接入车次中有空格,请新输入!");
		return;
	}
	if(m_strInsertTrainId.GetLength( ) != 0)
	{
		/*
		if(m_pDoc->FindTrain(m_strInsertTrainId) != -1)
		{
			AfxMessageBox("列车已经存在");
	    	return;
		}*/
	}
	else 
	{
		AfxMessageBox("请输入车次号");	
		return;
	}
 
	CDialog::OnOK( );
}

void CInsertTrain::OnButtonCancel() 
{
	CDialog::OnCancel( );
}

void CInsertTrain::OnOK() 
{
	UpdateData(TRUE);

	if(m_strArrivalTrainId.GetLength() > 0)
	{
		m_strArrivalTrainId = ChangeAlpha(m_strArrivalTrainId);
		if( m_strArrivalTrainId.Find(" ")>=0)
		{
			MessageBox("接入车次中有空格,请新输入!");
			return  ;
		}
	}
	m_strInsertTrainId = ChangeAlpha(m_strInsertTrainId);
	if( m_strInsertTrainId.Find(" ")>=0)
	{
		MessageBox("车次中有空格,请新输入!");
		return  ;
	}
	UpdateData(FALSE);

	if(m_strInsertTrainId.GetLength( ) == 0)
	{
		AfxMessageBox("请输入在车次框中输入车次号");	
	    return;
	}

	m_bSecret=false;
	CButton* pBtn=(CButton *)GetDlgItem(IDC_SECRET_TRAIN);
	if(pBtn && pBtn->GetCheck())
	{
		CString str;
		str.Format("请注意,列车 %s 将设置为保密列车！ \n\n确认设置保密列车选择\"是\"\n否则选择\"否\"", m_strInsertTrainId);
		if(IDYES==MessageBox(str, "提示", MB_YESNO|MB_ICONWARNING))
			m_bSecret=true;
	}
	
	m_nCurSel = m_strRoute.GetCurSel( ) ;
	m_nRouteNo = m_strRoute.GetItemData(m_nCurSel) ;

	int nState;
	CButton *pWnd;
    pWnd = (CButton *)GetDlgItem(IDC_GRAPHIC);
    if(nState = pWnd->GetCheck( ))
       m_bAddGraphic = TRUE;
	else
       m_bAddGraphic = FALSE;

	pWnd = (CButton *)GetDlgItem(IDC_BY_SHIFT);
    if(nState = pWnd->GetCheck( ))
       m_bAddShift = TRUE;
	else
       m_bAddShift = FALSE;
		
    pWnd = (CButton *)GetDlgItem(IDC_START);
    if(nState = pWnd->GetCheck( ))
		m_nStartMode = SCHD_START;
	else
		m_nStartMode = SCHD_ACCEPT;

    pWnd = (CButton *)GetDlgItem(IDC_TERMINAL);
    if(nState = pWnd->GetCheck( ))
	    m_nEndMode = SCHD_TERMINAL;
	else
		m_nEndMode = SCHD_GIVEOUT;

    pWnd = (CButton *)GetDlgItem(IDC_BE_CHANGED);
    if(nState = pWnd->GetCheck( ))
		m_bChanged = TRUE;
	else
		m_bChanged = FALSE;

	int nSel=m_strAbsRunType.GetCurSel();
	if(nSel==-1)
	{
		AfxMessageBox("请选择区间运行时分类型");
	}
	m_nRunType=m_strAbsRunType.GetItemData(nSel);
	CString strSelText;
	m_strAbsRunType.GetLBText(nSel, strSelText);
	if(m_nRunType!=255)
	{
		CString str;
		str.Format("请注意,列车 %s 将采用 %s 区间运行时分类型！ \n\n确定选择\"是\"\n否则选择\"否\"", m_strInsertTrainId, strSelText);
		if(IDYES!=MessageBox(str, "提示", MB_YESNO|MB_ICONWARNING))
			return;
	}
	CDialog::OnOK();
}

void CInsertTrain::OnCancel() 
{
	CDialog::OnCancel();
}
void CInsertTrain::OnShift()
{
	CButton *pWnd;
	//rec_flag = !rec_flag;
	if(((CButton *)GetDlgItem(IDC_BY_SHIFT))->GetCheck()==1
		|| ((CButton *)GetDlgItem(IDC_GRAPHIC))->GetCheck()==1)
		rec_flag=FALSE;
	else
		rec_flag=TRUE;
    pWnd = (CButton *)GetDlgItem(IDC_START);
 	pWnd->EnableWindow (rec_flag);
    pWnd = (CButton *)GetDlgItem(IDC_FETCH);
	pWnd->EnableWindow (rec_flag);
    pWnd = (CButton *)GetDlgItem(IDC_TERMINAL);
	pWnd->EnableWindow (rec_flag);
    pWnd = (CButton *)GetDlgItem(IDC_GIVEOUT);
	pWnd->EnableWindow (rec_flag);
    pWnd = (CButton *)GetDlgItem(IDC_BE_CHANGED);
	pWnd->EnableWindow (rec_flag);
    pWnd = (CButton *)GetDlgItem(IDC_ROUTE);
	pWnd->EnableWindow (rec_flag);
	m_strAbsRunType.EnableWindow(rec_flag);
	pWnd = (CButton *)GetDlgItem(IDC_GRAPHIC);
	pWnd->SetCheck(0);
}

void CInsertTrain::OnGraphic() 
{
	CButton *pWnd;
	//rec_flag = !rec_flag;
	if(((CButton *)GetDlgItem(IDC_BY_SHIFT))->GetCheck()==1
		|| ((CButton *)GetDlgItem(IDC_GRAPHIC))->GetCheck()==1)
		rec_flag=FALSE;
	else
		rec_flag=TRUE;
    pWnd = (CButton *)GetDlgItem(IDC_START);
 	pWnd->EnableWindow (rec_flag);
    pWnd = (CButton *)GetDlgItem(IDC_FETCH);
	pWnd->EnableWindow (rec_flag);
    pWnd = (CButton *)GetDlgItem(IDC_TERMINAL);
	pWnd->EnableWindow (rec_flag);
    pWnd = (CButton *)GetDlgItem(IDC_GIVEOUT);
	pWnd->EnableWindow (rec_flag);
    pWnd = (CButton *)GetDlgItem(IDC_BE_CHANGED);
	pWnd->EnableWindow (rec_flag);
    pWnd = (CButton *)GetDlgItem(IDC_ROUTE);
	pWnd->EnableWindow (rec_flag);
	m_strAbsRunType.EnableWindow(rec_flag);
	pWnd = (CButton *)GetDlgItem(IDC_BY_SHIFT);
	pWnd->SetCheck(0);

}
void CInsertTrain::OnEditchangeTrainID()
{
	OnEnChangeEdit1();

	if(!m_bAutoSelectShiftAddTrain)
		return;
	BYTE run_prio=255,train_type=255,run_type=255;
	BOOL can_free_add=TRUE;
	CString tid;
	GetDlgItem(IDC_INSERT_TRAINID)->GetWindowText(tid);
	if(GetTGBasicParam()->GetTrainPrio(tid,&train_type,&run_type,&run_prio))
	{
		if(!GetTGFunctionConfig()->IsFreeAddTrain(train_type))
		{
				((CButton *)GetDlgItem(IDC_BY_SHIFT))->SetCheck(1);
				((CButton *)GetDlgItem(IDC_GRAPHIC))->SetCheck(0);
				 OnShift();
		}
		else
		{
			    ((CButton *)GetDlgItem(IDC_BY_SHIFT))->SetCheck(0);
				((CButton *)GetDlgItem(IDC_GRAPHIC))->SetCheck(0);
				 OnShift();
		}
	}
}

void CInsertTrain::OnEditchangeRoute() 
{
}

void CInsertTrain::OnSelchangeRoute() 
{
 	m_nCurSel = m_strRoute.GetCurSel( ) ;
 	CButton *pWnd;
 	pWnd = (CButton *)GetDlgItem(IDC_GRAPHIC);
    if(m_nCurSel != 0)
	{
		m_nRouteNo = m_strRoute.GetItemData(m_nCurSel) ;
		
 	    pWnd->EnableWindow (FALSE);
		pWnd = (CButton *)GetDlgItem(IDC_START);
		pWnd->EnableWindow (TRUE);
		pWnd = (CButton *)GetDlgItem(IDC_FETCH);
		pWnd->EnableWindow (TRUE);
		pWnd = (CButton *)GetDlgItem(IDC_TERMINAL);
		pWnd->SetCheck(FALSE);
		pWnd->EnableWindow (TRUE);
		pWnd = (CButton *)GetDlgItem(IDC_GIVEOUT);
		pWnd->SetCheck(TRUE);
		pWnd->EnableWindow (TRUE);	
		pWnd = (CButton *)GetDlgItem(IDC_BE_CHANGED);
		pWnd->EnableWindow (FALSE);
		int inflag,outflag;
		if(GetTGBasicParam()->GetPasswayFlag(m_nSectionID,m_nRouteNo,&inflag,&outflag))
		{
			if(inflag==SCHD_START) // 只能始发
			{
				((CButton *)GetDlgItem(IDC_START))->SetCheck(1);
				((CButton *)GetDlgItem(IDC_FETCH))->SetCheck(0);
				((CButton *)GetDlgItem(IDC_START))->EnableWindow(FALSE);
				((CButton *)GetDlgItem(IDC_FETCH))->EnableWindow(FALSE);
			}
			if(outflag==SCHD_TERMINAL) // 只能终到
			{
				((CButton *)GetDlgItem(IDC_TERMINAL))->SetCheck(1);
				((CButton *)GetDlgItem(IDC_GIVEOUT))->SetCheck(0);
				((CButton *)GetDlgItem(IDC_TERMINAL))->EnableWindow(FALSE);
				((CButton *)GetDlgItem(IDC_GIVEOUT))->EnableWindow(FALSE);
			}
		}
	}else{
	    pWnd->EnableWindow(TRUE);
		pWnd = (CButton *)GetDlgItem(IDC_BE_CHANGED);
		pWnd->EnableWindow (TRUE);

		pWnd = (CButton *)GetDlgItem(IDC_TERMINAL);
		pWnd->SetCheck(FALSE);
		pWnd->EnableWindow(FALSE);

		pWnd = (CButton *)GetDlgItem(IDC_GIVEOUT);
		pWnd->SetCheck(FALSE);
		pWnd->EnableWindow(FALSE);


	}
	
}
// this function is added by cf in order to send message LOAD_PASSWAY_INFO to CAD 
void CInsertTrain::LoadPasswayInfo()
{
	passway_count=200; // qiaoyt 20090817
	if(GetTGBasicParam()->GetPasswayInfo(m_nSectionID,info,&passway_count)) // 如果本地可读出，则不用发服务器！
	{
		for(int i=0;i<passway_count;i++)
		{
			int itemindex=m_strRoute.AddString(info[i].Pass_way_name);
			m_strRoute.SetItemData(itemindex,info[i].Pass_way_id);
		}
	}
}
//this function is added by cf in order to answer for UM_PASSWAY_INFO message

//add by cf
void CInsertTrain::fillDataStruct(int total, int pos)
{
}

void CInsertTrain::OnDestroy() 
{
	CDialog::OnDestroy();
	
	if(m_pKbDlg != NULL)
	{
		delete m_pKbDlg;
		m_pKbDlg = NULL;
	}
}
void CInsertTrain::OnMoving( UINT nSide, LPRECT lpRect )
{
	if((m_pKbDlg != NULL) && (::IsWindow(m_pKbDlg->GetSafeHwnd())))
	{
		CRect kbrc;
		m_pKbDlg->GetWindowRect(&kbrc);
		
		switch(m_nKBStyle&0x0f)//position 
		{
		case KB_TOP:
			{
				m_pKbDlg->MoveWindow(lpRect->left, lpRect->top-kbrc.Height(),
					kbrc.Width(), kbrc.Height());
			}
			break;
		case KB_LEFT:
			{
				m_pKbDlg->MoveWindow(lpRect->left-kbrc.Width(), lpRect->top, 
					kbrc.Width(), kbrc.Height());				
			}
			break;
		case KB_BOTTOM:
			{
				m_pKbDlg->MoveWindow(lpRect->left, lpRect->bottom, 
					kbrc.Width(), kbrc.Height());
			}
			break;
		case KB_RIGHT:
			{
				m_pKbDlg->MoveWindow(lpRect->right, lpRect->top, 
					kbrc.Width(), kbrc.Height());			
			}
			break;
		default:
			break;
		}
	}


	CDialog::OnMoving(nSide, lpRect);
}
void CInsertTrain::OnWindowPosChanged( WINDOWPOS* lpwndpos )
{
	if((m_pKbDlg != NULL) && (::IsWindow(m_pKbDlg->GetSafeHwnd())))
	{
		CRect kbrc;
		m_pKbDlg->GetWindowRect(&kbrc);
		switch(m_nKBStyle&0x0f)//position 
		{
		case KB_TOP:
			{
				m_pKbDlg->MoveWindow(lpwndpos->x, lpwndpos->y-kbrc.Height(),
					kbrc.Width(), kbrc.Height());
			}
			break;
		case KB_LEFT:
			{
				m_pKbDlg->MoveWindow(lpwndpos->x-kbrc.Width(), lpwndpos->y, 
					kbrc.Width(), kbrc.Height());				
			}
			break;
		case KB_BOTTOM:
			{
				m_pKbDlg->MoveWindow(lpwndpos->x, lpwndpos->y+lpwndpos->cy, 
					        kbrc.Width(), kbrc.Height());
			}
			break;
		case KB_RIGHT:
			{
				m_pKbDlg->MoveWindow(lpwndpos->x+lpwndpos->cx, lpwndpos->y, 
							kbrc.Width(), kbrc.Height());			
			}
			break;
		default:
			break;
		}
	}

	CDialog::OnWindowPosChanged( lpwndpos );
}

void CInsertTrain::OnEnChangeEdit1()
{
	CString strTrainId;
	CWnd* pWnd = GetDlgItem(IDC_INSERT_TRAINID);
	if(NULL != pWnd)
		pWnd->GetWindowText(strTrainId);

	if(strTrainId.IsEmpty())
		return;

	CString tempTrainId;
	CString temp;

	int len = strTrainId.GetLength();
	for(int i= len-1;i>=0;i--)
	{
		char lastchar = strTrainId.GetAt(i);
		if(lastchar < '0' || lastchar > '9')
			break;
	}
	tempTrainId = strTrainId.Right(len - i - 1);
	int TrainIdNum = atoi(tempTrainId);
	tempTrainId = strTrainId.Left(len - tempTrainId.GetLength());
	if((TrainIdNum%2) == 0)
	{
		if(TrainIdNum > 0)
			temp.Format("%d", TrainIdNum-1);
		else
			temp.Format("%d", TrainIdNum+1);
		tempTrainId += temp;
	}
	else
	{
		temp.Format("%d", TrainIdNum+1);
		tempTrainId += temp;
	}

	if(!GetTGFunctionConfig()->IsShowArriveTrainid())
	{
		pWnd = GetDlgItem(IDC_ARRIVAL_TRAINID);
		if(NULL != pWnd)
			pWnd->SetWindowText(strTrainId);
	}
	else
	{
		pWnd = GetDlgItem(IDC_ARRIVAL_TRAINID);
		if(NULL != pWnd)
			pWnd->SetWindowText(tempTrainId);
	}
}

