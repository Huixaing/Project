#include "stdafx.h"
#include "Tg_BLOCK.h"

CTg_BLOCK::CTg_BLOCK()
{
	Clear();
}
CTg_BLOCK::CTg_BLOCK(const CTg_BLOCK& rhs)
{
	Copy(rhs);
}
CTg_BLOCK& CTg_BLOCK::operator=(const CTg_BLOCK& rhs)
{
	if(this == &rhs)
		return (*this);
	
	Copy(rhs);

	return *this;
}
CTg_BLOCK::~CTg_BLOCK()
{
	this->dwFlag = 0;
}

void CTg_BLOCK::RestoreDrawFlag()
{
	m_bFocus = FALSE;
	bMoveCenter = FALSE;
	bMoveRight = FALSE;
	bMoveLeft = FALSE;
	bMoveTop = FALSE;
	bMoveBottom = FALSE;
}
void CTg_BLOCK::Clear()
{
	this->combstation = 0;
	this->mark_index = 0;
	this->start_time = -1;
	this->end_time = -1;
	this->type = 0;

	this->flag = 0;
	this->line = 0;
	this->station = 0;
	this->index = 0;
	this->position = 0;
	this->count = 0;
	memset(ele_number, 0, sizeof(ele_number));
	memset(text, 0, sizeof(text));
	
	this->s_meter = 0;
	this->e_meter = 0;
	this->e_kilo = 0;

	this->slow_time = 0;
	this->station1 = 0;
	this->station2 = 0;
	this->direction = 0;

	// add your code here
	this->bFlag = false;
	this->dwFlag = 0;
	this->adjust_status = 0;

	this->bMoveCenter = FALSE;
	this->bMoveRight = FALSE;
	this->bMoveLeft = FALSE;
	this->bMoveTop = FALSE;
	this->bMoveBottom = FALSE;
	this->m_bFocus = FALSE;
	
	mColor = RGB(255,0,0);
	myPosition = CRect(0,0,0,0);
	m_nUpdateTime=0;
}

void CTg_BLOCK::Copy(const CTg_BLOCK& rhs)
{
	this->mark_index = rhs.mark_index;
	this->combstation = rhs.combstation;

	this->start_time = rhs.start_time;
	this->end_time = rhs.end_time;

	this->type = rhs.type;

	this->flag = rhs.flag;
	this->line = rhs.line;
	this->station = rhs.station;
	this->index = rhs.index;
	this->position = rhs.position;
	this->count = rhs.count;
	memcpy(ele_number, rhs.ele_number, sizeof(ele_number));
	strncpy(text, rhs.text, sizeof(rhs.text));
	
	this->s_meter = rhs.s_meter;
	this->e_meter = rhs.e_meter;
	this->e_kilo = rhs.e_kilo;
	this->slow_time = rhs.slow_time;

	this->station1 = rhs.station1;
	this->station2 = rhs.station2;
	this->direction = rhs.direction;

	// add your code here
	this->bFlag = rhs.bFlag;
	this->dwFlag = 0;

	this->adjust_status = rhs.adjust_status;

	this->bMoveCenter = rhs.bMoveCenter;
	this->bMoveRight = rhs.bMoveRight;
	this->bMoveLeft = rhs.bMoveLeft;
	this->bMoveTop = rhs.bMoveTop;
	this->bMoveBottom = rhs.bMoveBottom;
	this->m_bFocus = rhs.m_bFocus;

	this->mColor = rhs.mColor;

	this->cRect = rhs.cRect;
	this->lRect = rhs.lRect;
	this->rRect = rhs.rRect;
	this->tRect = rhs.tRect;
	this->bRect = rhs.bRect;
	this->m_drawRect = rhs.m_drawRect;
	this->myPosition = rhs.myPosition;
	this->m_nUpdateTime = rhs.m_nUpdateTime;
}

//--------------------------------------
void CTg_BLOCK::SetColor(COLORREF newColor)
{
	mColor = newColor; 
}

CString CTg_BLOCK::GetKiloMDesp()
{
	CString ret="";
	
	if(position != 0 || s_meter != 0 || e_meter != 0 || e_kilo != 0)
	{
		ret.Format("%dkm%dm至%dkm%dm",position,s_meter,e_kilo,e_meter);
	}
	
	return ret;
}

bool CTg_BLOCK::GetKiloMValue(ULONG& beginKilo, ULONG& endKilo)
{
	beginKilo=0;
	endKilo=0;
	if(position!=0 || s_meter!=0)
		beginKilo = position*1000 + s_meter;
	if(e_kilo!=0 || e_meter!=0)
		endKilo = e_kilo*1000 + e_meter;

	if(beginKilo == 0 && endKilo == 0)
		return false;
	
	return true;
}

void CTg_BLOCK::SetReasonTextFromTDMS()
{
	CString strKm = GetKiloMDesp();
	if(!strKm.IsEmpty())
	{
		CString kmtext = this->text;
		for(int i=0; i<kmtext.GetLength() && i<=(kmtext.GetLength()-strKm.GetLength()); i++)
		{
			CString token = kmtext.Mid(i,strKm.GetLength());
			if(token == strKm)
			{
				token = kmtext.Left(i);
				token  += kmtext.Right(kmtext.GetLength()-i-strKm.GetLength());

				memset(text, 0, sizeof(text));
				strncpy(text, token, sizeof(text)-1);
				text[sizeof(text)-1]=0;
				break;
			}
		}
	}
}

CString CTg_BLOCK::toXML(DWORD nResult)
{
	CString xml,tmp;

	xml = "<MARK ";
	{
		tmp.Format(" nResult=\"%d\"", nResult);
		xml += tmp;
        tmp.Format(" basic_index=\"%d\"", combstation);
		xml += tmp;
		tmp.Format(" mark_index=\"%d\"", mark_index);
		xml += tmp;
		tmp.Format(" start_time=\"%d\"", start_time);
		xml += tmp;
		tmp.Format(" end_time=\"%d\"", end_time);
		xml += tmp;
		tmp.Format(" type=\"%d\"", type);
		xml += tmp;
		tmp.Format(" flag=\"%d\"", flag);
		xml += tmp;
		tmp.Format(" line=\"%d\"", line);
		xml += tmp;
		tmp.Format(" station=\"%d\"", station);
		xml += tmp;
		tmp.Format(" index=\"%d\"", index);
		xml += tmp;
		tmp.Format(" position=\"%d\"", position);
		xml += tmp;

		
		// 控制text文本长度
		CString strText = text;
		if(!strText.IsEmpty())
		{
			if(strText.GetLength() >= MAX_SCHD_BLOCK_TEXT_LEN)
			{
				text[MAX_SCHD_BLOCK_TEXT_LEN-1] = 0;
				strText = text;
			}
		}

		tmp.Format(" text=\"%s\"", GetValidStr(strText));
		xml += tmp;
		tmp.Format(" s_meter=\"%d\"", s_meter);
		xml += tmp;
		tmp.Format(" e_meter=\"%d\"", e_meter);
		xml += tmp;
		tmp.Format(" e_kilo=\"%d\"", e_kilo);
		xml += tmp;
		tmp.Format(" slow_time=\"%d\"", slow_time);
		xml += tmp;
		tmp.Format(" station1=\"%d\"", station1);
		xml += tmp;
		tmp.Format(" station2=\"%d\"", station2);
		xml += tmp;
		tmp.Format(" direction=\"%d\"", direction);
		xml += tmp;
		tmp.Format(" adjust_status=\"%d\"", adjust_status);
		xml += tmp;

		xml += ">";

		for(int idx =0; idx < count; idx++)
		{
			tmp.Format("<ele_number ele_val=\"%d\"/>", ele_number[idx]);
			xml += tmp;
		}
	}
	xml += "</MARK>";

	return xml;
}

CString CTg_BLOCK::toString()
{
	CString xml,tmp;

	xml = "";
	
	tmp.Format(" start_time=\"%d\"", start_time);
	xml += tmp;
	tmp.Format(" end_time=\"%d\"", end_time);
	xml += tmp;
	tmp.Format(" type=\"%d\"", type);
	xml += tmp;
	tmp.Format(" flag=\"%d\"", flag);
	xml += tmp;
	tmp.Format(" line=\"%d\"", line);
	xml += tmp;
		
	return xml;
}

void CTg_BLOCK::OffsetPosition(int x)
{
	cRect.OffsetRect(x, 0);
	lRect.OffsetRect(x, 0);
	rRect.OffsetRect(x, 0);
	tRect.OffsetRect(x, 0);
	bRect.OffsetRect(x, 0);
	m_drawRect.OffsetRect(x, 0);
	myPosition.OffsetRect(x, 0);
}

bool CTg_BLOCK::IfInTimeScope(TIME stime, TIME etime) const
{
	if(start_time>=stime && start_time<=etime)
		return true;
	if(end_time>=stime && end_time<=etime)
		return true;
	if(start_time<=stime && end_time>=etime)
		return true;
	return false;
}
