#if !defined(AFX_TG_H__13E61F94_7BF3_11D1_9CF0_000021000CE6__INCLUDED_)
#define AFX_TG_H__13E61F94_7BF3_11D1_9CF0_000021000CE6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// tg.h : header file
//
#include "defines.h"
#include "gpcdef.h"
#include "resource.h"
#include "mainframe.h"
#include "plotinform.h"
#include "MyCmdl.h"

class CTgDocument;
/////////////////////////////////////////////////////////////////////////////
// CWintg thread
struct CGPCContext
{
	ULONG mShiftID;
	WORD  mSectionId;
	short mPixelPerMinute;
	BYTE  mStatsicTime;
	BYTE  mLineWidth;
	__time64_t mTmRun;
	int   mReverseFlag[2][20]; //0:基本图 1:非基本图
	CGPCContext()
	{
		mShiftID=0;
		mSectionId=0;
		mPixelPerMinute=0;
		mStatsicTime=0;
		mLineWidth=0;
		mTmRun=0;
		memset(mReverseFlag, 0, sizeof(mReverseFlag));
	}
};

class  CWintg : public CWinApp
{
	DECLARE_DYNCREATE(CWintg)
public:
	CWintg( );           // protected constructor used by dynamic creation
	virtual ~CWintg();
// Attributes
public:
	int OpenNewDocument(void);
	void PlotSchedule(CTgDocument *pdoc,int sche_type,const CString& plot_param, const int plotflag);
	bool OpenPlotWindow(PRINTINFORM *pPrintInform, CTgDocument *pdoc, const CString& plot_param);
    CView *GetOpenedPlotWindow();
	bool IsPlotWindowOpened();
	bool SendMessageToPlotWindow(UINT msg,WPARAM wp,LPARAM lp);
	CView* GetTYPlanSignDlgParent();

// Operations
public:
	CMultiDocTemplate *m_pWintgPlotFormDoc;
	
	///////// NEWTG //////////////////
	CMultiDocTemplate *m_pNewTGDocTemplate;
	CMultiDocTemplate *m_pNewTGDocTemplateMMIPlan;
	CMultiDocTemplate *m_pNewTGDocTemplateTYPlan;
	CMultiDocTemplate *m_pNewTGDocTemplateWork;
	CMultiDocTemplate *m_pNewTGDocTemplateShift;
	CMultiDocTemplate *m_pNewTGDocTemplateBasic;
	//////////////////////////////////
	ushort m_hTgTaskId;

	void AddDocTemplate(CDocTemplate *pDocTemplate);
	int GetOpenDocumentCount();
	POSITION GetFirstDocTemplatePosition() const;
	CDocTemplate *GetNextDocTemplate(POSITION & rPosition) const;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWintg)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle(LONG lCount);
	virtual BOOL IsIdleMessage(MSG* pMsg);
	virtual int Run();
	//}}AFX_VIRTUAL

// Implementation
protected:
	int OnFileNew( );
	CDocManager * m_pDocManager;
	CMyCmdL  m_cmdline;
	CWnd *m_pAlarmWnd;

	// Generated message map functions
	//{{AFX_MSG(CWintg)
		// NOTE - the ClassWizard will add and remove member functions here.
	   afx_msg void OnShowAlarm();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	bool InitTask();
	BOOL PumpMessage( );
public:  // NEW
	BOOL CreateScheduleWindow(int section_id, int schd_type,int shift_id,BOOL force_query=FALSE,BOOL can_close=TRUE);
	BOOL CreateScheduleWindowDync(int parent_section_id, int section_id,int  schd_type,int shift_id,BOOL force_query);
	
	CMainFrame *GetMainFrame() {return (CMainFrame *)GetMainWnd();}
	
	BOOL  CreateMMIPlanWindow(int section_id,int shift_id,BOOL force_query);
	BOOL  CreateTYPlanWindow(int section_id,int shift_id,BOOL force_query);

	CFrameWnd * FindScheduleWindow(int section_id,int  schd_type,int shift_id, int& nFlag);
	CFrameWnd * FindScheduleWindow(int parent_section_id, int section_id,int  schd_type);
	
	CFrameWnd * FindScheduleWindowOneTg(int section_id,int  schd_type,int& nFlag);

	void  CloseSectionIdFrame(int nSectionId);
	void  CloseBackupBasicFrame(int nSectionId);
	bool  ClearTGOCXSchedule(int section_id,int  schd_type);

private:
	BOOL LoadTGNOXML(CString pathname);
	void InitMSLink();
public:
	bool bWinTGExit; // 运行图程序是否退出

public:
	void CheckDisconnectWithCommsvr();
	int  m_nDisconnectWithCommsvr;

public:
	int m_nFirstReqWork;
	int m_nFirstReqBasic;

public:
	BOOL ProgramOnce(LPCSTR szAppID, BOOL bForceStartup);

public:
	struct STARTUP_INFO
	{
		CString szName;
		HANDLE hSemaphore;
	};
	STARTUP_INFO startup;
};

extern CWintg  theWintgApp;
extern CGPCContext m_gpcContext;
/////////////////////////////////////////////////////////////////////////////
/*CWintg * WintgAllocate( );
/void WintgDeallocate(CWintg *psWintg);
BOOL WintgCreateThread(CWintg *psWintg,DWORD dwCreateFlags, 
	UINT nStackSize, 
	LPSECURITY_ATTRIBUTES lpSecurityAttrs);*/
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TG_H__13E61F94_7BF3_11D1_9CF0_000021000CE6__INCLUDED_)
