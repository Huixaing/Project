/*
***            Filename : HWNDs.h
	contains the interface objects used globally. for pmtg
***
 */
#ifndef WIDGET_HEADER
#define WIDGET_HEADER

#include "stdafx.h"
#include "gpcdef.h"
//#include "MacroSwitch.h"


#define TIME_CHANGE                     1
#define MINIMUMTIME                     120
#define SEPERATORBARWIDTH               4
#define MAX_STATION_PER_SECTION         128//32
#define VERTICAL_SPACE                  80 //上下两个边界和时间线的空白
//#define VERTICAL_ROUTE_SPACE			60
#define HORIZONTAL_SPACE                20
#define TIME_AXIS_HEIGHT                40

#define MAX_TRAINID_CMD_NUM 5 //车次数目
//#define STATION_AXIS_WIDTH              90

// change or insert plan operator status constant
#define MIDDLE_START		1
#define MIDDLE_STOP			2
#define ADD_PLANSTATION		4

// cursor status
#define IN_CROSS	1
#define IN_ARROW	2
#define IN_WAIT		3
#define NUM_COLUMNS		9//10

// follwing constant represent tg operator status
#define NORMAL_STATUS 0
#define INSERT_TRAIN 1
#define PURGE_TRAIN 2
#define MODIFY_TRAIN 3
#define EDIT_TRAIN  3
#define MODIFYING 4
#define EXIT 5
#define TRAIN_RENAME 6
#define INSERT_TRAIN_GOON 7
#define INSERT_TRAIN_RETURN 8

#define INSERT_TRAIN_ABS_A 9     //前行到区间
#define INSERT_TRAIN_ABS_B 10    //从区间返回

//dmis 攻关
#define RECEIVE_POINT_STATUS 50

//#define TRAIN_RECORD 7
#define ADD_BEGIN 8
#define ADD_RUN 9
#define PLAN_AUTOMODI 10
#define PLAN_MAPMODI 11
#define MICRO_ADJUST 12
#define MACRO_ADJUST	13
/* added by Huizhong Song on 10/11/1996 */
#define MIDMODIFY 14
#define EDITABSBLOCK 15
#define EDITSIDEBLOCK 16
#define EDITABSSLOWMOVE 17
#define EDITTRAINRECORD 18
#define TRAINIDRENAME 19
#define EDITTRAINATTRIBUTE 20
#define SHOWALLTRAIN 21
#define MULTI_EDIT	 22
#define MOVINGBAR	23
#define	EDIT_MID_START	24
#define EDIT_MID_STOP	25
#define LIST_EDIT		26
#define WAIT_STATUS		27
// for edit train
#define EDIT_TRAIN_RECORD 		30
#define VIEW_RUNNING_GRAPHIC 	35
#define RECEIVING_SCHEDULE_DATA	36
#define INPUT_WEATHER_STATUS	40
#define BROKEN_WITH_CAD     	42

#define  MANUAL_ADJUST 11
#define  AUTO_ADJUST 13
// item mark  ************** lyc 2001/4/24 ***********
#define  DRAW_MARK_ITEM_STATUS   41
#define  ABS_INHIBIT_MARK   1//51
#define  ABS_FIX_MARK       2//55
#define  ABS_SLOW_MARK      3//52
#define  SIDE_SLOW_MARK     4//53
#define  SIDE_INHIBIT_MARK  5//54
#define  ABS_TASK_MARK      6//56
#define  GRAPH_NOTE_MARK    7//57
#define  TEXT_NOTE_MARK     8//temporary use
#define  SELF_CLOSE_MARK    9// cad name : change_block_type
#define  SIDE_TIANCHUANG_MARK     10//
//added by Qain Guorong 2003-5-12
#define	 ABS_ELE_FIX_MARK	11	//区间电网检修
#define  SIDE_OCCUPY_MARK   12 

#define  NEW_GRAPH_NOTE_MARK   15
#define  NEW_ABS_ELE_FIX_MARK  16

//added by Qain Guorong 2003-5-12 ends here

#define UM_INSERT_TRAINVIEW		(WM_USER + 200)
#define UM_SHOW_BUTTON			(WM_USER + 201)
#define UM_HIDE_BUTTON			(WM_USER + 202)
#define UM_MODIFY_BUTTON		(WM_USER + 203)


#define UM_EDIT_BUTTON			(WM_USER + 205)
#define UM_ADD_BUTTON			(WM_USER + 206)
#define UM_CHANGE_BUTTON		(WM_USER + 207)
#define UM_CHANGEID_BUTTON		(WM_USER + 208)
#define UM_DELETE_BUTTON		(WM_USER + 209)
#define UM_START_BUTTON			(WM_USER + 210)
#define UM_STOP_BUTTON			(WM_USER + 211)
#define UM_ACK_BUTTON			(WM_USER + 212)
#define UM_CANCEL_BUTTON		(WM_USER + 213)

#define UM_SWITCH_TO_FORM		(WM_USER + 214)
#define UM_SWITCH_TO_GRAPH		(WM_USER + 215)
#define UM_CHANGE_TRAINVIEW		(WM_USER + 216)
#define UM_INFORM_SIDEINHIBIT	(WM_USER + 217)
#define UM_INFORM_ABSINHIBIT	(WM_USER + 218)
#define UM_INFORM_ABSSLOW		(WM_USER + 219)
#define UM_ABS_INHIBIT			(WM_USER + 220)
#define UM_ABS_SLOW				(WM_USER + 221)
#define UM_SIDE_INHIBIT			(WM_USER + 222)
#define UM_PLOT					(WM_USER + 223)
#define UM_WEATHER_INFORM		(WM_USER + 224)
#define UM_LATE_REASON			(WM_USER + 225)
#define UM_TRAIN_INFORM			(WM_USER + 226)
#define UM_STATION_INFORM		(WM_USER + 227)
#define UM_BASIC_AUTUMN			(WM_USER + 228)
#define UM_BASIC_SPRING			(WM_USER + 229)
#define UM_BASIC_NORMAL			(WM_USER + 230)
#define UM_BASIC_WINTER			(WM_USER + 231)
#define UM_BASIC_SUMMER			(WM_USER + 232)
#define UM_HISTORY_SCHEDULE		(WM_USER + 233)
#define UM_WORK_SCHEDULE		(WM_USER + 234)
#define UM_CURR_SHIFT			(WM_USER + 235)
#define UM_NEXT_SHIFT			(WM_USER + 236)
#define UM_LAST_SHIFT			(WM_USER + 237)
#define UM_PRINTER_ONTIME_TRAIN	(WM_USER + 238)
//#define UM_STATIONBUTTON_CHECK	(WM_USER + 239)
#define UM_SELECT_LAYOUT		(WM_USER + 240)
#define UM_SELECT_SECTION		(WM_USER + 241)
#define UM_TGMSG_LATEREASONINFO (WM_USER + 242)
#define UM_SHOW_WAITTRAIN		(WM_USER + 243)
#define UM_REDRAW_MODIFYTRAIN	(WM_USER + 244)
#define UM_CONNECT_STATUS		(WM_USER + 245)
#define UM_PRINT_ONTIME_SECTION	(WM_USER + 246)
#define UM_PRINT_ONTIME_STATION	(WM_USER + 247)
#define UM_PRINT_RUNNING_RATE		(WM_USER + 248)
#define UM_PRINT_TECHNOLOGY_SPEED	(WM_USER + 249)
#define UM_TGMSG_TIME_SYNC			(WM_USER + 250)
#define UM_TGMSG_LOCALADJUSTTRAIN   (WM_USER + 251)
#define UM_TGMSG_ADJUSTSTATUSCHANGE (WM_USER + 252)
#define UM_TGMSG_WIDEADJUSTTRAIN    (WM_USER + 253)
#define UM_TGMSG_TWOMINUTE_AUTOACK  (WM_USER + 254)
#define UM_TGMSG_TWOMINUTE_AUTOCANCEL (WM_USER + 255)
#define UM_TGMSG_CONFLICT_INFORM	(WM_USER + 256)
#define UM_TGMSG_CONFLICT_INSERT    (WM_USER + 257)
#define UM_TGMSG_HISTORY_DELETE		(WM_USER + 258)
#define UM_TGMSG_CONFLICT_PURGE    (WM_USER + 259)
#define UM_PLOT_HISTORY_ADD			(WM_USER + 260)
#define UM_MACRO_ADJUST				(WM_USER + 261)
#define UM_MICRO_ADJUST				(WM_USER + 262)
#define UM_SELECT_CONFLICT			(WM_USER + 263)
#define UM_TGMSG_CAD_FAIL			(WM_USER + 264)
#define UM_SHOW_DIRECTUP			(WM_USER + 265)
#define UM_SHOW_DIRECTDOWN			(WM_USER + 266)
#define UM_SHOW_DIRECTMIXED			(WM_USER + 267)
#define UM_SHOW_FREIGHT				(WM_USER + 268)
#define UM_SHOW_PASSENGER			(WM_USER + 269)
#define UM_SHOW_ALLTRAIN			(WM_USER + 270)
#define UM_TGMSG_SHOWPROMPT			(WM_USER + 271)
#define UM_SHIFT_COMMAND				(WM_USER+272)
#define UM_SELECT_ROUTE				(WM_USER+273)
#define UM_INPUT_TIME					(WM_USER+274)
#define UM_CLOCK_BUTTON				(WM_USER+275)
#define UM_SMALL_GRAPH				(WM_USER+276)
#define UM_WIDTH_GRAPH				(WM_USER+277)

#define UM_WINTG_DISPATCHER_COMMAND				(WM_USER + 291)
#define UM_DISPATCHER_COMMAND_EDIT_FOCUS_CHANGE  (WM_USER + 292)
#define UM_TRAIN_PRIORITY_MESSAGE				(WM_USER + 293)
#define UM_DISPATCHER_COMMAND_RECEIVE_STATUS	(WM_USER + 295)
#define UM_DISPATCHER_COMMAND_ACKNOLOGY_STATUS	(WM_USER + 296)
#define UM_QUERY_HISTORY_DISPATCHER_COMMAND		(WM_USER + 297)
#define UM_RECEIVE_STATION_ENTRY_RUN_SEQUENCE   (WM_USER + 298)
#define UM_STATION_CURRENT_STORED_TRAIN			(WM_USER + 299)
#define UM_RECEIVE_SECTION_STATION_TRAIN_LIST	(WM_USER + 300)
#define UM_DISPATCHER_COMMAND_RECEIVE_SIGNATURE_STATUS  (WM_USER + 301)
#define UM_RECEIVE_STATION_WORK_INFORM			(WM_USER + 302)
#define UM_SCHEDULE_RECEIVE_FINISHED			(WM_USER + 303)
#define UM_RECEIVE_ABS_RUNTIME_INFORM			(WM_USER + 304)
//FOR DMIS 攻关
#define UM_SCHD_INIT_TRAINID_MSG	        	(WM_USER + 305)
#define UM_MsgBasicTrainListReply               (WM_USER + 306)
#define UM_NEIGHBER_PLAN_MSG	            	(WM_USER + 307)
#define UM_STPC_PLAN_STATUS_MSG	            	(WM_USER + 309)
#define UM_STPC_PLAN_STATUS_IMP_MSG             (WM_USER + 310)
#define UM_NEIGHBER_RECV_PLAN_STATUS_MSG	    (WM_USER + 311)
#define UM_DC_EDIT_FOCUS_CHANGE					(WM_USER + 312)
#define UM_RECEIVE_DC							(WM_USER + 313)
#define UM_TRANSFER_DC							(WM_USER + 314)
#define UM_NOTEPAD                              (WM_USER + 315)    
#define UM_PASSWAY                              (WM_USER + 316)  
#define UM_PASSWAY_UTILITY                      (WM_USER + 317)    
#define UM_PASSWAY_INFO                         (WM_USER + 318)    
#define UM_PASSWAY_STATION                      (WM_USER + 319)    
#define UM_TRAIN_WORK_PLAN                      (WM_USER + 320)  
#define UM_CHANGE_PASSWORD                      (WM_USER + 321)   
#define UM_JIAOJIECHE_STATISTIC                 (WM_USER + 322)   
#define UM_DDT_CHANGED_MSG                      (WM_USER + 323) 
#define UM_TRAIN_PRIORITY_INFO                  (WM_USER + 324)   
#define UM_DISPATCH_CONSOLE_INFO                (WM_USER + 325)   
#define UM_AUTO_ADJUST                          (WM_USER + 326)  
#define UM_ADJUSTSTART_CADCONFIRM               (WM_USER + 327)  
#define UM_STATION_STATUS                       (WM_USER + 328)   
 
#define UM_USER_LIST_MSG						(WM_USER + 329)		 
#define UM_STPC_PLAN_STATUS_IMP_RETURN_MSG      (WM_USER + 330)
#define UM_STPC_REPORT_TIME_MSG					(WM_USER + 331)	    
#define UM_DC_EDIT_FREE_FOCUS_CHANGE			(WM_USER + 332)
#define UM_RECEIVE_ALL							(WM_USER + 333)
#define UM_SELECT_ONE_STATION                   (WM_USER + 334)
#define UM_SHUAIGUACHE_STATISTICS               (WM_USER + 335) 
#define UM_LONG_INCREASE_TRAIN                  (WM_USER + 336)
#define UM_LONG_STOP_TRAIN                      (WM_USER + 337)
#define UM_NEIGHBOR_RECEIVE                     (WM_USER + 338)

//added for nCTC message
#define  UM_MsgShuntNoteGpcOperEcho              (WM_USER + 339)
#define  UM_MsgShuntNoteHead                     (WM_USER + 340)
#define  UM_MsgShuntNoteGou                      (WM_USER + 341)
#define  UM_MsgShuntNoteStatus                   (WM_USER + 342)
#define  UM_MsgShuntNoteListInit                 (WM_USER + 343)
#define  UM_MsgShuntNoteListComplete             (WM_USER + 344)
#define  UM_MsgShuntNoteInit                     (WM_USER + 345)
#define  UM_MsgShuntNoteComplete                 (WM_USER + 346)
#define  UM_MsgShuntNoteRouteList                (WM_USER + 347)
#define  UM_MsgShuntNoteStoreCar                 (WM_USER + 348)
#define  UM_MsgShuntNotePicture                  (WM_USER + 349)
#define  UM_MsgShuntNoteSendOrder                (WM_USER + 350)

#define  UM_MsgRouteListResult                   (WM_USER + 351)
#define  UM_MsgRouteLongInfo                     (WM_USER + 352)
#define  UM_MsgRouteShortInfo                    (WM_USER + 353)
#define  UM_MsgRouteInfoDel                      (WM_USER + 354)
#define  UM_MsgRouteInfoMove                     (WM_USER + 355)
#define  UM_MsgRouteOperResult                   (WM_USER + 356)

#define  UM_MsgNextPlanSendInit                     (WM_USER + 357)
#define  UM_MsgNextPlanSendComplete                 (WM_USER + 358)
#define  UM_MsgNextPlanInsert                       (WM_USER + 359)
#define  UM_MsgNextPlanRecvStatus                   (WM_USER + 360)
#define  UM_MsgNextPlanSignStatus                   (WM_USER + 361)
#define  UM_MsgNextPlanRequest                      (WM_USER + 362)          
#define  UM_MsgWordPhasePlanStatus                  (WM_USER + 363)
#define  UM_MsgWordPhasePlanSend                    (WM_USER + 364)
#define  UM_MsgCommandStatus						(WM_USER + 365)
#define  UM_LOGIN_SUCCESS                           (WM_USER + 367) //成功登陆主机

 

//无线调度命令查询机车位置
#define UM_DDML_WL_ENGINELOCATION               (WM_USER +369)

//调度命令授权消息
#define UM_DDML_AUTHORITY                       (WM_USER +370)

//基本图换图
#define UM_BASIC_GRAPH_CHANGE					(WM_USER + 371)
#define UM_STPC_GUZHANG_INFO                    (WM_USER + 372)
#define UM_CMREPORT_STATUS                      (WM_USER + 373)  // 控制模式报告
#define UM_TRAININOUT                           (WM_USER + 374)  //    交接车统计
#define UM_TRAIN_PUNCTUALITY                    (WM_USER + 375)  //    列车正点率统计
#define UM_TRAIN_SPEED                          (WM_USER + 376)  //    列车正点率统计

// 限速命令确认
#define UM_TCC_LIMITSPEED_CMD_CONFIRM           (WM_USER + 380)
#define UM_TCC_LIMITSPEED_CMD_CANCEL			(WM_USER + 381)//限速命令取消

#define UM_LOCOMOTIVE_QUERY			            (WM_USER + 382)//机车信息查询

#define  UM_MsgDCmdCurrentNo                      (WM_USER + 383) // 命令号码
#define  UM_MsgDCmdTitle                          (WM_USER + 384) // 命令标题
#define  UM_MsgDCmdSendList                       (WM_USER + 385) // 命令下发目标
#define  UM_MsgDCmdData                           (WM_USER + 386) // 命令概况
#define  UM_MsgDCmdContent                        (WM_USER + 387) // 命令内容

#define  UM_MsgDCmdTcc                            (WM_USER + 388) // 列控命令
#define  UM_MsgDCmdRecv                           (WM_USER + 389) // 签收消息
  
#define UM_GPC_2_GPC_MESSAGE                    (WM_USER + 390)//调度台间消息确认传送
	   
#define  UM_MsgDCmdRefresh                       (WM_USER + 391) // 命令刷新消息
#define  UM_MsgDCmdTitleEnd                       (WM_USER + 392) // 标题结束，此消息触发刷新命令箱

#define  UM_TRANSFER_DCMD						(WM_USER + 393) // 调度命令集中到Form处理使用
#define UM_ARMS_STATUS_INFORM				(WM_USER + 400)	//电力臂设置

#endif
