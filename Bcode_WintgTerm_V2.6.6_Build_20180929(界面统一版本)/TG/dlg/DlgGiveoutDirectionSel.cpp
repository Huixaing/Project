// DlgGiveoutDirectionSel.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgGiveoutDirectionSel.h"
//#include "distance.h"

// CDlgGiveoutDirectionSel 对话框
#define COLUMN_COUNT  3

_TCHAR *_gszTitle[COLUMN_COUNT] =
{
	_T("序号"), _T("车站方向"),_T("选择") 
};


IMPLEMENT_DYNAMIC(CDlgGiveoutDirectionSel, CDialog)
CDlgGiveoutDirectionSel::CDlgGiveoutDirectionSel(CTGDoc *pDoc, CTG_TRAIN *single,CWnd* pParent, int nFlag, int rec_index)
	: CDialog(CDlgGiveoutDirectionSel::IDD, pParent)
{
	m_pDoc = pDoc;
	m_nTrain = single;
	
	m_pGridCtrl = NULL;
	m_nFlag = nFlag;
	m_nFoucsRecIndex = rec_index;
	abs_line_num=0;
	m_station=0;
}

CDlgGiveoutDirectionSel::CDlgGiveoutDirectionSel(CTGDoc *pDoc, CWnd* pParent, int nFlag, std::vector<int>& abs_line_list, int station)
	: CDialog(CDlgGiveoutDirectionSel::IDD, pParent)
{
	m_pDoc = pDoc;

	m_pGridCtrl = NULL;
	m_nFlag = nFlag;
	m_station = station;

	abs_line_num = abs_line_list.size();
	for(int i=0;i<abs_line_num;i++)
	{
		abs_lineno[i] = abs_line_list[i];
	}
}

CDlgGiveoutDirectionSel::CDlgGiveoutDirectionSel(CTGDoc *pDoc, CWnd* pParent, int nFlag, int abs_line_list[], int count, int station)
	: CDialog(CDlgGiveoutDirectionSel::IDD, pParent)
{
	m_pDoc = pDoc;

	m_pGridCtrl = NULL;
	m_nFlag = nFlag;
	m_station = station;

	abs_line_num = count;
	for(int i=0;i<abs_line_num;i++)
	{
		abs_lineno[i] = abs_line_list[i];
	}
}

CDlgGiveoutDirectionSel::~CDlgGiveoutDirectionSel()
{
	if (m_pGridCtrl)
		delete m_pGridCtrl;
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

	// 交出口
	if(m_nFlag == 0) 
	{
		this->SetWindowText("交出方向选择");
		InitExitLineNo();
	}
	// 交出口
	else if(m_nFlag == 4) 
	{
		this->SetWindowText("交出方向选择");
		InitExitLineNo4();
	}
	// 交出口--不过滤
	else if(m_nFlag == 6) 
	{
		this->SetWindowText("交出方向选择");
		InitExitLineNo5();
	}
	// 接入口
	else if(m_nFlag == 1)
	{
		this->SetWindowText("接入方向选择");
		InitEntryLineNo();
	}
	else if(m_nFlag == 5)
	{
		this->SetWindowText("接入方向选择");
		InitEntryLineNo5();
	}
	// 按照给定线号
	else if(m_nFlag == 2)
	{
		this->SetWindowText("停车区间选择");
		InitAbsLineNo();
	}
	// 按照给定线号
	else if(m_nFlag == 3)
	{
		this->SetWindowText("停车区间选择");
		InitAbsLineNo();
	}
	// 按照给定线号
	else if(m_nFlag == 8)
	{
		this->SetWindowText("接入区间选择");
		InitAbsLineNo8();
	}
	else if(m_nFlag == 9)
	{
		this->SetWindowText("交出区间选择");
		InitAbsLineNo8();
	}
	
	nSelDirect= -1;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CDlgGiveoutDirectionSel::initialGridCtrl()
{
	int col;
	if (NULL==m_pGridCtrl)
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

void CDlgGiveoutDirectionSel::InitAbsLineNo8()
{
	//	_T("序号"),_T("方向"),_T("交出车站"),_T("选择") 
	if(m_pGridCtrl == NULL)
		return;
	m_pGridCtrl->DeleteNonFixedRows();
 
	abs_suited = 0;
	// 根据列车方向,符合同方向的优先放在前面
	for(int i=0;i<abs_line_num;i++)
	{
		int nLineNo = abs_lineno[i];
		const ABS_LINE* line = m_pDoc->m_ptgctrl->mConfigTg.m_tgdef.GetLineDefine(nLineNo);
		if(line == NULL)
			continue;
		CString sStationStart,sStationEnd, strLineName=line->sLineName;
		if(line->nStartStation!=0)
			sStationStart.Format("%s",m_pDoc->GetStationName(line->nStartStation) );
		else
			sStationStart ="管外";
		if(line->nEndStation!=0)
			sStationEnd.Format("%s",m_pDoc->GetStationName(line->nEndStation ));
		else
			sStationEnd ="管外";
        
		int entry = m_pDoc->ConvertAbsnumStationToEntry(m_station, nLineNo);
		if(entry <= 0)
		{
			CString msg;
			msg.Format("找不到车站%d对应线号%d的出入口,请检查ENTRY.cfg文件", m_station, nLineNo);
			m_pDoc->sysprint(5, 0, "[TG]%s", msg);
		}
		else
		{
			sLineName[abs_suited].Format("%s--->%s(%d)%s", sStationStart, sStationEnd, nLineNo, strLineName);
			abs_line_suited[abs_suited] = nLineNo;
			exit_list[abs_suited] = entry;
			abs_suited++;
		}
	}

	FillGrid();
}

void CDlgGiveoutDirectionSel::InitAbsLineNo()
{
	//	_T("序号"),_T("方向"),_T("交出车站"),_T("选择") 
	if(m_pGridCtrl == NULL)
		return;
	m_pGridCtrl->DeleteNonFixedRows();
 
	// 根据列车方向,符合同方向的优先放在前面
	std::map<int, int> abs_lines;
	abs_lines.clear();
	for(int i=0;i<abs_line_num;i++)
	{
		abs_lines[i] = abs_lineno[i];
	}

	//在所有线号中查找当前找到的线号名称
	CString sStationStart,sStationEnd;
	abs_suited = 0;
	std::map<int, int>::iterator it = abs_lines.begin();
	for( ; it != abs_lines.end();it++)
	{
		int nLineNo = it->second;
		const ABS_LINE* line = m_pDoc->m_ptgctrl->mConfigTg.m_tgdef.GetLineDefine(nLineNo);
		if(line == NULL)
			continue;
		
		if(line->nStartStation>0)
			sStationStart.Format("%s",m_pDoc->GetStationName(line->nStartStation) );
		else
			sStationStart ="管外";
		if(line->nEndStation>0)
			sStationEnd.Format("%s",m_pDoc->GetStationName(line->nEndStation ));
		else
			sStationEnd ="管外";
        
		int entry = m_pDoc->ConvertAbsnumStationToEntry(m_station, nLineNo);
		if(entry <= 0)
		{
			CString msg;
			msg.Format("找不到车站%d对应线号%d的出入口,请检查ENTRY.cfg文件", m_station, nLineNo);
			m_pDoc->sysprint(5, 0, "[TG]%s", msg);
			AfxMessageBox(msg, MB_ICONHAND);
		}
		else
		{
			sLineName[abs_suited].Format("%s--->%s(%d)", sStationStart, sStationEnd, nLineNo);
			abs_line_suited[abs_suited] = nLineNo;
			exit_list[abs_suited] = entry;
			abs_suited++;
		}
	}

	FillGrid();
}

void CDlgGiveoutDirectionSel::InitExitLineNo5()
{
	//	_T("序号"),_T("方向"),_T("交出车站"),_T("选择") 
	if(m_pGridCtrl == NULL || m_nTrain == NULL)
		return;
	m_pGridCtrl->DeleteNonFixedRows();
 
	TgTRAIN_RECORD *rec = NULL, *pre = NULL;
	rec = m_nTrain->GetRecordByPosition(m_nFoucsRecIndex);
	if(rec == NULL) return;
	pre = m_nTrain->GetRecordByPosition(m_nFoucsRecIndex-1);
	
	//在所有线号中查找当前找到的线号名称
	CString sStationStart,sStationEnd;
    abs_suited = 0;
	for(int idx = 0; idx < exit_lines.size(); idx++)
	{
		int nLineNo = exit_lines[idx];
		const ABS_LINE* line = m_pDoc->m_ptgctrl->mConfigTg.m_tgdef.GetLineDefine(nLineNo);
		if(line==NULL)
			continue;

	
		if(line->nStartStation>0)
			sStationStart.Format("%s",m_pDoc->GetStationName(line->nStartStation) );
		else
			sStationStart ="管外";
		if(line->nEndStation>0)
			sStationEnd.Format("%s",m_pDoc->GetStationName(line->nEndStation ));
		else
			sStationEnd ="管外";
            
		int entry = m_pDoc->ConvertAbsnumStationToEntry(rec->station, nLineNo);
		if(entry <= 0)
		{
			CString msg;
			msg.Format("找不到车站%d对应线号%d的出入口,请检查ENTRY.cfg文件", rec->station, nLineNo);
			m_pDoc->sysprint(5, 0, "[TG]%s", msg);
		}
		else
		{
			sLineName[abs_suited].Format("%s--->%s(%d)", sStationStart, sStationEnd, nLineNo);
			abs_line_suited[abs_suited] = nLineNo;
			exit_list[abs_suited] = entry;
			abs_suited++;
		}
	}

	FillGrid();
}

void CDlgGiveoutDirectionSel::InitExitLineNo()
{
	//	_T("序号"),_T("方向"),_T("交出车站"),_T("选择") 
	if(m_pGridCtrl == NULL || m_nTrain == NULL)
		return;
	m_pGridCtrl->DeleteNonFixedRows();
 
	TgTRAIN_RECORD *rec = NULL, *pre = NULL;
	rec = m_nTrain->GetRecordByPosition(m_nFoucsRecIndex);
	if(rec == NULL) return;
	pre = m_nTrain->GetRecordByPosition(m_nFoucsRecIndex-1);
	
	//在所有线号中查找当前找到的线号名称
	CString sStationStart,sStationEnd;
    abs_suited = 0;
	for(int idx = 0; idx < exit_lines.size(); idx++)
	{
		int nLineNo = exit_lines[idx];
		const ABS_LINE* line = m_pDoc->m_ptgctrl->mConfigTg.m_tgdef.GetLineDefine(nLineNo);
		if(line==NULL)
			continue;

		if(NULL != pre)
		{
			if(line->nStartStation == pre->station || line->nEndStation == pre->station)
				continue;
		}
		if(((line->nStartStation == rec->station || line->nEndStation == rec->station) && line->nDirect == m_nTrain->m_nDirectFlag))
		{
			if(line->nStartStation>0)
				sStationStart.Format("%s",m_pDoc->GetStationName(line->nStartStation) );
			else
				sStationStart ="管外";
			if(line->nEndStation>0)
				sStationEnd.Format("%s",m_pDoc->GetStationName(line->nEndStation ));
			else
				sStationEnd ="管外";
            
			int entry = m_pDoc->ConvertAbsnumStationToEntry(rec->station, nLineNo);
			if(entry <= 0)
			{
				CString msg;
				msg.Format("找不到车站%d对应线号%d的出入口,请检查ENTRY.cfg文件", rec->station, nLineNo);
				m_pDoc->sysprint(5, 0, "[TG]%s", msg);
				//AfxMessageBox(msg, MB_ICONHAND);
			}
			else
			{
				sLineName[abs_suited].Format("%s--->%s(%d)", sStationStart, sStationEnd, nLineNo);
				abs_line_suited[abs_suited] = nLineNo;
				exit_list[abs_suited] = entry;
				abs_suited++;
			}
		}

		if(line->nDirect ==3)
		{
			int beginStaNo, endStaNo;
			if(line->nStartStation == rec->station)
			{
				beginStaNo = line->nStartStation;
				endStaNo   = line->nEndStation;
			}
			else if(line->nEndStation == rec->station)
			{
				endStaNo = line->nStartStation;
				beginStaNo   = line->nEndStation;
			}
			else
			{
				continue;
			}

			if(beginStaNo>0)
				sStationStart.Format("%s",m_pDoc->GetStationName(beginStaNo));
			else
				sStationStart ="管外";
			if(endStaNo>0)
				sStationEnd.Format("%s",m_pDoc->GetStationName(endStaNo));
			else
				sStationEnd ="管外";
            
			int entry = m_pDoc->ConvertAbsnumStationToEntry(rec->station, nLineNo);
			if(entry <= 0)
			{
				CString msg;
				msg.Format("找不到车站%d对应线号%d的出入口,请检查ENTRY.cfg文件", rec->station, nLineNo);
				m_pDoc->sysprint(5, 0, "[TG]%s", msg);
				//AfxMessageBox(msg, MB_ICONHAND);
			}
			else
			{
				sLineName[abs_suited].Format("%s--->%s(%d)双向", sStationStart, sStationEnd, nLineNo);
				abs_line_suited[abs_suited] = nLineNo;
				exit_list[abs_suited] = entry;
				abs_suited++;
			}
		}
	}

	FillGrid();
}

void CDlgGiveoutDirectionSel::InitExitLineNo4()
{
	//	_T("序号"),_T("方向"),_T("交出车站"),_T("选择") 
	if(m_pGridCtrl == NULL || m_nTrain == NULL)
		return;
	m_pGridCtrl->DeleteNonFixedRows();
 
	TgTRAIN_RECORD *rec = NULL, *pre = NULL;
	rec = m_nTrain->GetRecordByPosition(m_nFoucsRecIndex);
	if(rec == NULL) return;
	pre = m_nTrain->GetRecordByPosition(m_nFoucsRecIndex-1);
	
	//在所有线号中查找当前找到的线号名称
	CString sStationStart,sStationEnd;
    abs_suited = 0;
	for(int idx = 0; idx < exit_lines.size(); idx++)
	{
		int nLineNo = exit_lines[idx];
		const ABS_LINE* line = m_pDoc->m_ptgctrl->mConfigTg.m_tgdef.GetLineDefine(nLineNo);
		if(line==NULL)
			continue;

		if(line->nStartStation>0)
			sStationStart.Format("%s",m_pDoc->GetStationName(line->nStartStation) );
		else
			sStationStart ="管外";
		if(line->nEndStation>0)
			sStationEnd.Format("%s",m_pDoc->GetStationName(line->nEndStation ));
		else
			sStationEnd ="管外";
        
		int entry = m_pDoc->ConvertAbsnumStationToEntry(rec->station, nLineNo);
		if(entry <= 0)
		{
			CString msg;
			msg.Format("找不到车站%d对应线号%d的出入口,请检查ENTRY.cfg文件", rec->station, nLineNo);
			m_pDoc->sysprint(5, 0, "[TG]%s", msg);
		}
		else
		{
			if(line->nDirect ==3)
				sLineName[abs_suited].Format("%s--->%s(%d)(双向)", sStationStart, sStationEnd, nLineNo);
			else
				sLineName[abs_suited].Format("%s--->%s(%d)", sStationStart, sStationEnd, nLineNo);
			abs_line_suited[abs_suited] = nLineNo;
			exit_list[abs_suited] = entry;
			abs_suited++;
		}
	}

	FillGrid();
}

void CDlgGiveoutDirectionSel::InitEntryLineNo()
{
	//	_T("序号"),_T("方向"),_T("交出车站"),_T("选择") 
	if(m_pGridCtrl == NULL || m_nTrain == NULL)
		return;
	
	m_pGridCtrl->DeleteNonFixedRows();
 
	TgTRAIN_RECORD *rec = NULL;
	rec = m_nTrain->GetRecordByPosition(m_nFoucsRecIndex);
	if(rec == NULL) 
		return;
	 
	CString sStationStart,sStationEnd;
	abs_suited = 0;
	for(int idx = 0; idx < entry_lines.size(); idx++)
	{
		int nLineNo = entry_lines[idx];
		const ABS_LINE* line = m_pDoc->m_ptgctrl->mConfigTg.m_tgdef.GetLineDefine(nLineNo);
		if(line == NULL)
			continue;
		
		if(line->nEndStation == rec->station && line->nDirect == m_nTrain->m_nDirectFlag)
		{
			if(line->nStartStation>0)
				sStationStart.Format("%s",m_pDoc->GetStationName(line->nStartStation) );
			else
				sStationStart ="管外";
			if(line->nEndStation>0)
				sStationEnd.Format("%s",m_pDoc->GetStationName(line->nEndStation ));
			else
				sStationEnd ="管外";
            
			int entry = m_pDoc->ConvertAbsnumStationToEntry(rec->station, nLineNo);
			if(entry <= 0)
			{
				CString msg;
				msg.Format("找不到车站%d对应线号%d的出入口,请检查ENTRY.cfg文件", rec->station, nLineNo);
				m_pDoc->sysprint(5, 0, "[TG]%s", msg);
				//AfxMessageBox(msg, MB_ICONHAND);
			}
			else
			{
				sLineName[abs_suited].Format("%s--->%s(%d)", sStationStart, sStationEnd, nLineNo);
				abs_line_suited[abs_suited] = nLineNo;
				exit_list[abs_suited] = entry;
				abs_suited++;
			}
		}
		else if(line->nDirect ==3)
		{
			int beginStaNo, endStaNo;
			if(line->nStartStation == rec->station)
			{
				endStaNo = line->nStartStation;
				beginStaNo   = line->nEndStation;
			}
			else if(line->nEndStation == rec->station)
			{
				beginStaNo = line->nStartStation;
				endStaNo   = line->nEndStation;
			}
			else
			{
				continue;
			}
				
			if(beginStaNo>0)
				sStationStart.Format("%s",m_pDoc->GetStationName(beginStaNo) );
			else
				sStationStart ="管外";
			if(endStaNo>0)
				sStationEnd.Format("%s",m_pDoc->GetStationName(endStaNo));
			else
				sStationEnd ="管外";
			
			int entry = m_pDoc->ConvertAbsnumStationToEntry(rec->station, nLineNo);
			if(entry <= 0)
			{
				CString msg;
				msg.Format("找不到车站%d对应线号%d的出入口,请检查ENTRY.cfg文件", rec->station, nLineNo);
				m_pDoc->sysprint(5, 0, "[TG]%s", msg);
				//AfxMessageBox(msg, MB_ICONHAND);
			}
			else
			{
				sLineName[abs_suited].Format("%s--->%s(%d)双向", sStationStart, sStationEnd, nLineNo);
				abs_line_suited[abs_suited] = nLineNo;
				exit_list[abs_suited] = entry;
				abs_suited++;
			}
		}
	}

	FillGrid();
}

void CDlgGiveoutDirectionSel::InitEntryLineNo5()
{
	//	_T("序号"),_T("方向"),_T("交出车站"),_T("选择") 
	if(m_pGridCtrl == NULL || m_nTrain == NULL)
		return;
	
	m_pGridCtrl->DeleteNonFixedRows();
 
	TgTRAIN_RECORD *rec = NULL;
	rec = m_nTrain->GetRecordByPosition(m_nFoucsRecIndex);
	if(rec == NULL) 
		return;
	 
	CString sStationStart,sStationEnd;
	abs_suited = 0;
	for(int idx = 0; idx < entry_lines.size(); idx++)
	{
		int nLineNo = entry_lines[idx];
		const ABS_LINE* line = m_pDoc->m_ptgctrl->mConfigTg.m_tgdef.GetLineDefine(nLineNo);
		if(line == NULL)
			continue;

		if(line->nStartStation>0)
			sStationStart.Format("%s",m_pDoc->GetStationName(line->nStartStation) );
		else
			sStationStart ="管外";
		if(line->nEndStation>0)
			sStationEnd.Format("%s",m_pDoc->GetStationName(line->nEndStation ));
		else
			sStationEnd ="管外";
        
		int entry = m_pDoc->ConvertAbsnumStationToEntry(rec->station, nLineNo);
		if(entry <= 0)
		{
			CString msg;
			msg.Format("找不到车站%d对应线号%d的出入口,请检查ENTRY.cfg文件", rec->station, nLineNo);
			m_pDoc->sysprint(5, 0, "[TG]%s", msg);
		}
		else
		{
			if(line->nDirect ==3)
				sLineName[abs_suited].Format("%s--->%s(%d)(双向)", sStationStart, sStationEnd, nLineNo);
			else
				sLineName[abs_suited].Format("%s--->%s(%d)", sStationStart, sStationEnd, nLineNo);
			abs_line_suited[abs_suited] = nLineNo;
			exit_list[abs_suited] = entry;
			abs_suited++;
		}
	}

	FillGrid();
}

void CDlgGiveoutDirectionSel::FillGrid()
{
	if(m_pGridCtrl==NULL)
		return;

	m_pGridCtrl->SetRowCount(abs_suited + 1);  

	CGridCellCheck *pCell;
	for(int i = 0; i < abs_suited; i++)
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

		//1 车站方向
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.nState = GVIS_READONLY;
		Item.row = i+1;  //行
		Item.col = 1;  //列
		Item.crBkClr = CLR_RED;        
		Item.nFormat = DT_LEFT|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
		Item.strText.Format("%s", sLineName[i]);
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
				pCell->SetCheck(TRUE); 
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
				nSelDirect = exit_list[i];
				break;
			}
		}
	}
   if(i>= abs_suited && abs_suited!=0)
   {
	   AfxMessageBox("请选择方向");
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


















