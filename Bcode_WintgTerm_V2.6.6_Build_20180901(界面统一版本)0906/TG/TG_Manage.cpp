//
#include "stdafx.h"
#include "TG_Manage.h"
#include "TGForm.h"
#include "TGDoc.h"
#define MSG_UPDATE_RELATION 10012
CTG_Mange::CTG_Mange()
{
	RemoveAllTrain();
	RemoveAllMark();
	myPForm = NULL;
	myPDoc = NULL;
	m_bBatchDraw=false;
}

CTG_Mange::~CTG_Mange()
{
	RemoveAllTrain();
	RemoveAllMark();
	myPForm = NULL;
	myPDoc = NULL;
}

void CTG_Mange::SetMyForm(CTGForm* form)
{
	ASSERT(form != NULL);
	myPForm = form;
}
	
void CTG_Mange::SetMyDoc(CTGDoc* doc)
{
	ASSERT(doc != NULL);
	myPDoc = doc;
}

CTG_TRAIN* CTG_Mange::GetTrain(TRAIN_KEY key)
{
	map<TRAIN_KEY,  CTG_TRAIN>::iterator 
	it = myTrain.find(key);
	if (it != myTrain.end() && it->first != 0)
	{
		return &it->second;
	}
	return NULL;
}

CTg_BLOCK* CTG_Mange::GetMark(MARK_KEY key)
{
	map<MARK_KEY,  CTg_BLOCK>::iterator it = myMark.find(key);
	if (it != myMark.end() && it->first != 0)
	{
		return &it->second;
	}
	return NULL;
}

void CTG_Mange::RemoveAllTrain()
{
	myTrain.clear();
}

void CTG_Mange::RemoveAllMark()
{
	myMark.clear();
}

void CTG_Mange::BeginLoad()
{
	myPDoc->sysprint(0,0, "[BeginLoad]清空TGOCX中运行图数据及交路关系");

	RemoveAllTrain();
	RemoveAllMark();
	myPDoc->myChezuhao.ClearAllTrainCheHao();
	myPForm->Redraw();
}

void CTG_Mange::EndLoad()
{
	myPForm->Redraw();
	myPDoc->sysprint(0,0, "[EndLoad]TGOCX绘制运行图数据");
}

void CTG_Mange::DeleteTrain(const TRAIN_KEY key, DWORD nEraseFlag)
{
	if(!myPDoc->m_bAcceptCadData)
		return;

	std::map<TRAIN_KEY, CTG_TRAIN>::iterator it = myTrain.find(key);
	if (it != myTrain.end())
	{
		CTG_TRAIN train=(*it).second;
		myTrain.erase(it);

		if(myPForm->m_sFocusTrain != NULL)
		{
			if(myPForm->m_sFocusTrain->m_nTrainIndex == key)
			{
				SetTrainDeleteByOtherGPC(myPForm->m_sFocusTrain);
				myPForm->RefreshTrain(myPForm->m_sFocusTrain);  //2016.09.20 两个运行图同时操作一根线有问题
			}
		}

		myPDoc->sysprint(0,0, "DeleteTrain %s index %u schd %d %s", myPDoc->GetTrainShowId(train), key, myPDoc->GetTgTermType(), train.toString());

		if(myPDoc->m_ptgctrl->mConfigTg.m_context.IsEnableCZH())
		{
			myPDoc->myChezuhao.DeleteCheZuHao(train);
			if (myPDoc->GetTgType()==TG_TYPE::BASIC_TG && myPDoc->m_ptgctrl->mConfigTg.m_context.GetBasicSchdCanModify())
			{
				myPDoc->ReGenTrainReturnTrainID(train);
			}
		}
		
		if(nEraseFlag==0)
		   myPForm->EraseTrain(train);

		if(myPForm->m_sFocusTrain != NULL)
		{
			if(myPForm->m_sFocusTrain->m_nTrainIndex == key)
			{
				myPForm->m_sFocusTrain = NULL;
			}
		}
		if(myPDoc->m_ptgctrl->mConfigTg.m_context.IsEnableNewCZH())
		{
			myPDoc->ReGenTrainJiaoluRelation(); // DeleteTrain
			if(nEraseFlag==0)
			{
				DrawPrevTrain(train.prev_train_index, 0);
				NoticeDlgUpdate();
			}
		}
		TrytoDeleteOtherTrain(train);
	}
}

void CTG_Mange::DeleteMark(const MARK_KEY key)
{
	if(!myPDoc->m_bAcceptCadData)
		return;
	std::map<MARK_KEY,  CTg_BLOCK>::iterator it = myMark.find(key);
	if (it != myMark.end())
	{
		myPDoc->DelMarkMoveIndex(key);
		if(myPForm->m_nFocusFigure != NULL)
		{
			if(myPForm->m_nFocusFigure->mark_index == key)
			{
				SetDeleteByOtherGPC(myPForm->m_nFocusFigure);
			}
		}

		myPForm->EraseBlock((*it).second);
		myMark.erase(it);
		myPDoc->sysprint(0,0, "删除施工标记(DeleteMark):%ld schd %d", key, myPDoc->GetTgTermType());
	}
}

int CTG_Mange::FindInvisibleMark(DWORD btm, DWORD etm, ULONG mark_index[], const ULONG index_cnt, int nDispDistanceNum)
{
	map<MARK_KEY,  CTg_BLOCK>::iterator it = myMark.begin();
	int count=0;
	for( ; it != myMark.end(); it++)
	{
		if((etm<it->second.start_time)||(it->second.end_time<btm))
			continue;

			switch(it->second.type)
			{
			case ABS_INHIBIT_MARK:
			case ABS_SLOW_MARK:
			case ABS_FIX_MARK:
			case NEW_ABS_ELE_FIX_MARK:
			case ABS_ELE_FIX_MARK:
			case SELF_CLOSE_MARK:
				{
					UINT nMinIndex = (it->second.station < it->second.index)?it->second.station:it->second.index;
					UINT nMaxIndex = (it->second.station > it->second.index)?it->second.station:it->second.index;

					if(nMinIndex < 0 || nMinIndex>=512 || nMaxIndex < 0 || nMaxIndex>=512)
					{
						myPDoc->sysprint(5,0, "由于站序1 %d 或站序2 %d 越界(0<=index<512),选中该符号block %d , type %d ", it->second.station ,it->second.index, it->second.mark_index, it->second.type);
						if(count < index_cnt)
						{
							mark_index[count]=it->first;
							count++;
						}
					}
					if(nMinIndex >=nDispDistanceNum)
					{
						myPDoc->sysprint(5,0, "站序1 %d 或站序2 %d 不在有效显示区间内,选中该符号block %d , type %d ", it->second.station ,it->second.index, it->second.mark_index, it->second.type);
						if(count < index_cnt)
						{
							mark_index[count]=it->first;
							count++;
						}
					}
					if(nMaxIndex >= nDispDistanceNum )
					{
						nMaxIndex=nDispDistanceNum-1;
						if(nMinIndex>=nMaxIndex)
						{
							myPDoc->sysprint(5,0, "由于站序1 %d 或站序2 %d 不在有效显示区间内,选中该符号block %d , type %d ", it->second.station ,it->second.index, it->second.mark_index, it->second.type);
							if(count < index_cnt)
							{
								mark_index[count]=it->first;
								count++;
							}
						}
					}
				}
				break;
			case SIDE_INHIBIT_MARK:
			case SIDE_SLOW_MARK:
			case SIDE_OCCUPY_MARK:
			case SIDE_TIANCHUANG_MARK:
				{
					UINT nIndex = it->second.index;
					if(nIndex < 0 || nIndex>=512 || nIndex >= nDispDistanceNum)
					{
						myPDoc->sysprint(5,0, "由于站序 %d 不在有效显示区间内,选中该符号block %d , type %d",it->second.index, it->second.mark_index, it->second.type);
						if(count < index_cnt)
						{
							mark_index[count]=it->first;
							count++;
						}
					}
				}
				break;
			case GRAPH_NOTE_MARK:
			case NEW_GRAPH_NOTE_MARK:
				break;
			default:
				break;
			}
	}

	return count;
}

int CTG_Mange::GetInValidTrainCount(int trainIndex[],int num)
{
	int count=0;
	std::map<TRAIN_KEY, CTG_TRAIN>::iterator it = myTrain.begin();
	for (; it!=myTrain.end(); it++)
	{
		if(count>=num)
			break;
		for(int i=0; i<it->second.myRecord.size(); i++)
		{
			if(it->second.myRecord[i].in_station_index==-1 || it->second.myRecord[i].out_station_index==-1)
			{
				trainIndex[count]=it->first;
				count++;
				break;
			}
		}
	}

	return count;
}

void CTG_Mange::GenTrainIndex()
{
	std::map<TRAIN_KEY, CTG_TRAIN>::iterator it = myTrain.begin();
	for (; it != myTrain.end(); it++)
	{
		//myPDoc->ReGeneratetStationInOutIndex((*it).second); //20150126 不再重新计算节点号
		myPDoc->SendUpdateTrain((*it).second);
	}
}

const CTG_TRAIN* CTG_Mange::GetTrainByPoint(CPoint& pt)
{
	std::map<TRAIN_KEY, CTG_TRAIN>::iterator it = myTrain.begin();
	for (; it != myTrain.end(); it++)
	{
		if((*it).second.IsInShowTrainIDRect(pt))
			return &((*it).second);
	}

	return NULL;
}

void CTG_Mange::GenTrainCZHRelation(CString czh)
{
	if(!myPDoc->m_ptgctrl->mConfigTg.m_context.IsEnableCZH())
		return;

	myPForm->DrawSelTrainCZHLine();
	std::map<TRAIN_KEY, CTG_TRAIN>::iterator it = myTrain.begin();
	while(it != myTrain.end())
	{
		if(it->second.myTrainInfo.shift_index_string==czh)
		{
			myPDoc->myChezuhao.AppendCheZuHao(it->second);
		}
		it++;
	}
}

void CTG_Mange::InitTrainDrawInfo(CTG_TRAIN& train)
{
	myPDoc->SetStationInOutIndex(train);
	myPDoc->SetTrainDirect(train);
	myPDoc->SetDirectAtStation(&train);
	myPDoc->SetTrainRect(&train);
	myPDoc->SetTimeValid(&train);

	myPDoc->SetTrainSideAndSchdReal(train);
	myPDoc->SetTrainWidthColor(&train);

	train.SetTrainType();
	train.SetEleLocoTrain();
}

void CTG_Mange::UpdateTrain(CTG_TRAIN& train)
{
	if(!myPDoc->m_bAcceptCadData)
		return;

	TRAIN_KEY key = train.m_nTrainIndex;
	//1. 计算train的属性
	if(train.GetRecordNum() <= 0)
	{	
		myPDoc->sysprint(0,0, "删除运行线，由于没有合法记录(UpdateTrain):索引: %ld", key);
		return;
	}

	if(TG_TYPE::BASIC_TG != myPDoc->GetTgType())
	{
		TIME begin=0, end=0;
		myPDoc->GetAcceptDataScope(begin, end);
		if(!train.IfInTimeScope(begin,end))
		{
			myPDoc->sysprint(0,0, "UpdateTrain失败:%s index %u schd %d %s", myPDoc->GetTrainShowId(train), key, myPDoc->GetTgTermType(), train.toString());
			
			CTime sTime(begin), eTime(end);
			myPDoc->sysprint(0,0, "不在当前显示范围:%s -- %s", 
				sTime.Format("%Y-%m-%d %H:%M"), eTime.Format("%Y-%m-%d %H:%M"));
				
			return;
		}
	}

	InitTrainDrawInfo(train);

	//2. 更新内存和界面
	std::map<TRAIN_KEY, CTG_TRAIN>::iterator it = myTrain.find(key);
	if (it == myTrain.end())
	{
		myPForm->DrawTrain(train, false);
		myTrain[key] = train;
		UpdateSingleTrain(key, train);
		myPDoc->sysprint(0,0, "AddTrain:%s index %u schd %d %s", myPDoc->GetTrainShowId(train), key, myPDoc->GetTgTermType(), train.toString());

		if(myPDoc->m_ptgctrl->mConfigTg.m_context.IsEnableCZH())
		{
			myPDoc->myChezuhao.UpdateCheZuHao(train);
			myPForm->DrawSelTrainCZHLine();
		}	
		else if(myPDoc->m_ptgctrl->mConfigTg.m_context.IsEnableNewCZH())
		{
			myPDoc->ReGenTrainJiaoluRelation();  //AppendTrain
			DrawPrevTrain(train.prev_train_index, 0);
			NoticeDlgUpdate();
		}
	}
	else
	{
		myPDoc->sysprint(0,0, "UpdateTrain:%s index %u schd %d %s", myPDoc->GetTrainShowId(train), key, myPDoc->GetTgTermType(), train.toString());

		if(myPForm->m_sFocusTrain != NULL)
		{
			if(myPForm->m_sFocusTrain->m_nTrainIndex == key)
			{
				//SetTrainUpdateByOtherGPC(myPForm->m_sFocusTrain);

				myPForm->m_nFocusTrainIndex = key;
				myPForm->RefreshTrain(&(myPDoc->m_myFocusTrain));
			}
		}
		// 本台选中的信息,其他台更新怎本台选中信息消失
		CTG_TRAIN *ptrain = &(it->second);
		train.myTrainInfo.whole_static = ptrain->myTrainInfo.whole_static;
		for(UINT i=0; i<train.GetRecordNum(); i++)
		{
			TgTRAIN_RECORD* rd = train.GetRecordByRecIndex(i);
			if(NULL == rd)
				continue;

			for(UINT j=0; j<ptrain->GetRecordNum(); j++)
			{
				TgTRAIN_RECORD* oldrd = ptrain->GetRecordByRecIndex(j);
				if(NULL != oldrd && rd->station == oldrd->station)
				{
					rd->static_arrive = oldrd->static_arrive;
					rd->static_depart = oldrd->static_depart;
					rd->static_manual = oldrd->static_manual;
					
					break;
				}
			}
		}

		if(!train.IsTrainCoreSame((*it).second))
		{
			myPForm->EraseTrain((*it).second);

			myPForm->DrawTrain(train, train.myTrainInfo.b_self_update);
			TrytoModifyOtherTrain(train);
			// Update it
			TRAIN_INDEX prev_old_train_index=(*it).second.prev_train_index;
			myTrain[key] = train;
			UpdateSingleTrain(key, train);

			if(myPDoc->m_ptgctrl->mConfigTg.m_context.IsEnableCZH())
			{
				myPDoc->myChezuhao.UpdateCheZuHao(train);
				myPForm->DrawSelTrainCZHLine();

				if (myPDoc->GetTgType()==TG_TYPE::BASIC_TG && myPDoc->m_ptgctrl->mConfigTg.m_context.GetBasicSchdCanModify())
				{
					if(train.IsTrainIDChange((*it).second))
					{
						myPDoc->ReGenTrainReturnTrainID(train);
					}
				}
			}
			else if(myPDoc->m_ptgctrl->mConfigTg.m_context.IsEnableNewCZH())
			{
				// step1. 更新交路关系
				if(prev_old_train_index!=train.prev_train_index)
				{
					myPDoc->ReGenTrainJiaoluRelation();  //UpdateTrain
					DrawPrevTrain(train.prev_train_index, prev_old_train_index);
				}
				// step2. 更新车组中列车时间
				myPDoc->myChezuhao.UpdateCheZuHaoByRelation(train);
				NoticeDlgUpdate();
			}
		}
		else
		{
			myPForm->DrawTrain(train, train.myTrainInfo.b_self_update);

			// Update it
			myTrain[key] = train;
			UpdateSingleTrain(key, train);
		}

		if(myPForm->m_sFocusTrain != NULL)
		{
			if(myPForm->m_sFocusTrain->m_nTrainIndex == key)
			{
				myPDoc->m_myFocusTrain = train;
				myPForm->m_nFocusTrainIndex = train.GetTrainIndex();
			}
		}
	}
}
void CTG_Mange::UpdateMark(CTg_BLOCK& block)
{
	if(!myPDoc->m_bAcceptCadData)
		return;


	// 其他途径生成的标记---需要生成索引 位置1 位置2
	if(block.direction == 0xFF)
	{
		if(!myPDoc->GenDefaultBlock(block))
		{
			block.combstation = 0;
			myPDoc->sysprint(5,0,"Insert new mark from opms. But genDefaultBlock fail");
			return;
		}
        
		block.combstation = block.station1 << 16;
		block.combstation += block.station2;

		CString xml = block.toXML();
		myPDoc->m_ptgctrl->FIRE_TG_UPDATE_MARK(block.mark_index, xml);
	}

	if(TG_TYPE::BASIC_TG != myPDoc->GetTgType())
	{
		TIME begin=0, end=0;
		myPDoc->GetAcceptDataScope(begin, end);
		if(!block.IfInTimeScope(begin,end))
		{
			myPDoc->sysprint(0,0, "UpdateMark失败:%u shcd %d, %s", block.mark_index, myPDoc->GetTgTermType(), block.toString());
			
			CTime sTime(begin), eTime(end);		
			myPDoc->sysprint(0,0, "不在当前显示范围:%s -- %s", 
					sTime.Format("%Y-%m-%d %H:%M"), eTime.Format("%Y-%m-%d %H:%M"));
					
			return;
		}
	}

	MARK_KEY key=block.mark_index;
	std::map<MARK_KEY,  CTg_BLOCK>::iterator it = myMark.find(key);
	if (it == myMark.end())
	{
		myPForm->DrawBlock(block);
		myMark[key] = block;
		myPDoc->sysprint(0,0, "增加施工标记(UpdateMark):%u shcd %d, %s", key, myPDoc->GetTgTermType(), block.toString());
	}
	else
	{
		if(myPForm->m_nFocusFigure != NULL)
		{
			if(myPForm->m_nFocusFigure->mark_index != key)
			{
				SetUpdateByOtherGPC(myPForm->m_nFocusFigure);
			}
		}

		myPForm->EraseBlock((*it).second);
		myPForm->DrawBlock(block);

		// Update it
		myMark[key] = block;
		block.text[255]=0;
		myPDoc->sysprint(0,0, "更新施工标记(UpdateMark):%u schd %d, %s %s", key, myPDoc->GetTgTermType(), block.text, block.toString());

		if(myPForm->m_nFocusFigure != NULL)
		{
			if(myPForm->m_nFocusFigure->mark_index == key)
			{
				block.m_bFocus = myPForm->m_nFocusFigure->m_bFocus;

				block.bMoveBottom = myPForm->m_nFocusFigure->bMoveBottom;
				block.bMoveCenter = myPForm->m_nFocusFigure->bMoveCenter;
				block.bMoveLeft = myPForm->m_nFocusFigure->bMoveLeft;

				block.bMoveRight = myPForm->m_nFocusFigure->bMoveRight;
				block.bMoveTop = myPForm->m_nFocusFigure->bMoveTop;

				myPDoc->m_myFocusBlock = block;
				myPForm->m_nFocusBlockIndex = block.mark_index;
			}
		}
	}
}

void CTG_Mange::SetBatchDrawEnv(LONG nFlag)
{
	if(nFlag==0)
	{
		myPDoc->sysprint(0,0, "[SetBatchDrawEnv]设置批量绘制标志,清空TGOCX中运行线数据及交路");

		RemoveAllMark();
		RemoveAllTrain();
		myPDoc->myChezuhao.ClearAllTrainCheHao();
		
		m_bBatchDraw=true; // 整体画线
		myPForm->SetRightScrollPosition();
	}
	else if(nFlag==1)
	{
		m_bBatchDraw=true; // 整体画线
		myPDoc->sysprint(0,0, "[SetBatchDrawEnv]确保设置批量绘制标志");
	}

	myPForm->ResetFocusTrain();
}

void CTG_Mange::BatchAddTrain(CTG_TRAIN& train)
{
	if(m_bBatchDraw)
	{
		if(TG_TYPE::BASIC_TG != myPDoc->GetTgType())
		{
			TIME begin=0, end=0;
			myPDoc->GetAcceptDataScope(begin, end);
			if(!train.IfInTimeScope(begin,end))
			{
				myPDoc->sysprint(0,0, "批量[BatchAddTrain]失败:%s index %u schd %d %s", myPDoc->GetTrainShowId(train), 
					train.m_nTrainIndex, myPDoc->GetTgTermType(), train.toString());

				CTime sTime(begin), eTime(end);
				myPDoc->sysprint(0,0, "不在当前显示范围:%s -- %s", 
					sTime.Format("%Y-%m-%d %H:%M"), eTime.Format("%Y-%m-%d %H:%M"));
					
				return;
			}
		}

		InitTrainDrawInfo(train);

		myTrain[train.m_nTrainIndex] = train;

		myPDoc->sysprint(0,0, "批量[BatchAddTrain]:%s index %u schd %d %s", myPDoc->GetTrainShowId(train), 
			train.m_nTrainIndex, myPDoc->GetTgTermType(), train.toString());
	}
}

void CTG_Mange::BatchAddBlock(CTg_BLOCK& block)
{
	if(m_bBatchDraw)
	{
		if(TG_TYPE::BASIC_TG != myPDoc->GetTgType())
		{
			TIME begin=0, end=0;
			myPDoc->GetAcceptDataScope(begin, end);
			if(!block.IfInTimeScope(begin,end))
			{
				myPDoc->sysprint(0,0, "批量[BatchAddBlock]失败:%u schd %d %s", 
					block.mark_index, myPDoc->GetTgTermType(), block.toString());
					
				CTime sTime(begin), eTime(end);
				myPDoc->sysprint(0,0, "不在当前显示范围:%s -- %s", 
					sTime.Format("%Y-%m-%d %H:%M"), eTime.Format("%Y-%m-%d %H:%M"));
					
				return;
			}
		}

		myMark[block.mark_index] = block;
		myPDoc->sysprint(0,0, "批量[BatchAddBlock]:%u schd %d, %s", block.mark_index, myPDoc->GetTgTermType(), block.toString());
	}
}

void CTG_Mange::BatchDraw(LONG nFlag)
{
	if(myPDoc->m_ptgctrl->mConfigTg.m_context.IsEnableCZH())
	{
		myPDoc->sysprint(0,0, "[BatchDraw]重新生成车组关系");
		myPDoc->ReGenTrainCZHRelation();
		myPForm->DrawSelTrainCZHLine();
	}
	else if(myPDoc->m_ptgctrl->mConfigTg.m_context.IsEnableNewCZH())
	{
		myPDoc->sysprint(0,0, "[BatchDraw]重新生成车组关系,根据列车前序索引");
		myPDoc->ReGenTrainJiaoluRelation();  // BatchDraw
	}
	myPDoc->sysprint(0,0, "[BatchDraw]批量绘制运行图");
	myPForm->Redraw();

	myPDoc->sysprint(0,0, "[BatchDraw]恢复正常绘制运行图");
	m_bBatchDraw=false;
}

BYTE CTG_Mange::IsBatchStatus(void)
{
	return m_bBatchDraw ? TRUE : FALSE;
}

bool CTG_Mange::DataIsExistInOCX(ULONG nTrainIndex, ULONG data_type)
{
	if(1==data_type)
	{
		CTG_TRAIN* ptrain = GetTrain(nTrainIndex);
		if(ptrain!=NULL)
		{
			return true;
		}
	}
	else if(2==data_type)
	{
		CTg_BLOCK* pBlock = GetMark(nTrainIndex);
		if(pBlock != NULL)
		{
			return true;
		}
	}
	return false;
}

void CTG_Mange::SetTrainIndexRelation(DWORD prev_train_index, DWORD this_train_index)
{
	if(!myPDoc->m_ptgctrl->mConfigTg.m_context.IsEnableNewCZH())
	{
		return;
	}
	
	//myPDoc->sysprint(0,0, "处理车组关系 this_train_index %u prev_train_index %u.", 
	//											this_train_index, prev_train_index);
	for(int i=0; i<m_nTrainGroupCount; i++)
	{
		for(int n=0; n<m_trainGroup[i].train_count; n++)
		{
			if(m_trainGroup[i].prev_train_index[n]==prev_train_index &&
				m_trainGroup[i].this_train_index[n]==this_train_index)
			{
				myPDoc->sysprint(0,0, "该车组关系 this_train_index %u prev_train_index %u 已处理, 返回.", 
												this_train_index, prev_train_index);
				return;
			}
		}
	}

	CTG_TRAIN*ptrain=GetTrain(this_train_index);
	if(ptrain==NULL)
	{
		myPDoc->sysprint(0,0, "按照 %u 找不到列车, 不能处理车组关系 this_train_index %u prev_train_index %u, 返回.", 
												this_train_index, this_train_index, prev_train_index);
		return;
	}

	CString czh;
	// 试图删除车组索引关系
	if(prev_train_index==0)
	{
		for(int i=0; i<m_nTrainGroupCount; i++)
		{
			int p=-1, q=-1;
			for(int n=0; n<m_trainGroup[i].train_count; n++)
			{
				if(m_trainGroup[i].this_train_index[n]==this_train_index)
				{
					if(n==0)
					{
						m_trainGroup[i].prev_train_index[n]=0;
					}
					else if(n>0)
					{
						p=n;
					}
					break;
				}
			}

			if(p!=-1)
			{
				TrainGroup tmp=m_trainGroup[i];
				for(int m=m_trainGroup[i].train_count-1; m>=p; m--)
				{
					m_trainGroup[i].prev_train_index[m]=0;
					m_trainGroup[i].this_train_index[m]=0;
					m_trainGroup[i].train_count--;
				}
				
				if(p<tmp.train_count)
				{
					TrainGroup newTrainGroup;
					newTrainGroup.group_index=tmp.this_train_index[p];
					newTrainGroup.train_count=0;
					for(int m=p; m<tmp.train_count; m++)
					{
						newTrainGroup.prev_train_index[m-p] =
							tmp.prev_train_index[m];
						newTrainGroup.this_train_index[m-p] =
							tmp.this_train_index[m];


						newTrainGroup.train_count++;
					}
					newTrainGroup.prev_train_index[0] = 0;

					m_trainGroup[m_nTrainGroupCount]=newTrainGroup;
					m_nTrainGroupCount++;
				}
				return;
			}
		}
	}
	
	int group_index1=-1;
	if(prev_train_index>0)
	{
		for(int i=0; i<m_nTrainGroupCount; i++)
		{
			int n=m_trainGroup[i].train_count;
			if(m_trainGroup[i].this_train_index[n-1]==prev_train_index)
			{
				group_index1=i;
				break;
			}
		}
	}

	int group_index2=-1;
	int group_index3=-1;
	if(this_train_index>0)
	{
		for(int i=0; i<m_nTrainGroupCount; i++)
		{
			if(m_trainGroup[i].this_train_index[0]==this_train_index)
			{
				group_index2=i;
				break;
			}
		}
		for(int i=0; i<m_nTrainGroupCount; i++)
		{
			if(m_trainGroup[i].prev_train_index[0]==this_train_index)
			{
				group_index3=i;
				break;
			}
		}
	}

	if(group_index1>=0 && group_index2>=0)
	{
		// 手工合并
		if(group_index1!=group_index2)
		{
			TrainGroup newGroup=m_trainGroup[group_index1];
			TrainGroup delGroup=m_trainGroup[group_index2];
			for(int n=0; n<delGroup.train_count; n++)
			{
				if(n==0)
					newGroup.prev_train_index[newGroup.train_count]
						= prev_train_index;
				else
					newGroup.prev_train_index[newGroup.train_count]
						= delGroup.prev_train_index[n];
				newGroup.this_train_index[newGroup.train_count]
						= delGroup.this_train_index[n];

				newGroup.train_count++;

			}
			m_trainGroup[group_index1]=newGroup;

			for(int n=group_index2; n<m_nTrainGroupCount; n++)
			{
				m_trainGroup[n]=m_trainGroup[n+1];
			}
			m_nTrainGroupCount--;
		}
	}
	else if(group_index1>=0 && group_index3>=0)
	{
		// 自动加线, 合并
		if(group_index1!=group_index3)
		{
			TrainGroup newGroup=m_trainGroup[group_index1];
			TrainGroup delGroup=m_trainGroup[group_index3];
			newGroup.prev_train_index[newGroup.train_count]=prev_train_index;
			newGroup.this_train_index[newGroup.train_count]=this_train_index;
			newGroup.train_count++;


			for(int n=0; n<delGroup.train_count; n++)
			{
				newGroup.prev_train_index[newGroup.train_count]
						= delGroup.prev_train_index[n];
				newGroup.this_train_index[newGroup.train_count]
						= delGroup.this_train_index[n];

				newGroup.train_count++;

			}
			m_trainGroup[group_index1]=newGroup;

			for(int n=group_index3; n<m_nTrainGroupCount; n++)
			{
				m_trainGroup[n]=m_trainGroup[n+1];
			}
			m_nTrainGroupCount--;
		}
	}
	else if(group_index1>=0)
	{
		// 自动加线, 添加到前面车组关系的最后
		TrainGroup newGroup=m_trainGroup[group_index1];
		newGroup.prev_train_index[newGroup.train_count]=prev_train_index;
		newGroup.this_train_index[newGroup.train_count]=this_train_index;
		newGroup.train_count++;
		m_trainGroup[group_index1]=newGroup;

	}
	else if(group_index3>=0)
	{
		// 自动加线, 添加到后面车组关系的最前
		TrainGroup newGroup=m_trainGroup[group_index3];
		for(int n=newGroup.train_count-1; n>=0; n--)
		{
			newGroup.prev_train_index[n+1]=newGroup.prev_train_index[n];
			newGroup.this_train_index[n+1]=newGroup.this_train_index[n];
		}
		newGroup.prev_train_index[0]=prev_train_index;
		newGroup.this_train_index[0]=this_train_index;
		newGroup.train_count++;
		m_trainGroup[group_index3]=newGroup;

	}
	else if(group_index2>=0)
	{
		// 调整更新后线
		TrainGroup newGroup=m_trainGroup[group_index2];
		newGroup.prev_train_index[0]=prev_train_index;
		m_trainGroup[group_index2]=newGroup;
	}
	else
	{
		// 独立门户
		if(m_nTrainGroupCount<MAX_CZH_MEM)
		{
			m_trainGroup[m_nTrainGroupCount].group_index=this_train_index;
			m_trainGroup[m_nTrainGroupCount].train_count=1;
			m_trainGroup[m_nTrainGroupCount].this_train_index[0]=this_train_index;
			m_trainGroup[m_nTrainGroupCount].prev_train_index[0]=prev_train_index;
			m_nTrainGroupCount++;
		}
		else
		{
			myPDoc->sysprint(0,0, "新增车组关系 %u 存放列车 %u 前序索引 %u 失败,超过车组关系最大数目, 当前车组关系个数 %d.", 
					this_train_index, this_train_index, prev_train_index, m_nTrainGroupCount);
		}
	}
}

bool CTG_Mange::GetMaybePrevTrains(int station, long depart, CString& text, CStringArray& list_trainno, CDWordArray& list_trainindex)
{
	list_trainno.RemoveAll();
	list_trainindex.RemoveAll();

	map<TRAIN_KEY,  CTG_TRAIN>::iterator it;
	for (it=myTrain.begin(); it != myTrain.end(); it++)
	{
		TgTRAIN_RECORD* pLast=it->second.GetLastRecord();
		if(pLast==NULL)
			continue;
		if(pLast->station != station)
			continue;
		if(! IsFlagTerminal(pLast->flag))
			continue;
		if (myPDoc->GetTgType()==TG_TYPE::BASIC_TG)
		{
			;// 基本图不考虑时间顺序
		}
		else
		{
			if(pLast->arrive>depart)
				continue;
		}
		
		CTime tm(pLast->arrive);
		CString date="(";
		date += tm.Format("%Y-%m-%d %H:%M");
		date += ")";
		list_trainno.Add(pLast->arrive_train_id+date);
		list_trainindex.Add(it->first);
	}
	
	if(list_trainno.GetCount()>0)
	{
		return true;
	}
	text="选中列车没有可选前续列车, 不能勾连前续列车";
	return false;
}

void CTG_Mange::GetMaybeNextTrains(int station, long arrive, CString& trainid, CStringArray& list_trainno, CDWordArray& list_trainindex)
{
	list_trainno.RemoveAll();
	list_trainindex.RemoveAll();

	map<TRAIN_KEY,  CTG_TRAIN>::iterator it;
	for (it=myTrain.begin(); it != myTrain.end(); it++)
	{
		TgTRAIN_RECORD* pfirst=it->second.GetFirstRecord();
		if(pfirst==NULL)
			continue;
		if(pfirst->station != station)
			continue;
		if(! IsFlagStart(pfirst->flag))
			continue;
		if (myPDoc->GetTgType()==TG_TYPE::BASIC_TG)
		{
			;// 基本图不考虑时间顺序
		}
		else
		{
			if(pfirst->depart<arrive+60)
				continue;
		}
		if(pfirst->depart_train_id!=trainid)
			continue;
		
		CTime tm(pfirst->depart);
		CString date="(";
		date += tm.Format("%Y-%m-%d %H:%M");
		date += ")";
		list_trainno.Add(pfirst->depart_train_id+date);
		list_trainindex.Add(it->first);
	}
}

bool CTG_Mange::GenTrainInfo(CTG_TRAIN* ptrain, TrainInfo& info)
{
	CTG_TRAIN train=*ptrain;
	if(TG_TYPE::BASIC_TG == myPDoc->GetTgType())
		myPDoc->SetBasicTrainDate(&train);

	const TgTRAIN_RECORD* first=train.GetFirstRecord();
	const TgTRAIN_RECORD* last=train.GetLastRecord();
	if(first==NULL || last==NULL)
		return false;

	info.train_index=train.GetTrainIndex();
	info.begin_station=first->station;
	info.end_station=last->station;
	info.begin_side=first->depart_side;
	info.end_side=last->arrive_side;
	
	info.stTime=0;
	if(IsFlagStart(first->flag))
	{
		info.stTime=first->depart;
		strncpy(info.trainname, first->depart_train_id, 10);
		info.trainname[9]=0;
	}
	else
	{
		info.stTime=first->arrive;
		strncpy(info.trainname, first->arrive_train_id, 10);
		info.trainname[9]=0;
	}
		
	info.edTime=0;
	if(IsFlagTerminal(last->flag))
	{
		info.edTime=last->arrive;
	}
	else
	{
		info.edTime=last->depart;
	}

	return true;
}

void CTG_Mange::GetTrainRelation(vector<TrainGroupInfo>& list, bool bSortByName)
{
	m_nTrainGroupCount=0;
	memset(m_trainGroup, 0, sizeof(m_trainGroup));
	
	map<TRAIN_KEY,  CTG_TRAIN>::iterator it;
	for (it=myTrain.begin(); it != myTrain.end(); it++)
	{
		SetTrainIndexRelation(it->second.prev_train_index, it->first);
	}

	GetTrainGroupInfo(list, bSortByName);
}

void CTG_Mange::GetTrainGroupInfo(vector<TrainGroupInfo>& list, bool bSortByName)
{
	int count=0;
	for(int m=0; m<m_nTrainGroupCount; m++)
	{
		TrainGroupInfo group;
		group.train_count=0;
		CString str;
		for(int n=0; n<m_trainGroup[m].train_count; n++)
		{
			TrainInfo info;
			CTG_TRAIN* ptrain=this->GetTrain(m_trainGroup[m].this_train_index[n]);
			if(ptrain)
			{
				GenTrainInfo(ptrain, info);
				group.trains[group.train_count]=info;
				
				if(str=="")
					str=myPDoc->GetTrainShowId(ptrain);
				else
				{
					str+="/";
					str+=myPDoc->GetTrainShowId(ptrain);
				}
				group.train_count++;
			}
		}
		group.strName.Format("%s(%u)", str, m_trainGroup[m].group_index);
		
		if(bSortByName)
		{
			for(int n=0; n<list.size(); n++)
			{
				if(list[n].strName>group.strName)
				{
					break;
				}
			}
			if(n>=list.size())
			{
				list.push_back(group);
			}
			else
			{
				list.insert(list.begin()+n, group);
			}
		}
		else
		{
			list.push_back(group);
		}
	}
}

void CTG_Mange::DrawPrevTrain(TRAIN_INDEX prev_train_index1, TRAIN_INDEX prev_train_index2)
{
	if(prev_train_index1!=0)
	{
		CTG_TRAIN* ptrain=GetTrain(prev_train_index1);
		if(ptrain)
		{
			CTG_TRAIN train=*ptrain;
			myPForm->EraseTrain(train);
			myPForm->DrawTrain(train, false);
		}
	}
	if(prev_train_index2!=0)
	{
		CTG_TRAIN* ptrain=GetTrain(prev_train_index2);
		if(ptrain)
		{
			CTG_TRAIN train=*ptrain;
			myPForm->EraseTrain(train);
			myPForm->DrawTrain(train, false);
		}
	}
}

void CTG_Mange::NoticeDlgUpdate()
{
	CString title1, title2;
	myPDoc->GetJiaoluDlgName(title1, title2);
	CWnd * pWnd=CWnd::FindWindow(NULL,title1);
	if(pWnd)
	{
		pWnd->SendMessage(MSG_UPDATE_RELATION);
	}
	
	pWnd=CWnd::FindWindow(NULL, title2);
	if(pWnd)
	{
		pWnd->SendMessage(MSG_UPDATE_RELATION);
	}
}

void CTG_Mange::SetSingleTrain(bool bSingleTrainMode)
{
	if(bSingleTrainMode)
	{
		myTrainTmp=myTrain;
		myTrain.clear();
	}
	else
	{
		myTrain=myTrainTmp;
		myTrainTmp.clear();
	}
}

void CTG_Mange::ClearSingleTrain()
{
	myTrain.clear();
}

void CTG_Mange::AppendSingleTrain(TRAIN_INDEX index, const CTG_TRAIN& train)
{
	myTrain[index] = train;
}

void CTG_Mange::UpdateSingleTrain(TRAIN_INDEX index, const CTG_TRAIN& train)
{
	if(myPDoc->m_bShowSingleTrain)
		myTrainTmp[index] = train;
}

void CTG_Mange::TrytoModifyOtherTrain(CTG_TRAIN& train)
{
	std::vector<CTG_TRAIN> train_list;

	// 1. 区间途中折返车次
	myPDoc->TrytoFindAbsEngineReturnTrain(&train, train_list);
	for (int n=0; n<train_list.size(); n++)
	{
		bool bModify=myPDoc->TrytoMoveOtherTrain(&train, train_list[n]);
		if(bModify)
		{
			myPDoc->SendUpdateTrain(train_list[n]);
		}
	}

	// 2. 区间内分部运行
	train_list.clear();
	myPDoc->TrytoFindAbsPartRunTrain(&train, train_list);
	for (int n=0; n<train_list.size(); n++)
	{
		bool bModify=myPDoc->TrytoMoveOtherTrain(&train, train_list[n]);
		if(bModify)
		{
			myPDoc->SendUpdateTrain(train_list[n]);
		}
	}
}

void CTG_Mange::TrytoDeleteOtherTrain(CTG_TRAIN& train)
{
	std::vector<CTG_TRAIN> train_list;

	// 1. 区间途中折返车次
	myPDoc->TrytoFindAbsEngineReturnTrain(&train, train_list);
	for (int n=0; n<train_list.size(); n++)
	{
		myPDoc->sysprint(0,0, "主车次被删除, 需要删除区间途中折返车次");
		myPDoc->SendDeleteTrain(train_list[n]);
	}

	TgTRAIN_RECORD* pFirst=train.GetFirstRecord();
	if(pFirst==NULL)
		return;

	CTG_TRAIN* ptrain=NULL;
	if(IsAbsAttainEngineReturn(train.myTrainInfo.adjust_flag) && (pFirst!=NULL))
	{
		if(pFirst->special_train_index!=0)
		{
			ptrain=GetTrain(pFirst->special_train_index);
		}
	}
	
	if(ptrain)
	{
		CTG_TRAIN newtrain=*ptrain;
		bool bSend=false;
		for(int n=0; n<newtrain.GetRecordNum(); n++)
		{
			TgTRAIN_RECORD* prec=newtrain.GetRecordByRecIndex(n);
			TgTRAIN_RECORD* prec_next=newtrain.GetRecordByRecIndex(n+1);
			if((prec!=NULL) && (prec_next!=NULL))
			{
				if(prec->station == pFirst->station && IsFlagAttainEngine(prec->flag))
				{
					if(prec->out_station_index==pFirst->in_station_index)
					{
						ClearFlagAttainEngine(prec->flag);
						bSend=true;
					}
				}
			}
		}

		if(bSend)
		{
			myPDoc->sysprint(0,0, "删除区间途中折返车次, 修改区间途中折返主车次");
			myPDoc->SendUpdateTrain(newtrain);
		}
	}
	// 2. 区间内分部运行
	train_list.clear();
	myPDoc->TrytoFindAbsPartRunTrain(&train, train_list);
	for (int n=0; n<train_list.size(); n++)
	{
		myPDoc->sysprint(0,0, "主车次被删除, 需要删除区间内分部运行");
		myPDoc->SendDeleteTrain(train_list[n]);
	}
	
	ptrain=NULL;
	if(IsAbsPartRunTrain(train.myTrainInfo.adjust_flag) && (pFirst!=NULL))
	{
		if(pFirst->special_train_index!=0)
		{
			ptrain=GetTrain(pFirst->special_train_index);
		}
	}
	
	if(ptrain)
	{
		CTG_TRAIN newtrain=*ptrain;
		bool bSend=false;
		for(int n=0; n<newtrain.GetRecordNum(); n++)
		{
			TgTRAIN_RECORD* prec=newtrain.GetRecordByRecIndex(n);
			if(prec!=NULL)
			{
				if(prec->station == pFirst->station && IsFlagTrainPartRun(prec->flag))
				{
					if(prec->in_station_index==pFirst->out_station_index)
					{
						ClearFlagTrainPartRun(prec->flag);
						bSend=true;
					}
				}
			}
		}

		if(bSend)
		{
			myPDoc->sysprint(0,0, "删除区间内分部运行车次, 修改区间内分部运行主车次");
			myPDoc->SendUpdateTrain(newtrain);
		}
	}
}

CString CTG_Mange::CheckTrainSide(TRAIN_INDEX train_index, int station, short sideno, DWORD arrive, DWORD depart)
{
	CString str;
	vector<TrainGroupInfo> list;
	GetTrainGroupInfo(list,false);

	for(int n=0; n < list.size(); n++)
	{
		for(int m=1; m < list[n].train_count && m < MAX_CZH_TRAIN; m++)
		{
			if(list[n].trains[m-1].end_station==station && list[n].trains[m].begin_station==station &&
				list[n].trains[m-1].train_index!=train_index && list[n].trains[m].train_index!=train_index)
			{
				if(list[n].trains[m-1].end_side == list[n].trains[m].begin_side)
				{
					if(list[n].trains[m-1].end_side==sideno)
					{
						if(depart>=list[n].trains[m-1].edTime && arrive<=list[n].trains[m].stTime)
						{
							str.Format("%s站%s已被%s次和%s次作为交路折返使用", 
								myPDoc->GetStationName(station),
								myPDoc->GetSideName(station, sideno), 
								list[n].trains[m-1].trainname, list[n].trains[m].trainname);
							break;
						}
					}
				}
			}
		}
		if(str!="")
		{
			break;
		}
	}
	return str;
}

void CTG_Mange::GetPrevAndAfterTrainIndex(TRAIN_INDEX train_index, TRAIN_INDEX& prev_train_index, TRAIN_INDEX& next_train_index)
{
	prev_train_index=0;
	next_train_index=0;

	for(int i=0; i<m_nTrainGroupCount; i++)
	{
		for(int n=0; n<m_trainGroup[i].train_count; n++)
		{
			if(m_trainGroup[i].prev_train_index[n]==train_index)
			{
				next_train_index=m_trainGroup[i].this_train_index[n];
			}
			
			if(m_trainGroup[i].this_train_index[n]==train_index)
			{
				prev_train_index=m_trainGroup[i].prev_train_index[n];
			}
		}
	}
}