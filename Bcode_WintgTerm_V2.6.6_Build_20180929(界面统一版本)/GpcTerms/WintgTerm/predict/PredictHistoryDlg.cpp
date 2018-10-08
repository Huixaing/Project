// TransportInfo.cpp : implementation file
//

#include "stdafx.h"
#include "io.h"
#include "string.h"
#include "DlgInputDate.h"
#include ".\PredictHistoryDlg.h"
#include "memshare.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPredictHistoryDlg dialog
#define   IDC_PREDICT_HIS_GRID_ID  64322
#import  <msxml.dll> named_guids
using namespace MSXML;

extern void GetPredictFromXML(TrainPredict& predict, MSXML::IXMLDOMNodePtr pRootNode);

extern void GetPredictGridHeader(CString& xml);
extern void StplanToXML(const TrainPredict& predict, CString& strxml);

CPredictHistoryDlg::CPredictHistoryDlg(int nSectionID, CWnd* pParent)
: CDialog(CPredictHistoryDlg::IDD, pParent)
{
	for(int i=0; i<MAX_PREDICT_STATION_PLAN; i++)
		m_pStPlans[i]=0;
	m_nSectionID=nSectionID;
}

void CPredictHistoryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPredictHistoryDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPredictHistoryDlg, CDialog)
	//{{AFX_MSG_MAP(CPredictHistoryDlg)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnCbnSelchangeStation)
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON1, LoadFromHistory)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPredictHistoryDlg message handlers
int   CPredictHistoryDlg::OnCreate(LPCREATESTRUCT lpCreat)
{
	if(CDialog::OnCreate(lpCreat))
		return -1;
	CRect rect(0,0,0,0);

	CString xml;
	GetPredictGridHeader(xml);
	
	m_grid.CreateGrid(xml, this,rect,IDC_PREDICT_HIS_GRID_ID);
	m_grid.SetEditable(FALSE);
	
	return 0;
}

void  CPredictHistoryDlg::OnSize(UINT nType,int cx,int cy)
{
	if(m_grid.GetSafeHwnd())
		m_grid.MoveWindow(1,50,cx-1,cy-60);
	CDialog::OnSize(nType,cx,cy);
}

BOOL  CPredictHistoryDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();	

	CFont font;
	font.CreateFont(18, 0,0,0, FW_HEAVY, 0,0,0, DEFAULT_CHARSET,
		OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, "Arial");
	m_grid.SetFont(&font);
	font.DeleteObject();

	m_grid.AutoWidth();
	m_grid.Refresh();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPredictHistoryDlg::AppendPredictPlan(TrainPredict& predict)
{
	int i=0;
	for(; i<MAX_PREDICT_STATION_PLAN; i++)
	{
		if(m_pStPlans[i]!=NULL)
		{
			if(m_pStPlans[i]->GetStationNo()==predict.station_no)
			{
				break;
			}
		}
		else
		{
			m_pStPlans[i]=new CStationPredict(predict.station_no, 0);
			break;
		}
	}

	if(i>=MAX_PREDICT_STATION_PLAN)
		return;

	for(UINT k=0; k<MAX_STPLAN_NUM; k++)
	{
		if(m_pStPlans[i]->m_stplan[k].train_index==0 && m_pStPlans[i]->m_stplan[k].station_no==0)
		{
			m_pStPlans[i]->m_stplan[k]=predict;
			return;
		}
		if(m_pStPlans[i]->m_stplan[k].train_index==0 && m_pStPlans[i]->m_stplan[k].station_no==65535)
		{
			m_pStPlans[i]->m_stplan[k]=predict;
			return;
		}
	}
	
	return;
}

void CPredictHistoryDlg::LoadFromHistory()
{
	CDlgInputDate dlg(this);
	if(dlg.DoModal() != IDOK)
		return;
	
	CTime t;
	t = dlg.m_inputDate;
	m_strShiftid = t.Format("%Y%m%d");
	if(dlg.m_inputBanbie == 1)
		m_strShiftid += "01";
	else
		m_strShiftid += "02";

	CString fileName;
	fileName.Format("%s\\predict\\%d_%s.xml",gpcdef.GetWorkPathName(), m_nSectionID, m_strShiftid);

	// 判断历史文件是否存在
	CFileFind finder;

	// 如果文件存在，不进行转储
	if(!finder.FindFile(fileName))
	{
		CString text;
		text.Format("不存在 %s 班次历史记录", m_strShiftid);
		AfxMessageBox(text);
		return;
	}

	for(int i=0; i<MAX_PREDICT_STATION_PLAN; i++)
	{
		delete m_pStPlans[i];
		m_pStPlans[i]=0;
	}
	
	m_grid.DeleteNonFixedRows();

	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		AfxMessageBox(er.ErrorMessage());
		return;
	}
	
	if(VARIANT_TRUE != pXMLDoc->load( _bstr_t( fileName ) ) )
	{
		CString  prompt;
		prompt.Format("预告计划 %s不存在或者不符合xml语法(可以用浏览器检查是否能打开)", fileName);
		//AfxMessageBox(prompt);
		return;
	}
	
	MSXML::IXMLDOMNodePtr pChildNode,pNode,pRootNode;

	pRootNode = pXMLDoc->selectSingleNode( "PREDICT_PLAN");
	if(pRootNode == NULL)
		return;

	MSXML::IXMLDOMNodeListPtr pPlan = pRootNode->selectNodes( "PLAN");
	if(NULL == pPlan)
		return;
	
	for(int idx = 0; idx < pPlan->Getlength(); idx++)
	{
		pNode = pPlan->Getitem(idx);
		if(pNode==NULL)
			continue;
		
		TrainPredict predict;
		GetPredictFromXML(predict, pNode);
		
		AppendPredictPlan(predict);
	}

	// 根据文件中车站设置车站下拉框
	UpdateStationList();
	return;
}

void CPredictHistoryDlg::UpdateStationList()
{
	// TODO: 在此添加控件通知处理程序代码
	bool bExist=false;
	CComboBox* pBox = (CComboBox*)GetDlgItem(IDC_COMBO1);
	if(pBox)
	{
		pBox->ResetContent();
		for(int i=0; i<MAX_PREDICT_STATION_PLAN; i++)
		{
			if(m_pStPlans[i]==NULL)
				break;
			
			bExist=true;
			
			UINT id=pBox->AddString(gpcdef.GetStationName(m_pStPlans[i]->GetStationNo()));
			pBox->SetItemData(id, m_pStPlans[i]->GetStationNo());
		}

		if(bExist)
		{
			pBox->SetCurSel(0);
			OnCbnSelchangeStation();
		}
	}
}

void CPredictHistoryDlg::OnCbnSelchangeStation()
{
	CComboBox* pBox = (CComboBox*)GetDlgItem(IDC_COMBO1);
	if(pBox==NULL)
		return;

	int nSel=pBox->GetCurSel();
	if(nSel<0)
		return;

	int station=pBox->GetItemData(nSel);
	if(station>0)
	{
		for(int i=0; i<MAX_PREDICT_STATION_PLAN; i++)
		{
			if(m_pStPlans[i]->GetStationNo()==station)
				break;
		}
		if(i>=MAX_PREDICT_STATION_PLAN)
			return;

		CString strTitle;
		strTitle.Format("%s %s 预告计划 ", m_strShiftid, gpcdef.GetStationName(station));
		SetWindowText(strTitle);

		m_grid.DeleteNonFixedRows();

		mPlan.clear();
		for(int n=0; n<MAX_STPLAN_NUM; n++)
		{
			if(m_pStPlans[i]->m_stplan[n].train_index==0 && m_pStPlans[i]->m_stplan[n].station_no==0)
			{
				break;
			}
			if(m_pStPlans[i]->m_stplan[n].train_index==0 && m_pStPlans[i]->m_stplan[n].station_no==65535)
			{
				continue;
			}

			mPlan.insert(make_pair(m_pStPlans[i]->m_stplan[n].sort, m_pStPlans[i]->m_stplan[n]));
			
		}
		ReloadStPlan();
	}
	this->SetFocus();
}

void CPredictHistoryDlg::ReloadStPlan()
{
	CString  xml;
	xml="<STATION_REC_LIST>";
	UINT count=0;
	CTime tm = CTime::GetCurrentTime();

	map<UINT, TrainPredict>::iterator it = mPlan.begin();
	for( ; it!=mPlan.end(); it++)
	{		
		CString trainstr;
		StplanToXML(it->second, trainstr);

		if(trainstr.IsEmpty())
			continue;

		xml+="<RECORD>";
		xml+=trainstr;
		xml+="</RECORD>";
	}
	xml+="</STATION_REC_LIST>";

	m_grid.AddRowDataFromXML(xml, "STATION_REC_LIST/RECORD");
	m_grid.Refresh();
}

void CPredictHistoryDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	for(int i=0; i<MAX_PREDICT_STATION_PLAN; i++)
	{
		if(m_pStPlans[i])
		    delete m_pStPlans[i];
		m_pStPlans[i]=0;
	}
	CDialog::OnClose();

	delete this;
}
