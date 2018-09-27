
/* messages.h */

#ifndef __MESSAGE_H__
#define __MESSAGE_H__
// database error code
#define TABLEISNOTEXISTING		-204
#define LOSINGCONNECT			-900
#define CONNECTISNOTEXISTING		-1024
#define FORCECOMMAND			-1224
#define TCPIPERROR			-30081

// for all message code interchanged between view and document only
#define HORIZONTAL_POSITION_CHANGE      	2
#define SCHEDULE_STATUS_CHANGE          	4
#define SEPERATORBAR_POSITION_CHANGE    	5
#define OPERATOR_STATUS_CHANGE          	6
#define ADD_TRAINID_MESSAGE		        	7
#define DELETE_TRAINID_MESSAGE		     	8
#define CHANGE_TRAINID_MESSAGE			    9
#define CHANGE_TRAIN_MESSAGE			    10
#define FOCUS_TRAIN_CHANGE			        11
#define TRAIN_RECORD_CHANGE			        12
#define VIEWFORMAT_CHANGE			        13	
#define FOCUS_SECTION_CHANGE			    14
#define FOCUS_PAGE_CHANGE			        20
#define UM_CHANGE_CONNECT_STATUS	     	21
#define UM_CHANGE_STATION_STATUS	     	23
#define REDRAW_MESSAGE					    24
#define RECT_REDRAW_MESSAGE                 25

#define VERTICAL_POSITION_CHANGE_WORK       	3
#define VERTICAL_POSITION_CHANGE_BASIC        	30
#define VERTICAL_POSITION_CHANGE_HIST        	31
#define VERTICAL_POSITION_CHANGE        	32
#define BASIC_TIME_ARRANGE_CHANGE           33
#define NEIGHBER_TIME_ARRANGE_CHANGE		34
#define NEIGHBER_SECTION_CHANGE				35

// TRAIN STOP REASON
#define SCHD_STOP_PLAN				3
#define SCHD_STOP_LOWPRIORITY			1
#define SCHD_STOP_WORK				2
#define SCHD_STOP_UNKNOWN			0

/*---------------------------------------------------------------------------
			mouse position definitions
---------------------------------------------------------------------------*/

#define ARRIVAL_POSITION        1
#define DEPART_POSITION		    2
#define UNION_POSITION          3
#define ABS_ARRIVAL_POSITION    4
#define ABS_DEPART_POSITION     5
// train run direction
#define UP_DIRECT                       	0
#define DOWN_DIRECT                     	1
#define DUAL_DIRECT                         2

//add train method
#define ADD_TRAIN_FREE                       	1
#define ADD_TRAIN_ROUTE                        	2
#define ADD_TRAIN_GRAPHIC                       3

// train run flag
#define		SCHD_START				1
#define     SCHD_ACCEPT             2
#define		SCHD_TERMINAL			3
#define     SCHD_GIVEOUT            4
#define		SCHD_PASS				5
#define		SCHD_STOP				6

 
#define		STOP_ABS				10
#define		STOP_SIGNAL				11

 
#define USERID_OK                0x00
#define USERID_NOT_EXIST         0x01
#define USERID_CONSOLE_DISMATCH  0x02
#define CONSOLE_ALREADY_ONLINE   0x03
 

#define BREAK_TIME				1140
//color constant
#define	CLR_DARKBLUE		RGB(0,0,250)
#define CLR_WHITE		RGB(255,255,255)	//RGB(255,255,255)
#define CLR_BLACK		RGB(0,0,0)
#define CLR_DARKBROWN	RGB(24,114,96)
#define CLR_PALEGRAY		RGB(192,192,192)
#define CLR_GRAY		RGB(127,127,127)
#define CLR_DARKGRAY		RGB(96,96,96)
#define CLR_RED			RGB(255,0,0)
#define CLR_ORANGE		RGB(255,128,0)
#define CLR_INDIANRED   	RGB(255,128,128)
#define CLR_GREEN		RGB(0,255,0)
#define CLR_DARKGREEN		RGB(0,100,0)
#define CLR_SPRINGGREEN 	RGB(0,255,128)
#define CLR_LAWNGREEN		RGB(0,200,0)
#define CLR_LIGHTRED		RGB(255,128,128)
#define CLR_LIGHTGREEN  	RGB(128,255,0)
#define CLR_PRINT          RGB(50,255,50)

#define CLR_BLUE		RGB(0,0,255)
#define CLR_CYAN		RGB(0,255,255)
#define	CLR_DARKCYAN		RGB(127,255,255)
#define	CLR_DARKPINK		RGB(255,20,147)
#define	CLR_PINK		RGB(255,127,80)
#define CLR_LIGHTCYAN		RGB(128,255,255)
#define CLR_DARKYELLOW		RGB(255,255,0)
#define CLR_YELLOW		RGB(255,255,0)
#define CLR_MAGENTA		RGB(255,0,255)
#define CLR_PURPLE		RGB(165,62,204 )
#define CLR_SKYBLUE		RGB(126,192,238)
#define CLR_NAVY		RGB(0,0,128)
#define CLR_GOLD		RGB(255,215,0)
#define CLR_BROWN		RGB(165,42,42)

#define CLR_LAST_PLAN_REGION   RGB(206,206,206)
#define CLR_CLOSE_UP_REGION    RGB(226,226,226)
#define CLR_ADJUST_REGION      RGB(240,240,240)

#define	NORMAL_TRAIN		0
#define CONFLICT_SELECT		1
#define CONFLICT_UNSELECT	2

#if 0
/* following constant for menu operations from GPC to IP */
//#define MANUAL_OPERATION                     0xab70
#define EDIT_TRAIN_LINE_UP      0x38        /* from GPC to IP */
	#define EDIT_INFORM              0x01
	#define EDIT_FINISH              0x02

#define SHIFT_TRAIN             0x39        /* from GPC to IP */
	#define AbsToSta                 0x01
	#define StaToAbs                 0x02
    	#define StaToSta                 0x03

#define INPUT_TRAIN_ID          0x3a        /* from GPC to IP */
    	#define InputTrainInStation      0x01
	#define InputTrainInAbs          0x02

#define RENAME_TRAIN_ID         0x3b        /* from GPC to IP */
#define DELETE_TRAIN_ID         0x3c
#define INPUT_TRAIN_ID_SIM      0x3d        /* from GPC to IP */
#define DELETE_TRAIN_ID_SIM     0x3e

/* above constant for menu operations from GPC to IP */
#endif
/* following constant for CTC operation commands from the workstation */
#define CRT_BEGIN               0X80
#define STATION_CALL            0x82            /* for CRT to CTC */
//#define ROUTE_SET               0x83            /* for CRT to CTC */
#define DG_ROUTE_SET            0x84            /* for CRT to CTC */
#define PASSING_ROUTE_SET       0x85            /* for CRT to CTC */
#define SHUNT_RT_SET            0x86            /* for CRT to CTC */
#define ROUTE_CANCEL            0x87            /* for CRT to CTC */
#define ENTER_TEST_ROUTE        0x88            /* for CRT to CTC */
#define SIGNAL_RECLEAR          0x89            /* for CRT to CTC */
#define ZONE_CTL_ACK            0x8a            /* for CRT to CTC */
#define ZONE_CTL_INHIBIT        0x8b            /* for CRT to CTC */
#define ZONE_CTL_RELEASE        0x8c            /* for CRT to CTC */
#define LOCAL_CTL_ACK           0x8d            /* for CRT to CTC */
#define LOCAL_CTL_INHIBIT       0x8e            /* for CRT to CTC */
#define LOCAL_CTL_RELEASE       0x8f            /* for CRT to CTC */
#define DEPART_PERMIT           0x90            /* for CRT to CTC */
#define SUP_STORED_ROUTE        0x91            /* for CRT to CTC */
#define STATION_SELECTION       0x92            /* for CRT to CTC */
#define ROUTE_STORING           0x93            /* for CRT to CTC */
#define DG_ROUTE_STORING        0x94            /* for CRT to CTC */
#define PASS_ROUTE_INSERT       0x95            /* for CRT to CTC */
#define REPLACE_STORED_ROUTE    0x96            /* for CRT to CTC */
#define REPLACE_STORED_DG_ROUTE 0x97            /* for CRT to CTC */
#define PASS_ROUTE_REPLACE      0x98            /* for CRT to CTC */
#define DELETE_STORED_ROUTE     0x99            /* for CRT to CTC */
#define ENTER_ROUTE_STORING     0x9a            /* for CRT to CTC */


#define    TEST_MODE                       0x9b
#define    EXIT_ROUTE_STORING              0x9c
#define    DEL_ALL_STO_RT                  0x9d
#define    EXIT_TEST_ROUTE                 0x9e
#define    UNSUP_STORED_ROUTE              0x9f
#define    AUTO_PASSING_TOGGLE             0xA0
#define    CAD_MODE_TOGGLE                 0xA1
#define     CANCEL_DEPART_PERMIT   0xA2

//#define    MESSAGE_DATA                       0x11
//#define    MESSAGE_BEGIN                      0x01
//#define    MESSAGE_END                        0x10

#define    SIDE_OCCUPANCY_MESSAGE             0x20
#define    ABS_OCCUPANCY_MESSAGE              0x21

#define    SIDE_BLOCK_MESSAGE                 0x20
#define    ABS_BLOCK_MESSAGE                  0x21


#define    BASIC_MESSAGE                      0x0038

#define    LINKAGE_ACTIVE                     0x0048
#define    UNDESIRED_STATIONS                 0x0051
#define    LOAD_PRINT_SCHEDULE                0x0064
#define    LOAD_STATION_BLOCK                 0x0066
#define    LOAD_STATION_OCCUPANCY             0x0067

#define    MENU_RESPONSE                      0x0075
     #define    DOWN_LOAD_LINEUP_TRAINS       0x00
     #define    TRAIN_ID_MOVE_IN              0x01
     #define    TRAIN_ID_MOVE_OUT             0x02
#define    SCHD_TRAIN_CHANGE_INFORM           0x0085
#define    SCHD_TRAIN_RENAME_INFORM           0x0092
#define    HISTORY_TRAIN_RENAME_INFORM        0x0096

#define    TERMINATE_REQUEST                  0x00b0
#define    LOGIN_OUT_REQUEST                  0x00c0
#define    STATION_CONTROL_REQUEST            0x00d0
#define    TIME_MESSAGE_REQUEST               0x00d5
//#define    ALARM_DISPLAY                      0x0114


#define    PRINT_DATA                         0x0160

/***** electric affair message : BEGIN *****/
#define    LOAD_INDICATION                    0x0800
#define    UNLOAD_INDICATION                  0x0810
#define    INDICATION_INIT                    0x0820
#define    INDICATION_CHANGE                  0x0830
#define    STATION_ALARM                      0x0840
/***** electric affair message : END *****/

/********** command for background window: BEGIN*****/
#define UM_SET_TITLE						0x1841
#define UM_SHOW_WINDOW						0x1842
#define UM_HIDE_WINDOW						0x1843
/********** command for background window: END*****/

/************* command for report task ************/
#define SCHEDULE_BEGIN				0xab11
#define SCHEDULE_DATA				0xab12
#define SCHEDULE_END				0xab13
#define REPORT_COMMAND				0xab14
#define SECTION_PUNCTUALITY			0x00000001
#define STATION_PUNCTUALITY			0x00000002
#define TRAIN_RUNNING_AVERAGE_SPEED	0x00000004
#define TRAIN_TECHNOLOGY_AVERAGE_SPEED	0x00000008
/************* command for report task: END************/


// stop reason constant

/*----msg from tg to layout-----*/
 
  #define  SEND_CTC_CONTROL_ERROR           1
  #define  SEND_MENU_CONTROL_ERROR          2
  #define  HISTORY_RENAME_TRAIN_ERROR       3
  #define  HISTORY_RECORD_TRAIN_ERROR       4
  #define  HISTORY_RECORD_DELETE_ERROR      5
  #define  ERROR_SCHD_TYPE                  6
  #define  ERROR_STATION_COUNT              7
  #define  ERROR_SCHD_INSERT                8
  #define  ERROR_SCHD_DEL                   9
  #define  ERROR_SCHD_CHANGE               10
  #define  ERROR_SCHD_OPERATION            11
  #define  SCHD_RENAME_TRAIN_ERROR         12
  #define  R_GET_SIDE_BLOCK_ERROR          13
  #define  SET_SIDE_BLOCK_ERROR            14
  #define  R_GET_ABS_BLOCK_ERROR           15
  #define  SET_ABS_BLOCK_ERROR             16
  #define  R_GET_ABS_SLOWMOVE_ERROR        17
  #define  SET_ABS_SLOWMOVE_ERROR          18
  #define  TRAIN_STOP_IN_MID_ERROR         19
  #define  TRAIN_START_IN_MID_ERROR        20

#define    INIT_PMTG_MESSAGE                  0xF001
//#define    INIT_LAYOUT_MESSAGE                0xF002
#define    CONFLICT_RECORD_SELECT             0xF003
#define    QUERY_CONFLICT_TRAIN               0xF004
#define    UNSELECT_CONFLICT_TRAIN            0xF005
#define	   INIT_PLOT_MESSAGE				  0xF006
//#define    CLOSE_LAYOUT_MESSAGE			      0xf007
#define	   INIT_REPORT_MESSAGE			      0xf008

#define    CLOSE_PMTG_MESSAGE                  0x06c3
#define    SELECT_BASIC_GRAPHIC_MESSAGE        0x06c4
 

#define			SET_WEATHER_INFO				0xb00b
#define			R_GET_STATION_INFO				0xb00c
#define			SET_STATION_INFO				0xb00d
#define			SET_TRAIN_ATTRIBUTE				0xb010
#define			SET_TRAIN_MARSHAL					0xb011
#define			R_GET_LATEREASON_INFO			0xb012
#define			SET_LATEREASON_INFO				0xb013
#define			PLOT_GET_LATEREASON_INFO		0xb014

#define         START_PRINT_JOB                 0xB102
 
#define    LOAD_STPC_REPORT_DATA              0x069a
#define    SET_STPC_REPORT_DATA               0x069b
#define    DELETE_STPC_REPORT_DATA            0x069c


#define    STANDBY_MESSAGE       0x00
#define    PMTG_MESSAGE          0x01
#define    LAYOUT_MESSAGE        0x02
#define    PRINT_MESSAGE         0x03
#define    PLOT_MESSAGE          0x04
#define    AFFAIR_MESSAGE        0x05
#define    DISPATCH_MESSAGE        0x06


//sarah-map
#define	   SCREEN_MESSAGE		 0x10

/**
* following constant for defining conflict element type 
*/
#define CAD_STA_ENTRY                 0
#define CAD_STA_SIDE                  1
#define CAD_STA_EXIT                  2
#define CAD_STA_ABS                   3
#define CAD_STA_THROAT                4
#define CAD_STA_SPECIAL_ENTRY         5
#define CAD_STA_SIDE_PROHIBIT         6
#define CAD_STA_ABS_PROHIBIT          7
#define CAD_STA_SIDE_SUPER_WIDTH      8
#define CAD_STA_ABS_SUPER_WIDTH       9
#define CAD_STA_SIDE_START_DIFFICULT  10
#define CAD_STA_ABS_MEET_PROHIBIT     11
#define CAD_STA_SIDE_LENGTH           12
/**
* above constant for defining conflict element type 
*/


#define R_LOAD_RUNNING_VARIABLE    0xA000
#define A_LOAD_RUNNING_VARIABLE    0xA001

	#define PROLONG_KM_TABLE           0x01
	#define TRACK_VARIABLE_TABLE       0x02
	#define DIFF_STATION_NAME_TABLE    0x03
	#define RUNNING_LEVEL              0x04
	#define RUNNING_INTERVAL_TIME_SLOT 0x05
	#define RUNNING_TIME_SLOT          0x06

#define R_UPDATE_RUNNING_VARIABLE  0xA002
#define A_UPDATE_RUNNING_VARIABLE  0xA003

#define R_LOAD_SCHD_CONTENT        0xA004
#define A_LOAD_SCHD_CONTENT        0xA005

	#define  PASSENGE_DELAY      0x01
	#define  DELETE_TRAIN        0x02
	#define  CHANGE_TRAIN        0x03
	#define  START_FROM_MID      0x04
	#define  SUPER_LONG          0x05
	#define  OVER_LIMITATION     0x06
	#define  PRIORITY_CHANGE     0x07
	#define  SIDE_SPECIFIC       0x08
	#define  SIDE_INHIBIT        0x09
	#define  ABS_INHIBIT         0x0A
	#define  ABS_LIMIT_SPEED     0x0B
	#define  ADD_TRAIN           0x0C
	#define  STOP_FROM_MID       0x0D

#define R_UPDATE_SCHD_CONTENT			0xA006
#define A_UPDATE_SCHD_CONTERT			0xA007

#define COPY_BASIC_SCHEDULE				0xA008


#define RELOAD_BASIC_SCHEDULE			0xA00C
#define COMBINE_TABLE_AND_SCHEDULE		0xA00D


// added by song
#define		REPORT_WEATHER_INFORM				0x05a0
#define		R_GET_LATE_TRAIN_INFORM				0x05a1
#define		A_GET_LATE_TRAIN_INFORM				0x05a2
#define		A_UPDATE_LATE_TRAIN_INFORM			0x05a3
 

#define    CAD_STOP_AUTO_ADJUST      0
#define    CAD_START_SMALL_ADJUST    1
#define    CAD_START_WIDE_ADJUST     2

#define    CAD_AUTO_CONFIRM             0
#define    CAD_AUTO_REFUSE              1
#define    CAD_AUTO_ADJUST_NO_SOLUTION  2
#define    CAD_HOST_BUSY                3

#define    CAD_GPC_CONFIRM                  0
#define    CAD_GPC_REFUSE                   1

/**
*	above constant for defining cad status
*/

#define ZBARWIDTH                4
#define TRAIN_ID_LEN  10


#define    MAX_INIT_NUM                       12
  #define  ILOAD_ICON                         0
  #define  ILOAD_EICON                        1 
  #define  ILOAD_SCHD                         5
  #define  ILOAD_HSTY                         6
  #define  ILOAD_CONFLICT_DB                  7
  #define  ILOAD_CONFLICTS                    8
  #define  ILOAD_15_MINUTES                   9
  #define  ILOAD_STATION_OCCU                 10
 
 
#define    SIDE_OCCUPANCY_MESSAGE             0x20
#define    ABS_OCCUPANCY_MESSAGE              0x21

#define    SIDE_BLOCK_MESSAGE                 0x20
#define    ABS_BLOCK_MESSAGE                  0x21

/**
*	following constant defines message source or message destine
*/
#define    LOGIN_MESSAGE                      0x00
#define    PMTG_MESSAGE                       0x01
#define    LAYOUT_MESSAGE                     0x02
#define    PRINT_MESSAGE                      0x03
#define    PLOT_MESSAGE                       0x04
#define    AFFAIR_MESSAGE                     0x05
#define    TIME_MESSAGE                       0x06
#define	   BITDISP_MESSAGE					  0X07
//#define	   DIGITALMAP_2_LAYOUT				  0x0209
#define    LAYOUT_2_DIGITALMAP				  0x020A

/**
*	above constant defines message source or message destine
*/


/* The following messages defined for LOGIN */
#define    LOGIN_MESSAGE_BEGIN                0x0100
#define    LOGIN_MESSAGE_END                  0x010f
////#define    LOG_REQUEST                        0x0100

	#define      LOGIN                              0x00
	#define      LOGOUT                             0x01
	#define      AUTHORITY                          0x02

 
#define  LOGIN_OK                           0x03
  
#define CHANGE_PASSWORD        0x04      
 
/* The following messages defined for TIME_MESSAGE */
#define    TIME_MESSAGE_BEGIN                 0x0110
#define    TIME_MESSAGE_END                   0x011f
//#define    TIME_SYNC                          0x0110
#define    SHUT_DOWN_MESSAGE                  0x0111
/* The following messages defined for LAYOUT */
#define    LAYOUT_MESSAGE_BEGIN               0x0200
#define    LAYOUT_MESSAGE_END                 0x02ff
#define    ICON_MESSAGE                       0x0200
#define    INIT_LOAD_ICON                     0x0201
//#define    ICON_RESET                         0x0202
//#define    ICON_DISPLAY                       0x0203
#define    ICON_STATION_CLOSED                0x0204
//#define    ICON_INIT                          0x0205
#define    TEXT_DISPLAY                       0x0206
#define    TEXT_INIT                          0x0207
 
#define    PRINT_MESSAGE_BEGIN                0x0300
#define    PRINT_MESSAGE_END                  0x03ff

/* The following messages defined for PLOT */
#define    PLOT_MESSAGE_BEGIN                 0x0400
#define    PLOT_MESSAGE_END                   0x04ff

/* The following messages defined for AFFAIR */
#define    AFFAIR_MESSAGE_BEGIN               0x0500
#define    AFFAIR_MESSAGE_END                 0x05ff 
#define    LOAD_MONITOR_INDICATION            0x0500
#define    MONITOR_INDICATION_INIT            0x0501
#define    MONITOR_INDICATION_CHANGE          0x0502
#define    INIT_LOAD_EICON                    0x0503
 
#define	   EICON_STATION_CLOSED		  0x0507
#define    AFFAIR_CONTINUE_REPLAY	  0x0516
#define    AFFAIR_BEGIN_REPLAY		  0x0517
#define    AFFAIR_STOP_REPLAY		  0x0518
#define    AFFAIR_PAUSE_REPLAY		  0x0519
#define    AFFAIR_SPEED_REPLAY		  0x0520
#define    AFFAIR_SLOW_REPLAY		  0x0521
 
/* The following messages defined for PMTG */
#define    PMTG_MESSAGE_BEGIN                 0x0600
#define    PMTG_MESSAGE_END                   0x06ff

//#define    SCHEDULE_MESSAGE                   0x0600
#define    HISTORY_MESSAGE                    0x0601
#define    PLOT_SCHEDULE_MESSAGE              0x0601
#define    CADMAP_MESSAGE                     0x0602
#define    CAD_DSPT_MESSAGE                   0x0603
#define    CONFLICT_DB_MESSAGE                0x0604
#define    ALARM_MESSAGE                      0x0605

//#define    LOAD_SCHEDULE                      0x0607
#define    LOAD_HISTORY                       0x0608
#define    LOAD_STATION_SIDE_BLOCK            0x0609
#define    LOAD_STATION_ABS_BLOCK             0x060a
#define    LOAD_STATION_ABS_SLOWMOVE          0x060b
#define    LOAD_STATION_SIDE_OCCUPANCY        0x060c
#define    LOAD_STATION_ABS_OCCUPANCY         0x060d
//DMIS CAPTURE;
#define    SET_TG_MARK_ITEM_INFO              0x060e

#define    SET_STATION_SIDE_BLOCK             0x060e
#define    SET_STATION_ABS_BLOCK              0x060f
#define    SET_STATION_ABS_SLOWMOVE           0x0610

//#define    QUERY_TRAIN_ASM_INFO               0x060f   //wjh ,甩挂作业完成情况查询
#define    SEND_TRAIN_ASM_INFO                0x0610

#define    STATION_OCCUPANCY_MESSAGE          0x0611
#define    STATION_BLOCK_MESSAGE              0x0612

#define    TRAIN_START_FROM_STATION           0x0613
#define    TRAIN_ARRIVE_OR_DEPART             0x0614
#define    TRAIN_ROUTE_TO_STATION             0x0615

#define    TRAIN_SCHEDULE_OPERATION           0x0616
#define    TRAIN_STOP_IN_MID                  0x0617
#define    TRAIN_START_IN_MID                 0x0618
//#define    TRAIN_SCHD_CHANGE_INFORM           0x0619
#define    SCHD_RENAME_TRAIN                  0x061a
#define    SCHD_RENAME_TRAIN_INFORM           0x061b
#define    HISTORY_RENAME_TRAIN               0x061c
#define    HISTORY_RENAME_TRAIN_INFORM        0x061d
#define    HISTORY_RECORD_EDIT                0x061e
#define    HISTORY_RECORD_TRAIN               0x061f
#define    HISTORY_RECORD_DELETE              0x0620
#define    HISTORY_RECORD_TRAIN_INFORM        0x0621
#define    HISTORY_RECORD_DELETE_INFORM       0x0622
#define    HISTORY_PURGE_DATA                 0x0623              

#define    STATION_SCHEDULE_OPREATION         0x0630
//#define    CTC_CONTROL_MESSAGE                0x0631
#define    GPC_TASK_MESSAGE                   0x0632
//#define    DATA_FILE_LOAD                     0x0633

//#define    DEFINE_BASIC_SCHEDULE              0x0640

#define    R_LOAD_TRAIN_INFORMATION           0x0641
#define    A_LOAD_TRAIN_INFORMATION           0x0642
#define    R_LOAD_SPECIFIC_TRAIN_INFO         0x0643
#define    UPDATE_SPECIFIC_TRAIN_INFO         0x0644

#define    R_GET_SIDE_BLOCK                   0x0645
#define    A_GET_SIDE_BLOCK                   0x0646
#define    SET_SIDE_BLOCK                     0x0647

#define    R_GET_ABS_BLOCK                    0x0648
#define    A_GET_ABS_BLOCK                    0x0649
#define    SET_ABS_BLOCK                      0x064a


#define    R_GET_ABS_SLOWMOVE                 0x064b
#define    A_GET_ABS_SLOWMOVE                 0x064c
#define    SET_ABS_SLOWMOVE                   0x064d
 
#define	   CHANGE_BASIC_SCHEDULE_TYPE	      0x0655
//#define    GPC_AUTO_ADJUST_INFORM             0x0650
#define    LOCAL_ADJUST_DONE                  0x0651
#define    WIDE_ADJUST_DONE                   0x0652
#define    GPC_AUTO_ADJUST_CONFIRM            0x0653
//#define    LOCAL_ADJUST_AUTO_CONFIRM          0x0654
#define    WIDE_ADJUST_AUTO_CONFIRM           0x0655
#define    CADMAP_TRAIN_CONFLICT_INFORM       0x0656
#define    CADMAP_TRAIN_CONFLICT_HAPPEN       0x0657
#define    CADMAP_TRAIN_CONFLICT_DISAPPEAR    0x0658
#define    CADMAP_TRAIN_DELAY_INFORM          0x0659
#define    CADMAP_STATUS_CHANGE               0x065a
//// #define    PLOT_RUNNING_GRAPH                 0x065b

////#define    TRAIN_SCHEDULE_DELETE_INFORM	      0x065c
////#define    TRAIN_SCHEDULE_CHANGE_INFORM       0x065d
////#define    TRAIN_SCHEDULE_INSERT_INFORM	      0x065e
////#define    TRAIN_SCHEDULE_TURNREALITY_INFORM  0x065f
//#define    ADJUST_SINGLE_TRAIN_INFORM         0x0654


#define    R_REQUEST_TRAIN_LATEREASON	  0x0660
#define    R_GET_TRAIN_LATEREASON	      0x0661
#define    R_SET_TRAIN_LATEREASON	      0x0662

#define    R_REQUEST_TRAIN_MARSHAL	      0x0664
#define    R_GET_TRAIN_MARSHAL		      0x0665
#define    R_SET_TRAIN_MARSHAL		      0x0666

//#define    R_REQUEST_WEATHER_INFO	      0x0668
#define    R_GET_WEATHER_INFO	      	  0x0669
//#define    R_SET_WEATHER_INFO	      	  0x066a

#define    R_REQUEST_STATION_INFO	      0x066c
#define    R_GET_STATION_INFORM		      0x066d
#define    R_SET_STATION_INFORM		      0x066e

#define    R_REQUEST_SPECIAL_TRAIN_INFORM     0x0671
#define    R_UPDATE_SPECIAL_TRAIN_INFORM      0x0672
#define    R_GET_SPECIAL_TRAIN_INFORM	      0x0673
#define    TRAIN_MIDDLE_CHANGE		          0x0675

//SIDE,ABS AND GRAPH MARK MESSAGE
#define    STATION_SIDE_BLOCK_MESSAGE	      0x0680
#define    STATION_ABS_BLOCK_MESSAGE	      0x0681
#define    STATION_ABS_SLOWMOVE_MESSAGE	      0x0682
#define    STATION_SIDE_SLOWMOVE_MESSAGE	  0x0683
#define    STATION_TEXT_NODE_MESSAGE	      0x0684
//END
 ////#define    LOAD_INTERVAL_TIME                 0x0687
////#define    SET_INTERVAL_TIME                  0x0688
////#define    LOAD_ABS_RUN_TIME                  0x0689
////#define    SET_ABS_RUN_TIME                   0x068a

////#define    LOAD_TRAIN_PRIO                    0x068b
#define    SET_TRAIN_LOCK                     0x068c
#define    LOAD_TRAIN_LOCK                    0x068d

 
#define    GIVE_ACCIDENT                                 0x06b1
 
#define    STATION_ABS_TASK_MESSAGE  	      0x06c5
#define    INIT_SCHD_TRAIN_MESSAGE            0x0690
#define    R_LOAD_TRAIN_MARSHAL				  0x0691

#define S_MANUAL_SELECT_STOPRUN_TRAIN			0x06a6
#define SET_SINGLE_STATION_RECORD				0x06ae
#define MANUAL_ACK_TRAINID_ALARM				0x06ad

////#define     ADD_GPC_USER                                 0x06a5
////#define     DEL_GPC_USER                                 0x06a6
////#define     MODIFY_GPC_USER                              0x06a7
////#define     REQUEST_GPC_USER                             0x06a8 
#define     SEND_GPC_USER                                0x06a9

#define     QUERY_DISPATCH_COMMAND                       0x06aa
#define     SEND_DISPATCH_STATUS                         0x06ab
#define     INSERT_DISPATCH_COMMAND                      0x06ac
#define     LOAD_DISPATCH_HISTORY                        0x06ad
 
#define     STATION_CARS_INFO                            0x0699

 
#define    GET_STATION_TRAIN_LIST                        0x06af
#define    SET_STATION_TRAIN_LIST                        0x06b0
#define    SEND_STATION_TRAIN_LIST                       0x06b1
#define    GET_STATION_ENTRY_TRAIN_LIST                  0x06b2
#define    SET_STATION_ENTRY_TRAIN_LIST                  0x06b3
#define    SEND_STATION_ENTRY_TRAIN_LIST                 0x06b4
#define    GET_TRAIN_CLASS                               0x06b5
#define    MODIFY_TRAIN_CLASS                            0x06b6
#define    SEND_TRAIN_CLASS                              0x06b7
#define    SEND_BASIC_SCHEDULE_TRAIN_PLAN                0x06b8
#define    MANUAL_CHOOSE_TRAIN_PLAN                      0x06b9
#define    REQUEST_BASIC_SCHEDULE_TRAIN_PLAN             0x06ba
#define    REQUEST_HISTORY_SCHEDULE_TRAIN_PLAN             0x06c7
#define    SEND_HISTORY_SCHEDULE_TRAIN_PLAN             0x06c8
 #define    CLOSE_PMTG_MESSAGE                  0x06c3
#define    SELECT_BASIC_GRAPHIC_MESSAGE        0x06c4
////#define    STPC_REPORT_MANUAL_PROC             0x06c9
////#define    STPC_REPORT_MANUAL_REQUEST          0x06ca
#define    SEND_TRAIN_WORK                               0x06bd
#define    REPLACE_TRAIN_PLAN                            0x06be
#define    REQUEST_ABS_RUN_TIME							 0x06c0

  
#define    DEL_SUBAO                                     0x06d5
  

////#define  QUERY_WORD_PHASE_PLAN            0x06e9
////#define  SEND_WORD_PHASE_PLAN             0x06ea
#define  GIVE_WORD_PHASE_PLAN             0x06eb
#define  RECV_WORD_PHASE_RECEIVE_STATUS   0x06ec
#define NULL_SEND_TIME         2500 
 /********************************************************************/
/* The following messages defined for adjust train priority and dispatch console info*/
/* add by cf														*/
/********************************************************************/
////#define    LOAD_ADJUST_TRAIN_LIST                         0x0609
////#define    SET_ADJUST_TRAIN_LIST                          0x060A
////#define    LOAD_DISPATCH_CONSOLE_INFO                     0x060B
#define    SET_DISPATCH_CONSOLE_INFO                      0x060C
/*******************************************************************/
/* above add by cf                                                 */
/******************************************************************/


/**********************added by Wang Jinhai***********************/
//GPC->CAD
////#define    QUERY_NOTEBOOK                                0x06d1
////#define    QUERY_SUBAO                                   0x06d2
////#define    SET_NOTEBOOK                                  0x06d3
////#define    SET_SUBAO                                     0x06d4
////#define    DEL_SUBAO                                     0x06d5
//CAD->GPC
////#define    GIVE_NOTEBOOK                                 0x06d6
////#define    GIVE_SUBAO                                    0x06d7
////#define    LOAD_PASSWAY_UTILITY                          0x06da
////#define    LOAD_PASSWAY_INFO	                       	 0x06db
////#define    LOAD_PASSWAY_STATION	                         0x06dc
////#define    SET_PASSWAY_UTILITY	                         0x06dd
////#define    DEL_PASSWAY				                     0x06de
////#define    SET_PASSWAY			                         0x06df
////#define    DEL_PASSWAY_UTILITY                              0x06e0

/**
*	above constant for define pmtg operation error status
*/
////#define SEND_DISPATCH_COMMAND			0x06cb//用于gpc发调度命令至车站或gpc
////#define TRANSFER_DISPATCH_COMMAND		0x06cd//用于gpc接收从另一个gpc发来的信息，需要自己签收的命令
////#define SEND_DISPATCH_RECEIVE_STATUS	0x06ce//用于gpc接收签收调度命令
////#define RECV_DISPATCH_RECEIVE_STATUS	0x06cf
#define WIRELESS_SEND_DISPATCH_COMMAND   0x06c6 //GPC发送调度命令给机车
#define WIRELESS_QUERY_TRAIN_POS   0x0624 //GPC向服务器查询机车位置信息
#define WIRELESS_ANSWER_TRAIN_POS  0x0625 //服务器回复GPC列车的位置
#define DISPATCH_COMMAND_AUTHORITY 0x0626//向服务器查询某用户的权限信息
////#define CMD_TEMPLATE_FILE				0x06e1
////#define CMD_TEMPLATE_FILE_UPDATED		0x06e2
///#define    SET_DDY_NAME                                  0x06e5
///#define    GET_DDY_NAME                                  0x06e6
///#define    GIVE_DDY_NAME                                 0x06e7
 
/* Message for Train in and out info */
////#define    QUERY_TRAININOUT                               0x06e8
////#define    QUERY_TRAIN_PUNCTUALITY                        0x0694
////#define	   QUERY_TRAIN_SPEED                             0x0695
#define    ADD_HAT_TRAIN	5
#define    SET_DISPATCH_CONSOLE_INFO                      0x060C
 

/*******************************************************************/
/* The following messages defined for BITDISP */
/************* command for dispbitmsg task ************/
#define    BITDISP_MESSAGE_BEGIN                 0x0700
#define    BITDISP_MESSAGE_END                   0x07ff
#define	   BIT_INITIAL							 0x0700
#define	   BIT_DISPLAY							 0x0701
//#define	   R_REQUEST_BITDISP_INFO				 0x0702
#define	   DISP_INDICATION_INIT					 0x0700
#define    DISP_INDICATION_CHANGE				 0x0701


#define    DISP_LDTS_STATUS						 0x0704
#define    DISP_CDTS_STATUS			             0x0705
#define    DISP_BASELEVEL_STATUS		         0x0706

/************* command for dispbitmsg task: END************/
/* The following messages defined for DISPATCH */
#define    DISPATCH_MESSAGE_BEGIN               0x0500
#define    DISPATCH_MESSAGE_END                 0x05ff 
#define    DISPATCH_CONTINUE_REPLAY	  0x0516
#define    DISPATCH_BEGIN_REPLAY		  0x0517
#define    DISPATCH_STOP_REPLAY		  0x0518
#define    DISPATCH_PAUSE_REPLAY		  0x0519
#define    DISPATCH_SPEED_REPLAY		  0x0520
#define    DISPATCH_SLOW_REPLAY		  0x0521
/*********                                      ***********/

/**
*	following constant for define pmtg operation error status
*/
#define    PROCESS_MESSAGE_ERROR              0x06f0
  #define  ERROR_OPERATION_OK                 0
  #define  ERROR_SEND_CTC_CONTROL             1
  #define  ERROR_SEND_MENU_CONTROL            2
  #define  ERROR_HISTORY_RENAME_TRAIN         3
  #define  ERROR_HISTORY_RECORD_TRAIN         4
  #define  ERROR_HISTORY_RECORD_DELETE        5
  #define  ERROR_SCHD_TYPE                    6
  #define  ERROR_STATION_COUNT                7
  #define  ERROR_SCHD_INSERT                  8
  #define  ERROR_SCHD_DEL                     9
  #define  ERROR_SCHD_CHANGE                 10
  #define  ERROR_SCHD_OPERATION              11
  #define  ERROR_SCHD_RENAME_TRAIN           12
  #define  ERROR_R_GET_SIDE_BLOCK            13
  #define  ERROR_SET_SIDE_BLOCK              14
  #define  ERROR_R_GET_ABS_BLOCK             15
  #define  ERROR_SET_ABS_BLOCK               16
  #define  ERROR_R_GET_ABS_SLOWMOVE          17
  #define  ERROR_SET_ABS_SLOWMOVE            18
  #define  ERROR_TRAIN_STOP_IN_MID           19
  #define  ERROR_TRAIN_START_IN_MID          20

  #define  ERROR_INIT_STRUCT                 21
  #define  ERROR_OPEN_PIPE                   22
  #define  ERROR_BIND_CONNECT                23
  #define  ERROR_GPC_QUERY                   24
  #define  ERROR_INIT_GPC                    25
  #define  ERROR_GET_MAP_LOADED_MESSAGE      26

  #define  ERROR_MAP_BLOCK                   27
  #define  ERROR_MAP_READ                    28
  #define  ERROR_READING_QUEUE               29
  #define  ERROR_GET_SCHD_REGISTER           30
  #define  ERROR_GET_MAP_REGISTER            31
  #define  ERROR_GET_STATION_OCCUPANCY       32
  #define  ERROR_GET_HIS_ID                  33

  #define  ERROR_READ_CAD_DISPLAY_CHANGES    34
  #define  ERROR_PROCESS_CAD_DISPLAY_CHANGES 35
  #define  ERROR_READ_MONITOR_CHANGES        36
  #define  ERROR_READ_DISPLAY_CHANGES        37
  #define  ERROR_READ_ALARM_CHANGES          38
  #define  ERROR_READ_STBY_SEC_CHANGES       39
  #define  ERROR_PROCESS_STBY_CHANGES        40
  #define  ERROR_PROCESS_MONITOR_CHANGES     41

  #define  ERROR_LOAD_TRAIN_INFO	     42
  #define  ERROR_UPDATE_TRAIN_INFO	     43
  #define  ERROR_HISTORY_RECORD_EDIT         44

  #define  ERROR_INIT_ICON                   45
  #define  ERROR_INIT_TEXT                   46
  #define  ERROR_INIT_SCHEDULE               47
  #define  ERROR_INIT_HISTORY                48
  #define  ERROR_INIT_SHIFT                  49
  #define  ERROR_INIT_CONFLICT               50
  #define  ERROR_INIT_CONFLICT_DB            51

 
 
#define     LOAD_STATION_CARS_INFO                      0x816
#define     LOAD_SHIFT_COMMAND_TABLE                    0x817
#define     SET_SHIFT_COMMAND_TABLE_GENERAL_INFO        0x818
#define     SET_SHIFT_COMMAND_TABLE_TRAIN_INFO          0x819
#define     SEND_SHIFT_COMMAND_TABLE_INFO               0x81a
#define     REQUEST_TRAIN_PUNCTUALITY                   0x81b
#define     REQUEST_TRAIN_SPEED                         0x81c

#define     SHIFT_COMMAND_TABLE_RESPONSE                0x81d
#define     STATION_CARS_INFO_CHANGE                    0x81e
#define     SHIFT_COMMAND_MESSAGE                       0x81f
#define     SEND_BL_SHIFT_COMMAND_TABLE                 0x20
//#define     STATION_CARS_INFO                           0x821

#define   SHIFT_COMMAND_TABLE_GENERAL_INFO              0x822
#define   SHIFT_COMMAND_TABLE_TRAIN_INFO                0x823
#define     R_REPLY_TRAIN_PUNCTUALITY                   0x824
#define     R_REPLY_TRAIN_SPEED                         0x825

 
#define    RECV_STPC_MANUAL_PLAN_STATUS       0x06c5

//// #define    DEFINE_BASIC_SCHEDULE              0x0640

#define    SEND_STPC_MANUAL_PLAN              0x06c4
#define    RECV_STPC_MANUAL_PLAN_STATUS       0x06c5

////#define    INIT_LONG_INCREASE_TRAIN       0x06b6
////#define    INIT_LONG_STOP_TRAIN           0x06b5
 
#define     SEND_NEXT_CONSOLE                 0x06d8
#define     RECV_NEXT_CONSOLE                 0x06d9
 

#define UM_SET_TIME_MESSAGE			(WM_USER + 1001)
//#define UPDATE_TEXT_STATUS					0xf018

#define UM_START_DOWN_LOAD_FILE					0x0801
#define UM_STOP_DOWN_LOAD_FILE					0x0802
//#define	DOWN_LOAD_FILE_STATUS			  0x0210
#define UM_INDICATION_FILENAME			  0x0211

 
//#define SEND_DISPATCH_COMMAND			0x06cb//用于gpc发调度命令至车站或gpc
#define TRANSFER_DISPATCH_COMMAND		0x06cd//用于gpc接收从另一个gpc发来的信息，需要自己签收的命令
//#define SEND_DISPATCH_RECEIVE_STATUS	0x06ce//用于gpc接收签收调度命令
#define RECV_DISPATCH_RECEIVE_STATUS	0x06cf

//#define CMD_TEMPLATE_FILE				0x06e1
#define CMD_TEMPLATE_FILE_UPDATED		0x06e2

//// #define    QUERY_TRAIN_ASM_INFO               0x060f   //wjh ,甩挂作业完成情况查询
 
////#define UNDO_ADJUST_WORK  0x0651  //add by cf after 7-18
////#define    DEFINE_BASIC_SCHEDULE                     0x0652
////#define    QUERY_BASIC_SCHEDULE                      0x0653
////#define  TG_ENABLE_LAYOUT       0x9000
////#define  TG_DISABLE_LAYOUT    	0x9001
////#define    GPC_AUTO_ADJUST_INFORM             0x0650
////#define     UPDATE_STATION_CARS_INFO                     0x06c1
////#define     SAVE_STATION_CARS_INFO                       0x0691 ////2003.3.12
////#define     STATION_CARS_INFO                            0x0699

////#define    QUERY_ACCIDENT                                0x06b0
////#define    GIVE_ACCIDENT                                 0x06b1
////#define    ADD_ONE_ACCIDENT                              0x06b2
////#define    DEL_ONE_ACCIDENT                              0x06b3
////#define    CHANGE_ONE_ACCIDENT                           0x06b4
///#define    R_REQUEST_WEATHER_INFO	      0x0668
////#define    R_GET_WEATHER_INFO	      	  0x0669
////#define    R_SET_WEATHER_INFO	      	  0x066a
////#define    HOST_ACTIVE                        0xF200
////#define    HOST_INACTIVE                      0xF201
////#define    TIME_SYNC                          0x0110
////#define    SCHEDULE_MESSAGE                   0x0600
////#define    TRAIN_SCHD_CHANGE_INFORM           0x0619
////#define    DATA_FILE_LOAD                     0x0633
////#define    LOAD_SCHEDULE                      0x0607
#define    QUERY_TRAIN_WORK                              0x06bb
#define    SET_TRAIN_WORK                                0x06bc
#define    SEND_TRAIN_WORK                               0x06bd
////#define    REPLACE_TRAIN_PLAN                            0x06be
////#define    REQUEST_ABS_RUN_TIME							 0x06c0
////#define     LOAD_STATION_CARS_INFO                      0x816
////#define     DEL_TRAIN_WORK                               0x06ae  


const int EARLY_1 =6;   /*早时间1*/
const int EARLY_2 =7;

const int LATE_1  =18;  /*晚时间1*/
const int LATE_2  =19;
 
//sarah-map
#define UM_FENJIEKOU_STATICS_MSG	WM_USER + 100
#define  UM_CTC_LOGIN                  (WM_USER + 55)

#define UM_SHOW_LAYOUT				WM_USER + 401
#define UM_SHOW_STATION_LAYOUT		WM_USER + 402
#define UM_NOTE_CLICK	            WM_USER + 403    
#define UM_NOTE_SAVE	            WM_USER + 404    
#define UM_SECTION_CHANGE           WM_USER + 405
#define UM_NOTE_DELETE              WM_USER + 406
#define UM_NOTE_SEND                WM_USER + 407
#define UM_NOTE_PRINT               WM_USER + 408
#define UM_NOTE_QUERY               WM_USER + 409
#define UM_NOTE_ADD                 WM_USER + 410 
#define UM_NOTE_VIEW                WM_USER + 411 
#define UM_WORD_INPUT               WM_USER + 412
#define UM_GOU_INPUT                WM_USER + 413
#define UM_GOU_INPUT_ADD            WM_USER + 414
#define UM_NOTE_PROPERTY            WM_USER + 415
#define UM_NOTE_OPEN                WM_USER + 416 
//#define UM_NOTE_PRINT               WM_USER + 417 

/************************************************************************/
/* for aid dispatch process
/************************************************************************/
// load or quit dialog for aid dispatch
#define UM_INPUT_INTELLIGENT		WM_USER + 500
#define UM_NOTE_VIEW_SSC_MANANGER	WM_USER + 501
// insert, delete or update gou
#define UM_AID_GOU_APPEND			WM_USER + 502
#define UM_AID_GOU_INSERT_AT_HEAD	WM_USER + 503
#define UM_AID_GOU_INSERT			WM_USER + 504
#define UM_AID_GOU_DELETE_AT_HEAD	WM_USER + 505
#define UM_AID_GOU_DELETE			WM_USER + 506
#define UM_AID_GOU_DELETE_AT_TAIL	WM_USER + 507
#define UM_AID_GOU_UPDATE			WM_USER + 508
// update gou head
#define UM_AID_GOU_HEAD_UPDATE		WM_USER + 510
// OnLButtonDowm(Up) For Shunting
#define UM_AID_SHUNTING_ADD			WM_USER + 511
#define UM_AID_SHUNTING_MINS		WM_USER + 512
#define UM_AID_SHUNTING_CANCLE		WM_USER + 513
// LOAD PROCESS PROGRESS BAR
#define UM_INFO_PROGRESS_INIT		WM_USER + 514
#define UM_INFO_PROGRESS_SET_POS	WM_USER + 515
#define UM_INFO_PROGRESS_TOTLE		WM_USER + 516
#define UM_INFO_PROGRESS_SET_TITLE	WM_USER + 517
// station store carriage manager
#define UM_SSC_MANAGER_GET_BLOCK    WM_USER + 518
#define UM_SSC_MANAGER_LOAD			WM_USER + 519
#endif
 
