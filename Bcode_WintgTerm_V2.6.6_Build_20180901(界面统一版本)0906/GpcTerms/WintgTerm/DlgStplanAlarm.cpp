// DlgStplanAlarm.cpp : implementation file
//

#include "stdafx.h"
#include "memshare.h"
#include "resource.h"
#include "DlgStplanAlarm.h"
#include ".\dlgstplanalarm.h"
#include "mmsystem.h"

#import  <msxml.dll> named_guids
using namespace MSXML;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgStplanAlarm dialog


CDlgStplanAlarm::CDlgStplanAlarm(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgStplanAlarm::IDD, pParent)
{
	m_known=false;
}

void CDlgStplanAlarm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDlgStplanAlarm, CDialog)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgMessageBox message handlers
BOOL CDlgStplanAlarm::OnInitDialog() 
{
	CDialog::OnInitDialog();
	InitDlgPosition(this);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

bool CDlgStplanAlarm::IsAlarm()
{
	bool bArrival=false;
	CTime tm=CTime::GetCurrentTime();
	for(int i=0; i<m_StplanAlarmTmList.size(); i++)
	{
		if(tm.GetHour()==m_StplanAlarmTmList[i].hour
			&& tm.GetMinute()==m_StplanAlarmTmList[i].minute)
		{
			bArrival=true;
			break;
		}
	}

	if(bArrival)
	{
		if(!m_known)
			return true;
	}
	else
	{
		m_known=false;
	}
	return false;
}

void CDlgStplanAlarm::Alarm()
{
	CString path;
	path.Format("%s\\Sound\\alarm\\请下达阶段计划.wav", gpcdef.GetDataPathName());
	CFileFind find;
	if(find.FindFile(path))
		PlaySound(path, NULL, SND_FILENAME|SND_ASYNC);
	find.Close();
	this->SetForegroundWindow();
	GpcLogTrainMessage(0,0,"请注意,该下发阶段计划了!");
}

void CDlgStplanAlarm::OnBnClickedOk()
{
	CString txt;
	txt.Format("[计划报警] 调度员操作:知道了.");
	GpcLogTrainMessage(0,0,txt);
	this->ShowWindow(SW_HIDE);
	m_known=true;
}

void CDlgStplanAlarm::OnClose()
{
}

BOOL CDlgStplanAlarm::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_CANCEL))
	{
		return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

bool CDlgStplanAlarm::LoadStplanAlarmXML(int section_id)
{
	m_StplanAlarmTmList.clear();

	CString strFilename;
	strFilename.Format("%s\\stplan_alarm.xml", gpcdef.GetDataPathName());

	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		return false;
	}

	if(VARIANT_TRUE != pXMLDoc->load( _bstr_t( strFilename) ) )
	{
		return false; 
	}

	_variant_t v;
	MSXML::IXMLDOMNodePtr pNode,pChildNode, pTimeNode,pRoot;
	pRoot=pXMLDoc->selectSingleNode("StplanAlarm");

	MSXML::IXMLDOMNodeListPtr pList = pRoot->selectNodes( "Section");
	if(pList==NULL)
		return false;

	bool bStplanAlarm=false;
	for(int idx = 0; idx < pList->Getlength(); idx++)
	{
		pNode = pList->Getitem(idx);
		if(pNode==NULL)
			continue;

		int conid=-1;
		pChildNode=pNode->selectSingleNode("@id");
		if(pChildNode)
		{
			v = pChildNode->GetnodeTypedValue();
			conid = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}

		if(conid != section_id)
			continue;

		bStplanAlarm=true;

		MSXML::IXMLDOMNodeListPtr pTimeNodeList=pNode->selectNodes("Time");
		if(pTimeNodeList)
		{
			for (int m=0;m<pTimeNodeList->Getlength();m++)
			{	
				pTimeNode=pTimeNodeList->Getitem(m);
				if (pTimeNode==NULL)
					continue;

				int hour=99, minute=99;
				pChildNode=pTimeNode->selectSingleNode("@hour");
				if(pChildNode)
				{
					v = pChildNode->GetnodeTypedValue();
					hour = atoi(CString(v.bstrVal));
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				pChildNode=pTimeNode->selectSingleNode("@minute");
				if(pChildNode)
				{
					v = pChildNode->GetnodeTypedValue();
					minute = atoi(CString(v.bstrVal));
					SysFreeString(v.bstrVal);
					v.Clear();
				}

				CString text1="", text2="";
				if(hour<0 || hour>23)
				{
					text1.Format("小时 %d 不对, 合法值(0<=hour<=23)", hour);
				}
				if(minute<0 || minute>59)
				{
					text2.Format("分钟 %d 不对, 合法值(0<=minute<=59)", minute);
				}

				if(text1=="" && text2 == "")
				{
					AlarmTM alarmTm;
					alarmTm.hour=hour;
					alarmTm.minute=minute;
					m_StplanAlarmTmList.push_back(alarmTm);
				}
				else
				{
					AfxMessageBox("本台配置计划下达提醒" + text1 + text2);
				}
			}
		}
	}

	if(bStplanAlarm) 
	{
		if(m_StplanAlarmTmList.size()>0)
			return true;
		else
			return false;// 有该台配置但是没有时间,报错
	}
	else // 没有该台配置不报错
	{
		return true; 
	}
}


