#include "stdafx.h"
#include "resource.h"
#include <io.h>
#include <afxinet.h>
#include "TGPspData.h"
#include "utility/xml_utility.h"
#import  <msxml.dll> named_guids
using namespace MSXML;
#define  GET_NODE_STRVALUE(dest,pXMLDoc,node) \
  {MSXML::IXMLDOMNodePtr pNode = pXMLDoc->selectSingleNode(_bstr_t(node)); \
   if(pNode) \
			{ \
				_variant_t v; \
				v = pNode->GetnodeTypedValue(); \
				dest = CString((v.bstrVal)); \
				SysFreeString(v.bstrVal); \
                v.Clear(); \
			} \
			else dest.Empty();};


	
BOOL GetViewInfoFromMMPLAN(CString &dest,const char *xml,const char *node)
{
	 MSXML::IXMLDOMDocumentPtr pXMLDoc;
	 HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	  if(FAILED(hr))
		{
			_com_error er(hr);
			AfxMessageBox(er.ErrorMessage());
			return FALSE;
		}
	  
	  if(VARIANT_TRUE != pXMLDoc->loadXML( _bstr_t( xml ) ) )
		{
			return FALSE;
		} 
	  MSXML::IXMLDOMNodePtr pNodeRoot =pXMLDoc->selectSingleNode(_bstr_t(node));
	  if(pNodeRoot==NULL)
		  return FALSE;
	  CString PLANID,DBH,MID,WORKPOS,LINENM,DIR,WORKKM,WORKTP,PJNM,UNITNM,LEADER,RANK,WKCON,SPLIM,LIMINFO;
	  /*
		  MSXML::IXMLDOMNodePtr pNode = pXMLDoc->selectSingleNode(_bstr_t(node)); 
			if(pNode) 
			{ 
				_variant_t v; 
				v = pNode->GetnodeTypedValue(); 
				dest = CString((v.bstrVal)); 
				SysFreeString(v.bstrVal); 
                v.Clear(); 
			} 

			dest.Empty();*/
	  GET_NODE_STRVALUE(PLANID,pNodeRoot,"PLANID")
	  GET_NODE_STRVALUE(DBH,pNodeRoot,"DBH")
	  GET_NODE_STRVALUE(MID,pXMLDoc,"MID")
	  GET_NODE_STRVALUE(WORKPOS,pNodeRoot,"WORKPOS")
	  GET_NODE_STRVALUE(LINENM,pNodeRoot,"LINENM")
	  GET_NODE_STRVALUE(DIR,pNodeRoot,"DIR")
	  GET_NODE_STRVALUE(WORKKM,pNodeRoot,"WORKKM")
	  GET_NODE_STRVALUE(WORKTP,pNodeRoot,"WORKTP")
	  GET_NODE_STRVALUE(PJNM,pNodeRoot,"PJNM")
	  GET_NODE_STRVALUE(UNITNM,pNodeRoot,"UNITNM")
	  GET_NODE_STRVALUE(LEADER,pNodeRoot,"LEADER")

	  GET_NODE_STRVALUE(RANK,pNodeRoot,"RANK")
	  GET_NODE_STRVALUE(WKCON,pNodeRoot,"WKCON")
	  GET_NODE_STRVALUE(SPLIM,pNodeRoot,"SPLIM")
	  GET_NODE_STRVALUE(LIMINFO,pNodeRoot,"LIMINFO")

	  CString title;
	  title.Format("月计划:%s 电报:%s,地点: %s,线路:%s,方向:%s,里程:%s,类型:%s,项目:%s,等级:%s",
			MID,DBH,WORKPOS,LINENM,DIR,WORKKM,WORKTP,PJNM,RANK);
	  CString unitstr;
	  unitstr.Format("单位名称:%s  施工负责人:%s",UNITNM,LEADER);

	
	  InsertXMLNodeValue(dest,"TITLE",title);
	  InsertXMLNodeValue(dest,"UNITNM",unitstr);
	  InsertXMLNodeValue(dest,"WKCON",WKCON);
	  InsertXMLNodeValue(dest,"SPLIM",SPLIM);
	  InsertXMLNodeValue(dest,"LIMINFO",LIMINFO);
	 
	  return FALSE;
}


BOOL GetListInfoFromMMPLAN(CString &dest,const char *xml,const char *node)
{
	 MSXML::IXMLDOMDocumentPtr pXMLDoc;
	 HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	  if(FAILED(hr))
		{
			_com_error er(hr);
			AfxMessageBox(er.ErrorMessage());
			return FALSE;
		}
	  
	  if(VARIANT_TRUE != pXMLDoc->loadXML( _bstr_t( xml ) ) )
		{
			return FALSE;
		} 
	  MSXML::IXMLDOMNodePtr pNodeRoot =pXMLDoc->selectSingleNode(_bstr_t(node));
	  if(pNodeRoot==NULL)
		  return FALSE;
	  CString PLANID,TM,STTM,EDTM,WORKPOS,LINENM,WORKTP;
	  GET_NODE_STRVALUE(PLANID,pNodeRoot,"PLANID")
	  GET_NODE_STRVALUE(WORKPOS,pNodeRoot,"WORKPOS")
	  GET_NODE_STRVALUE(WORKTP,pNodeRoot,"WORKTP")
	  GET_NODE_STRVALUE(LINENM,pNodeRoot,"LINENM")
	  GET_NODE_STRVALUE(TM,pNodeRoot,"TM")
	  GET_NODE_STRVALUE(STTM,pNodeRoot,"STTM")
	  GET_NODE_STRVALUE(EDTM,pNodeRoot,"EDTM")
	  
	  CString title;
	  title.Format("地点: %s,线路:%s,类型:%s",
			WORKPOS,LINENM,WORKTP);
	  
	  InsertXMLNodeValue(dest,"PLANID",PLANID);
	  InsertXMLNodeValue(dest,"TITLE",title);
	  InsertXMLNodeValue(dest,"TM",TM);
	  InsertXMLNodeValue(dest,"STTM",STTM);
	  InsertXMLNodeValue(dest,"EDTM",EDTM);
	  
	 
	  return FALSE;
}
