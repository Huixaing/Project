// DlgGiveoutDirectionSel.cpp : 实现文件
//

#include "stdafx.h"
#include "Resource.h"
#include "DlgGiveoutDirectionSel.h"
#include "distance.h"
#include ".\dlggiveoutdirectionsel.h"

extern int WriteMessage(PROCESSID pid,ushort usQueueHandle,
				 ushort usCount,char *pbBuffer);
extern int ConvertAbsnumStationToEntry(ushort station,ushort abs_num);

// CDlgGiveoutDirectionSel 对话框
#define COLUMN_COUNT  3

_TCHAR *_gszTitle[COLUMN_COUNT] =
{
	_T("序号"), _T("交出车站"),_T("选择") 
};


IMPLEMENT_DYNAMIC(CDlgGiveoutDirectionSel, CDialog)
CDlgGiveoutDirectionSel::CDlgGiveoutDirectionSel(CWintgDoc *pDoc, CSingleTrain *single,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgGiveoutDirectionSel::IDD, pParent)
{
	m_pDoc = pDoc;
	m_nTrain = single;
	m_nCount = 0;
	m_pGridCtrl = NULL;
}

CDlgGiveoutDirectionSel::~CDlgGiveoutDirectionSel()
{
}

void CDlgGiveoutDirectionSel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgGiveoutDirectionSel, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgGiveoutDirectionSel 消息处理程序
BOOL CDlgGiveoutDirectionSel::OnInitDialog() 
{
	CDialog::OnInitDialog();
	 
	initialGridCtrl();
	fillGrid();
	nSelExit= -1;
	InitDlgPosition(this);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CDlgGiveoutDirectionSel::initialGridCtrl()
{
	int col;
	m_pGridCtrl= new CGridCtrl;
	if(!m_pGridCtrl) 
		return ;
	
	CRect rect2;
	CRect rect;
	CWnd *pWnd;
	pWnd= GetDlgItem(IDC_GRID2);
	if(pWnd == NULL) return;
	pWnd->GetClientRect(rect);
	pWnd->MapWindowPoints(this,rect);

	m_pGridCtrl->Create(rect,this,IDC_GRID2);
	try{
		m_pGridCtrl->SetFixedRowCount(1);
		m_pGridCtrl->SetFixedColumnCount(1);
		m_pGridCtrl->SetColumnCount(COLUMN_COUNT);           
		m_pGridCtrl->SetRowCount(1);
		m_pGridCtrl->SetFixedTextColor(CLR_BLUE);      //文本颜色
		m_pGridCtrl->SetGridLineColor(RGB(166,202,240));       //线的颜色
       	m_pGridCtrl->SetFixedBkColor(GetSysColor(COLOR_3DFACE));
		m_pGridCtrl->SetFixedColumnSelection(TRUE);
		m_pGridCtrl->SetFixedRowSelection(TRUE);

	}
	catch(CMemoryException * e)
	{
		e->ReportError();
		e->Delete();
		return ;
	}
	
    for (col = 0; col < COLUMN_COUNT; col++)
	{ 
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		if(col == 0)
		{
			Item.nState = GVIS_READONLY|GVIS_FIXEDROW|GVIS_FIXEDCOL;
		}
		else
		{
			Item.nState = GVIS_READONLY|GVIS_FIXEDROW;
		}
		Item.row = 0;
		Item.col = col;
		Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
		Item.strText.Format(_gszTitle[col]);
		m_pGridCtrl->SetItem(&Item);
		
	}
	m_pGridCtrl->AutoSizeColumns();
}

void CDlgGiveoutDirectionSel::fillGrid()
{

	////	_T("序号"),_T("方向"),_T("交出车站"),_T("选择") 
 
	ASSERT_VALID(m_pGridCtrl);
	m_pGridCtrl->DeleteNonFixedRows();
 
	Pstruct TRAINRECORD *rec = NULL;
	if(m_pDoc->m_sFocusTrain == NULL) return;
	rec = m_nTrain ->GetIndexRecord (m_nTrain->GetRecordNum () -1);
	if(rec == NULL) return;
	Pstruct _node_info *node_info_current;
	 
	int abs_line_count = 0; int abs_line[64];int i;  
    int node_index_current = m_pDoc->m_nCurrentNodeIndex;
	node_info_current = &(*m_pDoc->m_sDispDistance)[node_index_current].distance.node_info;

	//根据列车方向,符合同方向的优先放在前面
	if(m_nTrain->m_nDirectFlag == DOWN_DIRECT || m_nTrain->m_nDirectFlag == DUAL_DIRECT)
	{
		for( i=0; i<node_info_current->down_line_count; i++)
			abs_line[abs_line_count ++] = node_info_current->down_line_list [i];


		for(i=0; i<node_info_current->up_line_count; i++)
			abs_line[abs_line_count ++] = node_info_current->up_line_list [i];
	}

	else ////if(m_nTrain->m_nDirectFlag == UP_DIRECT || m_nTrain->m_nDirectFlag == DUAL_DIRECT)
	{
		for(i=0; i<node_info_current->up_line_count; i++)
			abs_line[abs_line_count ++] = node_info_current->up_line_list [i];

		for( i=0; i<node_info_current->down_line_count; i++)
			abs_line[abs_line_count ++] = node_info_current->down_line_list [i];
 
	}


 
	 
	//在所有线号中查找当前找到的线号名称
	CString sStationStart,sStationEnd;
     abs_suited = 0;
	for( i=0;i<abs_line_count;i++)
	{
		for(int j= 0;j< m_pDoc->m_nAbsLineCount ;j++)  
		{
			 
			if(m_pDoc->abs_line_info[j].nLineNo == abs_line[i] 
			  && (  (m_pDoc->abs_line_info[j].nStartStation == rec->station && m_pDoc->abs_line_info[j].nDirect == m_nTrain->m_nDirectFlag  ) || (( m_pDoc->abs_line_info[j].nStartStation == rec->station || m_pDoc->abs_line_info[j].nEndStation == rec->station) && m_pDoc->abs_line_info[j].nDirect ==3 ) )//双向的始发车站可能为nEndStation
				  )
			{

				if(m_pDoc->abs_line_info[j].nStartStation>0)
					sStationStart.Format("%s",gpcdef.GetStationName(m_pDoc->abs_line_info[j].nStartStation ));
				else
					sStationStart ="管外";
				if(m_pDoc->abs_line_info[j].nEndStation>0)
					sStationEnd.Format("%s",gpcdef.GetStationName(m_pDoc->abs_line_info[j].nEndStation ));
				else
					sStationEnd ="管外";
               
				if( m_pDoc->abs_line_info[j].nStartStation == rec->station)
				  sLineName[abs_suited].Format("%s(%d) %s－->%s",m_pDoc->abs_line_info[j].sLineName,  abs_line[i],sStationStart,sStationEnd );
				else
                  sLineName[abs_suited].Format("%s(%d) %s<-－>%s",m_pDoc->abs_line_info[j].sLineName,  abs_line[i],sStationEnd,sStationStart );
				abs_line_suited[abs_suited] = abs_line[i] ;
				exit_list[abs_suited] = ConvertAbsnumStationToEntry(rec->station,  abs_line[i]);
				abs_suited++;
			}
		}
		//m_combo_next_abs.AddString(str);

	}

	m_pGridCtrl->SetRowCount(abs_suited + 1);  

	CGridCellCheck *pCell;
	for(  i = 0; i< abs_suited;i++)
	{		 

		//0 序号
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.nState = GVIS_READONLY;
		Item.row = i+1;  //行
		Item.col = 0;  //列
		Item.crBkClr = CLR_RED;        
		Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
		Item.strText.Format("%d",i+1);
		m_pGridCtrl->SetItem(&Item);            

		//1 交出车站
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.nState = GVIS_READONLY;
		Item.row = i+1;  //行
		Item.col = 1;  //列
		Item.crBkClr = CLR_RED;        
		Item.nFormat = DT_LEFT|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
		Item.strText.Format("%s", sLineName[i] );

		//m_pGridCtrl->SetCellType(count,1,RUNTIME_CLASS(CUnEditAbleGridCell));
		m_pGridCtrl->SetItem(&Item);            

		//2选择
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.nState = GVIS_READONLY;
		Item.row = i+1;  //行
		Item.col = 2;  //列
		Item.crBkClr = CLR_RED;        
		Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
		Item.strText.Empty();
		m_pGridCtrl->SetCellType(i+1,2,RUNTIME_CLASS(CGridCellCheck));
		m_pGridCtrl->SetItem(&Item);            

		 
		pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(i+1,2);
		if(pCell)
		{
			if(i==0)
			{
				pCell->SetCheck(TRUE);

			}	 
			else
				pCell->SetCheck(FALSE);
		}
		 
	}

    m_pGridCtrl->AutoSizeColumns();
}

void CDlgGiveoutDirectionSel::OnBnClickedOk()
{	 
	CGridCellCheck *pCell;
	for(int  i = 0; i< abs_suited;i++)
	{		 
		pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(i+1,2);
		if(pCell) 
		{
			if(pCell->GetCheck())
			{
				nSelExit = exit_list[i];
				break;
			}
		}
	}
   if(i>= abs_suited && abs_suited!=0)
   {
	   AfxMessageBox("请选择交出方向");
	   return;
   }
	OnOK();
}

BOOL CDlgGiveoutDirectionSel::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{

    int   idCtrl = (int) wParam;
	if(idCtrl!=IDC_GRID2 ) 
		return CDialog::OnNotify(wParam, lParam, pResult);
	//消息具体内容

    NM_GRIDVIEW *pItem=(NM_GRIDVIEW*)  lParam;;
    int nMessageType=pItem->hdr.code;
    int nCol;CGridCellCheck *pCell;
	if(idCtrl == IDC_GRID2)
	{  
		 
		if(nMessageType==LVN_ITEMCHANGED)   //单击选中消息
		{
			int nRow = m_pGridCtrl->GetFocusCell().row;
			nCol = m_pGridCtrl->GetFocusCell().col;
			pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(nRow,2);
			if(pCell) 
			{

				if(!pCell->GetCheck())
				{ 
					pCell->SetCheck(TRUE);
		
					for(int  i = 0; i< abs_suited;i++)
					{ 
						if(nRow == (i+1) ) continue;
						pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(i+1,2);
						if(pCell) 
						{ 
							pCell->SetCheck(FALSE);
						}

					}


				}

			}
		}

	}
   return CDialog::OnNotify(wParam, lParam, pResult);
}


















