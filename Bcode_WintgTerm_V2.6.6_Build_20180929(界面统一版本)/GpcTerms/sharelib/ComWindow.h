#pragma once
#include "CDunicomm.h"

// CComWindow

class CComWindow : public CWnd
{
	DECLARE_DYNAMIC(CComWindow)

public:
	CComWindow();
	virtual ~CComWindow();
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

public:
	CDUnicomm m_cUniComm;

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
//////////////////////////////////////
	DECLARE_EVENTSINK_MAP()
	void NewMessageUnicomm(long source_id, long dest_id,long label,long tm,const VARIANT& msg);
	void EntityStatusChangedUnicomm(long entity_id, short status);
	void CommStatusChangedUnicomm(short status);
	void MessageAckedUnicomm(long source_id,long dest_id,long label);
	void NewXMLMessageUnicomm(long source_id,long dest_id, long label, long tm,const char *msgstr);
///////////////////////////////////////
	afx_msg void OnClose();
private:
#define MAX_BROKEN_LOG  5   // 60s 内中断5次
#define BROKEN_LOG_RANGE 60 // 60 s
	time_t   tmBrokenLog[MAX_BROKEN_LOG];

};


// 向综合维护系统发送状态信息
