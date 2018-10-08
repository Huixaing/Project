// AbsSlowProperty.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"

//#include "wintg.h"
#include "AbsSlowProperty.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAbsSlowProperty dialog


CAbsSlowProperty::CAbsSlowProperty(CWintgDoc *pDoc,CAbsMark * pFigure,CWnd* pParent /*=NULL*/)
	: CDialog(CAbsSlowProperty::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAbsSlowProperty)
	m_nDistance = 0;
	m_nSpeed = 0;
	m_s_meter = 0;
	m_e_kilo = 0;
	m_e_meter = 0;
	m_append_time = 0;
	m_pFigure = pFigure;
	//}}AFX_DATA_INIT
	m_pDoc = pDoc;
}


void CAbsSlowProperty::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAbsSlowProperty)
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



BEGIN_MESSAGE_MAP(CAbsSlowProperty, CDialog)
	//{{AFX_MSG_MAP(CAbsSlowProperty)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAbsSlowProperty message handlers
#define NUM_DIRECTION  3
#define NUM_REASON     5
#define NUM_BLOCK      3


BOOL CAbsSlowProperty::OnInitDialog() 
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

	for ( i = 0; i< NUM_BLOCK; i++){
	    m_nBlockType.InsertString(i,_gszBlockType[i]);
	}

 	m_strReason.SetWindowText (m_pFigure->rec.text);
    m_append_time = m_pFigure ->rec.slow_time/60;
//	m_nDirect.SetCurSel( m_pFigure ->rec.line);
	m_strreason.Format("%s", m_pFigure->rec.text);
	m_strreason.TrimLeft();
	m_strreason.TrimRight();
	
	
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


	InitDlgPosition(this);
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CAbsSlowProperty::OnOK() 
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
	if(str.GetLength() > 254){
	     AfxMessageBox("文字<原因>太长,请重新输入!");
		 return;
	}
    strcpy(m_pFigure ->rec.text,(LPCTSTR)str);
	///////////////////////////////////////////////
	//add by coding fan
	///////////////////////////////////////////////
	int i,j ;
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

	CDialog::OnOK();
}

void CAbsSlowProperty::InitIdentify()
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
