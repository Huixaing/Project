// DlgBatchSideModify.cpp : 实现文件
//

#include "stdafx.h"
#include "function.h"
#include <baseMsg\msgid.h>
#include <baseMsg\clbth.h>
#include "tg_basicpara.h"
#include "tg_function_config.h"
#include "DlgBatchSideModify.h"
#include ".\dlgbatchsidemodify.h"
#include "GridCtrl_src/GridCellCheck.h"
#include "gridctrl_src/GridCellCombo.h"
#include "memshare.h"

#import  <msxml.dll> named_guids
using namespace MSXML;

void  CSideGridView::SetParentDlg(DlgBatchSideModify* pDlg)
{
	m_pDlg = pDlg;
}

BOOL  CSideGridView::CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid)
{
	m_record_col_count=0;
	strcpy(m_train_record_def[m_record_col_count].caption,"序号");
	sprintf(m_train_record_def[m_record_col_count].attr_name,"no");
	m_train_record_def[m_record_col_count].data_type=CTC_DATA_STR;
	m_train_record_def[m_record_col_count].flag=0;
	m_record_col_count++;

	strcpy(m_train_record_def[m_record_col_count].caption,"车站名称");
	sprintf(m_train_record_def[m_record_col_count].attr_name,"station_name");
	m_train_record_def[m_record_col_count].data_type=CTC_DATA_STR;
	m_train_record_def[m_record_col_count].flag=0;
	m_record_col_count++;

	strcpy(m_train_record_def[m_record_col_count].caption,"股道");
	sprintf(m_train_record_def[m_record_col_count].attr_name,"side");
	m_train_record_def[m_record_col_count].data_type=CTC_DATA_STR;
	m_train_record_def[m_record_col_count].flag=30;
	m_record_col_count++;

	CXMLDataGrid::CreateGrid(parent,rect,ctrlid,m_train_record_def,m_record_col_count);

	SetColumnWidth(0, 50);
	SetColumnWidth(1, 80);
	SetColumnWidth(2, 80);
	
	return TRUE;
}

void  CSideGridView::DisplaySideInfo(const char *xml,const char *rootnode)
{
	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		AfxMessageBox(er.ErrorMessage());
		return;
	}

	if(VARIANT_TRUE != pXMLDoc->loadXML( _bstr_t( xml ) ) )
	{
		return;
	} 

	MSXML::IXMLDOMNodeListPtr pRecAbsList = pXMLDoc->selectNodes(rootnode);
	int rec_count = pRecAbsList->Getlength();
	for(int i = 0; i <rec_count; i++)
	{
		int col;
		int row=InsertRow("");
		if(row<0)
			break;
		for ( col = 0; col < m_nFieldNum; col++)
		{ 
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT|GVIF_PARAM;
			Item.row = row;
			Item.col = col;
			if(m_bSingleLine)
				Item.nFormat = DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX;
			else
				Item.nFormat = DT_LEFT|DT_VCENTER|DT_NOPREFIX;
			Item.lParam=0;

			CString attr;
			CString value="NA";
			attr.Format("%s", m_FiledInfo[col].attr_name);

			MSXML::IXMLDOMNodePtr pNode = pRecAbsList->Getitem(i)->selectSingleNode(_bstr_t(attr));
			if(pNode)
			{
				_variant_t v;
				v = pNode->GetnodeTypedValue();
				Item.strText = CString((v.bstrVal));
				SysFreeString(v.bstrVal);
                v.Clear();
			}
			else 
			{
				Item.strText = "";
			}

			SetItem(&Item);
		}
		CString colorstr;
		MSXML::IXMLDOMNodePtr pNode = pRecAbsList->Getitem(i)->selectSingleNode("COLOR");
		if(pNode)
		{
			_variant_t v;
			v = pNode->GetnodeTypedValue();
			colorstr = CString((v.bstrVal));
			SysFreeString(v.bstrVal);
               v.Clear();
		}

		if(!stricmp(colorstr,"RED"))
			SetItemBkColour(row,0,RGB(255,0,0));
		else
			if(!stricmp(colorstr,"GREEN"))
				SetItemBkColour(row,0,RGB(0,255,0));
			else
				if(!stricmp(colorstr,"BLUE"))
					SetItemBkColour(row,0,RGB(0,0,255));
				else
					if(!stricmp(colorstr,"YELLOW"))
						SetItemBkColour(row,0,RGB(255,255,0));
					else
						if(!stricmp(colorstr,"WHITE"))
							SetItemBkColour(row,0,RGB(255,255,255));
						else
							if(!stricmp(colorstr,"GRAY"))
								SetItemBkColour(row,0,RGB(192,192,192));
							else
								SetItemBkColour(row,0,RGB(192,192,0));

		int itemdata=0;
		pNode = pRecAbsList->Getitem(i)->selectSingleNode("ITEMDATA0");
		if(pNode)
		{
			_variant_t v;
			v = pNode->GetnodeTypedValue();
			itemdata = atoi(CString((v.bstrVal)));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		if(itemdata>=0)
			GetCell(row,0)->SetData(itemdata);

		itemdata=0;
		pNode = pRecAbsList->Getitem(i)->selectSingleNode("ITEMDATA1");
		if(pNode)
		{
			_variant_t v;
			v = pNode->GetnodeTypedValue();
			itemdata = atoi(CString((v.bstrVal)));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		if(itemdata>=0)
			GetCell(row,1)->SetData(itemdata);
	}
	Refresh();
}

void  CSideGridView::BeginEditCell(int row,int col)
{
	if(m_FiledInfo[col].flag==30) // 股道
	{
		int station=GetCell(row,1)->GetData();
		CStringArray options;
		int value;
		for(int i=0;i<MAX_SIDE_COUNT;i++)
		{
			const char *ptmpstr=GetTGBasicParam()->GetSideNameByIndex(station,i,&value);
			if(ptmpstr)
				options.Add(ptmpstr);
			else
				break;
		}
		SetCellType(row ,col, RUNTIME_CLASS(CGridCellCombo));
		CGridCellCombo *pCell=(CGridCellCombo *)GetCell(row,col);
		pCell->SetOptions(options);
		pCell->SetStyle(pCell->GetStyle() | CBS_DROPDOWNLIST);
	}
	else
		CXMLDataGrid::BeginEditCell(row,col);
}

void  CSideGridView::EndEditCell(int row,int col)
{
	SetCellType(row ,col,RUNTIME_CLASS(CGridCell));
	if(m_train_record_def[col].flag==30) // 股道
	{
		int rec_index = GetCell(row,0)->GetData();
		int station   = GetCell(row,1)->GetData();
		
		CGridCellCombo *pCell=(CGridCellCombo *)GetCell(row,col);
		int sideno=GetTGBasicParam()->GetSideIDByName(station,pCell->GetText());
		bool bSucc=false;
		if(sideno>0)
		{
			bSucc=m_pDlg->ModifySide(rec_index, station, sideno);
			m_pDlg->DisplayTrainSide();
		}
		if(!bSucc)
		{
			AfxMessageBox("股道修改失败!");
		}
	}
	else
		CXMLDataGrid::EndEditCell(row,col);
}

// DlgBatchSideModify 对话框

IMPLEMENT_DYNAMIC(DlgBatchSideModify, CDialog)
DlgBatchSideModify::DlgBatchSideModify(const CTG_SingleTrain& train, CString strCaption, CTgDocument* pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(DlgBatchSideModify::IDD, pParent)
{
	m_train=train;
	m_strCaption=strCaption;
	m_pDoc=pDoc;
}

DlgBatchSideModify::~DlgBatchSideModify()
{
}

void DlgBatchSideModify::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DlgBatchSideModify, CDialog)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

BOOL DlgBatchSideModify::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetWindowText(m_strCaption);

	m_sideGrid.SetParentDlg(this);
	m_sideGrid.SetEditable(TRUE);
	DisplayTrainSide();
	return TRUE;
}
// DlgBatchSideModify 消息处理程序
int DlgBatchSideModify::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	CRect rect(0,0,0,0);
	m_sideGrid.CreateGrid(this,rect,54321);
	return 0;
}

void DlgBatchSideModify::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if(m_sideGrid.GetSafeHwnd())
		m_sideGrid.MoveWindow(0,0,cx-8,cy-40);
}

void DlgBatchSideModify::DisplayTrainSide()
{
	CString xml="<SIDELIST>";
	for(int n=0; n<m_train.GetTrainRecordCount();n++)
	{
		CTG_TrainRecord *prec=m_train.GetTrainRecordByRecIndex(n);
		if(prec==NULL)
			continue;
		int station=prec->GetStationNo();
		int sideno=prec->m_data.arrive_side;
		int sideno2=prec->m_data.depart_side;

		xml+="<RECORD>";
		CString tmp;
		tmp.Format("<no>%d</no>", n+1);
		xml += tmp;
		
		tmp.Format("<station_name>%s</station_name>", gpcdef.GetStationName(station));
		xml += tmp;

		tmp.Format("<ITEMDATA0>%d</ITEMDATA0>", n);
		xml += tmp;

		tmp.Format("<ITEMDATA1>%d</ITEMDATA1>", station);
		xml += tmp;

		CString sidename;
		const char* p=GetTGBasicParam()->GetSideNameByID(station, sideno);
		if(p)
			sidename=p;
		else
			sidename="?G";

		if(sideno2!=sideno)
		{
			sidename = sidename+"|";
			const char* p=GetTGBasicParam()->GetSideNameByID(station, sideno2);
			if(p)
				sidename+=p;
			else
				sidename+="?G";
		}

		tmp.Format("<side>%s</side>",sidename);
		xml += tmp;

		xml+="</RECORD>";
	}
	xml+="</SIDELIST>";

	m_sideGrid.DeleteNonFixedRows();
	m_sideGrid.DisplaySideInfo(xml, "SIDELIST/RECORD");
}

bool DlgBatchSideModify::ModifySide(int rec_index, int station, int sideno)
{
	CTG_TrainRecord *prec=m_train.GetTrainRecordByRecIndex(rec_index);
	if(prec==NULL)
		return false;
	if(prec->GetStationNo()!=station)
		return false;

	GpcLogTrainMessage(0, 0, "调度员修改:%s次%s站: (%d,%d)-->(%d,%d)", 
		m_train.GetTrainID(), gpcdef.GetStationName(station),
		prec->m_data.arrive_side, prec->m_data.depart_side, sideno, sideno);
	
	prec->m_data.arrive_side = sideno;
	prec->m_data.depart_side = sideno;
	return true;
}

void DlgBatchSideModify::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_pDoc!=NULL)
	{
		GpcLogTrainMessage(0, 0, "调度员点击确定");
		CStringArray ary_list;
		m_pDoc->CheckTrainSideInfo(m_train, ary_list);
		if(ary_list.GetCount()>0)
		{
			GpcLogTrainMessage(0, 0, "存在如下报警:");
			CString str;
			for(int n=0; n<ary_list.GetCount(); n++)
			{
				str += ary_list.GetAt(n);
				GpcLogTrainMessage(0, 0, "%s", ary_list.GetAt(n)); 
				str += "\n";
			}
			if(MessageBox(str, "报警", MB_YESNO|MB_ICONHAND)==IDYES)
			{
				GpcLogTrainMessage(0, 0, "调度员选择不理会报警");
				OnOK();
			}
			else
			{
				GpcLogTrainMessage(0, 0, "调度员选择返回修改"); 
			}
		}
		else
		{
			OnOK();
		}
	}
	else
	{
		OnOK();
	}
}
