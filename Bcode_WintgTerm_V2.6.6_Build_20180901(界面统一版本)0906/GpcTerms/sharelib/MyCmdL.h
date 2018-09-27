// MyCmdL.h: interface for the CMyCmdL class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYCMDL_H__7FB28CF5_A263_11D5_9296_0004AC45F4D7__INCLUDED_)
#define AFX_MYCMDL_H__7FB28CF5_A263_11D5_9296_0004AC45F4D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMyCmdL : public CCommandLineInfo  
{
	
	CString   m_lastflag;					
public:
	enum{OPEN_WINDOW_WORK=0x00000001,OPEN_WINDOW_BASIC=0x00000002,OPEN_WINDOW_SHIFT=0x00000004,OPEN_WINDOW_MMPLAN=0x00000008,OPEN_WINDOW_TYPLAN=0x00000010};
	bool      m_bIgnServer;
	bool      m_bAutoLogin;
	CString   m_userid;
	CString   m_username;
	CString   m_password;
	CString   m_datapath;
	CString   m_gpcname;
	CString   m_commcfgfile;
	CString   m_entityfile;
	CString   m_tgcfgfile;
	int       m_db_mode;
	bool      m_bQueryMode;
	bool      m_bShowAlarm;
	bool      m_bStplanAlarm;
	bool      m_bCommSvrMode;
	bool      m_bGSMRCommMode;
    unsigned long m_shiftid;
	long     m_server_entity_id;
	short    m_nLogLevel;
	unsigned short   m_nLogTypeMask;
	unsigned long    m_nInitOpenWindow;
	int      m_nStartCMDIndex;
	int      m_nEndCMDIndex;
	CMyCmdL();
	virtual ~CMyCmdL();
	virtual void ParseParam( LPCTSTR lpszParam, BOOL bFlag, BOOL bLast );
};

#endif // !defined(AFX_MYCMDL_H__7FB28CF5_A263_11D5_9296_0004AC45F4D7__INCLUDED_)
