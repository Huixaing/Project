
#include "stdafx.h"
#include "BaseTrain.h"

CBaseTrain::CBaseTrain()
{
	InitSingleTrain();
}

CBaseTrain::CBaseTrain(const CBaseTrain& rhs)
{
	CopyTrain(rhs); 
}
CBaseTrain& CBaseTrain::operator=(const CBaseTrain& rhs)
{
	if(this == &rhs)
		return *this;

	CopyTrain(rhs);
		
	return *this;
}

CBaseTrain::~CBaseTrain( )
{
}

void CBaseTrain::CopyTrain(const CBaseTrain& rhs)
{
	m_nTrainType = rhs.m_nTrainType;

	m_nDirectFlag = rhs.m_nDirectFlag; 
	
	ptoftrainid_count = rhs.ptoftrainid_count;
	for(int i= 0;i<ptoftrainid_count;i++)
		stPttoTrainid[i].Set(rhs.stPttoTrainid[i]);

	m_nPointCount = rhs.m_nPointCount;
	for(int i= 0;i<m_nPointCount;i++)
		m_pPerPointInfo[i].Set(rhs.m_pPerPointInfo[i]);
	
	for(int i = 0; i < 8; i++)
		m_nTerminalMarkRect[i] = rhs.m_nTerminalMarkRect[i];
	m_nTerminalMarkedRectCount = rhs.m_nTerminalMarkedRectCount;  

	for(int i = 0; i < MAX_DAY_OF_BASIC_TRAIN; i++)
		myPosition[i] = rhs.myPosition[i];
	days = rhs.days;
	
	m_nUpdateTime = rhs.m_nUpdateTime;

	for(int i = 0; i < 8; i++)
		m_nShowTrainIDRect[i] = rhs.m_nShowTrainIDRect[i];
	m_nShowTrainIDNum=rhs.m_nShowTrainIDNum;
	
}

void CBaseTrain::InitSingleTrain()
{
	m_nTrainType = TYPE::PASSENGER;

	m_nDirectFlag = 1; 

	ptoftrainid_count = 0;
	for(int i= 0;i<16;i++)
		stPttoTrainid[i].init();

	for(int i= 0;i<MAX_REC;i++)
	   m_pPerPointInfo[i].init();
	m_nPointCount = 0;

	for(int i = 0; i < MAX_DAY_OF_BASIC_TRAIN; i++)
		myPosition[i].SetRectEmpty();
	days = 0;

	m_nShowTrainIDNum=0;
	for(int i = 0; i < 8; i++)
		m_nShowTrainIDRect[i].SetRectEmpty();

	m_nUpdateTime = 0;

	m_nTerminalMarkedRectCount = 0; 
	for(int i = 0; i < 8; i++)
		m_nTerminalMarkRect[i].SetRectEmpty();
}

CPoint CBaseTrain::GetContinuePoint( )
{
	if(m_nPointCount > 0)
		return m_pPerPointInfo[m_nPointCount-1].point;
	else 
		return CPoint(0,0);
}

void CBaseTrain::InitShowTrainIDRect()
{
	m_nShowTrainIDNum=0;
	for(int i=0; i<8; i++)
		m_nShowTrainIDRect[i].SetRectEmpty();

	m_nTerminalMarkedRectCount=0;
	for(int i=0; i<8; i++)
		m_nTerminalMarkRect[i].SetRectEmpty();
}

void CBaseTrain::AddShowTrainIDRect(CRect& rect)
{
	if(m_nShowTrainIDNum>=8)
		return;
	m_nShowTrainIDRect[m_nShowTrainIDNum]=rect;
	m_nShowTrainIDNum++;
}

bool CBaseTrain::IsInShowTrainIDRect(const CPoint& pt)
{
	for(int i=0; i<m_nShowTrainIDNum; i++)
	{
		if(m_nShowTrainIDRect[i].PtInRect(pt))
			return true;
	}

	return false;
}

bool  CBaseTrain::IsSameSelRect(int n, int m)
{
	if((n>=0&&n<m_nPointCount) && (m>=0&&m<m_nPointCount))
	{
		if((m_pPerPointInfo[n].record_index == m_pPerPointInfo[m].record_index)
		  &&(m_pPerPointInfo[n].start_depart_flag == m_pPerPointInfo[m].start_depart_flag)
		  &&(m_pPerPointInfo[n].nSubSection == m_pPerPointInfo[m].nSubSection)
		  &&(m_pPerPointInfo[n].point == m_pPerPointInfo[m].point))
		{
			return true;
		}
	}

	return false;
}

void CBaseTrain::AddTrainTerminalRect(int uStartX, int uStartY, int uEndX, int uEndY)
{
	if(m_nTerminalMarkedRectCount>=8)
		return;

	CRect rect(uStartX, uStartY, uEndX, uEndY);
	m_nTerminalMarkRect[m_nTerminalMarkedRectCount]=rect;
	m_nTerminalMarkedRectCount++;
}

bool CBaseTrain::IsInTrainTerminalRect(const CRect& tmprect)
{
	CRect rect;
	int j=0;
	for(; j <= m_nTerminalMarkedRectCount; j++)
	{	 
		if(rect.IntersectRect(m_nTerminalMarkRect[j], tmprect))
		{
			break;	
		}
	}
	if(j <= m_nTerminalMarkedRectCount)
		return true;
	else
		return false;
}

void CBaseTrain::OffsetPosition(int x)
{
	for(int i=0; i<m_nTerminalMarkedRectCount; i++)
	{
		m_nTerminalMarkRect[i].OffsetRect(x,0);
	}
	
	for(int i=0; i<days; i++)
	{
		myPosition[i].OffsetRect(x,0);
	}

	for(int i=0; i<m_nShowTrainIDNum; i++)
	{
		m_nShowTrainIDRect[i].OffsetRect(x,0);
	}
	
	for(int i=0; i<ptoftrainid_count; i++)
	{
		stPttoTrainid[i].ptActual.Offset(x,0);
	}

	for(int i=0; i<m_nPointCount; i++)
	{
		m_pPerPointInfo[i].point.Offset(x,0);
	}
}
