
#define EXTERN_LINKAGE_DEFINE
#include "basemsg.h"

CLBTH_NAMESPACE_BEGIN



/////////////////////////////////////////////////////////////////////
EXTERN Archive& operator<<(Archive& ar, const MSGHEAD& head)
{
	return ar << head.length << head.id;
}
EXTERN Archive& operator>>(Archive& ar, MSGHEAD& head)
{
	return ar >> head.length >> head.id;
}
/////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNCREATE_BASEMSG(BaseMsg, MSGID_BASEMSG)

EXTERN Archive& operator<<(Archive& ar, const BaseMsg* pBaseMsg)
	{ ASSERT(NULL!=pBaseMsg && ar.isStoring());
		((BaseMsg*)pBaseMsg)->serialize(ar); return ar; }
EXTERN Archive& operator<<(Archive& ar, const BaseMsg& rBaseMsg)
	{ ASSERT(ar.isStoring());
		((BaseMsg*)&rBaseMsg)->serialize(ar); return ar; }
EXTERN Archive& operator>>(Archive& ar, BaseMsg** pBaseMsg)
	{ ASSERT(NULL!=pBaseMsg && NULL==*pBaseMsg && ar.isLoading());
		*pBaseMsg = ar.readObject(0); return ar; }
EXTERN Archive& operator>>(Archive& ar, const BaseMsg** pBaseMsg)
	{ ASSERT(NULL!=pBaseMsg && NULL==*pBaseMsg && ar.isLoading());
		*pBaseMsg = ar.readObject(0); return ar; }
EXTERN Archive& operator>>(Archive &ar, BaseMsg& mBaseMsg)
{
	ASSERT(ar.isLoading());
	ASSERT((&mBaseMsg)->getTypeIDVirtual() == BaseMsg::getMsgHead(ar).id);
	mBaseMsg.serialize(ar);
	return ar;
}



BaseMsg::BaseMsg() : m_refDeferDelete(1), m_bDeferDelete(FALSE)
{
}

BaseMsg::~BaseMsg()
{
}

BaseMsg* BaseMsg::clone()
{
	Archive ar(Archive::store);
	serialize(ar);

	ar.setMode(Archive::load);
	ar.seek(Archive::BEGIN, 0);

	BaseMsg* pMsg = BaseMsgFactory::readObject(ar);

	return pMsg;
}

HRCODE BaseMsg::doSerialize(Archive& ar)
{
	if(ar.good())
		return HRCODE(HRCODE::HC_SUCCESS);
	else
	{
		if(ar.isStoring())
			return HRCODE(HRCODE::HC_FAIL, HRCODE::E_WRITE_OUTOFBUFFER);
		else
			return HRCODE(HRCODE::HC_FAIL, HRCODE::E_READ_OUTOFBUFFER);
	}
}

HRCODE BaseMsg::serialize(Archive& ar)
{
	HRCODE hr;
	UINT  pos_begin = ar.getCurrentPos();
	UINT  maxcount  = ar.getCount();
	MSGHEAD head;

	if(ar.isStoring())
	{
		if(ar.getSurplusBufSize() < MSGHEAD::SIZEOF)
			goto error;

		head.id = getTypeIDVirtual();
		ar << head;

		hr = doSerialize(ar);
		if(hr.failed() || !ar.good())
			goto error;

		head.length = ar.getCurrentPos() - pos_begin;	// 包括头自身的长度
		ar.seek(Archive::CURRENT, -head.length);
		ar << head;
		ar.seek(Archive::CURRENT, head.length - MSGHEAD::SIZEOF);
	}
	else
	{
		if(ar.getSurplusCount() < MSGHEAD::SIZEOF)
			goto error;

		ar >> head;
		if(head.id != getTypeIDVirtual())
		{
			hr = HRCODE(HRCODE::HC_FAIL, HRCODE::E_OBJECT_NOTCREATED_BY_MSGID, HRCODE::SYS);
			goto error;
		}

		if(ar.getSurplusCount() < head.length - MSGHEAD::SIZEOF)
			goto error;

		hr = doSerialize(ar);
		if(hr.failed() || !ar.good())
			goto error;

		if(ar.getCurrentPos() - pos_begin != head.length)
		{
			hr = HRCODE(HRCODE::HC_FAIL, HRCODE::E_MSGLEN_NEQ_DATALEN, HRCODE::SYS);
			goto error;
		}
	}
	return hr;

error:
	ar.seek(Archive::BEGIN, pos_begin);
	if(!ar.good())
	{
		if(ar.getCount() > maxcount)
			ar.shrink(ar.getCount() - maxcount);
		hr = HRCODE(HRCODE::HC_FAIL, 
			ar.isStoring() ? HRCODE::E_WRITE_OUTOFBUFFER : HRCODE::E_READ_OUTOFBUFFER
			);
	}
	return hr;
}

MSGHEAD BaseMsg::getMsgHead(Archive& ar)
{
	MSGHEAD head;
	ar >> head;
	ar.seek(Archive::CURRENT, -(LONG)MSGHEAD::SIZEOF);
	return head;
}

void BaseMsg::deferDelete()
{
	m_refDeferDelete ++;
	m_bDeferDelete = TRUE;
}

void BaseMsg::endDeferDelete()
{
	m_refDeferDelete --;
	if(m_refDeferDelete <= 1)
		delete this;
}

bool BaseMsg::isDeferDelete()
{
	return m_bDeferDelete;
}

void* BaseMsg::operator new(size_t nSize)
{
	return malloc(nSize);	// must concern no enough memory
}

void  BaseMsg::operator delete(void *p)
{
	free(p);
}

void* BaseMsg::operator new(size_t nSize, void *p)
{
	return p;
}

#ifndef __AIXC__
void  BaseMsg::operator delete(void *p, void*)
{
	free(p);
}
#endif

size_t BaseMsg::countObjectLength()
{
	return 0;
}

INT BaseMsg::toString(void *buf, INT buflen)
{
	if(NULL == buf)
		return -1;

	const char *p = BaseMsgFactory::classNameFromTypeID(getTypeIDVirtual());
	if(buflen > strlen(p))
	{
		strcpy((char*)buf, p);
		return strlen(p);
	}
	else
	{
		memcpy(buf, p, buflen);
		return buflen;
	}
}

#ifdef _MSC_VER
string BaseMsg::toString()
{
	return BaseMsgFactory::classNameFromTypeID(getTypeIDVirtual());
}
#endif


/////////////////////////////////////////////////////////////////////
// special use for LiRC, only
MSGHEAD BaseMsg::getMsgHead(void *buf, int buflen)
{
	if(NULL == buf || buflen < MSGHEAD::SIZEOF)
		return MSGHEAD();
	Archive ar(Archive::load, buflen, buf);
	return getMsgHead(ar);
}

void BaseMsg::moveToNextMsg(Archive& ar)
{
	MSGHEAD  head;
	head = getMsgHead(ar);
	ar.seek(Archive::CURRENT, head.length);
}

HRCODE BaseMsg::serialize(INT mode, void** buf, size_t len)
{
	ASSERT(NULL!=buf && NULL!=*buf);
	ASSERT(mode == Archive::load || mode == Archive::store);
	Archive ar(mode, len, *buf);

	HRCODE  hr = serialize(ar);
	if(hr.succeed())
		*buf = ar.getCurrentPtr();

	return hr;
}

HRCODE BaseMsg::doSerialize(INT mode, void** buf, size_t len)
{
	ASSERT(NULL!=buf && NULL!=*buf);
	ASSERT(mode == Archive::load || mode == Archive::store);
	Archive ar(mode, len, *buf);
	HRCODE hr = doSerialize(ar);
	if(hr.succeed())
		*buf = ar.getCurrentPtr();

	return hr;
}
/////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// class BaseMsgFactory
BaseMsg* BaseMsgFactory::readObject(Archive& ar)
{
	MSGHEAD  head = BaseMsg::getMsgHead(ar);
	
	BaseMsg *pBaseMsg = createObject(head.id);
	if(pBaseMsg && !pBaseMsg->serialize(ar))
	{
		delete pBaseMsg;
		pBaseMsg = NULL;
	}
	return pBaseMsg;
}

BaseMsg* BaseMsgFactory::createObject(const MSGHEAD::msgid_t& id)
{
	BaseMsg::CREATOR  pCreator;
	BaseMsgInit *p = (BaseMsgInit*)getCreatorMap().GetAt(id);
	if(p && (pCreator = p->pCreator))
	{
		BaseMsg *pBaseMsg = pCreator();
		ASSERT(NULL != pBaseMsg);
		
		//ONODE *pNode = new ONODE;
		//pNode->pBaseMsg = pBaseMsg;
		//pNode->next = object_list_head;
		//object_list_head = pNode;
		
		return pBaseMsg;
	}
	return NULL;
}

// link operation need lock 
//BaseMsgFactory::ONODE *BaseMsgFactory::object_list_head = NULL;
void BaseMsgFactory::deleteObject(BaseMsg *pBaseMsg)
{
	if(pBaseMsg && !pBaseMsg->isDeferDelete())
		delete pBaseMsg;
	return;

	/*ONODE *p = object_list_head;
	ONODE *prev = p;
	while(p)
	{
		if(p->pBaseMsg->m_bDeferDelete)
		{
			prev = p;
			p = p->next;
		}
		else
		{
			delete p->pBaseMsg;
			if(p != object_list_head)
			{
				prev->next = p->next;
				delete p;
				p = prev->next;
				prev = p;
				p = p->next;
			}
			else
				p = prev = NULL;
		}
	}*/
}

BOOL BaseMsgFactory::hasClassID(const MSGHEAD::msgid_t& szClassIDRequested)
{
	return getCreatorMap().LookupKey(szClassIDRequested);
}

BaseMsgFactory::init_container_t& BaseMsgFactory::getCreatorMap()
{
	static init_container_t creator_map;
	return creator_map;
}


const char * BaseMsgFactory::stringFromTypeID(const MSGHEAD::msgid_t& id)
{
	BaseMsgInit *p = (BaseMsgInit*)getCreatorMap().GetAt(id);
	if(p)
		return p->pszTypeID;
	return NULL;
}

const char * BaseMsgFactory::classNameFromTypeID(const MSGHEAD::msgid_t& id)
{
	BaseMsgInit *p = (BaseMsgInit*)getCreatorMap().GetAt(id);
	if(p)
		return p->pszClassName;
	return NULL;
}
//////////////////////////////////////////////////////////////////////////





/////////////////////////////////////////////////////////////////////
// class EventSink
/////////////////////////////////////////////////////////////////////
HRCODE EventSink::PostOneEvent(Archive& ar,
	UINIFIEDMSGPROC fUnifiedMsgProc,
	const void* inParam)
{
	if(NULL == fUnifiedMsgProc)
	{
		ASSERT(FALSE);
		return HRCODE(HRCODE::HC_FAIL, 
			HRCODE::E_UNREGISTER_UNIFIEDMSGPRO_OR_LISTENER,
			HRCODE::SYS);
	}
	if( ar.eof() )
	{
		return HRCODE(HRCODE::HC_SUCCESS, 
			HRCODE::S_ARCHIVE_NOMORE_DATA,
			HRCODE::SYS);
	}

	BYTE *orgbuf = ar.getCurrentPtr();
	UINT  pos_begin = ar.getCurrentPos();
	
	MSGHEAD  head = BaseMsg::getMsgHead(ar);
	if(head.id == MSGID_DEFAULT || head.id == MSGID_BASEMSG)
		return HRCODE(HRCODE::HC_FAIL, HRCODE::E_MSGID_RESERVE, HRCODE::SYS);

	VPPARAM param;
	param.inParam = inParam;	//const_cast<void*>(inParam);
	param.pBaseMsg = BaseMsgFactory::createObject(head.id);
	param.msgid = head.id;
	param.buf_len = head.length;
	param.buf_ptr = orgbuf;

	HRCODE hr;
	if(param.pBaseMsg)
	{
		// 创建对象成功时，消息头必须正确
		if(param.buf_len < MSGHEAD::SIZEOF)
		{
			hr = HRCODE(HRCODE::HC_FAIL, 
				HRCODE::E_MSGLEN_LT_MSGHEADSIZE, HRCODE::SYS);
		}
		else
		{
			hr = param.pBaseMsg->serialize(ar);
			if( hr.succeed() )
				hr = fUnifiedMsgProc(param);
		}
	}
	else
	{
		// 不认识的消息ID, 消息头可能不正确, 客户必须更正
		hr = fUnifiedMsgProc(param);
		if( hr.succeed() )
		{
			int len, len2;
			if(orgbuf != param.buf_ptr)	// 指针被调整
			{
				len = (BYTE*)param.buf_ptr - orgbuf;
				len2 = ar.seek(Archive::CURRENT, len);
				if(len2 != len)
				{
					ar.seek(Archive::CURRENT, -len2);
					hr = HRCODE(HRCODE::HC_FAIL,
						HRCODE::E_SEEK_OUTOFBUFFER, HRCODE::SYS);
				}
				else if(len2 != param.buf_len)
					hr.sys_code = HRCODE::S_MSGLEN_INCORRECT;
			}
			else	// 偏移量被调整
			{
				if(param.buf_len < MSGHEAD::SIZEOF)// 消息头必须正确
				{
					hr = HRCODE(HRCODE::HC_FAIL, 
						HRCODE::E_MSGLEN_LT_MSGHEADSIZE, HRCODE::SYS);
				}
				else
				{
					len2 = ar.seek(Archive::CURRENT, param.buf_len);
					if(len2 != param.buf_len)
					{
						ar.seek(Archive::CURRENT, -len2);
						hr = HRCODE(HRCODE::HC_FAIL,
							HRCODE::E_SEEK_OUTOFBUFFER, HRCODE::SYS);
					}
				}
			}
		}
	}
	BaseMsgFactory::deleteObject(param.pBaseMsg);

	return hr;
}


HRCODE EventSink::DefaultMsgProc(const VPPARAM& vpParam)
{
	return HRCODE(HRCODE::HC_FAIL, HRCODE::E_UNREGISTER_DEFAULT_PROC, HRCODE::SYS);
}

HRCODE EventSink::PostOneEvent(Archive& ar,
	MSGHEAD::lsn_container_t& listener, 
	const void* inParam)
{
	if( ar.eof() )
	{
		return HRCODE(HRCODE::HC_SUCCESS, 
			HRCODE::S_ARCHIVE_NOMORE_DATA, HRCODE::SYS);
	}
	
	BYTE *orgbuf = ar.getCurrentPtr();
	UINT  pos_begin = ar.getCurrentPos();

	MSGHEAD  head = BaseMsg::getMsgHead(ar);
	if(head.id == MSGID_DEFAULT || head.id == MSGID_BASEMSG)
		return HRCODE(HRCODE::HC_FAIL, HRCODE::E_MSGID_RESERVE, HRCODE::SYS);

	VPPARAM param;
	param.inParam = inParam;	//const_cast<void*>(inParam);
	param.pBaseMsg = BaseMsgFactory::createObject(head.id);
	param.msgid = head.id;
	param.buf_len = head.length;
	param.buf_ptr = orgbuf;
	
	HRCODE hr;
	if(param.pBaseMsg)
	{
		// 创建对象成功时，消息头必须正确
		if(param.buf_len < MSGHEAD::SIZEOF)
		{
			hr = HRCODE(HRCODE::HC_FAIL, 
				HRCODE::E_MSGLEN_LT_MSGHEADSIZE, HRCODE::SYS);
		}
		else
		{
			hr = param.pBaseMsg->serialize(ar);
			if(hr.succeed())
			{
				BASEMSGPROC OnBaseMsg;
				if( OnBaseMsg = (BASEMSGPROC)listener.GetAt(head.id) )
				{
					hr = OnBaseMsg(param);
					//hr = HRCODE(HRCODE::HC_SUCCESS);
				}

				else	// 没有注册处理函数
				{
					BASEMSGPROC OnUnknown = (BASEMSGPROC)listener.GetAt(0);
					if(NULL == OnUnknown) // 你忘记注册出错处理函数了
						OnUnknown = DefaultMsgProc;
					hr = OnUnknown(param);
				}
			}
		}
	}
	else
	{
		// 不认识的消息ID, 消息头可能不正确, 客户必须更正
		BASEMSGPROC OnUnknown = (BASEMSGPROC)listener.GetAt(0);
		if(NULL == OnUnknown) // 你忘记注册出错处理函数了
			OnUnknown = DefaultMsgProc;
		hr = OnUnknown(param);
		if(hr.succeed())
		{
			int len, len2;
			if(orgbuf != param.buf_ptr)	// 指针被调整
			{
				len = (BYTE*)param.buf_ptr - orgbuf;
				len2 = ar.seek(Archive::CURRENT, len);
				if(len2 != len)
				{
					ar.seek(Archive::CURRENT, -len2);
					hr = HRCODE(HRCODE::HC_FAIL,
						HRCODE::E_SEEK_OUTOFBUFFER, HRCODE::SYS);
				}
				else if(len2 != param.buf_len)
					hr.sys_code = HRCODE::S_MSGLEN_INCORRECT;
			}
			else	// 偏移量被调整
			{
				
				if(param.buf_len < MSGHEAD::SIZEOF)// 消息头必须正确
				{
					hr = HRCODE(HRCODE::HC_FAIL, 
						HRCODE::E_MSGLEN_LT_MSGHEADSIZE, HRCODE::SYS);
				}
				else
				{
					len2 = ar.seek(Archive::CURRENT, param.buf_len);
					if(len2 != param.buf_len)
					{
						ar.seek(Archive::CURRENT, -len2);
						hr = HRCODE(HRCODE::HC_FAIL,
							HRCODE::E_SEEK_OUTOFBUFFER, HRCODE::SYS);
					}
				}
			}
		}
	}
	BaseMsgFactory::deleteObject(param.pBaseMsg);

	return hr;
}

HRCODE EventSink::PostEvent(Archive& ar,
	MSGHEAD::lsn_container_t& listener,
	const void* inParam)
{
	HRCODE hr;
	while( !ar.eof())
	{
		hr = PostOneEventEx(ar, listener, inParam);
		if(hr.failed())
			return hr;
	}
	return HRCODE(HRCODE::HC_SUCCESS, 
		HRCODE::S_ARCHIVE_NOMORE_DATA, HRCODE::SYS);
}

HRCODE EventSink::PostEvent(Archive& ar,
	UINIFIEDMSGPROC fUnifiedMsgProc,
	const void* inParam)
{
	if(NULL == fUnifiedMsgProc)
	{
		ASSERT(FALSE);
		return HRCODE(HRCODE::HC_FAIL, 
			HRCODE::E_UNREGISTER_UNIFIEDMSGPRO_OR_LISTENER,
			HRCODE::SYS);
	}

	HRCODE hr;
	while( !ar.eof() )
	{
		hr = PostOneEvent(ar, fUnifiedMsgProc, inParam);
		if(hr.failed())
			return hr;
	}
	return HRCODE(HRCODE::HC_SUCCESS, 
		HRCODE::S_ARCHIVE_NOMORE_DATA, HRCODE::SYS);
}




UINIFIEDMSGPROC  EventSink::RegisterUnifiedMsgProc(UINIFIEDMSGPROC f)
{
	UINIFIEDMSGPROC  temp = fUnifiedMsgProc;
	fUnifiedMsgProc = f;
	return temp;
}

MSGHEAD::lsn_container_t* 
EventSink::RegisterListener(MSGHEAD::lsn_container_t& listener)
{
	MSGHEAD::lsn_container_t *temp = pListener;
	pListener = &listener;
	return temp;
}

HRCODE EventSink::PostEvent(Archive& ar, const void* inParam)
{
	if(NULL == pListener && NULL == fUnifiedMsgProc)
	{
		return HRCODE(HRCODE::HC_FAIL, 
			HRCODE::E_UNREGISTER_UNIFIEDMSGPRO_OR_LISTENER,
			HRCODE::SYS);
	}

	if(pListener)
		return PostEvent(ar, *pListener, inParam);
	else
		return PostEvent(ar, fUnifiedMsgProc, inParam);
}

HRCODE EventSink::PostOneEvent(Archive& ar, const void* inParam)
{
	if(NULL == pListener && NULL == fUnifiedMsgProc)
	{
		return HRCODE(HRCODE::HC_FAIL, 
			HRCODE::E_UNREGISTER_UNIFIEDMSGPRO_OR_LISTENER,
			HRCODE::SYS);
	}

	if(pListener)
		return PostOneEvent(ar, *pListener, inParam);
	else
		return PostOneEvent(ar, fUnifiedMsgProc, inParam);
}

/////////////////////////////////////////////////////////////////////

HRCODE EventSink::PostOneEventEx(Archive& ar,
	MSGHEAD::lsn_container_t& listener, 
	const void* inParam)
{
	if( ar.eof() )
	{
		return HRCODE(HRCODE::HC_SUCCESS, 
			HRCODE::S_ARCHIVE_NOMORE_DATA, HRCODE::SYS);
	}
	
	BYTE *orgbuf = ar.getCurrentPtr();
	UINT  pos_begin = ar.getCurrentPos();

	MSGHEAD  head = BaseMsg::getMsgHead(ar);
	if(head.id == MSGID_DEFAULT || head.id == MSGID_BASEMSG)
		return HRCODE(HRCODE::HC_FAIL, HRCODE::E_MSGID_RESERVE, HRCODE::SYS);

	VPPARAM param;
	param.inParam = inParam;	//const_cast<void*>(inParam);
	param.pBaseMsg = NULL; //BaseMsgFactory::createObject(head.id);
	param.msgid = head.id;
	param.buf_len = head.length;
	param.buf_ptr = orgbuf;

	HRCODE hr;

	BaseMsgInit *p = (BaseMsgInit*)BaseMsgFactory::getCreatorMap().GetAt(head.id);
	if(p)
	{
		// 创建对象成功时，消息头必须正确
		if(param.buf_len < MSGHEAD::SIZEOF)
		{
			hr = HRCODE(HRCODE::HC_FAIL, 
				HRCODE::E_MSGLEN_LT_MSGHEADSIZE, HRCODE::SYS);
		}
		else
		{
			BASEMSGPROC OnBaseMsg = (BASEMSGPROC)listener.GetAt(head.id);
			if( OnBaseMsg )
				hr = p->pProcessor(ar, OnBaseMsg, param);
			else
			{
				BASEMSGPROC OnUnknown = (BASEMSGPROC)listener.GetAt(0);
				if(NULL == OnUnknown) // 你忘记注册出错处理函数了
					OnUnknown = DefaultMsgProc;
				hr = p->pProcessor(ar, OnUnknown, param);
			}
		}
	}
	else
	{
		// 不认识的消息ID, 消息头可能不正确, 客户必须更正
		BASEMSGPROC OnUnknown = (BASEMSGPROC)listener.GetAt(0);
		if(NULL == OnUnknown) // 你忘记注册出错处理函数了
			OnUnknown = DefaultMsgProc;
		hr = OnUnknown(param);
		if(hr.succeed())
		{
			int len, len2;
			if(orgbuf != param.buf_ptr)	// 指针被调整
			{
				len = (BYTE*)param.buf_ptr - orgbuf;
				len2 = ar.seek(Archive::CURRENT, len);
				if(len2 != len)
				{
					ar.seek(Archive::CURRENT, -len2);
					hr = HRCODE(HRCODE::HC_FAIL,
						HRCODE::E_SEEK_OUTOFBUFFER, HRCODE::SYS);
				}
				else if(len2 != param.buf_len)
					hr.sys_code = HRCODE::S_MSGLEN_INCORRECT;
			}
			else	// 偏移量被调整
			{
				
				if(param.buf_len < MSGHEAD::SIZEOF)// 消息头必须正确
				{
					hr = HRCODE(HRCODE::HC_FAIL, 
						HRCODE::E_MSGLEN_LT_MSGHEADSIZE, HRCODE::SYS);
				}
				else
				{
					len2 = ar.seek(Archive::CURRENT, param.buf_len);
					if(len2 != param.buf_len)
					{
						ar.seek(Archive::CURRENT, -len2);
						hr = HRCODE(HRCODE::HC_FAIL,
							HRCODE::E_SEEK_OUTOFBUFFER, HRCODE::SYS);
					}
				}
			}
		}
	}
	//BaseMsgFactory::deleteObject(param.pBaseMsg);

	return hr;
}



/////////////////////////////////////////////////////////////////////

CLBTH_NAMESPACE_END
