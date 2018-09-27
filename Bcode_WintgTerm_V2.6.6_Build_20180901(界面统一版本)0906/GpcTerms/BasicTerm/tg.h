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
	WORD  mSectionId;
	short mPixelPerMinute;
	BYTE  mStatsicTime;
	BYTE  mLineWidth;
	__time64_t mTmRun;
	int   mReverseFlag[2][20]; //0:基本图 1:非基本图
	CGPCContext()
	{
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
	
// Operations
public:
	CMultiDocTemplate *m_pWintgPlotFormDoc;
	CMultiDocTemplate *m_pTemplateBasic;
	CMultiDocTemplate *m_pTemplatePreBasic;
	CMultiDocTemplate *m_pTemplateBackBasic;
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
	int OnFileNew();
	CDocManager * m_pDocManager;
	CMyCmdL  m_cmdline;
	DECLARE_MESSAGE_MAP()
	bool InitTask();
	BOOL PumpMessage( );
public:  // NEW
	BOOL CreateScheduleWindow(int section_id, int schd_type, BOOL can_close=TRUE);
	
	CMainFrame *GetMainFrame() {return (CMainFrame *)GetMainWnd();}
	
	CFrameWnd * FindScheduleWindow(int section_id,int schd_type, int& nFlag);
		
	void  CloseSectionIdFrame(int nSectionId);
	void  CloseBackupBasicFrame(int nSectionId);
	bool  ClearTGOCXSchedule(int section_id,int  schd_type);

private:
	BOOL LoadTGNOXML(CString pathname);
public:
	bool bWinTGExit; // 运行图程序是否退出

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
