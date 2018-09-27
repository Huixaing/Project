#pragma once

#include "StdAfx.h"
#include "dbfunc2.h"
#include <vector>

#import  <msxml.dll> named_guids
using namespace MSXML;

struct ifTRAINRECORD 
{
	std::vector<Pstruct TRAINRECORD> recs;

	CString toXMLString();
	bool fromXML(MSXML::IXMLDOMNodePtr pRoot);
	CString toXMLStringOneRec();
	bool fromXMLOneRec(MSXML::IXMLDOMNodePtr pRoot);
};

struct ifONETRAINRECORD 
{
	Pstruct TRAINRECORD rec;

	CString toXMLString();
	bool fromXML(MSXML::IXMLDOMNodePtr pRoot);
};

struct ifTRAININFO 
{
	Pstruct TRAIN_INFO info;

	CString toXMLString();
	bool fromXML(MSXML::IXMLDOMNodePtr pRoot, int& result);
	enum RETURN_VAL
	{
		STA_NAME_TOO_LONG = 1,
		TRAIN_ID_TOO_MORE,
		TRAIN_ID_TOO_LONG,
		ENGINE_ID_TOO_MORE,
		ENGINE_ID_TOO_LONG,
		STA_TEXT_TOO_MORE,
		STA_TEXT_TOO_LONG,
		ABS_TEXT_TOO_MORE,
		ABS_TEXT_TOO_LONG,
		LOAD_TEXT_TOO_MORE,
		LOAD_TEXT_TOO_LONG,
		SPARE_TOO_LONG
	};
};

struct ifMARKITEM 
{
	Pstruct _tagMarkItem item;
	CString toXMLString();
	bool fromXML(MSXML::IXMLDOMNodePtr pRoot, long nMarkIndex, int& result);
	enum RETURN_VAL
	{
		TEXT_TOO_LONG = 1,
	};
};

struct ifADDONETRAINBYROUTE
{
	struct ADDONETRAINBYROUTE
	{
		short complex_status;
		long date;
		long time;
		int start_mode;
		int end_mode;
	};

	ADDONETRAINBYROUTE data;

	CString toXMLString();
	bool fromXML(MSXML::IXMLDOMNodePtr pRootNode);
};