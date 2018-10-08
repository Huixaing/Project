// ComWindow.cpp : 实现文件
//

#include "stdafx.h"
#include "ComWindow.h"
#include "msgdispatch.h"
#include "icommtask.h"
#include ".\comwindow.h"
#include "memshare.h"
#include <vector>

extern int ProcessCommMessage(const L3_MSG_HEADER *msghdr,BYTE  *ptr, int length);
extern int LocalProcessEntityStatus(int status);
extern int LocalProcessConnectStatus(int status);
#define IDC_UNICOMM   1000

// CComWindow

IMPLEMENT_DYNAMIC(CComWindow, CWnd)
CComWindow::CComWindow()
{
	for(int i=0;i<MAX_BROKEN_LOG;i++)
	   tmBrokenLog[i]=0;
}

CComWindow::~CComWindow()
{
	DestroyWindow();
}



BEGIN_EVENTSINK_MAP(CComWindow, CWnd)
	ON_EVENT(CComWindow, IDC_UNICOMM, 1, NewMessageUnicomm, VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_VARIANT)
	ON_EVENT(CComWindow, IDC_UNICOMM, 2, EntityStatusChangedUnicomm, VTS_I4 VTS_I2)
	ON_EVENT(CComWindow, IDC_UNICOMM, 3, CommStatusChangedUnicomm, VTS_I2)
	ON_EVENT(CComWindow, IDC_UNICOMM, 4, MessageAckedUnicomm, VTS_I4 VTS_I4 VTS_I4)
END_EVENTSINK_MAP()


COMM_EVENT_HANDLER  g_pCommEventHandler=NULL;
bool  RegisterCommEventHandler(COMM_EVENT_HANDLER p)
{
	  g_pCommEventHandler=p;
	  return true;
}

void CComWindow::NewMessageUnicomm(long source_id,long dest_id, long label, long tm,const VARIANT& msg)
{
	COleSafeArray varRcv;
	BYTE *pdata;
	varRcv=msg;
    varRcv.AccessData((LPVOID*)&pdata);
	int datalen=varRcv.GetOneDimSize();  
	if(pdata)
	{
		L3_MSG_HEADER hdr;
		memset(&hdr,0,sizeof(hdr));
		hdr.source_id=source_id;
		hdr.dest_id=dest_id;
		hdr.label=label;
		hdr.generate_time.tm=tm;  
		ProcessCommMessage(&hdr,pdata, datalen);  // 外部定义全局函数
	}
	else
		GpcLogSysMessage(0, "Recv: NewMessageUnicomm src_entity=%d ,des_entity=%d  数据为空",source_id, dest_id);
	varRcv.UnaccessData();
}

void CComWindow::EntityStatusChangedUnicomm(long entity_id, short status)
{
	GpcLogSysMessage(0, "Recv: EntityStatusChangedUnicomm entity=%d , status=%d",entity_id, status);
	LocalProcessEntityStatus(status);
	if(g_pCommEventHandler)
	  (*g_pCommEventHandler)(1,entity_id,status);
}

void CComWindow::CommStatusChangedUnicomm(short status)
{
	GpcLogSysMessage(0, "Recv: CommStatusChangedUnicomm status=%d",status);

	LocalProcessConnectStatus(status);
	if(g_pCommEventHandler)
		  (*g_pCommEventHandler)(0,0,status);
}

void CComWindow::MessageAckedUnicomm(long source_id,long dest_id,long label) // 需要自动确认应答消息反馈
{

}

BEGIN_MESSAGE_MAP(CComWindow, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CLOSE()
END_MESSAGE_MAP()



// CComWindow 消息处理程序


int CComWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rect(0,0,0,0);
	if(!m_cUniComm.Create(NULL, WS_CHILD|WS_VISIBLE,rect,this,IDC_UNICOMM))
		return -1;
	return 0;
}

void CComWindow::OnSize(UINT nType, int cx, int cy)
{
	m_cUniComm.MoveWindow(0,0,cx,cy);
}


void CComWindow::OnClose()
{
	ShowWindow(SW_HIDE);
}
