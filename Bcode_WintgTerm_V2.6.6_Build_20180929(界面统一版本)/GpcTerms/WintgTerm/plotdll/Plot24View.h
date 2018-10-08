// Plot24View.h: interface for the CPlot24View class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLOT24VIEW_H__3833AA5A_1815_416B_AABF_4A60736A2B87__INCLUDED_)
#define AFX_PLOT24VIEW_H__3833AA5A_1815_416B_AABF_4A60736A2B87__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PlotView.h"
#include "Plot24Doc.h"


#endif // !defined(AFX_PLOT24VIEW_H__3833AA5A_1815_416B_AABF_4A60736A2B87__INCLUDED_)
/////////////////////////////////////////////////////////////////////////////
// CPlot24View view
class CPlot24View : public CPlotView
{
protected:
	CPlot24View();           // protected constructor used by dynamic creation
	virtual ~CPlot24View();
	DECLARE_DYNCREATE(CPlot24View)

// Attributes
public:
	BYTE *m_picBuf;
	UINT m_picWidth;
	UINT m_picHeight;

// Operations
private:
	void PutFileToServer(CString filePathName, CString fileName);
public:
	int  DelPictures(time_t elapsedtime);
	BOOL LoadBMP(CString fileName);
	BOOL SaveJPG(CString fileName, BOOL color);
	BOOL SaveBmp(HBITMAP hBitmap, CString FileName);
public:
	void Query_StationCar();
	void Query_Phase();
	void Query_Rem();
	void Query_Subao();
	void Query_GetDdyName();
	
	void OnSubaoData(CLBTH::MsgSubaoFormat*  pMsg);
	void OnStationCurrentStoredTrainFlag(CLBTH::MsgSimpleStationCarCtrl* pMsg);
	void OnStationCurrentStoredTrain(CLBTH::MsgSimpleStationCarFormat* pMsg);
    void OnNotepadSheetMsg(CLBTH::MsgNotePadFormat *pMsg);
    void OnRealPhaseMsg(CLBTH::MsgWordPhasePlanSend *pMsg);

	LRESULT OnCommMsg(WPARAM wParam, LPARAM lParam); 

	void OnPlotOutput();
public:
	
	virtual CPlotDoc *GetDocument( );
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlot24View)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	//}}AFX_VIRTUAL

// Implementation
protected:
	
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CPlot24View)
	afx_msg void OnSavepicture();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()	
};

/////////////////////////////////////////////////////////////////////////////
