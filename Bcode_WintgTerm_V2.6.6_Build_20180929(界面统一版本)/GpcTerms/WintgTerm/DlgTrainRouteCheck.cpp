
#include "stdafx.h"
#include "resource.h"
#include "tgpspdata.h"
#include "tg_basicpara.h"
#include "tg_schedule_base.h"
#include "sysshare.h"
#include "function.h"
#include "GridCtrl_src/GridCellCheck.h"
#include "gridctrl_src/GridCellCombo.h"
#include "tgdocument.h"
#include "DlgTrainRouteCheck.h"

BOOL  CTrainRouteCheckListGrid::CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid)
{
	m_pTrainDataBase=NULL;
	m_train_col_count=0;
	strcpy(m_train_def[m_train_col_count].caption,"序号");
	sprintf(m_train_def[m_train_col_count].attr_name,"no");
	m_train_def[m_train_col_count].data_type=CTC_DATA_STR;
	m_train_def[m_train_col_count].flag=0;
	m_train_col_count++;

	strcpy(m_train_def[m_train_col_count].caption,"车次号");
	sprintf(m_train_def[m_train_col_count].attr_name,"trainid");
	m_train_def[m_train_col_count].data_type=CTC_DATA_STR;
	m_train_def[m_train_col_count].flag=0;
	m_train_col_count++;

	strcpy(m_train_def[m_train_col_count].caption,"首站");
	sprintf(m_train_def[m_train_col_count].attr_name,"first_station");
	m_train_def[m_train_col_count].data_type=CTC_DATA_STR;
	m_train_def[m_train_col_count].flag=0;
	m_train_col_count++;

	strcpy(m_train_def[m_train_col_count].caption,"时间");
	sprintf(m_train_def[m_train_col_count].attr_name,"first_time");
	m_train_def[m_train_col_count].data_type=CTC_DATA_STR;
	m_train_def[m_train_col_count].flag=0;
	m_train_col_count++;

	strcpy(m_train_def[m_train_col_count].caption,"径路报警");
	sprintf(m_train_def[m_train_col_count].attr_name,"train_valide");
	m_train_def[m_train_col_count].data_type=CTC_DATA_STR;
	m_train_def[m_train_col_count].flag=0;
	m_train_col_count++;

	CXMLDataGrid::CreateGrid(parent,rect,ctrlid,m_train_def,m_train_col_count);

	SetColumnWidth(0, 50);
	SetColumnWidth(1, 60);
	SetColumnWidth(2, 90);
	SetColumnWidth(3, 160);
	SetColumnWidth(4, 280);

	return TRUE;
}

void  CTrainRouteCheckListGrid::SetScheduleBase(CTG_TrainScheduleBase *pTrainDataBase,CTG_TrainScheduleBase *pBasicTrainDataBase)
{
	m_pTrainDataBase=pTrainDataBase;
	m_pBasicTrainDataBase=pBasicTrainDataBase;
}

void CTrainRouteCheckListGrid::ShowTrainDifferent()
{
	for(int i=0;i<mlistTrain.size();i++)
	{
		CGridCellBase *p = GetCell(i+1,0);
		if(p) 
			p->SetData(mlistTrain[i].train_index);
		CGridCellBase *q = GetCell(i+1,1);
		if (q)
			q->SetData(mlistTrain[i].basic_index);
		//GetCell(i+1,1)->SetData(mlistTrain[i].basic_index);
		CGridCell *pCellResult=(CGridCell *)GetCell(i+1,2);
		// 显示列车异常信息
		if(mlistTrain[i].train_valide >0 && pCellResult)
			pCellResult->SetBackClr(RGB(255,100,255));
	}

	return;
}

CString CTrainRouteCheckListGrid::GetTrainResult(long train_index, long basic_index, CString trainid, CString& strStName, CString& strTm, DWORD train_valide)
{
	CString tmp;
	CString trainstr="<RECORD>";

	tmp.Format("<no>%d</no>", mlistTrain.size()+1);
	trainstr +=tmp;

	tmp.Format("<trainid>%s</trainid>", trainid);
	trainstr +=tmp;

	tmp.Format("<first_station>%s</first_station>", strStName);
	trainstr +=tmp;

	tmp.Format("<first_time>%s</first_time>", strTm);
	trainstr +=tmp;

	if(train_valide>0)
	{	
		CString ss;
		if(IsRecordCountDifferent(train_valide))
			ss+="车站个数不同";
		if(IsPathDifferent(train_valide))
			ss+=",经由车站不同";
		if(IsTrainExistDifferent(train_valide))
			ss+="基本图中无此车";
		if(IsEntryDifferent(train_valide))
			ss+=",入口不同";
		if(IsExitDifferent(train_valide))
			ss+=",出口不同";

		tmp.Format("<train_valide>%s;</train_valide>", ss);
		trainstr +=tmp;
	}

	trainstr+="</RECORD>";

	NodeTrain rd;
	rd.train_index=train_index;
	rd.basic_index=basic_index;
	rd.train_valide=train_valide;

	mlistTrain.push_back(rd);

	return trainstr;
}

int  CTrainRouteCheckListGrid::ShowTrainData(CTG_TrainPtrGroup *pGroup, int flag, CTG_ScheduleInfo::SCHEDULE_TYPE schd_type)
{
	mlistTrain.clear();

	CString xml="<TRAINLIST>";
	for(int i=0;i<pGroup->GetTrainCount();i++)
	{
		CTG_SingleTrain *ptrain=pGroup->GetTrainPtr(i);
		if(!ptrain)
			continue;

		// 找到基本图车
		CString trainid;
		trainid = ptrain->GetTrainID();
		CTG_TrainRecord * pfirst=ptrain->GetTrainFirstRecord();
		if(pfirst==NULL)
			continue;

		CString strStName=gpcdef.GetStationName(pfirst->GetStationNo());
		CTG_SingleTrain *pbasetrain=NULL;
		if(m_pBasicTrainDataBase!=NULL)
			pbasetrain=m_pBasicTrainDataBase->FindTrainsByStationAndTrainNo(pfirst->GetStationNo(), trainid);
		
		DWORD train_valide=0;
		if(pbasetrain)
		{
			if (ptrain->GetTrainRecordCount() != pbasetrain->GetTrainRecordCount())
			{
				SetRecordCountDifferent(train_valide);
			}
			else
			{
				for(int n=0; n<ptrain->GetTrainRecordCount(); n++)
				{
					CTG_TrainRecord *prec=ptrain->GetTrainRecordByRecIndex(n);
					if(prec==NULL) continue;
					
					CTG_TrainRecord *pbasicrec=pbasetrain->GetTrainRecordByRecIndex(n);
					if(pbasicrec==NULL) continue;

					if(prec->GetStationNo()==pbasicrec->GetStationNo())
					{
						if(prec->IsStartRec() && pbasicrec->IsStartRec())
						{
							;
						}
						else
						{
							if(prec->m_data.entry != pbasicrec->m_data.entry)
								SetEntryDifferent(train_valide);
						}

						if(prec->IsTerminalRec() && pbasicrec->IsTerminalRec())
						{
							;
						}
						else
						{
							if(prec->m_data.exit != pbasicrec->m_data.exit)
								SetExitDifferent(train_valide);
						}
					}
					else
						SetPathDifferent(train_valide);
				}
			}
		}
		else
			SetTrainExistDifferent(train_valide);

		if (train_valide==0 && flag==SHOW_WARNING_TRAIN)
			continue;

		long basic_index = 0;
		if (pbasetrain)
			basic_index = pbasetrain->GetTrainIndex();
		long tm=0;
		if(i==0 && pfirst->IsStartRec())
			tm=pfirst->GetDepartTime();
		else
			tm=pfirst->GetArriveTime();
		
		CString strTm;
		if(schd_type==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT && tm>0)
		{
			CTime t(tm);
			strTm=t.Format("%y年%m月%d日 %H:%M:%S");
		}
		else if(schd_type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV && tm>=0)
		{
			strTm=BasicStringTime(tm);
		}
		
		xml+=GetTrainResult(ptrain->GetTrainIndex(),basic_index, trainid, strStName, strTm, train_valide);
	}
	if(xml=="<TRAINLIST>")
	{
		AfxMessageBox("没有可显示列车股道信息");
	}
	xml+="</TRAINLIST>";	
	InsertRows(xml,"TRAINLIST/RECORD");

	ShowTrainDifferent();

	Refresh();

	return mlistTrain.size();
}

BOOL  CTrainRouteCheckResultGrid::CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid)
{
	m_currTrain=NULL;
	m_basicTrain=NULL;

	m_result_col_count=0;
	strcpy(m_result_def[m_result_col_count].caption,"序号");
	sprintf(m_result_def[m_result_col_count].attr_name,"no");
	m_result_def[m_result_col_count].data_type=CTC_DATA_STR;
	m_result_def[m_result_col_count].flag=0;
	m_result_col_count++;

	strcpy(m_result_def[m_result_col_count].caption,"当前车次");
	sprintf(m_result_def[m_result_col_count].attr_name,"curr_trainid");
	m_result_def[m_result_col_count].data_type=CTC_DATA_STR;
	m_result_def[m_result_col_count].flag=0;
	m_result_col_count++;

	strcpy(m_result_def[m_result_col_count].caption,"车站");
	sprintf(m_result_def[m_result_col_count].attr_name,"curr_station");
	m_result_def[m_result_col_count].data_type=CTC_DATA_STR;
	m_result_def[m_result_col_count].flag=0;
	m_result_col_count++;

	strcpy(m_result_def[m_result_col_count].caption,"入口");
	sprintf(m_result_def[m_result_col_count].attr_name,"curr_entry");
	m_result_def[m_result_col_count].data_type=CTC_DATA_STR;
	m_result_def[m_result_col_count].flag=0;
	m_result_col_count++;

	strcpy(m_result_def[m_result_col_count].caption,"出口");
	sprintf(m_result_def[m_result_col_count].attr_name,"curr_exit");
	m_result_def[m_result_col_count].data_type=CTC_DATA_STR;
	m_result_def[m_result_col_count].flag=0;
	m_result_col_count++;

	strcpy(m_result_def[m_result_col_count].caption,"基本图车次");
	sprintf(m_result_def[m_result_col_count].attr_name,"basic_trainid");
	m_result_def[m_result_col_count].data_type=CTC_DATA_STR;
	m_result_def[m_result_col_count].flag=0;
	m_result_col_count++;

	strcpy(m_result_def[m_result_col_count].caption,"车站");
	sprintf(m_result_def[m_result_col_count].attr_name,"basic_station");
	m_result_def[m_result_col_count].data_type=CTC_DATA_STR;
	m_result_def[m_result_col_count].flag=0;
	m_result_col_count++;

	strcpy(m_result_def[m_result_col_count].caption,"入口");
	sprintf(m_result_def[m_result_col_count].attr_name,"basic_entry");
	m_result_def[m_result_col_count].data_type=CTC_DATA_STR;
	m_result_def[m_result_col_count].flag=0;
	m_result_col_count++;

	strcpy(m_result_def[m_result_col_count].caption,"出口");
	sprintf(m_result_def[m_result_col_count].attr_name,"basic_exit");
	m_result_def[m_result_col_count].data_type=CTC_DATA_STR;
	m_result_def[m_result_col_count].flag=0;
	m_result_col_count++;

	CXMLDataGrid::CreateGrid(parent,rect,ctrlid,m_result_def,m_result_col_count);

	SetColumnWidth(0, 20);
	SetColumnWidth(1, 60);
	SetColumnWidth(2, 80);
	SetColumnWidth(3, 100);
	SetColumnWidth(4, 100);
	SetColumnWidth(5, 60);
	SetColumnWidth(6, 80);
	SetColumnWidth(7, 100);
	SetColumnWidth(8, 100);

	return TRUE;
}

void  CTrainRouteCheckResultGrid::SetTrainResult(CTG_SingleTrain *pTrainData,CTG_SingleTrain *pBasicTrainData)
{
	m_currTrain=pTrainData;
	m_basicTrain=pBasicTrainData;
}

void  CTrainRouteCheckResultGrid::ShowTrainsDifferent()
{
	for(int i=1;i<GetRowCount();i++)
	{
		for(int m=1; m<=4; m++)
		{
			CString str1, str2;
			CGridCellBase* p = GetCell(i, m);
			if (p)
				str1 = p->GetText();

			CGridCellBase* q = GetCell(i, m+4);
			if (q)
				str2 = q->GetText();
			
			if (str1!="" && str2!="")
			{
				if (str1 != str2)
				{
					p->SetBackClr(RGB(255,100,255));
				}
			}
			else if(str1!="")
			{
				p->SetBackClr(RGB(255,100,255));
			}
			else if(str2="")
			{
				q->SetBackClr(RGB(255,100,255));
			}
		}
	}

	return;
}

void  CTrainRouteCheckResultGrid::ShowTrainData()
{
	if (!m_currTrain)
		return;

	CString xml="<RESULTLIST>";	
	if(NULL==m_basicTrain)
	{
		for (int i=0; i<m_currTrain->GetTrainRecordCount(); i++)
		{
			CString tmp;
			CString trainstr="<RECORD>";

			tmp.Format("<no>%d</no>", i+1);
			trainstr +=tmp;

			const CTG_TrainRecord *curr_rec = m_currTrain->GetTrainRecordByRecIndex_Const(i);
			if (curr_rec)
			{
				CString arrId = curr_rec->GetArriveTID();
				CString depId = curr_rec->GetDepartTID();
				if (arrId==depId)
					tmp.Format("<curr_trainid>%s</curr_trainid>", arrId);
				else
					tmp.Format("<curr_trainid>%s/%s</curr_trainid>", arrId, depId);
				trainstr +=tmp;
			
				tmp.Format("<curr_station>%s</curr_station>", gpcdef.GetStationName(curr_rec->GetStationNo()));
				trainstr +=tmp;

				if (curr_rec->GetEntry()==0 || curr_rec->GetEntry()==255)
					tmp.Format("<curr_entry>始发</curr_entry>");
				else
					tmp.Format("<curr_entry>%d口(%s)</curr_entry>",curr_rec->GetEntry(),GetTGBasicParam()->GetAbsNameByStationEntry(curr_rec->GetStationNo(), curr_rec->GetEntry()));
				trainstr +=tmp;

				if (curr_rec->GetExit()==0 || curr_rec->GetExit()==255)
					tmp.Format("<curr_exit>终到</curr_exit>");
				else
					tmp.Format("<curr_exit>%d口(%s)</curr_exit>",curr_rec->GetExit(),GetTGBasicParam()->GetAbsNameByStationEntry(curr_rec->GetStationNo(), curr_rec->GetExit()));
				trainstr +=tmp;
			}
			
			trainstr+="</RECORD>";
			xml += trainstr;
		}
	}
	else
	{
		const CTG_TrainRecord *curr_rec = m_currTrain->GetTrainRecordByRecIndex_Const(0);
		if(NULL==curr_rec)
		{
			for (int m=0; m<m_basicTrain->GetTrainRecordCount(); m++)
			{
				CString tmp;
				CString trainstr="<RECORD>";

				tmp.Format("<no>%d</no>", m+1);
				trainstr +=tmp;

				const CTG_TrainRecord *basic_rec = m_basicTrain->GetTrainRecordByRecIndex_Const(m);
				if (basic_rec)
				{
					CString arrId = basic_rec->GetArriveTID();
					CString depId = basic_rec->GetDepartTID();
					if (arrId == depId)
						tmp.Format("<basic_trainid>%s</basic_trainid>", arrId);
					else
						tmp.Format("<basic_trainid>%s/%s</basic_trainid>", arrId, depId);
					trainstr +=tmp;

					tmp.Format("<basic_station>%s</basic_station>", gpcdef.GetStationName(basic_rec->GetStationNo()));
					trainstr +=tmp;

					if (basic_rec->GetEntry()==0 || basic_rec->GetEntry()==255)
						tmp.Format("<basic_entry>始发</basic_entry>");
					else
						tmp.Format("<basic_entry>%d口(%s)</basic_entry>", basic_rec->GetEntry(),GetTGBasicParam()->GetAbsNameByStationEntry(basic_rec->GetStationNo(), basic_rec->GetEntry()));
					trainstr +=tmp;

					if (basic_rec->GetExit()==0 || basic_rec->GetExit()==255)
						tmp.Format("<basic_exit>终到</basic_exit>");
					else
						tmp.Format("<basic_exit>%d口(%s)</basic_exit>", basic_rec->GetExit(),GetTGBasicParam()->GetAbsNameByStationEntry(basic_rec->GetStationNo(), basic_rec->GetExit()));
					trainstr +=tmp;
				}

				
				trainstr+="</RECORD>";
				xml += trainstr;
			}
		}
		else
		{
			int m_bindex=-1;
			for (int m=0; m<m_basicTrain->GetTrainRecordCount(); m++)
			{
				const CTG_TrainRecord *basic_rec = m_basicTrain->GetTrainRecordByRecIndex_Const(m);
				if (basic_rec)
				{
					if(basic_rec->GetStationNo()==curr_rec->GetStationNo())
					{
						m_bindex=m;
						break;
					}
				}
			}

			if(m_bindex==-1)
			{
				int no=1;
				for (int i=0; i<m_currTrain->GetTrainRecordCount(); i++)
				{
					CString tmp;
					CString trainstr="<RECORD>";

					tmp.Format("<no>%d</no>", no++);
					trainstr +=tmp;

					const CTG_TrainRecord *curr_rec = m_currTrain->GetTrainRecordByRecIndex_Const(i);
					if (curr_rec)
					{
						CString arrId = curr_rec->GetArriveTID();
						CString depId = curr_rec->GetDepartTID();
						if (arrId==depId)
							tmp.Format("<curr_trainid>%s</curr_trainid>", arrId);
						else
							tmp.Format("<curr_trainid>%s/%s</curr_trainid>", arrId, depId);
						trainstr +=tmp;
					
						tmp.Format("<curr_station>%s</curr_station>", gpcdef.GetStationName(curr_rec->GetStationNo()));
						trainstr +=tmp;

						if (curr_rec->GetEntry()==0 || curr_rec->GetEntry()==255)
							tmp.Format("<curr_entry>始发</curr_entry>");
						else
							tmp.Format("<curr_entry>%d口(%s)</curr_entry>",curr_rec->GetEntry(),GetTGBasicParam()->GetAbsNameByStationEntry(curr_rec->GetStationNo(), curr_rec->GetEntry()));
						trainstr +=tmp;

						if (curr_rec->GetExit()==0 || curr_rec->GetExit()==255)
							tmp.Format("<curr_exit>终到</curr_exit>");
						else
							tmp.Format("<curr_exit>%d口(%s)</curr_exit>",curr_rec->GetExit(),GetTGBasicParam()->GetAbsNameByStationEntry(curr_rec->GetStationNo(), curr_rec->GetExit()));
						trainstr +=tmp;
					}
					
					trainstr+="</RECORD>";
					xml += trainstr;
				}
				for (int m=0; m<m_basicTrain->GetTrainRecordCount(); m++)
				{
					CString tmp;
					CString trainstr="<RECORD>";

					tmp.Format("<no>%d</no>", no++);
					trainstr +=tmp;

					const CTG_TrainRecord *basic_rec = m_basicTrain->GetTrainRecordByRecIndex_Const(m);
					if (basic_rec)
					{
						CString arrId = basic_rec->GetArriveTID();
						CString depId = basic_rec->GetDepartTID();
						if (arrId == depId)
							tmp.Format("<basic_trainid>%s</basic_trainid>", arrId);
						else
							tmp.Format("<basic_trainid>%s/%s</basic_trainid>", arrId, depId);
						trainstr +=tmp;

						tmp.Format("<basic_station>%s</basic_station>", gpcdef.GetStationName(basic_rec->GetStationNo()));
						trainstr +=tmp;

						if (basic_rec->GetEntry()==0 || basic_rec->GetEntry()==255)
							tmp.Format("<basic_entry>始发</basic_entry>");
						else
							tmp.Format("<basic_entry>%d口(%s)</basic_entry>", basic_rec->GetEntry(),GetTGBasicParam()->GetAbsNameByStationEntry(basic_rec->GetStationNo(), basic_rec->GetEntry()));
						trainstr +=tmp;

						if (basic_rec->GetExit()==0 || basic_rec->GetExit()==255)
							tmp.Format("<basic_exit>终到</basic_exit>");
						else
							tmp.Format("<basic_exit>%d口(%s)</basic_exit>", basic_rec->GetExit(),GetTGBasicParam()->GetAbsNameByStationEntry(basic_rec->GetStationNo(), basic_rec->GetExit()));
						trainstr +=tmp;
					}

					trainstr+="</RECORD>";
					xml += trainstr;
				}
			}
			else
			{
				int no=1;
				int count=m_currTrain->GetTrainRecordCount();
				if(count<m_basicTrain->GetTrainRecordCount())
					count=m_basicTrain->GetTrainRecordCount();

				for(int i=0; i<count; i++)
				{
					if(i<m_bindex)
					{
						CString tmp;
						CString trainstr="<RECORD>";

						tmp.Format("<no>%d</no>", no++);
						trainstr +=tmp;

						const CTG_TrainRecord *basic_rec = m_basicTrain->GetTrainRecordByRecIndex_Const(i);
						if (basic_rec)
						{
							CString arrId = basic_rec->GetArriveTID();
							CString depId = basic_rec->GetDepartTID();
							if (arrId == depId)
								tmp.Format("<basic_trainid>%s</basic_trainid>", arrId);
							else
								tmp.Format("<basic_trainid>%s/%s</basic_trainid>", arrId, depId);
							trainstr +=tmp;

							tmp.Format("<basic_station>%s</basic_station>", gpcdef.GetStationName(basic_rec->GetStationNo()));
							trainstr +=tmp;

							if (basic_rec->GetEntry()==0 || basic_rec->GetEntry()==255)
								tmp.Format("<basic_entry>始发</basic_entry>");
							else
								tmp.Format("<basic_entry>%d口(%s)</basic_entry>", basic_rec->GetEntry(),GetTGBasicParam()->GetAbsNameByStationEntry(basic_rec->GetStationNo(), basic_rec->GetEntry()));
							trainstr +=tmp;

							if (basic_rec->GetExit()==0 || basic_rec->GetExit()==255)
								tmp.Format("<basic_exit>终到</basic_exit>");
							else
								tmp.Format("<basic_exit>%d口(%s)</basic_exit>", basic_rec->GetExit(),GetTGBasicParam()->GetAbsNameByStationEntry(basic_rec->GetStationNo(), basic_rec->GetExit()));
							trainstr +=tmp;
						}

						trainstr+="</RECORD>";
						xml += trainstr;
					}
					else
					{
						CString tmp;
						CString trainstr="<RECORD>";

						tmp.Format("<no>%d</no>", no++);
						trainstr +=tmp;

						const CTG_TrainRecord *curr_rec = m_currTrain->GetTrainRecordByRecIndex_Const(i-m_bindex);
						if (curr_rec)
						{
							CString arrId = curr_rec->GetArriveTID();
							CString depId = curr_rec->GetDepartTID();
							if (arrId==depId)
								tmp.Format("<curr_trainid>%s</curr_trainid>", arrId);
							else
								tmp.Format("<curr_trainid>%s/%s</curr_trainid>", arrId, depId);
							trainstr +=tmp;
						
							tmp.Format("<curr_station>%s</curr_station>", gpcdef.GetStationName(curr_rec->GetStationNo()));
							trainstr +=tmp;

							if (curr_rec->GetEntry()==0 || curr_rec->GetEntry()==255)
								tmp.Format("<curr_entry>始发</curr_entry>");
							else
								tmp.Format("<curr_entry>%d口(%s)</curr_entry>",curr_rec->GetEntry(),GetTGBasicParam()->GetAbsNameByStationEntry(curr_rec->GetStationNo(), curr_rec->GetEntry()));
							trainstr +=tmp;

							if (curr_rec->GetExit()==0 || curr_rec->GetExit()==255)
								tmp.Format("<curr_exit>终到</curr_exit>");
							else
								tmp.Format("<curr_exit>%d口(%s)</curr_exit>",curr_rec->GetExit(),GetTGBasicParam()->GetAbsNameByStationEntry(curr_rec->GetStationNo(), curr_rec->GetExit()));
							trainstr +=tmp;
						}
						const CTG_TrainRecord *basic_rec = m_basicTrain->GetTrainRecordByRecIndex_Const(i);
						if (basic_rec)
						{
							CString arrId = basic_rec->GetArriveTID();
							CString depId = basic_rec->GetDepartTID();
							if (arrId == depId)
								tmp.Format("<basic_trainid>%s</basic_trainid>", arrId);
							else
								tmp.Format("<basic_trainid>%s/%s</basic_trainid>", arrId, depId);
							trainstr +=tmp;

							tmp.Format("<basic_station>%s</basic_station>", gpcdef.GetStationName(basic_rec->GetStationNo()));
							trainstr +=tmp;

							if (basic_rec->GetEntry()==0 || basic_rec->GetEntry()==255)
								tmp.Format("<basic_entry>始发</basic_entry>");
							else
								tmp.Format("<basic_entry>%d口(%s)</basic_entry>", basic_rec->GetEntry(),GetTGBasicParam()->GetAbsNameByStationEntry(basic_rec->GetStationNo(), basic_rec->GetEntry()));
							trainstr +=tmp;

							if (basic_rec->GetExit()==0 || basic_rec->GetExit()==255)
								tmp.Format("<basic_exit>终到</basic_exit>");
							else
								tmp.Format("<basic_exit>%d口(%s)</basic_exit>", basic_rec->GetExit(),GetTGBasicParam()->GetAbsNameByStationEntry(basic_rec->GetStationNo(), basic_rec->GetExit()));
							trainstr +=tmp;
						}

						
						trainstr+="</RECORD>";
						xml += trainstr;
					}
				}
			}
		}
	}
	//for (int i=0; i<m_currTrain->GetTrainRecordCount(); i++)
	//{
	//	const CTG_TrainRecord *curr_rec = m_currTrain->GetTrainRecordByRecIndex_Const(i);
	//	if (!curr_rec)
	//		continue;

	//	CString tmp;
	//	CString trainstr="<RECORD>";

	//	tmp.Format("<no>%d</no>", i+1);
	//	trainstr +=tmp;

	//	CString arrId = curr_rec->GetArriveTID();
	//	CString depId = curr_rec->GetDepartTID();
	//	if (arrId==depId)
	//		tmp.Format("<curr_trainid>%s</curr_trainid>", arrId);
	//	else
	//		tmp.Format("<curr_trainid>%s/%s</curr_trainid>", arrId, depId);
	//	trainstr +=tmp;
	//
	//	tmp.Format("<curr_station>%s</curr_station>", gpcdef.GetStationName(curr_rec->GetStationNo()));
	//	trainstr +=tmp;

	//	if (curr_rec->GetEntry()==0 || curr_rec->GetEntry()==255)
	//		tmp.Format("<curr_entry>始发</curr_entry>");
	//	else
	//		tmp.Format("<curr_entry>%d口(%s)</curr_entry>",curr_rec->GetEntry(),GetTGBasicParam()->GetAbsNameByStationEntry(curr_rec->GetStationNo(), curr_rec->GetEntry()));
	//	trainstr +=tmp;

	//	if (curr_rec->GetExit()==0 || curr_rec->GetExit()==255)
	//		tmp.Format("<curr_exit>终到</curr_exit>");
	//	else
	//		tmp.Format("<curr_exit>%d口(%s)</curr_exit>",curr_rec->GetExit(),GetTGBasicParam()->GetAbsNameByStationEntry(curr_rec->GetStationNo(), curr_rec->GetExit()));
	//	trainstr +=tmp;

	//	if (m_basicTrain)
	//	{
	//		const CTG_TrainRecord *basic_rec = m_basicTrain->GetTrainRecordByRecIndex_Const(i);
	//		if (basic_rec)
	//		{
	//			CString arrId = basic_rec->GetArriveTID();
	//			CString depId = basic_rec->GetDepartTID();
	//			if (arrId == depId)
	//				tmp.Format("<basic_trainid>%s</basic_trainid>", basic_rec->GetArriveTID());
	//			else
	//				tmp.Format("<basic_trainid>%s/%s</basic_trainid>", basic_rec->GetArriveTID(), basic_rec->GetDepartTID());
	//			trainstr +=tmp;

	//			tmp.Format("<basic_station>%s</basic_station>", gpcdef.GetStationName(basic_rec->GetStationNo()));
	//			trainstr +=tmp;

	//			if (basic_rec->GetEntry()==0 || basic_rec->GetEntry()==255)
	//				tmp.Format("<basic_entry>始发</basic_entry>");
	//			else
	//				tmp.Format("<basic_entry>%d口(%s)</basic_entry>", basic_rec->GetEntry(),GetTGBasicParam()->GetAbsNameByStationEntry(basic_rec->GetStationNo(), basic_rec->GetEntry()));
	//			trainstr +=tmp;

	//			if (basic_rec->GetExit()==0 || basic_rec->GetExit()==255)
	//				tmp.Format("<basic_exit>终到</basic_exit>");
	//			else
	//				tmp.Format("<basic_exit>%d口(%s)</basic_exit>", basic_rec->GetExit(),GetTGBasicParam()->GetAbsNameByStationEntry(basic_rec->GetStationNo(), basic_rec->GetExit()));
	//			trainstr +=tmp;
	//		}
	//	}

	//	trainstr+="</RECORD>";
	//	xml += trainstr;
	//}

	//if (m_basicTrain && i<m_basicTrain->GetTrainRecordCount())
	//{
	//	for (int m=i; m<m_basicTrain->GetTrainRecordCount(); m++)
	//	{
	//		CString tmp;
	//		CString trainstr="<RECORD>";

	//		tmp.Format("<no>%d</no>", m+1);
	//		trainstr +=tmp;

	//		const CTG_TrainRecord *basic_rec = m_basicTrain->GetTrainRecordByRecIndex_Const(m);
	//		if (basic_rec)
	//		{
	//			CString arrId = basic_rec->GetArriveTID();
	//			CString depId = basic_rec->GetDepartTID();
	//			if (arrId == depId)
	//				tmp.Format("<basic_trainid>%s</basic_trainid>", arrId);
	//			else
	//				tmp.Format("<basic_trainid>%s/%s</basic_trainid>", arrId, depId);
	//			trainstr +=tmp;

	//			tmp.Format("<basic_station>%s</basic_station>", gpcdef.GetStationName(basic_rec->GetStationNo()));
	//			trainstr +=tmp;

	//			if (basic_rec->GetEntry()==0 || basic_rec->GetEntry()==255)
	//				tmp.Format("<basic_entry>始发</basic_entry>");
	//			else
	//				tmp.Format("<basic_entry>%d口(%s)</basic_entry>", basic_rec->GetEntry(),GetTGBasicParam()->GetAbsNameByStationEntry(basic_rec->GetStationNo(), basic_rec->GetEntry()));
	//			trainstr +=tmp;

	//			if (basic_rec->GetExit()==0 || basic_rec->GetExit()==255)
	//				tmp.Format("<basic_exit>终到</basic_exit>");
	//			else
	//				tmp.Format("<basic_exit>%d口(%s)</basic_exit>", basic_rec->GetExit(),GetTGBasicParam()->GetAbsNameByStationEntry(basic_rec->GetStationNo(), basic_rec->GetExit()));
	//			trainstr +=tmp;
	//		}

	//		trainstr+="</RECORD>";
	//		xml += trainstr;
	//	}
	//}

	if(xml=="<RESULTLIST>")
	{
		AfxMessageBox("没有可显示列车股道信息");
	}
	xml+="</RESULTLIST>";	
	InsertRows(xml,"RESULTLIST/RECORD");

	ShowTrainsDifferent();

	ExpandColumnsToFit();
	
	Refresh();
}

IMPLEMENT_DYNCREATE(CDlgTrainRouteCheck, CDialog)
CDlgTrainRouteCheck::CDlgTrainRouteCheck(CWnd* pParent) : CDialog(CDlgTrainRouteCheck::IDD,pParent)
{
	m_nFlag=1;
	m_pBasicDataBase=NULL;
}

CDlgTrainRouteCheck::~CDlgTrainRouteCheck()
{
}

void CDlgTrainRouteCheck::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDlgTrainRouteCheck, CDialog)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_BN_CLICKED(ID_RELOAD, OnBnClickedReload)
	ON_BN_CLICKED(ID_CHECK_STATION_ACK_SIDE3, OnBnClickedCheckTrain)
	ON_NOTIFY(NM_DBLCLK, IDC_STATIC_CHECK2, OnChangeTrainResult) 
END_MESSAGE_MAP()

void CDlgTrainRouteCheck::SetDocument(CTgDocument *pdoc,CTG_ScheduleInfo::SCHEDULE_TYPE schd_type, BOOL flag)
{
	m_pDoc=pdoc;
	m_pTrainDataBase=m_pDoc->GetScheduleData(schd_type);
	if(CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT==schd_type || CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV==schd_type)
		m_pBasicDataBase=m_pDoc->GetScheduleData(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC);
	else
		m_pBasicDataBase=NULL;

	m_nFlag=flag;
	m_schd_type=schd_type;
	m_traingrid.SetScheduleBase(m_pTrainDataBase, m_pBasicDataBase);
}
int  CDlgTrainRouteCheck::OnCreate(LPCREATESTRUCT lpCreat)
{
	if(CDialog::OnCreate(lpCreat))
		return -1;

	return 0;
}

BOOL CDlgTrainRouteCheck::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect rect(0,0,0,0);
	GetDlgItem(IDC_STATIC_CHECK2)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	m_traingrid.CreateGrid(this,rect,IDC_STATIC_CHECK2);
	m_traingrid.SetSingleRowSelection(TRUE);

	CRect rect2(0,0,0,0);
	GetDlgItem(IDC_STATIC_CHECK3)->GetWindowRect(&rect2);
	ScreenToClient(&rect2);
	m_resultgrid.CreateGrid(this,rect2,IDC_STATIC_CHECK3);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDlgTrainRouteCheck::InitDlgCtrlItem()
{
	((CButton *)GetDlgItem(IDC_CHK_PASSENGER))->SetCheck(1);
	((CButton *)GetDlgItem(IDC_CHK_FREIGHT))->SetCheck(1);

	m_traingrid.DeleteNonFixedRows();
	m_resultgrid.DeleteNonFixedRows();
}

void CDlgTrainRouteCheck::OnSize(UINT nType,int cx,int cy)
{
	if(m_traingrid.GetSafeHwnd())
		m_traingrid.MoveWindow(0,50,cx,cy-55);
	CDialog::OnSize(nType,cx,cy);
}

void CDlgTrainRouteCheck::OnClose()
{
	this->ShowWindow(SW_HIDE);
}
void  CDlgTrainRouteCheck::OnCancel()
{
	OnClose();
}

CTG_SingleTrain  *CDlgTrainRouteCheck::GetRowTrainRecord(int row)
{
	if(row<=0 || row>=m_traingrid.GetRowCount())
		return NULL;

	long train_index=m_traingrid.GetCell(row,0)->GetData();

	CTG_SingleTrain *ptrain=m_pTrainDataBase->GetSingleTrain(train_index);
	if(ptrain==NULL)
		return NULL;

	return ptrain;
}

bool CDlgTrainRouteCheck::GetSelTrains(CTG_TrainPtrGroup& group, CString& str)
{
	bool bSel=false;
	str="请注意, 你确实要查看:";
	if(((CButton *)GetDlgItem(IDC_CHK_PASSENGER))->GetCheck()==1)
	{
		str+=" 客车";
		bSel=true;
	}
	if(((CButton *)GetDlgItem(IDC_CHK_FREIGHT))->GetCheck()==1)
	{
		str+=" 货车";
		bSel=true;
	}

	if(!bSel)
	{
		AfxMessageBox("请选择, 要查看的列车类型!");
		return false;
	}


	group.Clear();
	CTG_TrainPtrGroup group2;
	m_pTrainDataBase->FindTrains(group2);
	for(int i=0;i<group2.GetTrainCount();i++)
	{
		CTG_SingleTrain *ptr=group2.GetTrainPtr(i);

		if(((CButton *)GetDlgItem(IDC_CHK_PASSENGER))->GetCheck()==0)
		{
			if(ptr->IsPassengerTrain())
				continue;
		}
		if(((CButton *)GetDlgItem(IDC_CHK_FREIGHT))->GetCheck()==0)
		{
			if(!ptr->IsPassengerTrain())
				continue;
		}

		group.AddTrainPtr(ptr);
	}

	group.SetTrainsPtr();

	if(group.GetTrainCount()<=0)
	{
		MessageBox("没有要显示的列车!", "提示", MB_OK|MB_ICONWARNING);
	}
	return true;
}

void CDlgTrainRouteCheck::OnBnClickedCheckTrain()
{
	CString str;
	CTG_TrainPtrGroup group;
	if(!GetSelTrains(group, str))
		return;

	str+=" 径路报警 车次!";

	if(IDNO==MessageBox(str, "提示", MB_YESNO | MB_ICONQUESTION))
		return;

	m_traingrid.DeleteNonFixedRows();
	int count=m_traingrid.ShowTrainData(&group, CTrainRouteCheckListGrid::SHOW_WARNING_TRAIN, m_schd_type);
}

void CDlgTrainRouteCheck::OnBnClickedReload()
{
	CString str;
	CTG_TrainPtrGroup group;
	if(!GetSelTrains(group, str))
		return;
	str+=" 车次!";
	if(IDNO==MessageBox(str, "提示", MB_YESNO | MB_ICONQUESTION))
		return;

	m_traingrid.DeleteNonFixedRows();
	int count=m_traingrid.ShowTrainData(&group, CTrainRouteCheckListGrid::SHOW_ALL_TRAIN, m_schd_type);
}

void CDlgTrainRouteCheck::OnChangeTrainResult(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	int row = pItem->iRow;
	int col = pItem->iColumn;

	if (row>0 && row<m_traingrid.GetRowCount())
	{
		long train_index=m_traingrid.GetCell(row,0)->GetData();
		CTG_SingleTrain *currTrain = m_pTrainDataBase->GetSingleTrain(train_index);
		long basic_index=m_traingrid.GetCell(row,1)->GetData();

		CTG_SingleTrain *basicTrain = NULL;
		if (basic_index!=0)
			basicTrain = m_pBasicDataBase->GetSingleTrain(basic_index);

		m_resultgrid.DeleteNonFixedRows();

		m_resultgrid.SetTrainResult(currTrain, basicTrain);
		m_resultgrid.ShowTrainData();
	}
}