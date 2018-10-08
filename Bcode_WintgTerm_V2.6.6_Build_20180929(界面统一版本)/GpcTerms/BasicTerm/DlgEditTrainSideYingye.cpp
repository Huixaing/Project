// DlgTrainRecord.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "DlgEditTrainSideYingye.h"
#include "tgpspdata.h"
#include "tg_basicpara.h"
#include "tg_schedule_base.h"
#include "sysshare.h"
#include "function.h"
#include "GridCtrl_src/GridCellCheck.h"
#include "gridctrl_src/GridCellCombo.h"
#include "tgdocument.h"
#include <vector>
#include <map>
#include <algorithm> 
#include ".\DlgEditTrainSideYingye.h"
#include "inifile.h"

#import  <msxml.dll> named_guids
using namespace MSXML;
#define   UPDATE_SIDE WM_USER+1
#define   XLS_STA_SIDE_MAX	1024

// CDlgEditTrainSideYingye 对话框
CDlgEditTrainSideYingye::CDlgEditTrainSideYingye(CWnd* pParent) : CDialog(CDlgEditTrainSideYingye::IDD,pParent)
{
	station_count=0;
	memset(station_list, 0, sizeof(station_list));
}

CDlgEditTrainSideYingye::~CDlgEditTrainSideYingye()
{	
	DeleteXlsData();
}

void CDlgEditTrainSideYingye::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_STTM2, mStationCombo);

	DDX_Control(pDX, IDC_STATIC_XLS, m_xls);
}

BEGIN_MESSAGE_MAP(CDlgEditTrainSideYingye, CDialog)
	ON_WM_CLOSE()
	
	ON_CBN_SELCHANGE(IDC_COMBO_STTM2, OnCbnSelchangeComboStation)
	
	
	ON_BN_CLICKED(IDC_BUTTON_MODIFY, OnBnClickedButtonModify)
	ON_BN_CLICKED(IDC_BUTTON_CHOOSEEXCEL, OnBnClickedButtonChooseexcel)
END_MESSAGE_MAP()

void CDlgEditTrainSideYingye::SetDocument(CTgDocument *pdoc)
{
	m_pDoc=pdoc;
	m_pTrainDataBase=m_pDoc->GetScheduleData(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV);
}

BOOL CDlgEditTrainSideYingye::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect rect(0,0,0,0);
	m_xls.GetWindowRect(&rect);
	this->ScreenToClient(&rect);
	m_gridxls.CreateGrid(this,rect,IDC_STATIC_XLS);

	CFont* pFont=this->GetFont(); 
	m_gridxls.SetFont(pFont,1);
	m_gridxls.SetModified(TRUE);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDlgEditTrainSideYingye::InitDlgCtrlItem()
{
	if(mStationCombo.GetCount()<1)
	{
		CTG_ScheduleInfo schd;
		m_pTrainDataBase->GetScheduleInfo(schd);
		memset(station_list, 0, sizeof(station_list));
		int nRet=gpcdef.GetTGStationListForCheck(schd.m_nSectionID,sizeof(station_list)/sizeof(station_list[0]),station_list);
		if(nRet<0)
		{
			station_count=0;
			AfxMessageBox("获取车站时,车站数组容量太小,不能获取完整车站!");
		}
		else
		{
			station_count=nRet;
			mStationCombo.ResetContent(); // 重复增加
			for(int i=0; i<station_count; i++)
			{
				CString str=gpcdef.GetStationName(station_list[i]);
				int idx=mStationCombo.AddString(str);
				mStationCombo.SetItemData(idx, station_list[i]);
			}
			
			m_nOldSelSta = -1;
			mStationCombo.SetCurSel(m_nOldSelSta);
		}
	}
}

void CDlgEditTrainSideYingye::OnCbnSelchangeComboStation()//选站
{
	int nSel=mStationCombo.GetCurSel(); //Comments:获得当前站的编号 从0开始
	if(nSel!=m_nOldSelSta)
	{
		m_gridxls.DeleteNonFixedRows();
		ShowXlsData(nSel);
		m_nOldSelSta = nSel;
	}
}

void  CDlgEditTrainSideYingye::OnClose()
{
	this->ShowWindow(SW_HIDE);
}
void  CDlgEditTrainSideYingye::OnCancel()
{
	OnClose();
}

BOOL  CXlsListGridView::CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid)
{
	m_record_col_count=0;
	strcpy(m_train_record_def[m_record_col_count].caption,"顺号");
	sprintf(m_train_record_def[m_record_col_count].attr_name,"no");
	m_train_record_def[m_record_col_count].data_type=CTC_DATA_STR;
	m_train_record_def[m_record_col_count].flag=0;
	m_record_col_count++;

	strcpy(m_train_record_def[m_record_col_count].caption,"车次");
	sprintf(m_train_record_def[m_record_col_count].attr_name,"trainid");
	m_train_record_def[m_record_col_count].data_type=CTC_DATA_STR;
	m_train_record_def[m_record_col_count].flag=0;
	m_record_col_count++;

	strcpy(m_train_record_def[m_record_col_count].caption,"营业");
	sprintf(m_train_record_def[m_record_col_count].attr_name,"yingye");
	m_train_record_def[m_record_col_count].data_type=CTC_DATA_STR;
	m_train_record_def[m_record_col_count].flag=0;
	m_record_col_count++;

	strcpy(m_train_record_def[m_record_col_count].caption,"股道");
	sprintf(m_train_record_def[m_record_col_count].attr_name,"side_name");
	m_train_record_def[m_record_col_count].data_type=CTC_DATA_STR;
	m_train_record_def[m_record_col_count].flag=0;
	m_record_col_count++;

	strcpy(m_train_record_def[m_record_col_count].caption,"日期");
	sprintf(m_train_record_def[m_record_col_count].attr_name,"date");
	m_train_record_def[m_record_col_count].data_type=CTC_DATA_STR;
	m_train_record_def[m_record_col_count].flag=0;
	m_record_col_count++;

	strcpy(m_train_record_def[m_record_col_count].caption,"CTC股道");
	sprintf(m_train_record_def[m_record_col_count].attr_name,"arrive_tm");
	m_train_record_def[m_record_col_count].data_type=CTC_DATA_STR;
	m_train_record_def[m_record_col_count].flag=30;
	m_record_col_count++;

	strcpy(m_train_record_def[m_record_col_count].caption,"结果");
	sprintf(m_train_record_def[m_record_col_count].attr_name,"result");
	m_train_record_def[m_record_col_count].data_type=CTC_DATA_STR;
	m_train_record_def[m_record_col_count].flag=0;
	m_record_col_count++;

	CXMLDataGrid::CreateGrid(parent,rect,ctrlid,m_train_record_def,m_record_col_count);

	SetColumnWidth(0, 50);
	SetColumnWidth(1, 85);
	SetColumnWidth(2, 65);
	SetColumnWidth(3, 65);
	SetColumnWidth(4, 200);
	SetColumnWidth(5, 65);
	SetColumnWidth(6, 80);
	
	return TRUE;
}

void  CXlsListGridView::BeginEditCell(int row,int col)
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

void  CXlsListGridView::EndEditCell(int row,int col)
{
	SetCellType(row ,col,RUNTIME_CLASS(CGridCell));
	if(m_train_record_def[col].flag==30) // 股道
	{
		int station=GetCell(row,1)->GetData();

		CGridCellCombo *pCell=(CGridCellCombo *)GetCell(row,col);
		int sideid=GetTGBasicParam()->GetSideIDByName(station,pCell->GetText());
		if(IS_VALID_SIDE(sideid))
		{
			GetCell(row,col)->SetData(sideid);
			Refresh();
			return;
		}
		AfxMessageBox("股道修改失败!");
	}
	
	CXMLDataGrid::EndEditCell(row,col);
}

//选择Excel文件
void CDlgEditTrainSideYingye::OnBnClickedButtonModify()
{
	// TODO: 在此添加控件通知处理程序代码
	int nSel=mStationCombo.GetCurSel(); //Comments:获得当前站的编号 从0开始
	if(nSel<0)
	{
		MessageBox("请选择要查看的车站!","提示",MB_OK);
		return;
	}

	int nStation=mStationCombo.GetItemData(nSel);//Comments:获得站号

	//map<string, string> mapSideChar;
	//CString strCharMap;
	//strCharMap.Format("%s\\CharMap.cfg",gpcdef.GetDataDir());
	//CIniFile t;
	//t.OpenFile(strCharMap);
	//if(t.IsOK())
	//{
	//	if(t.GotoSection("SIDE_NUMBER"))
	//	{
	//		char key[32];
	//		char content[256];
	//		while(t.GetContent(key,	content,sizeof(content)-1))
	//		{
	//			mapSideChar.insert(std::pair<string, string>(key, content));
	//		}
	//	}
	//}

	for (int i=1; i<m_gridxls.GetRowCount(); i++)
	{
		CString train_name=m_gridxls.GetItemText(i,1);
		CString stryingye=m_gridxls.GetItemText(i,2);
		CString str=m_gridxls.GetItemText(i,3);

		//map<string, string>::const_iterator iterMap = mapSideChar.find((LPCTSTR)str);
		//if(iterMap!=mapSideChar.end())
		//	str = iterMap->second.c_str();

		str+="G";
		int sideid=GetTGBasicParam()->GetSideIDByName(nStation,str);
		if(sideid>0 && sideid<255)
		{
			const char* p=GetTGBasicParam()->GetSideNameByID(nStation,sideid);
			if(p)
			{
				m_gridxls.SetItemText(i,5,p);
				CString strResult=ModifyStationSide(train_name, stryingye, nStation, sideid, i);
				m_gridxls.SetItemText(i,6,strResult);
			}
		}
	}
	m_gridxls.Refresh();
}

void GetTrainName(CString trainid, CStringArray& list)
{
	int pos=trainid.Find('/',0);
	if(pos==-1)
	{
		list.Add(trainid);
	}
	else if(pos>0)
	{
		CString trainid1 = trainid.Left(pos);
		trainid = trainid.Right(trainid.GetLength()-pos-1);
		pos=trainid.Find('/',0);
		if(pos==-1)
		{
			CString trainid2=trainid;
			if(trainid2.GetLength()<trainid1.GetLength())
			{
				CString str=trainid1.Left(trainid1.GetLength()-trainid2.GetLength());
				str += trainid2;
				trainid2 = str;
			}
			else if(trainid2.GetLength()>trainid1.GetLength())
			{
				trainid2="";
			}

			list.Add(trainid1);
			if(trainid2 != trainid1 && trainid2 !="")
			{
				list.Add(trainid2);
			}
		}
		else
		{
			CString trainid2 = trainid.Left(pos);
			if(trainid2.GetLength()<trainid1.GetLength())
			{
				CString str=trainid1.Left(trainid1.GetLength()-trainid2.GetLength());
				str += trainid2;
				trainid2 = str;
			}
			else if(trainid2.GetLength()>trainid1.GetLength())
			{
				trainid2="";
			}
			
			list.Add(trainid1);
			if(trainid2 != trainid1 && trainid2 !="")
			{
				list.Add(trainid2);
			}

			trainid = trainid.Right(trainid.GetLength()-pos-1);
			pos=trainid.Find('/',0);
			if(pos==-1)
			{
				CString trainid3 = trainid;
				if(trainid3.GetLength()<trainid1.GetLength())
				{
					CString str=trainid1.Left(trainid1.GetLength()-trainid3.GetLength());
					str += trainid3;
					trainid3 = str;
				}
				else if(trainid3.GetLength()>trainid1.GetLength())
				{
					trainid3="";
				}

				if(trainid3 != trainid1 && trainid3 != trainid2 && trainid3 !="")
				{
					list.Add(trainid3);
				}
			}
		}
	}
}

bool IsMatchTrainNo(CTG_TrainRecord *prec, CStringArray& train_list)
{
	bool bMatch=false;
	if(prec != NULL)
	{
		for(int n=0; n<train_list.GetCount(); n++)
		{
			CString trainname=train_list.GetAt(n);
			if(trainname==prec->GetArriveTID() || trainname==prec->GetArriveTID())
			{
				bMatch = true;
				break;
			}
		}
	}

	return bMatch;
}

CString CDlgEditTrainSideYingye::ModifyStationSide(CString trainname1, CString stryingye, int station, BYTE sideno, int row)
{
	CString str="修改失败";
	CTG_TrainPtrGroup group;
	m_pTrainDataBase->FindTrains(group,station);

	CTG_ScheduleInfo schd_info;
	m_pTrainDataBase->GetScheduleInfo(schd_info);
	CStringArray train_list;
	GetTrainName(trainname1, train_list);
	for(int i=0;i<group.GetTrainCount();i++)
	{
		CTG_SingleTrain *ptrain=group.GetTrainPtr(i);
		if(ptrain==NULL)
			continue;

		int count=0;
		for(int n=0; n<ptrain->GetTrainRecordCount(); n++)
		{
			CTG_TrainRecord *prec=ptrain->GetTrainRecordByRecIndex(n);
			if(prec==NULL) continue;

			if(prec->GetStationNo()==station)
			{
				if(IsMatchTrainNo(prec, train_list))
				{
					CTG_SingleTrain train=*ptrain;
					CTG_TrainRecord *prec=train.GetTrainRecordByRecIndex(n);
					prec->m_data.arrive_side=sideno;
					prec->m_data.depart_side=sideno;
					str = "修改成功";

					if(stryingye=="△")
					{
						TG_ClearTechStopTrain(prec->m_data);
					}
					else
					{

						TG_SetTechStopTrain(prec->m_data);
					}
					m_pTrainDataBase->UpdateTrain(schd_info, train, SYCFLAG_NULL);
					break;
				}
			}
		}

		if(str == "修改成功")
		{
			break;
		}	
	}

	if(str == "修改成功")
	{
		m_gridxls.SetItemFgColour(row, 6, ::GetSysColor(COLOR_WINDOWTEXT));
	}
	else
	{
		m_gridxls.SetItemFgColour(row, 6, RGB(255,0,0));
	}

	return str;
}

void CDlgEditTrainSideYingye::OnBnClickedButtonChooseexcel()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog file(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"EXCEL文件(*.xls;*.xlsx)|*.xls;*.xlsx||",AfxGetMainWnd());
	if(file.DoModal() == IDOK)
	{
		m_gridxls.DeleteNonFixedRows();
		DeleteXlsData();

		if(LoadXlsFile(file.GetPathName()))
		{
			ShowXlsData(mStationCombo.GetCurSel());
			CString str("\"");
			vector<LPSTAINFO>::const_iterator iter = m_vecStaInfo.begin();
			for (; iter!=m_vecStaInfo.end(); iter++)
			{
				str += CString((*iter)->name) + ",";
			}
			str.TrimRight(',');
			str+=("\"");
			str += "成功加载";
			AfxMessageBox(str);
		}
	}
}

BOOL CDlgEditTrainSideYingye::LoadXlsFile(const char* lpstrfilename)
{
	CApplication app;
	CWorkbooks books;
	CWorkbook book;
	CWorksheets sheets;
	CWorksheet sheet;

	LPDISPATCH lpDisp;
	COleVariant covOptional((long)DISP_E_PARAMNOTFOUND,VT_ERROR);

	if(!app.CreateDispatch(_T("Excel.Application")))
	{
		AfxMessageBox(_T("无法创建Excel应用！"));
		return FALSE;
	}

	books.AttachDispatch(app.get_Workbooks());
	lpDisp = books.Open(lpstrfilename,covOptional, covOptional,covOptional,covOptional,covOptional,covOptional,covOptional,
				covOptional,covOptional,covOptional,covOptional,covOptional,covOptional,covOptional);

	//得到Workbook
	book.AttachDispatch(lpDisp);
	//得到Worksheets
	sheets.AttachDispatch(book.get_Worksheets());

	//得到当前活跃sheet,若有单元格处于编辑状态，则次操作不能返回，会一直等待
	sheet.AttachDispatch( book.get_ActiveSheet());

	//获得行数与列数
	CRange usedRange;
	CRange mRange;

	usedRange.AttachDispatch(sheet.get_UsedRange());

	lpDisp = usedRange.get_Rows();
	mRange.AttachDispatch(lpDisp,true);
	int row_count = mRange.get_Count();


	lpDisp = usedRange.get_Columns();
	mRange.AttachDispatch(lpDisp,true);
	int col_count = mRange.get_Count();


	LoadXlsCell(mRange, usedRange, col_count, row_count);

	book.Close(covOptional,COleVariant(lpstrfilename),covOptional);
	books.Close();
	app.Quit();
	usedRange.ReleaseDispatch();
	mRange.ReleaseDispatch();
	sheet.ReleaseDispatch();
	sheets.ReleaseDispatch();
	book.ReleaseDispatch();
	books.ReleaseDispatch();
	app.ReleaseDispatch();	


	return m_vecStaInfo.size()>0;
}

void CDlgEditTrainSideYingye::LoadXlsCell(CRange &mRange, CRange &usedRange, int nW, int nH)
{
	map<int, XLSDATTYPE> mapXlsDatPos;
	vector<STATIONCELL>   mapXlsStPos;

	int nBadDat=0;
	CString strIndex, strTrainID;
	COleVariant vResult;
	for (int i=2; i<=nH; i++)
	{
		for (int j=1; j<=nW; j++)
		{
			mRange.AttachDispatch(usedRange.get_Item(COleVariant((long)i),COleVariant((long)j)).pdispVal);//获取第i行j列的数据
			vResult = mRange.get_Value2();
			if(vResult.vt == VT_BSTR)
			{
				CString strVal(vResult.bstrVal);
				strVal.Trim();
				if(2==i || 3==i)
				{
					XLSDATTYPE xlsType=GetCellDataType(strVal);
					if(2==i)
					{
						if(CDlgEditTrainSideYingye::INDEX==xlsType)
						{
							mapXlsDatPos.insert(std::pair<int, XLSDATTYPE>(j, xlsType));
						}
						else if(CDlgEditTrainSideYingye::STATION!=xlsType)
						{
							vector<STATIONCELL>::const_iterator iterSta=mapXlsStPos.begin(); 
							for (; iterSta!=mapXlsStPos.end(); iterSta++)
							{
								if(strVal.Compare(iterSta->name)==0)
									break;
							}
							if(iterSta==mapXlsStPos.end())
							{
								STATIONCELL st;
								memset(&st, 0, sizeof(STATIONCELL));
								strcpy(st.name, (LPCTSTR)strVal);
								st.pos = j;
								mapXlsStPos.push_back(st);
							}
							else
							{
								strIndex = strVal;
								nBadDat=1;
								break;
							}
						}
					}
					else if(3==i && CDlgEditTrainSideYingye::UNKOWN!=xlsType)
					{
						mapXlsDatPos.insert(std::pair<int, XLSDATTYPE>(j, xlsType));
					}
				}
				else 
				{
					if(CellInTrainID(mapXlsDatPos, j))
					{
						strTrainID = strVal;
					}
					else
					{
						map<int, XLSDATTYPE>::const_iterator iter = mapXlsDatPos.find(j);
						if(iter!=mapXlsDatPos.end() && iter->first==j && CDlgEditTrainSideYingye::INDEX==iter->second)
							strIndex = strVal;

						ParseXlsCell(strIndex, strTrainID, mapXlsDatPos, mapXlsStPos, strVal, j);					
					}
				}
			}
			else if(vResult.vt == VT_R8)
			{
				map<int, XLSDATTYPE>::const_iterator iter = mapXlsDatPos.find(j);
				if(iter!=mapXlsDatPos.end())
				{
					if(iter->first==j)
					{
						BOOL bIsString=FALSE;
						if(CDlgEditTrainSideYingye::INDEX==iter->second || CDlgEditTrainSideYingye::SIDE==iter->second || CDlgEditTrainSideYingye::TRAINID==iter->second)
							bIsString=TRUE;

						if(bIsString)
						{
							CString strVal;
							strVal.Format("%d", int(vResult.dblVal));
							if(CDlgEditTrainSideYingye::INDEX==iter->second)
								strIndex = strVal;
							else if(CDlgEditTrainSideYingye::TRAINID==iter->second)
								strTrainID = strVal;
							ParseXlsCell(strIndex, strTrainID, mapXlsDatPos, mapXlsStPos, strVal, j);
						}
					}
				}//end mapXlsDatPos
			}//end vResult
		}// end for nH
		if(nBadDat>0)
			break;
	}// end for nW

	if(1==nBadDat)
	{
		strTrainID.Format("有重名车站\"%s\"，请修改后重新录入", strIndex);
		AfxMessageBox(strTrainID);
		DeleteXlsData();
	}
}

void CDlgEditTrainSideYingye::ParseXlsCell(LPCTSTR strIndex, LPCTSTR strTrainID, const map<int, XLSDATTYPE> &cellType, vector<STATIONCELL> &cellSt, LPCTSTR strCell, int col)
{
	if(strlen(strTrainID)<1)
		return;

	const STATIONCELL* const lpStCell = CellInStation(cellSt, col);
	if(NULL==lpStCell)
		return;

	BOOL bNewNode=FALSE;
	LPSTAINFO lpInfo= StationInInfo(lpStCell->name);
	if(NULL==lpInfo)
	{
		LPSTAINFO lpOne = new STAINFO;
		memset(lpOne, 0, sizeof(STAINFO));
		strcpy(lpOne->name, lpStCell->name);
		lpOne->count = 0;
		lpOne->detail = new STASIDE[XLS_STA_SIDE_MAX];
		memset(lpOne->detail, 0, sizeof(STASIDE)*XLS_STA_SIDE_MAX);
		lpInfo = lpOne;
		bNewNode=TRUE;
	}

	map<int, XLSDATTYPE>::const_iterator iter = cellType.begin();
	for (; iter!=cellType.end(); iter++)
	{
		if(iter->first==col && iter->second == CDlgEditTrainSideYingye::SIDE)
		{
			strcpy(lpInfo->detail[lpInfo->count].xlsSide, strCell);
			break;
		}
		else if(iter->first==col && iter->second == CDlgEditTrainSideYingye::YINGYE)
		{
			strcpy(lpInfo->detail[lpInfo->count].xlsYingye, strCell);
			break;
		}
		else if(iter->first==col && iter->second == CDlgEditTrainSideYingye::DATE)
		{
			strcpy(lpInfo->detail[lpInfo->count].xlsIndex, strIndex);
			strcpy(lpInfo->detail[lpInfo->count].xlsTrainId, strTrainID);
			strcpy(lpInfo->detail[lpInfo->count].xlsDate, strCell);
			lpInfo->count++;
			break;
		}
	}
	if(bNewNode)
		m_vecStaInfo.push_back(lpInfo);
}


const CDlgEditTrainSideYingye::LPSTATIONCELL CDlgEditTrainSideYingye::CellInStation(const vector<STATIONCELL> &cellSt, int col)const
{
	vector<STATIONCELL>::const_iterator iter = cellSt.begin();
	for (; iter!=cellSt.end(); iter++)
	{
		if(col>=iter->pos && col<iter->pos+3)
			return const_cast<LPSTATIONCELL>(&(*iter));
	}
	return NULL;
}

BOOL CDlgEditTrainSideYingye::CellInTrainID(const map<int, XLSDATTYPE> &cellTrainID, int col)const
{
	map<int, XLSDATTYPE>::const_iterator iter = cellTrainID.begin();
	for (; iter!=cellTrainID.end(); iter++)
	{
		if(iter->first==col && iter->second == CDlgEditTrainSideYingye::TRAINID)
			return TRUE;
	}
	return FALSE;
}

CDlgEditTrainSideYingye::LPSTAINFO CDlgEditTrainSideYingye::StationInInfo(const char *szName)const
{
	vector<LPSTAINFO>::const_iterator iter = m_vecStaInfo.begin();
	for (; iter!=m_vecStaInfo.end(); iter++)
	{
		if(strcmp((*iter)->name, szName)==0)
			return const_cast<LPSTAINFO>(*iter);
	}
	return NULL;
}

CDlgEditTrainSideYingye::XLSDATTYPE CDlgEditTrainSideYingye::GetCellDataType(CString &strCell)const
{
	int nLenStr = strCell.GetLength();
	if(nLenStr>4)
	{
		int nReal=0;
		const char * const plpsz = (LPCTSTR)strCell;
		char * const lpszTemp = new char[nLenStr+1];
		memset(lpszTemp, 0, sizeof(char)*(nLenStr+1));
		for (int i=0; i<nLenStr; i++)
		{
			if(plpsz[i]=='\r' || plpsz[i]=='\n' || plpsz[i]==' ')
				continue;
			else
			{
				strncat(lpszTemp+nReal, plpsz+i, 1);
				nReal++;
			}
		}
		strCell.Format("%s", lpszTemp);
		delete[] lpszTemp;
	}

	if(strCell.Compare("顺号")==0)
		return CDlgEditTrainSideYingye::INDEX;
	if(strCell.Compare("车站")==0)
		return CDlgEditTrainSideYingye::STATION;
	if(strCell.Compare("车次")==0)
		return CDlgEditTrainSideYingye::TRAINID;
	else if(strCell.Compare("股道")==0)
		return CDlgEditTrainSideYingye::SIDE;
	else if(strCell.Compare("业务")==0)
		return CDlgEditTrainSideYingye::YINGYE;
	else if(strCell.Compare("日期")==0)
		return CDlgEditTrainSideYingye::DATE;

	return CDlgEditTrainSideYingye::UNKOWN;
}

void CDlgEditTrainSideYingye::DeleteXlsData()
{
	vector<LPSTAINFO>::iterator iter = m_vecStaInfo.begin();
	for (; iter!=m_vecStaInfo.end(); iter++)
	{
		delete[] (*iter)->detail;
		delete[] const_cast<LPSTAINFO>(*iter);
	}
	m_vecStaInfo.clear();
}

void CDlgEditTrainSideYingye::ShowXlsData(int nStaSel)
{
	CString strStaName;
	LPSTAINFO lpInfo=NULL;
	if(nStaSel<0)
	{
		int i=0;
		for (; i<mStationCombo.GetCount(); i++)
		{
			mStationCombo.GetLBText(i, strStaName);
			lpInfo = StationInInfo(strStaName);
			if(lpInfo)
				break;
		}
		if(lpInfo)
			mStationCombo.SetCurSel(i);
	}
	else
	{
		mStationCombo.GetLBText(nStaSel, strStaName);
		lpInfo = StationInInfo(strStaName);
	}

	if(lpInfo)
	{
		m_gridxls.SetRowCount(lpInfo->count+1);
		for (int i=0; i<lpInfo->count; i++)
		{
			m_gridxls.SetItemText(i+1,0,lpInfo->detail[i].xlsIndex);
			m_gridxls.SetItemText(i+1,1,lpInfo->detail[i].xlsTrainId);
			m_gridxls.SetItemText(i+1,2,lpInfo->detail[i].xlsYingye);
			m_gridxls.SetItemText(i+1,3,lpInfo->detail[i].xlsSide);
			m_gridxls.SetItemText(i+1,4,lpInfo->detail[i].xlsDate);
		}
		m_gridxls.ExpandColumnsToFit();
	}
	m_gridxls.Refresh();
}
