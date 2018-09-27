#pragma once

class CStreamXmlExchange
{
public:
	CStreamXmlExchange(void);
	~CStreamXmlExchange(void);
public:
	//0x01
	int AckBzStreamToxml(char *src,int len,char *dest);
	int ReqBzXmlToStream(char *src,int len,char*dest);
	//0x02
	int AckYLByGidStreamToxml(char *src,int len,char *dest);
	int ReqYLByGidXmlToStream(char *src,int len,char *dest);
	//0x03
	int AckYLbyTimeStreamToxml(char *src,int len,char *dest);
	int ReqYLbyTimeXmlToStream(char *src,int len,char *dest);
	//0x04
	int AckYLbyTnoStreamToxml(char *src,int len,char *dest);
	int ReqYLbyTnoXmlToStream(char *src,int len,char *dest);//0x04请求
	//0x05
	int AckYLbyStartTimeAndGidStreamToxml(char *src,int len,char *dest);
	int ReqYLbyStartTimeAndGidXmlToStream(char *src,int len,char*dest);
	//0x06
	int AckGidbyTnoStreamToxml(char *src,int len,char *dest);
	int ReqGidbyTnoXmlToStream(char *src,int len,char *dest);
	//0x07
	int AckTrainTflagStreamToxml(char*src,int len,char *dest);
	int ReqTrainTflagXmlToStream(char *src,int len,char *dest);
	//0x08
	int AckTrainNewStatusStreamToxml(char*src,int len,char *dest);
	int ReqTrainNewStatusXmlToStream(char *src,int len,char *dest);
	//0x09
	int AckJhDuPlanStreamToxml(char*src,int len,char* dest);
	int SendJhDuPlanXmlToStream(char *src,int len,char*dest);
	//0x0a
	int AckKdAdjustPlanStreamToxml(char *src,int len,char* dest);
	int SendKdAdjustPlanXmlToStream(char *src,int len,char *dest);
	//0x0b
	int AckOtherInfoStreamToxml(char *src,int len,char*dest);
	int SendOtherInfoXmlToStream(char *src,int len,char *dest);
	//0x0d
	int AckDisCmdStreamToxml(char *src,int len,char*dest);
	int ReqDisCmdXmlToStream(char *src,int len,char *dest);
	//0x0e
	int AckWorkFlagInfoStreamToxml(char*src,int len,char*dest);
	int ReqWorkFlagInfoXmlToStream(char*src,int len,char *dest);
	//0x0f
	int AckWorkMidRsInfoStreamToxml(char *src,int len,char *dest);
	int ReqWorkMidRsInfoXmlToStream(char *src,int len,char *dest);
	//0x10
	int AckKdCmdInfoStreamToxml(char *src,int len,char*dest);
	int ReqKdCmdInfoXmlToStream(char *src,int len,char*dest);
	//0x12
	int AckBsInfoStreamToxml(char *src,int len,char *dest);
	int ReqBsInfoXmlToStream(char *src,int len,char*dest);
	//0x13
	int AckWorkPlanInfoStreamToxml(char *src,int len,char*dest);
	int ReqWorkPlanInfoXmlToStream(char *src,int len,char*dest);
	//0x14
	int AckWorkDescriptionStreamToxml(char *src,int len,char*dest);
	int ReqWorkDescriptionXmlToStream(char *src,int len,char*dest);

	CString CreateNode(CString node, CString content);
	CString StreamBzRecordToXml(char *src,int len);
	CString StreamTMtoXml(char *src,int len=0);//时间转化
	int XmlTmtoStream(char *src,char *dest,int len=0);//时间转化
	CString StreamYlSTinfoToXml(char *src,int len=0);

};
