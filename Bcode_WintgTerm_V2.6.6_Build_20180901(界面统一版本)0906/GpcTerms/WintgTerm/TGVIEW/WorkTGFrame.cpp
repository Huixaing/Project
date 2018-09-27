// TGFrame.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "WorkTGFrame.h"
#include "sysshare.h"
#include "tgview.h"
#include "function.h"
#include "tgmsg.h"
#include "alarm/SoundPlay.h"
#include "mmsystem.h"
#include "utility\mmi_function_config.h"
// CTGFrame
BOOL   CheckAndLaunchMemoryNotify();

IMPLEMENT_DYNCREATE(CWorkTgFrame, CTgFrame)

CWorkTgFrame::CWorkTgFrame()
{
}

CWorkTgFrame::~CWorkTgFrame()
{
}


BEGIN_MESSAGE_MAP(CWorkTgFrame, CTgFrame)
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_COMMAND(ID_WIN_MEMORY, OnMemoryWnd)
	ON_MESSAGE(UM_MEMORY_NOIFY,OnMemoryNotify)
END_MESSAGE_MAP()

// CTGFrame 消息处理程序
int CWorkTgFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
		CTgFrame::OnCreate(lpCreateStruct);

		AddButtonToBar(8,"鼠标收点",IDC_RECEIVE_POINT,BS_CHECKBOX);
		AddButtonToBar(9,"转实际",ID_TURN_REALITY,0);
		
		AddButtonToBar(10,"下计划",IDC_SEND_STATION_PLAN,0);
		AddButtonToBar(11,"接收计划",ID_WORK_GET_TRAIN_FROM_OPMS,BS_OWNERDRAW);
		AddButtonToBar(12,"备忘提醒",ID_WIN_MEMORY,BS_OWNERDRAW);
		AddButtonToBar(13,"邻台计划",IDC_RICEIVE_PLAN_1,BS_OWNERDRAW);
		AddButtonToBar(14,"开始画线",ID_INSERT_NEW_TRAIN,0);
		AddButtonToBar(15,"结束画线",ID_DRAW_NEW_TRAIN_END,0);
		AddButtonToBar(16,"日班计划",ID_MANUAL_SELECT_TRAIN,0);
		AddButtonToBar(17,"图标注释",ID_TEXT_NOTE,0);
		if(GetTGFunctionConfig()->IsEnableTempAddTrain())
			AddButtonToBar(18,"临时加车",ID_TEMP_ADD_TRAIN,0,FALSE);
		else
			AddButtonToBar(18,"事故概况",ID_SHIGU_OUTLINE,0,FALSE);
		AddButtonToBar(19,"列车作业",ID_TRAIN_WORK_PLAN,0);
		AddButtonToBar(20,"调度命令",IDC_NEW_DC_COMMAND,0);
		SetTimer(7306,1000,NULL);
		SetTimer(7321,60000,NULL); // 语音提示
		SetTimer(7905,1000,NULL);
		m_nNotifyAlert=0;
		m_nNotifyPlan=0;
		m_nNotifyNeigPlan=0;

		return 0;
}

void CWorkTgFrame::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent==7321) //每分钟
	{
	    if(m_nNotifyPlan!=0)
			PlaySimpleAlarmSound(1001,0);  // 阶段计划提示语音
		else
		if(m_nNotifyAlert!=0)
			PlaySimpleAlarmSound(1002,0);  // 备忘提醒语音
	}
	else
	if(nIDEvent==7306) //每秒
	{
		extern DWORD g_bMemoryNotifing;
		if(g_bMemoryNotifing)
		{
			if(m_nNotifyAlert==0)
				m_nNotifyAlert=1;

			if(m_nNotifyAlert==1)
			{
				m_pButtons[12]->Attach(0,NULL,RGB(255,0,0));
				m_nNotifyAlert=2;
			}
			else
			if(m_nNotifyAlert==2)
			{
				m_pButtons[12]->Attach(0,NULL,RGB(192,192,192));
				m_nNotifyAlert=1;
			}
		}
		else
		{
			if(m_nNotifyAlert != 0)
			{
				m_nNotifyAlert=0;
				m_pButtons[12]->Attach(0,NULL,RGB(192,192,192));
			}
		}

		if(m_nNotifyPlan==1)
		{
			m_pButtons[11]->Attach(0,NULL,RGB(255,0,0));
			m_pButtons[11]->Invalidate();  
			m_nNotifyPlan=2;
		}
		else
		if(m_nNotifyPlan==2)
		{
			m_pButtons[11]->Attach(0,NULL,RGB(192,192,192));
			m_pButtons[11]->Invalidate();
			m_nNotifyPlan=1;
		}
	}
	else
	if(nIDEvent==7905) //每秒
	{
		CTgDocument *pDoc = (CTgDocument*)this->GetActiveDocument();
		if(pDoc != NULL)
		{
			if(pDoc->m_vectNeigPlan.size() > 0)
			{
				if(m_nNotifyNeigPlan==0)
				{
					m_pButtons[13]->Attach(0,NULL,RGB(255,0,0));
					m_pButtons[13]->Invalidate();
					m_nNotifyNeigPlan = 1;

					if(GetTGFunctionConfig()->IsNeibPlanPlaySound())
					{
						if(m_nSoundCout%3==0)
						{
							CString path;
							path.Format("%s\\Sound\\alarm\\邻台调整计划下达.wav", gpcdef.GetDataPathName());
							PlaySound(path, NULL, SND_FILENAME|SND_ASYNC);
							m_nSoundCout=1;
						}
						else
						{
							m_nSoundCout++;
						}
					}
				}
				else
				{
					m_pButtons[13]->Attach(0,NULL,RGB(192,192,192));
					m_pButtons[13]->Invalidate();
					m_nNotifyNeigPlan = 0;
				}
			}
			else
			{
				m_nSoundCout=0;
				m_pButtons[13]->Attach(0,NULL,RGB(192,192,192));
				//m_pButtons[13]->Invalidate();
				m_nNotifyNeigPlan=0;
			}
		}
		else
		{
			m_nNotifyNeigPlan=0;
		}
	}
}
void CWorkTgFrame::OnMemoryWnd()
{
	CheckAndLaunchMemoryNotify();

	AfxGetMainWnd()->PostMessage(UM_MEMORY_NOIFY,1,0); 
}
LRESULT CWorkTgFrame::OnMemoryNotify(WPARAM wParam, LPARAM lParam)
{
	if(wParam==0) // diable alert
	{
		m_nNotifyAlert=0;
		m_pButtons[12]->Attach(0,NULL,RGB(192,192,192));
	}
	else
	if(wParam==1)
		m_nNotifyAlert=1;
	else
	if(wParam==2) // diable plan alert
	{
		m_nNotifyPlan=0;
		m_pButtons[11]->Attach(0,NULL,RGB(192,192,192));
		m_pButtons[11]->Invalidate();
	}
	else
	if(wParam==3)
		m_nNotifyPlan=1;
	
	return 0;
}