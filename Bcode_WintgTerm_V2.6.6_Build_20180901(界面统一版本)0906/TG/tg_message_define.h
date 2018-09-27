#ifndef TG_MESSAGE_DEFINE_H
#define TG_MESSAGE_DEFINE_H

#include "Tg_TRAIN.h"

//---------------------DATA_MANAGE------------------------





#define MSG_ADDNEW_TRAIN_GRAPH			WM_USER + 1004 
#define MSG_ADDNEW_TRAIN_GRAPH_END		WM_USER + 1005

#define MSG_GEN_TRAIN_INDEX				WM_USER + 1006
//---------------------------------------------------------
#define MSG_SELECT_TG_SECTION			WM_USER + 1011

#define MSG_INVALIDATE_TG				WM_USER + 1012
#define MSG_TRAINS_OPERATOR				WM_USER + 1013
#define MSG_SET_CTRL_MODE				WM_USER + 1014
#define MSG_SET_STATION_STATUS			WM_USER + 1015
#define MSG_SET_PLAN_STATUS				WM_USER + 1016
#define MSG_REFRESH_TG					WM_USER + 1017
#define MSG_ALIVE_WINDOW				WM_USER + 1018

#define MSG_MENUADD_MARK_GRAPH			WM_USER + 1020
#define MSG_LOAD_DATA				    WM_USER + 1021

#define MSG_GET_PLOT_PARAM			    WM_USER + 1030
#define MSG_GET_TG_DATA			        WM_USER + 1031

#define MSG_ADDNEW_TRAIN_GRAPH_XML		WM_USER + 1032

#define MSG_SET_TRAIN_FOCUS				WM_USER + 1033
#define MSG_SET_MULTI_TRAIN_SIDELOCK	WM_USER + 1034

struct DrawTrainInfo
{
	int nSecretTrain;
	int nStartMode;
	int nEndMode;
	CString strTrainId;
	CString strArrivalTrainId;
};

#endif