#include "StdAfx.h"
#include "sysshare.h"
#include "tg_schedule_base.h"
#include "tgdocument.h"
#include ".\DepartPredictDlg.h"
#include "Mmsystem.h"
#include ".\TrainDepartPredictHolder.h"
extern bool CreateRecursiveDir(const char* filename);
extern CString GetRecordXML(TrainPredict& predict);
// 获取车站控制模式,下发阶段计划检查使用
extern TG_STATION_STATUS *GetStationStatusPtr(int station); // defined in tg_logic_main.cpp

CTrainDepartPredictHolder::CTrainDepartPredictHolder(CTgDocument* pDoc, int con_id)
{
	for(int i=0; i<15; i++)
	{
		for(int j=0; j<3; j++)
		{
			CDlgTimeMessageBox::WindowPosition[i][j]=0;
		}
	}
	m_pDoc=pDoc;
	m_nShiftID=0;
	m_conid=con_id;
	m_pMMI=NULL;
	m_pWorkSchedule=NULL;

	CWorkScheduleHolder* pSchdHoder=m_pDoc->GetTGDataHolder();
	if(pSchdHoder)
	{
		m_pWorkSchedule = pSchdHoder->GetWorkScheduleData();
	}
	
	for(int i=0;i<MAX_PREDICT_STATION_PLAN;i++)
		m_pStPlans[i]=NULL;
	
	LogPredict(0,"[自动预告] 生成conid:%d 预告服务 TrainDepartPredictHolder %X", m_conid, this);
	
	STATION_NO list_station[256];
	STATION_NO list_neib_station[256];
	memset(list_station, 0, sizeof(list_station));
	memset(list_neib_station, 0, sizeof(list_neib_station));

	int stationnum=gpcdef.GetYugaoStation(m_conid, 256, list_station, list_neib_station);
	for(int i=0; i<stationnum; i++)
	{
		STATION_NO  station		=list_station[i];
		STATION_NO  neib_station	=list_neib_station[i];
		if(station==0 || neib_station==0) 
			continue;
		
		for(int i=0;i<MAX_PREDICT_STATION_PLAN;i++)
		{
			if(m_pStPlans[i])
			{
				if(m_pStPlans[i]->GetStationNo()==station)
				{
					m_pStPlans[i]->AddNeibStation(neib_station);
					break;
				}
			}
			else
			{
				m_pStPlans[i] =new CStationPredict(station, neib_station);
				if(m_pStPlans[i])
				{
					LogPredict(0,"[自动预告] 注册车站:%s(%d)成功,邻站为:%s(%d)", 
						gpcdef.GetStationName(station), station, 
						gpcdef.GetStationName(neib_station), neib_station);
				}
				else
				{
					LogPredict(0,"[自动预告] 注册车站:%s(%d)失败,邻站为:%s(%d), 原因是申请内存失败", 
						gpcdef.GetStationName(station), station, 
						gpcdef.GetStationName(neib_station), neib_station);
				}
				break;
			}
		}
	}

	for(int i=0;i<MAX_PREDICT_STATION_PLAN;i++)
	{
		if(m_pStPlans[i])
		{
			m_pStPlans[i]->LoadStplanFromFile();
		}
	}
}

CTrainDepartPredictHolder::~CTrainDepartPredictHolder(void)
{
	LogPredict(0,"[自动预告] 删除conid:%d 预告服务 TrainDepartPredictHolder %X", m_conid, this);
	
	for(int i=0;i<MAX_PREDICT_STATION_PLAN;i++)
	{
		if(m_pStPlans[i])
		{
			delete m_pStPlans[i];
			m_pStPlans[i]=NULL;
		}
	}
}

CStationPredict* CTrainDepartPredictHolder::GetStPredictPlan(STATION_NO this_station)
{
	for(int i=0;i<MAX_PREDICT_STATION_PLAN;i++)
	{
		if(m_pStPlans[i])
		{
			if(m_pStPlans[i]->GetStationNo()==this_station)
			{
				return m_pStPlans[i];
			}
		}
	}

	return NULL;
}

CStationPredict* CTrainDepartPredictHolder::GetStPredictPlan(STATION_NO this_station, STATION_NO neib_station)
{
	for(int i=0;i<MAX_PREDICT_STATION_PLAN;i++)
	{
		if(m_pStPlans[i])
		{
			if(m_pStPlans[i]->GetStationNo()==this_station && m_pStPlans[i]->IsMyNeibStation(neib_station))
			{
				return m_pStPlans[i];
			}
		}
	}

	return NULL;
}

bool CTrainDepartPredictHolder::IsNeedDowithDepartPredict(STATION_NO station)
{
	TG_STATION_STATUS * pst = GetStationStatusPtr(station);
	if(NULL != pst)
	{
		if(pst->ctrl_mode==SMODE_DISTRIBUTED ||
			pst->ctrl_mode==SMODE_CENTER_CTRL)
		{
			return true;
		}
	}

	return false;
}

void CTrainDepartPredictHolder::SendRevYugao(MsgTrainDepartPredict *pBuf)
{
	MsgTrainDepartPredict msg;

	msg.nSourceStno = pBuf->nDestStno;		// 发车车站
	msg.nDestStno = pBuf->nSourceStno;		// 发车目标车站
	msg.nDestEntry = pBuf->nDestEntry;		// 目标车站接车入口
	strcpy(msg.strTid, pBuf->strTid);			// 车次号
	msg.nTrainIndex = pBuf->nTrainIndex;	// Train Index
	msg.nRecIndex = pBuf->nRecIndex;
	
	GPCENTITY entityInfo;
	if (false == gpcdef.GetEntityInfo(gpcdef.GetCurUserName(),entityInfo))
	{
		AfxMessageBox("配置文件错误：找不到本用户的ENTITY!");
		return;
	}
	msg.nSrcEntity = entityInfo.nEntityId;		// 发送预告回执的实体号
	msg.nSrcType=MsgTrainDepartPredict::GPC;// 发送预告的实体类型	
	msg.nOperType=MsgTrainDepartPredict::SHOUDAO;	// 预告类型

	CTime tm=CTime::GetCurrentTime();
	msg.nOperTime = tm.GetTime();				// 预告时间

	long eid=GetSpecialEntityID(SERVER_TYPE_PREDICT);
	SendBMsg(&msg, eid);

	LogPredict(0, "发送收到预告回执 train %s SourceStno %d DestStno %d entry %d trainIndex %d recIndex %d", 
			msg.strTid, msg.nSourceStno, msg.nDestStno, 
			msg.nDestEntry, msg.nTrainIndex, msg.nRecIndex);

}

BOOL  CTrainDepartPredictHolder::ProcessPredictByNeibMsg(MsgTrainDepartPredict *pBuf)
{
	if(pBuf->nOperType != MsgTrainDepartPredict::YUGAO && pBuf->nOperType != MsgTrainDepartPredict::QUXIAO &&
		pBuf->nOperType != MsgTrainDepartPredict::SHOUDAO && pBuf->nOperType != MsgTrainDepartPredict::TONGYI)
	{
		LogPredict(0, "nSrcEntity %d 发送的预告消息,但是nOperType不正确: trainid %s nSourceStno %d nDestEntry %d nDestStno %d nSrcType %d nOperType %d trainIndex %d recIndex %d nOperTime %d", 
					pBuf->nSrcEntity, pBuf->strTid, pBuf->nSourceStno, pBuf->nDestEntry, pBuf->nDestStno, pBuf->nSrcType, pBuf->nOperType, 
					pBuf->nTrainIndex, pBuf->nRecIndex, pBuf->nOperTime);

		return FALSE;
	}
	LogPredict(0, "***************************");

	LogPredict(0, "处理收到预告消息:trainid %s nSourceStno %d nDestEntry %d nDestStno %d nSrcType %d nOperType %d trainIndex %d recIndex %d nOperTime %d", 
					pBuf->strTid, pBuf->nSourceStno, pBuf->nDestEntry, pBuf->nDestStno, pBuf->nSrcType, pBuf->nOperType, 
					pBuf->nTrainIndex, pBuf->nRecIndex, pBuf->nOperTime);
	
	
	LogPredict(0, "目的站 %d 是调度台代表车站 源站为邻站 %d", pBuf->nDestStno,pBuf->nSourceStno);
		
	bool bUpdateUI=false;
	if(pBuf->nOperType == MsgTrainDepartPredict::YUGAO || pBuf->nOperType == MsgTrainDepartPredict::QUXIAO)
	{
		if(pBuf->nOperType == MsgTrainDepartPredict::YUGAO)
		{
			SendRevYugao(pBuf);
			LogPredict(0,"收到邻站预告,查找本地计划");
		}
		else
			LogPredict(0,"收到邻站取消发车,查找本地计划");

		STATION_NO station = pBuf->nDestStno;
		STATION_NO neib_station = pBuf->nSourceStno;
		TRAIN_INDEX train_index = pBuf->nTrainIndex;
		short rec_index = pBuf->nRecIndex;
		CString arrive_trainid=pBuf->strTid;

		CStationPredict* pstation = GetStPredictPlan(station, neib_station);
		if(NULL == pstation)
		{
			LogPredict(0, "没有本站 %d 和邻站 %d 的预告配置, 无法更新预告相关时间", station, neib_station);
			return false;
		}
			
		std::vector<int> list_idx;
		pstation->GetStplanByTrainNoAndPreStation(arrive_trainid, neib_station, list_idx);
		if(list_idx.size()<=0)
		{
			LogPredict(0,"根据预告车站车次 %s 找不到对应计划, 无法处理邻站发送来的预告.", arrive_trainid);
			return false;
		}
		else
		{
			LogPredict(0,"根据预告车站车次 %s 找到 %d 个计划:", arrive_trainid, list_idx.size());
		}
			
		int idx = -1;
		if(list_idx.size()>1)
		{
			for(int n=0; n<list_idx.size(); n++)
			{
				int no = list_idx[n];
				if(pstation->m_stplan[no].train_index==train_index && pstation->m_stplan[no].rec_index==rec_index)
				{
					idx = no;
					LogPredict(0,"根据train_index %u rec_index %d 定位到具体计划", train_index, rec_index);
					break;
				}
			}

			if(idx==-1)
			{
				CDepartPredictDlg* pMMI = GetMMI();
				if(pMMI!=NULL)
				{
					if(pBuf->nOperType == MsgTrainDepartPredict::YUGAO)
						pMMI->SaveTrainNo(station, neib_station, arrive_trainid, TrainPredict::NEIB_YUGAO, pBuf->nOperTime, 1);
					else
						pMMI->SaveTrainNo(station, neib_station, arrive_trainid, TrainPredict::NEIB_CANCEL, pBuf->nOperTime, 1);
					pMMI->ShowWindow(SW_SHOW);
					return TRUE;
				}
			}
		}
		else
		{
			idx=list_idx[0];
		}
		
		if(idx!=-1)
		{
			if(pBuf->nOperType == MsgTrainDepartPredict::YUGAO)
			{
				bUpdateUI = true;

				pstation->m_stplan[idx].neib_yugao_time  = pBuf->nOperTime;
				pstation->m_stplan[idx].neib_cancel_time = 0;
				pstation->SaveStplanToFile();
				LogPredict(0,"设置邻站预告 成功, train_index %u rec_index %d trainid %s 预告时间:%d.", 
					pstation->m_stplan[idx].train_index, pstation->m_stplan[idx].rec_index, pBuf->strTid, pBuf->nOperTime);
			}

			if(pBuf->nOperType == MsgTrainDepartPredict::QUXIAO)
			{
				bUpdateUI = true;

				pstation->m_stplan[idx].tongyi_neib_time =0;
				pstation->m_stplan[idx].neib_yugao_time = 0;
				pstation->m_stplan[idx].neib_cancel_time = pBuf->nOperTime;
				pstation->SaveStplanToFile();
				LogPredict(0,"设置邻站取消发车 成功, train_index %u rec_index %d trainid %s 邻站取消发车时间 %d", 
					pstation->m_stplan[idx].train_index, pstation->m_stplan[idx].rec_index, pBuf->strTid, pBuf->nOperTime);
			}
		}
	}
	else if(pBuf->nOperType == MsgTrainDepartPredict::SHOUDAO)
	{
		LogPredict(0,"接收到 预告收到回执，train_index %d rec_index %d trainid %s tm %d", 
				pBuf->nTrainIndex, pBuf->nRecIndex, pBuf->strTid, pBuf->nOperTime);
	}
	else if(pBuf->nOperType == MsgTrainDepartPredict::TONGYI)
	{
		STATION_NO station = pBuf->nDestStno;
		STATION_NO neib_station = pBuf->nSourceStno;
		TRAIN_INDEX train_index = pBuf->nTrainIndex;
		short rec_index = pBuf->nRecIndex;
		CString depart_trainid=pBuf->strTid;

		LogPredict(0,"收到邻站同意,查找本地计划");

		CStationPredict* pstation = GetStPredictPlan(station, neib_station);
		if(NULL == pstation)
		{
			LogPredict(0, "没有本站 %d 和邻站 %d 的预告配置, 无法更新预告相关时间", station, neib_station);
			return false;
		}
			
		std::vector<int> list_idx;
		pstation->GetStplanByTrainNoAndNextStation(depart_trainid, neib_station, list_idx);
		if(list_idx.size()<=0)
		{
			LogPredict(0,"根据预告车次 %s 找不到对应计划, 无法处理邻站发送来的同意.", depart_trainid);
			return false;
		}
		else
		{
			LogPredict(0,"根据预告车次 %s 找到 %d 个计划:", depart_trainid, list_idx.size());
		}
			
		int idx = -1;
		if(list_idx.size()>1)
		{
			for(int n=0; n<list_idx.size(); n++)
			{
				int no = list_idx[n];
				if(pstation->m_stplan[no].train_index==train_index && pstation->m_stplan[no].rec_index==rec_index)
				{
					idx = no;
					LogPredict(0,"根据train_index %u rec_index %d 定位到具体计划", train_index, rec_index);
					break;
				}
			}

			if(idx==-1)
			{
				CDepartPredictDlg* pMMI = GetMMI();
				if(pMMI!=NULL)
				{
					pMMI->SaveTrainNo(station, neib_station, depart_trainid, TrainPredict::NEIB_TONGYI, pBuf->nOperTime, 2);
					pMMI->ShowWindow(SW_SHOW);
					return TRUE;
				}
			}
		}
		else
		{
			idx=list_idx[0];
		}
		
		if(idx>=0)
		{
			if(pstation->m_stplan[idx].yugao_neib_time>0)
			{
				pstation->m_stplan[idx].neib_tongyi_time = pBuf->nOperTime;
				pstation->SaveStplanToFile();
				bUpdateUI = true;

				LogPredict(0,"设置邻站同意 成功, train_index %d rec_index %d trainid %s tm %d", 
							pBuf->nTrainIndex, pBuf->nRecIndex, pBuf->strTid, pBuf->nOperTime);
			}
			else
			{
				CDepartPredictDlg* pMMI = GetMMI();
				if(pMMI!=NULL)
				{
					pMMI->SaveTrainNo(station, neib_station, depart_trainid, TrainPredict::NEIB_TONGYI, pBuf->nOperTime, 2);
					pMMI->ShowWindow(SW_SHOW);
					return TRUE;
				}
			}
		}
	}
		
	if(bUpdateUI==true)
	{
		CString path;
		path.Format("%s\\Sound\\alarm\\", gpcdef.GetDataPathName());
		
		CString text;
		if(pBuf->nOperType == MsgTrainDepartPredict::YUGAO)
		{
			text.Format("%s 向 %s 预告 %s次 发车", 
				gpcdef.GetStationName(pBuf->nSourceStno), 
				gpcdef.GetStationName(pBuf->nDestStno), 
				pBuf->strTid);

			path+="邻站发车预告.wav";
		}
		else if(pBuf->nOperType == MsgTrainDepartPredict::QUXIAO)
		{
			text.Format("%s 取消向 %s 发车 %s次", 
				gpcdef.GetStationName(pBuf->nSourceStno), 
				gpcdef.GetStationName(pBuf->nDestStno), 
				pBuf->strTid);
			path+="邻站取消发车.wav";
		}
		else if(pBuf->nOperType == MsgTrainDepartPredict::TONGYI)
		{
			text.Format("%s 已接受 %s %s次 发车预告", 
				gpcdef.GetStationName(pBuf->nSourceStno), 
				gpcdef.GetStationName(pBuf->nDestStno), 
				pBuf->strTid);
			path+="邻站同意发车.wav";
		}
		else
		{
			text="";
			path="";
		}
				
		if(!text.IsEmpty())
		{
			int pX=0, pY=0;
			for(int y=0; y<3; y++)
			{
				bool bfind=false;
				for(int x=0; x<12; x++)
				{
					if(CDlgTimeMessageBox::WindowPosition[x][y]==0)
					{
						pX=x;
						pY=y;
						bfind=true;
						break;
					}
				}
				if(bfind)
					break;
			}
	
			CDlgTimeMessageBox* m_pMessageDlg = new CDlgTimeMessageBox(text, 30, pX, pY, NULL);
			if(m_pMessageDlg)
			{
				m_pMessageDlg->Create(IDD_DLG_TIME_MESSAGE);
				m_pMessageDlg->ShowWindow(SW_SHOW);
				CRect sz;
				m_pMessageDlg->GetWindowRect(&sz);
				m_pMessageDlg->MoveWindow(pY*50, pX*80, sz.Width(), sz.Height());

				if(path!="")
					PlaySound(path, NULL, SND_FILENAME|SND_ASYNC);
			}
		}
		NoticeMMIUpdateUI(WM_USER+1, 2, 0);
	}
	
	return TRUE;
}

void CTrainDepartPredictHolder::LogPredict(short process_id, const char *fmt, ...)
{
	char tmp[1024];
	memset(tmp, 0, 1024);

	va_list va;
	va_start(va,fmt);
	_vsnprintf(tmp,sizeof(tmp)-1,fmt,va);
	va_end(va);
	tmp[1023]=0;
	
	CTime tm=CTime::GetCurrentTime();
	SaveLogMessageStr("predict", tm.GetTime(), tmp);
}

bool CTrainDepartPredictHolder::IsMyStation(STATION_NO station)
{
	for(int i=0;i<MAX_PREDICT_STATION_PLAN;i++)
	{
		if(m_pStPlans[i])
		{
			if(m_pStPlans[i]->GetStationNo()==station)
				return true;
		}
	}

	return false;
}

bool CTrainDepartPredictHolder::IsMyNeibStation(STATION_NO station, STATION_NO neib_station)
{
	for(int i=0;i<MAX_PREDICT_STATION_PLAN;i++)
	{
		if(m_pStPlans[i])
		{
			if(m_pStPlans[i]->GetStationNo()==station)
			{
				if(m_pStPlans[i]->IsMyNeibStation(neib_station))
					return true;
				else
					return false;
			}
		}
	}

	return false;
}

void CTrainDepartPredictHolder::SaveStplanToHistory()
{
	ULONG nEndTime=0;
	CTime curtm=CTime::GetCurrentTime();
	int  nHour=curtm.GetHour();
	if(nHour>=18 && nHour<20)
	{
		CTime tm(curtm.GetYear(), curtm.GetMonth(), curtm.GetDay(), 18,0,0);
		nEndTime=tm.GetTime();
	}
	else if(nHour>=6 && nHour<8)
	{
		CTime tm(curtm.GetYear(), curtm.GetMonth(), curtm.GetDay(), 6,0,0);
		nEndTime=tm.GetTime();
	}
	else
	{
		return;
	}

	long shiftid=CTG_ScheduleInfo::Get_CurrentShiftid();
	long preshiftid=CTG_ScheduleInfo::Get_PreShiftid(shiftid);
	CString fileName;
	fileName.Format("%s\\predict\\%d_%d.xml", gpcdef.GetWorkPathName(), m_conid, preshiftid);
	
	// 判断历史文件是否存在
	CFileFind finder;

	// 如果文件存在，不进行转储
	if(finder.FindFile(fileName))
		return;
	
	CreateRecursiveDir(fileName);

	CStdioFile file;
	if(!file.Open(fileName, CFile::modeCreate|CFile::modeWrite))
		return;

	file.WriteString("<PREDICT_PLAN>\n");
	// 转储
	for(int i=0;i<MAX_PREDICT_STATION_PLAN;i++)
	{
		if(m_pStPlans[i])
		{
			for(UINT n=0; n<MAX_STPLAN_NUM; n++)
			{
				if(m_pStPlans[i]->m_stplan[n].train_index==0 && m_pStPlans[i]->m_stplan[n].station_no==0)
					break;
				
				if(m_pStPlans[i]->m_stplan[n].train_index==0 && m_pStPlans[i]->m_stplan[n].station_no==65535)
					continue;
				
				if(m_pStPlans[i]->m_stplan[n].start==1)
				{
					if(m_pStPlans[i]->m_stplan[n].depart_time>=nEndTime)
						continue;
				}
				else if(m_pStPlans[i]->m_stplan[n].start==2)
				{
					if(m_pStPlans[i]->m_stplan[n].arrive_time>=nEndTime)
						continue;
				}
				else
				{
					if(m_pStPlans[i]->m_stplan[n].depart_time>=nEndTime)
						continue;
				}

				file.WriteString(GetRecordXML(m_pStPlans[i]->m_stplan[n]));
				m_pStPlans[i]->m_stplan[n].train_index=0;
				m_pStPlans[i]->m_stplan[n].station_no=65535;
			}
			file.Flush();
			
			m_pStPlans[i]->SaveStplanToFile();
		}
	}

	file.WriteString("</PREDICT_PLAN>\n");
	file.Close();

	NoticeMMIUpdateUI(WM_USER+1, 3, 0);
}

// 界面相关
CDepartPredictDlg* CTrainDepartPredictHolder::GetMMI()
{
	if(m_pMMI==NULL)
		return NULL;
	
	if(::IsWindow(m_pMMI->m_hWnd))
		return m_pMMI;
	return NULL;
}

int  CTrainDepartPredictHolder::GetPredictStationList(STATION_NO list[], int num)
{
	int count=0;
	for(int i=0;i<MAX_PREDICT_STATION_PLAN&&count<num;i++)
	{
		if(m_pStPlans[i])
		{
			list[count]= m_pStPlans[i]->GetStationNo();
			count++;
		}
	}

	return count;
}

void CTrainDepartPredictHolder::NoticeMMIUpdateUI(int msgid, int type, int flag)
{
	CDepartPredictDlg* pMMI = GetMMI();
	if(pMMI!=NULL)
	{
		pMMI->SendMessage(msgid, type, flag);
	}
	return;
}

void CTrainDepartPredictHolder::SendStplan(std::vector<stplannotice>& predict_notice, STATION_NO station)
{	
	CStationPredict* pStplanStation = GetStPredictPlan(station);
	if(pStplanStation==NULL)
		return;

	LogPredict(0,"==============================================");
	LogPredict(0,"接收计划 车站:%s(%d) 计划数目:%d", gpcdef.GetStationName(station), station, predict_notice.size());
	if(predict_notice.size()<=0)
	{
		return;
	}

	for(int i=0; i<predict_notice.size(); i++)
	{
		if(predict_notice[i].flag==1)
			LogPredict(0,"删除计划 sta %d train_index %d rec_index %d", station, predict_notice[i].train_index, predict_notice[i].rec_index);
		else
			LogPredict(0,"更新计划 sta %d train_index %d rec_index %d", station, predict_notice[i].train_index, predict_notice[i].rec_index);
	}

	std::vector<stplannotice> notice;
	for(int i=predict_notice.size()-1; i>=0; i--)
	{
		int k=0;
		for(; k<notice.size(); k++)
		{
			if(notice[k].train_index==predict_notice[i].train_index && notice[k].rec_index==predict_notice[i].rec_index)
			{
				break;
			}
		}

		if(k>=notice.size())
		{
			notice.push_back(predict_notice[i]);
		}
	}
		
	if(m_pWorkSchedule==NULL)
		return;

	// 处理计划
	for(int i=0; i<notice.size(); i++)
	{
		if(notice[i].flag==1)
		{
			TrainPredict predict;
			pStplanStation->DeletePredict(notice[i].train_index, notice[i].rec_index, predict);
			LogPredict(0,"删除计划station %d train_index %u rec_index %d", station, notice[i].train_index, notice[i].rec_index);
			this->OutputPredict(predict);
		}
		else
		{
			TrainPredict predict;
			if (OrganizeStplan(predict, notice[i].train_index, notice[i].rec_index, station))
			{
				int nResult=pStplanStation->UpdatePredictPlan(predict);
				if(nResult==0)
				{
					LogPredict(0,"新增计划失败station %d train_index %u rec_index %d 前一站:%d和后一站:%d均不是配置中需要预告的邻站", 
					predict.station_no, predict.train_index, predict.rec_index, 
					predict.pre_station_no,	predict.next_station_no);
				}
				else if(nResult==1)
				{
					LogPredict(0,"更新计划成功station %d train_index %u rec_index %d", 
						predict.station_no, predict.train_index, predict.rec_index);
					this->OutputPredict(predict);
				}
				else if(nResult==2)
				{
					LogPredict(0,"新增计划成功station %d train_index %d rec_index %d", 
						predict.station_no, predict.train_index, predict.rec_index);
					this->OutputPredict(predict);
				}
				else
				{
					LogPredict(0,"新增计划失败station %d train_index %d rec_index %d 超出可容纳计划个数", 
						predict.station_no, predict.train_index, predict.rec_index);
				}
			}
			else
			{
				LogPredict(0,"新增计划失败station %d train_index %u rec_index %d", 
									station, notice[i].train_index, notice[i].rec_index);
			}
		}
	}
	pStplanStation->SaveStplanToFile();
	NoticeMMIUpdateUI(WM_USER+1, 3, 0);
	return;	
}

bool CTrainDepartPredictHolder::OrganizeStplan(TrainPredict& predict, TRAIN_INDEX train_index, short rec_index, STATION_NO station)
{
	CTG_SingleTrain *ptrain=m_pWorkSchedule->GetSingleTrain(train_index);
	if(NULL == ptrain)
	{
		LogPredict(0,"按照train_index %d 找不到列车", train_index);
		return false;
	}
	const CTG_TrainRecord *prec = ptrain->GetTrainRecordNoDelete_Const(rec_index);
	if(NULL == prec) 
	{
		LogPredict(0,"按照rec_index %d 找不到车站", rec_index);
		return false;
	}
	if(prec->GetStationNo() != station) 
	{
		LogPredict(0,"按照rec_index %d 找到的车站%d和实际车站%d不同", 
			rec_index,prec->GetStationNo(),station);
		return false;
	}

	const CTG_TrainRecord* prerec = NULL;
	if(rec_index>0)
		prerec	= ptrain->GetTrainRecordNoDelete_Const(rec_index-1);
	const CTG_TrainRecord *precnext = ptrain->GetTrainRecordNoDelete_Const(rec_index+1);
	
	predict.train_index=ptrain->GetTrainIndex();
	predict.rec_index=prec->GetIndex();
	predict.station_no=prec->GetStationNo();
	
	if(ptrain->IsPassengerTrain())
		predict.passenger=1;
	else
		predict.passenger=2;

	if(prec->IsActualDepartTimeRecord())
		predict.depart_real=1;
	else
		predict.depart_real=0;

	if(prec->IsActualArriveTimeRecord())
		predict.arrive_real=1;
	else
		predict.arrive_real=0;

	predict.start=0;
	if(prerec)
	{
		predict.pre_station_no = prerec->GetStationNo();
		
		strcpy(predict.arrive_train_id, prec->m_data.arrive_train_id);
		predict.entry=prec->m_data.entry;
		
		predict.plan_arrive_time=prec->m_data.plan_arrive;
		predict.arrive_time=prec->m_data.arrive;

		predict.sort=prec->m_data.arrive;
	}
	else
	{
		if(prec->IsStartRec())
		{
			predict.start=1;

			predict.pre_station_no = 0;

			memset(predict.arrive_train_id, 0, sizeof(predict.arrive_train_id));
			predict.entry=0;

			predict.plan_arrive_time=0;
			predict.arrive_time=0;

			predict.sort=prec->m_data.depart;
		}
		else
		{
			WORD pre_station_no=0;
			WORD abs_num=0, dir;
			if(GetTGBasicParam()->ConvertStationEntryToAbsnum(prec->GetStationNo(), prec->GetEntry(), &abs_num, &dir))
			{
				WORD station1, station2, entry1, entry2;
				if(GetTGBasicParam()->ConvertAbsnumToStationEntry(&station1, &entry1,
					&station2, &entry2, abs_num))
				{
					if(station1 == prec->GetStationNo())
						pre_station_no = station2;
					else if(station2 == prec->GetStationNo())
						pre_station_no = station1;
				}
			}

			predict.pre_station_no=pre_station_no;

			strcpy(predict.arrive_train_id, prec->m_data.arrive_train_id);
			predict.entry=prec->m_data.entry;
			
			predict.plan_arrive_time=prec->m_data.plan_arrive;
			predict.arrive_time=prec->m_data.arrive;

			predict.sort=prec->m_data.arrive;
		}
	}
	predict.neib_yugao_time=0;
	predict.tongyi_neib_time=0;
	predict.neib_cancel_time=0;

	if(precnext)
	{
		predict.next_station_no = precnext->GetStationNo();
		predict.next_entry      = precnext->GetEntry();

		strcpy(predict.depart_train_id, prec->m_data.depart_train_id);
		predict.exit=prec->m_data.exit;
		
		predict.plan_depart_time=prec->m_data.plan_depart;
		predict.depart_time=prec->m_data.depart;
	}
	else
	{
		if(prec->IsTerminalRec())
		{
			predict.start=2;
			predict.next_station_no = 0;
			predict.next_entry =0;
			memset(predict.depart_train_id, 0, sizeof(predict.depart_train_id));
			predict.exit				=	0;
			predict.plan_depart_time	=	0;
			predict.depart_time			=	0;
		}
		else
		{
			WORD abs_num=0, dir;
			if(GetTGBasicParam()->ConvertStationEntryToAbsnum(prec->GetStationNo(), prec->GetExit(), &abs_num, &dir))
			{
				WORD next_station_no=0;
				int  next_entry=0; 
				WORD station1, station2, entry1, entry2;
				if(GetTGBasicParam()->ConvertAbsnumToStationEntry(&station1, &entry1,
					&station2, &entry2, abs_num))
				{
					if(station1 == prec->GetStationNo())
					{
						next_station_no = station2;
						next_entry=entry2;
					}
					else if(station2 == prec->GetStationNo())
					{
						next_station_no = station1;
						next_entry=entry1;
					}
				}

				predict.next_station_no	=	next_station_no;
				predict.next_entry      =	next_entry;

				strcpy(predict.depart_train_id, prec->m_data.depart_train_id);
				predict.exit=prec->m_data.exit;
		        
				predict.plan_depart_time=prec->m_data.plan_depart;
				predict.depart_time=prec->m_data.depart;
			}
		}
	}
	predict.yugao_neib_time=0;
	predict.neib_tongyi_time=0;
	predict.cancel_neib_time=0;

	return true;
}

void CTrainDepartPredictHolder::ProcessStationReportTime(TRAIN_INDEX train_index, short rec_index, STATION_NO station, ULONG adjust_status, TIME arrive, TIME depart)
{
	CStationPredict* pstation = NULL;
	pstation = GetStPredictPlan(station);
	if(NULL == pstation)
	{
		LogPredict(0, "找不到车站 %d 的预告计划, 无法处理实际点", station);
		return;
	}
	TrainPredict predict;
	int idx=pstation->GetStplanByTrainIndex(train_index, rec_index, predict);
	if(idx==-1)
	{
		LogPredict(0,"根据train_index %u rec_index %d 找不到计划, 无法处理实际点", train_index, rec_index);
		return;
	}

	if(TG_IsActualArrivalTime(adjust_status) && pstation->m_stplan[idx].arrive_real==0)
	{
		pstation->m_stplan[idx].arrive_time = arrive;
		pstation->m_stplan[idx].arrive_real = 1;

		LogPredict(0,"[自动预告] 接收到车站 %d 到达报点, 设置到点为实际点成功, train_index %d rec_index %d arrive %d", 
				station, train_index, rec_index, arrive);

		pstation->SaveStplanToFile();
		NoticeMMIUpdateUI(WM_USER+1, 2, 0);
	}

	if(TG_IsActualDepartTime(adjust_status) && pstation->m_stplan[idx].depart_real==0)
	{
		pstation->m_stplan[idx].depart_time = depart;
		pstation->m_stplan[idx].depart_real = 1;

		LogPredict(0,"[自动预告] 接收到车站 %d 发车报点, 设置发点为实际点成功, train_index %d rec_index %d depart %d", 
				station, train_index, rec_index, depart);

		pstation->SaveStplanToFile();
		NoticeMMIUpdateUI(WM_USER+1, 2, 0);
	}
}

void CTrainDepartPredictHolder::OutputPredict(const TrainPredict& predict)
{
	LogPredict(0,"计划详细内容:"); 

	LogPredict(0,"车站:%d 列车索引:%u 站序:%d 始发终到:%d 客货:%d",
		predict.station_no, predict.train_index, predict.rec_index, predict.start, predict.passenger);
		
	LogPredict(0,"入口:%d 出口:%d 到达车次:%s 出发车次:%s 图定到达:%u 到达:%u 图定出发:%u 出发:%u 到达是否实际:%d 出发是否实际:%d",
		predict.entry, predict.exit, predict.arrive_train_id, predict.depart_train_id, 
		predict.plan_arrive_time, predict.arrive_time, 
		predict.plan_depart_time, predict.depart_time, predict.arrive_real, predict.depart_real); 

	LogPredict(0,"前方站:%d 邻站同意时间:%u 邻站预告时间:%u 邻站取消时间:%u",
		predict.pre_station_no, predict.neib_tongyi_time, predict.neib_yugao_time, predict.neib_cancel_time); 
	
	LogPredict(0,"后方站:%d 后方站入口:%d 同意邻站时间:%u 预告邻站时间:%u 取消邻站时间:%u",
		predict.next_station_no, predict.next_entry, predict.tongyi_neib_time, predict.yugao_neib_time, predict.cancel_neib_time); 
}
