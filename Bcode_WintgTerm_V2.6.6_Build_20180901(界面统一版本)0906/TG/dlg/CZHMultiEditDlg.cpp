// CZHManage.cpp : 实现文件
//

#include "stdafx.h"
#include "TG.h"

#include "CZHMultiEditDlg.h"
#include ".\czhmultieditdlg.h"
#include ".\dlg\InputTextDlg.h"

#define   IDC_CZH_GRID_ID  55323
// CZHMultiEditDlg 对话框
void GetCZHGridHeader(CString& xml)
{
	CString tmp;
	xml = "<FIELDS>";
	tmp.Format("<FIELD caption=\"序\" attr_name=\"ID\" />");
	xml += tmp;
	tmp.Format("<FIELD caption=\"车次号\" attr_name=\"TRAIN_ID\" />");
	xml += tmp;
	tmp.Format("<FIELD caption=\"本台开始\" attr_name=\"CON_ST_STATION\" />");
	xml += tmp;
	tmp.Format("<FIELD caption=\"本台结束\" attr_name=\"CON_ED_STATION\" />");
	xml += tmp;
	tmp.Format("<FIELD caption=\"开始时间\" attr_name=\"CON_ST_TIME\" />");
	xml += tmp;
	tmp.Format("<FIELD caption=\"结束时间\" attr_name=\"CON_ED_TIME\" />");
	xml += tmp;
	
	xml += "</FIELDS>";
}

IMPLEMENT_DYNAMIC(CZHMultiEditDlg, CDialog)
CZHMultiEditDlg::CZHMultiEditDlg(CString title, CString strInitCzh, CTGDoc *pDoc, CWnd* pParent, BYTE flag)
	: CDialog(CZHMultiEditDlg::IDD, pParent)
{
	m_pDoc=pDoc;
	m_nFlag=flag;
	m_strInitCzh=strInitCzh;
	m_title=title;
}

CZHMultiEditDlg::~CZHMultiEditDlg()
{
}

void CZHMultiEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_COMBO_CZH, m_group);

	DDX_CBString(pDX, IDC_EDIT2, m_trainid);
	DDV_MaxChars(pDX, m_trainid, 9);
}


BEGIN_MESSAGE_MAP(CZHMultiEditDlg, CDialog)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_CBN_SELCHANGE(IDC_COMBO_CZH, OnSelCZHChange)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON1, OnBnAddCZH)
	ON_BN_CLICKED(IDC_BUTTON7, OnBnModifyCZH)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnDeleteCZH)
	ON_BN_CLICKED(IDC_BUTTON5, OnBnAddTrain)
	ON_BN_CLICKED(IDC_BUTTON6, OnBnDeleteTrain)
	ON_MESSAGE(MSG_UPDATE_CZH, OnUpdateSelCZHTrain)
	ON_MESSAGE(10101, OnSelectTrain)	
END_MESSAGE_MAP()


// CZHMultiEditDlg 消息处理程序
int   CZHMultiEditDlg::OnCreate(LPCREATESTRUCT lpCreat)
{
	if(CDialog::OnCreate(lpCreat))
		return -1;
	CRect rect(0,0,0,0);

	CString xml;
	GetCZHGridHeader(xml);
	
	m_grid.CreateGrid(xml, this,rect,IDC_CZH_GRID_ID);
	m_grid.SetEditable(FALSE);

	return 0;
}

void  CZHMultiEditDlg::OnSize(UINT nType,int cx,int cy)
{
	if(m_grid.GetSafeHwnd())
	{
		m_grid.MoveWindow(1,30,cx-1,cy-93);
	}

	CDialog::OnSize(nType,cx,cy);
}

BOOL  CZHMultiEditDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();	

	CFont font;
	font.CreateFont(18, 0,0,0, FW_BOLD, 0,0,0, DEFAULT_CHARSET,
		OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, "Arial");
	m_grid.SetFont(&font);
	font.DeleteObject();

	CEdit* pEdit= NULL;
	pEdit= (CEdit*)GetDlgItem(IDC_EDIT2);
	if(pEdit)
		pEdit->SetLimitText(9);
	
	UpdateSelectComboBox(m_strInitCzh);
	
	OnSelCZHChange();

	m_grid.AutoWidth();
	m_grid.Refresh();

	this->SetWindowText(m_title);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CZHMultiEditDlg::OnUpdateSelCZHTrain(WPARAM wParam, LPARAM lParam)
{
	// 判断车组列表是否需要刷新
	CStringArray array;
	m_pDoc->myChezuhao.GetTrainCZHText(array);
	bool bCzhChged=false;
	if(array.GetCount()!=m_group.GetCount())
	{
		bCzhChged=true;
	}
	else
	{
		for(int i=0; i<array.GetCount(); i++)
		{
			CString czh=array.GetAt(i);

			int n=0;
			for(n=0; n<m_group.GetCount();n++)
			{
				CString text;
				m_group.GetLBText(i,text);
				if(text==czh)
				{
					break;
				}
			}
			if(n>=m_group.GetCount())
			{
				bCzhChged=true;
				break;
			}
		}
	}

	if(bCzhChged)
	{
		CString czh;
		int nSel=m_group.GetCurSel();
		if(nSel>=0)
		{
			m_group.GetLBText(nSel, czh);
		}

		m_group.ResetContent();

		nSel=-1;
		for(int i=0; i<array.GetCount(); i++)
		{
			int index=m_group.AddString(array.GetAt(i));
			
			if(czh==array.GetAt(i))
				nSel=index;
		}

		if(nSel>=0)
		{
			m_group.SetCurSel(nSel);
		}
	}

	OnSelCZHChange();
	return 0;
}

void CZHMultiEditDlg::OnSelCZHChange()
{
	int nSel=m_group.GetCurSel();
	if(nSel>=0)
	{
		CString czh;
		m_group.GetLBText(nSel, czh);
		TRAIN_CHEZUHAO* pTrainsCZH=m_pDoc->myChezuhao.GetCzhByCheZuHao(czh);
		ShowCZHTrains(pTrainsCZH);
	}

	SetDeleteTrainInfo("", "");
}

void CZHMultiEditDlg::ShowCZHTrains(TRAIN_CHEZUHAO* pTrainsCZH)
{
	m_grid.DeleteNonFixedRows();
	m_grid.Refresh();

	if(pTrainsCZH==NULL)
		return;

	CString xml, tmp;
	xml="<CZH>";
	std::map<ULONG, TRAIN_CZH_INFO>::iterator it = pTrainsCZH->trains.begin(), next_it;
	for(;it!=pTrainsCZH->trains.end(); it++)
	{
		xml+="<RECORD>";
		
		tmp.Format("<TRAIN_INDEX>%d</TRAIN_INDEX>", it->second.train_index);
		xml += tmp;

		tmp.Format("<TRAIN_ID>%s</TRAIN_ID>", it->second.trainid);
		xml += tmp;

		tmp.Format("<CON_ST_STATION>%s</CON_ST_STATION>", m_pDoc->GetStationName(it->second.begin_station));
		xml += tmp;

		tmp.Format("<CON_ED_STATION>%s</CON_ED_STATION>", m_pDoc->GetStationName(it->second.end_station));
		xml += tmp;

		if(m_pDoc->GetTgType() == TG_TYPE::BASIC_TG)
		{
			CString day, time;
			if(it->second.stTime>0)
			{
				day.Format("%d", it->second.stTime/86400);
				time = ConvertTimetoString(it->second.stTime%86400);
				tmp.Format("<CON_ST_TIME>%s-%s</CON_ST_TIME>", day, time);
				xml += tmp;
			}
			if(it->second.edTime>0)
			{
				day.Format("%d", it->second.edTime/86400);
				time = ConvertTimetoString(it->second.edTime%86400);
				tmp.Format("<CON_ED_TIME>%s-%s</CON_ED_TIME>", day, time);
				xml += tmp;
			}
		}
		else
		{
			if(it->second.stTime>0)
			{
				CTime tm(it->second.stTime);
				tmp.Format("<CON_ST_TIME>%s</CON_ST_TIME>", tm.Format("%Y-%m-%d %H:%M"));
				xml += tmp;
			}

			if(it->second.edTime>0)
			{
				CTime tm(it->second.edTime);
				tmp.Format("<CON_ED_TIME>%s</CON_ED_TIME>", tm.Format("%Y-%m-%d %H:%M"));
				xml += tmp;
			}
		}

		xml+="</RECORD>";
	}

	xml+="</CZH>";

	m_grid.AddRowDataFromXML(xml, "CZH/RECORD");
	m_grid.Refresh();
}

void CZHMultiEditDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	DestroyWindow();
}

void CZHMultiEditDlg::UpdateSelectComboBox(CString czh)
{
	m_group.ResetContent();

	int nSel=-1;
	CStringArray array;
	m_pDoc->myChezuhao.GetTrainCZHText(array);
	
	for(int i=0; i<array.GetCount(); i++)
	{
		int index=m_group.AddString(array.GetAt(i));
		
		if(czh==array.GetAt(i))
			nSel=index;
	}

	if(nSel>=0)
	{
		m_group.SetCurSel(nSel);
	}

	OnSelCZHChange();
}

CString CZHMultiEditDlg::GetSelectCZH()
{
	CString czh="";

	int nSel=m_group.GetCurSel();
	if(nSel>=0)
	{
		m_group.GetLBText(nSel, czh);
	}
	else
	{
		MessageBox("请选中车组号", "提示", MB_OK|MB_ICONQUESTION);
	}

	return czh;
}

CString CZHMultiEditDlg::GetEditCZH()
{
	CString czh="";
	CInputTextDlg dlg(1);
	if(dlg.DoModal()!=IDOK)
		return "";
	czh=dlg.inputtext;
	
	if(czh.IsEmpty())
	{
		MessageBox("车组号不能为空", "提示", MB_OK|MB_ICONQUESTION);
	}

	return czh;
}

void CZHMultiEditDlg::OnBnAddCZH()
{
	// TODO: 在此添加控件通知处理程序代码
	CString czh = GetEditCZH();
	if(czh.IsEmpty())
		return;

	CString text;
	UINT i=0;
	for(i=0; i<m_group.GetCount();i++)
	{
		text="";
		m_group.GetLBText(i,text);
		if(text==czh)
		{
			text.Format("车组号 %s 已存在", czh);
			MessageBox(text, "提示", MB_OK|MB_ICONQUESTION);
			break;
		}
	}

	if(i>=m_group.GetCount())
	{
		m_pDoc->AddNewTrainCZH(czh);
	}
	
	UpdateSelectComboBox(czh);
	
	return;
}

void CZHMultiEditDlg::OnBnModifyCZH()
{
	// TODO: 在此添加控件通知处理程序代码
	CString czhnew = GetEditCZH();
	if(czhnew.IsEmpty())
		return;

	CString czh = GetSelectCZH();
	if(czh.IsEmpty())
		return;

	CString text;
	if(czh == czhnew)
	{
		text.Format("请注意, 修改前后车组号相同,均为%s, 不需要修改", czhnew);
		MessageBox(text, "提示", MB_ICONQUESTION|MB_OK);
		return;
	}
	
	TRAIN_CHEZUHAO* pTrainsCZH=m_pDoc->myChezuhao.GetCzhByCheZuHao(czh);
	if(pTrainsCZH==NULL)
	{
		text.Format("请注意, 车组号 %s 不存在", czh);
		MessageBox(text, "提示", MB_OK|MB_ICONQUESTION);
		return;
	}
	else if(pTrainsCZH->trains.size()==0)
	{
		pTrainsCZH->czh=czhnew;
		UpdateSelectComboBox(czhnew);
		return;
	}

	text.Format("请注意, 你确认 将车组号为 %s 的列车车组修改为 %s", czh, czhnew);
	if(IDNO==MessageBox(text, "提示", MB_ICONQUESTION|MB_YESNO))
		return;

	// 增加新车组号
	UINT i=0;
	for(i=0; i<m_group.GetCount();i++)
	{
		CString text;
		m_group.GetLBText(i,text);
		if(text==czhnew)
		{
			text.Format("修改后的车组号 %s 已存在,你确认使用该车组号吗?", czhnew);
			if(IDYES==MessageBox(text, "提示", MB_ICONQUESTION|MB_YESNO))
			{
				break;
			}
			else
			{
				text.Format("请注意, 车组号 %s 修改失败,请修改车组号", czh);
				MessageBox(text, "提示", MB_OK|MB_ICONHAND);
				return;
			}
		}
	}

	if(i>=m_group.GetCount())
	{
		m_pDoc->AddNewTrainCZH(czhnew);
	}

	m_pDoc->ModifyTrainCZH(czh, czhnew);

	UpdateSelectComboBox(czhnew);
	
	return;
}

void CZHMultiEditDlg::OnBnDeleteCZH()
{
	// TODO: 在此添加控件通知处理程序代码
	CString czh = GetSelectCZH();
	if(czh.IsEmpty())
		return;

	CString text;
	
	TRAIN_CHEZUHAO* pTrainsCZH=m_pDoc->myChezuhao.GetCzhByCheZuHao(czh);
	if(pTrainsCZH==NULL)
	{
		UpdateSelectComboBox(czh);
		return;
	}
	else if(pTrainsCZH->trains.size()==0)
	{
		pTrainsCZH->czh="";
		UpdateSelectComboBox(czh);
		return;
	}
	text.Format("请注意, 你确认 将车组号为 %s 的列车车组删除吗?", czh);
	if(IDNO==MessageBox(text, "提示", MB_ICONQUESTION|MB_YESNO))
		return;

	m_pDoc->DeleteTrainCZH(czh);
	m_group.SetCurSel(-1);
	m_grid.DeleteNonFixedRows();
	m_grid.Refresh();
	UpdateSelectComboBox(czh);
	return;
}

void CZHMultiEditDlg::OnBnAddTrain()
{
	// TODO: 在此添加控件通知处理程序代码
	CString czh = GetSelectCZH(), trainid="";
	if(czh.IsEmpty())
		return;
	CWnd* pWnd=GetDlgItem(IDC_EDIT2);
	if(pWnd)
		pWnd->GetWindowText(trainid);

	if(trainid.IsEmpty())
	{
		MessageBox("请输入要增加的车次", "提示", MB_OK|MB_ICONQUESTION);
		return;
	}

	m_pDoc->AddTrainToCZHGroup(czh, trainid);
	
	return;
}

LRESULT CZHMultiEditDlg::OnSelectTrain(WPARAM wParam,LPARAM lParam)
{
	ULONG trainindex=(ULONG)lParam;
	CString strSelTrainid=(char*)wParam;
		
	CString trainindexstr;
	trainindexstr.Format("%d", trainindex);
	SetDeleteTrainInfo(strSelTrainid, trainindexstr);
	return TRUE;
}

void CZHMultiEditDlg::OnBnDeleteTrain()
{
	// TODO: 在此添加控件通知处理程序代码
	CString czh = GetSelectCZH(), trainid="", trainindex="";
	if(czh.IsEmpty())
		return;

	CWnd* pWnd=GetDlgItem(IDC_EDIT6);
	if(pWnd)
		pWnd->GetWindowText(trainindex);

	if(trainindex.IsEmpty())
	{
		MessageBox("请选中要删除的车次", "提示", MB_OK|MB_ICONQUESTION);
		return;
	}

	CTG_TRAIN* pTrain=m_pDoc->GetTrainByIndex(atol(trainindex));
	if(pTrain==NULL)
	{
		MessageBox("请选中要删除的车次", "提示", MB_OK|MB_ICONQUESTION);
		return;
	}

	trainid=m_pDoc->GetTrainShowId(pTrain);
	CString text;
	text.Format("你确实要从车组 %s 中删除 %s 吗?", czh, trainid);
	if(IDYES!=MessageBox(text, "提示", MB_YESNO|MB_ICONQUESTION))
	{
		return;
	}

	if(m_pDoc->DelTrainFromCZHGroup(czh, trainindex))
	{
		SetDeleteTrainInfo("", "");
	}
	
	return;
}

void CZHMultiEditDlg::PostNcDestroy() 
{
	delete this;
}

void CZHMultiEditDlg::SetDeleteTrainInfo(CString trainid, CString trainindexstr)
{
	CWnd* pWnd=GetDlgItem(IDC_EDIT8);
	if(pWnd)
		pWnd->SetWindowText(trainid);

	pWnd=GetDlgItem(IDC_EDIT6);
	if(pWnd)
		pWnd->SetWindowText(trainindexstr);
}
