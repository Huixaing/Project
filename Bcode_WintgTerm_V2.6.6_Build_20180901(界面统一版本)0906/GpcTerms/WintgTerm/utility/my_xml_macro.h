#pragma once
#define LOAD_XML(xml,node,pNodeX) \
	MSXML::IXMLDOMNodePtr pNodeX; \
{MSXML::IXMLDOMDocumentPtr pXMLDoc; \
	 HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);\
	  if(FAILED(hr)) \
		{ \
			_com_error er(hr); \
			AfxMessageBox(er.ErrorMessage()); \
			return FALSE; \
		} \
	  if(VARIANT_TRUE != pXMLDoc->loadXML( _bstr_t( xml ) ) ) \
		{  \
			return FALSE; \
		}  \
	  pNodeX =pXMLDoc->selectSingleNode(_bstr_t(node)); \
	  if(pNodeX==NULL) \
	  return FALSE; }

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

#define  GET_NODE_INTVALUE(dest,pXMLDoc,node) \
  {MSXML::IXMLDOMNodePtr pNode = pXMLDoc->selectSingleNode(_bstr_t(node)); \
   if(pNode) \
			{ \
				_variant_t v; \
				v = pNode->GetnodeTypedValue(); \
				dest = atoi(CString((v.bstrVal))); \
				SysFreeString(v.bstrVal); \
                v.Clear(); \
			} \
			else dest=-1;};

#define  GET_NODE_TIMEVALUE(dest,pXMLDoc,node) \
  {MSXML::IXMLDOMNodePtr pNode = pXMLDoc->selectSingleNode(_bstr_t(node)); \
   if(pNode) \
			{ \
				_variant_t v; \
				v = pNode->GetnodeTypedValue(); \
				dest = XMLStringToTime((CString((v.bstrVal)))); \
				SysFreeString(v.bstrVal); \
                v.Clear(); \
			} \
			else dest=-1;};

#define  GET_NODE_UINTVALUE(dest,pXMLDoc,node) \
  {MSXML::IXMLDOMNodePtr pNode = pXMLDoc->selectSingleNode(_bstr_t(node)); \
   if(pNode) \
			{ \
				_variant_t v; \
				v = pNode->GetnodeTypedValue(); \
				dest = atoi(CString((v.bstrVal))); \
				SysFreeString(v.bstrVal); \
                v.Clear(); \
			} \
			else dest=0;};