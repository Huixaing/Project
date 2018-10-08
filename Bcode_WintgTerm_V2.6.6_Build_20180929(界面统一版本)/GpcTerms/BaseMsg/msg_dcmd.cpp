
#define EXTERN_LINKAGE_DEFINE
#include "msgid.h"

#include "msg_tcc.h"	//for import kmmark_t
#include "msg_dcmd.h"

CLBTH_NAMESPACE_BEGIN


IMPLEMENT_CLBTH(MsgDispatchCommandSend, MSGID_MsgDispatchCommandSend)
IMPLEMENT_SERIALIZE_OPERATOR(MsgDispatchCommandSend)
MsgDispatchCommandSend::MsgDispatchCommandSend()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)	
}

MsgDispatchCommandSend::~MsgDispatchCommandSend()
{
}

HRCODE MsgDispatchCommandSend::doSerialize(Archive& ar)
{
	IWORD len;
	if(ar.isStoring())
	{
		ar << send_entity << recv_entity <<send_time << edit_time;
		  

		ar << con_id << source_station;
	    ar << unique_cmdindex << bianMa << cmdNo;
	
		cmdName[sizeof(cmdName)-1]=0;			
		len=strlen(cmdName);
		if(len > sizeof(cmdName))
			return HRCODE(HRCODE::HC_FAIL);		
		ar << len;
		ar.write(cmdName, len);
		 
		rereader[sizeof(rereader)-1]=0;			
		len=strlen(rereader);
		if(len > sizeof(rereader))
			return HRCODE(HRCODE::HC_FAIL);		
		ar << len;
		ar.write(rereader, len);
		
		dispatcher[sizeof(dispatcher)-1]=0;			
		len=strlen(dispatcher);
		if(len > sizeof(dispatcher))
			return HRCODE(HRCODE::HC_FAIL);		
		ar<<len;
		ar.write(dispatcher, len);

		unit[sizeof(unit)-1]=0;			
		len=strlen(unit);
		if(len > sizeof(unit))
			return HRCODE(HRCODE::HC_FAIL);		
		ar<<len;
		ar.write(unit, len);
		
		reader[sizeof(reader)-1]=0;			
		len=strlen(reader);
		if(len > sizeof(reader))
			return HRCODE(HRCODE::HC_FAIL);		
		ar<<len;
		ar.write(reader, len);

		
		int i;
		ar<< recv_count;		 
		for( i=0;i<recv_count;i++)
		{
			ar << recv_list[i].recv_entity;		 
			ar.write(recv_list[i].chaosongMemo, 64);
		}
		
		ar<< chaosong_recv_count;
		for( i=0;i<chaosong_recv_count;i++)
			ar << chaosong_recv_list[i];
		
		chaosongMemo[sizeof(chaosongMemo)-1]=0;			
		len=strlen(chaosongMemo);
		if(len > sizeof(chaosongMemo))
			return HRCODE(HRCODE::HC_FAIL);		
		ar << len;
		ar.write(chaosongMemo, len);
		
		ar << chaosong_train_list_count;
		for( i=0;i<chaosong_train_list_count;i++)
		{
            ar.write( chaosong_train_list[i].train_id, MAX_TRAIN_NO_LEN );					
			ar.write( chaosong_train_list[i].locomotive, MAX_TRAIN_NO_LEN );
			ar << chaosong_train_list[i].recv_lirc;
		}

		ar << (short)cmd_type;	
		ar << (short)send_way;
		ar << (short)cmd_destination;	
		
		ar << ContentLength;
		if(ContentLength > sizeof(content))
			return HRCODE(HRCODE::HC_FAIL);	
		
		content[ContentLength]=0;			
		len=strlen(content);
		if(len > sizeof(content))
			return HRCODE(HRCODE::HC_FAIL);		
		ar<<len;
		ar.write(content, len);

		ar.write(train_id, MAX_TRAIN_NO_LEN );					
		ar.write(locomotive, MAX_TRAIN_NO_LEN );
//列控相关
		ar << (IBYTE)bTccCmd;
		ar << nCmdID;
		ar << nStaID[0] << nStaID[1];
		ar << nTccLineID;
		ar << nTccNum;
		
		for(i=0; i<nTccNum; i++)
		{
			ar << tccline[i].nTccID;
			ar << tccline[i].nTccLineID;
		}
	

		ar << (IBYTE)bFullLimit;
		ar << nKmMark[0] << nKmMark[1];
		ar << (IBYTE)bStartNow;
		ar << (IBYTE)bEndless;
		ar << nTime[0]<< nTime[1];
		ar << (IBYTE)bCancelLimit;
		ar << nTrainID;
		for(i=0; i<nTrainID; i++)
		{
			ar.write(train_cmd[i].szTrainID, MAX_TRAIN_NO_LEN );	//车次号
			ar << train_cmd[i].nSpeedMaxVal;	//限速值
		} 
//列控end
	}
	else
	{
		ar >> send_entity >> recv_entity >> send_time >> edit_time;
	 
		ar >> con_id >> source_station;
		ar >> unique_cmdindex >> bianMa >> cmdNo;

		ar >> len;
		if(len > sizeof(cmdName)-1)
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(cmdName, len);
		cmdName[len]=0;

		ar >> len;
		if(len > sizeof(rereader)-1)
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(rereader, len);
		rereader[len]=0;

		ar >> len;
		if(len > sizeof(dispatcher)-1)
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(dispatcher, len);
		dispatcher[len]=0;
		
		ar >> len;
		if(len > sizeof(unit)-1)
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(unit, len);
		unit[len]=0;

		ar >> len;
		if(len > sizeof(reader)-1)
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(reader, len);
		reader[len]=0;

		
		int i;
		ar >> recv_count;
		for( i=0;i<recv_count;i++)
		{
			ar >> recv_list[i].recv_entity;
			ar.read(recv_list[i].chaosongMemo, 64);
		}
		ar>> chaosong_recv_count;	 
		for( i=0;i<chaosong_recv_count;i++)
			ar >> chaosong_recv_list[i];
	 
		ar >> len;
		if(len > sizeof(chaosongMemo)-1)
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(chaosongMemo, len);
		chaosongMemo[len]=0;
		 
		ar >> chaosong_train_list_count;
		for( i=0;i<chaosong_train_list_count;i++)
		{
            ar.read( chaosong_train_list[i].train_id, MAX_TRAIN_NO_LEN );					
			ar.read( chaosong_train_list[i].locomotive, MAX_TRAIN_NO_LEN );	
			chaosong_train_list[i].train_id[MAX_TRAIN_NO_LEN-1] = '\0';
			chaosong_train_list[i].locomotive[MAX_TRAIN_NO_LEN-1] = '\0';
			ar >> chaosong_train_list[i].recv_lirc;
		}
		short t;
		ar >> t;
		cmd_type = (CMD_TYPE)t;
	 	
		ar >> t;
		send_way = (SEND_WAY)t;

		ar >> t;
		cmd_destination = (CMD_DESTINATION)t;
	 
		ar >> ContentLength;

		ar >> len;
		if(len > sizeof(content)-1)
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(content, len);
		content[len]=0;

	    ar.read(train_id,MAX_TRAIN_NO_LEN);
		ar.read(locomotive,MAX_TRAIN_NO_LEN);
		train_id[MAX_TRAIN_NO_LEN-1] = '\0';
	 	locomotive[MAX_TRAIN_NO_LEN-1] = '\0';

//列控相关		 
		ar >> bTccCmd;
		ar >> nCmdID;
		ar >> nStaID[0] >> nStaID[1];
		ar >> nTccLineID;
		ar >> nTccNum;
		
		for(i=0; i<nTccNum; i++)
		{
			ar >> tccline[i].nTccID;
			ar >> tccline[i].nTccLineID;
		}
	
		ar >> bFullLimit;
		ar >> nKmMark[0] >> nKmMark[1];
		ar >> bStartNow;
		ar >> bEndless;
		ar >> nTime[0] >> nTime[1];
		ar >> bCancelLimit;

		ar >> nTrainID;
		for(i=0; i<nTrainID; i++)
		{
			ar.read( train_cmd[i].szTrainID, MAX_TRAIN_NO_LEN);
			ar >> train_cmd[i].nSpeedMaxVal;
		}

//列控end
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgDispatchCommandSend::countObjectLength()
{	
	return sizeof(send_entity)+sizeof(recv_entity)+sizeof(send_time)+sizeof(edit_time) + sizeof(con_id)  +
	sizeof(unique_cmdindex) +sizeof(bianMa) + sizeof(cmdNo) + sizeof(cmdName) +
	sizeof(rereader) + sizeof(dispatcher) + sizeof(unit) + sizeof(reader) + 
	sizeof(recv_count) + recv_count*sizeof(RECV_LIST) +
	sizeof(chaosong_recv_count)+ chaosong_recv_count*sizeof(chaosong_recv_list[0]) +
	sizeof(chaosong_train_list_count) + chaosong_train_list_count*sizeof(chaosong_train_list[0])+
	sizeof(chaosongMemo) + sizeof(cmd_type) + sizeof(send_way) +  sizeof(cmd_destination) + 
	sizeof(ContentLength) + sizeof(content) + sizeof(MAX_TRAIN_NO_LEN) + sizeof(MAX_TRAIN_NO_LEN)+
	sizeof(bTccCmd)+sizeof(nCmdID)+sizeof(nStaID)+sizeof(nTccLineID)+sizeof(nTccNum)+nTccNum*sizeof(tccline[0])+
	sizeof(bFullLimit)+sizeof(nKmMark)+sizeof(bStartNow)+sizeof(bEndless)+sizeof(nTime)+sizeof(bCancelLimit)+
	sizeof(nTrainID)+nTrainID*sizeof(train_cmd[0]);
	
 
}


IMPLEMENT_CLBTH(MsgDispatchCommandStatus, MSGID_MsgDispatchCommandStatus)
IMPLEMENT_SERIALIZE_OPERATOR(MsgDispatchCommandStatus)
MsgDispatchCommandStatus::MsgDispatchCommandStatus()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)	
}

MsgDispatchCommandStatus::~MsgDispatchCommandStatus()
{
}

HRCODE MsgDispatchCommandStatus::doSerialize(Archive& ar)
{
	IBYTE len;
	if(ar.isStoring())
	{
		ar << send_entity << recv_entity <<send_time << recv_time;
	    ar << unique_cmdindex << bianMa << cmdNo;
	
		ar << (short)cmd_type;		
		
		ar.write(train_id, MAX_TRAIN_NO_LEN );					
		ar.write(locomotive, MAX_TRAIN_NO_LEN );		
	 
		ar << (short)recv_destin;	
		
		ar << (short)recv_status;	
		
		sign_name[sizeof(sign_name)-1]=0;			
		len=strlen(sign_name);
		if(len > sizeof(sign_name))
			return HRCODE(HRCODE::HC_FAIL);		
		ar<<len;
		ar.write(sign_name, len);
		
		ar << sign_time;
	}
	else
	{
		ar >> send_entity >> recv_entity >> send_time >> recv_time;
		ar >> unique_cmdindex >> bianMa >> cmdNo;

		short t;
		ar >> t;
		cmd_type = (CMD_TYPE)t;
	 
		ar.read(train_id, MAX_TRAIN_NO_LEN );					
		ar.read(locomotive, MAX_TRAIN_NO_LEN );	
		train_id[MAX_TRAIN_NO_LEN-1] = '\0';
	 	locomotive[MAX_TRAIN_NO_LEN-1] = '\0';
		
		ar >> t;
		recv_destin = (RECV_DESTIN)t;
			 
		ar >> t;
		recv_status = (RECV_STATUS)t;
	 
		ar >> len;
		if(len > sizeof(sign_name)-1)
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(sign_name, len);
		sign_name[len]=0;
        
		ar >> sign_time;
	 
		
	 
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgDispatchCommandStatus::countObjectLength()
{	
	return sizeof(send_entity)+sizeof(recv_entity)+sizeof(send_time)+sizeof(recv_time) +
	sizeof(unique_cmdindex) +sizeof(bianMa) + sizeof(cmdNo) + sizeof(cmd_type) +
    sizeof(MAX_TRAIN_NO_LEN) + sizeof(MAX_TRAIN_NO_LEN) +sizeof(recv_status)+ sizeof(recv_destin)+
	sizeof(sign_name) + sizeof(sign_time);

 
}

IMPLEMENT_CLBTH(MsgDCmdTcc, MSGID_MsgDCmdTcc)
IMPLEMENT_SERIALIZE_OPERATOR(MsgDCmdTcc)
MsgDCmdTcc::MsgDCmdTcc()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
MsgDCmdTcc::~MsgDCmdTcc()
{

}
HRCODE MsgDCmdTcc::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nCmdType << nDirection;
		ar << nConsoleID;
		ar.write(raw_sysid, sizeof(raw_sysid));
		ar << nCmdID << nStaID[0] << nStaID[1] << nLineID
			<< nKmMark[0] << nKmMark[1] << nTime[0] << nTime[1];

		IBYTE booltag = 0;
		if(bFullLimit)
			booltag |= 0x01;
		if(bStartNow)
			booltag |= 0x02;
		if(bEndless)
			booltag |= 0x04;
		if(bCancelLimit)
			booltag |= 0x08;
		if(bChecked)
			booltag |= 0x10;
		ar << booltag;

		if(nTccNum > sizeof(tcc)/sizeof(tcc[0]))
			return HRCODE(HRCODE::HC_FAIL);
		ar << nTccNum;
		int i = 0;
		for (i=0; i<nTccNum; i++)
		{
			ar << tcc[i].nTccID << tcc[i].nTccLineID << tcc[i].nCheckorID << tcc[i].nKmMark[0] << tcc[i].nKmMark[1];
			char b = tcc[i].bNeedCheck ? 1 : 0;
			ar << b;
		}

		if(nTrainNum > sizeof(train_cmd)/sizeof(train_cmd[0]))
			return HRCODE(HRCODE::HC_FAIL);
		ar << nTrainNum;
		for (i=0; i<nTrainNum; i++)
		{
			ar << train_cmd[i].nSpeedMaxVal << train_cmd[i].nSpeed;

			IWORD len = strlen(train_cmd[i].szTrainID);
			if(len > sizeof(train_cmd[i].szTrainID))
				len = sizeof(train_cmd[i].szTrainID);
			ar << len;
			ar.write(train_cmd[i].szTrainID, len);
		}

	}
	else
	{
		ar >> nCmdType >> nDirection;
		ar >> nConsoleID;
		ar.read(raw_sysid, sizeof(raw_sysid));
		ar >> nCmdID >> nStaID[0] >> nStaID[1] >> nLineID
			>> nKmMark[0] >> nKmMark[1] >> nTime[0] >> nTime[1];

		IBYTE booltag = 0;
		ar >> booltag;

		bFullLimit = (booltag & 0x01) ? ITRUE : IFALSE;
		bStartNow  = (booltag & 0x02) ? ITRUE : IFALSE;
		bEndless   = (booltag & 0x04) ? ITRUE : IFALSE;
		bCancelLimit=(booltag & 0x08) ? ITRUE : IFALSE;
		bChecked   = (booltag & 0x10) ? ITRUE : IFALSE;

		ar >> nTccNum;
		if(nTccNum > sizeof(tcc)/sizeof(tcc[0]))
			return HRCODE(HRCODE::HC_FAIL);
		int i = 0;
		for (i=0; i<nTccNum; i++)
		{
			ar >> tcc[i].nTccID >> tcc[i].nTccLineID >> tcc[i].nCheckorID >> tcc[i].nKmMark[0] >> tcc[i].nKmMark[1];
			char b;
			ar >> b;
			tcc[i].bNeedCheck = b;
		}

		ar >> nTrainNum;
		if(nTrainNum > sizeof(train_cmd)/sizeof(train_cmd[0]))
			return HRCODE(HRCODE::HC_FAIL);
		for (i=0; i<nTrainNum; i++)
		{
			ar >> train_cmd[i].nSpeedMaxVal >> train_cmd[i].nSpeed;
			IWORD len = 0;
			ar >> len;
			if(len > sizeof(train_cmd[i].szTrainID))
				return HRCODE(HRCODE::HC_FAIL);
			ar.read(train_cmd[i].szTrainID, len);
		}

	}
	return HRCODE(HRCODE::HC_SUCCESS);
}

size_t MsgDCmdTcc::countObjectLength()
{
	return sizeof(nConsoleID)+sizeof(nCmdID)+sizeof(nStaID)+sizeof(nLineID)+sizeof(nKmMark)+sizeof(nTime)+1
		+sizeof(nTccNum)+nTccNum*sizeof(tcc[0])
		+sizeof(nTrainNum)+nTrainNum*sizeof(train_cmd[0]);
}




IMPLEMENT_CLBTH(MsgDCmdTcc2, MSGID_MsgDCmdTcc2)
IMPLEMENT_SERIALIZE_OPERATOR(MsgDCmdTcc2)
MsgDCmdTcc2::MsgDCmdTcc2()
{
	CLBTH_ZERO_INIT_OBJECT(MsgDCmdTcc);
}
MsgDCmdTcc2::~MsgDCmdTcc2()
{

}
HRCODE MsgDCmdTcc2::doSerialize(Archive& ar)
{
	HRCODE hr = MsgDCmdTcc::doSerialize(ar);
	if(hr.failed())
		return hr;

	nReserveLen = 0;

	if(ar.isStoring())
	{
		ar << nCreatorID;	//草拟者
		ar << nCheckorID;	//中心校验者
		ar << nConfirmorID;	//确认分解者
		ar << nCreateTime;	//命令创建时间

		ar << reason << nReserveLen;
	}
	else
	{
		ar >> nCreatorID;	//草拟者
		ar >> nCheckorID;	//中心校验者
		ar >> nConfirmorID;	//确认分解者
		ar >> nCreateTime;	//命令创建时间

		ar >> reason >> nReserveLen;
		IWORD nLen = nReserveLen;
		if(nLen)
			ar.seek(Archive::CURRENT, nLen);
	}

	return HRCODE(HRCODE::HC_SUCCESS);
}

size_t MsgDCmdTcc2::countObjectLength()
{
	return MsgDCmdTcc::countObjectLength()
		+sizeof(nCreatorID)
		+sizeof(nCheckorID)
		+sizeof(nConfirmorID)
		+sizeof(nCreateTime)
		+ sizeof(reason) + sizeof(nReserveLen);
}



CLBTH_NAMESPACE_END

