
#include "stdafx.h"
#include "resource.h"
#include "sysshare.h"
#include "function.h"
#include "GridCtrl_src/GridCellCheck.h"
#include "gridctrl_src/GridCellCombo.h"
#include "SchduleCompare.h"

///////////////////////////////////////////////////
void CFindSchduleGrid::OnSelRowCol(int nRow, int nCol) 
{
	if(nRow>=1 && nRow<GetRowCount())
	{
		TRAIN_INDEX train_index=GetCell(nRow,0)->GetData();
		::SendMessage(m_hViewWnd, WM_USER + 2001, train_index, 0);
	}
}

BOOL CFindSchduleGrid::InsertRows(const char *xml,const char *rootnode)
{
	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		AfxMessageBox(er.ErrorMessage());
		return FALSE;
	}

	if(VARIANT_TRUE != pXMLDoc->loadXML( _bstr_t( xml ) ) )
	{
		return FALSE;
	} 

	MSXML::IXMLDOMNodeListPtr pRecAbsList = pXMLDoc->selectNodes(rootnode);//ds_MaintainPlan_List/
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

			Item.strText=GetNodeStrValue(pRecAbsList->Getitem(i),attr);
			SetItem(&Item);
		}

		TRAIN_INDEX train_index=0;
		train_index=GetNodeIntValue(pRecAbsList->Getitem(i),"train_index");
		GetCell(row,0)->SetData(train_index);
	}
	Refresh();
	return TRUE;
}

CString GetNodeStrValue(MSXML::IXMLDOMNodePtr pRoot, CString node)
{
	CString dest;
	MSXML::IXMLDOMNodePtr pNode = pRoot->selectSingleNode(_bstr_t(node));
	if(pNode)
	{
		_variant_t v;
		v = pNode->GetnodeTypedValue();
		dest = CString((v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	return dest;
}

int GetNodeIntValue(MSXML::IXMLDOMNodePtr pRoot, CString node)
{
	int dest=0;
	MSXML::IXMLDOMNodePtr pNode = pRoot->selectSingleNode(_bstr_t(node));
	if(pNode)
	{
		_variant_t v;
		v = pNode->GetnodeTypedValue();
		dest = atoi(CString((v.bstrVal)));
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	return dest;
}

/////////////////////////////////////////////////////////////////////////
BOOL CStationSelGrid::CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid)
{
	m_result_col_count=0;
	strcpy(m_result_def[m_result_col_count].caption,"车站");
	sprintf(m_result_def[m_result_col_count].attr_name,"station");
	m_result_def[m_result_col_count].data_type=CTC_DATA_STR;
	m_result_def[m_result_col_count].flag=0;
	m_result_col_count++;

	strcpy(m_result_def[m_result_col_count].caption,"选中");
	sprintf(m_result_def[m_result_col_count].attr_name,"select");
	m_result_def[m_result_col_count].data_type=CTC_DATA_STR;
	m_result_def[m_result_col_count].flag=2;
	m_result_col_count++;

	CXMLDataGrid::CreateGrid(parent,rect,ctrlid,m_result_def,m_result_col_count);

	SetColumnWidth(0, 100);
	SetColumnWidth(1, 50);
	return TRUE;
}

void  CStationSelGrid::AddRow(int station, int iSel)
{
	int row=InsertRow("");
	if(row<0)
		return;
	CString sta_name=gpcdef.GetStationName(station);
	
	GV_ITEM Item;
	Item.mask = GVIF_TEXT|GVIF_FORMAT|GVIF_PARAM;
	Item.row = row;
	Item.col = 0;
	if(m_bSingleLine)
		Item.nFormat = DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX;
	else
		Item.nFormat = DT_LEFT|DT_VCENTER|DT_NOPREFIX;
	Item.lParam  = 0;
	Item.strText = sta_name;
	SetItem(&Item);

	Item.col = 1;
	Item.strText = "";
	SetItem(&Item);

	SetCellType(row ,1, RUNTIME_CLASS(CGridCellCheck));
	CGridCellCheck* pCheck=(CGridCellCheck*)GetCell(row,1);
	if(pCheck)
	{
		pCheck->SetData(station);
		if(iSel==1)
			pCheck->SetCheck(TRUE);
		else
			pCheck->SetCheck(FALSE);
	}
}

int CStationSelGrid::GetSelectStation(int station_list[])
{
	int count = 0;
	for(int row=1;row<GetRowCount();row++ )
	{
		int station = 0;
		CGridCellCheck *pCell = (CGridCellCheck *)GetCell(row,1);
		if(pCell)
		{
			if(pCell->GetCheck()==FALSE)
				station=0;
			else
				station=pCell->GetData();
		}
		if(station>0)
		{
			station_list[count]=station;
			count++;
		}
	}

	return count;
}

ResultList::ResultList()
{
	clear();
}

ResultList::~ResultList()
{
	clear();
}

void ResultList::clear()
{
	station=0;
	stationName="";
	list_trains.clear();
}

ResultListForWork::ResultListForWork()
{
	clear();
}

ResultListForWork::~ResultListForWork()
{
	clear();
}

void ResultListForWork::clear()
{
	station=0;
	stationName="";
	list_trains.clear();
}



CString GetTimeStr(TIME tm1)
{
	CString str;
	str.Format("%02d:%02d", (tm1/3600)%24, (tm1/60)%60);
	return str;
}

CString GetDateTimeStr(TIME tm1)
{
	CTime ctm(tm1);
	return ctm.Format("%d日%H:%M");
}

CString GetStationNeedAlarm(DWORD side_valide)
{
	CString ss="";
	if(0!=side_valide)
	{
		if(IsPassNotMainSide(side_valide))
			ss+=",侧线通过";
		if(IsSideDisconnectEntry(side_valide))
			ss+=",股道和出入口不通";
		if(IsSideNotSame(side_valide))
			ss+=",接发股道不同";
		if(IsNotHighPlatform(side_valide))
			ss+=",非高站台";
		if(IsNoPlatform(side_valide))
			ss+=",无站台";
		if(IsNotStopThisClassTrain(side_valide))
			ss+=",不能接发此种车";
		if(IsNotStopCRHTrain(side_valide))
			ss+=",不能接动车组";
		if(IsNotGiveSide(side_valide))
			ss+=",无股道";
		if(IsNotSpaceTrain(side_valide))
			ss+=",不能接超限车";
		if(IsNotCheckSide(side_valide))
			ss+=",站细获取失败,无法检查股道";
	}
	return ss;
}

CString GetSideName(int station, BYTE arrive_side, BYTE depart_side)
{
	CString str="";
	const CStationDef* pSideDef = GetTGBasicParam()->GetStationDef(station);
	if(pSideDef!=NULL)
	{
		SIDE_NAME arrive_side_name, depart_side_name;
		memset(arrive_side_name, 0, sizeof(arrive_side_name));
		memset(depart_side_name, 0, sizeof(depart_side_name));
		if(arrive_side==depart_side)
		{
			pSideDef->GetSideName(arrive_side, arrive_side_name);
			str=arrive_side_name;
		}
		else
		{
			pSideDef->GetSideName(arrive_side, arrive_side_name);
			pSideDef->GetSideName(depart_side, depart_side_name);
			str=arrive_side_name;
			str+="-";
			str+=depart_side_name;
		}
	}

	return str;
}

DWORD CheckSideInfo(CTG_SingleTrain* ptrain, CTG_TrainRecord* prec, int rec_index)
{
	DWORD side_valide=0;
	const CStationDef* pSideDef = GetTGBasicParam()->GetStationDef(prec->m_data.station);
	if(pSideDef==NULL)
	{
		SetNotCheckSide(side_valide);
	}
	else
	{
		if(IS_VALID_SIDE(prec->m_data.arrive_side) && IS_VALID_SIDE(prec->m_data.depart_side))
		{
			if (GetTGBasicParam()->IsTrainPassNotUseMainSide(prec->m_data))
			{
				SetPassNotMainSide(side_valide);
			}
			
			BYTE nTrainType = ptrain->IsPassengerTrain() ? CStationDef::RECV_PASSENGER : CStationDef::RECV_CARGO;
			bool bCheck=true;
			if(rec_index==0 && prec->IsStartRec())
			{
				if(!pSideDef->isReceiveTrain(prec->m_data.depart_side, nTrainType) )
				{
					SetNotStopThisClassTrain(side_valide);
				}
				if (GetTGBasicParam()->IsTrainSideNotConnectWithEntry(prec->m_data.station, prec->m_data.depart_side, prec->m_data.exit))
				{
					SetSideDisconnectEntry(side_valide);
				}
				if(!pSideDef->isReceiveExceedTrain(prec->m_data.depart_side, ptrain->GetTrainInfo()->m_data.space_rank))
				{
					SetNotSpaceTrain(side_valide);
				}
				bCheck=false;
			}
			if(rec_index==ptrain->GetTrainRecordCount()-1 && prec->IsTerminalRec())
			{
				if(!pSideDef->isReceiveTrain(prec->m_data.arrive_side, nTrainType) )
				{
					SetNotStopThisClassTrain(side_valide);
				}
				if (GetTGBasicParam()->IsTrainSideNotConnectWithEntry(prec->m_data.station, prec->m_data.arrive_side, prec->m_data.entry))
				{
					SetSideDisconnectEntry(side_valide);
				}
				if(!pSideDef->isReceiveExceedTrain(prec->m_data.arrive_side, ptrain->GetTrainInfo()->m_data.space_rank))
				{
					SetNotSpaceTrain(side_valide);
				}
				bCheck=false;
			}

			if(bCheck)
			{
				if(prec->m_data.depart_side==prec->m_data.arrive_side)
				{
					if(!pSideDef->isReceiveTrain(prec->m_data.arrive_side, nTrainType))
					{
						SetNotStopThisClassTrain(side_valide);
					}
					if (GetTGBasicParam()->IsTrainSideNotConnectWithEntry(prec->m_data.station, prec->m_data.arrive_side, prec->m_data.entry))
					{
						SetSideDisconnectEntry(side_valide);
					}
					if (GetTGBasicParam()->IsTrainSideNotConnectWithEntry(prec->m_data.station, prec->m_data.depart_side, prec->m_data.exit))
					{
						SetSideDisconnectEntry(side_valide);
					}
					if(!pSideDef->isReceiveExceedTrain(prec->m_data.arrive_side, ptrain->GetTrainInfo()->m_data.space_rank))
					{
						SetNotSpaceTrain(side_valide);
					}
				}
				else
				{
					SetSideNotSame(side_valide);

					if(!pSideDef->isReceiveTrain(prec->m_data.arrive_side, nTrainType))
					{
						SetNotStopThisClassTrain(side_valide);
					}
					if(!pSideDef->isReceiveTrain(prec->m_data.depart_side, nTrainType) )
					{
						SetNotStopThisClassTrain(side_valide);
					}
					if (GetTGBasicParam()->IsTrainSideNotConnectWithEntry(prec->m_data.station, prec->m_data.arrive_side, prec->m_data.entry))
					{
						SetSideDisconnectEntry(side_valide);
					}
					if (GetTGBasicParam()->IsTrainSideNotConnectWithEntry(prec->m_data.station, prec->m_data.depart_side, prec->m_data.exit))
					{
						SetSideDisconnectEntry(side_valide);
					}

					if(!pSideDef->isReceiveExceedTrain(prec->m_data.arrive_side, ptrain->GetTrainInfo()->m_data.space_rank))
					{
						SetNotSpaceTrain(side_valide);
					}
					if(!pSideDef->isReceiveExceedTrain(prec->m_data.depart_side, ptrain->GetTrainInfo()->m_data.space_rank))
					{
						SetNotSpaceTrain(side_valide);
					}
				}
			}

			if(ptrain->IsPassengerTrain())
			{
				if ( prec->IsStartRec() || prec->IsTerminalRec() || !prec->IsPassRec())
				{				
					// 无站台,不能办理客运作业
					if (!pSideDef->isPlatform(prec->m_data.arrive_side) )
						SetNoPlatform(side_valide);
					else
					{
						// 动车组接入非动车股道，非高站台股道
						if (ptrain->IsCRHTrain())
						{
							if(!GetTGBasicParam()->VerifyNotCRHStopSide(prec->m_data.arrive_side, prec->m_data.station, ptrain->GetTrainInfo()->m_data))
								SetNotStopCRHTrain(side_valide);
							else if(!pSideDef->isHighPlatform(prec->m_data.arrive_side))
								SetNotHighPlatform(side_valide);
						}
					}
				}
			}
		}
		else
		{
			SetNotGiveSide(side_valide);
		}
	}
	return side_valide;
}

void GetNodeRecord(NodeRecord& result, const TG_TrainRecord& data, int m, int count)
{
	result.arrive_side = data.arrive_side;
	result.depart_side = data.depart_side;
	result.entry = data.entry;
	result.exit  = data.exit;
	
	if(m==0 && TG_IsFlagStart(data.flag))
	{
		result.accept = 0;
		result.entry  = 0;
		result.depart = data.depart;
		result.arrive = result.depart;
	}
	else
	{
		result.accept = 1;
		result.arrive = data.arrive;
		result.depart = data.depart;
	}

	if(((m+1)==count) && TG_IsFlagTerminal(data.flag))
	{
		result.giveout = 0;
		result.exit    = 0;
		result.arrive = data.arrive;
		result.depart = result.arrive;
	}
	else
	{
		result.giveout = 1;
		result.arrive  = data.arrive;
		result.depart  = data.depart;
	}

	if(result.arrive==result.depart)
		result.pass_stay=1;
	else
		result.pass_stay=0;
}

CString GetStationPassStayInfo(BYTE accept, BYTE giveout, BYTE pass_stay)
{
	CString str;
	if(accept==0 && giveout==0)
	{
		str.Format("始发,终到");
	}
	else if(accept==0)
	{
		str.Format("始发");
	}
	else if(giveout==0)
	{
		str.Format("终到");
	}
	else
	{
		if(pass_stay==1)
			str.Format("通过");
		else
			str.Format("到开");
	}

	return str;
}