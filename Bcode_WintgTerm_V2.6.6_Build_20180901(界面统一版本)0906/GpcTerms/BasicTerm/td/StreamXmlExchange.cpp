#include "StdAfx.h"
#include ".\streamxmlexchange.h"
#import <msxml4.dll>
CStreamXmlExchange::CStreamXmlExchange(void)
{
}

CStreamXmlExchange::~CStreamXmlExchange(void)
{
}

//0x01应答
//<ACK_BZ><VER></VER><SUBVER></SUBVER><BZ_RECORD></BZ_RECORD><BZ_RECORD><BZ_RECORD>...</ACK_BZ>
int CStreamXmlExchange::AckBzStreamToxml(char *src,int len,char *dest)
{
	ASSERT(src!=NULL);
	ASSERT(dest!=NULL);
	int ver=(unsigned char)src[0];
	int subver=(unsigned char)src[1];
	CString strtmp;
	CString str;
	strtmp.Format("%d",ver);
	str=CreateNode("VER",strtmp);
	strtmp.Format("%d",subver);
	str+=CreateNode("SUBVER",strtmp);
	src=src+2;
	strtmp=StreamBzRecordToXml(src,len-2);
	str+=strtmp;
	strtmp = CreateNode("ACK_BZ",str);
	memcpy(dest,(LPTSTR)(LPCTSTR)strtmp,strtmp.GetLength());
	dest[strtmp.GetLength()]=0;
	return strtmp.GetLength()+1;
}

//0x01请求
int CStreamXmlExchange::ReqBzXmlToStream(char *src,int len,char*dest)
{
	ASSERT(src!=NULL);
	int streamlen=0;
	int stnmlen=0;
	char *name=NULL;
	DWORD gid=0;
	BYTE  adflag=0;
	MSXML2::IXMLDOMDocumentPtr pDoc;
	HRESULT hr = pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument40), NULL, CLSCTX_INPROC_SERVER);
	if( SUCCEEDED(hr) && VARIANT_TRUE == pDoc->loadXML(src))
	{
		MSXML2::IXMLDOMNodeListPtr pNodeList_Text = pDoc->selectNodes("REQ_BZ");
		ASSERT(pNodeList_Text!=NULL);
		do
		{
			MSXML2::IXMLDOMNodePtr pNode_Text = pNodeList_Text->Getitem(0);
			MSXML2::IXMLDOMNodePtr pNode;
			CString str;
			_variant_t varVal;

			if(pNode = pNode_Text->selectSingleNode("STNM"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				stnmlen = str.GetLength();
				name = new char[stnmlen+1];
				memset(name,0,stnmlen+1);
				memcpy(name,(LPTSTR)(LPCTSTR)str,stnmlen);
				*(USHORT*)dest=stnmlen;
				streamlen=2;
				memcpy(dest+streamlen,name,stnmlen);
				streamlen+=stnmlen;
				delete []name;
			}else
			{
				*(USHORT*)dest=0;
				streamlen=2;
			}
			if(pNode = pNode_Text->selectSingleNode("GID"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				gid = atoi(str);
				*(DWORD*)(dest+streamlen)=gid;
				streamlen+=4;
			}else
			{
				*(DWORD*)(dest+streamlen)=0;
				streamlen+=4;
			}
			if(pNode = pNode_Text->selectSingleNode("ADFLAG"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				adflag = atoi(str);
				*(BYTE*)(dest+streamlen)=adflag;
				streamlen+=1;
			}else
			{
				*(BYTE*)(dest+streamlen)=0;
				streamlen+=1;
			}
		}while(0);
	}else
		return -1;
	return streamlen;
}

//0x02应答
//<ACK_YL><GID></GID><TNO></TNO><IFLAG></IFLAG><OFLAG></OFLAG><STST></STST><ENDST></ENDST><LSTTM></LSTTM>
//<LEDTM></LEDTM><STTM></STTM><FJTM></FJTM><ST_RECORD></ST_RECORD><ST_RECORD><ST_RECORD>...</ACK_YL>
int CStreamXmlExchange::AckYLByGidStreamToxml(char *src,int len,char *dest)
{
	ASSERT(src!=NULL);
	ASSERT(dest!=NULL);
	CString strtmp;
	CString strp;
	//[GID]
	DWORD gid = *(DWORD*)src;
	strtmp.Format("%d",gid);
	strp=CreateNode("GID",strtmp);
	//[TNO]
	src=src+4;
	int tnolen=*(USHORT*)src;
	src=src+2;
	strtmp="";
	memcpy((LPTSTR)(LPCTSTR)strtmp,src,tnolen);
	strp+=CreateNode("TNO",strtmp);
	//[IFLAG]
	src=src+tnolen;
	strtmp="";
	int iflag=(unsigned char)src[0];
	strtmp.Format("%d",iflag);
	strp+=CreateNode("IFLAG",strtmp);
	//[OFLAG]
	src=src+1;
	strtmp="";
	int oflag=(unsigned char)src[0];
	strtmp.Format("%d",oflag);
	strp+=CreateNode("OFLAG",strtmp);
	//[STST]
	src=src+1;
	int ststlen=*(USHORT*)src;
	src=src+2;
	strtmp="";
	memcpy((LPTSTR)(LPCTSTR)strtmp,src,ststlen);
	strp+=CreateNode("STST",strtmp);
	//[ENDST]
	src=src+ststlen;
	int endstlen=*(USHORT*)src;
	src=src+2;
	strtmp="";
	memcpy((LPTSTR)(LPCTSTR)strtmp,src,endstlen);
	strp+=CreateNode("ENDST",strtmp);
	//[LSTTM]
	src=src+endstlen;
	strtmp = StreamTMtoXml(src,7);
	strp+=CreateNode("LSTTM",strtmp);
	//[LEDTM]
	src=src+7;
	strtmp = StreamTMtoXml(src,7);
	strp+=CreateNode("LEDTM",strtmp);
	//[STTM]
	src=src+7;
	strtmp = StreamTMtoXml(src,7);
	strp+=CreateNode("STTM",strtmp);
	//[FJTM]
	src=src+7;
	strtmp = StreamTMtoXml(src,7);
	strp+=CreateNode("FJTM",strtmp);
	//车站详细信息
	src=src+7;
	strp+=StreamYlSTinfoToXml(src);
	strtmp=CreateNode("ACK_YL",strp);
	memcpy(dest,(LPTSTR)(LPCTSTR)strtmp,strtmp.GetLength());
	dest[strtmp.GetLength()]=0;
	return strtmp.GetLength()+1;
}

//0x02请求
int CStreamXmlExchange::ReqYLByGidXmlToStream(char *src,int len,char *dest)
{
	ASSERT(src!=NULL);
	int streamlen=0;
	BYTE sec[7];
	int stnmlen=0;
	char *stnm=NULL;
	int linenum=0;
	DWORD gid=0;
	MSXML2::IXMLDOMDocumentPtr pDoc;
	HRESULT hr = pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument40), NULL, CLSCTX_INPROC_SERVER);
	if( SUCCEEDED(hr) && VARIANT_TRUE == pDoc->loadXML(src))
	{
		MSXML2::IXMLDOMNodeListPtr pNodeList_Text = pDoc->selectNodes("REQ_YL");
		ASSERT(pNodeList_Text!=NULL);
		do
		{
			MSXML2::IXMLDOMNodePtr pNode_Text = pNodeList_Text->Getitem(0);
			MSXML2::IXMLDOMNodePtr pNode;
			CString str;
			_variant_t varVal;

			if(pNode = pNode_Text->selectSingleNode("SEC"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				memset(sec,0,sizeof(sec));
				memcpy(sec,str,str.GetLength());
				memcpy(dest,sec,6);
				streamlen=6;
			}else
			{
				streamlen=6;
			}
			if (pNode = pNode_Text->selectSingleNode("STNM"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				stnmlen = str.GetLength();
				stnm = new char[stnmlen+1];
				memset(stnm,0,stnmlen+1);
				memcpy(stnm,(LPTSTR)(LPCTSTR)str,stnmlen);
				*(USHORT*)(dest+streamlen)=stnmlen;
				streamlen+=2;
				memcpy(dest+streamlen,stnm,stnmlen);
				streamlen+=stnmlen;
				delete []stnm;
			}else
			{
				*(USHORT*)(dest+streamlen)=0;
				streamlen+=2;
			}
			if(pNode = pNode_Text->selectSingleNode("GID"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				int i=0;
				char *token=NULL;
				token=strtok((LPTSTR)(LPCTSTR)str,",");
				int tmpstreamlen=streamlen;
				streamlen+=2;
				while(token!=NULL)
				{
					gid = atoi(token);
					i++;
					*(DWORD*)(dest+streamlen)=gid;
					streamlen+=4;
					token=strtok(NULL,",");
				}
				*(USHORT*)(dest+tmpstreamlen)=i;
			}else
			{
				*(USHORT*)(dest+streamlen)=0;
				streamlen+=2;
			}
		}while(0);
	}else
		return -1;
	return streamlen;
}

//0x03应答
int CStreamXmlExchange::AckYLbyTimeStreamToxml(char *src,int len,char *dest)
{
	return AckYLByGidStreamToxml(src,len,dest);
}

//0x03请求
int CStreamXmlExchange::ReqYLbyTimeXmlToStream(char *src,int len,char *dest)
{
	ASSERT(src!=NULL);
	int streamlen=0;
	BYTE disid[7];
	MSXML2::IXMLDOMDocumentPtr pDoc;
	HRESULT hr = pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument40), NULL, CLSCTX_INPROC_SERVER);
	if( SUCCEEDED(hr) && VARIANT_TRUE == pDoc->loadXML(src))
	{
		MSXML2::IXMLDOMNodeListPtr pNodeList_Text = pDoc->selectNodes("REQ_YL");
		ASSERT(pNodeList_Text!=NULL);
		do
		{
			MSXML2::IXMLDOMNodePtr pNode_Text = pNodeList_Text->Getitem(0);
			MSXML2::IXMLDOMNodePtr pNode;
			CString str;
			_variant_t varVal;

			if(pNode = pNode_Text->selectSingleNode("DISID"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				memset(disid,0,sizeof(disid));
				memcpy(disid,str,str.GetLength());
				memcpy(dest,disid,6);
				streamlen=6;
			}else
			{
				streamlen=6;
			}
			if (pNode = pNode_Text->selectSingleNode("STTM"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				BYTE tm[8];
				memset(tm,0,sizeof(tm));
				int len = XmlTmtoStream((LPTSTR)(LPCTSTR)str,(char*)tm,7);
				memcpy(dest+streamlen,tm,7);
				streamlen+=7;
			}else
			{
				streamlen+=7;
			}

			if (pNode = pNode_Text->selectSingleNode("EDTM"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				BYTE tm[8];
				memset(tm,0,sizeof(tm));
				int len = XmlTmtoStream((LPTSTR)(LPCTSTR)str,(char*)tm,7);
				memcpy(dest+streamlen,tm,7);
				streamlen+=7;
			}
			else
			{
				streamlen+=7;
			}
			if(pNode = pNode_Text->selectSingleNode("STFG"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				BYTE stfg=atoi(str);
				*(BYTE*)(dest+streamlen)=stfg;
				streamlen+=1;
			}
			else
			{
				streamlen+=1;
			}
			if(pNode = pNode_Text->selectSingleNode("PAFG"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				BYTE pafg=atoi(str);
				*(BYTE*)(dest+streamlen)=pafg;
				streamlen+=1;
			}
			else
			{
				streamlen+=1;
			}
			if(pNode = pNode_Text->selectSingleNode("CAFG"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				BYTE cafg=atoi(str);
				*(BYTE*)(dest+streamlen)=cafg;
				streamlen+=1;
			}
			else
			{
				streamlen+=1;
			}
			if(pNode = pNode_Text->selectSingleNode("STNM"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				*(USHORT*)(dest+streamlen)=str.GetLength();
				streamlen+=2;
				memcpy((dest+streamlen),(LPTSTR)(LPCTSTR)str,str.GetLength());
				streamlen+=str.GetLength();
			}
			else
			{
				streamlen+=2;
			}
		}while(0);
	}else
		return -1;
	return streamlen;
}

//0x04应答
int CStreamXmlExchange::AckYLbyTnoStreamToxml(char *src,int len,char *dest)
{
	return AckYLByGidStreamToxml(src,len,dest);
}

//0x04请求
int CStreamXmlExchange::ReqYLbyTnoXmlToStream(char *src,int len,char *dest)
{
	ASSERT(src!=NULL);
	int streamlen=0;
	BYTE disid[7];
	MSXML2::IXMLDOMDocumentPtr pDoc;
	HRESULT hr = pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument40), NULL, CLSCTX_INPROC_SERVER);
	if( SUCCEEDED(hr) && VARIANT_TRUE == pDoc->loadXML(src))
	{
		MSXML2::IXMLDOMNodeListPtr pNodeList_Text = pDoc->selectNodes("REQ_YL");
		ASSERT(pNodeList_Text!=NULL);
		do
		{
			MSXML2::IXMLDOMNodePtr pNode_Text = pNodeList_Text->Getitem(0);
			MSXML2::IXMLDOMNodePtr pNode;
			CString str;
			_variant_t varVal;

			if(pNode = pNode_Text->selectSingleNode("DISID"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				memset(disid,0,sizeof(disid));
				memcpy(disid,str,str.GetLength());
				memcpy(dest,disid,6);
				streamlen=6;
			}else
			{
				streamlen=6;
			}
			if(pNode = pNode_Text->selectSingleNode("TNO"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				*(USHORT*)(dest+streamlen)=str.GetLength();
				streamlen+=2;
				memcpy((dest+streamlen),(LPTSTR)(LPCTSTR)str,str.GetLength());
				streamlen+=str.GetLength();
			}
			else
			{
				streamlen+=2;
			}
			if (pNode = pNode_Text->selectSingleNode("STTM"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				BYTE tm[8];
				memset(tm,0,sizeof(tm));
				int len = XmlTmtoStream((LPTSTR)(LPCTSTR)str,(char*)tm,7);
				memcpy(dest+streamlen,tm,7);
				streamlen+=7;
			}
			else
			{
				streamlen+=7;
			}
			if (pNode = pNode_Text->selectSingleNode("EDTM"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				BYTE tm[8];
				memset(tm,0,sizeof(tm));
				int len = XmlTmtoStream((LPTSTR)(LPCTSTR)str,(char*)tm,7);
				memcpy(dest+streamlen,tm,7);
				streamlen+=7;
			}
			else
			{
				streamlen+=7;
			}
		}while(0);
	}else
		return -1;
	return streamlen;
}

//0x05应答
int CStreamXmlExchange::AckYLbyStartTimeAndGidStreamToxml(char *src,int len,char *dest)
{
	return AckYLByGidStreamToxml(src,len,dest);
}

//0x05请求
int CStreamXmlExchange::ReqYLbyStartTimeAndGidXmlToStream(char *src,int len,char*dest)
{
	ASSERT(src!=NULL);
	int streamlen=0;
	BYTE disid[7];
	MSXML2::IXMLDOMDocumentPtr pDoc;
	HRESULT hr = pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument40), NULL, CLSCTX_INPROC_SERVER);
	if( SUCCEEDED(hr) && VARIANT_TRUE == pDoc->loadXML(src))
	{
		MSXML2::IXMLDOMNodeListPtr pNodeList_Text = pDoc->selectNodes("REQ_YL");
		ASSERT(pNodeList_Text!=NULL);
		do
		{
			MSXML2::IXMLDOMNodePtr pNode_Text = pNodeList_Text->Getitem(0);
			MSXML2::IXMLDOMNodePtr pNode;
			CString str;
			_variant_t varVal;

			if(pNode = pNode_Text->selectSingleNode("DISID"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				memset(disid,0,sizeof(disid));
				memcpy(disid,str,str.GetLength());
				memcpy(dest,disid,6);
				streamlen=6;
			}
			else
			{
				streamlen=6;
			}
			if(pNode = pNode_Text->selectSingleNode("TNO"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				*(USHORT*)(dest+streamlen)=str.GetLength();
				streamlen+=2;
				memcpy((dest+streamlen),(LPTSTR)(LPCTSTR)str,str.GetLength());
				streamlen+=str.GetLength();
			}
			else
			{
				streamlen+=2;
			}
			if (pNode = pNode_Text->selectSingleNode("STTM"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				BYTE tm[8];
				memset(tm,0,sizeof(tm));
				int len = XmlTmtoStream((LPTSTR)(LPCTSTR)str,(char*)tm,7);
				memcpy(dest+streamlen,tm,7);
				streamlen+=7;
			}
			else
			{
				streamlen+=7;
			}
		}while(0);
	}else
		return -1;
	return streamlen;
}

//0x06应答
int CStreamXmlExchange::AckGidbyTnoStreamToxml(char *src,int len,char *dest)
{
	ASSERT(src!=NULL);
	ASSERT(dest!=NULL);
	CString strtmp="";
	CString strp="";
	//[TNO]
	int tnolen=*(USHORT*)src;
	src=src+2;
	memcpy((LPTSTR)(LPCTSTR)strtmp,src,tnolen);
	strp=CreateNode("TNO",strtmp);
	//[GID]
	src=src+tnolen;
	DWORD gid=*(DWORD*)src;
	strtmp.Format("%d",gid);
	strp+=CreateNode("GID",strtmp);
	strtmp = CreateNode("ACK_GID",strp);
	memcpy(dest,(LPTSTR)(LPCTSTR)strtmp,strtmp.GetLength());
	dest[strtmp.GetLength()]=0;
	return strtmp.GetLength()+1;
}

//0x06请求
int CStreamXmlExchange::ReqGidbyTnoXmlToStream(char *src,int len,char *dest)
{
	ASSERT(src!=NULL);
	int streamlen=0;
	BYTE disid[7];
	MSXML2::IXMLDOMDocumentPtr pDoc;
	HRESULT hr = pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument40), NULL, CLSCTX_INPROC_SERVER);
	if( SUCCEEDED(hr) && VARIANT_TRUE == pDoc->loadXML(src))
	{
		MSXML2::IXMLDOMNodeListPtr pNodeList_Text = pDoc->selectNodes("REQ_GID");
		ASSERT(pNodeList_Text!=NULL);
		do
		{
			MSXML2::IXMLDOMNodePtr pNode_Text = pNodeList_Text->Getitem(0);
			MSXML2::IXMLDOMNodePtr pNode;
			CString str;
			_variant_t varVal;

			if(pNode = pNode_Text->selectSingleNode("DISID"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				memset(disid,0,sizeof(disid));
				memcpy(disid,str,str.GetLength());
				memcpy(dest,disid,6);
				streamlen=6;
			}
			else
			{
				streamlen=6;
			}
			if (pNode = pNode_Text->selectSingleNode("STTM"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				BYTE tm[8];
				memset(tm,0,sizeof(tm));
				int len = XmlTmtoStream((LPTSTR)(LPCTSTR)str,(char*)tm,7);
				memcpy(dest+streamlen,tm,7);
				streamlen+=7;
			}
			else
			{
				streamlen+=7;
			}
			if (pNode = pNode_Text->selectSingleNode("EDTM"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				BYTE tm[8];
				memset(tm,0,sizeof(tm));
				int len = XmlTmtoStream((LPTSTR)(LPCTSTR)str,(char*)tm,7);
				memcpy(dest+streamlen,tm,7);
				streamlen+=7;
			}
			else
			{
				streamlen+=7;
			}
			if(pNode = pNode_Text->selectSingleNode("TNO"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				char *token=NULL;
				token=strtok((LPTSTR)(LPCTSTR)str,",");
				int tmplen = streamlen;
				streamlen+=2;
				int i=0;
				while(token!=NULL)
				{
					USHORT tnnolen=0;
					tnnolen=(USHORT)strlen(token);
					*(USHORT*)(dest+streamlen)=tnnolen;
					streamlen+=2;
					memcpy(dest+streamlen,token,tnnolen);
					streamlen+=tnnolen;
					i++;
					token=strtok(NULL,",");
				}
				*(USHORT*)(dest+tmplen)=i;
			}
			else
			{
				streamlen+=2;
			}
		}while(0);
	}else
		return -1;
	return streamlen;
}

//0x07应答
int CStreamXmlExchange::AckTrainTflagStreamToxml(char*src,int len,char *dest)
{
	ASSERT(src!=NULL);
	int flag=src[0];
	CString strtmp;
	strtmp.Format("%d",flag);
	CString str=CreateNode("ACK_TFG",strtmp);
	strtmp=str;
	memcpy(dest,(LPTSTR)(LPCTSTR)strtmp,strtmp.GetLength());
	dest[strtmp.GetLength()]=0;
	return strtmp.GetLength()+1;
}

//0x07请求
int CStreamXmlExchange::ReqTrainTflagXmlToStream(char *src,int len,char *dest)
{
	ASSERT(src!=NULL);
	int streamlen=0;
	BYTE disid[7];
	MSXML2::IXMLDOMDocumentPtr pDoc;
	HRESULT hr = pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument40), NULL, CLSCTX_INPROC_SERVER);
	if( SUCCEEDED(hr) && VARIANT_TRUE == pDoc->loadXML(src))
	{
		MSXML2::IXMLDOMNodeListPtr pNodeList_Text = pDoc->selectNodes("REQ_TFG");
		ASSERT(pNodeList_Text!=NULL);
		do
		{
			MSXML2::IXMLDOMNodePtr pNode_Text = pNodeList_Text->Getitem(0);
			MSXML2::IXMLDOMNodePtr pNode;
			CString str;
			_variant_t varVal;

			if(pNode = pNode_Text->selectSingleNode("DISID"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				memset(disid,0,sizeof(disid));
				memcpy(disid,str,str.GetLength());
				memcpy(dest,disid,6);
				streamlen=6;
			}else
				streamlen=6;
			if (pNode = pNode_Text->selectSingleNode("GTID"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				DWORD gtid = atoi(str);
				*(DWORD*)(dest+streamlen)=gtid;
				streamlen+=4;
			}
			else
			{
				streamlen+=4;
			}
		}while(0);
	}else
		return -1;
	return streamlen;
}

//0x08应答
int CStreamXmlExchange::AckTrainNewStatusStreamToxml(char*src,int len,char *dest)
{
	CString strtmp1;
	CString strp1;
	//[DISID]
	memcpy((LPTSTR)(LPCTSTR)strtmp1,src,6);
	strp1 = CreateNode("DISID",strtmp1);
	src=src+6;
	CString strtmp;
	CString strp;
	//[GTID]
	DWORD gtid = *(DWORD*)src;
	strtmp.Format("%d",gtid);
	strp=CreateNode("GTID",strtmp);
	//[YLFG]
	src=src+4;
	int ylfg=(unsigned char)src[0];
	strtmp.Format("%d",ylfg);
	strp+=CreateNode("YLFG",strtmp);
	//[LYLFG]
	src=src+1;
	int lylfg=(unsigned char)src[0];
	strtmp.Format("%d",lylfg);
	strp+=CreateNode("LYLFG",strtmp);
	//[GID]
	src=src+1;
	DWORD gid = *(DWORD*)src;
	strtmp.Format("%d",gid);
	strp+=CreateNode("GID",strtmp);
	//[TNO]
	src=src+4;
	int tnolen=*(USHORT*)src;
	src=src+2;
	strtmp="";
	memcpy((LPTSTR)(LPCTSTR)strtmp,src,tnolen);
	strp+=CreateNode("TNO",strtmp);
	//[IFLAG]
	src=src+tnolen;
	strtmp="";
	int iflag=(unsigned char)src[0];
	strtmp.Format("%d",iflag);
	strp+=CreateNode("IFLAG",strtmp);
	//[OFLAG]
	src=src+1;
	strtmp="";
	int oflag=(unsigned char)src[0];
	strtmp.Format("%d",oflag);
	strp+=CreateNode("OFLAG",strtmp);
	//[STST]
	src=src+1;
	int ststlen=*(USHORT*)src;
	src=src+2;
	strtmp="";
	memcpy((LPTSTR)(LPCTSTR)strtmp,src,ststlen);
	strp+=CreateNode("STST",strtmp);
	//[ENDST]
	src=src+ststlen;
	int endstlen=*(USHORT*)src;
	src=src+2;
	strtmp="";
	memcpy((LPTSTR)(LPCTSTR)strtmp,src,endstlen);
	strp+=CreateNode("ENDST",strtmp);
	//[LSTTM]
	src=src+endstlen;
	strtmp = StreamTMtoXml(src,7);
	strp+=CreateNode("LSTTM",strtmp);
	//[LEDTM]
	src=src+7;
	strtmp = StreamTMtoXml(src,7);
	strp+=CreateNode("LEDTM",strtmp);
	//[STTM]
	src=src+7;
	strtmp = StreamTMtoXml(src,7);
	strp+=CreateNode("STTM",strtmp);
	//[FJTM]
	src=src+7;
	strtmp = StreamTMtoXml(src,7);
	strp+=CreateNode("FJTM",strtmp);
	//车站详细信息
	src=src+7;
	strp+=StreamYlSTinfoToXml(src);
	strtmp=CreateNode("YL_RECORD",strp);
	strp = strp1+strtmp;
	strtmp=CreateNode("ACK_NEWYL",strp);
	memcpy(dest,(LPTSTR)(LPCTSTR)strtmp,strtmp.GetLength());
	dest[strtmp.GetLength()]=0;
	return strtmp.GetLength()+1;
}

//0x08请求
int CStreamXmlExchange::ReqTrainNewStatusXmlToStream(char *src,int len,char *dest)
{
	ASSERT(src!=NULL);
	int streamlen=0;
	BYTE disid[7];
	MSXML2::IXMLDOMDocumentPtr pDoc;
	HRESULT hr = pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument40), NULL, CLSCTX_INPROC_SERVER);
	if( SUCCEEDED(hr) && VARIANT_TRUE == pDoc->loadXML(src))
	{
		MSXML2::IXMLDOMNodeListPtr pNodeList_Text = pDoc->selectNodes("REQ_NEWYL");
		ASSERT(pNodeList_Text!=NULL);
		do
		{
			MSXML2::IXMLDOMNodePtr pNode_Text = pNodeList_Text->Getitem(0);
			MSXML2::IXMLDOMNodePtr pNode;
			CString str;
			_variant_t varVal;

			if(pNode = pNode_Text->selectSingleNode("DISID"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				memset(disid,0,sizeof(disid));
				memcpy(disid,str,str.GetLength());
				memcpy(dest,disid,6);
				streamlen=6;
			}
			else
			{
				streamlen=6;
			}
			if (pNode = pNode_Text->selectSingleNode("GTID"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				char *token=NULL;
				token=strtok((LPTSTR)(LPCTSTR)str,",");
				int tmplen = streamlen;
				streamlen+=2;
				int j=0;
				while(token!=NULL)
				{
					DWORD gid=atoi(token);
					*(DWORD*)(dest+streamlen)=gid;
					streamlen+=4;
					j++;
					token=strtok(NULL,",");
				}
				*(USHORT*)(dest+tmplen)=j;
			}
			else
			{
				streamlen+=2;
			}
		}while(0);
	}else
		return -1;
	return streamlen;
}

//0x09,应答
int CStreamXmlExchange::AckJhDuPlanStreamToxml(char*src,int len,char* dest)
{
	ASSERT(src!=NULL);
	ASSERT(dest!=NULL);
	int flag=(unsigned char)src[0];
	CString strtmp;
	strtmp.Format("%d",flag);
	CString strp;
	strp = CreateNode("ACK_INFO",strtmp);
	memcpy(dest,(LPTSTR)(LPCTSTR)strp,strp.GetLength());
	dest[strp.GetLength()]=0;
	return strp.GetLength()+1;
}

//0x09发送
int CStreamXmlExchange::SendJhDuPlanXmlToStream(char *src,int len,char*dest)
{
	ASSERT(src!=NULL);
	int streamlen=0;
	BYTE disid[7];
	MSXML2::IXMLDOMDocumentPtr pDoc;
	HRESULT hr = pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument40), NULL, CLSCTX_INPROC_SERVER);
	if( SUCCEEDED(hr) && VARIANT_TRUE == pDoc->loadXML(src))
	{
		MSXML2::IXMLDOMNodeListPtr pNodeList_Text = pDoc->selectNodes("SEND_PLAN");
		ASSERT(pNodeList_Text!=NULL);
		do
		{
			MSXML2::IXMLDOMNodePtr pNode_Text = pNodeList_Text->Getitem(0);
			MSXML2::IXMLDOMNodePtr pNode;
			CString str;
			_variant_t varVal;

			if(pNode = pNode_Text->selectSingleNode("PLANID"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				memset(disid,0,sizeof(disid));
				memcpy(disid,str,str.GetLength());
				memcpy(dest,disid,6);
				streamlen=6;
			}else
			{
				streamlen=6;
			}
			if(pNode = pNode_Text->selectSingleNode("MSGID"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				BYTE  msgid[9];
				memset(msgid,0,sizeof(msgid));
				memcpy(msgid,str,str.GetLength());
				memcpy(dest+streamlen,msgid,8);
				streamlen+=8;
			}else
			{
				streamlen+=8;
			}
			if(pNode = pNode_Text->selectSingleNode("LDISID"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				memset(disid,0,sizeof(disid));
				memcpy(disid,str,str.GetLength());
				memcpy(dest+streamlen,disid,6);
				streamlen+=6;
			}else
			{
				streamlen+=6;
			}
			if(pNode = pNode_Text->selectSingleNode("DISNM"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				USHORT disnmlen=str.GetLength();
				*(USHORT*)(dest+streamlen)=disnmlen;
				streamlen+=2;
				memcpy(dest+streamlen,(LPTSTR)(LPCTSTR)str,disnmlen);
				streamlen+=disnmlen;
			}else
			{
				streamlen+=2;
			}
			if(pNode = pNode_Text->selectSingleNode("MNM"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				USHORT mnmlen=str.GetLength();
				*(USHORT*)(dest+streamlen)=mnmlen;
				streamlen+=2;
				memcpy(dest+streamlen,(LPTSTR)(LPCTSTR)str,mnmlen);
				streamlen+=mnmlen;
			}else
			{
				streamlen+=2;
			}
			if(pNode = pNode_Text->selectSingleNode("MSG"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				USHORT msglen=str.GetLength();
				*(USHORT*)(dest+streamlen)=msglen;
				streamlen+=2;
				memcpy(dest+streamlen,(LPTSTR)(LPCTSTR)str,msglen);
				streamlen+=msglen;
			}else
			{
				streamlen+=2;
			}
		}while(0);
	}else
		return -1;
	return streamlen;
}

//0x0a应答
int CStreamXmlExchange::AckKdAdjustPlanStreamToxml(char *src,int len,char* dest)
{
	return AckJhDuPlanStreamToxml(src,len,dest);
}

//0x0a请求
int CStreamXmlExchange::SendKdAdjustPlanXmlToStream(char *src,int len,char *dest)
{
	ASSERT(src!=NULL);
	int streamlen=0;
	BYTE disid[7];
	MSXML2::IXMLDOMDocumentPtr pDoc;
	HRESULT hr = pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument40), NULL, CLSCTX_INPROC_SERVER);
	if( SUCCEEDED(hr) && VARIANT_TRUE == pDoc->loadXML(src))
	{
		MSXML2::IXMLDOMNodeListPtr pNodeList_Text = pDoc->selectNodes("SEND_ADPLAN");
		ASSERT(pNodeList_Text!=NULL);
		do
		{
			MSXML2::IXMLDOMNodePtr pNode_Text = pNodeList_Text->Getitem(0);
			MSXML2::IXMLDOMNodePtr pNode;
			CString str;
			_variant_t varVal;

			if(pNode = pNode_Text->selectSingleNode("DISID"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				memset(disid,0,sizeof(disid));
				memcpy(disid,str,str.GetLength());
				memcpy(dest,disid,6);
				streamlen=6;
			}else
			{
				streamlen=6;
			}
			if (pNode = pNode_Text->selectSingleNode("GID"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				char *token=NULL;
				token=strtok((LPTSTR)(LPCTSTR)str,",");
				int tmplen = streamlen;
				streamlen+=2;
				int j=0;
				while(token!=NULL)
				{
					DWORD gid=atoi(token);
					*(DWORD*)(dest+streamlen)=gid;
					streamlen+=4;
					j++;
					token=strtok(NULL,",");
				}
				*(USHORT*)(dest+tmplen)=j;
			}else
			{
				streamlen+=2;
			}
		}while(0);
	}else
		return -1;
	return streamlen;
}

//0x0b应答
int CStreamXmlExchange::AckOtherInfoStreamToxml(char *src,int len,char*dest)
{
	return AckJhDuPlanStreamToxml(src,len,dest);
}

//0x0b请求
int CStreamXmlExchange::SendOtherInfoXmlToStream(char *src,int len,char *dest)
{
	ASSERT(src!=NULL);
	int streamlen=0;
	BYTE disid[7];
	MSXML2::IXMLDOMDocumentPtr pDoc;
	HRESULT hr = pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument40), NULL, CLSCTX_INPROC_SERVER);
	if( SUCCEEDED(hr) && VARIANT_TRUE == pDoc->loadXML(src))
	{
		MSXML2::IXMLDOMNodeListPtr pNodeList_Text = pDoc->selectNodes("SEND_MSG");
		ASSERT(pNodeList_Text!=NULL);
		do
		{
			MSXML2::IXMLDOMNodePtr pNode_Text = pNodeList_Text->Getitem(0);
			MSXML2::IXMLDOMNodePtr pNode;
			CString str;
			_variant_t varVal;

			if(pNode = pNode_Text->selectSingleNode("DISID"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				memset(disid,0,sizeof(disid));
				memcpy(disid,str,str.GetLength());
				memcpy(dest,disid,6);
				streamlen=6;
			}else
			{
				streamlen=6;
			}
			if (pNode = pNode_Text->selectSingleNode("MSG"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				*(WORD*)(dest+streamlen)=str.GetLength();
				streamlen+=2;
				char *msg=NULL;
				msg = new char[str.GetLength()+1];
				memset(msg,0,str.GetLength()+1);
				memcpy(msg,(LPTSTR)(LPCTSTR)str,str.GetLength());
				memcpy(dest+streamlen,msg,str.GetLength());
				streamlen+=str.GetLength();
				if(msg!=NULL)
					delete []msg;
			}else
			{
				streamlen+=2;
			}
		}while(0);
	}
	return streamlen;
}

//0x0d
int CStreamXmlExchange::AckDisCmdStreamToxml(char *src,int len,char*dest)
{
	ASSERT(src!=NULL);
	CString strtmp;
	CString strp;
	//[ID]
	int idlen=*(USHORT*)src;
	src=src+2;
	strtmp="";
	memcpy((LPTSTR)(LPCTSTR)strtmp,src,idlen);
	strp=CreateNode("ID",strtmp);
	//[SDERTP]
	src=src+idlen;
	int sdertp=(unsigned char)src[0];
	strtmp="";
	strtmp.Format("%d",sdertp);
	strp+=CreateNode("SDERTP",strtmp);
	//[SDERID]
	src=src+1;
	int sderid=*(USHORT*)src;
	strtmp="";
	strtmp.Format("%d",sderid);
	strp+=CreateNode("SDERID",strtmp);
	//[RECVTP]
	src=src+2;
	int recvtp=(unsigned char)src[0];
	strtmp="";
	strtmp.Format("%d",recvtp);
	strp+=CreateNode("RECVTP",strtmp);
	//[RECVID]
	src=src+1;
	int recvid=*(USHORT*)src;
	strtmp="";
	strtmp.Format("%d",recvid);
	strp+=CreateNode("RECVID",strtmp);
	//[CMDINDEX]
	src=src+2;
	DWORD cmdindex=*(DWORD*)src;
	strtmp="";
	strtmp.Format("%d",cmdindex);
	strp+=CreateNode("CMDINDEX",strtmp);
	//[CMDID]
	src=src+4;
	strtmp="";
	memcpy((LPTSTR)(LPCTSTR)strtmp,src,10);
	strp+=CreateNode("CMDID",strtmp);
	//[CMDTP]
	src=src+10;
	int cmdtp=*(USHORT*)src;
	strtmp="";
	strtmp.Format("%d",cmdtp);
	strp+=CreateNode("CMDTP",strtmp);
	//[SDTM]
	src=src+2;
	strtmp+=StreamTMtoXml(src,7);
	strp+=CreateNode("SDTM",strtmp);
	//[SDUNIT]
	src=src+7;
	int sdunitlen=(unsigned char)src[0];
	strtmp="";
	src=src+1;
	memcpy((LPTSTR)(LPCTSTR)strtmp,src,sdunitlen);
	strp+=CreateNode("SDUNIT",strtmp);
	//[SENDER]
	src=src+sdunitlen;
	int senderlen=(unsigned char)src[0];
	strtmp="";
	src=src+1;
	memcpy((LPTSTR)(LPCTSTR)strtmp,src,senderlen);
	strp+=CreateNode("SENDER",strtmp);
	//[RECVUNIT]
	src=src+senderlen;
	int recvunitlen=(unsigned char)src[0];
	strtmp="";
	src=src+1;
	memcpy((LPTSTR)(LPCTSTR)strtmp,src,recvunitlen);
	strp+=CreateNode("RECVUNIT",strtmp);
	//[STNUM]
	src=src+recvunitlen;
	int stnum=(unsigned char)src[0];
	src=src+1;
	CString STINFO="";
	for (int j=0;j<stnum;j++)
	{
		//[STID]
		int stid=*(USHORT*)src;
		strtmp="";
		strtmp.Format("%d",stid);
		STINFO=CreateNode("STID",strtmp);
		//[STNM]
		src=src+2;
		int stnmlen=(unsigned char)src[0];
		src=src+1;
		strtmp="";
		memcpy((LPTSTR)(LPCTSTR)strtmp,src,stnmlen);
		STINFO+=CreateNode("STNM",strtmp);
		//[CPDEST]
		src=src+stnmlen;
		int cpdestlen=(unsigned char)src[0];
		src=src+1;
		strtmp="";
		memcpy((LPTSTR)(LPCTSTR)strtmp,src,cpdestlen);
		STINFO+=CreateNode("CPDEST",strtmp);
		strp+=CreateNode("STINFO",STINFO);
		src=src+cpdestlen;
	}
	//[TOPIC]
	int topiclen=(unsigned char)src[0];
	src=src+1;
	strtmp="";
	memcpy((LPTSTR)(LPCTSTR)strtmp,src,topiclen);
	strp+=CreateNode("TOPIC",strtmp);
	//[CONTENT]
	src=src+topiclen;
	int contentlen=*(USHORT*)src;
	src=src+2;
	strtmp="";
	memcpy((LPTSTR)(LPCTSTR)strtmp,src,contentlen);
	strp+=CreateNode("CONTENT",strtmp);
	//[PRNM]
	src=src+contentlen;
	int prnmlen=*(USHORT*)src;
	src=src+2;
	strtmp="";
	memcpy((LPTSTR)(LPCTSTR)strtmp,src,prnmlen);
	strp+=CreateNode("PRNM",strtmp);
	strtmp=CreateNode("ACK_CMD",strp);
	strp=strtmp;
	memcpy(dest,(LPTSTR)(LPCTSTR)strp,strp.GetLength());
	dest[strp.GetLength()]=0;
	return strp.GetLength()+1;
}

//0x0d请求
int CStreamXmlExchange::ReqDisCmdXmlToStream(char *src,int len,char *dest)
{
	ASSERT(src!=NULL);
	int streamlen=0;
	MSXML2::IXMLDOMDocumentPtr pDoc;
	HRESULT hr = pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument40), NULL, CLSCTX_INPROC_SERVER);
	if( SUCCEEDED(hr) && VARIANT_TRUE == pDoc->loadXML(src))
	{
		MSXML2::IXMLDOMNodePtr pNode = pDoc->selectSingleNode("REQ_CMD");
		ASSERT(pNode!=NULL);
		_variant_t varVal;
		CString str;
		varVal = pNode->GetnodeTypedValue();
		str = varVal.bstrVal;
		*(WORD*)(dest+streamlen)=str.GetLength();
		streamlen+=2;
		char *msg=NULL;
		msg = new char[str.GetLength()+1];
		memset(msg,0,str.GetLength()+1);
		memcpy(msg,(LPTSTR)(LPCTSTR)str,str.GetLength());
		memcpy(dest+streamlen,msg,str.GetLength());
		streamlen+=str.GetLength();
		if(msg!=NULL)
			delete []msg;
	}
	return streamlen;
}

//0x0e应答
int CStreamXmlExchange::AckWorkFlagInfoStreamToxml(char*src,int len,char*dest)
{
	ASSERT(src!=NULL);
	ASSERT(dest!=NULL);
	CString strtmp;
	CString strp;
	//[TYPE]
	int type = (unsigned char)src[0];
	strtmp.Format("%d",type);
	strp=CreateNode("TYPE",strtmp);
	//[STTM]
	src=src+1;
	strtmp = StreamTMtoXml(src,7);
	strp+=CreateNode("STTM",strtmp);
	//[EDTM]
	src=src+7;
	strtmp = StreamTMtoXml(src,7);
	strp+=CreateNode("EDTM",strtmp);
	//[STST]
	src=src+7;
	int ststlen=*(USHORT*)src;
	src=src+2;
	strtmp="";
	memcpy((LPTSTR)(LPCTSTR)strtmp,src,ststlen);
	strp+=CreateNode("STST",strtmp);
	//[EDST]
	src=src+ststlen;
	int edstlen=*(USHORT*)src;
	src=src+2;
	strtmp="";
	memcpy((LPTSTR)(LPCTSTR)strtmp,src,edstlen);
	strp+=CreateNode("EDST",strtmp);
	//[STTB]
	src=src+edstlen;
	int sttb=*(USHORT*)src;
	strtmp.Format("%d",sttb);
	strp+=CreateNode("STTB",strtmp);
	//[EDTB]
	src=src+2;
	int edtb=*(USHORT*)src;
	strtmp.Format("%d",edtb);
	strp+=CreateNode("EDTB",strtmp);
	//[STKM]
	src=src+2;
	int stkm=*(USHORT*)src;
	strtmp.Format("%d",stkm);
	strp+=CreateNode("STKM",strtmp);
	//[STM]
	src=src+2;
	int stm=*(USHORT*)src;
	strtmp.Format("%d",stm);
	strp+=CreateNode("STM",strtmp);
	//[EDKM]
	src=src+2;
	int edkm=*(USHORT*)src;
	strtmp.Format("%d",edkm);
	strp+=CreateNode("EDKM",strtmp);
	//[EDM]
	src=src+2;
	int edm=*(USHORT*)src;
	strtmp.Format("%d",edm);
	strp+=CreateNode("EDM",strtmp);
	//[LIM]
	src=src+2;
	int lim=*(USHORT*)src;
	strtmp.Format("%d",lim);
	strp+=CreateNode("LIM",strtmp);
	//[OTHER]
	src=src+2;
	int otherlen=*(USHORT*)src;
	src=src+2;
	strtmp="";
	memcpy((LPTSTR)(LPCTSTR)strtmp,src,otherlen);
	strp+=CreateNode("OTHER",strtmp);
	//[DIR]
	src=src+otherlen;
	DWORD dir = *(DWORD*)src;
	strtmp.Format("%d",dir);
	strp+=CreateNode("DIR",strtmp);
	//[PLANID]
	src=src+4;
	int planidlen=*(USHORT*)src;
	src=src+2;
	strtmp="";
	memcpy((LPTSTR)(LPCTSTR)strtmp,src,planidlen);
	strp+=CreateNode("PLANID",strtmp);
	//[PLANCD]
	src=src+planidlen;
	int plancdlen=*(USHORT*)src;
	src=src+2;
	strtmp="";
	memcpy((LPTSTR)(LPCTSTR)strtmp,src,plancdlen);
	strp+=CreateNode("PLANID",strtmp);
	strtmp = CreateNode("ACK_WKINFO",strp);
	strp=strtmp;
	memcpy(dest,(LPTSTR)(LPCTSTR)strp,strp.GetLength());
	dest[strp.GetLength()]=0;
	return strp.GetLength()+1;
}

//0x0e请求
int CStreamXmlExchange::ReqWorkFlagInfoXmlToStream(char*src,int len,char *dest)
{
	ASSERT(src!=NULL);
	int streamlen=0;
	BYTE disid[7];
	MSXML2::IXMLDOMDocumentPtr pDoc;
	HRESULT hr = pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument40), NULL, CLSCTX_INPROC_SERVER);
	if( SUCCEEDED(hr) && VARIANT_TRUE == pDoc->loadXML(src))
	{
		MSXML2::IXMLDOMNodeListPtr pNodeList_Text = pDoc->selectNodes("REQ_WKINFO");
		ASSERT(pNodeList_Text!=NULL);
		do
		{
			MSXML2::IXMLDOMNodePtr pNode_Text = pNodeList_Text->Getitem(0);
			MSXML2::IXMLDOMNodePtr pNode;
			CString str;
			_variant_t varVal;

			if(pNode = pNode_Text->selectSingleNode("DISID"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				memset(disid,0,sizeof(disid));
				memcpy(disid,str,str.GetLength());
				memcpy(dest,disid,6);
				streamlen=6;
			}else
			{
				streamlen=6;
			}
			if (pNode = pNode_Text->selectSingleNode("TM"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				BYTE tm[9];
				memset(tm,0,sizeof(tm));
				int len = XmlTmtoStream((LPTSTR)(LPCTSTR)str,(char *)tm,8);
				memcpy(dest+streamlen,tm,8);
				streamlen+=8;
			}else
			{
				streamlen+=8;
			}
		}while(0);
	}
	return streamlen;

}

//0x0f应答
int CStreamXmlExchange::AckWorkMidRsInfoStreamToxml(char *src,int len,char *dest)
{
	CString strtmp;
	CString strp;
	//[DISID]
	memcpy((LPTSTR)(LPCTSTR)strtmp,src,6);
	strp = CreateNode("DISID",strtmp);
	//[TM]
	src=src+6;
	strtmp=StreamTMtoXml(src,8);
	strp+=CreateNode("TM",strtmp);
	//[ID]
	src=src+8;
	int idlen=*(USHORT*)src;
	src=src+2;
	strtmp="";
	memcpy((LPTSTR)(LPCTSTR)strtmp,src,idlen);
	strp+=CreateNode("ID",strtmp);
	//[TRNINFO]
	src=src+idlen;
	int trninfolen=*(USHORT*)src;
	src=src+2;
	strtmp="";
	memcpy((LPTSTR)(LPCTSTR)strtmp,src,trninfolen);
	strp+=CreateNode("TRNINFO",strtmp);
	//[FLAG]
	src=src+trninfolen;
	int flag = (unsigned char)src[0];
	strtmp="";
	strtmp.Format("%d",flag);
	strp+=CreateNode("FLAG",strtmp);
	strtmp = CreateNode("ACK_RSINFO",strp);
	strp=strtmp;
	memcpy(dest,(LPTSTR)(LPCTSTR)strp,strp.GetLength());
	dest[strp.GetLength()]=0;
	return strp.GetLength()+1;
}

//0x0f请求
int CStreamXmlExchange::ReqWorkMidRsInfoXmlToStream(char *src,int len,char *dest)
{
	ASSERT(src!=NULL);
	int streamlen=0;
	BYTE disid[7];
	MSXML2::IXMLDOMDocumentPtr pDoc;
	HRESULT hr = pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument40), NULL, CLSCTX_INPROC_SERVER);
	if( SUCCEEDED(hr) && VARIANT_TRUE == pDoc->loadXML(src))
	{
		MSXML2::IXMLDOMNodeListPtr pNodeList_Text = pDoc->selectNodes("REQ_RSINFO");
		ASSERT(pNodeList_Text!=NULL);
		do
		{
			MSXML2::IXMLDOMNodePtr pNode_Text = pNodeList_Text->Getitem(0);
			MSXML2::IXMLDOMNodePtr pNode;
			CString str;
			_variant_t varVal;

			if(pNode = pNode_Text->selectSingleNode("DISID"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				memset(disid,0,sizeof(disid));
				memcpy(disid,str,str.GetLength());
				memcpy(dest,disid,6);
				streamlen=6;
			}else
			{
				streamlen=6;
			}
			if (pNode = pNode_Text->selectSingleNode("TM"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				BYTE tm[9];
				memset(tm,0,sizeof(tm));
				int len = XmlTmtoStream((LPTSTR)(LPCTSTR)str,(char *)tm,8);
				memcpy(dest+streamlen,tm,8);
				streamlen+=8;
			}else
			{
				streamlen+=8;
			}
		}while(0);
	}
	return streamlen;
}

//0x10应答
int CStreamXmlExchange::AckKdCmdInfoStreamToxml(char *src,int len,char*dest)
{
	ASSERT(src!=NULL);
	CString strp;
	DWORD cmdinfolen = *(DWORD*)src;
	src=src+4;
	memcpy((LPTSTR)(LPCTSTR)strp,src,cmdinfolen);
	CString strtmp=CreateNode("ACK_INFO",strp);
	strp=strtmp;
	memcpy(dest,(LPTSTR)(LPCTSTR)strp,strp.GetLength());
	dest[strp.GetLength()]=0;
	return strp.GetLength()+1;
}

//0x10请求
int CStreamXmlExchange::ReqKdCmdInfoXmlToStream(char *src,int len,char*dest)
{
	ASSERT(src!=NULL);
	int streamlen=0;
	MSXML2::IXMLDOMDocumentPtr pDoc;
	HRESULT hr = pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument40), NULL, CLSCTX_INPROC_SERVER);
	if( SUCCEEDED(hr) && VARIANT_TRUE == pDoc->loadXML(src))
	{
		MSXML2::IXMLDOMNodePtr pNode = pDoc->selectSingleNode("REQ_INFO");
		ASSERT(pNode!=NULL);
		_variant_t varVal;
		CString str;
		varVal = pNode->GetnodeTypedValue();
		str = varVal.bstrVal;
		*(DWORD*)dest=atoi(str);
		streamlen=4;
	}else
		return -1;
	return streamlen;
}

//0x12应答
int CStreamXmlExchange::AckBsInfoStreamToxml(char *src,int len,char *dest)
{
	return AckKdCmdInfoStreamToxml(src,len,dest);
}

//0x12请求
int CStreamXmlExchange::ReqBsInfoXmlToStream(char *src,int len,char*dest)
{
	return ReqKdCmdInfoXmlToStream(src,len,dest);
}

//0x13应答
int CStreamXmlExchange::AckWorkPlanInfoStreamToxml(char *src,int len,char*dest)
{
	ASSERT(src!=NULL);
	CString strtmp;
	CString strp;
	//[TM]
	strtmp=StreamTMtoXml(src,8);
	strp=CreateNode("TM",strtmp);
	//[ID]
	src=src+8;
	int idlen = *(USHORT*)src;
	src=src+2;
	strtmp="";
	memcpy((LPTSTR)(LPCTSTR)strtmp,src,idlen);
	strp+=CreateNode("ID",strtmp);
	//[FLAG]
	src=src+idlen;
	int flag = (unsigned char)src[0];
	strtmp.Format("%d",flag);
	strp+=CreateNode("FLAG",strtmp);
	//[MID]
	src=src+1;
	int midlen = *(USHORT*)src;
	src=src+2;
	strtmp="";
	memcpy((LPTSTR)(LPCTSTR)strtmp,src,midlen);
	strp+=CreateNode("MID",strtmp);
	//[WORKPOS]
	src=src+midlen;
	int workposlen = *(USHORT*)src;
	src=src+2;
	strtmp="";
	memcpy((LPTSTR)(LPCTSTR)strtmp,src,workposlen);
	strp+=CreateNode("WORKPOS",strtmp);
	//[LINENM]
	src=src+workposlen;
	int linenmlen = *(USHORT*)src;
	src=src+2;
	strtmp="";
	memcpy((LPTSTR)(LPCTSTR)strtmp,src,linenmlen);
	strp+=CreateNode("LINENM",strtmp);
	//[DIR]
	src=src+linenmlen;
	int dir = (unsigned char)src[0];
	strtmp.Format("%d",dir);
	strp+=CreateNode("DIR",strtmp);
	//[WORKKM]
	src=src+1;
	int workkmlen = *(USHORT*)src;
	src=src+2;
	strtmp="";
	memcpy((LPTSTR)(LPCTSTR)strtmp,src,workkmlen);
	strp+=CreateNode("WORKKM",strtmp);
	//[STTM]
	src=src+workkmlen;
	strtmp = StreamTMtoXml(src,7);
	strp+=CreateNode("STTM",strtmp);
	//[EDTM]
	src=src+7;
	strtmp = StreamTMtoXml(src,7);
	strp+=CreateNode("EDTM",strtmp);
	//[WORKTP]
	src=src+7;
	int worktplen = *(USHORT*)src;
	src=src+2;
	strtmp="";
	memcpy((LPTSTR)(LPCTSTR)strtmp,src,worktplen);
	strp+=CreateNode("WORKTP",strtmp);
	//[PJNM]
	src=src+worktplen;
	int pjnmlen = *(USHORT*)src;
	src=src+2;
	strtmp="";
	memcpy((LPTSTR)(LPCTSTR)strtmp,src,pjnmlen);
	strp+=CreateNode("PJNM",strtmp);
	//[UNITNM]
	src=src+pjnmlen;
	int unitnmlen = *(USHORT*)src;
	src=src+2;
	strtmp="";
	memcpy((LPTSTR)(LPCTSTR)strtmp,src,unitnmlen);
	strp+=CreateNode("UNITNM",strtmp);
	//[LEADER]
	src=src+unitnmlen;
	int leaderlen = *(USHORT*)src;
	src=src+2;
	strtmp="";
	memcpy((LPTSTR)(LPCTSTR)strtmp,src,leaderlen);
	strp+=CreateNode("LEADER",strtmp);
	//[RANK]
	src=src+leaderlen;
	int rank = *(USHORT*)src;
	strtmp.Format("%d",rank);
	strp+=CreateNode("RANK",strtmp);
	//[TELID]
	src=src+2;
	int telidlen = *(USHORT*)src;
	src=src+2;
	strtmp="";
	memcpy((LPTSTR)(LPCTSTR)strtmp,src,telidlen);
	strp+=CreateNode("TELID",strtmp);
	//[WKCON]
	src=src+telidlen;
	int wkconlen = *(USHORT*)src;
	src=src+2;
	strtmp="";
	memcpy((LPTSTR)(LPCTSTR)strtmp,src,wkconlen);
	strp+=CreateNode("WKCON",strtmp);
	//[SPLIM]
	src=src+wkconlen;
	int splimlen = *(USHORT*)src;
	src=src+2;
	strtmp="";
	memcpy((LPTSTR)(LPCTSTR)strtmp,src,splimlen);
	strp+=CreateNode("SPLIM",strtmp);
	//[LIMINFO]
	src=src+splimlen;
	int liminfolen = *(USHORT*)src;
	src=src+2;
	strtmp="";
	memcpy((LPTSTR)(LPCTSTR)strtmp,src,splimlen);
	strp+=CreateNode("LIMINFO",strtmp);
	strtmp = CreateNode("ACK_WKINFO",strp);
	strp=strtmp;
	memcpy(dest,(LPTSTR)(LPCTSTR)strp,strp.GetLength());
	dest[strp.GetLength()]=0;
	return strp.GetLength()+1;
}


//0x13请求
int CStreamXmlExchange::ReqWorkPlanInfoXmlToStream(char *src,int len,char*dest)
{
	ASSERT(src!=NULL);
	int streamlen=0;
	MSXML2::IXMLDOMDocumentPtr pDoc;
	HRESULT hr = pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument40), NULL, CLSCTX_INPROC_SERVER);
	if( SUCCEEDED(hr) && VARIANT_TRUE == pDoc->loadXML(src))
	{
		MSXML2::IXMLDOMNodeListPtr pNodeList_Text = pDoc->selectNodes("REQ_WKINFO");
		ASSERT(pNodeList_Text!=NULL);
		do
		{
			MSXML2::IXMLDOMNodePtr pNode_Text = pNodeList_Text->Getitem(0);
			MSXML2::IXMLDOMNodePtr pNode;
			CString str;
			_variant_t varVal;

			if(pNode = pNode_Text->selectSingleNode("ID"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				*(WORD*)dest=str.GetLength();
				streamlen=2;
				char *msg=NULL;
				msg = new char[str.GetLength()+1];
				memset(msg,0,str.GetLength()+1);
				memcpy(msg,(LPTSTR)(LPCTSTR)str,str.GetLength());
				memcpy(dest+streamlen,msg,str.GetLength());
				streamlen+=str.GetLength();
				if(msg!=NULL)
					delete []msg;
			}else
			{
				streamlen=2;
			}
			if (pNode = pNode_Text->selectSingleNode("FLAG"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;
				BYTE flag = atoi(str);
				*(BYTE*)(dest+streamlen)=flag;
				streamlen+=1;
			}else
			{
				streamlen+=1;
			}
		}while(0);
	}
	return streamlen;
}

//0x14应答
int CStreamXmlExchange::AckWorkDescriptionStreamToxml(char *src,int len,char*dest)
{
	ASSERT(src!=NULL);
	CString strtmp;
	CString strp;
	//[ID]
	int idlen = *(USHORT*)src;
	src=src+2;
	strtmp="";
	memcpy((LPTSTR)(LPCTSTR)strtmp,src,idlen);
	strp=CreateNode("ID",strtmp);
	//[CMDID]
	src=src+idlen;
	int cmdidlen = *(USHORT*)src;
	src=src+2;
	strtmp="";
	memcpy((LPTSTR)(LPCTSTR)strtmp,src,cmdidlen);
	strp+=CreateNode("CMDID",strtmp);
	//[CONTENT]
	src=src+cmdidlen;
	int contentlen = *(USHORT*)src;
	src=src+2;
	strtmp="";
	memcpy((LPTSTR)(LPCTSTR)strtmp,src,contentlen);
	strp+=CreateNode("CONTENT",strtmp);
	strtmp = CreateNode("ACK_WKDES",strp);
	strp=strtmp;
	memcpy(dest,(LPTSTR)(LPCTSTR)strp,strp.GetLength());
	dest[strp.GetLength()]=0;
	return strp.GetLength()+1;
}

//0x14
int CStreamXmlExchange::ReqWorkDescriptionXmlToStream(char *src,int len,char*dest)
{
	ASSERT(src!=NULL);
	int streamlen=0;
	MSXML2::IXMLDOMDocumentPtr pDoc;
	HRESULT hr = pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument40), NULL, CLSCTX_INPROC_SERVER);
	if( SUCCEEDED(hr) && VARIANT_TRUE == pDoc->loadXML(src))
	{
		MSXML2::IXMLDOMNodePtr pNode = pDoc->selectSingleNode("REQ_WKDES");
		ASSERT(pNode!=NULL);
		_variant_t varVal;
		CString str;
		varVal = pNode->GetnodeTypedValue();
		str = varVal.bstrVal;
		*(WORD*)(dest+streamlen)=str.GetLength();
		streamlen+=2;
		char *msg=NULL;
		msg = new char[str.GetLength()+1];
		memset(msg,0,str.GetLength()+1);
		memcpy(msg,(LPTSTR)(LPCTSTR)str,str.GetLength());
		memcpy(dest+streamlen,msg,str.GetLength());
		streamlen+=str.GetLength();
		if(msg!=NULL)
			delete []msg;
	}else
		return -1;
	return streamlen;
}

CString CStreamXmlExchange::StreamYlSTinfoToXml(char *src,int len/*=0*/)
{
	ASSERT(src!=NULL);
	int stcnt=*(USHORT*)src;
	src=src+2;
	CString strtmp;
	CString strp;
	CString StRecord="";
	for (int i=0;i<stcnt;i++)
	{
		//[STNM]
		int stnmlen=*(USHORT*)src;
		src=src+2;
		strtmp="";
		memcpy((LPTSTR)(LPCTSTR)strtmp,src,stnmlen);
		strp=CreateNode("STNM",strtmp);
		//[ARND]
		src=src+stnmlen;
		strtmp="";
		int arnd=*(USHORT*)src;
		strtmp.Format("%d",arnd);
		strp+=CreateNode("ARND",strtmp);
		//[DPND]
		src=src+2;
		strtmp="";
		int dpnd=*(USHORT*)src;
		strtmp.Format("%d",dpnd);
		strp+=CreateNode("DPND",strtmp);
		//[ENTRY]
		src=src+2;
		strtmp="";
		int entry=*(USHORT*)src;
		strtmp.Format("%d",entry);
		strp+=CreateNode("ENTRY",strtmp);
		//[EXIT]
		src=src+2;
		strtmp="";
		int exit=*(USHORT*)src;
		strtmp.Format("%d",exit);
		strp+=CreateNode("EXIT",strtmp);
		//[ARTNO]
		src=src+2;
		int artnolen=*(USHORT*)src;
		src=src+2;
		strtmp="";
		memcpy((LPTSTR)(LPCTSTR)strtmp,src,artnolen);
		strp=CreateNode("ARTNO",strtmp);
		//[DPTNO]
		src=src+artnolen;
		int dptnolen=*(USHORT*)src;
		src=src+2;
		strtmp="";
		memcpy((LPTSTR)(LPCTSTR)strtmp,src,dptnolen);
		strp=CreateNode("DPTNO",strtmp);
		//[ARTM]
		src=src+dptnolen;
		strtmp = StreamTMtoXml(src,7);
		strp+=CreateNode("ARTM",strtmp);
		//[DPTM]
		src=src+7;
		strtmp = StreamTMtoXml(src,7);
		strp+=CreateNode("DPTM",strtmp);
		//[SARTM]
		src=src+7;
		strtmp = StreamTMtoXml(src,7);
		strp+=CreateNode("SARTM",strtmp);
		//[SDPTM]
		src=src+7;
		strtmp = StreamTMtoXml(src,7);
		strp+=CreateNode("SDPTM",strtmp);
		//[BSTM]
		src=src+7;
		strtmp="";
		int bstm=*(USHORT*)src;
		strtmp.Format("%d",bstm);
		strp+=CreateNode("BSTM",strtmp);
		//[IMPINFO]
		src=src+2;
		strtmp="";
		int impinfo=*(USHORT*)src;
		strtmp.Format("%d",impinfo);
		strp+=CreateNode("IMPINFO",strtmp);
		//[STPINFO]
		src=src+2;
		strtmp="";
		int stpinfo=*(USHORT*)src;
		strtmp.Format("%d",stpinfo);
		strp+=CreateNode("STPINFO",strtmp);
		//[METINFO]
		src=src+2;
		int metlen=*(USHORT*)src;
		strtmp="";
		if(metlen>=1)
		{
			src=src+2;
			strtmp.Format("%d",*(USHORT*)src);
		}
		for(int j=1;j<metlen;j++)
		{
			src=src+2;
			int metinfo=*(USHORT*)src;
			CString tmpstr;
			tmpstr.Format(",%d",metinfo);
			strtmp+=tmpstr;
		}
		strp+=CreateNode("METINFO",strtmp);
		//[SGINFO]
		src=src+2;
		int sginfolen=*(USHORT*)src;
		strtmp="";
		src=src+2;
		memcpy((LPTSTR)(LPCTSTR)strtmp,src,sginfolen);
		strp+=CreateNode("SGINFO",strtmp);
		StRecord+=CreateNode("ST_RECORD",strp);
		//next
		src=src+sginfolen;
		strp="";
	}
	return StRecord;
}

CString CStreamXmlExchange::StreamBzRecordToXml(char *src,int len/*=0*/)
{
	CString strp;
	CString strtmp;
	int i=0;
	int TrainNum =*(USHORT*)src;
	src=src+2;
	CString xmlBzRecord="";

	//[BZ_RECORD]
	for (;i<TrainNum;i++)
	{
		//<RKTM>
		strtmp = StreamTMtoXml(src,7);
		strp = CreateNode("RKTM",strtmp);
		//<IDX>
		src=src+7;
		strtmp="";
		
		int index=0;
		index = (unsigned char)(src[0])+(USHORT)(src[1]<<8);
		strtmp.Format("%d",index);
		strp += CreateNode("IDX",strtmp);
		//[TYPE]
		src=src+2;
		strtmp="";
		int trantypelen=*(USHORT*)src;
		src=src+2;
		memcpy((LPTSTR)(LPCTSTR)strtmp,src,trantypelen);
		strp += CreateNode("TYPE",strtmp);
		//[YZ]
		src=src+trantypelen;
		strtmp="";
		int yz = (unsigned char)src[0];
		strtmp.Format("%d",yz);
		strp += CreateNode("YZ",strtmp);
		//[CNO]
		src=src+1;
		strtmp="";
		memcpy((LPTSTR)(LPCTSTR)strtmp,src,8);
		strp += CreateNode("CNO",strtmp);
		//[SWT]
		src=src+8;
		strtmp="";
		DWORD swt = *(DWORD *)src;
		strtmp.Format("%d",swt);
		strp += CreateNode("SWT",strtmp);
		//[LEN]
		src = src+ 4;
		strtmp="";
		DWORD len = *(DWORD *)src;
		strtmp.Format("%d",len);
		strp += CreateNode("LEN",strtmp);
		//[LDFLAG]
		src = src+ 4;
		strtmp="";
		int ldflag = (unsigned char)src[0];
		strtmp.Format("%d",ldflag);
		strp += CreateNode("LDFLAG",strtmp);
		//[LD]
		src = src+ 1;
		strtmp="";
		int ld=(unsigned char)(src[0])+(USHORT)(src[1]<<8);
		strtmp.Format("%d",ld);
		strp+=CreateNode("LD",strtmp);
		//[FIRSTCD]
		src = src+2;
		strtmp="";
		memcpy((LPTSTR)(LPCTSTR)strtmp,src,3);
		strp+=CreateNode("FIRSTCD",strtmp);
		//[LSTSTCD]
		src = src+3;
		strtmp="";
		memcpy((LPTSTR)(LPCTSTR)strtmp,src,3);
		strp+=CreateNode("LSTSTCD",strtmp);
		//[OBJNM]
		src = src+3;
		strtmp="";
		int objnmlen = *(USHORT*)src;
		src = src+2;
		memcpy((LPTSTR)(LPCTSTR)strtmp,src,objnmlen);
		strp+=CreateNode("OBJNM",strtmp);
		//[RCVNM]
		src = src+objnmlen;
		int rcvnmlen = *(USHORT*)src;
		src = src+2;
		strtmp="";
		memcpy((LPTSTR)(LPCTSTR)strtmp,src,rcvnmlen);
		strp+=CreateNode("RCVNM",strtmp);
		//[FZCD]
		src = src+rcvnmlen;
		strtmp="";
		memcpy((LPTSTR)(LPCTSTR)strtmp,src,3);
		strp+=CreateNode("FZCD",strtmp);
		//[PBCNT]
		src = src+3;
		int pbcnt = (unsigned char)(src[0])+(USHORT)(src[1]<<8);
		strtmp="";
		strtmp.Format("%d",pbcnt);
		strp+=CreateNode("PBCNT",strtmp);
		//[NOTE]
		src = src+2;
		int notelen = *(USHORT*)src;
		src = src+2;
		strtmp = "";
		memcpy((LPTSTR)(LPCTSTR)strtmp,src,notelen);
		strp+=CreateNode("NOTE",strtmp);
		//[CTPNO]
		src = src+notelen;
		strtmp="";
		int ctpno = *(USHORT*)src;
		strtmp.Format("%d",ctpno);
		strp+=CreateNode("CTPNO",strtmp);
		//[FIRSTNM]
		src = src+2;
		int firstnmlen = *(USHORT*)src;
		src = src+2;
		strtmp="";
		memcpy((LPTSTR)(LPCTSTR)strtmp,src,firstnmlen);
		strp+=CreateNode("FIRSTNM",strtmp);
		//[LSTSTNM]
		src = src+firstnmlen;
		int lststnmlen = *(USHORT*)src;
		src = src+2;
		strtmp="";
		memcpy((LPTSTR)(LPCTSTR)strtmp,src,lststnmlen);
		strp+=CreateNode("LSTSTNM",strtmp);
		//[FZNM]
		src = src+lststnmlen;
		int fznmlen = *(USHORT*)src;
		src = src+2;
		strtmp="";
		memcpy((LPTSTR)(LPCTSTR)strtmp,src,fznmlen);
		strp+=CreateNode("FZNM",strtmp);
		//[HSFLAG]
		src=src+fznmlen;
		int hsflag= (unsigned char)src[0];
		strtmp.Format("%d",hsflag);
		strp+=CreateNode("HSFLAG",strtmp);
		//[ZBFLAG]
		src=src+1;
		int zbflag= (unsigned char)src[0];
		strtmp.Format("%d",zbflag);
		strp+=CreateNode("ZBFLAG",strtmp);
		//[JYFLAG]
		src=src+1;
		int jyflag= (unsigned char)src[0];
		strtmp.Format("%d",jyflag);
		strp+=CreateNode("JYFLAG",strtmp);
		xmlBzRecord+=CreateNode("BZ_RECORD",strp);
		strp="";
		//next
		src=src+1;
	}
	return xmlBzRecord;
}

CString CStreamXmlExchange::StreamTMtoXml(char *src,int len)
{
	CString str="";
	switch (len)
	{
	case 7:
		{
			int year = *(USHORT*)src;
			int Month = *(BYTE*)(src+2);
			int day = *(BYTE*)(src+3);
			int hour = *(BYTE*)(src+4);
			int minute = *(BYTE*)(src+5);
			int second = *(BYTE*)(src+6);
			str.Format("%04d-%02d-%02d %02d:%02d:%02d",year,Month,day,hour,minute,second);
		}
		break;
	case 8:
		{
			char year[5];
			char month[3];
			char day[3];
			memset(year,0,sizeof(year));
			memset(month,0,sizeof(month));
			memset(day,0,sizeof(day));
			memcpy(year,src,4);
			memcpy(month,src+4,2);
			memcpy(day,src+6,2);
			str.Format("%s-%s-%s 00:00:00",year,month,day);
		}
		break;
	default:
		str="0001-01-01 00:00:00";
		break;
	}
	return str;
}

int CStreamXmlExchange::XmlTmtoStream(char *src,char *dest,int len/* =0 */)
{
	ASSERT(src!=NULL);
	switch (len)
	{
	case 7:
		{
			int year=0;
			int month=0;
			int day=0;
			int hour=0;
			int minute=0;
			int second=0;
			sscanf(src,"%d-%d-%d %d:%d:%d",&year,&month,&day,&hour,&minute,&second);
			*(USHORT*)dest=year;
			*(BYTE*)(dest+2)=month;
			*(BYTE*)(dest+3)=day;
			*(BYTE*)(dest+4)=hour;
			*(BYTE*)(dest+5)=minute;
			*(BYTE*)(dest+6)=second;
		}
		break;
	case 8:
		{
			char year[4];
			char month[2];
			char day[2];
			memset(year,0,sizeof(year));
			memset(month,0,sizeof(month));
			memset(day,0,sizeof(day));
			char *token=NULL;
			token=strtok(src,"-");
			if(token!=NULL)
				memcpy(year,token,4);
			token=strtok(NULL,"-");
			if(token!=NULL)
				memcpy(month,token,2);
			token=strtok(NULL,"-");
			if(token!=NULL)
				memcpy(day,token,2);
			memcpy(dest,year,4);
			memcpy(dest+4,month,2);
			memcpy(dest+6,day,2);
		}
		break;
	default:
		break;
	}
	return len;
}

CString CStreamXmlExchange::CreateNode(CString node, CString content)
{
	CString str;
	str.Format("<%s>%s</%s>",node,content,node);
	return str;
}