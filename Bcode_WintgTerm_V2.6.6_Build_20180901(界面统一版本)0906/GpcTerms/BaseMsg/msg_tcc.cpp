
#define EXTERN_LINKAGE_DEFINE
#include "msgid.h"
#include "msg_tcc.h"

CLBTH_NAMESPACE_BEGIN


//GSMR 消息结束

// 列控中心消息定义开始
// 临时限速命令下达
IMPLEMENT_CLBTH(MsgStccCmdSend, MSGID_MsgStccCmdSend)
IMPLEMENT_SERIALIZE_OPERATOR(MsgStccCmdSend)
MsgStccCmdSend::MsgStccCmdSend()
{
}
MsgStccCmdSend::~MsgStccCmdSend()
{
}

HRCODE MsgStccCmdSend::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << source << cmd_type << sys_id << cmd_no 
			<< start_tm << end_tm << line_no << start_pos << end_pos
			<< speed 
			<< start_sta_no << end_sta_no 
			<< confirm
			<< packet_sum << packet_no;
	}
	else
	{
		ar >> source >> cmd_type >> sys_id >> cmd_no 
			>> start_tm >> end_tm >> line_no >> start_pos >> end_pos
			>> speed 
			>> start_sta_no >> end_sta_no
			>> confirm
			>> packet_sum >> packet_no;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgStccCmdSend::countObjectLength()
{
	return sizeof(source)+sizeof(cmd_type)+sizeof(sys_id)+sizeof(cmd_no)
		+sizeof(start_tm)+sizeof(end_tm)+sizeof(line_no)+sizeof(start_pos)+sizeof(end_pos)
		+sizeof(speed)
		+sizeof(start_sta_no)+sizeof(end_sta_no)
		+sizeof(confirm)
		+sizeof(packet_sum)+sizeof(packet_no);
}

// 临时限速命令执行结果
IMPLEMENT_CLBTH(MsgStccCmdStatus, MSGID_MsgStccCmdStatus)
IMPLEMENT_SERIALIZE_OPERATOR(MsgStccCmdStatus)
MsgStccCmdStatus::MsgStccCmdStatus()
{
}
MsgStccCmdStatus::~MsgStccCmdStatus()
{
}

HRCODE MsgStccCmdStatus::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << action_type << status_type << sta_no << sys_id
			<< cmd_no << start_tm << end_tm << line_no << start_pos << end_pos
			<< speed << packet_sum << packet_no;
	}
	else
	{
		ar >> action_type >> status_type >> sta_no >> sys_id
			>> cmd_no >> start_tm >> end_tm >> line_no >> start_pos >> end_pos
			>> speed >> packet_sum >> packet_no;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgStccCmdStatus::countObjectLength()
{
	return sizeof(action_type)+sizeof(status_type)+sizeof(sta_no)+sizeof(sys_id)
		+sizeof(cmd_no)+sizeof(start_tm)+sizeof(end_tm)+sizeof(line_no)
		+sizeof(start_pos)+sizeof(end_pos)
		+sizeof(speed)+sizeof(packet_sum)+sizeof(packet_no);
}

// 车站列控状态报告
IMPLEMENT_CLBTH(MsgStccReport, MSGID_MsgStccReport)
IMPLEMENT_SERIALIZE_OPERATOR(MsgStccReport)
MsgStccReport::MsgStccReport()
{
}
MsgStccReport::~MsgStccReport()
{
}

HRCODE MsgStccReport::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << sta_no << comm_active 
			<< stcc_master[0] << stcc_master[1]
			<< ilock_channel[0] << ilock_channel[1]
			<< ctc_channel[0] << ctc_channel[1]
			<< leu_channel[0] << leu_channel[1] << leu_channel[2] << leu_channel[3]
			<< leu1_ans[0] << leu1_ans[1] << leu1_ans[2] << leu1_ans[3]
			<< leu2_ans[0] << leu2_ans[1] << leu2_ans[2] << leu2_ans[3]
			<< leu3_ans[0] << leu3_ans[1] << leu3_ans[2] << leu3_ans[3]
			<< leu4_ans[0] << leu4_ans[1] << leu4_ans[2] << leu4_ans[3]
			<< init_section_num
			<< init_line_no[0] << init_line_no[1] << init_line_no[2] << init_line_no[3]
			<< init_line_no[4] << init_line_no[5] << init_line_no[6] << init_line_no[7]
			<< init_direction[0] << init_direction[1] << init_direction[2] << init_direction[3]
			<< init_direction[4] << init_direction[5] << init_direction[6] << init_direction[7];
	}
	else
	{
		ar >> sta_no >> comm_active 
			>> stcc_master[0] >> stcc_master[1]
			>> ilock_channel[0] >> ilock_channel[1]
			>> ctc_channel[0] >> ctc_channel[1]
			>> leu_channel[0] >> leu_channel[1] >> leu_channel[2] >> leu_channel[3]
			>> leu1_ans[0] >> leu1_ans[1] >> leu1_ans[2] >> leu1_ans[3]
			>> leu2_ans[0] >> leu2_ans[1] >> leu2_ans[2] >> leu2_ans[3]
			>> leu3_ans[0] >> leu3_ans[1] >> leu3_ans[2] >> leu3_ans[3]
			>> leu4_ans[0] >> leu4_ans[1] >> leu4_ans[2] >> leu4_ans[3]
			>> init_section_num
			>> init_line_no[0] >> init_line_no[1] >> init_line_no[2] >> init_line_no[3]
			>> init_line_no[4] >> init_line_no[5] >> init_line_no[6] >> init_line_no[7]
			>> init_direction[0] >> init_direction[1] >> init_direction[2] >> init_direction[3]
			>> init_direction[4] >> init_direction[5] >> init_direction[6] >> init_direction[7];
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgStccReport::countObjectLength()
{
	return sizeof(sta_no)+sizeof(comm_active)
		+sizeof(stcc_master[0])*2
		+sizeof(ilock_channel[0])*2
		+sizeof(ctc_channel[0])*2
		+sizeof(leu_channel[0])*4
		+sizeof(leu1_ans[0])*4
		+sizeof(leu2_ans[0])*4
		+sizeof(leu3_ans[0])*4
		+sizeof(leu4_ans[0])*4
		+sizeof(init_section_num)
		+sizeof(init_line_no[0])*8
		+sizeof(init_direction[0])*8;
}

// 服务器发向STPC, GPC的确认列表
IMPLEMENT_CLBTH(MsgTccCommand, MSGID_MsgTccCommand)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTccCommand)
MsgTccCommand::MsgTccCommand()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
MsgTccCommand::~MsgTccCommand()
{
}
HRCODE MsgTccCommand::doSerialize(Archive& ar)
{
	enum {
		FULL_LIMIT   = 0x01,
		START_NOW    = 0x02,
		END_LESS     = 0x04,
		CANCEL_LIMIT = 0x08,
	};

	int i;
	if(ar.isStoring())
	{
		ar << console_no;
		ar.write(sys_id, sizeof(sys_id));
		ar.write(raw_sysid, sizeof(raw_sysid));
		ar << cmd_no 
			<< sta_no[0] << sta_no[1]
			<< line_no
			<< km_marker[0] << km_marker[1]
			<< time[0] << time[1]
			<< speed;

		ar << creator << checkor << confirmor;

		IBYTE booltag = 0;
		if(bFullLimit)
			booltag |= FULL_LIMIT;
		if(bStartNow)
			booltag |= START_NOW;
		if(bEndless)
			booltag |= END_LESS;
		if(bCancelLimit)
			booltag |= CANCEL_LIMIT;

		ar << booltag;

		ar << alarm_num;
		if (alarm_num > sizeof(alarm)/sizeof(alarm[0]))
			return HRCODE(HRCODE::HC_FAIL);		
		for(i=0; i<alarm_num; i++)
			ar << alarm[i];

		ar << tcc_num;
		if(tcc_num > sizeof(tcc_no)/sizeof(tcc_no[0]))
			return HRCODE(HRCODE::HC_FAIL);

		for(i=0; i<tcc_num; i++)
		{
			ar << tcc_no[i]
				<< tcc_line_no[i]
				<< tcc_status[i]
				<< tcc_time[i]
				<< tcc_checkor[i]
				<< tcc_sender[i]
				<< tcc_alarm_num[i];

			if (tcc_alarm_num[i] > sizeof(tcc_alarm[i])/sizeof(tcc_alarm[i][0]))
				return HRCODE(HRCODE::HC_FAIL);

			for(int j=0; j<tcc_alarm_num[i]; j++)
				ar << tcc_alarm[i][j];

		}

		ar << nMarkIndex << create_time;
	}
	else
	{
		ar >> console_no;
		ar.read(sys_id, sizeof(sys_id));
		ar.read(raw_sysid, sizeof(raw_sysid));
		ar >> cmd_no 
			>> sta_no[0] >> sta_no[1]
			>> line_no
			>> km_marker[0] >> km_marker[1]
			>> time[0] >> time[1]
			>> speed;

		ar >> creator >> checkor >> confirmor;

		IBYTE booltag;
		ar >> booltag;
		bFullLimit = 0 != (booltag & FULL_LIMIT);
		bStartNow = 0 != (booltag & START_NOW);
		bEndless = 0 != (booltag & END_LESS);
		bCancelLimit = 0 != (booltag & CANCEL_LIMIT);

		ar >> alarm_num;
		if (alarm_num > sizeof(alarm)/sizeof(alarm[0]))
			return HRCODE(HRCODE::HC_FAIL);		
		for(i=0; i<alarm_num; i++)
			ar >> alarm[i];

		ar >> tcc_num;
		if(tcc_num > sizeof(tcc_no)/sizeof(tcc_no[0]))
			return HRCODE(HRCODE::HC_FAIL);

		for(i=0; i<tcc_num; i++)
		{
			ar >> tcc_no[i]
				>> tcc_line_no[i]
				>> tcc_status[i]
				>> tcc_time[i]
				>> tcc_checkor[i]
				>> tcc_sender[i]
				>> tcc_alarm_num[i];

				if (tcc_alarm_num[i] > sizeof(tcc_alarm[i])/sizeof(tcc_alarm[i][0]))
					return HRCODE(HRCODE::HC_FAIL);

				for(int j=0; j<tcc_alarm_num[i]; j++)
					ar >> tcc_alarm[i][j];

		}

		ar >> nMarkIndex >> create_time;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}

size_t MsgTccCommand::countObjectLength()
{
	size_t t=sizeof(console_no)
		+sizeof(sys_id) + sizeof(raw_sysid)
		+sizeof(cmd_no)
		+sizeof(sta_no)
		+sizeof(line_no)
		+sizeof(km_marker)
		+sizeof(time)
		+sizeof(speed)
		+sizeof(creator)
		+sizeof(checkor)
		+sizeof(confirmor)
		+1
		+sizeof(alarm_num)
		+sizeof(alarm[0])*alarm_num
		+sizeof(tcc_num)
		+sizeof(tcc_no[0])*tcc_num
		+sizeof(tcc_line_no[0])*tcc_num
		+sizeof(tcc_status[0])*tcc_num
		+sizeof(tcc_time[0])*tcc_num
		+sizeof(tcc_checkor[0])*tcc_num
		+sizeof(tcc_sender[0])*tcc_num
		+sizeof(tcc_alarm_num[0])*tcc_num
		+sizeof(nMarkIndex)
		+sizeof(create_time);

	for(int i=0; i<tcc_num; i++)
	{
		t+=sizeof(tcc_alarm[i][0])*tcc_alarm_num[i];
	}

	return t;
}

// 服务器操作
IMPLEMENT_CLBTH(MsgTccCommandAdd, MSGID_MsgTccCommandAdd)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTccCommandAdd)
MsgTccCommandAdd::MsgTccCommandAdd()
{
}
MsgTccCommandAdd::~MsgTccCommandAdd()
{
}

IMPLEMENT_CLBTH(MsgTccCommandDel, MSGID_MsgTccCommandDel)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTccCommandDel)
MsgTccCommandDel::MsgTccCommandDel()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
MsgTccCommandDel::~MsgTccCommandDel()
{
}
HRCODE MsgTccCommandDel::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << console_no;
		ar.write(sys_id, sizeof(sys_id));
		ar << cmd_no << tcc_no;
	}
	else
	{
		ar >> console_no;
		ar.read(sys_id, sizeof(sys_id));
		ar >> cmd_no >> tcc_no;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}

size_t MsgTccCommandDel::countObjectLength()
{
	return sizeof(console_no)
		+sizeof(sys_id)
		+sizeof(cmd_no)
		+sizeof(tcc_no);
}

IMPLEMENT_CLBTH(MsgTccCommandDump, MSGID_MsgTccCommandDump)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTccCommandDump)
MsgTccCommandDump::MsgTccCommandDump()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
MsgTccCommandDump::~MsgTccCommandDump()
{
}
HRCODE MsgTccCommandDump::doSerialize(Archive& ar)
{
	if(ar.isStoring())
		ar << console_no;
	else
		ar >> console_no;
	return HRCODE(HRCODE::HC_SUCCESS);
}

size_t MsgTccCommandDump::countObjectLength()
{
	return sizeof(console_no);
}

IMPLEMENT_CLBTH(MsgTccCommandQuery, MSGID_MsgTccCommandQuery)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTccCommandQuery)
MsgTccCommandQuery::MsgTccCommandQuery()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
MsgTccCommandQuery::~MsgTccCommandQuery()
{
}
HRCODE MsgTccCommandQuery::doSerialize(Archive& ar)
{
	if(ar.isStoring())
		ar << console_no << tcc_no;
	else
		ar >> console_no >> tcc_no;
	return HRCODE(HRCODE::HC_SUCCESS);
}

size_t MsgTccCommandQuery::countObjectLength()
{
	return sizeof(console_no)
		+sizeof(tcc_no);
}

// 临时限速命令下达
IMPLEMENT_CLBTH(MsgStccCommandSend, MSGID_MsgStccCommandSend)
IMPLEMENT_SERIALIZE_OPERATOR(MsgStccCommandSend)
MsgStccCommandSend::MsgStccCommandSend()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
MsgStccCommandSend::~MsgStccCommandSend()
{
}
HRCODE MsgStccCommandSend::doSerialize(Archive& ar)
{
	enum {
		FULL_LIMIT   = 0x01,
		START_NOW    = 0x02,
		END_LESS     = 0x04,
		CANCEL_LIMIT = 0x08,
		INIT_OP		 = 0x10,
	};

	if(ar.isStoring())
	{
		ar << console_no;
		ar.write(sys_id, sizeof(sys_id));
		ar << cmd_no 
			<< sta_no[0] << sta_no[1] << line_no
			<< km_marker[0] << km_marker[1]
			<< time[0] << time[1]
			<< speed
			<< tcc_no
			<< tcc_line_no
			<< tcc_sender
			<< section_dir;

		IBYTE booltag = 0;
		if(bFullLimit)
			booltag |= FULL_LIMIT;
		if(bStartNow)
			booltag |= START_NOW;
		if(bEndless)
			booltag |= END_LESS;
		if(bCancelLimit)
			booltag |= CANCEL_LIMIT;
		if(bInitOpration)
			booltag |= INIT_OP;

		ar << booltag;
	}
	else
	{
		ar >> console_no;
		ar.read(sys_id, sizeof(sys_id));
		ar >> cmd_no 
			>> sta_no[0] >> sta_no[1] >> line_no
			>> km_marker[0] >> km_marker[1]
			>> time[0] >> time[1]
			>> speed
			>> tcc_no
			>> tcc_line_no
			>> tcc_sender
			>> section_dir;

		IBYTE booltag;
		ar >> booltag;
		bFullLimit = 0 != (booltag & FULL_LIMIT);
		bStartNow = 0 != (booltag & START_NOW);
		bEndless = 0 != (booltag & END_LESS);
		bCancelLimit = 0 != (booltag & CANCEL_LIMIT);
		bInitOpration = 0 != (booltag & INIT_OP);
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}

size_t MsgStccCommandSend::countObjectLength()
{
	return sizeof(console_no)
		+sizeof(sys_id)
		+sizeof(cmd_no)
		+sizeof(sta_no)
		+sizeof(line_no)
		+sizeof(km_marker)
		+sizeof(time)
		+sizeof(speed)
		+sizeof(tcc_no)
		+sizeof(tcc_line_no)
		+sizeof(tcc_sender)
		+sizeof(section_dir)
		+1;
}

// LiRC、GPC发向服务器的命令状态改变(???)
IMPLEMENT_CLBTH(MsgTccCommandStatusChanged, MSGID_MsgTccCommandStatusChanged)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTccCommandStatusChanged)
MsgTccCommandStatusChanged::MsgTccCommandStatusChanged()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
MsgTccCommandStatusChanged::~MsgTccCommandStatusChanged()
{
}
HRCODE MsgTccCommandStatusChanged::doSerialize(Archive& ar)
{
	enum {
		STATUS_CHANGED = 0x01,
		SENDER_SET = 0x02,
		TIME_SET = 0x04,
	};

	if(ar.isStoring())
	{
		ar << console_no;
		ar.write(sys_id, sizeof(sys_id));
		ar << cmd_no << tcc_no
			<< tcc_status << tcc_sender << tcc_time;

		IBYTE booltag = 0;
		if(b_status_changed)
			booltag |= STATUS_CHANGED;
		if(b_sender_set)
			booltag |= SENDER_SET;
		if(b_time_set)
			booltag |= TIME_SET;
		ar << booltag;
	}
	else
	{
		ar >> console_no;
		ar.read(sys_id, sizeof(sys_id));
		ar >> cmd_no >> tcc_no
			>> tcc_status >> tcc_sender >> tcc_time;

		IBYTE booltag = 0;
		ar >> booltag;
		b_status_changed = 0 != (booltag & STATUS_CHANGED);
		b_sender_set = 0 != (booltag & SENDER_SET);
		b_time_set = 0 != (booltag & TIME_SET);
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}

size_t MsgTccCommandStatusChanged::countObjectLength()
{
	return sizeof(console_no)
		+sizeof(sys_id)
		+sizeof(cmd_no)
		+sizeof(tcc_no)
		+sizeof(tcc_status)
		+sizeof(tcc_sender)
		+1;
}

// LiRC对下命令操作的返回
IMPLEMENT_CLBTH(MsgStccCommandSendResult, MSGID_MsgStccCommandSendResult)
IMPLEMENT_SERIALIZE_OPERATOR(MsgStccCommandSendResult)
MsgStccCommandSendResult::MsgStccCommandSendResult()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
MsgStccCommandSendResult::~MsgStccCommandSendResult()
{
	if(strComment)
		free(strComment);
}
HRCODE MsgStccCommandSendResult::doSerialize(Archive& ar)
{
	HRCODE hr = MsgStccCommandSend::doSerialize(ar);
	if(hr.failed())
		return hr;

	if(ar.isStoring())
	{
		ar << result << comment_length;
		UINT len = ar.write(strComment, comment_length);
		if(len != comment_length)
			return HRCODE(HRCODE::HC_FAIL);
	}
	else
	{
		ar >> result >> comment_length;
		strComment = (char*)malloc(comment_length + 1);
		memset(strComment, 0, comment_length + 1);
		UINT len = ar.read(strComment, comment_length);
		if(len != comment_length)
			return HRCODE(HRCODE::HC_FAIL);
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}

size_t MsgStccCommandSendResult::countObjectLength()
{
	return MsgStccCommandSend::countObjectLength()
		+sizeof(result)
		+sizeof(comment_length)
		+comment_length;
}

void MsgStccCommandSendResult::SetComment(const char* pComment, INT len)
{
	if(strComment)
		free(strComment);
	strComment = (char*)malloc(len + 1);
	ASSERT(strComment);
	memset(strComment, 0, len + 1);
	strncpy(strComment, pComment, len);
	comment_length = len;
}

void MsgStccCommandSendResult::GetComment(char buf[], INT len)
{
	INT nCopyLen = comment_length;
	if(len < comment_length)
		nCopyLen = len;
	strncpy(buf, strComment, nCopyLen);
}

// 向LiRC申请实时限速列表
IMPLEMENT_CLBTH(MsgStccCommandListQuery, MSGID_MsgStccCommandListQuery)
IMPLEMENT_SERIALIZE_OPERATOR(MsgStccCommandListQuery)
MsgStccCommandListQuery::MsgStccCommandListQuery()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
MsgStccCommandListQuery::~MsgStccCommandListQuery()
{
}
HRCODE MsgStccCommandListQuery::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << tcc_no << inquirer;
	}
	else
	{
		ar >> tcc_no >> inquirer;
	}

	return HRCODE(HRCODE::HC_SUCCESS);
}

size_t MsgStccCommandListQuery::countObjectLength()
{
	return sizeof(tcc_no) + sizeof(inquirer);
}

// LiRC报告的实时限速列表
IMPLEMENT_CLBTH(MsgStccCommandList, MSGID_MsgStccCommandList)
IMPLEMENT_SERIALIZE_OPERATOR(MsgStccCommandList)
MsgStccCommandList::MsgStccCommandList()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
MsgStccCommandList::~MsgStccCommandList()
{
}
HRCODE MsgStccCommandList::doSerialize(Archive& ar)
{
	enum {
		FULL_LIMIT   = 0x01,
		START_NOW    = 0x02,
		END_LESS     = 0x04,
	};

	if(ar.isStoring())
	{
		ar << console_no;
		ar.write(sys_id, sizeof(sys_id));
		ar << cmd_no 
			<< sta_no[0] << sta_no[1] << line_no
			<< km_marker[0] << km_marker[1]
			<< time[0] << time[1]
			<< speed
			<< tcc_no << tcc_line_no << tcc_sender;
		
		IBYTE booltag = 0;
		if(bFullLimit)
			booltag |= FULL_LIMIT;
		if(bStartNow)
			booltag |= START_NOW;
		if(bEndless)
			booltag |= END_LESS;

		ar << booltag;
	}
	else
	{
		ar >> console_no;
		ar.read(sys_id, sizeof(sys_id));
		ar >> cmd_no 
			>> sta_no[0] >> sta_no[1] >> line_no
			>> km_marker[0] >> km_marker[1]
			>> time[0] >> time[1]
			>> speed
			>> tcc_no >> tcc_line_no >> tcc_sender;

		IBYTE booltag;
		ar >> booltag;
		bFullLimit = 0 != (booltag & FULL_LIMIT);
		bStartNow = 0 != (booltag & START_NOW);
		bEndless = 0 != (booltag & END_LESS);
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}

size_t MsgStccCommandList::countObjectLength()
{
	return sizeof(console_no)
		+sizeof(sys_id)
		+sizeof(cmd_no)
		+sizeof(sta_no)
		+sizeof(line_no)
		+sizeof(km_marker)
		+sizeof(time)
		+sizeof(speed)
		+sizeof(tcc_no)
		+sizeof(tcc_line_no)
		+sizeof(tcc_sender)
		+1;
}

// LiRC报告的实时限速列表(含设备元素列表)
IMPLEMENT_CLBTH(MsgStccCommandDetail, MSGID_MsgStccCommandDetail)
IMPLEMENT_SERIALIZE_OPERATOR(MsgStccCommandDetail)
MsgStccCommandDetail::MsgStccCommandDetail()
	: elem_count(0)
{
}
MsgStccCommandDetail::~MsgStccCommandDetail()
{
}
HRCODE MsgStccCommandDetail::doSerialize(Archive& ar)
{
	enum {
		FULL_LIMIT   = 0x01,
		START_NOW    = 0x02,
		END_LESS     = 0x04,
	};

	if(ar.isStoring())
	{
		ar << console_no;
		ar.write(sys_id, sizeof(sys_id));
		ar << cmd_no 
			<< sta_no[0] << sta_no[1] << line_no
			<< km_marker[0] << km_marker[1]
			<< time[0] << time[1]
			<< speed
			<< tcc_no << tcc_line_no << tcc_sender << work_time << line_type;
		
		IBYTE booltag = 0;
		if(bFullLimit)
			booltag |= FULL_LIMIT;
		if(bStartNow)
			booltag |= START_NOW;
		if(bEndless)
			booltag |= END_LESS;
		ar << booltag;

		if ( elem_count > MAX_CMD_ELEM_COUNT )
			return HRCODE(HRCODE::HC_FAIL);
		ar << elem_count;
		for ( int i = 0; i < elem_count; i++ )
		{
			ar << elements[i].sta_no << elements[i].elem_type << elements[i].elem_id;
		}
	}
	else
	{
		ar >> console_no;
		ar.read(sys_id, sizeof(sys_id));
		ar >> cmd_no 
			>> sta_no[0] >> sta_no[1] >> line_no
			>> km_marker[0] >> km_marker[1]
			>> time[0] >> time[1]
			>> speed
			>> tcc_no >> tcc_line_no >> tcc_sender >> work_time >> line_type;

		IBYTE booltag;
		ar >> booltag;
		bFullLimit = 0 != (booltag & FULL_LIMIT);
		bStartNow = 0 != (booltag & START_NOW);
		bEndless = 0 != (booltag & END_LESS);

		ar >> elem_count;
		if ( elem_count > MAX_CMD_ELEM_COUNT )
			return HRCODE(HRCODE::HC_FAIL);
		for ( int i = 0; i < elem_count; i++ )
		{
			ar >> elements[i].sta_no >> elements[i].elem_type >> elements[i].elem_id;
		}
	}
	return HRCODE(HRCODE::HC_SUCCESS);

}
size_t MsgStccCommandDetail::countObjectLength()
{
	return sizeof(console_no)
		+sizeof(sys_id)
		+sizeof(cmd_no)
		+sizeof(sta_no)
		+sizeof(line_no)
		+sizeof(km_marker)
		+sizeof(time)
		+sizeof(speed)
		+sizeof(tcc_no)
		+sizeof(tcc_line_no)
		+sizeof(tcc_sender)
		+sizeof(work_time)
		+sizeof(line_type)
		+1
		+sizeof(elem_count)
		+elem_count * (sizeof(STATION_NO)+sizeof(DEVICE_TYPE)+sizeof(DMIS_ID));
}

//列控中心消息定义结束
//三点检查消息:CTC向TCC发送操作请求消息
IMPLEMENT_CLBTH(MsgTccThreePointCheckRequest, MSGID_MsgTccThreePointCheckRequest)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTccThreePointCheckRequest)
MsgTccThreePointCheckRequest::MsgTccThreePointCheckRequest()
{
	memset( ctc_request, 0, sizeof(ctc_request) );
}
MsgTccThreePointCheckRequest::~MsgTccThreePointCheckRequest()
{
}

HRCODE MsgTccThreePointCheckRequest::doSerialize(Archive& ar)
{
	if( ar.isStoring() )
	{
		ar.write( ctc_request, sizeof(ctc_request) );
		ar<<tcc_data_vebose;
	}
	else
	{
		UINT count = ar.read( ctc_request, sizeof(ctc_request) );
		if( count != sizeof(ctc_request) )
			return HRCODE(HRCODE::HC_FAIL);
		ar>>tcc_data_vebose;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgTccThreePointCheckRequest::countObjectLength()
{
	return sizeof(ctc_request)+sizeof(tcc_data_vebose);
}
//三点检查消息:TCC向CTC发送确认回执消息
IMPLEMENT_CLBTH(MsgTccThreePointCheckConfirm, MSGID_MsgTccThreePointCheckConfirm)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTccThreePointCheckConfirm)
MsgTccThreePointCheckConfirm::MsgTccThreePointCheckConfirm()
{
	memset( tcc_confrim, 0, sizeof(tcc_confrim) );
}
MsgTccThreePointCheckConfirm::~MsgTccThreePointCheckConfirm()
{
}

HRCODE MsgTccThreePointCheckConfirm::doSerialize(Archive& ar)
{
	if( ar.isStoring() )
	{
		ar.write( tcc_confrim, sizeof(tcc_confrim) );
		ar<<tcc_data_vebose;
	}
	else
	{
		UINT count = ar.read( tcc_confrim, sizeof(tcc_confrim) );
		if( count != sizeof(tcc_confrim) )
			return HRCODE(HRCODE::HC_FAIL);
		ar>>tcc_data_vebose;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgTccThreePointCheckConfirm::countObjectLength()
{
	return sizeof(tcc_confrim)+sizeof(tcc_data_vebose);
}

EXTERN Archive& operator<<(Archive& ar, kmmark_t& m)
{
	ar << m.nKmsysCode << m.k_cl << m.m_cl << m.val;
	return ar;
}

EXTERN Archive& operator>>(Archive& ar, kmmark_t& m)
{
	ar >> m.nKmsysCode >> m.k_cl >> m.m_cl >> m.val;
	return ar;
}


// 客专车站列控状态报告
IMPLEMENT_CLBTH(MsgStccReport2, MSGID_MsgStccReport2)
IMPLEMENT_SERIALIZE_OPERATOR(MsgStccReport2)
MsgStccReport2::MsgStccReport2()
{
}
MsgStccReport2::~MsgStccReport2()
{
}

HRCODE MsgStccReport2::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << init_status;	//初始化状态: 未初始化, 初始化进行中, 初始化已完成
        ar << sta_no;      //发送站号
        ar << comm_active;
		ar << nStaNum;
		for(int iSta=0; iSta<nStaNum; iSta++)
		{
			ar << sta[iSta].nStaID << sta[iSta].stby_status 
			<< sta[iSta].master_status
			<< sta[iSta].ilock_channel[0] << sta[iSta].ilock_channel[1]
			<< sta[iSta].ctc_channel[0] << sta[iSta].ctc_channel[1]
			<< sta[iSta].switch_channel[0] << sta[iSta].switch_channel[1];
			int i;
			for(i=0; i<16; i++)
			{
				IBYTE b = sta[iSta].leu[i].bValid ? 1 : 0;
				ar << b 
					<< sta[iSta].leu[i].leu_channel[0]
					<< sta[iSta].leu[i].leu_channel[1]
					<< sta[iSta].leu[i].leu_ans[0]
					<< sta[iSta].leu[i].leu_ans[1]
					<< sta[iSta].leu[i].leu_ans[2]
					<< sta[iSta].leu[i].leu_ans[3];
			}
			ar << sta[iSta].nZCNum;
			if(sta[iSta].nZCNum > 8)
				return HRCODE(HRCODE::HC_FAIL);
			
			for(i=0; i<sta[iSta].nZCNum; i++)
			{
				ar << sta[iSta].ZC[i].nStaID
					<< sta[iSta].ZC[i].channel[0]
					<< sta[iSta].ZC[i].channel[1];
			}
		}
	}
	else
	{
		ar >> init_status;	//初始化状态: 未初始化, 初始化进行中, 初始化已完成
        ar >> sta_no;      //发送站号
        ar >> comm_active;
		ar >> nStaNum;
		if(nStaNum > 32)
			return HRCODE(HRCODE::HC_FAIL);

		for(int iSta=0; iSta<nStaNum; iSta++)
		{
			ar >> sta[iSta].nStaID >> sta[iSta].stby_status 
			>> sta[iSta].master_status
			>> sta[iSta].ilock_channel[0] >> sta[iSta].ilock_channel[1]
			>> sta[iSta].ctc_channel[0] >> sta[iSta].ctc_channel[1]
			>> sta[iSta].switch_channel[0] >> sta[iSta].switch_channel[1];

			int i;
			for(i=0; i<16; i++)
			{
				IBYTE b;
				ar >> b;
				sta[iSta].leu[i].bValid = b==1 ? ITRUE : IFALSE;
				ar >> sta[iSta].leu[i].leu_channel[0]
					>> sta[iSta].leu[i].leu_channel[1]
					>> sta[iSta].leu[i].leu_ans[0]
					>> sta[iSta].leu[i].leu_ans[1]
					>> sta[iSta].leu[i].leu_ans[2]
					>> sta[iSta].leu[i].leu_ans[3];
			}
			ar >> sta[iSta].nZCNum;
			if(sta[iSta].nZCNum > 8)
				return HRCODE(HRCODE::HC_FAIL);
			for(i=0; i<sta[iSta].nZCNum; i++)
			{
				ar >> sta[iSta].ZC[i].nStaID
					>> sta[iSta].ZC[i].channel[0]
					>> sta[iSta].ZC[i].channel[1];
			}
		}
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgStccReport2::countObjectLength()
{
	return sizeof(nStaNum)+nStaNum*sizeof(sta[0]);
}




// 客专 LiRC报告的实时限速列表
IMPLEMENT_CLBTH(MsgStccCommandList2, MSGID_MsgStccCommandList2)
IMPLEMENT_SERIALIZE_OPERATOR(MsgStccCommandList2)
MsgStccCommandList2::MsgStccCommandList2()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
MsgStccCommandList2::~MsgStccCommandList2()
{
}
HRCODE MsgStccCommandList2::doSerialize(Archive& ar)
{
	enum {
		FULL_LIMIT   = 0x01,
		START_NOW    = 0x02,
		END_LESS     = 0x04,
	};

	if(ar.isStoring())
	{
		ar << console_no;
		ar.write(sys_id, sizeof(sys_id));
		ar << cmd_no 
			<< sta_no[0] << sta_no[1] << line_no
			<< km_marker[0] << km_marker[1]
			<< time[0] << time[1]
			<< speed
			<< set_time
			<< tcc_no << tcc_line_no << tcc_sender
			<< tcc_km_marker[0] << tcc_km_marker[1];
		
		IBYTE booltag = 0;
		if(bFullLimit)
			booltag |= FULL_LIMIT;
		if(bStartNow)
			booltag |= START_NOW;
		if(bEndless)
			booltag |= END_LESS;

		ar << booltag;
	}
	else
	{
		ar >> console_no;
		ar.read(sys_id, sizeof(sys_id));
		ar >> cmd_no 
			>> sta_no[0] >> sta_no[1] >> line_no
			>> km_marker[0] >> km_marker[1]
			>> time[0] >> time[1]
			>> speed
			>> set_time
			>> tcc_no >> tcc_line_no >> tcc_sender
			>> tcc_km_marker[0] >> tcc_km_marker[1];

		IBYTE booltag;
		ar >> booltag;
		bFullLimit = 0 != (booltag & FULL_LIMIT);
		bStartNow = 0 != (booltag & START_NOW);
		bEndless = 0 != (booltag & END_LESS);
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}

size_t MsgStccCommandList2::countObjectLength()
{
	return sizeof(console_no)
		+sizeof(sys_id)
		+sizeof(cmd_no)
		+sizeof(sta_no)
		+sizeof(line_no)
		+sizeof(km_marker)
		+sizeof(time)
		+sizeof(speed)
		+sizeof(tcc_no)
		+sizeof(tcc_line_no)
		+sizeof(tcc_sender)
		+sizeof(tcc_km_marker)
		+1;
}


// 客专 临时限速命令下达
IMPLEMENT_CLBTH(MsgStccCommandSend2, MSGID_MsgStccCommandSend2)
IMPLEMENT_SERIALIZE_OPERATOR(MsgStccCommandSend2)
MsgStccCommandSend2::MsgStccCommandSend2()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
MsgStccCommandSend2::~MsgStccCommandSend2()
{
}
HRCODE MsgStccCommandSend2::doSerialize(Archive& ar)
{
	enum {
		FULL_LIMIT   = 0x01,
		START_NOW    = 0x02,
		END_LESS     = 0x04,
		CANCEL_LIMIT = 0x08,
		INIT_OP		 = 0x10,
	};

	if(ar.isStoring())
	{
		ar << console_no;
		ar.write(sys_id, sizeof(sys_id));
		ar << cmd_no 
			<< sta_no[0] << sta_no[1] << line_no
			<< km_marker[0] << km_marker[1]
			<< time[0] << time[1]
			<< speed
			<< tcc_no
			<< tcc_line_no
			<< tcc_sender
			<< section_dir
			<< tcc_km_marker[0] << tcc_km_marker[1]
			<< op;

		IBYTE booltag = 0;
		if(bFullLimit)
			booltag |= FULL_LIMIT;
		if(bStartNow)
			booltag |= START_NOW;
		if(bEndless)
			booltag |= END_LESS;
		if(bCancelLimit)
			booltag |= CANCEL_LIMIT;
		if(bInitOpration)
			booltag |= INIT_OP;

		ar << booltag;
	}
	else
	{
		ar >> console_no;
		ar.read(sys_id, sizeof(sys_id));
		ar >> cmd_no 
			>> sta_no[0] >> sta_no[1] >> line_no
			>> km_marker[0] >> km_marker[1]
			>> time[0] >> time[1]
			>> speed
			>> tcc_no
			>> tcc_line_no
			>> tcc_sender
			>> section_dir
			>> tcc_km_marker[0] >> tcc_km_marker[1]
			>> op;

		IBYTE booltag;
		ar >> booltag;
		bFullLimit = 0 != (booltag & FULL_LIMIT);
		bStartNow = 0 != (booltag & START_NOW);
		bEndless = 0 != (booltag & END_LESS);
		bCancelLimit = 0 != (booltag & CANCEL_LIMIT);
		bInitOpration = 0 != (booltag & INIT_OP);
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}

size_t MsgStccCommandSend2::countObjectLength()
{
	return sizeof(console_no)
		+sizeof(sys_id)
		+sizeof(cmd_no)
		+sizeof(sta_no)
		+sizeof(line_no)
		+sizeof(km_marker)
		+sizeof(time)
		+sizeof(speed)
		+sizeof(tcc_no)
		+sizeof(tcc_line_no)
		+sizeof(tcc_sender)
		+sizeof(section_dir)
		+sizeof(tcc_km_marker)
		+sizeof(op)
		+1;
}

// 列控限速初始化完成
IMPLEMENT_CLBTH(MsgStccInitComplete, MSGID_MsgStccInitComplete)
IMPLEMENT_SERIALIZE_OPERATOR(MsgStccInitComplete)
MsgStccInitComplete::MsgStccInitComplete()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
MsgStccInitComplete::~MsgStccInitComplete()
{
}
HRCODE MsgStccInitComplete::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << console_no;
		ar.write(sys_id, sizeof(sys_id));
		ar << cmd_no 
			<< sta_no[0] << sta_no[1] << line_no
			<< km_marker[0] << km_marker[1]
			<< tcc_no
			<< tcc_line_no
			<< tcc_sender
			<< tcc_km_marker[0] << tcc_km_marker[1];
	}
	else
	{
		ar >> console_no;
		ar.read(sys_id, sizeof(sys_id));
		ar >> cmd_no 
			>> sta_no[0] >> sta_no[1] >> line_no
			>> km_marker[0] >> km_marker[1]
			>> tcc_no
			>> tcc_line_no
			>> tcc_sender
			>> tcc_km_marker[0] >> tcc_km_marker[1];
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}

size_t MsgStccInitComplete::countObjectLength()
{
	return sizeof(console_no)
		+sizeof(sys_id)
		+sizeof(cmd_no)
		+sizeof(sta_no)
		+sizeof(line_no)
		+sizeof(km_marker)
		+sizeof(tcc_no)
		+sizeof(tcc_line_no)
		+sizeof(tcc_sender)
		+sizeof(tcc_km_marker);
}


// 服务器发向STPC, GPC的确认列表
IMPLEMENT_CLBTH(MsgTccCommand2, MSGID_MsgTccCommand2)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTccCommand2)
MsgTccCommand2::MsgTccCommand2()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
MsgTccCommand2::~MsgTccCommand2()
{
}
HRCODE MsgTccCommand2::doSerialize(Archive& ar)
{
	enum {
		FULL_LIMIT   = 0x01,
		START_NOW    = 0x02,
		END_LESS     = 0x04,
		CANCEL_LIMIT = 0x08,
	};

	if(ar.isStoring())
	{
		ar << console_no;
		ar.write(dcmd_no, sizeof(dcmd_no));
		ar << dcmd_entity << dcmd_time;
		ar.write(sys_id, sizeof(sys_id));
		ar.write(raw_sysid, sizeof(raw_sysid));
		ar << cmd_no 
			<< sta_no[0] << sta_no[1]
			<< line_no
			<< km_marker[0] << km_marker[1]
			<< time[0] << time[1]
			<< speed;

		ar << creator << checkor << confirmor;

		IBYTE booltag = 0;
		if(bFullLimit)
			booltag |= FULL_LIMIT;
		if(bStartNow)
			booltag |= START_NOW;
		if(bEndless)
			booltag |= END_LESS;
		if(bCancelLimit)
			booltag |= CANCEL_LIMIT;

		ar << booltag;

		ar << alarm_num;
		if (alarm_num > sizeof(alarm)/sizeof(alarm[0]))
			return HRCODE(HRCODE::HC_FAIL);		
		
		int i;
		for(i=0; i<alarm_num; i++)
			ar << alarm[i];
		
		ar << tcc_num;
		if(tcc_num > sizeof(tcc_no)/sizeof(tcc_no[0]))
			return HRCODE(HRCODE::HC_FAIL);

		for(i=0; i<tcc_num; i++)
		{
			ar << tcc_no[i]
				<< tcc_line_no[i]
				<< tcc_status[i]
				<< tcc_time[i]
				<< tcc_checkor[i]
				<< tcc_sender[i]
				<< tcc_km_marker[i][0] << tcc_km_marker[i][1]
				<< tcc_alarm_num[i];

			if (tcc_alarm_num[i] > sizeof(tcc_alarm[i])/sizeof(tcc_alarm[i][0]))
				return HRCODE(HRCODE::HC_FAIL);

			for(int j=0; j<tcc_alarm_num[i]; j++)
				ar << tcc_alarm[i][j];
		}

		ar << nMarkIndex << create_time;
	}
	else
	{
		ar >> console_no;
		ar.read(dcmd_no, sizeof(dcmd_no));
		ar >> dcmd_entity >> dcmd_time;
		ar.read(sys_id, sizeof(sys_id));
		ar.read(raw_sysid, sizeof(raw_sysid));
		ar >> cmd_no 
			>> sta_no[0] >> sta_no[1]
			>> line_no
			>> km_marker[0] >> km_marker[1]
			>> time[0] >> time[1]
			>> speed;

		ar >> creator >> checkor >> confirmor;

		IBYTE booltag;
		ar >> booltag;
		bFullLimit = 0 != (booltag & FULL_LIMIT);
		bStartNow = 0 != (booltag & START_NOW);
		bEndless = 0 != (booltag & END_LESS);
		bCancelLimit = 0 != (booltag & CANCEL_LIMIT);

		ar >> alarm_num;
		if (alarm_num > sizeof(alarm)/sizeof(alarm[0]))
			return HRCODE(HRCODE::HC_FAIL);		

		int i;
		for(i=0; i<alarm_num; i++)
			ar >> alarm[i];

		ar >> tcc_num;
		if(tcc_num > sizeof(tcc_no)/sizeof(tcc_no[0]))
			return HRCODE(HRCODE::HC_FAIL);

		for(i=0; i<tcc_num; i++)
		{
			ar >> tcc_no[i]
				>> tcc_line_no[i]
				>> tcc_status[i]
				>> tcc_time[i]
				>> tcc_checkor[i]
				>> tcc_sender[i]
				>> tcc_km_marker[i][0] >> tcc_km_marker[i][1]
				>> tcc_alarm_num[i];

				if (tcc_alarm_num[i] > sizeof(tcc_alarm[i])/sizeof(tcc_alarm[i][0]))
					return HRCODE(HRCODE::HC_FAIL);

				for(int j=0; j<tcc_alarm_num[i]; j++)
					ar >> tcc_alarm[i][j];

		}

		ar >> nMarkIndex >> create_time;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}

size_t MsgTccCommand2::countObjectLength()
{
	size_t t=sizeof(console_no)+sizeof(dcmd_no)
		+sizeof(dcmd_entity)+sizeof(dcmd_time)
		+sizeof(sys_id) + sizeof(raw_sysid)
		+sizeof(cmd_no)
		+sizeof(sta_no)
		+sizeof(line_no)
		+sizeof(km_marker)
		+sizeof(time)
		+sizeof(speed)
		+sizeof(creator)
		+sizeof(checkor)
		+sizeof(confirmor)
		+1
		+sizeof(alarm_num)
		+sizeof(alarm[0])*alarm_num
		+sizeof(tcc_num)
		+sizeof(tcc_no[0])*tcc_num
		+sizeof(tcc_line_no[0])*tcc_num
		+sizeof(tcc_status[0])*tcc_num
		+sizeof(tcc_time[0])*tcc_num
		+sizeof(tcc_checkor[0])*tcc_num
		+sizeof(tcc_sender[0])*tcc_num
		+sizeof(tcc_km_marker[0][0].nKmsysCode)*tcc_num*2
		+sizeof(tcc_km_marker[0][0].k_cl)*tcc_num*2
		+sizeof(tcc_km_marker[0][0].m_cl)*tcc_num*2
		+sizeof(tcc_km_marker[0][0].val)*tcc_num*2
		+sizeof(tcc_alarm_num[0])*tcc_num
		+sizeof(nMarkIndex)+sizeof(create_time);

	for(int i=0; i<tcc_num; i++)
	{
		t+=sizeof(tcc_alarm[i][0])*tcc_alarm_num[i];
	}

	return t;
}

IMPLEMENT_CLBTH(MsgTccCommandAdd2, MSGID_MsgTccCommandAdd2)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTccCommandAdd2)
MsgTccCommandAdd2::MsgTccCommandAdd2()
{
}
MsgTccCommandAdd2::~MsgTccCommandAdd2()
{
}

IMPLEMENT_CLBTH(MsgTccCommandData2, MSGID_MsgTccCommandData2)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTccCommandData2)
MsgTccCommandData2::MsgTccCommandData2()
{
}
MsgTccCommandData2::~MsgTccCommandData2()
{
}
HRCODE MsgTccCommandData2::doSerialize(Archive& ar)
{
	if (HRCODE(HRCODE::HC_SUCCESS) != MsgTccCommand2::doSerialize(ar))
		return HRCODE(HRCODE::HC_FAIL);

	if(ar.isStoring())
	{
		ar << (short)type;
	}
	else
	{
		short t;
		ar >> t;
		type = (MsgTccCommandData2::QUERY_TYPE) t;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}

size_t MsgTccCommandData2::countObjectLength()
{
	return MsgTccCommand2::countObjectLength()+sizeof(short);
}

IMPLEMENT_CLBTH(MsgTccCommandQuery2, MSGID_MsgTccCommandQuery2)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTccCommandQuery2)
MsgTccCommandQuery2::MsgTccCommandQuery2()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
MsgTccCommandQuery2::~MsgTccCommandQuery2()
{
}
HRCODE MsgTccCommandQuery2::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << console_no << tcc_no;
		ar.write(dcmd_no, sizeof(dcmd_no));
		ar << time_scope[0] << time_scope[1] << (short)flag << (short)type;
	}
	else
	{
		ar >> console_no >> tcc_no;
		ar.read(dcmd_no, sizeof(dcmd_no));
		ar >> time_scope[0] >> time_scope[1];

		short t;
		ar >> t;
		flag = (MsgTccCommandQuery2::QUERY_MODE) t;
		ar >> t;
		type = (MsgTccCommandQuery2::QUERY_TYPE) t;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}

size_t MsgTccCommandQuery2::countObjectLength()
{
	return sizeof(console_no)+sizeof(tcc_no)+sizeof(dcmd_no)+sizeof(time_scope)+sizeof(short)*2;
}

IMPLEMENT_CLBTH(MsgTccCommandSet, MSGID_MsgTccCommandSet)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTccCommandSet)
MsgTccCommandSet::MsgTccCommandSet()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
MsgTccCommandSet::~MsgTccCommandSet()
{
}
HRCODE MsgTccCommandSet::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << console_no << cmd_no << tcc_no << tcc_status << tcc_time << tcc_checkor << tcc_sender;
		ar.write(sys_id, sizeof(sys_id));
	}
	else
	{
		ar >> console_no >> cmd_no >> tcc_no >> tcc_status >> tcc_time >> tcc_checkor >> tcc_sender;
		ar.read(sys_id, sizeof(sys_id));
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}

size_t MsgTccCommandSet::countObjectLength()
{
	return sizeof(console_no)+sizeof(cmd_no)+sizeof(tcc_no)+sizeof(sys_id)
		+sizeof(tcc_status)+sizeof(tcc_time)+sizeof(tcc_checkor)+sizeof(tcc_sender);
}

IMPLEMENT_CLBTH(MsgTccCommandOperEcho, MSGID_MsgTccCommandOperEcho)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTccCommandOperEcho)
MsgTccCommandOperEcho::MsgTccCommandOperEcho()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
MsgTccCommandOperEcho::~MsgTccCommandOperEcho()
{
}
HRCODE MsgTccCommandOperEcho::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << console_no << cmd_no;
		ar.write(sys_id, sizeof(sys_id));
		ar << code << (short)oper << para;
	}
	else
	{
		ar >> console_no >> cmd_no;
		ar.read(sys_id, sizeof(sys_id));

		short t;
		ar >> code >> t >> para;
		oper = TccOper(t);
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}

size_t MsgTccCommandOperEcho::countObjectLength()
{
	return sizeof(console_no)+sizeof(cmd_no)+sizeof(sys_id)+sizeof(code)+sizeof(short)+sizeof(para);
}

// LiRC对下命令操作的返回
IMPLEMENT_CLBTH(MsgStccCommandSendResult2, MSGID_MsgStccCommandSendResult2)
IMPLEMENT_SERIALIZE_OPERATOR(MsgStccCommandSendResult2)
MsgStccCommandSendResult2::MsgStccCommandSendResult2()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
MsgStccCommandSendResult2::~MsgStccCommandSendResult2()
{
	if(strComment)
		free(strComment);
}
HRCODE MsgStccCommandSendResult2::doSerialize(Archive& ar)
{
	HRCODE hr = MsgStccCommandSend2::doSerialize(ar);
	if(hr.failed())
		return hr;

	if(ar.isStoring())
	{
		ar << result << comment_length;
		UINT len = ar.write(strComment, comment_length);
		if(len != comment_length)
			return HRCODE(HRCODE::HC_FAIL);
	}
	else
	{
		ar >> result >> comment_length;
		strComment = (char*)malloc(comment_length + 1);
		memset(strComment, 0, comment_length + 1);
		UINT len = ar.read(strComment, comment_length);
		if(len != comment_length)
			return HRCODE(HRCODE::HC_FAIL);
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}

size_t MsgStccCommandSendResult2::countObjectLength()
{
	return MsgStccCommandSend2::countObjectLength()
		+sizeof(result)
		+sizeof(comment_length)
		+comment_length;
}

void MsgStccCommandSendResult2::SetComment(const char* pComment, INT len)
{
	if(strComment)
		free(strComment);
	strComment = (char*)malloc(len + 1);
	ASSERT(strComment);
	memset(strComment, 0, len + 1);
	strncpy(strComment, pComment, len);
	comment_length = len;
}

void MsgStccCommandSendResult2::GetComment(char buf[], INT len)
{
	INT nCopyLen = comment_length;
	if(len < comment_length)
		nCopyLen = len;
	strncpy(buf, strComment, nCopyLen);
}

// LiRC报告的实时限速列表(含设备元素列表)
IMPLEMENT_CLBTH(MsgStccCommandDetail2, MSGID_MsgStccCommandDetail2)
IMPLEMENT_SERIALIZE_OPERATOR(MsgStccCommandDetail2)
MsgStccCommandDetail2::MsgStccCommandDetail2()
	: elem_count(0)
{
}
MsgStccCommandDetail2::~MsgStccCommandDetail2()
{
}
HRCODE MsgStccCommandDetail2::doSerialize(Archive& ar)
{
	enum {
		FULL_LIMIT   = 0x01,
		START_NOW    = 0x02,
		END_LESS     = 0x04,
	};

	if(ar.isStoring())
	{
		ar << console_no;
		ar.write(sys_id, sizeof(sys_id));
		ar << cmd_no 
			<< sta_no[0] << sta_no[1] << line_no
			<< km_marker[0] << km_marker[1]
			<< time[0] << time[1]
			<< speed
			<< tcc_no << tcc_line_no << tcc_sender << work_time << line_type;
		
		IBYTE booltag = 0;
		if(bFullLimit)
			booltag |= FULL_LIMIT;
		if(bStartNow)
			booltag |= START_NOW;
		if(bEndless)
			booltag |= END_LESS;
		ar << booltag;

		if ( elem_count > MAX_CMD_ELEM_COUNT )
			return HRCODE(HRCODE::HC_FAIL);
		ar << elem_count;
		for ( int i = 0; i < elem_count; i++ )
		{
			ar << elements[i].sta_no << elements[i].elem_type << elements[i].elem_id;
		}
	}
	else
	{
		ar >> console_no;
		ar.read(sys_id, sizeof(sys_id));
		ar >> cmd_no 
			>> sta_no[0] >> sta_no[1] >> line_no
			>> km_marker[0] >> km_marker[1]
			>> time[0] >> time[1]
			>> speed
			>> tcc_no >> tcc_line_no >> tcc_sender >> work_time >> line_type;

		IBYTE booltag;
		ar >> booltag;
		bFullLimit = 0 != (booltag & FULL_LIMIT);
		bStartNow = 0 != (booltag & START_NOW);
		bEndless = 0 != (booltag & END_LESS);

		ar >> elem_count;
		if ( elem_count > MAX_CMD_ELEM_COUNT )
			return HRCODE(HRCODE::HC_FAIL);
		for ( int i = 0; i < elem_count; i++ )
		{
			ar >> elements[i].sta_no >> elements[i].elem_type >> elements[i].elem_id;
		}
	}
	return HRCODE(HRCODE::HC_SUCCESS);

}
size_t MsgStccCommandDetail2::countObjectLength()
{
	return sizeof(console_no)
		+sizeof(sys_id)
		+sizeof(cmd_no)
		+sizeof(sta_no)
		+sizeof(line_no)
		+sizeof(km_marker)
		+sizeof(time)
		+sizeof(speed)
		+sizeof(tcc_no)
		+sizeof(tcc_line_no)
		+sizeof(tcc_sender)
		+sizeof(work_time)
		+sizeof(line_type)
		+1
		+sizeof(elem_count)
		+elem_count * (sizeof(STATION_NO)+sizeof(DEVICE_TYPE)+sizeof(DMIS_ID));
}




// 限速命令号 申请
IMPLEMENT_CLBTH(MsgStccCommandIDRequest, MSGID_MsgStccCommandIDRequest)
IMPLEMENT_SERIALIZE_OPERATOR(MsgStccCommandIDRequest)
MsgStccCommandIDRequest::MsgStccCommandIDRequest()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
MsgStccCommandIDRequest::~MsgStccCommandIDRequest()
{
}
HRCODE MsgStccCommandIDRequest::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << oper_mode << console_no << cmd_no << min_cmd_no << max_cmd_no << dwCookie;
		
		if(GET_CMDID != oper_mode && SET_CMDID != oper_mode)
			return HRCODE(HRCODE::HC_FAIL);

		if(SET_CMDID == oper_mode)
			ar << nStatus;
	}
	else
	{
		ar >> oper_mode >> console_no >> cmd_no >> min_cmd_no >> max_cmd_no >> dwCookie;
		
		if(GET_CMDID != oper_mode && SET_CMDID != oper_mode)
			return HRCODE(HRCODE::HC_FAIL);

		if(SET_CMDID == oper_mode)
			ar >> nStatus;
	}
	return HRCODE(HRCODE::HC_SUCCESS);

}
size_t MsgStccCommandIDRequest::countObjectLength()
{
	size_t n = sizeof(oper_mode) +sizeof(console_no)
		+sizeof(cmd_no)+sizeof(min_cmd_no)+sizeof(max_cmd_no);
	if(SET_CMDID == oper_mode)
		n += sizeof(nStatus);

	return n;
}


//-----------------------------------------------------------------------------
//
//TSR 列控消息
//
//-----------------------------------------------------------------------------

EXTERN Archive& operator<<(Archive& ar, tsr_cmd_t& m)
{
	ar << m.reason << m.sta_no << m.line_no << m.km_marker[0] << m.km_marker[1]
		<< m.bStartNow << m.bEndless << m.time[0] << m.time[1] << m.speed;
	ar.write( m.cmd_no, sizeof(m.cmd_no) );
	return ar;
}

EXTERN Archive& operator>>(Archive& ar, tsr_cmd_t& m)
{
	ar >> m.reason >> m.sta_no >> m.line_no >> m.km_marker[0] >> m.km_marker[1]
		>> m.bStartNow >> m.bEndless >> m.time[0] >> m.time[1] >> m.speed;
	ar.read( m.cmd_no, sizeof(m.cmd_no) );
	m.cmd_no[sizeof(m.cmd_no)-1] = '\0';
	return ar;
}

size_t tsr_cmd_t::countObjectLength()
{
	return sizeof(reason) + sizeof(cmd_no) + sizeof(sta_no) + sizeof(line_no)
		+ sizeof(kmmark_t)*2 + sizeof(bStartNow) + sizeof(bEndless) + sizeof(I_TIME)*2 + sizeof(speed);
}

// TSR限速命令发送
IMPLEMENT_CLBTH(MsgTSRCmdSend, MSGID_MsgTSRCmdSend)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTSRCmdSend)
MsgTSRCmdSend::MsgTSRCmdSend()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
MsgTSRCmdSend::~MsgTSRCmdSend()
{
}
HRCODE MsgTSRCmdSend::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar.write(szGUID, sizeof(szGUID));
		ar.write(dcmd_no, sizeof(dcmd_no));

		if(m_nContentLen > sizeof(m_cContent))
			m_nContentLen = sizeof(m_cContent);
		ar << m_nContentLen;

		ar.write(m_cContent, m_nContentLen);
	}
	else
	{
		ar.read(szGUID, sizeof(szGUID));
		ar.read(dcmd_no, sizeof(dcmd_no));

		ar >> m_nContentLen;
		if(m_nContentLen > sizeof(m_cContent))
			return HRCODE(HRCODE::HC_FAIL);

		ar.read(m_cContent, m_nContentLen);
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgTSRCmdSend::countObjectLength()
{
	if(m_nContentLen > sizeof(m_cContent))
		m_nContentLen = sizeof(m_cContent);

	return sizeof(szGUID) + sizeof(dcmd_no)
		+ sizeof(m_nContentLen) + m_nContentLen;
}

// TSR限速命令发送状态
IMPLEMENT_CLBTH(MsgTSRCmdSendResult, MSGID_MsgTSRCmdSendResult)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTSRCmdSendResult)
MsgTSRCmdSendResult::MsgTSRCmdSendResult()
{
}
MsgTSRCmdSendResult::~MsgTSRCmdSendResult()
{
}
HRCODE MsgTSRCmdSendResult::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar.write(szGUID, sizeof(szGUID));
		strResult[sizeof(strResult)-1] = '\0';
		IBYTE nLen = strlen(strResult);
		ar << sTsrCmd << nResult << nLen;
		ar.write(strResult, nLen);
	}
	else
	{
		ar.read(szGUID, sizeof(szGUID));
		IBYTE nLen = 0;
		ar >> sTsrCmd >> nResult >> nLen;
		if ( nLen >= sizeof(strResult) )
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(strResult, nLen);
		strResult[nLen] = '\0';
	}
	return HRCODE(HRCODE::HC_SUCCESS);

}
size_t MsgTSRCmdSendResult::countObjectLength()
{
	return sTsrCmd.countObjectLength() + sizeof(nResult) + sizeof(IBYTE) + strlen(strResult) + sizeof(szGUID);
}

// 限速激活、设置、撤销、初始化
IMPLEMENT_CLBTH(MsgTSRCmdOperation, MSGID_MsgTSRCmdOperation)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTSRCmdOperation)
MsgTSRCmdOperation::MsgTSRCmdOperation()
{
}
MsgTSRCmdOperation::~MsgTSRCmdOperation()
{
}
HRCODE MsgTSRCmdOperation::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << sTsrCmd << nConID << nOper;
	}
	else
	{
		ar >> sTsrCmd >> nConID >> nOper;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgTSRCmdOperation::countObjectLength()
{
	return sTsrCmd.countObjectLength() + sizeof(nOper) + sizeof(nConID);
}

// 限速操作反馈
IMPLEMENT_CLBTH(MsgTSRCmdOperResult, MSGID_MsgTSRCmdOperResult)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTSRCmdOperResult)
MsgTSRCmdOperResult::MsgTSRCmdOperResult()
{
}
MsgTSRCmdOperResult::~MsgTSRCmdOperResult()
{
}
HRCODE MsgTSRCmdOperResult::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		strResult[sizeof(strResult)-1] = '\0';
		IBYTE nLen = strlen(strResult);
		ar << sTsrCmd << nConID << nOper << nResult << nLen;
		ar.write(strResult, nLen);
	}
	else
	{
		IBYTE nLen = 0;
		ar >> sTsrCmd >> nConID >> nOper >> nResult >> nLen;
		if ( nLen >= sizeof(strResult) )
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(strResult, nLen);
		strResult[nLen] = '\0';
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgTSRCmdOperResult::countObjectLength()
{
	return sTsrCmd.countObjectLength() + sizeof(nConID) + sizeof(nOper) + sizeof(nResult) + sizeof(IBYTE) + strlen(strResult);
}

// 限速提示
IMPLEMENT_CLBTH(MsgTSRCmdAlarm, MSGID_MsgTSRCmdAlarm)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTSRCmdAlarm)
MsgTSRCmdAlarm::MsgTSRCmdAlarm()
{
}
MsgTSRCmdAlarm::~MsgTSRCmdAlarm()
{
}
HRCODE MsgTSRCmdAlarm::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << sTsrCmd << nConID << nAlarmLevel << nAlarmType << nValidTime;
	}
	else
	{
		ar >> sTsrCmd >> nConID >> nAlarmLevel >> nAlarmType >> nValidTime;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgTSRCmdAlarm::countObjectLength()
{
	return sTsrCmd.countObjectLength() + sizeof(nConID) + sizeof(nAlarmLevel) + sizeof(nAlarmType) + sizeof(nValidTime);
}

// TSR限速命令实时状态
IMPLEMENT_CLBTH(MsgTSRCmdStatus, MSGID_MsgTSRCmdStatus)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTSRCmdStatus)
MsgTSRCmdStatus::MsgTSRCmdStatus()
{
}
MsgTSRCmdStatus::~MsgTSRCmdStatus()
{
}
HRCODE MsgTSRCmdStatus::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << sTsrCmd << nConID << nStatus << nDetailStatus;
	}
	else
	{
		ar >> sTsrCmd >> nConID >> nStatus >> nDetailStatus;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgTSRCmdStatus::countObjectLength()
{
	return sTsrCmd.countObjectLength() + sizeof(nConID) + sizeof(nStatus) + sizeof(nDetailStatus);
}

// TSR限速命令查询
IMPLEMENT_CLBTH(MsgTSRCmdQuery, MSGID_MsgTSRCmdQuery)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTSRCmdQuery)
MsgTSRCmdQuery::MsgTSRCmdQuery()
	:	nStaCount(0)
{
}
MsgTSRCmdQuery::~MsgTSRCmdQuery()
{
}
HRCODE MsgTSRCmdQuery::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nQueryType << nConID << nStaCount;
		if ( nStaCount > sizeof(nzStations)/sizeof(STATION_NO) )
			return HRCODE(HRCODE::HC_FAIL);
		for ( int i = 0; i < nStaCount; i++ )
			ar << nzStations[i];
	}
	else
	{
		ar >> nQueryType >> nConID >> nStaCount;
		if ( nStaCount > sizeof(nzStations)/sizeof(STATION_NO) )
			return HRCODE(HRCODE::HC_FAIL);
		for ( int i = 0; i < nStaCount; i++ )
			ar >> nzStations[i];
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgTSRCmdQuery::countObjectLength()
{
	return sizeof(nQueryType) + sizeof(nConID) + sizeof(nStaCount) + nStaCount*sizeof(STATION_NO);
}

// TSRS状态查询
IMPLEMENT_CLBTH(MsgTSRServerRequest, MSGID_MsgTSRServerRequest)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTSRServerRequest)
MsgTSRServerRequest::MsgTSRServerRequest()
	: nConCount(0)
{
}
MsgTSRServerRequest::~MsgTSRServerRequest()
{
}
HRCODE MsgTSRServerRequest::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		if ( nConCount > MAX_REQ_CON_COUNT )
			return HRCODE(HRCODE::HC_FAIL);
		ar << nConCount;
		for ( int i = 0; i < nConCount; i++ )
			ar << nzConIDs[i];
	}
	else
	{
		ar >> nConCount;
		if ( nConCount > MAX_REQ_CON_COUNT )
			return HRCODE(HRCODE::HC_FAIL);
		for ( int i = 0; i < nConCount; i++ )
			ar >> nzConIDs[i];
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgTSRServerRequest::countObjectLength()
{
	return sizeof(nConCount) + nConCount*sizeof(CONID);
}

// TSRS状态报告
IMPLEMENT_CLBTH(MsgTSRServerStatus, MSGID_MsgTSRServerStatus)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTSRServerStatus)
MsgTSRServerStatus::MsgTSRServerStatus()
{
}
MsgTSRServerStatus::~MsgTSRServerStatus()
{
}
HRCODE MsgTSRServerStatus::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nTsrsID << nConID << nStatus << nTimeFlag;
	}
	else
	{
		ar >> nTsrsID >> nConID >> nStatus >> nTimeFlag;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}

// TSR限速命令个数
IMPLEMENT_CLBTH(MsgTSRCmdCount, MSGID_MsgTSRCmdCount)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTSRCmdCount)
MsgTSRCmdCount::MsgTSRCmdCount()
	: nCheckCount(0), nActiveCount(0), nSetCount(0), nCancelCount(0) 
{
}
MsgTSRCmdCount::~MsgTSRCmdCount()
{
}
HRCODE MsgTSRCmdCount::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nConID << nCheckCount << nActiveCount << nSetCount << nCancelCount << nReplyStatus;
	}
	else
	{
		ar >> nConID >> nCheckCount >> nActiveCount >> nSetCount >> nCancelCount >> nReplyStatus;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}

// TSR限速命令实时状态表示
IMPLEMENT_CLBTH(MsgTSRCmdDisplay, MSGID_MsgTSRCmdDisplay)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTSRCmdDisplay)
MsgTSRCmdDisplay::MsgTSRCmdDisplay()
	:	nElemCount(0), nRemoveFlag(0), MsgTSRCmdStatus()
{
	szComment[0] = '\0';
}
MsgTSRCmdDisplay::~MsgTSRCmdDisplay()
{
}
HRCODE MsgTSRCmdDisplay::doSerialize(Archive& ar)
{
	MsgTSRCmdStatus::doSerialize(ar);

	if(ar.isStoring())
	{
		if (nElemCount > MAX_ELEM_COUNT)
			return HRCODE(HRCODE::HC_FAIL);
		ar << nElemCount << nRemoveFlag;
		for ( int i = 0; i < nElemCount; i++ )
			ar << szElems[i].nStation << szElems[i].nDmisType << szElems[i].nDmisID;
		szComment[sizeof(szComment)-1] = '\0';
		short len = strlen(szComment);
		ar << len;
		if ( len > 0 )
			ar.write(szComment, len);
	}
	else
	{
		ar >> nElemCount >> nRemoveFlag;
		if (nElemCount > MAX_ELEM_COUNT)
			return HRCODE(HRCODE::HC_FAIL);
		for ( int i = 0; i < nElemCount; i++ )
			ar >> szElems[i].nStation >> szElems[i].nDmisType >> szElems[i].nDmisID;
		short len = 0;
		ar >> len;
		if ( len >= sizeof(szComment) )
			return HRCODE(HRCODE::HC_FAIL);
		if ( len > 0 )
			ar.read(szComment, len);
		szComment[len] = '\0';
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgTSRCmdDisplay::countObjectLength()
{
	return MsgTSRCmdStatus::countObjectLength() + sizeof(nElemCount) + nElemCount*sizeof(short)*3 + sizeof(short) + strlen(szComment) + sizeof(nRemoveFlag);
}


CLBTH_NAMESPACE_END

