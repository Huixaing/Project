#pragma once


// CMMIPlanDocument 文档

class CMMIPlanDocument : public CDocument
{
	DECLARE_DYNCREATE(CMMIPlanDocument)

public:
	CMMIPlanDocument();
	virtual ~CMMIPlanDocument();
	virtual void Serialize(CArchive& ar);   // 为文档 I/O 重写
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
};
