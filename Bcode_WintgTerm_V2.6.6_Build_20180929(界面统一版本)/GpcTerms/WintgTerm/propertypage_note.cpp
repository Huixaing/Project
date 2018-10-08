// CPropertyPageNotepad.cpp : implementation file
//

#include "stdafx.h"
#include <io.h>

#include "GridCtrl_src/GridCellCheck.h"
#include "gridctrl_src/GridCellCombo.h"

#include "function.h"
#include "tgmsg.h"
#include "messages.h"
#include "memshare.h"
#include "function.h"
#include <baseMsg\msgid.h>
#include <baseMsg\gpc_msgid.h>
#include <BaseMsg\clbth.h>
#include "db_msgproc.h"
#include "TgDocument.h"
#include "propertypage_note.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CEdit* g_curEdit;//当前常用词汇要输入的编辑框

//根据文件全路径创建需要的目录
bool CreateRecursiveDir(char* filename);

TCHAR *m_sBiaoTi[4] = {_T("车站"),_T("主机"),_T("电池"),_T("票据袋")};
double g_noteGridWdith[] = {0.4,0.2,0.2,0.2};


void f_PrintJISHI(CDC* pDC,CPrintInfo* pInfo,void* pVoid); 
void DrawTextInRect(CDC *pDC,const char *str,RECT *prect,int coltap,int rowtap,BYTE type);

/////////////////////////////////////////////////////////////////////////////
// CPageNotepad property page
//IMPLEMENT_DYNCREATE(CPageNotepad, CPropertyPage)
CPageNotepad::CPageNotepad(CTgDocument *pdoc) : CPropertyPage(CPageNotepad::IDD)
{
	//{{AFX_DATA_INIT(CPageNotepad)
	m_notebook = _T("");
	m_cfTime = _T("");
	//}}AFX_DATA_INIT
	m_pDoc = pdoc;
	this->schd_type  = WORK_SCHEDULE;
	m_pGridCtrl = NULL;
	m_nRowCount = 0;
	m_pNotifyWindow=NULL;
	select_shift_id=m_pDoc->GetCurrentShiftID();
	for(int i = 0;i<  15;i++)
	{
		parsecontent[i].Empty();
	}
	g_curEdit=NULL;
}

CPageNotepad::~CPageNotepad()
{
	if(m_pGridCtrl)
	{
		delete m_pGridCtrl;
		m_pGridCtrl=NULL;
	}
}

void CPageNotepad::OnDestroy()
{
	if(m_pGridCtrl)
	{
		delete m_pGridCtrl;
		m_pGridCtrl=NULL;
	}
	CPropertyPage::OnDestroy();
}

void CPageNotepad::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageNotepad)
	DDX_Text(pDX, IDC_NOTEBOOK, m_notebook);
	DDV_MaxChars(pDX, m_notebook, 2000);
	DDX_Text(pDX, IDC_EDIT_TIME, m_cfTime);
	DDV_MaxChars(pDX, m_cfTime, 19);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageNotepad, CPropertyPage)
	//{{AFX_MSG_MAP(CPageNotepad)
	ON_WM_DESTROY()
	ON_EN_CHANGE(IDC_NOTEBOOK, OnChangeNotebook)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_EN_SETFOCUS(IDC_NOTEBOOK, OnSetfocusNotebook)
	ON_EN_SETFOCUS(IDC_EDIT_CONTENT, OnSetfocusEditContent)
	ON_EN_SETFOCUS(IDC_EDIT_TIME, OnSetfocusEditTime)
	
	ON_MESSAGE(UM_UNI_MESSAGE, OnCommMsg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageNotepad message handlers

LRESULT CPageNotepad::OnCommMsg(WPARAM wParam, LPARAM lParam) 
{
	UNI_MESSAGE_STRUCT *pMsgStruct=( UNI_MESSAGE_STRUCT *)lParam;
	if(pMsgStruct->msg_id == MSGID_MsgNotePadFormat)
	{
		CLBTH::MsgNotePadFormat *MsgNotePadFormat= ( CLBTH::MsgNotePadFormat *)pMsgStruct->pmsg;
		
		if(ProcessMessage(MsgNotePadFormat))
		{
			parseNotes();
			UpdateData(FALSE);
		}
	}
	FREE_UNI_MESSAGE_STRUCT(pMsgStruct);
	return 0;
}

BOOL CPageNotepad::ProcessMessage(const MsgNotePadFormat *pBuf)
{
	if(pBuf->con_id!=m_pDoc->GetCurrentSectionNo())
	{
		GpcLogTrainMessage(0, 0, "收到记事, 非本台, 不处理 con_id %d msg_con_id %d doc %d", m_pDoc->GetCurrentSectionNo(), pBuf->con_id, this);
		return FALSE;
	}
	if(pBuf->shift_id!=m_pDoc->GetCurrentShiftID())
	{
		GpcLogTrainMessage(0, 0, "收到记事, 非本班, 不处理 shift_id %d msg_shift_id %d doc %d", m_pDoc->GetCurrentShiftID(), pBuf->shift_id, this);
		return FALSE;
	}
	if(pBuf->schd_type!=m_pDoc->GetScheduleType())
	{
		GpcLogTrainMessage(0, 0, "收到记事, 非本班类型, 不处理 schd_type %d msg_schd_type %d doc %d", m_pDoc->GetScheduleType(), pBuf->schd_type, this);
		return FALSE;
	}

	m_pDoc->m_cPhaseNotes.ddt_id = pBuf->con_id;
	m_pDoc->m_cPhaseNotes.shift_id = pBuf->shift_id;
	m_pDoc->m_cPhaseNotes.note_len = pBuf->data_len;
	strncpy(m_pDoc->m_cPhaseNotes.note,pBuf->data,pBuf->data_len); 
	return TRUE;
}

void CPageNotepad::OnOK() 
{  
}
BOOL CPageNotepad::OnApply() 
{   	  
	return TRUE;
} 

BOOL CPageNotepad::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	SetModified(TRUE);

	return CPropertyPage::OnCommand(wParam, lParam);
}
BOOL CPageNotepad::Query_Notebook()
{
	// 清空内存
	m_pDoc->m_cPhaseNotes.ddt_id = 0;
	m_pDoc->m_cPhaseNotes.shift_id = 0;
	m_pDoc->m_cPhaseNotes.note_len = 0;
	memset(m_pDoc->m_cPhaseNotes.note, 0, 2500); 

	// 刷新界面
	parseNotes();

	// 请求
	CLBTH::MsgNotePadQuery mMsgNotePadQuery;
	mMsgNotePadQuery.shift_id = m_pDoc->GetCurrentShiftID();
	mMsgNotePadQuery.con_id = m_pDoc->GetCurrentSectionNo();
	mMsgNotePadQuery.schd_type = m_pDoc->GetScheduleType();

	long eid=GetSpecialEntityID(SERVER_TYPE_DB,mMsgNotePadQuery.con_id);
	SendTGBMsg(mMsgNotePadQuery.con_id,&mMsgNotePadQuery,eid);	

	CString noteText;
	noteText.Format("向服务器申请记事: svr_entity %d shift_id %d con_id %d schd_type %d",
		eid, 
		mMsgNotePadQuery.shift_id, 
		mMsgNotePadQuery.con_id, 
		mMsgNotePadQuery.schd_type);

	GpcLogTrainMessage(0, 0, noteText);

	return true;    
}

BOOL CPageNotepad::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	InitIdentify();

	initStationList();
	//*** CEditEx Subclass our controls ***
	//
	m_edit.SubclassDlgItem( IDC_NOTEBOOK, this );
	m_edit.setFont( -17 );

	m_edit_jiaoban.SubclassDlgItem(IDC_EDIT_CONTENT,this);
	m_edit_jiaoban.setFont(-17);
	m_edit_jiaoban.SetLimitText (100);;
	//////////////

	//加入交班记事的内容

	//	m_cfTime.SubclassDlgItem(IDC_EDIT_TIME,this);
	//	m_cfTime.SetLimitValue(86399);

	//初始化GridCtrl
	initialGridCtrl();

	//必须解析notes.note 中的内容，各个部分用 || 区分开来
	m_notebook.Empty();

	RegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgNotePadFormat,0);

	Query_Notebook();
	parseNotes();
	UpdateData(FALSE);
	InformParent();
	return TRUE;   
}
BOOL CPageNotepad::Set_Notebook()
{
	CLBTH::MsgNotePadFormat mMsgNotePadFormat;

	CString tempstr;
	mMsgNotePadFormat.shift_id = m_pDoc->GetCurrentShiftID();
	mMsgNotePadFormat.con_id = m_pDoc->GetCurrentSectionNo();
	mMsgNotePadFormat.schd_type = m_pDoc->GetScheduleType();

	tempstr=m_pDoc->m_cPhaseNotes.note;
	mMsgNotePadFormat.data_len = tempstr.GetLength();
	strncpy(mMsgNotePadFormat.data, tempstr,tempstr.GetLength());
	mMsgNotePadFormat.data[mMsgNotePadFormat.data_len-1]=0;

	long eid=GetSpecialEntityID(SERVER_TYPE_DB,mMsgNotePadFormat.con_id);
	SendTGBMsg(mMsgNotePadFormat.con_id,&mMsgNotePadFormat,eid);	

	CString noteText;
	noteText.Format("向服务器保存记事: svr_entity %d shift_id %d con_id %d schd_type %d",
		eid, 
		mMsgNotePadFormat.shift_id, 
		mMsgNotePadFormat.con_id, 
		mMsgNotePadFormat.schd_type);

	GpcLogTrainMessage(0, 0, noteText);

	if(tempstr.GetLength() >= 256)
		tempstr = tempstr.Left(254);
	GpcLogTrainMessage(0, 0, tempstr);

	Query_Notebook();

	return true;
}

void CPageNotepad::OnChangeNotebook() 
{
	UpdateData(TRUE);	
}

BOOL CPageNotepad::PreTranslateMessage(MSG* pMsg) 
{ 	
	return CPropertyPage::PreTranslateMessage(pMsg);
}

void CPageNotepad::InitIdentify()
{
	if(m_pDoc->IsQueryMode())
	{
		CWnd *pWnd;
		pWnd = GetDlgItem(IDC_BUTTON2);
		pWnd->EnableWindow(FALSE);
	}
}

void CPageNotepad::OnButton1() 
{
	UpdateData(TRUE);
	// TODO: Add your control notification handler code here
	if(AfxMessageBox("打印记事栏?",MB_YESNO | MB_DEFBUTTON1 |MB_ICONQUESTION) != IDYES  )
		return; 

	CEdit *pEdit;
	pEdit = (CEdit *)GetDlgItem(IDC_NOTEBOOK);
	pEdit->GetWindowText(print_content);

	PrintNotepadDirectly();
}

void f_PrintJISHI(CDC* pDC,CPrintInfo* pInfo,void* pVoid)
{	
	((CPageNotepad*)pVoid)->OnPrint(pDC,pInfo);	
}

void DrawTextInRect(CDC *pDC,const char *str,RECT *prect,int coltap,int rowtap,BYTE type)
{
	char substr[4];
	int x,y;
	CSize size;
	size.cx=16;size.cy=16;
	x=prect->left;
	y=prect->top;
	pDC->SetTextAlign(0); 
	while(*str){
		if(*str==10){  // ignore linefeed
			str++; 
			continue;
		}else if(*str==13){ // return
			if(type == 0){
				x=prect->left;
				y+=size.cy+coltap*size.cy/10;
			}
			str++;
			continue;
		}
		if((BYTE)*str>0x80){
			substr[0]=*str++;
			substr[1]=*str++;
			substr[2]=0;
		}else{
			substr[0]=*str++;
			substr[1]=0;
		}
		size=pDC->GetTextExtent(substr,strlen(substr));
		if((x+size.cx)>prect->right){
			x=prect->left;
			y+=size.cy+coltap*size.cy/10;
		}
		pDC->TextOut(x,y,substr,strlen(substr));
		x+=size.cx+size.cx*coltap/(10*strlen(substr));
	}
}


void CPageNotepad::OnPrint(CDC *pDC, CPrintInfo *pInfo)
{

	if(this->GetSafeHwnd() == NULL || !::IsWindow(this->GetSafeHwnd()))
		return;

	CPen pen;
	CPoint point;
	CFont font,*oldfont;
	CPen *pOldPen;
	CTime t;
	CString sdate,sbanbie;
	CString main_title;

	pen.CreatePen(PS_SOLID,2,CLR_BLACK);
	pOldPen = pDC->SelectObject(&pen);

	t=CTime::GetCurrentTime();

	/////////////获取标题
	int id=m_pDoc->GetCurrentShiftID();
	int CurYear  = id/1000000;
	int CurMonth = id/10000-CurYear*100;
	int CurDay   = id/100-(id/10000)*100;
	sdate.Format("%4d年%2d月%2d日",CurYear,CurMonth,CurDay);
	int CurBanbie=id-(id/10)*10;
	sbanbie=(CurBanbie==1) ? "第一班" : "第二班";
	CString title,temp;
	title=(LPCTSTR)gpcdef.GetSectionName(m_pDoc->m_nCurrentSectionNo);
	temp=" "+sdate +sbanbie;
	title+=temp;

	main_title=title;


	int nWidth=pDC->GetDeviceCaps(HORZRES);
	int nHeight=pDC->GetDeviceCaps(VERTRES);

	CRect DrawRect(100,100,nWidth,nHeight),rect,tempRect;		

	font.CreateFont(-MulDiv(21,-pDC->GetDeviceCaps(LOGPIXELSY),72),
		0,0,0,FW_NORMAL,0,0,0,GB2312_CHARSET,
		OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,
		VARIABLE_PITCH|FF_SWISS,_T("黑体"));

	oldfont=pDC->SelectObject(&font);

	tempRect=DrawRect;
	tempRect.InflateRect(0,-5,0,0);
	pDC->DrawText(main_title,&tempRect,DT_CENTER|DT_TOP|DT_SINGLELINE);

	DrawRect.top  = DrawRect.top - MulDiv(21,-pDC->GetDeviceCaps(LOGPIXELSY),72) ;
	tempRect=DrawRect;
	tempRect.InflateRect(0,-5,0,0);
	pDC->DrawText("记 事 栏",&tempRect,DT_CENTER|DT_TOP|DT_SINGLELINE);

	pDC->SelectObject(oldfont);
	font.DeleteObject();

	//标题部分结束

	font.CreateFont(-MulDiv(12,-pDC->GetDeviceCaps(LOGPIXELSY),72),
		0,0,0,FW_BOLD,0,0,0,GB2312_CHARSET,
		OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,
		VARIABLE_PITCH|FF_SWISS,_T("黑体"));
	oldfont = pDC->SelectObject(&font);
	CSize x_Size;
	CString strDate ="打印日期和时间: ";

	DrawRect.top  = DrawRect.top - MulDiv(21,-pDC->GetDeviceCaps(LOGPIXELSY),72) ;
	tempRect=DrawRect;
	tempRect.InflateRect(0,-5,0,0);

	int bottom_line;
	bottom_line = tempRect.top;
	pDC->TextOut(tempRect.left,bottom_line - MulDiv(21,-pDC->GetDeviceCaps(LOGPIXELSY),72),strDate);
	pDC->SelectObject(oldfont);
	font.DeleteObject();


	font.CreateFont(-MulDiv(12,-pDC->GetDeviceCaps(LOGPIXELSY),72),
		0,0,0,FW_BOLD,0,0,0,GB2312_CHARSET,
		OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,
		VARIABLE_PITCH|FF_SWISS,_T("宋体"));
	oldfont = pDC->SelectObject(&font);
	x_Size = pDC->GetTextExtent(strDate);
	strDate.Format("%d年%d月%d日%d时%d分",t.GetYear(),t.GetMonth(),t.GetDay(),t.GetHour(),t.GetMinute());
	pDC->TextOut(tempRect.left + x_Size.cx + 10,bottom_line -MulDiv(21,-pDC->GetDeviceCaps(LOGPIXELSY),72),strDate);
	pDC->SelectObject(oldfont);
	font.DeleteObject();


	font.CreateFont(-MulDiv(12,-pDC->GetDeviceCaps(LOGPIXELSY),72),
		0,0,0,FW_BOLD,0,0,0,GB2312_CHARSET,
		OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,
		VARIABLE_PITCH|FF_SWISS,_T("黑体"));
	oldfont = pDC->SelectObject(&font);
	CString strTime = "调度员姓名:  ";
	temp.Format("%s",gpcdef.GetCurrentUsername());
	strTime += temp;
	CSize size_box = pDC->GetTextExtent(strTime);
	strTime = "调度员姓名: ";
	pDC->TextOut(tempRect.right - size_box.cx - 120 ,bottom_line -MulDiv(21,-pDC->GetDeviceCaps(LOGPIXELSY),72),strTime);
	pDC->SelectObject(oldfont);
	font.DeleteObject();

	font.CreateFont(-MulDiv(12,-pDC->GetDeviceCaps(LOGPIXELSY),72),
		0,0,0,FW_BOLD,0,0,0,GB2312_CHARSET,
		OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,
		VARIABLE_PITCH|FF_SWISS,_T("宋体"));
	oldfont = pDC->SelectObject(&font);
	strTime.Empty();
	strTime.Format("%s",gpcdef.GetCurrentUsername());
	x_Size = pDC->GetTextExtent("调度员姓名:  ");
	pDC->TextOut(tempRect.right - size_box.cx - 90 + x_Size.cx ,bottom_line -MulDiv(21,-pDC->GetDeviceCaps(LOGPIXELSY),72),strTime);
	pDC->SelectObject(oldfont);
	font.DeleteObject();


	font.CreateFont(-MulDiv(12,-pDC->GetDeviceCaps(LOGPIXELSY),72),
		0,0,0,FW_BOLD,0,0,0,GB2312_CHARSET,
		OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,
		VARIABLE_PITCH|FF_SWISS,_T("黑体"));
	oldfont = pDC->SelectObject(&font);
	strDate.Format("内  容: ");
	DrawRect.top  = DrawRect.top - MulDiv(12,-pDC->GetDeviceCaps(LOGPIXELSY),72);
	tempRect=DrawRect;
	tempRect.InflateRect(0,-5,0,0);
	bottom_line = tempRect.top;
	pDC->TextOut(tempRect.left,bottom_line - MulDiv(21,-pDC->GetDeviceCaps(LOGPIXELSY),72),strDate);
	pDC->SelectObject(oldfont);
	font.DeleteObject();

	tempRect.top = bottom_line - 3 * MulDiv(12,-pDC->GetDeviceCaps(LOGPIXELSY),72);
	CFont font_detail;
	font_detail.CreateFont(-MulDiv(12,-pDC->GetDeviceCaps(LOGPIXELSY),72),
		0,0,0,FW_BOLD,0,0,0,GB2312_CHARSET,
		OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,
		VARIABLE_PITCH|FF_SWISS,_T("宋体"));

	oldfont = pDC->SelectObject(&font_detail);
	char *content;
	content = new char[print_content.GetLength() + 10];
	int i;
	if(content != NULL)
	{
		for(i = 0;i < print_content.GetLength();i++)
		{
			content[i] = print_content.GetAt(i);
		}
		content[i] = 0x00;
		DrawTextInRect(pDC,content,&tempRect,1,1,0);
	}
	pDC->SelectObject(oldfont);
	pDC->SelectObject(pOldPen);
	if(content != NULL)
	{
		delete []content;
	}
}

void CPageNotepad::OnButton2() 
{
	UpdateData(TRUE);
	long time;
	time = OsConvertStringtoTime((char *)(LPCTSTR)m_cfTime);
	if(time ==  -1)
	{
		if(select_shift_id % 2 ==  0)
		{
			m_cfTime.Format("7:45");
		}
		else
		{
			m_cfTime.Format("19:45");
		}
	}

	CString note_content;
	CString tempStr;
	CString content;

	GetGridData(note_content);
	m_edit_jiaoban.GetWindowText(content);
	tempStr.Format("%s||%s||%s||%s",m_notebook,m_cfTime,content,note_content);
	strcpy(m_pDoc->m_cPhaseNotes.note,tempStr);

	Set_Notebook();
}

void CPageNotepad::initialGridCtrl()
{
	int col;
	m_pGridCtrl = new CGridCtrl;
	if(NULL == m_pGridCtrl) 
		return;

	CWnd * pWnd;
	CRect rect2;
	CRect rect;
	pWnd= GetDlgItem(IDC_GRID_CTRL);
	pWnd->GetClientRect(rect2);
	pWnd->GetClientRect(rect);
	pWnd->MapWindowPoints(this,rect);
	m_pGridCtrl->Create (rect,this,IDC_GRID_CTRL);
	try
	{
		m_pGridCtrl->SetFixedRowCount(1);
		m_pGridCtrl->SetColumnCount(4);           
		m_pGridCtrl->SetRowCount(5);
		m_pGridCtrl->SetFixedTextColor(CLR_BLUE);      //文本颜色
		m_pGridCtrl->SetGridLineColor(RGB(166,202,240));       //线的颜色
		m_pGridCtrl->SetFixedBkColor(GetSysColor(COLOR_3DFACE));

		int width = rect2.Width();			

		for(int i=0; i<4; i++)
		{
			m_pGridCtrl->SetColumnWidth(i,width*g_noteGridWdith[i]);
		}
		//m_pGridCtrl->ExpandColumnsToFit();
	}
	catch(CMemoryException * e)
	{
		e->ReportError();
		e->Delete();
		return ;
	}

	for (col = 0; col < 4; col++)
	{ 
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.nState = GVIS_READONLY|GVIS_FIXEDROW;

		Item.row = 0;
		Item.col = col;
		Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
		Item.strText.Format(m_sBiaoTi[col]);
		m_pGridCtrl->SetItem(&Item);
	}
}

void CPageNotepad::parseNotes()
{
	CString notes = m_pDoc->m_cPhaseNotes.note;
	int pos = 0;
	int numofparse = 0;
	CString tempStr;
	while(pos != -1)
	{
		tempStr = notes;
		pos = notes.Find(_T("||"));
		if(pos != -1)
		{
			notes = notes.Left(pos);
			parsecontent[numofparse] = notes;
			notes = tempStr;
			notes = notes.Right(notes.GetLength() - pos -2);
		}
		else
		{
			parsecontent[numofparse] = notes;
		}
		numofparse ++;
	}

	m_notebook = parsecontent[0];
	m_cfTime = parsecontent[1];

	m_edit_jiaoban.SetWindowText(parsecontent[2]);
	fillGrid(3);
	numofparse = 0;
}

void CPageNotepad::fillGrid(int start_pos)
{
	ASSERT_VALID(m_pGridCtrl);
	if(m_pGridCtrl == NULL)
		return;

	m_pGridCtrl->DeleteNonFixedRows();	
	m_pGridCtrl->SetRowCount(5);

	for(int i = 0;i< 4;i++)
	{
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.nState = GVIS_READONLY;
		Item.row = i+1;  //行
		Item.col = 0;  //列
		Item.crBkClr = CLR_RED;        
		Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
		Item.strText = parsecontent[start_pos + i * 4];
		m_pGridCtrl->SetItem(&Item);            

		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.nState = GVIS_READONLY;
		Item.row = i+1;  //行
		Item.col = 1;  //列
		Item.crBkClr = CLR_RED;        
		Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
		Item.strText = parsecontent[start_pos + i * 4 + 1];
		m_pGridCtrl->SetItem(&Item);            


		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.nState = GVIS_READONLY;
		Item.row = i+1;  //行
		Item.col = 2;  //列
		Item.crBkClr = CLR_RED;        
		Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
		Item.strText= parsecontent[start_pos + i*4 + 2];
		m_pGridCtrl->SetItem(&Item);            

		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.nState = GVIS_READONLY;
		Item.row = i+1;  //行
		Item.col = 3;  //列
		Item.crBkClr = CLR_RED;        
		Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
		Item.strText= parsecontent[start_pos + i*4 + 3];
		m_pGridCtrl->SetItem(&Item);            

	}
}

void CPageNotepad::GetGridData(CString &note_content)
{
	note_content.Empty();
	CGridCellCheck * pCell;
	CString tempStr;
	for ( int row = 0; row < 4; row++)
	{
		for(int col = 0;col < 4;col ++)
		{
			pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(row + 1,col);
			note_content +=  pCell->GetText();
			if(row == 3 && col == 3)
			{
			}
			else
			{
				note_content += _T("||");
			}
		}
	}
}

void CPageNotepad::initStationList()
{
	stationCount = sizeof(nStationList) / sizeof(nStationList[0]);
	gpcdef.GetStationList(m_pDoc->m_nCurrentSectionNo,stationCount,nStationList);
}

BOOL CPageNotepad::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	int   idCtrl = (int) wParam;

	//消息具体内容
	NM_GRIDVIEW *pItem=(NM_GRIDVIEW*)  lParam;

	int nMessageType=pItem->hdr.code;

	if(idCtrl==IDC_GRID_CTRL)
	{  
		if(nMessageType==GVN_BEGINLABELEDIT)
		{
			CGridCellCombo *pCell;

			CStringArray options;

			for(int i=0;i<stationCount;i++)
				options.Add( _T(gpcdef.GetStationName(nStationList[i])));
			options.Add(_T(""));

			if((pItem->iColumn) == 0)
			{
				m_pGridCtrl->SetCellType(pItem->iRow ,0, RUNTIME_CLASS(CGridCellCombo));
				pCell= (CGridCellCombo*) m_pGridCtrl->GetCell(pItem->iRow ,0);
				pCell->SetOptions(options);
				pCell->SetStyle(pCell->GetStyle() | CBS_DROPDOWN);
			}
		}

		if(nMessageType==GVN_ENDLABELEDIT)   //结束编辑消息
		{    
			CString content, content1;
			int tempInt;
			CGridCell *pCell;

			content=m_pGridCtrl->GetItemText(pItem->iRow,pItem->iColumn);


			switch(pItem->iColumn)
			{ 
			case 1: 
			case 2:
			case 3:
				if(content.IsEmpty())
				{
				}
				else
				{
					tempInt = atoi(content);
					content1.Format("%d",tempInt);
					pCell = (CGridCell *)m_pGridCtrl->GetCell(pItem->iRow,pItem->iColumn);
					pCell->SetText(content1);
				}
				break;

			default:
				break;
			}
		}
	}

	if(idCtrl!=IDC_GRID_CTRL) 
	{
		return CPropertyPage::OnNotify(wParam, lParam, pResult);
	}

	return true;
}

//获取打印设备函数
extern BOOL GetPrinterDevice(LPTSTR pszPrinterName, HGLOBAL* phDevNames, HGLOBAL* phDevMode);
void CPageNotepad::PrintNotepadDirectly()
{
	CWinApp* cApp=AfxGetApp();
	ASSERT_VALID(cApp);

	HGLOBAL hDevMode;
	HGLOBAL hDevNames;

	CString csPlotterName;
	csPlotterName.Format("%s",gpcdef.GetPrinterName());
	if (!GetPrinterDevice((char *)(LPCTSTR)csPlotterName, &hDevNames, &hDevMode))
	{
		AfxMessageBox(_T("无法获取指定的打印机设备\n"));
		return;
	}

	CPrintInfo pInfo;
	HDC prnDc;

	pInfo.m_pPD->m_pd.hDevMode = hDevMode;
	pInfo.m_pPD->m_pd.hDevNames = hDevNames;
	prnDc = pInfo.m_pPD->CreatePrinterDC();

	CDC printDC;
	printDC.Attach(prnDc);

	DOCINFO printDocumentName;
	CString sPrintName;
	sPrintName = "记事打印";

	char sPrintDocName[32];
	memcpy(sPrintDocName,sPrintName,32);

	printDocumentName.cbSize = sizeof(DOCINFO);
	printDocumentName.lpszDocName = sPrintDocName;
	printDocumentName.lpszOutput = NULL;
	printDC.StartDoc(&printDocumentName);
	printDC.StartPage( );

	OnPrint(&printDC,&pInfo);

	printDC.EndPage( );
	printDC.EndDoc( );
	printDC.Detach( );
	DeleteDC(prnDc);

	GlobalFree(hDevNames);
	GlobalFree(hDevMode);
}

void CPageNotepad::OnSetfocusNotebook() 
{
	g_curEdit = (CEdit*)GetDlgItem(IDC_NOTEBOOK);
}

void CPageNotepad::OnSetfocusEditContent() 
{
	g_curEdit =(CEdit*)GetDlgItem(IDC_EDIT_CONTENT);
}

void CPageNotepad::OnSetfocusEditTime()
{
	g_curEdit=NULL;
}
void CPageNotepad::InformParent()
{
	TRAIN_SELECT_PARA para;
	CTG_ScheduleInfo schd;
	m_pDoc->GetScheduleInfo( schd);
	TIME shift_start,shift_end;
	schd.GetShiftManagerRange(shift_start,shift_end); 
	para.starttime= shift_start;
	para.endtime= shift_end;
	para.list_type=1; 
	if(m_pNotifyWindow)
		m_pNotifyWindow->SendMessage(UM_TG_TRAIN_SELECTED,(WPARAM)0,(LPARAM)&para);
}

