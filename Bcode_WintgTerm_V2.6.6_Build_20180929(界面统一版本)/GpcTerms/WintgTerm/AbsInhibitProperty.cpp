// AbsInhibitProperty.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "AbsInhibitProperty.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAbsInhibitProperty dialog


CAbsInhibitProperty::CAbsInhibitProperty(CWintgDoc *pDoc,CAbsMark * pFigure,CWnd* pParent /*=NULL*/)
	: CDialog(CAbsInhibitProperty::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAbsInhibitProperty)
	m_nDistance = 0;
	m_e_meter = 0;
	m_s_meter = 0;
	m_e_kilo = 0;
	//}}AFX_DATA_INIT
	m_pFigure = pFigure;
	m_pDoc = pDoc;
	abs_line_count = 0;
	m_pcheckBoxList = NULL;
	m_linecount = 0; // 20081227 qiaoyt
}

CAbsInhibitProperty::~CAbsInhibitProperty()
{
	if(m_pcheckBoxList != NULL)
	{
		delete []m_pcheckBoxList;
		m_pcheckBoxList = NULL;
	}
}
void CAbsInhibitProperty::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAbsInhibitProperty)
	DDX_Control(pDX, IDC_LIST2, m_ctrlInhibitAbsLine);
	DDX_Control(pDX, IDC_LIST1, m_ctrlAllAbsLine);
	DDX_Control(pDX, IDC_COMBO_LINEPOS, m_LinePos);
	DDX_Control(pDX, IDC_COMBO_UPDOWNPOS, m_TextPos);
	DDX_Control(pDX, IDC_COMBO2, m_strReason);
	DDX_Control(pDX, IDC_COMBO1, m_nDirect);

	DDX_Text(pDX, IDC_EDIT1, m_nDistance);
	DDV_MinMaxInt(pDX, m_nDistance, 0, 9999);
	DDX_Text(pDX, IDC_E_METER, m_e_meter);
	DDV_MinMaxInt(pDX, m_e_meter, 0, 9999);
	DDX_Text(pDX, IDC_S_METER, m_s_meter);
	DDV_MinMaxInt(pDX, m_s_meter, 0, 9999);
	DDX_Text(pDX, IDC_E_KILO, m_e_kilo);
	DDV_MinMaxInt(pDX, m_e_kilo, 0, 9999);

	DDX_CBString(pDX, IDC_COMBO2, m_strreason);
	DDV_MaxChars(pDX, m_strreason, 128);  //实际上最大为255

	//}}AFX_DATA_MAP
 
 
 
 
}


BEGIN_MESSAGE_MAP(CAbsInhibitProperty, CDialog)
	//{{AFX_MSG_MAP(CAbsInhibitProperty)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DEL, OnDel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAbsInhibitProperty message handlers
#define NUM_DIRECTION  3
#define NUM_REASON     5


_TCHAR *_gszReason[NUM_REASON] = {
    "区间检修",
	"涵洞大修",
	"区间卸料",
	"区间停运",
	"区间作业"
};

BOOL CAbsInhibitProperty::OnInitDialog() 
{
	CDialog::OnInitDialog();
	_TCHAR *_gszDirection[NUM_DIRECTION+1] = {
		"上行",
		"下行",
		"上下行",
		""
	};

/*

    int i,j;
	int nMinIndex,nMaxIndex;
	Pstruct _node_info *node_info_min,*node_info_max; 
	
	nMinIndex = (m_pFigure->rec.station < m_pFigure->rec.index)?m_pFigure->rec.station:m_pFigure->rec.index;
	nMaxIndex = (m_pFigure->rec.station > m_pFigure->rec.index)?m_pFigure->rec.station:m_pFigure->rec.index;

	node_info_min = &(*m_pDoc->m_sDispDistance)[nMinIndex].distance.node_info;
	node_info_max = &(*m_pDoc->m_sDispDistance)[nMaxIndex].distance.node_info;

 	for(i=0;i<node_info_min->down_line_count;i++)
	{
		for(int j= 0;j<node_info_max->up_line_count ;j++)
		{
			if(node_info_min->down_line_list [i] == node_info_max->up_line_list [j])
				abs_line[abs_line_count ++] = node_info_min->down_line_list [i];
		}
	}

	for(i=0;i<node_info_min->up_line_count;i++)
	{
		for(int j= 0;j<node_info_max->down_line_count ;j++)
		{
			if(node_info_min->up_line_list [i] == node_info_max->down_line_list [j])
				abs_line[abs_line_count ++] = node_info_min->up_line_list [i];
		}
	}   
 
	int index = -1; 
	CString str;
	if(abs_line_count >0)//在所有线号中查找当前找到的线号名称
	{
		for(i=0;i<abs_line_count;i++)
		{
			for(int j= 0;j< m_pDoc->m_nAbsLineCount ;j++)
			{
				if(m_pDoc->abs_line_info[j].nLineNo == abs_line[i])
				{
                    str.Format("%s(%d)",m_pDoc->abs_line_info[j].sLineName,abs_line[i]);
 			        break;
				}
			}
 		    m_ctrlAllAbsLine.InsertString(i,str);
		    m_ctrlAllAbsLine.SetItemData (i,(DWORD)abs_line[i]);
			for(int k=0;k<m_pFigure->rec.count;k++){	
				if(abs_line[i] == m_pFigure->rec.ele_number[k]){
 					m_ctrlInhibitAbsLine.InsertString(i,str);
					m_ctrlInhibitAbsLine.SetItemData (i,(DWORD)abs_line[i]);				
				}
			}
		}
	}
*/
	int i;
	for ( i = 0; i< NUM_DIRECTION+1; i++){
        m_nDirect.InsertString(i,_gszDirection[i]);
	}
 
	m_nDirect.SetCurSel((m_pFigure->rec.line&0x0003));
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


    m_nDistance = m_pFigure->rec.position;  
   	m_s_meter = m_pFigure ->rec.s_meter;
   	m_e_kilo = m_pFigure ->rec.e_kilo ;
   	m_e_meter = m_pFigure ->rec.e_meter;

 
 	//m_strReason.SetWindowText (m_pFigure->rec.text);
 
	m_strreason.Format("%s", m_pFigure->rec.text);
	m_strreason.TrimLeft();
	m_strreason.TrimRight();

	if(IsLineFlagLightTrain(m_pFigure ->rec.line))
	{
		SetWindowText("其他");//SetWindowText("轻型车辆");
		CWnd* pwnd = NULL;
		pwnd = GetDlgItem(IDC_EDIT1);
		pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_S_METER);
		pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_E_METER);
		pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_E_KILO);
		pwnd->EnableWindow(FALSE);	
	
	}
	UpdateData(FALSE);
	InitIdentify();
	CreateCheckBox();

	InitDlgPosition(this);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAbsInhibitProperty::OnOK() 
{    
	if(m_pFigure == NULL)
		CDialog::OnOK();

	UpdateData(TRUE);
 	
	//m_pFigure->rec.line = m_nDirect.GetCurSel() ;

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


 	m_pFigure ->rec.position = m_nDistance ;
   	m_pFigure ->rec.s_meter = m_s_meter;
   	m_pFigure ->rec.e_kilo = m_e_kilo;
   	m_pFigure ->rec.e_meter = m_e_meter;

    CString str;
	m_strReason.GetWindowText (str);
	if(str.GetLength() > 254){
	     AfxMessageBox("文字<原因>太长,超出127个汉字，请重新输入!");
		 return;
	}
    strcpy(m_pFigure ->rec.text,(LPCTSTR)str);
// 	 m_strReason.GetLBText(m_strReason.GetCurSel(), m_pFigure->m_strReason);*/

	///////////////////////////////////////////////
	//add by coding fan
	///////////////////////////////////////////////
	int j ;
//	USHORT    ele_number[64];
	USHORT    count = 0;
    int schd_type = m_pFigure->m_nSchdType;


	m_pFigure->rec.count = 0;
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

/*
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
					m_pDoc->abs_line_info[j].nDirect == 3 )
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
					m_pDoc->abs_line_info[j].nDirect == 3 )
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

void CAbsInhibitProperty::InitIdentify()
{
	int schd_type = m_pFigure->m_nSchdType;
//	if((gpcdef.GetUserIdentify() == GPC_DISPATCHERS) || 
//		(gpcdef.GetUserIdentify() == GPC_SCHEDULE)||
 if(  !gpcdef.GetOperatorRight( DRAW_MARK)
		||((schd_type == HISTORY_SCHEDULE) && m_pDoc->m_bHistUnChange))
	{
		CWnd *pWnd;
		pWnd = GetDlgItem(IDC_COMBO1);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_EDIT1);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_COMBO2);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_COMBO_LINEPOS);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_COMBO_UPDOWNPOS);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_E_KILO);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_S_METER);
		pWnd->EnableWindow(FALSE);
		

		pWnd = GetDlgItem(IDC_E_METER);
		pWnd->EnableWindow(FALSE);

		
		
		pWnd = GetDlgItem(IDOK);
		pWnd->EnableWindow(FALSE);

	}
}

void CAbsInhibitProperty::OnAdd() 
{
	// TODO: Add your control notification handler code here
/*	int m_nCurAllSel = -1;
	CString str;
	int line = -1;
	m_nCurAllSel = m_ctrlAllAbsLine.GetCurSel();
	if(m_nCurAllSel < 0) return;
    line = m_ctrlAllAbsLine.GetItemData(m_nCurAllSel);

    m_ctrlAllAbsLine.GetText(m_nCurAllSel,str);
    m_ctrlInhibitAbsLine.InsertString(0,str);
	m_ctrlInhibitAbsLine.SetItemData(0,line);
*/	
}

void CAbsInhibitProperty::OnDel() 
{
	// TODO: Add your control notification handler code here
/*	int m_nCurSel = -1;
	m_nCurSel = m_ctrlInhibitAbsLine.GetCurSel();
	if(m_nCurSel >= 0)
        m_ctrlInhibitAbsLine.DeleteString(m_nCurSel);
*/     
}

//动态创建checkbox
void CAbsInhibitProperty::CreateCheckBox()
{
	int countofbox = 0;
	int i = 0;		
	int schd_type = m_pFigure->m_nSchdType;

	//added by qianguorong 2003-07-29
	m_linecount = 0;
	memset(m_line_list,0,sizeof(m_line_list));//实际的列出来的线号
	Pstruct _node_info *node_info_next;	
	int j=0;
	//added by qianguorong 2003-07-29 ends here

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
		node_info_next = &(*m_pDoc->m_sDispHistDistance)[j].distance.node_info;
	}
	else
	{
		node_info = &(*m_pDoc->m_sDispDistance)[i].distance.node_info;
		node_info_next = &(*m_pDoc->m_sDispDistance)[j].distance.node_info;
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
  if(!gpcdef.GetOperatorRight(DRAW_MARK)||
		((schd_type == HISTORY_SCHEDULE) && m_pDoc->m_bHistUnChange))
	{
		for(int j=0; j<countofbox; j++)
		{
			m_pcheckBoxList[j].EnableWindow(FALSE);
		}
	}
		
	//end of cf 
}

BOOL CAbsInhibitProperty::GetLineName(int lineNo, CString &lineName)
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
