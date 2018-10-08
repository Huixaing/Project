// AbsSlowPropertyEx.cpp : implementation file
//

#include "stdafx.h"
#include "AbsSlowPropertyEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAbsSlowPropertyEx dialog

CAbsSlowPropertyEx::CAbsSlowPropertyEx(CWintgDoc *pDoc,CAbsMark * pFigure,CWnd* pParent /*=NULL*/)
	: CDialog(CAbsSlowPropertyEx::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAbsSlowPropertyEx)
	m_nDistance = 0;
	m_nSpeed = 0;
	m_s_meter = 0;
	m_e_kilo = 0;
	m_e_meter = 0;
	m_append_time = 0;
	m_pFigure = pFigure;
	//}}AFX_DATA_INIT
	m_pDoc = pDoc;
	m_pcheckBoxList = NULL;
	m_linecount = 0; // 20081226
}

CAbsSlowPropertyEx::~CAbsSlowPropertyEx()
{
	if(m_pcheckBoxList != NULL)
	{
		delete []m_pcheckBoxList;
		m_pcheckBoxList = NULL;
	}

}
void CAbsSlowPropertyEx::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAbsSlowPropertyEx)
	DDX_Control(pDX, IDC_COMBO_UPDOWNPOS, m_TextPos);
	DDX_Control(pDX, IDC_COMBO_LINEPOS, m_LinePos);
	DDX_Control(pDX, IDC_COMBO3, m_nBlockType);
	DDX_Control(pDX, IDC_COMBO_REASON, m_strReason);
	DDX_CBString(pDX, IDC_COMBO_REASON, m_strreason);
	DDV_MaxChars(pDX, m_strreason, 128);  //实际上最大为255

	DDX_Control(pDX, IDC_COMBO1, m_nDirect);
	DDX_Text(pDX, IDC_EDIT1, m_nDistance);
	DDV_MinMaxInt(pDX, m_nDistance, 0, 9999);
	DDX_Text(pDX, IDC_EDIT2, m_nSpeed);
	DDV_MinMaxInt(pDX, m_nSpeed, 0, 9999);
	DDX_Text(pDX, IDC_S_METER, m_s_meter);
	DDV_MinMaxInt(pDX, m_s_meter, 0, 9999);
	DDX_Text(pDX, IDC_E_KILO, m_e_kilo);
	DDV_MinMaxInt(pDX, m_e_kilo, 0, 9999);
	DDX_Text(pDX, IDC_E_METER, m_e_meter);
	DDV_MinMaxInt(pDX, m_e_meter, 0, 9999);
	DDX_Text(pDX, IDC_EDIT_TIME, m_append_time);
	DDV_MinMaxInt(pDX, m_append_time, 0, 9999);
	//}}AFX_DATA_MAP
 
 
	 
 
	 
}


BEGIN_MESSAGE_MAP(CAbsSlowPropertyEx, CDialog)
	//{{AFX_MSG_MAP(CAbsSlowPropertyEx)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAbsSlowPropertyEx message handlers
#define NUM_DIRECTION  3
#define NUM_REASON     5
#define NUM_BLOCK      3


BOOL CAbsSlowPropertyEx::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	_TCHAR *_gszDirection[NUM_DIRECTION+1] = {
		"上行",
		"下行",
		"上下行",
		""
	};
	_TCHAR *_gszReason[NUM_REASON] = {
		"大修",
		"更换磁头",
		"区间卸料",
		"区间停运",
		"区间作业"
	};

	_TCHAR *_gszBlockType[NUM_BLOCK] = {
		"半自动闭塞",
		"自动闭塞",
		"电话闭塞"
	};
	int i;
	for ( i = 0; i< NUM_DIRECTION+1; i++){
        m_nDirect.InsertString(i,_gszDirection[i]);
	}
	/*
	for ( i = 0; i< NUM_REASON; i++){
	    m_strReason.InsertString(i,_gszReason[i]);
	}
	*/
	//读取数据绘制原因
	CString datadir = gpcdef.GetDataDir();
	CStdioFile f_words;
	
	if(f_words.Open(datadir+"\\DDML\\DrawingWords.cfg",CFile::modeCreate|CFile::modeNoTruncate|CFile::modeRead))
	{
		CString linetext;
		while(f_words.ReadString(linetext))
		{
			m_strReason.InsertString(-1,linetext);
		}
		f_words.Close();
	}

	for ( i = 0; i< NUM_BLOCK; i++){
	    m_nBlockType.InsertString(i,_gszBlockType[i]);
	}

 	m_strReason.SetWindowText (m_pFigure->rec.text);
    m_append_time = m_pFigure ->rec.slow_time/60;
//	m_nDirect.SetCurSel( m_pFigure ->rec.line);

	m_nDirect.SetCurSel((m_pFigure->rec.line&0x0003));
	
	_TCHAR *_gszTextPos[NUM_DIRECTION] = {
		"上",
		"中",
		"下"
	};
	for ( i = 0; i< NUM_DIRECTION; i++){
        m_LinePos.InsertString(i,_gszTextPos[i]);
	}
	m_LinePos.SetCurSel((m_pFigure->rec.line&0x0300)>>8);

	m_TextPos.AddString("上");
	m_TextPos.AddString("下");

	m_TextPos.SetCurSel((m_pFigure->rec.line&0x0400)>>10);

	m_strreason.Format("%s", m_pFigure->rec.text);
	m_strreason.TrimLeft();
	m_strreason.TrimRight();

    m_nDistance =  m_pFigure ->rec.position;  
    m_nSpeed =  m_pFigure ->rec.flag;
   	m_s_meter = m_pFigure ->rec.s_meter;
   	m_e_kilo = m_pFigure ->rec.e_kilo ;
   	m_e_meter = m_pFigure ->rec.e_meter;
	

	if(m_pFigure->rec.type == SELF_CLOSE_MARK)
	{
		m_nBlockType.SetCurSel((int)m_pFigure->rec.flag);
	}
	else
	{
		m_nBlockType.SetWindowText("");
	}
	
	UpdateData(FALSE);	

	if(m_pFigure)
	{
		if(m_pFigure->rec.type == SELF_CLOSE_MARK)
		{
			CWnd *pWnd;
			pWnd = GetDlgItem(IDC_EDIT2);
			pWnd->EnableWindow(FALSE);

			pWnd = GetDlgItem(IDC_EDIT1);
			pWnd->EnableWindow(FALSE);

			pWnd = GetDlgItem(IDC_S_METER);
			pWnd->EnableWindow(FALSE);

			pWnd = GetDlgItem(IDC_E_KILO);
			pWnd->EnableWindow(FALSE);
			
			pWnd = GetDlgItem(IDC_E_METER);
			pWnd->EnableWindow(FALSE);
			SetWindowText("更改闭塞方式");
		}
		else
		{
			m_nBlockType.EnableWindow(FALSE);
		}
	}

	this->InitIdentify();

	
	CreateCheckBox();
	InitDlgPosition(this);


	return TRUE;  // return TRUE unless you set the focus to a control
}

void CAbsSlowPropertyEx::OnOK() 
{
	if(m_pFigure == NULL)
		CDialog::OnOK();;
	UpdateData(TRUE);
  	//m_pFigure ->rec.line = m_nDirect.GetCurSel();

		int selIndex;
	selIndex = m_nDirect.GetCurSel();
	if(selIndex == 0)//封锁方向 	0"上行", 1"下行", 2"上下行"
		SetLineDirectionUp(m_pFigure ->rec.line);
	else if(selIndex == 1)
		SetLineDirectionDown(m_pFigure ->rec.line);
	else if(selIndex == 2)
		SetLineDirectionDual(m_pFigure ->rec.line);
	else
		SetLineDirectionNone(m_pFigure ->rec.line);

	
	selIndex = m_LinePos.GetCurSel();
	if(selIndex == 0)//中间线的位置 	0"上",1"中",2"下"
		SetLineLinePosTop(m_pFigure ->rec.line);
	else if(selIndex == 2)
		SetLineLinePosBottom(m_pFigure ->rec.line);
	else 
		SetLineLinePosMiddle(m_pFigure ->rec.line);

	selIndex = m_TextPos.GetCurSel();
	if(selIndex == 1)//文字的位置 	0"上",1"下"
		SetLineTextPosBottom(m_pFigure ->rec.line);
	else
		SetLineTextPosTop(m_pFigure ->rec.line);

  	m_pFigure->rec.position = m_nDistance ;
    m_pFigure ->rec.s_meter = m_s_meter;
   	m_pFigure ->rec.e_kilo = m_e_kilo;
   	m_pFigure ->rec.e_meter = m_e_meter;
    m_pFigure ->rec.slow_time = m_append_time*60;


	if(m_pFigure->rec.type == SELF_CLOSE_MARK)
	{
		m_pFigure->rec.flag = m_nBlockType.GetCurSel();
	}
	else
	{
		m_pFigure->rec.flag = m_nSpeed ;
	}

    CString str;
	m_strReason.GetWindowText (str);
	if(str.GetLength() >  254){
	     AfxMessageBox("文字<原因>太长,超出127个汉字,请重新输入!");
		 return;
	}
    strcpy(m_pFigure ->rec.text,(LPCTSTR)str);


	int j ;
//	USHORT    ele_number[64];
	USHORT    count = 0;
    int schd_type = m_pFigure->m_nSchdType;

//	Pstruct _node_info *node_info;

	m_pFigure->rec.count = 0;
	//added by qianguorong 2003-07-29
	/*				
	if(schd_type == WORK_SCHEDULE)
		node_info = &(*m_pDoc->m_sDispDistance)[m_pFigure->rec.station].distance.node_info;
	else if(schd_type == BASIC_SCHEDULE)
		node_info = &(*m_pDoc->m_sDispBasicDistance)[m_pFigure->rec.station].distance.node_info;
	else if(schd_type == HISTORY_SCHEDULE)
		node_info = &(*m_pDoc->m_sDispHistDistance)[m_pFigure->rec.station].distance.node_info;
	else
		node_info = &(*m_pDoc->m_sDispDistance)[m_pFigure->rec.station].distance.node_info;
	
	for(j=0; j<node_info->down_line_count; j++)
	{
		if(m_pcheckBoxList[j].GetCheck() == 1)
		{
			m_pFigure->rec.ele_number[m_pFigure->rec.count] = node_info->down_line_list[j];
			m_pFigure->rec.count ++;
		}
		
	}
	*/
	for(j=0; j<m_linecount; j++)
	{
		if(m_pcheckBoxList[j].GetCheck() == 1)
		{
			m_pFigure->rec.ele_number[m_pFigure->rec.count] = m_line_list[j];
			m_pFigure->rec.count ++;
		}
		
	}
	//added by qianguorong 2003-07-29 ends here

	///////////////////////////////////////////////
	//add by coding fan
	///////////////////////////////////////////////
/*	int i,j ;
	USHORT    ele_number[64];
	USHORT    count = 0;
    int schd_type = m_pFigure->m_nSchdType;

	do
	{
		{
			Pstruct _node_info *node_info;
			m_pFigure->rec.count = 0;
			for(int i=m_pFigure->rec.station;i<m_pFigure->rec.index;i++){
				if(schd_type == WORK_SCHEDULE)
					node_info = &(*m_pDoc->m_sDispDistance)[i].distance.node_info;
				else if(schd_type == BASIC_SCHEDULE)
					node_info = &(*m_pDoc->m_sDispBasicDistance)[i].distance.node_info;
				else if(schd_type == HISTORY_SCHEDULE)
					node_info = &(*m_pDoc->m_sDispHistDistance)[i].distance.node_info;
				else
					break;
				
				for(int j= 0;j <  node_info->down_line_count;j++){
					m_pFigure->rec.ele_number[m_pFigure->rec.count] = node_info->down_line_list[j];
					m_pFigure->rec.count ++;
				}
			}
		}
		
		
		
		if(m_pFigure->rec.line == 0)
		{
			for(i = 0;i<m_pFigure->rec.count;i++)
			{
				j=0;
				while(j<m_pDoc->m_nAbsLineCount) 
				{
					if(m_pDoc->abs_line_info[j].nLineNo == m_pFigure->rec.ele_number[i])
						break;
					j++;
				}
				if(j>=m_pDoc->m_nAbsLineCount)
				{
					break;
				}
				if(m_pDoc->abs_line_info[j].nDirect == 0 ||
					m_pDoc->abs_line_info[j].nDirect == 3)
				{
					ele_number[count++] = m_pFigure->rec.ele_number[i];
				}
			}
			m_pFigure->rec.count = count;
			for(i=0;i<count;i++)
			{
				m_pFigure->rec.ele_number[i] = ele_number[i];
			}
		}
		
		
		if(m_pFigure->rec.line == 1)
		{
			for(i = 0;i<m_pFigure->rec.count;i++)
			{
				j=0;
				while(j<m_pDoc->m_nAbsLineCount) 
				{
					if(m_pDoc->abs_line_info[j].nLineNo == m_pFigure->rec.ele_number[i])
						break;
					j++;
				}
				if(j>=m_pDoc->m_nAbsLineCount)
				{
					break;
				}
				if(m_pDoc->abs_line_info[j].nDirect == 1 ||
					m_pDoc->abs_line_info[j].nDirect == 3)
				{
					ele_number[count++] = m_pFigure->rec.ele_number[i];
				}
			}
			m_pFigure->rec.count = count;
			for(i=0;i<count;i++)
			{
				m_pFigure->rec.ele_number[i] = ele_number[i];
			}
		}
	}while(0);
	//////end of coding fan
*/
	

	CDialog::OnOK();
}

void CAbsSlowPropertyEx::InitIdentify()
{
	int schd_type = m_pFigure->m_nSchdType;
//	if((gpcdef.GetUserIdentify() == GPC_DISPATCHERS) ||
//		(gpcdef.GetUserIdentify() == GPC_SCHEDULE)||
   if(!gpcdef.GetOperatorRight( DRAW_MARK)||
		((schd_type == HISTORY_SCHEDULE) && m_pDoc->m_bHistUnChange))
	{
		CWnd *pWnd;
		pWnd = GetDlgItem(IDC_COMBO1);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_COMBO3);
		pWnd->EnableWindow(FALSE);
		

		pWnd = GetDlgItem(IDC_EDIT1);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_EDIT2);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_COMBO_REASON);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_COMBO_LINEPOS);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_COMBO_UPDOWNPOS);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_EDIT_TIME);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_S_METER);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_E_KILO);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_E_METER);
		pWnd->EnableWindow(FALSE);

		
		pWnd = GetDlgItem(IDOK);
		pWnd->EnableWindow(FALSE);

	}

}


//动态创建checkbox
void CAbsSlowPropertyEx::CreateCheckBox()
{

	//added by qianguorong 2003-07-29
	m_linecount = 0;
	memset(m_line_list,0,sizeof(m_line_list));//实际的列出来的线号
	Pstruct _node_info *node_info_next;	
	int j=0;
	//added by qianguorong 2003-07-29 ends here

	int countofbox = 0;
	int i = 0;		
	int schd_type = m_pFigure->m_nSchdType;

	//计算总的线数
	countofbox = 0;
	Pstruct _node_info *node_info;	
	//for (i=m_pFigure->rec.station; i<m_pFigure->rec.index; i++)
	//{

	i = m_pFigure->rec.station;
	//added by qianguorong 2003-07-29
	j = m_pFigure->rec.index; 
	
	if (schd_type == WORK_SCHEDULE)
	{
		node_info = &(*m_pDoc->m_sDispDistance)[i].distance.node_info;
		node_info_next = &(*m_pDoc->m_sDispDistance)[j].distance.node_info;
	}
	else if(schd_type == BASIC_SCHEDULE)
	{
		node_info = &(*m_pDoc->m_sDispBasicDistance)[i].distance.node_info;
		node_info_next = &(*m_pDoc->m_sDispBasicDistance)[j].distance.node_info;
	}
	else if(schd_type == HISTORY_SCHEDULE)
	{
		node_info = &(*m_pDoc->m_sDispHistDistance)[i].distance.node_info;
		node_info = &(*m_pDoc->m_sDispHistDistance)[j].distance.node_info;
	}
	else
	{
		node_info = &(*m_pDoc->m_sDispDistance)[i].distance.node_info;
		node_info = &(*m_pDoc->m_sDispDistance)[j].distance.node_info;
	}

	for(int ii=0; ii<node_info->down_line_count; ii++)
	{
		for(int jj=0; jj<node_info_next->up_line_count; jj++)
		{
			if(node_info->down_line_list[ii] == node_info_next->up_line_list[jj])
			{
				m_line_list[m_linecount++] = node_info->down_line_list[ii];
				break;
			}
		}
	}
	
	
	
	//countofbox += node_info->down_line_count;
	countofbox = m_linecount;
	//added by qianguorong 2003-07-29 ends here	
	//}
	
	CRect buttonRect;
	long  left,right,bottom,top;
	int row;
	long width,height;
	CString str;

	CWnd *boxWnd = GetDlgItem(IDC_STATIC_LINEBOX);
	
	boxWnd->GetClientRect(&buttonRect);
	boxWnd->MapWindowPoints(this,&buttonRect);
	
	left = buttonRect.left;
	right = buttonRect.right;
	
	top = buttonRect.top;
	bottom = buttonRect.bottom;


	width = (right - left) / 5;;
	height = (bottom - top - 2*HEIGHTSPACE);
		
	row = 1;
	
	buttonRect.left = left + WIDTHSPACE;
	buttonRect.right = buttonRect.left + width;
	buttonRect.top = top + HEIGHTSPACE;
	buttonRect.bottom = buttonRect.top + height;

	left = buttonRect.left;
	right = buttonRect.right;
	bottom = buttonRect.bottom;
	top = buttonRect.top;

	str.Empty();

	//added by qianguorong 2003-07-29 
	if(countofbox<=0)
		return;
	//added by qianguorong 2003-07-29 ends here
	
	m_pcheckBoxList = new CButton[countofbox];
	
	//node_info = &(*m_pDoc->m_sDispDistance)[m_pFigure->rec.station].distance.node_info;

	for(i=1;i<=countofbox;i++)
	{		
		//added by qianguorong 2003-07-29 
		//GetLineName(node_info->down_line_list[i-1],str);	
		GetLineName(m_line_list[i-1],str);
		//added by qianguorong 2003-07-29 ends here
		m_pcheckBoxList[i-1].Create(str,WS_CHILD | WS_VISIBLE | WS_TABSTOP|BS_AUTOCHECKBOX,buttonRect,this,IDC_CHECKBOX_START + i);
		buttonRect.left = left + ((i) % 5) * (width+WIDTHSPACE);
		buttonRect.right = right + ((i)%5) * (width+WIDTHSPACE);
		buttonRect.top = top + ((i)/5) * (height +HEIGHTSPACE);
		buttonRect.bottom = bottom + ((i)/5) *(height + HEIGHTSPACE);
	}

	int linenumber = 0;
	int elenumber = 0;
	for(i=0; i<m_pFigure->rec.count; i++)
	{
		elenumber = m_pFigure->rec.ele_number[i];
		for(int j=0; j<countofbox; j++)
		{
			//added by qianguorong 2003-07-29 
			//linenumber = node_info->down_line_list[j];
			linenumber = m_line_list[j];
			//added by qianguorong 2003-07-29 ends here
			if(linenumber == elenumber)
			{
				m_pcheckBoxList[j].SetCheck(TRUE);
				break;
			}
		}
	}

//	if((gpcdef.GetUserIdentify() == GPC_DISPATCHERS) || (gpcdef.GetUserIdentify() == GPC_SCHEDULE)||
	if( !gpcdef.GetOperatorRight(DRAW_MARK)||
		((schd_type == HISTORY_SCHEDULE) && m_pDoc->m_bHistUnChange))
	{
		for(int j=0; j<countofbox; j++)
		{
			m_pcheckBoxList[j].EnableWindow(FALSE);
		}
	}
		
	//end of cf 
}

BOOL CAbsSlowPropertyEx::GetLineName(int lineNo, CString &lineName)
{
	lineName = "";
	int j= 0;
	while(j<m_pDoc->m_nAbsLineCount) 
	{
		if(m_pDoc->abs_line_info[j].nLineNo == lineNo)
			break;
		j++;
	}
	if(j>=m_pDoc->m_nAbsLineCount)
		return FALSE;

	lineName = m_pDoc->abs_line_info[j].sLineName;
	return TRUE;
}

