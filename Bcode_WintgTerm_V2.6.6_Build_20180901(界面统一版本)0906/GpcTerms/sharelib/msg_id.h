# ifndef MSG_ID_INC
# define MSG_ID_INC


#define STANDBY_TASK_VERBOSE     0x1
#define COMM_TASK_VERBOSE        0x2
#define ROUTER_TASK_VERBOSE      0x4
#define SCHDLR_TASK_VERBOSE      0x8
#define USERS_TASK_VERBOSE       0x10
#define KEYBOARD_TASK_VERBOSE    0x20
#define TIMER_TASK_VERBOSE       0x40
#define SENDER_TASK_VERBOSE      0x80
#define EVENT_TASK_VERBOSE       0X100
#define CRT_TASK_VERBOSE         0x200
#define TERMINAL_TASK_VERBOSE    0x400
#define PRINTER_TASK_VERBOSE     0x800
#define PLOTTER_TASK_VERBOSE     0x1000
#define HISTORY_TASK_VERBOSE     0X2000


/* these are the main system task message types */
# define SYSKILL            0x00     /* immediate shut down request, no data */
# define STANDBY_MESSAGES   0x01     /* messages sent by the standby */
# define MAIN_MESSAGES      0x02     /* messages sent by main */
# define ROUTER_MESSAGES    0x03     /* messages sent by the router */
# define TRACKER_MESSAGES   0x04     /* messages sent by the tracker */
# define TIMER_MESSAGES     0x05     /* messages sent by the timer */
# define USERS_MESSAGES     0x06     /* messages sent form users task */
# define GPC_MESSAGES       0x07     /* messages from GPC process */
# define COMM_MESSAGES      0X08     /* messages from COMM process. */
# define TID_DB_MESSAGES    0x10     /* messages from TID_DB_SIGNAL_PROCESSOR */
# define HISTORY_MESSAGES   0X11     /* messages from HISTORY process. */


# define CRT_MESSAGES_MIN   0x100    /* messages form CRT process */   
# define CRT_MESSAGES_MAX   0x1ff    /* 3 bits crt add, 3 bits gpc 2 channel */
# define PRN_MESSAGES_MIN   0x200    /* messages form PRN process */   
# define PRN_MESSAGES_MAX   0x2ff    /* 3 bits prn add, 3 bits gpc 2 channel */
# define PLT_MESSAGES_MIN   0x300    /* messages form PLT process */   
# define PLT_MESSAGES_MAX   0x3ff    /* 3 bits plt add, 3 bits gpc 2 channel */
# define TRM_MESSAGES_MIN   0x400    /* messages form TRM process */   
# define TRM_MESSAGES_MAX   0x4ff    /* 3 bits trm add, 3 bits gpc 2 channel */

# define USERIF_MESSAGES      0X01    /* messages for USER process */
# define TRAIN_HIS_SIG_MESSAGES    0X02   /* messages for train his signal. */

/*----------------------------------------------------------------------------
    The following are the messages sent form the standby system 
----------------------------------------------------------------------------*/
#define STBY_BEGIN       1
#define STBY_NOTICE_ACTIVE    (STBY_BEGIN+1)      /* stby task to other tasks */
#define STBY_MT_PASS_MESS     (STBY_BEGIN+2)      /* stby task to MT task. */
#define STBY_SLAVE_NOTICE_ACT (STBY_BEGIN+3)      /* stby task to MT task. */
#define STBY_QUEUE_STATUS     (STBY_BEGIN+4)      /* stby baselevel to master baselevel. */
#define STBY_PASS_MESSAGE       0xff        /*  Passing message type for stby task */


/* the following messages are sent from the master mode thread */
#define SWITCH_OVER             1          /* main to standby, switch request. */
#define SET_STBY_TIME           2          /* main to standby, send data and time. */
#define EQ_TRAIN_ID_BEGIN       0x10       /* ip to standby */
#define EQ_TRAIN_ID_ALL         0x11
#define EQ_TRAIN_ID_END         0x12
#define EQ_TRAIN_ID_ADD         0x13
#define EQ_TRAIN_ID_SHIFT       0x14
#define EQ_TRAIN_ID_CHANGE      0x15
#define EQ_TRAIN_ID_DELETE      0x16
#define EQ_TRAIN_ID_INSERT      0x17
#define EQ_TRAIN_NUMBER         0x18

#define EQ_STA_STATUS_BEGIN     0X20
#define EQ_STA_STATUS_END       0X21
#define STBY_STA_STATUS_EQU     0X22
#define STBY_DTS_CDTS_CHANGE    0X23
#define STBY_DTS_LDTS_CHANGE    0X24

#define EQ_TRAIN_SCH_BEGIN      0X30
#define InitializeSched         (EQ_TRAIN_SCH_BEGIN+0)
#define SetTrainMasterSched     (EQ_TRAIN_SCH_BEGIN+1)
#define MasterSchedEnds         (EQ_TRAIN_SCH_BEGIN+2)
#define SetTrainWorkSched       (EQ_TRAIN_SCH_BEGIN+3)
#define SetTrainWorkSchedData   (EQ_TRAIN_SCH_BEGIN+4)
#define SetTrainInformation     (EQ_TRAIN_SCH_BEGIN+5)
#define SetTrainInformationData (EQ_TRAIN_SCH_BEGIN+6)
#define DeleteTrainSched        (EQ_TRAIN_SCH_BEGIN+7)
#define RenameTrainSched        (EQ_TRAIN_SCH_BEGIN+8)
#define START_SCHD_STBY         (EQ_TRAIN_SCH_BEGIN+9)
#define END_SCHD_STBY           (EQ_TRAIN_SCH_BEGIN+10)

#define EQ_TRAIN_HIS_BEGIN      0X40

#define EQ_ALARM_FILE_BEGIN     0X50

#define EQ_STORE_ROUTE_BEGIN    0X60

#define EQ_OTHER_BEGIN          0X70

/*----------------------------------------------------------------------------
    The following are the messages sent from the GPC
----------------------------------------------------------------------------*/
#define GPC_BEGIN               1
#define CRT_FUN_REQUEST         (GPC_BEGIN)               /* for CRT to CTC */
#define CRT_DENY_REQUEST        (CRT_FUN_REQUEST+1)      /* for CRT to CTC */
#define CRT_CALL_STATION        (CRT_DENY_REQUEST+1)     /* for CRT to CTC */
#define CRT_SET_ROUTE           (CRT_CALL_STATION+1)     /* for CRT to CTC */
#define CRT_SET_DG_ROUTE        (CRT_SET_ROUTE+1)        /* for CRT to CTC */
#define CRT_SET_PS_ROUTE        (CRT_SET_DG_ROUTE+1)        /* for CRT to CTC */
#define CRT_SET_ST_ROUTE        (CRT_SET_PS_ROUTE+1)        /* for CRT to CTC */
#define CRT_CANCEL_ROUTE        (CRT_SET_ST_ROUTE+1)     /* for CRT to CTC */
#define CRT_TEST_MODE_TOGGLE    (CRT_CANCEL_ROUTE+1)     /* for CRT to CTC */
#define CRT_RECLEAR_SIGNAL      (CRT_TEST_MODE_TOGGLE+1)  /* for CRT to CTC */
#define CRT_ZONE_CTRL_ACK       (CRT_RECLEAR_SIGNAL+1)   /* for CRT to CTC */
#define CRT_ZONE_CTRL_NACK      (CRT_ZONE_CTRL_ACK+1)    /* for CRT to CTC */
#define CRT_ZONE_CTRL_RETRV     (CRT_ZONE_CTRL_NACK+1)   /* for CRT to CTC */
#define CRT_LOCAL_CTRL_ACK      (CRT_ZONE_CTRL_RETRV+1)  /* for CRT to CTC */
#define CRT_LOCAL_CTRL_NACK     (CRT_LOCAL_CTRL_ACK+1)   /* for CRT to CTC */
#define CRT_LOCAL_CTRL_RETRV    (CRT_LOCAL_CTRL_NACK+1)  /* for CRT to CTC */
#define CRT_DEPART_ACK          (CRT_LOCAL_CTRL_RETRV+1)  /* for CRT to CTC */
#define CRT_SUP_MODE_TOGGLE     (CRT_DEPART_ACK+1)        /* for CRT to CTC */
#define CRT_STATION_SELECT      (CRT_SUP_MODE_TOGGLE+1)        /* for ctrl CRT to CTC */
#define CRT_STORE_ROUTE         (CRT_STATION_SELECT+1)   /* for ctrl CRT to CTC */
#define CRT_STORE_DG_ROUTE      (CRT_STORE_ROUTE+1)   /* for ctrl CRT to CTC */
#define CRT_REPLACE_STORE_ROUTE (CRT_STORE_DG_ROUTE+1)   /* for ctrl CRT to CTC */
#define CRT_REPLACE_STORE_DG_ROUTE (CRT_REPLACE_STORE_ROUTE+1)  /* for ctrl CRT to CTC */
#define CRT_DELETE_STORE_ROUTE  (CRT_REPLACE_STORE_DG_ROUTE+1)  /* for ctrl CRT to CTC */
#define CRT_LOGIN               (CRT_DELETE_STORE_ROUTE+1)      /* for ctrl CRT to CTC */
#define SHUNT_SWITCH_UNLOCKED   CRT_LOGIN                       /* IP to CTC */
#define CRT_LOGOUT              (CRT_LOGIN+1)                   /* for ctrl CRT to CTC */
#define CRT_TRAIN_ID_DELETE_SCHEDUAL    (CRT_LOGOUT+1)          /* for menu CRT to CTC */
#define CRT_TRAIN_ID_ADD_SCHEDUAL       (CRT_TRAIN_ID_DELETE_SCHEDUAL+1)  /* for menu CRT to CTC */
#define CRT_TRAIN_ID_CHANGE_SCHEDUAL    (CRT_TRAIN_ID_ADD_SCHEDUAL+1)      /* for menu CRT to CTC */
#define CRT_TRAIN_ID_CHANGE_HISTORY     (CRT_TRAIN_ID_CHANGE_SCHEDUAL+1)                       /* for menu CRT to CTC */
#define CRT_TRAIN_ID_ADD		(CRT_TRAIN_ID_CHANGE_HISTORY+1) 		      /* for menu CRT to CTC */
#define CRT_TRAIN_ID_CHANGE_YESTDAY	(CRT_TRAIN_ID_ADD+1)
#define CRT_STORAGE_MODE_TOGGLE         (CRT_TRAIN_ID_CHANGE_YESTDAY+1)
#define TERM_TASK_REQ			(CRT_STORAGE_MODE_TOGGLE+1)
#define TERM_TASK_ACK			(TERM_TASK_REQ+1)

#define GGPC_PORT_BEGIN 	0X50
#define GGPC_PORT_KEY           GGPC_PORT_BEGIN         /* for GGPC port to crt sub. */
#define GGPC_PORT_BUTTON        (GGPC_PORT_KEY+1)        /* for GGPC port to crt sub. */
#define DISPLAY_CHINESE         (GGPC_PORT_BUTTON+1)     /* for CRT task to GGPC port. */
#define GGPC_LOAD_SCREEN        (DISPLAY_CHINESE+1)      /* for CRT task to GGPC port. */
#define GGPC_FORK               (GGPC_LOAD_SCREEN+1)     /* for GGPC tasks to GGPC port. */
#define GGPC_STOP               (GGPC_FORK+1)            /* for GGPC tasks to GGPC port. */
#define GGPC_LINE               (GGPC_STOP+1)            /* for GGPC menu task to GGPC port. */
#define GGPC_CLEAR_AREA         (GGPC_LINE+1)            /* for GGPC menu task to GGPC port. */
#define GGPC_SELECT_SCREEN      (GGPC_CLEAR_AREA+1)      /* for GGPC crt tasks to GGPC port. */
#define GGPC_SELECT_MODE        (GGPC_SELECT_SCREEN+1)   /* for GGPC crt tasks to GGPC port. */
#define GGPC_PRN                (GGPC_SELECT_MODE+1)     /* for GGPC printer task to GGPC port. */
#define GGPC_PLT                (GGPC_PRN+1)             /* for GGPC plotter task to GGPC port. */
#define GGPC_PRN_STATUS         (GGPC_PLT+1)             /* for GGPC port to GGPC printer task. */
#define GGPC_PLT_STATUS         (GGPC_PRN_STATUS+1)      /* for GGPC port to GGPC plotter task. */
#define GGPC_ATTACH_KEY         (GGPC_PLT_STATUS+1)      /* for GGPC tasks to GGPC port. */
#define GGPC_ATTACH_MOUSE       (GGPC_ATTACH_KEY+1)      /* for GGPC tasks to GGPC port. */
#define GGPC_MENU_DISPLAY       (GGPC_ATTACH_MOUSE+1)    /* for GGPC menu task to GGPC port. */
#define GGPC_SELECT_VEDIO       (GGPC_MENU_DISPLAY+1)    /* for GGPC tasks to GGPC port. */

/* The following definitions are made by Mark Zheng 09-07-90 */

#define GGPC_INITIAL_GPC        (GGPC_SELECT_VEDIO+1)    /* From CC to GGPC */
#define GGPC_DEFINE_WINDOW      (GGPC_INITIAL_GPC+1)     /* define shift window size */
#define GGPC_DRAW_VECTOR_LINE   (GGPC_DEFINE_WINDOW+1)   /* update an icon */
#define GGPC_SCROLL_LEFT        (GGPC_DRAW_VECTOR_LINE+1) /* shift window lesft */
#define GGPC_SCROLL_UP          (GGPC_SCROLL_LEFT+1)
#define GGPC_MENU_CLEAR         (GGPC_SCROLL_UP+1)
#define GGPC_CLEAR_PAGE         (GGPC_MENU_CLEAR+1)      /* from CC to GGPC. */
#define GGPC_CRT_STATUS         (GGPC_CLEAR_PAGE+1)      /* from GGPC TO CC CRT tasks. */
#define GGPC_PRN_END            (GGPC_CRT_STATUS+1)      /* from GGPC to CC, CC to GGPC. */
#define GGPC_PLOT_END           (GGPC_PRN_END+1)        /* from GGPC to CC, CC to GGPC. */
#define GGPC_XOR_LINE           (GGPC_PLOT_END+1)
#define GGPC_XOR_TEXT           (GGPC_XOR_LINE+1)         /* x6d */
#define GGPC_DISP_MSG_ECHO      (GGPC_XOR_TEXT+1)        /* x6e */
#define GGPC_DISP_MSG_NACK      (GGPC_DISP_MSG_ECHO+1)   /* x6f */
#define GGPC_SPECIAL_LINE       (GGPC_DISP_MSG_NACK+1)   /* x70 */
#define GGPC_MOUSE_LOCATE       (GGPC_SPECIAL_LINE+1)    /* x71 */
#define GGPC_MENU_KEY           (GGPC_MOUSE_LOCATE+1)    /* 0x72 */
#define GGPC_RESET_ICON         (GGPC_MENU_KEY+1)       /* for IP to CRT. */
/*---------------TGCRT---------------------------*/
#define GGPC_TGCRT_FORK         (GGPC_RESET_ICON+1)
#define GGPC_TGCRT_STOP         (GGPC_TGCRT_FORK+1)
#define GGPC_TIMER_START        (GGPC_TGCRT_STOP+1)
#define GGPC_TIMER_STOP         (GGPC_TIMER_START+1)
#define GGPC_MOUSE_HANDLE       (GGPC_TIMER_STOP+1)
#define GGPC_MOUSE_LOCATE_XY    (GGPC_MOUSE_HANDLE+1)
#define GGPC_TGCRT_DATA         (GGPC_MOUSE_LOCATE_XY+1)
#define GGPC_TGCRT_RESPONSE     (GGPC_TGCRT_DATA+1)
#define GGPC_TGCRT_KB_TEXT      (GGPC_TGCRT_RESPONSE+1)
#define GGPC_TGCRT_RENAME_TD    (GGPC_TGCRT_KB_TEXT+1)
#define GGPC_EDIT_TRAIN         (GGPC_TGCRT_RENAME_TD+1)
#define GGPC_BEEP               (GGPC_EDIT_TRAIN+1)
/*---------------TGCRT---------------------------*/
#define GGPC_TERM_MODE          (GGPC_BEEP+1)    /* From cc to ggpc term task. */       /* 0x7d */
#define GGPC_TERM_MODE_REQ      (GGPC_TERM_MODE+1)        /* From ggpc term task to cc. */
#define GGPC_TERM_STATUS        (GGPC_TERM_MODE_REQ+1)    /* From ggpc term task to cc. */
#define GGPC_TRAIN_INFO_GET     (GGPC_TERM_STATUS+1)      /* From ggpc term task to cc. */   /* 0x80 */
#define GGPC_TRAIN_INFO         (GGPC_TRAIN_INFO_GET+1)   /* From cc to ggpc term task. */
#define GGPC_TRAIN_INFO_UPDATE  (GGPC_TRAIN_INFO+1)       /* From ggpc term task to cc. */
#define GGPC_TRAIN_INFO_LIST    (GGPC_TRAIN_INFO_UPDATE+1) /* From ggpc term task to cc. */
#define GGPC_TRAIN_SCH_GET      (GGPC_TRAIN_INFO_LIST+1)  /* From ggpc term task to cc. */
#define GGPC_TRAIN_SCH_DATA     (GGPC_TRAIN_SCH_GET+1)    /* From cc to ggpc term task. */
#define GGPC_TRAIN_SCH_UPDATE   (GGPC_TRAIN_SCH_DATA+1)   /* From ggpc term task to cc. */
#define GGPC_TRAIN_SCH_ID_GET   (GGPC_TRAIN_SCH_UPDATE+1) /* From ggpc term task to cc. */
#define GGPC_TRAIN_ID_LIST      (GGPC_TRAIN_SCH_ID_GET+1) /* From cc to ggpc term task. */   /* 0x88 */

#define GGPC_SOUND		(GGPC_TRAIN_ID_LIST+1)	  /* From cc to ggpc port. */
#define GGPC_CHINESE_WINDOW     (GGPC_TRAIN_ID_LIST+2)    /* From cc to ggpc port. */
#define GGPC_INSERT_TRAIN       (GGPC_TRAIN_ID_LIST+3)
#define GGPC_DELETE_TRAIN       (GGPC_TRAIN_ID_LIST+4)
#define GGPC_OCCUPY_SAME_SCREEN  (GGPC_TRAIN_ID_LIST+5)

#define GGPC_STATION_INACTIVE   0xF0                       /* from cc to Terminal Task */
#define GGPC_STATION_ACTIVE     (GGPC_STATION_INACTIVE+1)  /* from cc to Terminal Task */
#define GGPC_BIT_BLOCK_REPORT   (GGPC_STATION_ACTIVE+1)    /* from cc to Terminal Task */
#define GGPC_BIT_STATUS_CHANGE  (GGPC_BIT_BLOCK_REPORT+1)  /* from cc to Terminal Task */
#define GGPC_SECTION_NAME       (GGPC_BIT_STATUS_CHANGE+1) /* from cc to Terminal Task */
#define GGPC_STATION_NUM        (GGPC_SECTION_NAME+1)      /* from cc to Terminal Task */
#define GGPC_STATION_ID         (GGPC_STATION_NUM+1)       /* from cc to Terminal Task */
#define GGPC_STATION_NAME       (GGPC_STATION_ID+1)        /* from cc to Terminal Task */
#define GGPC_TERM_CONFIG        (GGPC_STATION_NAME+1)      /* from cc to Terminal Task */
#define GGPC_TERM_TRAIN         (GGPC_TERM_CONFIG+1)       /* from cc to Terminal Task */
#define GGPC_TERM_MONITOR       (GGPC_TERM_TRAIN+1)        /* from cc to Terminal Task */

#define CHANGE_ONE_SCHED_DATA   (GGPC_TERM_MONITOR+1)      /* 0xFB */
#define DELETE_ONE_SCHED_DATA   (CHANGE_ONE_SCHED_DATA+1)  /* 0xFC */
#define RENAME_ONE_SCHED_DATA   (DELETE_ONE_SCHED_DATA+1)  /* 0xFD */

/*---------------------------------------------------------------------------
    The following are messages for communication between CAD and BaseLevel.
 *--------------------------------------------------------------------------*/
#define  DEST_EVENT     0X39
  #define  INIT_STATION_BEGIN     0X20
  #define  INIT_STATION_END       0X21
  #define  INIT_STA_STATUS        0X22
  #define  INIT_INDI_BITBLOCK     0X23
  #define  STATION_STATUS_TO_CAD  0X24            /*for temp*/
  #define  INDI_BITBLOCK_TO_CAD   0X25            /* for temp*/
  #define  INDI_CHANGE_TO_CAD     0X26


#define  DEST_SCH       0X35
#define  DEST_ROUTE     0X36

#define	 DEST_HISTORY	0X37
  #define HIS_REGISTER		0X21
  #define RELEASE_HIS_REGISTER	0X22
  #define START_HIS_OPERATION	0X23
  #define END_HIS_OPERATION	0X24
  #define READ_HIS_RECORD	0X25
//  #define CHANGE_TRAIN_ID	0X26
  #define PURGE_TRAIN_RECORD    0X27
  #define GET_TRAIN_LAST_RECORD  0X28

  #define CAD_GET_HIS_INIT       0X30
  #define CAD_GET_HIS_LAST_INIT 0X31
  #define HIS_DATA_BLOCK        0X32
  #define HIS_INIT_BEGIN        0X33
  #define HIS_INIT_END          0X34
  #define HIS_DATA_RECORD       0X35
  #define HIS_SEND_RECORD	0X36
  #define HIS_RECORD_FINISH	0X37

#define  DEST_COMM      0X38

#define  CAD_DEST_CAD_TASK         0X31

#define CAD_INIT_DONE              0X40
#define  CAD_DEST_IP_TASK          0x32

#define  CAD_PASS_TIME             0X0F

/* COMM message from 0x1000 to 0x10ff */
#define  COMM_NOTICE_BL_UP     0X1000
#define  COMM_NOTICE_BL_DOWN   0X1001

/*-------------------------------------------------------------------------*
		db_ids and task_ids for db_operation
 *-------------------------------------------------------------------------*/
#define TRN_HIS_DB    1
#define TLE_DB        2
#define TRN_ID_DB     3
#define SCHD_DB       4
#define EVNT_DB       5
#define SCHD_TASK     1
#define MENU_TASK     2
#define BL_TASK       3

/*-------------------------------------------------------------------------*
		     input signals for trn_id_db
 *-------------------------------------------------------------------------*/
#define BL_TID_INIT_BEG           0
#define BL_TID_INPUT              1
#define BL_TID_INIT_END           2
#define BL_TID_CREATE             3
#define BL_TID_IN_DEAD_STA        4
#define BL_TID_SHIFT              5
#define BL_TID_EXT_SHIFT          6
#define BL_TID_DEL                7
#define BL_TID_CLEAR              8
#define TIMER_TID_SHIFT           9
#define MENU_TID_BINS            10
#define MENU_TID_ENTRY_INPUT     11
#define MENU_TID_ENTRY_CHANGE    12
#define MENU_TID_AINS            13
#define MENU_TID_ENTRY_DEL       14
#define MENU_TID_CONFIRM         15
#define MENU_TID_SIDE_INPUT      16
#define MENU_TID_SIDE_DEL        17
#define MENU_TID_CHANGE          18
#define MENU_TID_DEL             19
#define MENU_TID_SHIFT           20
#define SCHD_TID_IDENTIFY        21
#define SCHD_TID_CHANGE          22
#define SCHD_TID_INPUT           23

/*-------------------------------------------------------------------------*
		  output signals from trn_id_db to bl
 *-------------------------------------------------------------------------*/
#define TRNDB_TID_TIME_SHIFT          0x30
#define TRNDB_TID_MENU_BINS           0x31
#define TRNDB_TID_MENU_AINS           0x32
#define TRNDB_TID_MENU_CHANGE         0x33
#define TRNDB_TID_DEL                 0x34
#define TRNDB_TID_MENU_CONFIRM        0x35
#define TRNDB_TID_MENU_INPUT          0x36
#define TRNDB_TID_MENU_SHIFT          0x37
#define TRNDB_TID_SCHD_IDENTIFY       0x38
#define TRNDB_TID_SCHD_CHANGE         0x39
#define TRNDB_TID_SCHD_AINS           0x3a

/*-------------------------------------------------------------------------*
		  output signals from trn_id_db to menu
 *-------------------------------------------------------------------------*/
#define TRNDB_TID_WIN_CHANGE    0xff

/*-------------------------------------------------------------------------*
		  output signals from trn_id_db to schd
 *-------------------------------------------------------------------------*/
#define TRNDB_TID_REAL       1
#define TRNDB_TID_DEAD       2
#define TRNDB_TID_CHANGE     3
#define TRNDB_TID_CANCEL     4
#define TRNDB_TID_CONFIRM    5

/*---------------------------------------------------------------------------
  The following message is from TIMER_TASK to ROUTER_TASK.
  -------------------------------------------------------------------------*/
#define SCH_SET_ROUTE          1

#endif
