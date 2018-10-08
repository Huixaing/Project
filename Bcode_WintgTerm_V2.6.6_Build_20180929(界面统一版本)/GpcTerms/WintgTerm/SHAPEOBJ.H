
#ifndef _DRAW_SHAPE_HEADER_
#define _DRAW_SHAPE_HEADER_


enum SHAPE_STYLE
{
	rectangle,
	rounded_rectangle,
	ellipse,
};

enum SHAPE_COLOR_ENUM
{
	black,
	red,
	green,
	blue,
};

struct SHAPE_COLOR
{
	SHAPE_COLOR_ENUM e;
	operator COLORREF() const;
	const SHAPE_COLOR& operator = (COLORREF colorref);
	operator int() const;
	const SHAPE_COLOR& operator = (int n);
};

class CShape : public CObject
{
public:
	CShape();
	CShape(SHAPE_COLOR_ENUM colorenum, SHAPE_STYLE shapestyle, CRect& rect);
	DECLARE_SERIAL(CShape)

	SHAPE_COLOR m_shapecolor;
	SHAPE_STYLE m_shapestyle;
	CRect       m_rect;

	void Draw(CDC* pDC, BOOL bSelected);
	void SetRect(CRect& rect);
	virtual void Serialize(CArchive& ar);
};

#endif