#define VER_MC2XXSTD_H "Ver.2.20"
/*************************************************************************/
#ifndef mc2xx2prtMapH
#define mc2xx2prtMapH
/*************************************************************************/
#if defined(WIN32)
#define SSC_API	__stdcall
#else
#define SSC_API
#endif

/*----------------------------------------------------------------------*/
/*	extra point table                                                   */
/*----------------------------------------------------------------------*/
typedef struct
{
/* 0000h */
	long			position;
	unsigned long	speed;
	unsigned short	actime;
	unsigned short	dctime;
	unsigned short	dwell;
	unsigned short	subcmd;

/* 0010h */
	unsigned char	oas_num[2];
	unsigned char	reserve1[2];
	unsigned char	s_curve;
	unsigned char	reserve2[3];
	unsigned char	sub_axnum[3];
	unsigned char	reserve3[5];

/* 0020h */
} PNT_DATA_EX;

/*----------------------------------------------------------------------*/
/*	other axis status table                                             */
/*----------------------------------------------------------------------*/
typedef struct
{
/* 0000h */

	unsigned long		opt_own;
	unsigned long		opt_observ;
	long				data_own;
	long				data_observ;

/* 0010h */
	unsigned char		reserve1[8];
	unsigned long long	st_axbit;

/* 0020h */
	unsigned short		st_pnt_s;
	unsigned short		st_pnt_e;
	unsigned char		reserve2[12];

/* 0030h */
	unsigned char		reserve3[40];
	unsigned char		dout_ctrl;
	unsigned char		dout_num;
	unsigned short		dout_ctrlbit;
	unsigned short		dout_data;
	unsigned char		reserve4[10];

/* 0068h */
} OAS_DATA;

/*----------------------------------------------------------------------*/
/*	press table                                                         */
/*----------------------------------------------------------------------*/
typedef struct
{
/* 0000h */
	long			switch_position;
	long			position_limit;
	long			speed_limit;
	unsigned short	target_torque;
	unsigned short	continue_time;

/* 0010h */
	unsigned short	torque_settle_width;
	unsigned short	torque_settle_time;
	unsigned short	actime;
	unsigned short	dctime;
	unsigned short	condition;
	char			reserve1[6];

/* 0020h */
} PRESS_DATA;

/*----------------------------------------------------------------------*/
/*	sampling error table                                                */
/*----------------------------------------------------------------------*/
typedef struct
{
/* 0000h */
	unsigned long long	err_ax;
	unsigned char		reserve1[8];
	unsigned long		err_dat;
	unsigned char		reserve2[4];
	unsigned long		err_bit;
	unsigned char		reserve3[4];

/* 0020h */
} SMP_ERR;

/*----------------------------------------------------------------------*/
/*	sampling table                                                      */
/*----------------------------------------------------------------------*/
#define SMP_DAT_MAX			(32)
#define SMP_BIT_MEMSIZE		(1)

typedef struct
{
/* 0000h */
	long			smpdata[SMP_DAT_MAX];

/* 0080h */
	unsigned short	smpbit[SMP_BIT_MEMSIZE];
	unsigned char	reserve1[2];

/* 0084h */
} SMP_DATA;

/*----------------------------------------------------------------------*/
/*	transient command table                                             */
/*----------------------------------------------------------------------*/
typedef struct
{
/* 0000h */
	unsigned short		cmd_req;
	unsigned short		command;
	unsigned short		req_data[4];
	unsigned short		reserve[2];

/* 0010h */
} TRANSIENT_CMD;

/*----------------------------------------------------------------------*/
/*	transient status table                                              */
/*----------------------------------------------------------------------*/
typedef struct
{
/* 0000h */
	unsigned short		status;
	unsigned short		reserve1;
	unsigned short		ans_data[4];
	unsigned short		reserve2[2];

/* 0010h */
} TRANSIENT_STS;

/*----------------------------------------------------------------------*/
/*	log table                                                           */
/*----------------------------------------------------------------------*/
#define LOG_DAT_MAX		(4)

typedef struct
{
/* 0000h */
	short	axnum;
	short	eventcode;
	long	eventtime;
	short	eventdata[LOG_DAT_MAX];

/* 0010h */
} LOG_DATA;

/*----------------------------------------------------------------------*/
/*	alarm history table                                                 */
/*----------------------------------------------------------------------*/
typedef struct
{
/* 0000h */
	long long		system_time;
	unsigned long	free_run_cnt;
	unsigned char	ctrl_cycle;
	unsigned char	event_code;
	char			reserve1[2];

/* 0010h */
	unsigned char	sscnet_type;
	unsigned char	ctrl_mode;
	char			reserve2[2];
	unsigned short	axnum;
	unsigned short	alarm_code;
	unsigned char	drive_mode;
	char			reserve3[3];
	long			cmd_pos;

/* 0020h */
	long			fb_pos;
	char			reserve4[27];
	unsigned char	check_sum;

/* 0040h */
} ALH_DATA;

/*----------------------------------------------------------------------*/
/*	interrupt callback table                                            */
/*----------------------------------------------------------------------*/
typedef struct
{
/* 0000H */
	int					board_id;						/* board_id					*/
	int					channel;						/* channnel					*/
	unsigned long		free_run_cnt;					/* free run counter			*/
	unsigned char		sys_factor_bit;					/* system factor bit		*/
	char				reserve1;
	unsigned short		sys_factor;						/* system factor			*/

/* 0010H */
	unsigned long long	axis_factor_bit;				/* axis factor bit			*/
	char				reserve2[8];
	unsigned long		axis_factor[48];				/* axis factor				*/

/* 00E0H */
	unsigned short		unit_factor_bit;				/* unit factor bit			*/
	char				reserve3[14];
	unsigned short		unit_factor[8];					/* unit factor				*/

/* 0100H */
	unsigned long		oas_factor_bit;					/* oas factor bit			*/
	char				reserve4[12];
	unsigned char		oas_factor[32];					/* oas factor				*/

/* 0130H */
	unsigned long long 	pass_factor_bit;				/* pass position factor bit	*/
	char				reserve5[8];
	unsigned char		pass_factor[64];				/* pass position factor		*/

/* 0180H */
	char				reserve6[0x180];

/* 0300H */
} INT_CB_DATA;	/* Interrupt CallBack Data */

/*************************************************************************/
#endif /* mc2xx2prtMapH */
/*************************************************************************/

/*************************************************************************/
#ifndef mc2xxFuncStdH
#define mc2xxFuncStdH
/*************************************************************************/

/*----------------------------------------------------------------------*/
/*	sscGetLastError()'s answer data                                     */
/*----------------------------------------------------------------------*/

/* default & unknown */
#define SSC_FUNC_ERR_UNKNOWN						(0xFFFFFFFF)
#define SSC_FUNC_ERR_UNSURPORT_OS					(0xFFFFFFFE)

/* argument */
#define SSC_FUNC_ERR_ARGUMENT_01					(0x00000001)
#define SSC_FUNC_ERR_ARGUMENT_02					(0x00000002)
#define SSC_FUNC_ERR_ARGUMENT_03					(0x00000003)
#define SSC_FUNC_ERR_ARGUMENT_04					(0x00000004)
#define SSC_FUNC_ERR_ARGUMENT_05					(0x00000005)
#define SSC_FUNC_ERR_ARGUMENT_06					(0x00000006)
#define SSC_FUNC_ERR_ARGUMENT_07					(0x00000007)
#define SSC_FUNC_ERR_ARGUMENT_08					(0x00000008)
#define SSC_FUNC_ERR_ARGUMENT_09					(0x00000009)
#define SSC_FUNC_ERR_ARGUMENT_MISMATCH				(0x00000100)

/* time out */
#define SSC_FUNC_ERR_TIMEOUT_01						(0x00010000)
#define SSC_FUNC_ERR_TIMEOUT_02						(0x00010100)
#define SSC_FUNC_ERR_TIMEOUT_03						(0x00010200)
#define SSC_FUNC_ERR_TIMEOUT_04						(0x00010300)
#define SSC_FUNC_ERR_TIMEOUT_05						(0x00010400)
#define SSC_FUNC_ERR_TIMEOUT_06						(0x00010500)
#define SSC_FUNC_ERR_TIMEOUT_07						(0x00010600)
#define SSC_FUNC_ERR_TIMEOUT_08						(0x00010700)
#define SSC_FUNC_ERR_TIMEOUT_09						(0x00010800)

/* general-purpose access */
#define SSC_FUNC_ERR_BOUNDARY_ADDRESS				(0x00011000)
#define SSC_FUNC_ERR_ADDRESS_RANGE_OVER				(0x00011001)

/* device open and close */
#define SSC_FUNC_ERR_REOPEN							(0x00020000)
#define SSC_FUNC_ERR_UNOPEN							(0x00020010)
#define SSC_FUNC_ERR_NOT_FOUND_BOARD				(0x00021010)
#define SSC_FUNC_ERR_GET_CHANNEL_NUM				(0x00021011)
#define SSC_FUNC_ERR_UNSUPPORT_DEVICE_DRIVER		(0x00021012)

#define SSC_FUNC_ERR_CREATE_SEMAPHORE				(0x00022000)
#define SSC_FUNC_ERR_DELETE_SEMAPHORE				(0x00022001)
#define SSC_FUNC_ERR_WAIT_SEMAPHORE					(0x00022002)
#define SSC_FUNC_ERR_RELEASE_SEMAPHORE				(0x00022003)
#define SSC_FUNC_ERR_CREATE_EVENT					(0x00022010)
#define SSC_FUNC_ERR_DELETE_EVENT					(0x00022011)
#define SSC_FUNC_ERR_RESET_EVENT					(0x00022012)
#define SSC_FUNC_ERR_SET_EVENT						(0x00022013)
#define SSC_FUNC_ERR_WAIT_EVENT						(0x00022014)
#define SSC_FUNC_ERR_WAIT_EVENT_MULTI				(0x00022015)
#define SSC_FUNC_ERR_CREATE_THREAD					(0x00022020)
#define SSC_FUNC_ERR_DELETE_THREAD					(0x00022021)
#define SSC_FUNC_ERR_THREAD_PRIORITY				(0x00022022)
#define SSC_FUNC_ERR_RESUME_THREAD					(0x00022023)
#define SSC_FUNC_ERR_GET_EXIT_CODE_THREAD			(0x00022024)
#define SSC_FUNC_ERR_CREATE_MUTEX					(0x00022040)
#define SSC_FUNC_ERR_DELETE_MUTEX					(0x00022041)
#define SSC_FUNC_ERR_DEVICE_DRIVER					(0x00023000)

/* function for system */
#define SSC_FUNC_ERR_UNREADY_CHANNEL				(0x00030000)
#define SSC_FUNC_ERR_ALREADY_CHANNEL				(0x00030010)
#define SSC_FUNC_ERR_RUNNING_CHANNEL				(0x00030020)
#define SSC_FUNC_ERR_NOW_ALARM_SYSTEM				(0x00030030)
#define SSC_FUNC_ERR_FLASHROM_PARAM_LOAD			(0x00030040)
#define SSC_FUNC_ERR_STS_BIT_FRNG					(0x00030041)
#define SSC_FUNC_ERR_STS_BIT_FSNG					(0x00030042)
#define SSC_FUNC_ERR_STS_BIT_RCE					(0x00030050)
#define SSC_FUNC_ERR_STS_BIT_CCE					(0x00030051)
#define SSC_FUNC_ERR_ALREADY_ENABLE_WDT				(0x00030060)
#define SSC_FUNC_ERR_ALREADY_DISABLE_WDT			(0x00030061)
#define SSC_FUNC_ERR_STS_BIT_IFMO					(0x00030062)
#define SSC_FUNC_ERR_UNSET_GLOBAL_VALIABLE			(0x00030100)

/* function for paramter */
#define SSC_FUNC_ERR_STS_BIT_PREN1					(0x00040000)
#define SSC_FUNC_ERR_STS_BIT_PREN2					(0x00040001)
#define SSC_FUNC_ERR_MISMATCH_PARAM_READ_NUM1		(0x00040002)
#define SSC_FUNC_ERR_MISMATCH_PARAM_READ_NUM2		(0x00040003)
#define SSC_FUNC_ERR_STS_BIT_PWEN1					(0x00040020)
#define SSC_FUNC_ERR_STS_BIT_PWEN2					(0x00040021)
#define SSC_FUNC_ERR_STS_BIT_PWED1					(0x00040022)
#define SSC_FUNC_ERR_STS_BIT_PWED2					(0x00040023)
#define SSC_FUNC_ERR_MISMATCH_PARAM_WRITE_NUM1		(0x00040024)
#define SSC_FUNC_ERR_MISMATCH_PARAM_WRITE_NUM2		(0x00040025)
#define SSC_FUNC_ERR_MISMATCH_PARAM_WRITE_DATA1		(0x00040026)
#define SSC_FUNC_ERR_MISMATCH_PARAM_WRITE_DATA2		(0x00040027)

/* function for monitor */
#define SSC_FUNC_ERR_STS_BIT_MER1					(0x00050000)
#define SSC_FUNC_ERR_STS_BIT_MER2					(0x00050001)
#define SSC_FUNC_ERR_STS_BIT_MER3					(0x00050002)
#define SSC_FUNC_ERR_STS_BIT_MER4					(0x00050003)
#define SSC_FUNC_ERR_STS_BIT_MESV					(0x00050004)
#define SSC_FUNC_ERR_ALREADY_MONITOR_STOP			(0x00050010)
#define SSC_FUNC_ERR_NOT_START_MONITOR				(0x00050011)
#define SSC_FUNC_ERR_MONITOR_2READ					(0x00050012)

/* function for axis */
#define SSC_FUNC_ERR_NOW_DRIVING					(0x00060010)
#define SSC_FUNC_ERR_NOW_DRIVING_READY				(0x00060011)
#define SSC_FUNC_ERR_NOW_ALARM_SERVO				(0x00060020)
#define SSC_FUNC_ERR_NOW_ALARM_DRIVE				(0x00060030)
#define SSC_FUNC_ERR_MISMATCH_DRIVE_MODE			(0x00060040)
#define SSC_FUNC_ERR_CHG_POS_DIR					(0x00060041)
#define SSC_FUNC_ERR_STS_BIT_PCE					(0x00060060)
#define SSC_FUNC_ERR_STS_BIT_SCE					(0x00060070)
#define SSC_FUNC_ERR_STS_BIT_TACE					(0x00060080)
#define SSC_FUNC_ERR_STS_BIT_TDCE					(0x00060090)
#define SSC_FUNC_ERR_POINT_NUMBER_OVER				(0x000600A0)
#define SSC_FUNC_ERR_STS_BIT_CTLMCE					(0x000600A1)
#define SSC_FUNC_ERR_STS_BIT_IPCH_ON				(0x000600A2)
#define SSC_FUNC_ERR_STS_BIT_IPCH_OFF				(0x000600A3)
#define SSC_FUNC_ERR_SUB_AXIS_NUM					(0x000600A4)
#define SSC_FUNC_ERR_NOT_LIP_DRIVING				(0x000600A5)

/* function for sampling */
#define SSC_FUNC_ERR_ALREADY_START_SAMPLING			(0x00061010)
#define SSC_FUNC_ERR_ALREADY_STOP_SAMPLING			(0x00061011)
#define SSC_FUNC_ERR_STS_BIT_SREN					(0x00061020)
#define SSC_FUNC_ERR_MISMATCH_SMP_PARAM_READ_NUM	(0x00061021)
#define SSC_FUNC_ERR_STS_BIT_SWEN					(0x00061022)
#define SSC_FUNC_ERR_STS_BIT_SWED					(0x00061023)
#define SSC_FUNC_ERR_MISMATCH_SMP_PARAM_WRITE_NUM	(0x00061024)
#define SSC_FUNC_ERR_MISMATCH_SMP_PARAM_WRITE_DATA	(0x00061025)

/* function for log */
#define SSC_FUNC_ERR_NOW_LOGGING					(0x00062000)
#define SSC_FUNC_ERR_STS_BIT_LOGRE					(0x00062001)
#define SSC_FUNC_ERR_STS_BIT_LOGIE					(0x00062002)
#define SSC_FUNC_ERR_ALREADY_START_LOG				(0x00062003)
#define SSC_FUNC_ERR_ALREADY_STOP_LOG				(0x00062004)

/* function for alarm history */
#define SSC_FUNC_ERR_UNSUPPORT_ALH					(0x00062005)
#define SSC_FUNC_ERR_STS_BIT_ALHRE					(0x00062006)
#define SSC_FUNC_ERR_STS_BIT_ALHIE					(0x00062007)

/* function for mark detection */
#define SSC_FUNC_ERR_MARK_DETECT_UNUSABLE			(0x00070000)
#define SSC_FUNC_ERR_MARK_DETECT_UNDETECTED			(0x00070001)

/* function for interface mode */
#define SSC_FUNC_ERR_IFM_INP_OFF					(0x000D0000)
#define SSC_FUNC_ERR_STS_BIT_ZSE					(0x000D0001)
#define SSC_FUNC_ERR_IFM_CMD_BUF_FULL				(0x000D0002)
#define SSC_FUNC_ERR_DISABLE_EVENT_DETECT			(0x000D0003)

/* function for input output device */
#define SSC_FUNC_ERR_DVI_TABLE_RANGE_OVER			(0x000E0000)
#define SSC_FUNC_ERR_DVO_TABLE_RANGE_OVER			(0x000E0001)

/* function for transient */
#define SSC_FUNC_ERR_NOW_TRANSIENT_PROCESSING		(0x000E1000)
#define SSC_FUNC_ERR_TRANSIENT_INVALID_DATA			(0x000E1001)

/* function for dump file */
#define SSC_FUNC_ERR_OPEN_DUMP_FILE					(0x000F0000)

#define GLOBAL extern

#ifdef __cplusplus
extern "C" {
#endif


GLOBAL int SSC_API sscGetLastError(void);
GLOBAL int SSC_API sscGetMountChannel(int board_id, short *mountch);

GLOBAL int SSC_API sscOpen(int board_id);
GLOBAL int SSC_API sscClose(int board_id);

GLOBAL int SSC_API sscGetControlCycle(int board_id, int channel, short *ctrl_cycle);
GLOBAL int SSC_API sscGetBoardVersion(int board_id, int channel, char *version);
GLOBAL int SSC_API sscGetDriverVersion(int board_id, int channel, int axnum, char *version);
GLOBAL int SSC_API sscGetUnitVersion(int board_id, int channel, int utnum, char *version);
GLOBAL int SSC_API sscGetOperationCycleMonitor(int board_id, int channel, short *now, short *max, short *over, char *status);
GLOBAL int SSC_API sscClearOperationCycleMonitor(int board_id, int channel);

GLOBAL int SSC_API sscResetAllParameter(int board_id, int channel, int timeout);
GLOBAL int SSC_API sscChangeParameter(int board_id, int channel, int axnum, short prmnum, short data);
GLOBAL int SSC_API sscChange2Parameter(int board_id, int channel, int axnum, short *prmnum, short *data, char *status);
GLOBAL int SSC_API sscCheckParameter(int board_id, int channel, int axnum, short prmnum, short *data);
GLOBAL int SSC_API sscCheck2Parameter(int board_id, int channel, int axnum, short *prmnum, short *data, char *status);
GLOBAL int SSC_API sscLoadAllParameterFromFlashROM(int board_id, int channel, int timeout);
GLOBAL int SSC_API sscSaveAllParameterToFlashROM(int board_id, int channel, int timeout);
GLOBAL int SSC_API sscCheckSvPrmChangeNumEx(int board_id, int channel, int axnum, short *prmnum);

GLOBAL int SSC_API sscReboot(int board_id, int channel, int timeout);
GLOBAL int SSC_API sscSystemStart(int board_id, int channel, int timeout);
GLOBAL int SSC_API sscSetSystemCommandCode(int board_id, int channel, short commandcode);
GLOBAL int SSC_API sscGetSystemStatusCode(int board_id, int channel, short *statuscode);
GLOBAL int SSC_API sscReconnectSSCNET(int board_id, int channel, unsigned long long *ctrl_axbit, unsigned short *err_code);
GLOBAL int SSC_API sscDisconnectSSCNET(int board_id, int channel, int com_num, unsigned long long *ctrl_axbit, unsigned short *err_code);

GLOBAL int SSC_API sscSetCommandBitSignalEx(int board_id, int channel, int axnum, int bitnum, int bitdata);
GLOBAL int SSC_API sscGetStatusBitSignalEx(int board_id, int channel, int axnum, int bitnum, int *bitstatus);
GLOBAL int SSC_API sscWaitStatusBitSignalEx(int board_id, int channel, int axnum, int bitnum, int waitstatus, int timeout);

GLOBAL int SSC_API sscSetPointDataEx(int board_id, int channel, int axnum, int pntnum, PNT_DATA_EX *pPntDataEx);
GLOBAL int SSC_API sscCheckPointDataEx(int board_id, int channel, int axnum, int pntnum, PNT_DATA_EX *pPntDataEx);
GLOBAL int SSC_API sscSetPointOffset(int board_id, int channel, int axnum, short offset);
GLOBAL int SSC_API sscCheckPointOffset(int board_id, int channel, int axnum, short *offset);
GLOBAL int SSC_API sscGetDrivingPointNumber(int board_id, int channel, int axnum, short *driving_pnt);
GLOBAL int SSC_API sscSetLatestPointNumber(int board_id, int channel, int axnum, short latest_point);

GLOBAL int SSC_API sscSetPressData(int board_id, int channel, int axnum, PRESS_DATA *pPressData);
GLOBAL int SSC_API sscGetPressData(int board_id, int channel, int axnum, PRESS_DATA *pPressData);

GLOBAL int SSC_API sscJogStart(int board_id, int channel, int axnum, long speed, short tca, short tcd, char dir);
GLOBAL int SSC_API sscJogStop(int board_id, int channel, int axnum);
GLOBAL int SSC_API sscJogStopNoWait(int board_id, int channel, int axnum, short *stpsts);
GLOBAL int SSC_API sscIncStart(int board_id, int channel, int axnum, long distance, long speed, short tca, short tcd);
GLOBAL int SSC_API sscAutoStart(int board_id, int channel, int axnum, int point_s, int point_e);
GLOBAL int SSC_API sscHomeReturnStart(int board_id, int channel, int axnum);
GLOBAL int SSC_API sscLinearStart(int board_id, int channel, int axnum, int grpnum, int point_s, int point_e);
GLOBAL int SSC_API sscDataSetStart(int board_id, int channel, int axnum);
GLOBAL int SSC_API sscDriveStop(int board_id, int channel, int axnum, int timeout);
GLOBAL int SSC_API sscDriveStopNoWait(int board_id, int channel, int axnum, short *stpsts);
GLOBAL int SSC_API sscDriveRapidStop(int board_id, int channel, int axnum, int timeout);
GLOBAL int SSC_API sscDriveRapidStopNoWait(int board_id, int channel, int axnum, short *stpsts);
GLOBAL int SSC_API sscSetDriveMode(int board_id, int channel, int axnum, int drv_mode);
GLOBAL int SSC_API sscGetDriveMode(int board_id, int channel, int axnum, int *drv_mode);
GLOBAL int SSC_API sscGetDriveFinStatus(int board_id, int channel, int axnum, int fin_type, int *fin_status);
GLOBAL int SSC_API sscChangeControlMode(int board_id, int channel, int axnum, unsigned short ctrl_mode);

GLOBAL int SSC_API sscChangeManualPosition(int board_id, int channel, int axnum, long position);
GLOBAL int SSC_API sscChangeAutoPosition(int board_id, int channel, int axnum, int pntnum, long position);
GLOBAL int SSC_API sscChangeLinearPosition(int board_id, int channel, int axnum, int grpnum, int pntnum, long *pPosition);
GLOBAL int SSC_API sscChangeManualSpeed(int board_id, int channel, int axnum, long speed);
GLOBAL int SSC_API sscChangeAutoSpeed(int board_id, int channel, int axnum, int pntnum, long speed);
GLOBAL int SSC_API sscChangeManualAccTime(int board_id, int channel, int axnum, short acctime);
GLOBAL int SSC_API sscChangeAutoAccTime(int board_id, int channel, int axnum, int pntnum, short acctime);
GLOBAL int SSC_API sscChangeManualDecTime(int board_id, int channel, int axnum, short dectime);
GLOBAL int SSC_API sscChangeAutoDecTime(int board_id, int channel, int axnum, int pntnum, short dectime);

GLOBAL int SSC_API sscGetAlarm(int board_id, int channel, int axnum, int alarm_type, unsigned short *code, unsigned short *detail_code);
GLOBAL int SSC_API sscResetAlarm(int board_id, int channel, int axnum, int alarm_type);

GLOBAL int SSC_API sscSetMonitor(int board_id, int channel, int axnum, short *monnum);
GLOBAL int SSC_API sscStopMonitor(int board_id, int channel, int axnum);
GLOBAL int SSC_API sscGetMonitor(int board_id, int channel, int axnum, short *monnum, short *mondata);
GLOBAL int SSC_API sscGetMonitorEx(int board_id, int channel, int axnum, short *mondata1, short *mondata2);
GLOBAL int SSC_API sscGetCurrentCmdPositionFast(int board_id, int channel, int axnum, long *position);
GLOBAL int SSC_API sscGetCurrentFbPositionFast(int board_id, int channel, int axnum, long *position);
GLOBAL int SSC_API sscGetIoStatusFast(int board_id, int channel, int axnum, short *din);
GLOBAL int SSC_API sscGetCmdSpeedFast(int board_id, int channel, int axnum, long *speed);
GLOBAL int SSC_API sscGetFbSpeedFast(int board_id, int channel, int axnum, long *speed);
GLOBAL int SSC_API sscGetCurrentFbFast(int board_id, int channel, int axnum, short *currentFb);
GLOBAL int SSC_API sscGetPositionDroopFast(int board_id,int channel,int axnum,long *position_dp);
GLOBAL int SSC_API sscGetExFastMonitor(int board_id, int channel, int axnum, short *mondata);

GLOBAL int SSC_API sscWdEnable(int board_id, int channel, unsigned short wdcnt);
GLOBAL int SSC_API sscWdDisable(int board_id, int channel);
GLOBAL int SSC_API sscChangeWdCounter(int board_id, int channel);

GLOBAL int SSC_API sscSetOtherAxisStartData(int board_id, int channel, int oas_num, OAS_DATA *pOasData);
GLOBAL int SSC_API sscGetOtherAxisStartData(int board_id, int channel, int oas_num, OAS_DATA *pOasData);
GLOBAL int SSC_API sscOtherAxisStartAbortOn(int board_id, int channel, int oas_num);
GLOBAL int SSC_API sscOtherAxisStartAbortOff(int board_id, int channel, int oas_num);
GLOBAL int SSC_API sscGetOtherAxisStartStatus(int board_id, int channel, int oas_num, short *status);

GLOBAL int SSC_API sscSetIntPassPositionData(int board_id, int channel, int pass_num, unsigned long pass_option, long pass_data);
GLOBAL int SSC_API sscCheckIntPassPositionData(int board_id, int channel, int pass_num, unsigned long *pass_option, long *pass_data);
GLOBAL int SSC_API sscSetStartingPassNumber(int board_id, int channel, int axnum, int pass_start, int pass_end);
GLOBAL int SSC_API sscGetExecutingPassNumber(int board_id, int channel, int axnum, short *executing_pass);

GLOBAL int SSC_API sscStartSampling(int board_id, int channel);
GLOBAL int SSC_API sscStopSampling(int board_id, int channel);
GLOBAL int SSC_API sscSetSamplingParameter(int board_id, int channel, short prm_num, long prm_data);
GLOBAL int SSC_API sscGetSamplingParameter(int board_id, int channel, short prm_num, long *prm_data);
GLOBAL int SSC_API sscGetSamplingError(int board_id, int channel, SMP_ERR *pSmpErr);
GLOBAL int SSC_API sscGetSamplingStatus(int board_id, int channel, char *status, short *fin_page);
GLOBAL int SSC_API sscGetSamplingData(int board_id, int channel, int page_num, short *valid_num, SMP_DATA *pSmpData);

GLOBAL int SSC_API sscStartLog(int board_id, int channel);
GLOBAL int SSC_API sscStopLog(int board_id, int channel);
GLOBAL int SSC_API sscCheckLogStatus(int board_id, int channel, int *status);
GLOBAL int SSC_API sscCheckLogEventNum(int board_id, int channel, int *eventnum);
GLOBAL int SSC_API sscReadLogData(int board_id, int channel, int page_num, LOG_DATA *pLogData);
GLOBAL int SSC_API sscClearLogData(int board_id, int channel);

GLOBAL int SSC_API sscCheckAlarmHistoryEventNum(int board_id, int channel, int *eventnum);
GLOBAL int SSC_API sscGetAlarmHistoryData(int board_id, int channel, int page_num, ALH_DATA *pAlhData);
GLOBAL int SSC_API sscClearAlarmHistoryData(int board_id, int channel);

GLOBAL int SSC_API sscGetDigitalInputDataBit(int board_id, int channel, int din_num, int *din);
GLOBAL int SSC_API sscGetDigitalInputDataWord(int board_id, int channel, int din_word_num, unsigned short *din);
GLOBAL int SSC_API sscSetDigitalOutputDataBit(int board_id, int channel, int dout_num, int dout);
GLOBAL int SSC_API sscSetDigitalOutputDataWord(int board_id, int channel, int dout_word_num, unsigned short dout);
GLOBAL int SSC_API sscGetDigitalOutputDataBit(int board_id, int channel, int dout_num, int *dout);
GLOBAL int SSC_API sscGetDigitalOutputDataWord(int board_id, int channel, int dout_word_num, unsigned short *dout);

GLOBAL int SSC_API sscSetChar(int board_id, int channel, int offset, char data);
GLOBAL int SSC_API sscSetShort(int board_id, int channel, int offset, short data);
GLOBAL int SSC_API sscSetLong(int board_id, int channel, int offset, long data);
GLOBAL int SSC_API sscSetBlock(int board_id, int channel, int offset, int size, char *data_p);

GLOBAL int SSC_API sscGetChar(int board_id, int channel, int offset, char *data_p);
GLOBAL int SSC_API sscGetShort(int board_id, int channel, int offset, short *data_p);
GLOBAL int SSC_API sscGetLong(int board_id, int channel, int offset, long *data_p);
GLOBAL int SSC_API sscGetBlock(int board_id, int channel, int offset, int size, char *data_p);

GLOBAL int SSC_API sscGetMarkDetectionData(int board_id, int channel, int axnum, int mark_num, int *read_fin_num, char *edge, long *position);
GLOBAL int SSC_API sscGetMarkDetectionCounter(int board_id, int channel, int axnum, int mark_num, int *detected_counter);
GLOBAL int SSC_API sscClearMarkDetectionData(int board_id, int channel, int axnum, int mark_num);

GLOBAL int SSC_API sscIfmGetReadErrorCount(int board_id, int channel, short *errcnt);
GLOBAL int SSC_API sscIfmSetHomePosition(int board_id, int channel, int axnum, int mode, short *param);
GLOBAL int SSC_API sscIfmGetMaximumBufferNumber(int board_id, int channel, int axnum, short *bufnum);
GLOBAL int SSC_API sscIfmGetMaximumBufferNumberEx(int board_id, int channel, int axnum, unsigned short ctrl_mode, short *bufnum);
GLOBAL int SSC_API sscIfmRenewLatestBuffer(int board_id, int channel, int axnum, long bufdata, short *bufnum);
GLOBAL int SSC_API sscIfmRenewLatestBufferEx(int board_id, int channel, int axnum, unsigned short ctrl_mode, long bufdata, short *bufnum);
GLOBAL int SSC_API sscIfmCheckLatestBuffer(int board_id, int channel, int axnum, short *bufnum, long *bufdata);
GLOBAL int SSC_API sscIfmCheckLatestBufferEx(int board_id, int channel, int axnum, unsigned short ctrl_mode, short *bufnum, long *bufdata);
GLOBAL int SSC_API sscIfmGetTransmitBuffer(int board_id, int channel, int axnum, short *bufnum, long *bufdata);
GLOBAL int SSC_API sscIfmGetTransmitBufferEx(int board_id, int channel, int axnum, unsigned short ctrl_mode, short *bufnum, long *bufdata);
GLOBAL int SSC_API sscIfmTrqSetSpeedLimit(int board_id, int channel, int axnum, long speed);
GLOBAL int SSC_API sscIfmSetControlMode(int board_id, int channel, int axnum, unsigned short ctrl_mode);
GLOBAL int SSC_API sscIfmGetControlMode(int board_id, int channel, int axnum, unsigned short *ctrl_mode, char *status);
GLOBAL int SSC_API sscIfmGetEventStatusBits(int board_id, int channel, int bitnum, unsigned long *status_bits);

GLOBAL int SSC_API sscGetInputDeviceBit(int board_id, int channel, int bit_num, int *dev_in);
GLOBAL int SSC_API sscGetInputDeviceWord(int board_id, int channel, int word_num, int word_cnt, unsigned short *dev_in);
GLOBAL int SSC_API sscSetOutputDeviceBit(int board_id, int channel, int bit_num, int dev_out);
GLOBAL int SSC_API sscSetOutputDeviceWord(int board_id, int channel, int word_num, int word_cnt, unsigned short *dev_out);
GLOBAL int SSC_API sscGetOutputDeviceBit(int board_id, int channel, int bit_num, int *dev_out);
GLOBAL int SSC_API sscGetOutputDeviceWord(int board_id, int channel, int word_num, int word_cnt, unsigned short *dev_out);

GLOBAL int SSC_API sscSendRecieveTransientData(int board_id, int channel, int axnum, TRANSIENT_CMD *pTransientCmd, TRANSIENT_STS *pTransientSts, int timeout);

GLOBAL int SSC_API sscSaveDumpFile( int board_id, char *filepath );

#ifdef __cplusplus
}	/* extern C */
#endif

#undef GLOBAL

/*************************************************************************/
#endif /* mc2xxFuncStdH */
/*************************************************************************/

/*************************************************************************/
#ifndef mc2xxFuncIntH
#define mc2xxFuncIntH
/*************************************************************************/

/*--------------------------------------------------------------------*/
/*	sscGetLastError()'s answer data                                   */
/*--------------------------------------------------------------------*/

/* function for interrupt */
#define SSC_FUNC_ERR_ALREADY_START_INT_DRIVER		(0x10000100)
#define SSC_FUNC_ERR_ALREADY_END_INT_DRIVER			(0x10000101)
#define SSC_FUNC_ERR_ALREADY_OTHER_PROCESS_INT		(0x10000102)
#define SSC_FUNC_ERR_INT_DISABLE_MASK				(0x10000103)
#define SSC_FUNC_ERR_CLEAR_INT						(0x10000104)

#define SSC_FUNC_ERR_NOT_START_INT_DRIVER			(0x10000110)
#define SSC_FUNC_ERR_TERMINATE_INT_DRIVER			(0x10000111)
#define SSC_FUNC_ERR_TERMINATE_NOTIFY_EVENT			(0x10000112)
#define SSC_FUNC_ERR_SET_HOST_APPLICATION_EVENT		(0x10000113)
#define SSC_FUNC_ERR_ALREADY_REREGISTER_CALLBACK	(0x10000200)
#define SSC_FUNC_ERR_ALREADY_UNREREGISTER_CALLBACK	(0x10000201)

#define SSC_FUNC_ERR_ALREADY_START_ISR				(0x10000300)


/*----------------------------------------------------------------------*/
#define GLOBAL extern

#ifdef __cplusplus
extern "C" {
#endif

GLOBAL int SSC_API sscIntStart(int board_id, int priority);
GLOBAL int SSC_API sscIntEnd(int board_id);

GLOBAL int SSC_API sscIntEnable(int board_id, int channel);
GLOBAL int SSC_API sscIntDisable(int board_id, int channel);

GLOBAL int SSC_API sscRegisterIntCallback(int board_id, int channel, void *cbfunc);
GLOBAL int SSC_API sscUnregisterIntCallback(int board_id, int channel);

GLOBAL int SSC_API sscResetIntEvent(int board_id, int channel, int axnum, int eventnum, int eventfactor);
GLOBAL int SSC_API sscSetIntEvent(int board_id, int channel, int axnum, int eventnum, int eventfactor);
GLOBAL int SSC_API sscWaitIntEvent(int board_id, int channel, int axnum, int eventnum, int eventfactor, int timeout);

GLOBAL int SSC_API sscResetIntEventMulti(int board_id, int channel, int axnum);
GLOBAL int SSC_API sscSetIntEventMulti(int board_id, int channel, int axnum);
GLOBAL int SSC_API sscWaitIntEventMulti(int board_id, int channel, int axnum, int timeout, unsigned long *eventcode);

GLOBAL int SSC_API sscResetIntOasEvent(int board_id, int channel,int axnum, int oas_num);
GLOBAL int SSC_API sscSetIntOasEvent(int board_id, int channel,int axnum, int oas_num);
GLOBAL int SSC_API sscWaitIntOasEvent(int board_id, int channel, int axnum, int oas_num, int oas_type, int *oas_status, int timeout);

GLOBAL int SSC_API sscResetIntPassPosition(int board_id, int channel, int pass_start, int pass_end);
GLOBAL int SSC_API sscSetIntPassPosition(int board_id, int channel, int pass_start, int pass_end);
GLOBAL int SSC_API sscWaitIntPassPosition(int board_id, int channel, int pass_num, int *pass_status, int timeout);

GLOBAL int SSC_API sscResetIntDriveFin(int board_id, int channel, int axnum);
GLOBAL int SSC_API sscSetIntDriveFin(int board_id, int channel, int axnum);
GLOBAL int SSC_API sscWaitIntDriveFin(int board_id, int channel, int axnum, int fin_type, int *fin_status, int timeout);

#ifdef __cplusplus
}	/* extern C */
#endif

#undef GLOBAL

/*************************************************************************/
#endif /* mc2xxFuncIntH */
/*************************************************************************/

/*************************************************************************/
#ifndef mc2xxFuncCompatiH
#define mc2xxFuncCompatiH
/*************************************************************************/

/*----------------------------------------------------------------------*/
/*	point table                                                         */
/*----------------------------------------------------------------------*/
typedef struct
{
/* 0000H */
	long			position;
	unsigned long	speed;
	unsigned short	actime;
	unsigned short	dctime;
	unsigned short	dwell;
	unsigned short	subcmd;

/* 0010H */
} PNT_DATA;

/*--------------------------------------------------------------------*/
/*	sscGetLastError()'s answer data                                   */
/*--------------------------------------------------------------------*/

#define SSC_FUNC_ERR_IFC_SET_MONITOR_01		(0x00040030)
#define SSC_FUNC_ERR_IFC_SET_MONITOR_02		(0x00040130)
#define SSC_FUNC_ERR_IFC_GET_MONITOR_01		(0x00040040)
#define SSC_FUNC_ERR_IFC_GET_MONITOR_02		(0x00040140)
#define SSC_FUNC_ERR_IFC_STOP_MONITOR_01	(0x00040050)
#define SSC_FUNC_ERR_IFC_STOP_MONITOR_02	(0x00040150)

#define SSC_FUNC_ERR_INT_DRIVER_START		(0x10000020)
#define SSC_FUNC_ERR_END_TYPE_UNSET			(0x10000050)


/*--------------------------------------------------------------------*/
/*	standard function (compatible)                                    */
/*--------------------------------------------------------------------*/
#define GLOBAL extern

#ifdef __cplusplus
extern "C" {
#endif

GLOBAL int SSC_API sscGetIntOutMask(int board_id, short *iom);
GLOBAL int SSC_API sscGetHWIntOut(int board_id, short *hio);

GLOBAL int SSC_API sscCheckChannelReady(int board_id, int channel);

GLOBAL int SSC_API sscSetSystemCommandBitSignal(int board_id, int channel, int offset, int bitno, int data);
GLOBAL int SSC_API sscSetAxisCommandBitSignal(int board_id, int channel, int axnum, int offset, int bitno, int data);
GLOBAL int SSC_API sscGetSystemStatusBits(int board_id, int channel, short *statusbits);
GLOBAL int SSC_API sscGetAxisStatusBits(int board_id, int channel, int axnum, short *statusbits);
GLOBAL int SSC_API sscEmgStopOn(int board_id);
GLOBAL int SSC_API sscCheckEmgStopOn(int board_id);
GLOBAL int SSC_API sscCheckEmgStopOnNoWait(int board_id, short *emionsts);
GLOBAL int SSC_API sscEmgStopOff(int board_id);
GLOBAL int SSC_API sscCheckEmgStopOff(int board_id);
GLOBAL int SSC_API sscCheckEmgStopOffNoWait(int board_id, short *emioffsts);
GLOBAL int SSC_API sscAsynchronousOn(int board_id, int channel, int grpnum);
GLOBAL int SSC_API sscCheckAsynchronousOn(int board_id, int channel, int grpnum);
GLOBAL int SSC_API sscCheckAsynchronousOnNoWait(int board_id, int channel, int grpnum, short *asonsts);
GLOBAL int SSC_API sscAsynchronousOff(int board_id, int channel, int grpnum);
GLOBAL int SSC_API sscCheckAsynchronousOff(int board_id, int channel, int grpnum);
GLOBAL int SSC_API sscCheckAsynchronousOffNoWait(int board_id, int channel, int grpnum, short *asofsts);
GLOBAL int SSC_API sscCheckSyncMatchExecOn(int board_id, int channel, int grpnum);
GLOBAL int SSC_API sscCheckSyncMatchExecOnNoWait(int board_id, int channel, int grpnum, short *smeonsts);
GLOBAL int SSC_API sscCheckSyncMatchExecOff(int board_id, int channel, int grpnum);
GLOBAL int SSC_API sscCheckSyncMatchExecOffNoWait(int board_id, int channel, int grpnum, short *smeofsts);
GLOBAL int SSC_API sscServoOn(int board_id, int channel, int axnum);
GLOBAL int SSC_API sscCheckServoOn(int board_id, int channel, int axnum);
GLOBAL int SSC_API sscCheckServoOnNoWait(int board_id, int channel, int axnum, short *svonsts);
GLOBAL int SSC_API sscServoOff(int board_id, int channel, int axnum);
GLOBAL int SSC_API sscCheckServoOff(int board_id, int channel, int axnum);
GLOBAL int SSC_API sscCheckServoOffNoWait(int board_id, int channel, int axnum, short *svofsts);
GLOBAL int SSC_API sscTorqueLimitOn(int board_id, int channel, int axnum);
GLOBAL int SSC_API sscCheckTorqueLimitCrampOn(int board_id, int channel, int axnum);
GLOBAL int SSC_API sscCheckTorqueLimitCrampOnNoWait(int board_id, int channel, int axnum, short *tconsts);
GLOBAL int SSC_API sscTorqueLimitOff(int board_id, int channel, int axnum);
GLOBAL int SSC_API sscCheckTorqueLimitCrampOff(int board_id, int channel, int axnum);
GLOBAL int SSC_API sscCheckTorqueLimitCrampOffNoWait(int board_id, int channel, int axnum, short *tcofsts);
GLOBAL int SSC_API sscCheckPositionSwitchOn(int board_id, int channel, int axnum);
GLOBAL int SSC_API sscCheckPositionSwitchOnNoWait(int board_id, int channel, int axnum, short *psonsts);
GLOBAL int SSC_API sscCheckPositionSwitchOff(int board_id, int channel, int axnum);
GLOBAL int SSC_API sscCheckPositionSwitchOffNoWait(int board_id, int channel, int axnum, short *psofsts);
GLOBAL int SSC_API sscInterlockOn(int board_id, int channel, int axnum);
GLOBAL int SSC_API sscCheckInterlockStopOn(int board_id, int channel, int axnum);
GLOBAL int SSC_API sscCheckInterlockStopOnNoWait(int board_id, int channel, int axnum, short *isonsts);
GLOBAL int SSC_API sscInterlockOff(int board_id, int channel, int axnum);
GLOBAL int SSC_API sscCheckInterlockStopOff(int board_id, int channel, int axnum);
GLOBAL int SSC_API sscCheckInterlockStopOffNoWait(int board_id, int channel, int axnum, short *isofsts);
GLOBAL int SSC_API sscCheckStopPositionOverOn(int board_id, int channel, int axnum);
GLOBAL int SSC_API sscCheckStopPositionOverOnNoWait(int board_id, int channel, int axnum, short *ovonsts);
GLOBAL int SSC_API sscCheckStopPositionOverOff(int board_id, int channel, int axnum);
GLOBAL int SSC_API sscCheckStopPositionOverOffNoWait(int board_id, int channel, int axnum, short *ovofsts);
GLOBAL int SSC_API sscGainChangeOn(int board_id, int channel, int axnum);
GLOBAL int SSC_API sscCheckGainChangeOn(int board_id, int channel, int axnum);
GLOBAL int SSC_API sscCheckGainChangeOnNoWait(int board_id, int channel, int axnum, short *gconsts);
GLOBAL int SSC_API sscGainChangeOff(int board_id, int channel, int axnum);
GLOBAL int SSC_API sscCheckGainChangeOff(int board_id, int channel, int axnum);
GLOBAL int SSC_API sscCheckGainChangeOffNoWait(int board_id, int channel, int axnum, short *gcofsts);
GLOBAL int SSC_API sscCheckTorqueLimitSelectOn(int board_id, int channel, int axnum);
GLOBAL int SSC_API sscCheckTorqueLimitSelectOnNoWait(int board_id, int channel, int axnum, short *tsonsts);
GLOBAL int SSC_API sscCheckTorqueLimitSelectOff(int board_id, int channel, int axnum);
GLOBAL int SSC_API sscCheckTorqueLimitSelectOffNoWait(int board_id, int channel, int axnum, short *tsofsts);
GLOBAL int SSC_API sscCheckInterfareWaitOn(int board_id, int channel, int axnum);
GLOBAL int SSC_API sscCheckInterfareWaitOnNoWait(int board_id, int channel, int axnum, short *iwonsts);
GLOBAL int SSC_API sscCheckInterfareWaitOff(int board_id, int channel, int axnum);
GLOBAL int SSC_API sscCheckInterfareWaitOffNoWait(int board_id, int channel, int axnum, short *iwofsts);

GLOBAL int SSC_API sscSetPointData(int board_id, int channel, int axnum, int pntnum, PNT_DATA *pPntData);
GLOBAL int SSC_API sscCheckPointData(int board_id, int channel, int axnum, int pntnum, PNT_DATA *pPntData);

GLOBAL int SSC_API sscGetControlAlarmCode(int board_id, int channel, short *code);
GLOBAL int SSC_API sscGetOperationAlarmCode(int board_id, int channel, int axnum, short *code);
GLOBAL int SSC_API sscGetExOperationAlarmCode(int board_id, int channel, int axnum, short *code, short *detail_code);
GLOBAL int SSC_API sscGetServoAlarmCode(int board_id, int channel, int axnum, short *code);

GLOBAL int SSC_API sscControlAlarmReset(int board_id, int channel);
GLOBAL int SSC_API sscOperationAlarmReset(int board_id, int channel, int axnum);
GLOBAL int SSC_API sscServoAlarmReset(int board_id, int channel, int axnum);

GLOBAL int SSC_API sscGetEmgStatus(int board_id, int channel, short *emgstatus);
GLOBAL int SSC_API sscGetAxisFixConf(int board_id, int channel, int axnum, short *code);


/*--------------------------------------------------------------------*/
/*	interrupt function (compatible)                                   */
/*--------------------------------------------------------------------*/
GLOBAL int SSC_API sscSetIntFactor(int board_id, int channel, int num, short *factorbit);
GLOBAL int SSC_API sscGetIntFactor(int board_id, int channel, int num, short *factorbit);

GLOBAL int SSC_API sscTerminateInt(int board_id, int channel);
GLOBAL int SSC_API sscClearInt(int board_id, int channel);

GLOBAL int SSC_API sscSetPtpEndType(int board_id, int channel, int axnum, short ptpendtype);
GLOBAL int SSC_API sscResetEndChk(int board_id, int channel, int axnum);
GLOBAL int SSC_API sscEndChk(int board_id, int channel, int axnum, int *endstatus);

#ifdef __cplusplus
}	/* extern C */
#endif

#undef GLOBAL

/*************************************************************************/
#endif /* mc2xxFuncCompatiH */
/*************************************************************************/

/*************************************************************************/
#ifndef mc2xxFuncUserDefH
#define mc2xxFuncUserDefH
/*************************************************************************/

#define SSC_OK								(0)
#define SSC_NG								(-1)
#define SSC_UNOPEN							(-2)

#define SSC_CTRL_CYCLE_ERROR				(0x0000)
#define SSC_CTRL_CYCLE_888					(0x0001)
#define SSC_CTRL_CYCLE_444					(0x0002)
#define SSC_CTRL_CYCLE_222					(0x0003)
#define SSC_CTRL_CYCLE_111					(0x0004)
#define SSC_CTRL_CYCLE_1000					(0x0011)
#define SSC_CTRL_CYCLE_0500					(0x0012)
#define SSC_CTRL_CYCLE_0250					(0x0013)
#define SSC_CTRL_CYCLE_0125					(0x0014)

#define SSC_BIT_OCME						(0x01)
#define SSC_BIT_OCMW						(0x02)

#define SSC_BIT_PWFIN						(0x01)
#define SSC_BIT_PWEN						(0x02)
#define SSC_BIT_PWED						(0x04)
#define SSC_BIT_PRFIN						(0x01)
#define SSC_BIT_PREN						(0x02)

#define SSC_BIT_OFF							(0)
#define SSC_BIT_ON							(1)

#define SSC_DIR_PLUS						(0)
#define SSC_DIR_MINUS						(1)

#define SSC_DRIVING							(0)
#define SSC_DRIVE_FIN						(1)

#define SSC_DRV_MODE_NONE					(0)
#define SSC_DRV_MODE_AUTO					(1)
#define SSC_DRV_MODE_HOME					(2)
#define SSC_DRV_MODE_JOG					(3)
#define SSC_DRV_MODE_INC					(4)
#define SSC_DRV_MODE_LINEAR					(6)
#define SSC_DRV_MODE_DST					(7)

#define SSC_FIN_TYPE_SMZ					(2)
#define SSC_FIN_TYPE_CPO					(3)
#define SSC_FIN_TYPE_INP					(4)

#define SSC_FIN_STS_RDY						(0)
#define SSC_FIN_STS_STP						(1)
#define SSC_FIN_STS_MOV						(2)
#define SSC_FIN_STS_ALM_STP					(3)
#define SSC_FIN_STS_ALM_MOV					(4)

#define SSC_ALARM_SYSTEM					(0)
#define SSC_ALARM_SERVO						(1)
#define SSC_ALARM_OPERATION					(2)
#define SSC_ALARM_UNIT						(3)
#define SSC_ALARM_UNIT_CTRL					(4)

#define SSC_BIT_LSP							(0x0001)
#define SSC_BIT_LSN							(0x0002)
#define SSC_BIT_DOG							(0x0004)

#define SSC_BIT_OSOP						(0x0001)
#define SSC_BIT_OSFIN						(0x0002)
#define SSC_BIT_OSERR						(0x0004)

#define SSC_BIT_SMPW						(0x01)
#define SSC_BIT_SMPO						(0x02)
#define SSC_BIT_SMPF						(0x04)
#define SSC_BIT_SMPE						(0x08)

#define SSC_LOGO_OFF						(0)
#define SSC_LOGO_ON							(1)

#define SSC_INFINITE						(0)
#define SSC_DEFAULT_TIMEOUT					(0)

#define SSC_OAS_WAIT_TYPE_NON				(0)
#define SSC_OAS_WAIT_TYPE_OP				(1)
#define SSC_OAS_WAIT_TYPE_FIN				(2)

#define SSC_OAS_STS_NON						(0)
#define SSC_OAS_STS_OP						(1)
#define SSC_OAS_STS_FIN						(2)
#define SSC_OAS_STS_ERR						(3)
#define SSC_OAS_STS_OP_ERR					(4)

#define SSC_PASS_STS_NON					(0)
#define SSC_PASS_STS_FIN					(1)
#define SSC_PASS_STS_ERR					(2)

#define CMD_BIT_SYS_MIN						(1)
#define CMD_BIT_SYS_MAX						(256)

#define STS_BIT_SYS_MIN						(257)
#define STS_BIT_SYS_MAX						(512)

#define CMD_BIT_AX_MIN						(513)
#define CMD_BIT_AX_MAX						(768)

#define STS_BIT_AX_MIN						(769)
#define STS_BIT_AX_MAX						(1024)

#define CMD_BIT_UT_MIN						(1025)
#define CMD_BIT_UT_MAX						(1152)

#define STS_BIT_UT_MIN						(1153)
#define STS_BIT_UT_MAX						(1280)

#define SSC_INT_SYS_SYSE					(0)
#define SSC_INT_SYS_CALM					(1)
#define SSC_INT_SYS_EMIO					(2)
#define SSC_INT_SYS_03						(3)
#define SSC_INT_SYS_04						(4)
#define SSC_INT_SYS_05						(5)
#define SSC_INT_SYS_06						(6)
#define SSC_INT_SYS_OCME					(7)
#define SSC_INT_SYS_OASF					(8)
#define SSC_INT_SYS_PPI						(9)
#define SSC_INT_SYS_10						(10)
#define SSC_INT_SYS_11						(11)
#define SSC_INT_SYS_12						(12)
#define SSC_INT_SYS_13						(13)
#define SSC_INT_SYS_14						(14)
#define SSC_INT_SYS_15						(15)

#define SSC_INT_AX_RDY						(0)
#define SSC_INT_AX_INP						(1)
#define SSC_INT_AX_ZSP						(2)
#define SSC_INT_AX_ZPAS						(3)
#define SSC_INT_AX_TLC						(4)
#define SSC_INT_AX_SALM						(5)
#define SSC_INT_AX_SWRN						(6)
#define SSC_INT_AX_ABSE						(7)
#define SSC_INT_AX_OP						(8)
#define SSC_INT_AX_CPO						(9)
#define SSC_INT_AX_PF						(10)
#define SSC_INT_AX_ZP						(11)
#define SSC_INT_AX_SMZ						(12)
#define SSC_INT_AX_OALM						(13)
#define SSC_INT_AX_OPF						(14)
#define SSC_INT_AX_PSW						(15)

#define SSC_INT_AX_GAINO					(16)
#define SSC_INT_AX_FCLSO					(17)
#define SSC_INT_AX_TLSO						(18)
#define SSC_INT_AX_SPC						(19)
#define SSC_INT_AX_20						(20)
#define SSC_INT_AX_MAK1						(21)
#define SSC_INT_AX_MAK2						(22)
#define SSC_INT_AX_PRSMO					(23)
#define SSC_INT_AX_IWT						(24)
#define SSC_INT_AX_SINP						(25)
#define SSC_INT_AX_26						(26)
#define SSC_INT_AX_27						(27)
#define SSC_INT_AX_28						(28)
#define SSC_INT_AX_29						(29)
#define SSC_INT_AX_30						(30)
#define SSC_INT_AX_31						(31)

#define SSC_INT_UT_00						(0)
#define SSC_INT_UT_01						(1)
#define SSC_INT_UT_02						(2)
#define SSC_INT_UT_03						(3)
#define SSC_INT_UT_04						(4)
#define SSC_INT_UT_RUALM					(5)
#define SSC_INT_UT_RUWRN					(6)
#define SSC_INT_UT_07						(7)
#define SSC_INT_UT_08						(8)
#define SSC_INT_UT_09						(9)
#define SSC_INT_UT_10						(10)
#define SSC_INT_UT_11						(11)
#define SSC_INT_UT_12						(12)
#define SSC_INT_UT_RCALM					(13)
#define SSC_INT_UT_14						(14)
#define SSC_INT_UT_15						(15)

#define SSC_EVENT_AX_RDY					(0)
#define SSC_EVENT_AX_INP					(1)
#define SSC_EVENT_AX_ZSP					(2)
#define SSC_EVENT_AX_TLC					(3)
#define SSC_EVENT_AX_SALM					(4)
#define SSC_EVENT_AX_SWRN					(5)
#define SSC_EVENT_AX_ABSE					(6)
#define SSC_EVENT_AX_OALM					(7)
#define SSC_EVENT_AX_MAK1					(8)
#define SSC_EVENT_AX_MAK2					(9)
#define SSC_EVENT_AX_10						(10)
#define SSC_EVENT_AX_11						(11)
#define SSC_EVENT_AX_12						(12)
#define SSC_EVENT_AX_LSP					(13)
#define SSC_EVENT_AX_LSN					(14)
#define SSC_EVENT_AX_DOG					(15)

#define SSC_STS_CODE_READY_FIN				(0x0001)
#define SSC_STS_CODE_RUNNING				(0x000A)

#define SSC_SUBCMD_POS_ABS					(0x0000)
#define SSC_SUBCMD_POS_INC					(0x0001)
#define SSC_SUBCMD_STOP_INP					(0x0000)
#define SSC_SUBCMD_STOP_SMZ					(0x0010)
#define SSC_SUBCMD_STOP_CONTINUE			(0x0020)
#define SSC_SUBCMD_PNT_SWITCH_AFTER			(0x0000)
#define SSC_SUBCMD_PNT_SWITCH_BEFORE		(0x0040)
#define SSC_SUBCMD_DWELL					(0x0000)
#define SSC_SUBCMD_PREDWELL					(0x0080)
#define SSC_SUBCMD_PASS_POS_DISABLE			(0x0000)
#define SSC_SUBCMD_PASS_POS_ENABLE			(0x0100)
#define SSC_SUBCMD_PRESS_DISABLE			(0x0000)
#define SSC_SUBCMD_PRESS_ENABLE				(0x0200)
#define SSC_SUBCMD_PNT_LOOP_DISABLE			(0x0000)
#define SSC_SUBCMD_PNT_LOOP_START			(0x0800)
#define SSC_SUBCMD_PNT_LOOP_END				(0x1000)

#define SSC_PRESS_START_AUTO_CMD			(0x0000)
#define SSC_PRESS_START_AUTO_FB				(0x0001)
#define SSC_PRESS_START_MANUAL				(0x0002)
#define SSC_PRESS_END_AUTO					(0x0000)
#define SSC_PRESS_END_MANUAL				(0x0010)

#define SSC_CTRL_MODE_POSITION				(0x0000)
#define SSC_CTRL_MODE_PRESS					(0x0010)

#define SSC_PCIE_DISCONNECT					(0)
#define SSC_PCIE_CONNECT					(1)

#define SSC_OAS_OWN_RESIDUAL_DISTANCE		(0x00000000)	/* compatible */
#define SSC_OAS_OWN_REMAINING_DISTANCE		(0x00000000)
#define SSC_OAS_OWN_POSITION_PASS			(0x00000001)
#define SSC_OAS_OWN_JUDGE_COORD_FB			(0x00000000)
#define SSC_OAS_OWN_JUDGE_COORD_CMD			(0x00000010)

#define SSC_OAS_OBSERV_DISABLE				(0x00000000)
#define SSC_OAS_OBSERV_ENABLE				(0x00000001)
#define SSC_OAS_OBSERV_NONE					(0x00000000)
#define SSC_OAS_OBSERV_POSITION_PASS		(0x00000010)
#define SSC_OAS_OBSERV_JUDGE_COORD_FB		(0x00000000)
#define SSC_OAS_OBSERV_JUDGE_COORD_CMD		(0x00000100)
#define SSC_OAS_OBSERV_DATA_LESS			(0x00000000)
#define SSC_OAS_OBSERV_DATA_MORE			(0x00001000)

#define SSC_OAS_DO_DISABLE					(0x00)
#define SSC_OAS_DO_ENABLE					(0x01)

#define SSC_PASS_DIR_PLUS					(0x00000000)
#define SSC_PASS_DIR_MINUS					(0x00000001)
#define SSC_PASS_JUDGE_CMD_POS				(0x00000000)
#define SSC_PASS_JUDGE_FB_POS				(0x00000010)

#define SSC_IFM_CHK_INP_WAIT				(0)
#define SSC_IFM_CHK_INP_NOWAIT				(1)
#define SSC_IFM_CTRL_MODE_POSITION			(0)
#define SSC_IFM_CTRL_MODE_SPEED				(1)
#define SSC_IFM_CTRL_MODE_TORQUE			(2)
#define SSC_IFM_CTRL_MODE_ERR_OFF			(0)
#define SSC_IFM_CTRL_MODE_ERR_ON			(1)

#define SSC_TRANSIENT_CMD_SINGLE			(1)
#define SSC_TRANSIENT_STS_WAITING			(0x0001)
#define SSC_TRANSIENT_STS_START				(0x0002)
#define SSC_TRANSIENT_STS_RECEIVING			(0x0004)
#define SSC_TRANSIENT_STS_RECEIVE_FIN		(0x0008)
#define SSC_TRANSIENT_STS_VALID_DATA		(0x8000)
#define SSC_TRANSIENT_STS_INVALID_DATA		(0x0000)

/*----------------------------------------------------------------------*/
/* constant number for user application                                 */
/*----------------------------------------------------------------------*/

/*------------------------------------------*/
/* system command bits                      */
/*------------------------------------------*/
#define SSC_CMDBIT_SYS_ITE			(CMD_BIT_SYS_MIN + 0)
#define SSC_CMDBIT_SYS_ITS			(CMD_BIT_SYS_MIN + 1)
#define SSC_CMDBIT_SYS_03			(CMD_BIT_SYS_MIN + 2)
#define SSC_CMDBIT_SYS_04			(CMD_BIT_SYS_MIN + 3)
#define SSC_CMDBIT_SYS_HMA			(CMD_BIT_SYS_MIN + 4)
#define SSC_CMDBIT_SYS_06			(CMD_BIT_SYS_MIN + 5)
#define SSC_CMDBIT_SYS_07			(CMD_BIT_SYS_MIN + 6)
#define SSC_CMDBIT_SYS_08			(CMD_BIT_SYS_MIN + 7)

#define SSC_CMDBIT_SYS_SMPS			(CMD_BIT_SYS_MIN + 8)
#define SSC_CMDBIT_SYS_10			(CMD_BIT_SYS_MIN + 9)
#define SSC_CMDBIT_SYS_11			(CMD_BIT_SYS_MIN + 10)
#define SSC_CMDBIT_SYS_12			(CMD_BIT_SYS_MIN + 11)
#define SSC_CMDBIT_SYS_13			(CMD_BIT_SYS_MIN + 12)
#define SSC_CMDBIT_SYS_14			(CMD_BIT_SYS_MIN + 13)
#define SSC_CMDBIT_SYS_15			(CMD_BIT_SYS_MIN + 14)
#define SSC_CMDBIT_SYS_16			(CMD_BIT_SYS_MIN + 15)

#define SSC_CMDBIT_SYS_SEMI			(CMD_BIT_SYS_MIN + 16)
#define SSC_CMDBIT_SYS_18			(CMD_BIT_SYS_MIN + 17)
#define SSC_CMDBIT_SYS_19			(CMD_BIT_SYS_MIN + 18)
#define SSC_CMDBIT_SYS_20			(CMD_BIT_SYS_MIN + 19)
#define SSC_CMDBIT_SYS_21			(CMD_BIT_SYS_MIN + 20)
#define SSC_CMDBIT_SYS_22			(CMD_BIT_SYS_MIN + 21)
#define SSC_CMDBIT_SYS_23			(CMD_BIT_SYS_MIN + 22)
#define SSC_CMDBIT_SYS_24			(CMD_BIT_SYS_MIN + 23)

#define SSC_CMDBIT_SYS_25			(CMD_BIT_SYS_MIN + 24)
#define SSC_CMDBIT_SYS_26			(CMD_BIT_SYS_MIN + 25)
#define SSC_CMDBIT_SYS_27			(CMD_BIT_SYS_MIN + 26)
#define SSC_CMDBIT_SYS_28			(CMD_BIT_SYS_MIN + 27)
#define SSC_CMDBIT_SYS_29			(CMD_BIT_SYS_MIN + 28)
#define SSC_CMDBIT_SYS_30			(CMD_BIT_SYS_MIN + 29)
#define SSC_CMDBIT_SYS_31			(CMD_BIT_SYS_MIN + 30)
#define SSC_CMDBIT_SYS_32			(CMD_BIT_SYS_MIN + 31)

#define SSC_CMDBIT_SYS_ITFE			(CMD_BIT_SYS_MIN + 32)
#define SSC_CMDBIT_SYS_34			(CMD_BIT_SYS_MIN + 33)
#define SSC_CMDBIT_SYS_35			(CMD_BIT_SYS_MIN + 34)
#define SSC_CMDBIT_SYS_36			(CMD_BIT_SYS_MIN + 35)
#define SSC_CMDBIT_SYS_37			(CMD_BIT_SYS_MIN + 36)
#define SSC_CMDBIT_SYS_38			(CMD_BIT_SYS_MIN + 37)
#define SSC_CMDBIT_SYS_39			(CMD_BIT_SYS_MIN + 38)
#define SSC_CMDBIT_SYS_40			(CMD_BIT_SYS_MIN + 39)

#define SSC_CMDBIT_SYS_41			(CMD_BIT_SYS_MIN + 40)
#define SSC_CMDBIT_SYS_42			(CMD_BIT_SYS_MIN + 41)
#define SSC_CMDBIT_SYS_43			(CMD_BIT_SYS_MIN + 42)
#define SSC_CMDBIT_SYS_44			(CMD_BIT_SYS_MIN + 43)
#define SSC_CMDBIT_SYS_45			(CMD_BIT_SYS_MIN + 44)
#define SSC_CMDBIT_SYS_46			(CMD_BIT_SYS_MIN + 45)
#define SSC_CMDBIT_SYS_47			(CMD_BIT_SYS_MIN + 46)
#define SSC_CMDBIT_SYS_48			(CMD_BIT_SYS_MIN + 47)

#define SSC_CMDBIT_SYS_ASYN1		(CMD_BIT_SYS_MIN + 48)
#define SSC_CMDBIT_SYS_ASYN2		(CMD_BIT_SYS_MIN + 49)
#define SSC_CMDBIT_SYS_ASYN3		(CMD_BIT_SYS_MIN + 50)
#define SSC_CMDBIT_SYS_ASYN4		(CMD_BIT_SYS_MIN + 51)
#define SSC_CMDBIT_SYS_ASYN5		(CMD_BIT_SYS_MIN + 52)
#define SSC_CMDBIT_SYS_ASYN6		(CMD_BIT_SYS_MIN + 53)
#define SSC_CMDBIT_SYS_ASYN7		(CMD_BIT_SYS_MIN + 54)
#define SSC_CMDBIT_SYS_ASYN8		(CMD_BIT_SYS_MIN + 55)

#define SSC_CMDBIT_SYS_57			(CMD_BIT_SYS_MIN + 56)
#define SSC_CMDBIT_SYS_58			(CMD_BIT_SYS_MIN + 57)
#define SSC_CMDBIT_SYS_59			(CMD_BIT_SYS_MIN + 58)
#define SSC_CMDBIT_SYS_60			(CMD_BIT_SYS_MIN + 59)
#define SSC_CMDBIT_SYS_61			(CMD_BIT_SYS_MIN + 60)
#define SSC_CMDBIT_SYS_62			(CMD_BIT_SYS_MIN + 61)
#define SSC_CMDBIT_SYS_63			(CMD_BIT_SYS_MIN + 62)
#define SSC_CMDBIT_SYS_64			(CMD_BIT_SYS_MIN + 63)

#define SSC_CMDBIT_SYS_RBR			(CMD_BIT_SYS_MIN + 64)
#define SSC_CMDBIT_SYS_RBS			(CMD_BIT_SYS_MIN + 65)
#define SSC_CMDBIT_SYS_CRST			(CMD_BIT_SYS_MIN + 66)
#define SSC_CMDBIT_SYS_68			(CMD_BIT_SYS_MIN + 67)
#define SSC_CMDBIT_SYS_SMON			(CMD_BIT_SYS_MIN + 68)
#define SSC_CMDBIT_SYS_SMONR		(CMD_BIT_SYS_MIN + 69)
#define SSC_CMDBIT_SYS_71			(CMD_BIT_SYS_MIN + 70)
#define SSC_CMDBIT_SYS_72			(CMD_BIT_SYS_MIN + 71)

#define SSC_CMDBIT_SYS_73			(CMD_BIT_SYS_MIN + 72)
#define SSC_CMDBIT_SYS_74			(CMD_BIT_SYS_MIN + 73)
#define SSC_CMDBIT_SYS_75			(CMD_BIT_SYS_MIN + 74)
#define SSC_CMDBIT_SYS_76			(CMD_BIT_SYS_MIN + 75)
#define SSC_CMDBIT_SYS_77			(CMD_BIT_SYS_MIN + 76)
#define SSC_CMDBIT_SYS_78			(CMD_BIT_SYS_MIN + 77)
#define SSC_CMDBIT_SYS_79			(CMD_BIT_SYS_MIN + 78)
#define SSC_CMDBIT_SYS_80			(CMD_BIT_SYS_MIN + 79)

#define SSC_CMDBIT_SYS_LOGC			(CMD_BIT_SYS_MIN + 80)
#define SSC_CMDBIT_SYS_LOGR			(CMD_BIT_SYS_MIN + 81)
#define SSC_CMDBIT_SYS_83			(CMD_BIT_SYS_MIN + 82)
#define SSC_CMDBIT_SYS_LOGI			(CMD_BIT_SYS_MIN + 83)
#define SSC_CMDBIT_SYS_85			(CMD_BIT_SYS_MIN + 84)
#define SSC_CMDBIT_SYS_OCMC			(CMD_BIT_SYS_MIN + 85)
#define SSC_CMDBIT_SYS87			(CMD_BIT_SYS_MIN + 86)
#define SSC_CMDBIT_SYS88			(CMD_BIT_SYS_MIN + 87)

#define SSC_CMDBIT_SYS_RCC			(CMD_BIT_SYS_MIN + 88)
#define SSC_CMDBIT_SYS_90			(CMD_BIT_SYS_MIN + 89)
#define SSC_CMDBIT_SYS_91			(CMD_BIT_SYS_MIN + 90)
#define SSC_CMDBIT_SYS_CCC			(CMD_BIT_SYS_MIN + 91)
#define SSC_CMDBIT_SYS_93			(CMD_BIT_SYS_MIN + 92)
#define SSC_CMDBIT_SYS_94			(CMD_BIT_SYS_MIN + 93)
#define SSC_CMDBIT_SYS_95			(CMD_BIT_SYS_MIN + 94)
#define SSC_CMDBIT_SYS_96			(CMD_BIT_SYS_MIN + 95)

#define SSC_CMDBIT_SYS_97			(CMD_BIT_SYS_MIN + 96)
#define SSC_CMDBIT_SYS_98			(CMD_BIT_SYS_MIN + 97)
#define SSC_CMDBIT_SYS_99			(CMD_BIT_SYS_MIN + 98)
#define SSC_CMDBIT_SYS_100			(CMD_BIT_SYS_MIN + 99)
#define SSC_CMDBIT_SYS_101			(CMD_BIT_SYS_MIN + 100)
#define SSC_CMDBIT_SYS_102			(CMD_BIT_SYS_MIN + 101)
#define SSC_CMDBIT_SYS_103			(CMD_BIT_SYS_MIN + 102)
#define SSC_CMDBIT_SYS_104			(CMD_BIT_SYS_MIN + 103)

#define SSC_CMDBIT_SYS_105			(CMD_BIT_SYS_MIN + 104)
#define SSC_CMDBIT_SYS_106			(CMD_BIT_SYS_MIN + 105)
#define SSC_CMDBIT_SYS_107			(CMD_BIT_SYS_MIN + 106)
#define SSC_CMDBIT_SYS_108			(CMD_BIT_SYS_MIN + 107)
#define SSC_CMDBIT_SYS_109			(CMD_BIT_SYS_MIN + 108)
#define SSC_CMDBIT_SYS_110			(CMD_BIT_SYS_MIN + 109)
#define SSC_CMDBIT_SYS_111			(CMD_BIT_SYS_MIN + 110)
#define SSC_CMDBIT_SYS_112			(CMD_BIT_SYS_MIN + 111)

#define SSC_CMDBIT_SYS_113			(CMD_BIT_SYS_MIN + 112)
#define SSC_CMDBIT_SYS_114			(CMD_BIT_SYS_MIN + 113)
#define SSC_CMDBIT_SYS_115			(CMD_BIT_SYS_MIN + 114)
#define SSC_CMDBIT_SYS_116			(CMD_BIT_SYS_MIN + 115)
#define SSC_CMDBIT_SYS_117			(CMD_BIT_SYS_MIN + 116)
#define SSC_CMDBIT_SYS_118			(CMD_BIT_SYS_MIN + 117)
#define SSC_CMDBIT_SYS_119			(CMD_BIT_SYS_MIN + 118)
#define SSC_CMDBIT_SYS_120			(CMD_BIT_SYS_MIN + 119)

#define SSC_CMDBIT_SYS_121			(CMD_BIT_SYS_MIN + 120)
#define SSC_CMDBIT_SYS_122			(CMD_BIT_SYS_MIN + 121)
#define SSC_CMDBIT_SYS_123			(CMD_BIT_SYS_MIN + 122)
#define SSC_CMDBIT_SYS_124			(CMD_BIT_SYS_MIN + 123)
#define SSC_CMDBIT_SYS_125			(CMD_BIT_SYS_MIN + 124)
#define SSC_CMDBIT_SYS_126			(CMD_BIT_SYS_MIN + 125)
#define SSC_CMDBIT_SYS_127			(CMD_BIT_SYS_MIN + 126)
#define SSC_CMDBIT_SYS_128			(CMD_BIT_SYS_MIN + 127)

#define SSC_CMDBIT_SYS_SPWRT		(CMD_BIT_SYS_MIN + 128)
#define SSC_CMDBIT_SYS_130			(CMD_BIT_SYS_MIN + 129)
#define SSC_CMDBIT_SYS_131			(CMD_BIT_SYS_MIN + 130)
#define SSC_CMDBIT_SYS_132			(CMD_BIT_SYS_MIN + 131)
#define SSC_CMDBIT_SYS_133			(CMD_BIT_SYS_MIN + 132)
#define SSC_CMDBIT_SYS_134			(CMD_BIT_SYS_MIN + 133)
#define SSC_CMDBIT_SYS_135			(CMD_BIT_SYS_MIN + 134)
#define SSC_CMDBIT_SYS_136			(CMD_BIT_SYS_MIN + 135)

#define SSC_CMDBIT_SYS_SPRD			(CMD_BIT_SYS_MIN + 136)
#define SSC_CMDBIT_SYS_138			(CMD_BIT_SYS_MIN + 137)
#define SSC_CMDBIT_SYS_139			(CMD_BIT_SYS_MIN + 138)
#define SSC_CMDBIT_SYS_140			(CMD_BIT_SYS_MIN + 139)
#define SSC_CMDBIT_SYS_141			(CMD_BIT_SYS_MIN + 140)
#define SSC_CMDBIT_SYS_142			(CMD_BIT_SYS_MIN + 141)
#define SSC_CMDBIT_SYS_143			(CMD_BIT_SYS_MIN + 142)
#define SSC_CMDBIT_SYS_144			(CMD_BIT_SYS_MIN + 143)

#define SSC_CMDBIT_SYS_SMPSW		(CMD_BIT_SYS_MIN + 144)
#define SSC_CMDBIT_SYS_146			(CMD_BIT_SYS_MIN + 145)
#define SSC_CMDBIT_SYS_147			(CMD_BIT_SYS_MIN + 146)
#define SSC_CMDBIT_SYS_148			(CMD_BIT_SYS_MIN + 147)
#define SSC_CMDBIT_SYS_SMPSR		(CMD_BIT_SYS_MIN + 148)
#define SSC_CMDBIT_SYS_150			(CMD_BIT_SYS_MIN + 149)
#define SSC_CMDBIT_SYS_151			(CMD_BIT_SYS_MIN + 150)
#define SSC_CMDBIT_SYS_152			(CMD_BIT_SYS_MIN + 151)

#define SSC_CMDBIT_SYS_153			(CMD_BIT_SYS_MIN + 152)
#define SSC_CMDBIT_SYS_154			(CMD_BIT_SYS_MIN + 153)
#define SSC_CMDBIT_SYS_155			(CMD_BIT_SYS_MIN + 154)
#define SSC_CMDBIT_SYS_156			(CMD_BIT_SYS_MIN + 155)
#define SSC_CMDBIT_SYS_157			(CMD_BIT_SYS_MIN + 156)
#define SSC_CMDBIT_SYS_158			(CMD_BIT_SYS_MIN + 157)
#define SSC_CMDBIT_SYS_159			(CMD_BIT_SYS_MIN + 158)
#define SSC_CMDBIT_SYS_160			(CMD_BIT_SYS_MIN + 159)

#define SSC_CMDBIT_SYS_161			(CMD_BIT_SYS_MIN + 160)
#define SSC_CMDBIT_SYS_162			(CMD_BIT_SYS_MIN + 161)
#define SSC_CMDBIT_SYS_163			(CMD_BIT_SYS_MIN + 162)
#define SSC_CMDBIT_SYS_164			(CMD_BIT_SYS_MIN + 163)
#define SSC_CMDBIT_SYS_165			(CMD_BIT_SYS_MIN + 164)
#define SSC_CMDBIT_SYS_166			(CMD_BIT_SYS_MIN + 165)
#define SSC_CMDBIT_SYS_167			(CMD_BIT_SYS_MIN + 166)
#define SSC_CMDBIT_SYS_168			(CMD_BIT_SYS_MIN + 167)

#define SSC_CMDBIT_SYS_169			(CMD_BIT_SYS_MIN + 168)
#define SSC_CMDBIT_SYS_170			(CMD_BIT_SYS_MIN + 169)
#define SSC_CMDBIT_SYS_171			(CMD_BIT_SYS_MIN + 170)
#define SSC_CMDBIT_SYS_172			(CMD_BIT_SYS_MIN + 171)
#define SSC_CMDBIT_SYS_173			(CMD_BIT_SYS_MIN + 172)
#define SSC_CMDBIT_SYS_174			(CMD_BIT_SYS_MIN + 173)
#define SSC_CMDBIT_SYS_175			(CMD_BIT_SYS_MIN + 174)
#define SSC_CMDBIT_SYS_176			(CMD_BIT_SYS_MIN + 175)

#define SSC_CMDBIT_SYS_FTR			(CMD_BIT_SYS_MIN + 176)
#define SSC_CMDBIT_SYS_FTS			(CMD_BIT_SYS_MIN + 177)
#define SSC_CMDBIT_SYS_179			(CMD_BIT_SYS_MIN + 178)
#define SSC_CMDBIT_SYS_180			(CMD_BIT_SYS_MIN + 179)
#define SSC_CMDBIT_SYS_FIR			(CMD_BIT_SYS_MIN + 180)
#define SSC_CMDBIT_SYS_FIS			(CMD_BIT_SYS_MIN + 181)
#define SSC_CMDBIT_SYS_183			(CMD_BIT_SYS_MIN + 182)
#define SSC_CMDBIT_SYS_184			(CMD_BIT_SYS_MIN + 183)

#define SSC_CMDBIT_SYS_ALHR			(CMD_BIT_SYS_MIN + 184)
#define SSC_CMDBIT_SYS_186			(CMD_BIT_SYS_MIN + 185)
#define SSC_CMDBIT_SYS_ALHI			(CMD_BIT_SYS_MIN + 186)
#define SSC_CMDBIT_SYS_188			(CMD_BIT_SYS_MIN + 187)
#define SSC_CMDBIT_SYS_189			(CMD_BIT_SYS_MIN + 188)
#define SSC_CMDBIT_SYS_190			(CMD_BIT_SYS_MIN + 189)
#define SSC_CMDBIT_SYS_191			(CMD_BIT_SYS_MIN + 190)
#define SSC_CMDBIT_SYS_192			(CMD_BIT_SYS_MIN + 191)

#define SSC_CMDBIT_SYS_193			(CMD_BIT_SYS_MIN + 192)
#define SSC_CMDBIT_SYS_194			(CMD_BIT_SYS_MIN + 193)
#define SSC_CMDBIT_SYS_195			(CMD_BIT_SYS_MIN + 194)
#define SSC_CMDBIT_SYS_196			(CMD_BIT_SYS_MIN + 195)
#define SSC_CMDBIT_SYS_197			(CMD_BIT_SYS_MIN + 196)
#define SSC_CMDBIT_SYS_198			(CMD_BIT_SYS_MIN + 197)
#define SSC_CMDBIT_SYS_199			(CMD_BIT_SYS_MIN + 198)
#define SSC_CMDBIT_SYS_200			(CMD_BIT_SYS_MIN + 199)

#define SSC_CMDBIT_SYS_201			(CMD_BIT_SYS_MIN + 200)
#define SSC_CMDBIT_SYS_202			(CMD_BIT_SYS_MIN + 201)
#define SSC_CMDBIT_SYS_203			(CMD_BIT_SYS_MIN + 202)
#define SSC_CMDBIT_SYS_204			(CMD_BIT_SYS_MIN + 203)
#define SSC_CMDBIT_SYS_205			(CMD_BIT_SYS_MIN + 204)
#define SSC_CMDBIT_SYS_206			(CMD_BIT_SYS_MIN + 205)
#define SSC_CMDBIT_SYS_207			(CMD_BIT_SYS_MIN + 206)
#define SSC_CMDBIT_SYS_208			(CMD_BIT_SYS_MIN + 207)

#define SSC_CMDBIT_SYS_209			(CMD_BIT_SYS_MIN + 208)
#define SSC_CMDBIT_SYS_210			(CMD_BIT_SYS_MIN + 209)
#define SSC_CMDBIT_SYS_211			(CMD_BIT_SYS_MIN + 210)
#define SSC_CMDBIT_SYS_212			(CMD_BIT_SYS_MIN + 211)
#define SSC_CMDBIT_SYS_213			(CMD_BIT_SYS_MIN + 212)
#define SSC_CMDBIT_SYS_214			(CMD_BIT_SYS_MIN + 213)
#define SSC_CMDBIT_SYS_215			(CMD_BIT_SYS_MIN + 214)
#define SSC_CMDBIT_SYS_216			(CMD_BIT_SYS_MIN + 215)

#define SSC_CMDBIT_SYS_217			(CMD_BIT_SYS_MIN + 216)
#define SSC_CMDBIT_SYS_218			(CMD_BIT_SYS_MIN + 217)
#define SSC_CMDBIT_SYS_219			(CMD_BIT_SYS_MIN + 218)
#define SSC_CMDBIT_SYS_220			(CMD_BIT_SYS_MIN + 219)
#define SSC_CMDBIT_SYS_221			(CMD_BIT_SYS_MIN + 220)
#define SSC_CMDBIT_SYS_222			(CMD_BIT_SYS_MIN + 221)
#define SSC_CMDBIT_SYS_223			(CMD_BIT_SYS_MIN + 222)
#define SSC_CMDBIT_SYS_224			(CMD_BIT_SYS_MIN + 223)

#define SSC_CMDBIT_SYS_225			(CMD_BIT_SYS_MIN + 224)
#define SSC_CMDBIT_SYS_226			(CMD_BIT_SYS_MIN + 225)
#define SSC_CMDBIT_SYS_227			(CMD_BIT_SYS_MIN + 226)
#define SSC_CMDBIT_SYS_228			(CMD_BIT_SYS_MIN + 227)
#define SSC_CMDBIT_SYS_229			(CMD_BIT_SYS_MIN + 228)
#define SSC_CMDBIT_SYS_230			(CMD_BIT_SYS_MIN + 229)
#define SSC_CMDBIT_SYS_231			(CMD_BIT_SYS_MIN + 230)
#define SSC_CMDBIT_SYS_232			(CMD_BIT_SYS_MIN + 231)

#define SSC_CMDBIT_SYS_233			(CMD_BIT_SYS_MIN + 232)
#define SSC_CMDBIT_SYS_234			(CMD_BIT_SYS_MIN + 233)
#define SSC_CMDBIT_SYS_235			(CMD_BIT_SYS_MIN + 234)
#define SSC_CMDBIT_SYS_236			(CMD_BIT_SYS_MIN + 235)
#define SSC_CMDBIT_SYS_237			(CMD_BIT_SYS_MIN + 236)
#define SSC_CMDBIT_SYS_238			(CMD_BIT_SYS_MIN + 237)
#define SSC_CMDBIT_SYS_239			(CMD_BIT_SYS_MIN + 238)
#define SSC_CMDBIT_SYS_240			(CMD_BIT_SYS_MIN + 239)

#define SSC_CMDBIT_SYS_241			(CMD_BIT_SYS_MIN + 240)
#define SSC_CMDBIT_SYS_242			(CMD_BIT_SYS_MIN + 241)
#define SSC_CMDBIT_SYS_243			(CMD_BIT_SYS_MIN + 242)
#define SSC_CMDBIT_SYS_244			(CMD_BIT_SYS_MIN + 243)
#define SSC_CMDBIT_SYS_245			(CMD_BIT_SYS_MIN + 244)
#define SSC_CMDBIT_SYS_246			(CMD_BIT_SYS_MIN + 245)
#define SSC_CMDBIT_SYS_247			(CMD_BIT_SYS_MIN + 246)
#define SSC_CMDBIT_SYS_248			(CMD_BIT_SYS_MIN + 247)

#define SSC_CMDBIT_SYS_249			(CMD_BIT_SYS_MIN + 248)
#define SSC_CMDBIT_SYS_250			(CMD_BIT_SYS_MIN + 249)
#define SSC_CMDBIT_SYS_251			(CMD_BIT_SYS_MIN + 250)
#define SSC_CMDBIT_SYS_252			(CMD_BIT_SYS_MIN + 251)
#define SSC_CMDBIT_SYS_253			(CMD_BIT_SYS_MIN + 252)
#define SSC_CMDBIT_SYS_254			(CMD_BIT_SYS_MIN + 253)
#define SSC_CMDBIT_SYS_255			(CMD_BIT_SYS_MIN + 254)
#define SSC_CMDBIT_SYS_256			(CMD_BIT_SYS_MIN + 255)


/*------------------------------------------*/
/* system status bits                       */
/*------------------------------------------*/
#define SSC_STSBIT_SYS_ITO			(STS_BIT_SYS_MIN + 0)
#define SSC_STSBIT_SYS_IITO			(STS_BIT_SYS_MIN + 1)
#define SSC_STSBIT_SYS_EVDO			(STS_BIT_SYS_MIN + 2)
#define SSC_STSBIT_SYS_HRIF			(STS_BIT_SYS_MIN + 3)
#define SSC_STSBIT_SYS_BMA			(STS_BIT_SYS_MIN + 4)
#define SSC_STSBIT_SYS_PRINF		(STS_BIT_SYS_MIN + 5)
#define SSC_STSBIT_SYS_07			(STS_BIT_SYS_MIN + 6)
#define SSC_STSBIT_SYS_IFMO			(STS_BIT_SYS_MIN + 7)

#define SSC_STSBIT_SYS_SMPW			(STS_BIT_SYS_MIN + 8)
#define SSC_STSBIT_SYS_SMPO			(STS_BIT_SYS_MIN + 9)
#define SSC_STSBIT_SYS_SMPF			(STS_BIT_SYS_MIN + 10)
#define SSC_STSBIT_SYS_SMPE			(STS_BIT_SYS_MIN + 11)
#define SSC_STSBIT_SYS_13			(STS_BIT_SYS_MIN + 12)
#define SSC_STSBIT_SYS_AHINF		(STS_BIT_SYS_MIN + 13)
#define SSC_STSBIT_SYS_15			(STS_BIT_SYS_MIN + 14)
#define SSC_STSBIT_SYS_16			(STS_BIT_SYS_MIN + 15)

#define SSC_STSBIT_SYS_EMIO			(STS_BIT_SYS_MIN + 16)
#define SSC_STSBIT_SYS_18			(STS_BIT_SYS_MIN + 17)
#define SSC_STSBIT_SYS_TSTO			(STS_BIT_SYS_MIN + 18)
#define SSC_STSBIT_SYS_20			(STS_BIT_SYS_MIN + 19)
#define SSC_STSBIT_SYS_21			(STS_BIT_SYS_MIN + 20)
#define SSC_STSBIT_SYS_22			(STS_BIT_SYS_MIN + 21)
#define SSC_STSBIT_SYS_EMID			(STS_BIT_SYS_MIN + 22)
#define SSC_STSBIT_SYS_24			(STS_BIT_SYS_MIN + 23)

#define SSC_STSBIT_SYS_25			(STS_BIT_SYS_MIN + 24)
#define SSC_STSBIT_SYS_26			(STS_BIT_SYS_MIN + 25)
#define SSC_STSBIT_SYS_27			(STS_BIT_SYS_MIN + 26)
#define SSC_STSBIT_SYS_28			(STS_BIT_SYS_MIN + 27)
#define SSC_STSBIT_SYS_29			(STS_BIT_SYS_MIN + 28)
#define SSC_STSBIT_SYS_30			(STS_BIT_SYS_MIN + 29)
#define SSC_STSBIT_SYS_IPCH			(STS_BIT_SYS_MIN + 30)
#define SSC_STSBIT_SYS_32			(STS_BIT_SYS_MIN + 31)

#define SSC_STSBIT_SYS_33			(STS_BIT_SYS_MIN + 32)
#define SSC_STSBIT_SYS_34			(STS_BIT_SYS_MIN + 33)
#define SSC_STSBIT_SYS_35			(STS_BIT_SYS_MIN + 34)
#define SSC_STSBIT_SYS_36			(STS_BIT_SYS_MIN + 35)
#define SSC_STSBIT_SYS_37			(STS_BIT_SYS_MIN + 36)
#define SSC_STSBIT_SYS_38			(STS_BIT_SYS_MIN + 37)
#define SSC_STSBIT_SYS_39			(STS_BIT_SYS_MIN + 38)
#define SSC_STSBIT_SYS_40			(STS_BIT_SYS_MIN + 39)

#define SSC_STSBIT_SYS_41			(STS_BIT_SYS_MIN + 40)
#define SSC_STSBIT_SYS_42			(STS_BIT_SYS_MIN + 41)
#define SSC_STSBIT_SYS_43			(STS_BIT_SYS_MIN + 42)
#define SSC_STSBIT_SYS_44			(STS_BIT_SYS_MIN + 43)
#define SSC_STSBIT_SYS_45			(STS_BIT_SYS_MIN + 44)
#define SSC_STSBIT_SYS_46			(STS_BIT_SYS_MIN + 45)
#define SSC_STSBIT_SYS_47			(STS_BIT_SYS_MIN + 46)
#define SSC_STSBIT_SYS_48			(STS_BIT_SYS_MIN + 47)

#define SSC_STSBIT_SYS_ASYO1		(STS_BIT_SYS_MIN + 48)
#define SSC_STSBIT_SYS_ASYO2		(STS_BIT_SYS_MIN + 49)
#define SSC_STSBIT_SYS_ASYO3		(STS_BIT_SYS_MIN + 50)
#define SSC_STSBIT_SYS_ASYO4		(STS_BIT_SYS_MIN + 51)
#define SSC_STSBIT_SYS_ASYO5		(STS_BIT_SYS_MIN + 52)
#define SSC_STSBIT_SYS_ASYO6		(STS_BIT_SYS_MIN + 53)
#define SSC_STSBIT_SYS_ASYO7		(STS_BIT_SYS_MIN + 54)
#define SSC_STSBIT_SYS_ASYO8		(STS_BIT_SYS_MIN + 55)

#define SSC_STSBIT_SYS_SYEO1		(STS_BIT_SYS_MIN + 56)
#define SSC_STSBIT_SYS_SYEO2		(STS_BIT_SYS_MIN + 57)
#define SSC_STSBIT_SYS_SYEO3		(STS_BIT_SYS_MIN + 58)
#define SSC_STSBIT_SYS_SYEO4		(STS_BIT_SYS_MIN + 59)
#define SSC_STSBIT_SYS_SYEO5		(STS_BIT_SYS_MIN + 60)
#define SSC_STSBIT_SYS_SYEO6		(STS_BIT_SYS_MIN + 61)
#define SSC_STSBIT_SYS_SYEO7		(STS_BIT_SYS_MIN + 62)
#define SSC_STSBIT_SYS_SYEO8		(STS_BIT_SYS_MIN + 63)

#define SSC_STSBIT_SYS_RBOK			(STS_BIT_SYS_MIN + 64)
#define SSC_STSBIT_SYS_RBNG			(STS_BIT_SYS_MIN + 65)
#define SSC_STSBIT_SYS_CALM			(STS_BIT_SYS_MIN + 66)
#define SSC_STSBIT_SYS_68			(STS_BIT_SYS_MIN + 67)
#define SSC_STSBIT_SYS_SMOUT		(STS_BIT_SYS_MIN + 68)
#define SSC_STSBIT_SYS_SMRCH		(STS_BIT_SYS_MIN + 69)
#define SSC_STSBIT_SYS_SMER1		(STS_BIT_SYS_MIN + 70)
#define SSC_STSBIT_SYS_SMER2		(STS_BIT_SYS_MIN + 71)

#define SSC_STSBIT_SYS_73			(STS_BIT_SYS_MIN + 72)
#define SSC_STSBIT_SYS_74			(STS_BIT_SYS_MIN + 73)
#define SSC_STSBIT_SYS_75			(STS_BIT_SYS_MIN + 74)
#define SSC_STSBIT_SYS_76			(STS_BIT_SYS_MIN + 75)
#define SSC_STSBIT_SYS_77			(STS_BIT_SYS_MIN + 76)
#define SSC_STSBIT_SYS_78			(STS_BIT_SYS_MIN + 77)
#define SSC_STSBIT_SYS_79			(STS_BIT_SYS_MIN + 78)
#define SSC_STSBIT_SYS_80			(STS_BIT_SYS_MIN + 79)

#define SSC_STSBIT_SYS_LOGO			(STS_BIT_SYS_MIN + 80)
#define SSC_STSBIT_SYS_LOGRF		(STS_BIT_SYS_MIN + 81)
#define SSC_STSBIT_SYS_LOGRE		(STS_BIT_SYS_MIN + 82)
#define SSC_STSBIT_SYS_LOGIF		(STS_BIT_SYS_MIN + 83)
#define SSC_STSBIT_SYS_LOGIE		(STS_BIT_SYS_MIN + 84)
#define SSC_STSBIT_SYS_OCMCO		(STS_BIT_SYS_MIN + 85)
#define SSC_STSBIT_SYS_OCME			(STS_BIT_SYS_MIN + 86)
#define SSC_STSBIT_SYS_OCMW			(STS_BIT_SYS_MIN + 87)

#define SSC_STSBIT_SYS_RCO			(STS_BIT_SYS_MIN + 88)
#define SSC_STSBIT_SYS_RCF			(STS_BIT_SYS_MIN + 89)
#define SSC_STSBIT_SYS_RCE			(STS_BIT_SYS_MIN + 90)
#define SSC_STSBIT_SYS_CCO			(STS_BIT_SYS_MIN + 91)
#define SSC_STSBIT_SYS_CCF			(STS_BIT_SYS_MIN + 92)
#define SSC_STSBIT_SYS_CCE			(STS_BIT_SYS_MIN + 93)
#define SSC_STSBIT_SYS_95			(STS_BIT_SYS_MIN + 94)
#define SSC_STSBIT_SYS_96			(STS_BIT_SYS_MIN + 95)

#define SSC_STSBIT_SYS_97			(STS_BIT_SYS_MIN + 96)
#define SSC_STSBIT_SYS_98			(STS_BIT_SYS_MIN + 97)
#define SSC_STSBIT_SYS_99			(STS_BIT_SYS_MIN + 98)
#define SSC_STSBIT_SYS_100			(STS_BIT_SYS_MIN + 99)
#define SSC_STSBIT_SYS_101			(STS_BIT_SYS_MIN + 100)
#define SSC_STSBIT_SYS_102			(STS_BIT_SYS_MIN + 101)
#define SSC_STSBIT_SYS_103			(STS_BIT_SYS_MIN + 102)
#define SSC_STSBIT_SYS_104			(STS_BIT_SYS_MIN + 103)

#define SSC_STSBIT_SYS_105			(STS_BIT_SYS_MIN + 104)
#define SSC_STSBIT_SYS_106			(STS_BIT_SYS_MIN + 105)
#define SSC_STSBIT_SYS_107			(STS_BIT_SYS_MIN + 106)
#define SSC_STSBIT_SYS_108			(STS_BIT_SYS_MIN + 107)
#define SSC_STSBIT_SYS_109			(STS_BIT_SYS_MIN + 108)
#define SSC_STSBIT_SYS_110			(STS_BIT_SYS_MIN + 109)
#define SSC_STSBIT_SYS_111			(STS_BIT_SYS_MIN + 110)
#define SSC_STSBIT_SYS_112			(STS_BIT_SYS_MIN + 111)

#define SSC_STSBIT_SYS_113			(STS_BIT_SYS_MIN + 112)
#define SSC_STSBIT_SYS_114			(STS_BIT_SYS_MIN + 113)
#define SSC_STSBIT_SYS_115			(STS_BIT_SYS_MIN + 114)
#define SSC_STSBIT_SYS_116			(STS_BIT_SYS_MIN + 115)
#define SSC_STSBIT_SYS_117			(STS_BIT_SYS_MIN + 116)
#define SSC_STSBIT_SYS_118			(STS_BIT_SYS_MIN + 117)
#define SSC_STSBIT_SYS_119			(STS_BIT_SYS_MIN + 118)
#define SSC_STSBIT_SYS_120			(STS_BIT_SYS_MIN + 119)

#define SSC_STSBIT_SYS_121			(STS_BIT_SYS_MIN + 120)
#define SSC_STSBIT_SYS_122			(STS_BIT_SYS_MIN + 121)
#define SSC_STSBIT_SYS_123			(STS_BIT_SYS_MIN + 122)
#define SSC_STSBIT_SYS_124			(STS_BIT_SYS_MIN + 123)
#define SSC_STSBIT_SYS_125			(STS_BIT_SYS_MIN + 124)
#define SSC_STSBIT_SYS_126			(STS_BIT_SYS_MIN + 125)
#define SSC_STSBIT_SYS_127			(STS_BIT_SYS_MIN + 126)
#define SSC_STSBIT_SYS_128			(STS_BIT_SYS_MIN + 127)

#define SSC_STSBIT_SYS_SPWFIN1		(STS_BIT_SYS_MIN + 128)
#define SSC_STSBIT_SYS_SPWEN1		(STS_BIT_SYS_MIN + 129)
#define SSC_STSBIT_SYS_SPWED1		(STS_BIT_SYS_MIN + 130)
#define SSC_STSBIT_SYS_132			(STS_BIT_SYS_MIN + 131)
#define SSC_STSBIT_SYS_PWFIN2		(STS_BIT_SYS_MIN + 132)
#define SSC_STSBIT_SYS_PWEN2		(STS_BIT_SYS_MIN + 133)
#define SSC_STSBIT_SYS_PWED2		(STS_BIT_SYS_MIN + 134)
#define SSC_STSBIT_SYS_136			(STS_BIT_SYS_MIN + 135)

#define SSC_STSBIT_SYS_SPRFIN1		(STS_BIT_SYS_MIN + 136)
#define SSC_STSBIT_SYS_SPREN1		(STS_BIT_SYS_MIN + 137)
#define SSC_STSBIT_SYS_SPRFIN2		(STS_BIT_SYS_MIN + 138)
#define SSC_STSBIT_SYS_SPREN2		(STS_BIT_SYS_MIN + 139)
#define SSC_STSBIT_SYS_141			(STS_BIT_SYS_MIN + 140)
#define SSC_STSBIT_SYS_142			(STS_BIT_SYS_MIN + 141)
#define SSC_STSBIT_SYS_143			(STS_BIT_SYS_MIN + 142)
#define SSC_STSBIT_SYS_144			(STS_BIT_SYS_MIN + 143)

#define SSC_STSBIT_SYS_SWFIN		(STS_BIT_SYS_MIN + 144)
#define SSC_STSBIT_SYS_SWEN			(STS_BIT_SYS_MIN + 145)
#define SSC_STSBIT_SYS_SWED			(STS_BIT_SYS_MIN + 146)
#define SSC_STSBIT_SYS_148			(STS_BIT_SYS_MIN + 147)
#define SSC_STSBIT_SYS_SRFIN		(STS_BIT_SYS_MIN + 148)
#define SSC_STSBIT_SYS_SREN			(STS_BIT_SYS_MIN + 149)
#define SSC_STSBIT_SYS_151			(STS_BIT_SYS_MIN + 150)
#define SSC_STSBIT_SYS_152			(STS_BIT_SYS_MIN + 151)

#define SSC_STSBIT_SYS_153			(STS_BIT_SYS_MIN + 152)
#define SSC_STSBIT_SYS_154			(STS_BIT_SYS_MIN + 153)
#define SSC_STSBIT_SYS_155			(STS_BIT_SYS_MIN + 154)
#define SSC_STSBIT_SYS_156			(STS_BIT_SYS_MIN + 155)
#define SSC_STSBIT_SYS_157			(STS_BIT_SYS_MIN + 156)
#define SSC_STSBIT_SYS_158			(STS_BIT_SYS_MIN + 157)
#define SSC_STSBIT_SYS_159			(STS_BIT_SYS_MIN + 158)
#define SSC_STSBIT_SYS_160			(STS_BIT_SYS_MIN + 159)

#define SSC_STSBIT_SYS_161			(STS_BIT_SYS_MIN + 160)
#define SSC_STSBIT_SYS_162			(STS_BIT_SYS_MIN + 161)
#define SSC_STSBIT_SYS_163			(STS_BIT_SYS_MIN + 162)
#define SSC_STSBIT_SYS_164			(STS_BIT_SYS_MIN + 163)
#define SSC_STSBIT_SYS_165			(STS_BIT_SYS_MIN + 164)
#define SSC_STSBIT_SYS_166			(STS_BIT_SYS_MIN + 165)
#define SSC_STSBIT_SYS_167			(STS_BIT_SYS_MIN + 166)
#define SSC_STSBIT_SYS_168			(STS_BIT_SYS_MIN + 167)

#define SSC_STSBIT_SYS_169			(STS_BIT_SYS_MIN + 168)
#define SSC_STSBIT_SYS_170			(STS_BIT_SYS_MIN + 169)
#define SSC_STSBIT_SYS_171			(STS_BIT_SYS_MIN + 170)
#define SSC_STSBIT_SYS_172			(STS_BIT_SYS_MIN + 171)
#define SSC_STSBIT_SYS_173			(STS_BIT_SYS_MIN + 172)
#define SSC_STSBIT_SYS_174			(STS_BIT_SYS_MIN + 173)
#define SSC_STSBIT_SYS_175			(STS_BIT_SYS_MIN + 174)
#define SSC_STSBIT_SYS_176			(STS_BIT_SYS_MIN + 175)

#define SSC_STSBIT_SYS_FROK			(STS_BIT_SYS_MIN + 176)
#define SSC_STSBIT_SYS_FRNG			(STS_BIT_SYS_MIN + 177)
#define SSC_STSBIT_SYS_FSOK			(STS_BIT_SYS_MIN + 178)
#define SSC_STSBIT_SYS_FSNG			(STS_BIT_SYS_MIN + 179)
#define SSC_STSBIT_SYS_FIROK		(STS_BIT_SYS_MIN + 180)
#define SSC_STSBIT_SYS_FIRNG		(STS_BIT_SYS_MIN + 181)
#define SSC_STSBIT_SYS_FIOK			(STS_BIT_SYS_MIN + 182)
#define SSC_STSBIT_SYS_FING			(STS_BIT_SYS_MIN + 183)

#define SSC_STSBIT_SYS_ALHRF		(STS_BIT_SYS_MIN + 184)
#define SSC_STSBIT_SYS_ALHRE		(STS_BIT_SYS_MIN + 185)
#define SSC_STSBIT_SYS_ALHIF		(STS_BIT_SYS_MIN + 186)
#define SSC_STSBIT_SYS_ALHIE		(STS_BIT_SYS_MIN + 187)
#define SSC_STSBIT_SYS_189			(STS_BIT_SYS_MIN + 188)
#define SSC_STSBIT_SYS_190			(STS_BIT_SYS_MIN + 189)
#define SSC_STSBIT_SYS_191			(STS_BIT_SYS_MIN + 190)
#define SSC_STSBIT_SYS_192			(STS_BIT_SYS_MIN + 191)

#define SSC_STSBIT_SYS_193			(STS_BIT_SYS_MIN + 192)
#define SSC_STSBIT_SYS_194			(STS_BIT_SYS_MIN + 193)
#define SSC_STSBIT_SYS_195			(STS_BIT_SYS_MIN + 194)
#define SSC_STSBIT_SYS_196			(STS_BIT_SYS_MIN + 195)
#define SSC_STSBIT_SYS_197			(STS_BIT_SYS_MIN + 196)
#define SSC_STSBIT_SYS_198			(STS_BIT_SYS_MIN + 197)
#define SSC_STSBIT_SYS_199			(STS_BIT_SYS_MIN + 198)
#define SSC_STSBIT_SYS_200			(STS_BIT_SYS_MIN + 199)

#define SSC_STSBIT_SYS_201			(STS_BIT_SYS_MIN + 200)
#define SSC_STSBIT_SYS_202			(STS_BIT_SYS_MIN + 201)
#define SSC_STSBIT_SYS_203			(STS_BIT_SYS_MIN + 202)
#define SSC_STSBIT_SYS_204			(STS_BIT_SYS_MIN + 203)
#define SSC_STSBIT_SYS_205			(STS_BIT_SYS_MIN + 204)
#define SSC_STSBIT_SYS_206			(STS_BIT_SYS_MIN + 205)
#define SSC_STSBIT_SYS_207			(STS_BIT_SYS_MIN + 206)
#define SSC_STSBIT_SYS_208			(STS_BIT_SYS_MIN + 207)

#define SSC_STSBIT_SYS_209			(STS_BIT_SYS_MIN + 208)
#define SSC_STSBIT_SYS_210			(STS_BIT_SYS_MIN + 209)
#define SSC_STSBIT_SYS_211			(STS_BIT_SYS_MIN + 210)
#define SSC_STSBIT_SYS_212			(STS_BIT_SYS_MIN + 211)
#define SSC_STSBIT_SYS_213			(STS_BIT_SYS_MIN + 212)
#define SSC_STSBIT_SYS_214			(STS_BIT_SYS_MIN + 213)
#define SSC_STSBIT_SYS_215			(STS_BIT_SYS_MIN + 214)
#define SSC_STSBIT_SYS_216			(STS_BIT_SYS_MIN + 215)

#define SSC_STSBIT_SYS_217			(STS_BIT_SYS_MIN + 216)
#define SSC_STSBIT_SYS_218			(STS_BIT_SYS_MIN + 217)
#define SSC_STSBIT_SYS_219			(STS_BIT_SYS_MIN + 218)
#define SSC_STSBIT_SYS_220			(STS_BIT_SYS_MIN + 219)
#define SSC_STSBIT_SYS_221			(STS_BIT_SYS_MIN + 220)
#define SSC_STSBIT_SYS_222			(STS_BIT_SYS_MIN + 221)
#define SSC_STSBIT_SYS_223			(STS_BIT_SYS_MIN + 222)
#define SSC_STSBIT_SYS_224			(STS_BIT_SYS_MIN + 223)

#define SSC_STSBIT_SYS_225			(STS_BIT_SYS_MIN + 224)
#define SSC_STSBIT_SYS_226			(STS_BIT_SYS_MIN + 225)
#define SSC_STSBIT_SYS_227			(STS_BIT_SYS_MIN + 226)
#define SSC_STSBIT_SYS_228			(STS_BIT_SYS_MIN + 227)
#define SSC_STSBIT_SYS_229			(STS_BIT_SYS_MIN + 228)
#define SSC_STSBIT_SYS_230			(STS_BIT_SYS_MIN + 229)
#define SSC_STSBIT_SYS_231			(STS_BIT_SYS_MIN + 230)
#define SSC_STSBIT_SYS_232			(STS_BIT_SYS_MIN + 231)

#define SSC_STSBIT_SYS_233			(STS_BIT_SYS_MIN + 232)
#define SSC_STSBIT_SYS_234			(STS_BIT_SYS_MIN + 233)
#define SSC_STSBIT_SYS_235			(STS_BIT_SYS_MIN + 234)
#define SSC_STSBIT_SYS_236			(STS_BIT_SYS_MIN + 235)
#define SSC_STSBIT_SYS_237			(STS_BIT_SYS_MIN + 236)
#define SSC_STSBIT_SYS_238			(STS_BIT_SYS_MIN + 237)
#define SSC_STSBIT_SYS_239			(STS_BIT_SYS_MIN + 238)
#define SSC_STSBIT_SYS_240			(STS_BIT_SYS_MIN + 239)

#define SSC_STSBIT_SYS_241			(STS_BIT_SYS_MIN + 240)
#define SSC_STSBIT_SYS_242			(STS_BIT_SYS_MIN + 241)
#define SSC_STSBIT_SYS_243			(STS_BIT_SYS_MIN + 242)
#define SSC_STSBIT_SYS_244			(STS_BIT_SYS_MIN + 243)
#define SSC_STSBIT_SYS_245			(STS_BIT_SYS_MIN + 244)
#define SSC_STSBIT_SYS_246			(STS_BIT_SYS_MIN + 245)
#define SSC_STSBIT_SYS_247			(STS_BIT_SYS_MIN + 246)
#define SSC_STSBIT_SYS_248			(STS_BIT_SYS_MIN + 247)

#define SSC_STSBIT_SYS_249			(STS_BIT_SYS_MIN + 248)
#define SSC_STSBIT_SYS_250			(STS_BIT_SYS_MIN + 249)
#define SSC_STSBIT_SYS_251			(STS_BIT_SYS_MIN + 250)
#define SSC_STSBIT_SYS_252			(STS_BIT_SYS_MIN + 251)
#define SSC_STSBIT_SYS_253			(STS_BIT_SYS_MIN + 252)
#define SSC_STSBIT_SYS_254			(STS_BIT_SYS_MIN + 253)
#define SSC_STSBIT_SYS_255			(STS_BIT_SYS_MIN + 254)
#define SSC_STSBIT_SYS_256			(STS_BIT_SYS_MIN + 255)


/*------------------------------------------*/
/* axis command bits                        */
/*------------------------------------------*/
#define SSC_CMDBIT_AX_SON			(CMD_BIT_AX_MIN + 0)
#define SSC_CMDBIT_AX_2				(CMD_BIT_AX_MIN + 1)
#define SSC_CMDBIT_AX_3				(CMD_BIT_AX_MIN + 2)
#define SSC_CMDBIT_AX_4				(CMD_BIT_AX_MIN + 3)
#define SSC_CMDBIT_AX_TL			(CMD_BIT_AX_MIN + 4)
#define SSC_CMDBIT_AX_SRST			(CMD_BIT_AX_MIN + 5)
#define SSC_CMDBIT_AX_7				(CMD_BIT_AX_MIN + 6)
#define SSC_CMDBIT_AX_8				(CMD_BIT_AX_MIN + 7)

#define SSC_CMDBIT_AX_ST			(CMD_BIT_AX_MIN + 8)
#define SSC_CMDBIT_AX_DIR			(CMD_BIT_AX_MIN + 9)
#define SSC_CMDBIT_AX_STP			(CMD_BIT_AX_MIN + 10)
#define SSC_CMDBIT_AX_RSTP			(CMD_BIT_AX_MIN + 11)
#define SSC_CMDBIT_AX_13			(CMD_BIT_AX_MIN + 12)
#define SSC_CMDBIT_AX_ORST			(CMD_BIT_AX_MIN + 13)
#define SSC_CMDBIT_AX_15			(CMD_BIT_AX_MIN + 14)
#define SSC_CMDBIT_AX_16			(CMD_BIT_AX_MIN + 15)

#define SSC_CMDBIT_AX_AUT			(CMD_BIT_AX_MIN + 16)
#define SSC_CMDBIT_AX_ZRN			(CMD_BIT_AX_MIN + 17)
#define SSC_CMDBIT_AX_JOG			(CMD_BIT_AX_MIN + 18)
#define SSC_CMDBIT_AX_S				(CMD_BIT_AX_MIN + 19)
#define SSC_CMDBIT_AX_21			(CMD_BIT_AX_MIN + 20)
#define SSC_CMDBIT_AX_LIP			(CMD_BIT_AX_MIN + 21)
#define SSC_CMDBIT_AX_DST			(CMD_BIT_AX_MIN + 22)
#define SSC_CMDBIT_AX_24			(CMD_BIT_AX_MIN + 23)

#define SSC_CMDBIT_AX_25			(CMD_BIT_AX_MIN + 24)
#define SSC_CMDBIT_AX_26			(CMD_BIT_AX_MIN + 25)
#define SSC_CMDBIT_AX_27			(CMD_BIT_AX_MIN + 26)
#define SSC_CMDBIT_AX_28			(CMD_BIT_AX_MIN + 27)
#define SSC_CMDBIT_AX_29			(CMD_BIT_AX_MIN + 28)
#define SSC_CMDBIT_AX_30			(CMD_BIT_AX_MIN + 29)
#define SSC_CMDBIT_AX_31			(CMD_BIT_AX_MIN + 30)
#define SSC_CMDBIT_AX_32			(CMD_BIT_AX_MIN + 31)

#define SSC_CMDBIT_AX_ITL			(CMD_BIT_AX_MIN + 32)
#define SSC_CMDBIT_AX_RMONR			(CMD_BIT_AX_MIN + 33)
#define SSC_CMDBIT_AX_35			(CMD_BIT_AX_MIN + 34)
#define SSC_CMDBIT_AX_36			(CMD_BIT_AX_MIN + 35)
#define SSC_CMDBIT_AX_LSPC			(CMD_BIT_AX_MIN + 36)
#define SSC_CMDBIT_AX_LSNC			(CMD_BIT_AX_MIN + 37)
#define SSC_CMDBIT_AX_DOGC			(CMD_BIT_AX_MIN + 38)
#define SSC_CMDBIT_AX_40			(CMD_BIT_AX_MIN + 39)

#define SSC_CMDBIT_AX_SCHG			(CMD_BIT_AX_MIN + 40)
#define SSC_CMDBIT_AX_TACHG			(CMD_BIT_AX_MIN + 41)
#define SSC_CMDBIT_AX_TDCHG			(CMD_BIT_AX_MIN + 42)
#define SSC_CMDBIT_AX_PCHG			(CMD_BIT_AX_MIN + 43)
#define SSC_CMDBIT_AX_45			(CMD_BIT_AX_MIN + 44)
#define SSC_CMDBIT_AX_46			(CMD_BIT_AX_MIN + 45)
#define SSC_CMDBIT_AX_47			(CMD_BIT_AX_MIN + 46)
#define SSC_CMDBIT_AX_48			(CMD_BIT_AX_MIN + 47)

#define SSC_CMDBIT_AX_FST			(CMD_BIT_AX_MIN + 48)
#define SSC_CMDBIT_AX_50			(CMD_BIT_AX_MIN + 49)
#define SSC_CMDBIT_AX_51			(CMD_BIT_AX_MIN + 50)
#define SSC_CMDBIT_AX_52			(CMD_BIT_AX_MIN + 51)
#define SSC_CMDBIT_AX_53			(CMD_BIT_AX_MIN + 52)
#define SSC_CMDBIT_AX_54			(CMD_BIT_AX_MIN + 53)
#define SSC_CMDBIT_AX_55			(CMD_BIT_AX_MIN + 54)
#define SSC_CMDBIT_AX_56			(CMD_BIT_AX_MIN + 55)

#define SSC_CMDBIT_AX_PPISTP		(CMD_BIT_AX_MIN + 56)
#define SSC_CMDBIT_AX_58			(CMD_BIT_AX_MIN + 57)
#define SSC_CMDBIT_AX_59			(CMD_BIT_AX_MIN + 58)
#define SSC_CMDBIT_AX_60			(CMD_BIT_AX_MIN + 59)
#define SSC_CMDBIT_AX_61			(CMD_BIT_AX_MIN + 60)
#define SSC_CMDBIT_AX_62			(CMD_BIT_AX_MIN + 61)
#define SSC_CMDBIT_AX_63			(CMD_BIT_AX_MIN + 62)
#define SSC_CMDBIT_AX_64			(CMD_BIT_AX_MIN + 63)

#define SSC_CMDBIT_AX_GAIN			(CMD_BIT_AX_MIN + 64)
#define SSC_CMDBIT_AX_FCLS			(CMD_BIT_AX_MIN + 65)
#define SSC_CMDBIT_AX_67			(CMD_BIT_AX_MIN + 66)
#define SSC_CMDBIT_AX_CPC			(CMD_BIT_AX_MIN + 67)
#define SSC_CMDBIT_AX_69			(CMD_BIT_AX_MIN + 68)
#define SSC_CMDBIT_AX_70			(CMD_BIT_AX_MIN + 69)
#define SSC_CMDBIT_AX_71			(CMD_BIT_AX_MIN + 70)
#define SSC_CMDBIT_AX_72			(CMD_BIT_AX_MIN + 71)

#define SSC_CMDBIT_AX_73			(CMD_BIT_AX_MIN + 72)
#define SSC_CMDBIT_AX_74			(CMD_BIT_AX_MIN + 73)
#define SSC_CMDBIT_AX_75			(CMD_BIT_AX_MIN + 74)
#define SSC_CMDBIT_AX_76			(CMD_BIT_AX_MIN + 75)
#define SSC_CMDBIT_AX_77			(CMD_BIT_AX_MIN + 76)
#define SSC_CMDBIT_AX_78			(CMD_BIT_AX_MIN + 77)
#define SSC_CMDBIT_AX_79			(CMD_BIT_AX_MIN + 78)
#define SSC_CMDBIT_AX_80			(CMD_BIT_AX_MIN + 79)

#define SSC_CMDBIT_AX_81			(CMD_BIT_AX_MIN + 80)
#define SSC_CMDBIT_AX_82			(CMD_BIT_AX_MIN + 81)
#define SSC_CMDBIT_AX_83			(CMD_BIT_AX_MIN + 82)
#define SSC_CMDBIT_AX_84			(CMD_BIT_AX_MIN + 83)
#define SSC_CMDBIT_AX_ZSC			(CMD_BIT_AX_MIN + 84)
#define SSC_CMDBIT_AX_86			(CMD_BIT_AX_MIN + 85)
#define SSC_CMDBIT_AX_87			(CMD_BIT_AX_MIN + 86)
#define SSC_CMDBIT_AX_88			(CMD_BIT_AX_MIN + 87)

#define SSC_CMDBIT_AX_89			(CMD_BIT_AX_MIN + 88)
#define SSC_CMDBIT_AX_MKC1			(CMD_BIT_AX_MIN + 89)
#define SSC_CMDBIT_AX_MKD1			(CMD_BIT_AX_MIN + 90)
#define SSC_CMDBIT_AX_MKSEN1		(CMD_BIT_AX_MIN + 91)
#define SSC_CMDBIT_AX_93			(CMD_BIT_AX_MIN + 92)
#define SSC_CMDBIT_AX_MKC2			(CMD_BIT_AX_MIN + 93)
#define SSC_CMDBIT_AX_MKD2			(CMD_BIT_AX_MIN + 94)
#define SSC_CMDBIT_AX_MKSEN2		(CMD_BIT_AX_MIN + 95)

#define SSC_CMDBIT_AX_97			(CMD_BIT_AX_MIN + 96)
#define SSC_CMDBIT_AX_98			(CMD_BIT_AX_MIN + 97)
#define SSC_CMDBIT_AX_99			(CMD_BIT_AX_MIN + 98)
#define SSC_CMDBIT_AX_100			(CMD_BIT_AX_MIN + 99)
#define SSC_CMDBIT_AX_CTLMC			(CMD_BIT_AX_MIN + 100)
#define SSC_CMDBIT_AX_102			(CMD_BIT_AX_MIN + 101)
#define SSC_CMDBIT_AX_103			(CMD_BIT_AX_MIN + 102)
#define SSC_CMDBIT_AX_104			(CMD_BIT_AX_MIN + 103)

#define SSC_CMDBIT_AX_105			(CMD_BIT_AX_MIN + 104)
#define SSC_CMDBIT_AX_106			(CMD_BIT_AX_MIN + 105)
#define SSC_CMDBIT_AX_107			(CMD_BIT_AX_MIN + 106)
#define SSC_CMDBIT_AX_108			(CMD_BIT_AX_MIN + 107)
#define SSC_CMDBIT_AX_109			(CMD_BIT_AX_MIN + 108)
#define SSC_CMDBIT_AX_110			(CMD_BIT_AX_MIN + 109)
#define SSC_CMDBIT_AX_111			(CMD_BIT_AX_MIN + 110)
#define SSC_CMDBIT_AX_112			(CMD_BIT_AX_MIN + 111)

#define SSC_CMDBIT_AX_113			(CMD_BIT_AX_MIN + 112)
#define SSC_CMDBIT_AX_114			(CMD_BIT_AX_MIN + 113)
#define SSC_CMDBIT_AX_115			(CMD_BIT_AX_MIN + 114)
#define SSC_CMDBIT_AX_116			(CMD_BIT_AX_MIN + 115)
#define SSC_CMDBIT_AX_117			(CMD_BIT_AX_MIN + 116)
#define SSC_CMDBIT_AX_118			(CMD_BIT_AX_MIN + 117)
#define SSC_CMDBIT_AX_119			(CMD_BIT_AX_MIN + 118)
#define SSC_CMDBIT_AX_120			(CMD_BIT_AX_MIN + 119)

#define SSC_CMDBIT_AX_121			(CMD_BIT_AX_MIN + 120)
#define SSC_CMDBIT_AX_122			(CMD_BIT_AX_MIN + 121)
#define SSC_CMDBIT_AX_123			(CMD_BIT_AX_MIN + 122)
#define SSC_CMDBIT_AX_124			(CMD_BIT_AX_MIN + 123)
#define SSC_CMDBIT_AX_125			(CMD_BIT_AX_MIN + 124)
#define SSC_CMDBIT_AX_126			(CMD_BIT_AX_MIN + 125)
#define SSC_CMDBIT_AX_127			(CMD_BIT_AX_MIN + 126)
#define SSC_CMDBIT_AX_128			(CMD_BIT_AX_MIN + 127)

#define SSC_CMDBIT_AX_MON			(CMD_BIT_AX_MIN + 128)
#define SSC_CMDBIT_AX_MONR			(CMD_BIT_AX_MIN + 129)
#define SSC_CMDBIT_AX_131			(CMD_BIT_AX_MIN + 130)
#define SSC_CMDBIT_AX_132			(CMD_BIT_AX_MIN + 131)
#define SSC_CMDBIT_AX_133			(CMD_BIT_AX_MIN + 132)
#define SSC_CMDBIT_AX_134			(CMD_BIT_AX_MIN + 133)
#define SSC_CMDBIT_AX_135			(CMD_BIT_AX_MIN + 134)
#define SSC_CMDBIT_AX_136			(CMD_BIT_AX_MIN + 135)

#define SSC_CMDBIT_AX_137			(CMD_BIT_AX_MIN + 136)
#define SSC_CMDBIT_AX_138			(CMD_BIT_AX_MIN + 137)
#define SSC_CMDBIT_AX_139			(CMD_BIT_AX_MIN + 138)
#define SSC_CMDBIT_AX_140			(CMD_BIT_AX_MIN + 139)
#define SSC_CMDBIT_AX_141			(CMD_BIT_AX_MIN + 140)
#define SSC_CMDBIT_AX_142			(CMD_BIT_AX_MIN + 141)
#define SSC_CMDBIT_AX_143			(CMD_BIT_AX_MIN + 142)
#define SSC_CMDBIT_AX_144			(CMD_BIT_AX_MIN + 143)

#define SSC_CMDBIT_AX_145			(CMD_BIT_AX_MIN + 144)
#define SSC_CMDBIT_AX_146			(CMD_BIT_AX_MIN + 145)
#define SSC_CMDBIT_AX_147			(CMD_BIT_AX_MIN + 146)
#define SSC_CMDBIT_AX_148			(CMD_BIT_AX_MIN + 147)
#define SSC_CMDBIT_AX_149			(CMD_BIT_AX_MIN + 148)
#define SSC_CMDBIT_AX_150			(CMD_BIT_AX_MIN + 149)
#define SSC_CMDBIT_AX_151			(CMD_BIT_AX_MIN + 150)
#define SSC_CMDBIT_AX_152			(CMD_BIT_AX_MIN + 151)

#define SSC_CMDBIT_AX_153			(CMD_BIT_AX_MIN + 152)
#define SSC_CMDBIT_AX_154			(CMD_BIT_AX_MIN + 153)
#define SSC_CMDBIT_AX_155			(CMD_BIT_AX_MIN + 154)
#define SSC_CMDBIT_AX_156			(CMD_BIT_AX_MIN + 155)
#define SSC_CMDBIT_AX_157			(CMD_BIT_AX_MIN + 156)
#define SSC_CMDBIT_AX_158			(CMD_BIT_AX_MIN + 157)
#define SSC_CMDBIT_AX_159			(CMD_BIT_AX_MIN + 158)
#define SSC_CMDBIT_AX_160			(CMD_BIT_AX_MIN + 159)

#define SSC_CMDBIT_AX_PWRT			(CMD_BIT_AX_MIN + 160)
#define SSC_CMDBIT_AX_162			(CMD_BIT_AX_MIN + 161)
#define SSC_CMDBIT_AX_163			(CMD_BIT_AX_MIN + 162)
#define SSC_CMDBIT_AX_164			(CMD_BIT_AX_MIN + 163)
#define SSC_CMDBIT_AX_165			(CMD_BIT_AX_MIN + 164)
#define SSC_CMDBIT_AX_166			(CMD_BIT_AX_MIN + 165)
#define SSC_CMDBIT_AX_167			(CMD_BIT_AX_MIN + 166)
#define SSC_CMDBIT_AX_PSF			(CMD_BIT_AX_MIN + 167)

#define SSC_CMDBIT_AX_PRD			(CMD_BIT_AX_MIN + 168)
#define SSC_CMDBIT_AX_170			(CMD_BIT_AX_MIN + 169)
#define SSC_CMDBIT_AX_171			(CMD_BIT_AX_MIN + 170)
#define SSC_CMDBIT_AX_172			(CMD_BIT_AX_MIN + 171)
#define SSC_CMDBIT_AX_173			(CMD_BIT_AX_MIN + 172)
#define SSC_CMDBIT_AX_174			(CMD_BIT_AX_MIN + 173)
#define SSC_CMDBIT_AX_175			(CMD_BIT_AX_MIN + 174)
#define SSC_CMDBIT_AX_176			(CMD_BIT_AX_MIN + 175)

#define SSC_CMDBIT_AX_177			(CMD_BIT_AX_MIN + 176)
#define SSC_CMDBIT_AX_178			(CMD_BIT_AX_MIN + 177)
#define SSC_CMDBIT_AX_179			(CMD_BIT_AX_MIN + 178)
#define SSC_CMDBIT_AX_180			(CMD_BIT_AX_MIN + 179)
#define SSC_CMDBIT_AX_181			(CMD_BIT_AX_MIN + 180)
#define SSC_CMDBIT_AX_182			(CMD_BIT_AX_MIN + 181)
#define SSC_CMDBIT_AX_183			(CMD_BIT_AX_MIN + 182)
#define SSC_CMDBIT_AX_184			(CMD_BIT_AX_MIN + 183)

#define SSC_CMDBIT_AX_185			(CMD_BIT_AX_MIN + 184)
#define SSC_CMDBIT_AX_186			(CMD_BIT_AX_MIN + 185)
#define SSC_CMDBIT_AX_187			(CMD_BIT_AX_MIN + 186)
#define SSC_CMDBIT_AX_188			(CMD_BIT_AX_MIN + 187)
#define SSC_CMDBIT_AX_189			(CMD_BIT_AX_MIN + 188)
#define SSC_CMDBIT_AX_190			(CMD_BIT_AX_MIN + 189)
#define SSC_CMDBIT_AX_191			(CMD_BIT_AX_MIN + 190)
#define SSC_CMDBIT_AX_192			(CMD_BIT_AX_MIN + 191)

#define SSC_CMDBIT_AX_193			(CMD_BIT_AX_MIN + 192)
#define SSC_CMDBIT_AX_194			(CMD_BIT_AX_MIN + 193)
#define SSC_CMDBIT_AX_195			(CMD_BIT_AX_MIN + 194)
#define SSC_CMDBIT_AX_196			(CMD_BIT_AX_MIN + 195)
#define SSC_CMDBIT_AX_197			(CMD_BIT_AX_MIN + 196)
#define SSC_CMDBIT_AX_198			(CMD_BIT_AX_MIN + 197)
#define SSC_CMDBIT_AX_199			(CMD_BIT_AX_MIN + 198)
#define SSC_CMDBIT_AX_200			(CMD_BIT_AX_MIN + 199)

#define SSC_CMDBIT_AX_201			(CMD_BIT_AX_MIN + 200)
#define SSC_CMDBIT_AX_202			(CMD_BIT_AX_MIN + 201)
#define SSC_CMDBIT_AX_203			(CMD_BIT_AX_MIN + 202)
#define SSC_CMDBIT_AX_204			(CMD_BIT_AX_MIN + 203)
#define SSC_CMDBIT_AX_205			(CMD_BIT_AX_MIN + 204)
#define SSC_CMDBIT_AX_206			(CMD_BIT_AX_MIN + 205)
#define SSC_CMDBIT_AX_207			(CMD_BIT_AX_MIN + 206)
#define SSC_CMDBIT_AX_208			(CMD_BIT_AX_MIN + 207)

#define SSC_CMDBIT_AX_209			(CMD_BIT_AX_MIN + 208)
#define SSC_CMDBIT_AX_210			(CMD_BIT_AX_MIN + 209)
#define SSC_CMDBIT_AX_211			(CMD_BIT_AX_MIN + 210)
#define SSC_CMDBIT_AX_212			(CMD_BIT_AX_MIN + 211)
#define SSC_CMDBIT_AX_213			(CMD_BIT_AX_MIN + 212)
#define SSC_CMDBIT_AX_214			(CMD_BIT_AX_MIN + 213)
#define SSC_CMDBIT_AX_215			(CMD_BIT_AX_MIN + 214)
#define SSC_CMDBIT_AX_216			(CMD_BIT_AX_MIN + 215)

#define SSC_CMDBIT_AX_217			(CMD_BIT_AX_MIN + 216)
#define SSC_CMDBIT_AX_218			(CMD_BIT_AX_MIN + 217)
#define SSC_CMDBIT_AX_219			(CMD_BIT_AX_MIN + 218)
#define SSC_CMDBIT_AX_220			(CMD_BIT_AX_MIN + 219)
#define SSC_CMDBIT_AX_221			(CMD_BIT_AX_MIN + 220)
#define SSC_CMDBIT_AX_222			(CMD_BIT_AX_MIN + 221)
#define SSC_CMDBIT_AX_223			(CMD_BIT_AX_MIN + 222)
#define SSC_CMDBIT_AX_224			(CMD_BIT_AX_MIN + 223)

#define SSC_CMDBIT_AX_225			(CMD_BIT_AX_MIN + 224)
#define SSC_CMDBIT_AX_226			(CMD_BIT_AX_MIN + 225)
#define SSC_CMDBIT_AX_227			(CMD_BIT_AX_MIN + 226)
#define SSC_CMDBIT_AX_228			(CMD_BIT_AX_MIN + 227)
#define SSC_CMDBIT_AX_229			(CMD_BIT_AX_MIN + 228)
#define SSC_CMDBIT_AX_230			(CMD_BIT_AX_MIN + 229)
#define SSC_CMDBIT_AX_231			(CMD_BIT_AX_MIN + 230)
#define SSC_CMDBIT_AX_232			(CMD_BIT_AX_MIN + 231)

#define SSC_CMDBIT_AX_233			(CMD_BIT_AX_MIN + 232)
#define SSC_CMDBIT_AX_234			(CMD_BIT_AX_MIN + 233)
#define SSC_CMDBIT_AX_235			(CMD_BIT_AX_MIN + 234)
#define SSC_CMDBIT_AX_236			(CMD_BIT_AX_MIN + 235)
#define SSC_CMDBIT_AX_237			(CMD_BIT_AX_MIN + 236)
#define SSC_CMDBIT_AX_238			(CMD_BIT_AX_MIN + 237)
#define SSC_CMDBIT_AX_239			(CMD_BIT_AX_MIN + 238)
#define SSC_CMDBIT_AX_240			(CMD_BIT_AX_MIN + 239)

#define SSC_CMDBIT_AX_241			(CMD_BIT_AX_MIN + 240)
#define SSC_CMDBIT_AX_242			(CMD_BIT_AX_MIN + 241)
#define SSC_CMDBIT_AX_243			(CMD_BIT_AX_MIN + 242)
#define SSC_CMDBIT_AX_244			(CMD_BIT_AX_MIN + 243)
#define SSC_CMDBIT_AX_245			(CMD_BIT_AX_MIN + 244)
#define SSC_CMDBIT_AX_246			(CMD_BIT_AX_MIN + 245)
#define SSC_CMDBIT_AX_247			(CMD_BIT_AX_MIN + 246)
#define SSC_CMDBIT_AX_248			(CMD_BIT_AX_MIN + 247)

#define SSC_CMDBIT_AX_249			(CMD_BIT_AX_MIN + 248)
#define SSC_CMDBIT_AX_250			(CMD_BIT_AX_MIN + 249)
#define SSC_CMDBIT_AX_251			(CMD_BIT_AX_MIN + 250)
#define SSC_CMDBIT_AX_252			(CMD_BIT_AX_MIN + 251)
#define SSC_CMDBIT_AX_253			(CMD_BIT_AX_MIN + 252)
#define SSC_CMDBIT_AX_254			(CMD_BIT_AX_MIN + 253)
#define SSC_CMDBIT_AX_255			(CMD_BIT_AX_MIN + 254)
#define SSC_CMDBIT_AX_256			(CMD_BIT_AX_MIN + 255)


/*------------------------------------------*/
/* axis status bits                         */
/*------------------------------------------*/
#define SSC_STSBIT_AX_RDY			(STS_BIT_AX_MIN + 0)
#define SSC_STSBIT_AX_INP			(STS_BIT_AX_MIN + 1)
#define SSC_STSBIT_AX_ZSP			(STS_BIT_AX_MIN + 2)
#define SSC_STSBIT_AX_ZPAS			(STS_BIT_AX_MIN + 3)
#define SSC_STSBIT_AX_TLC			(STS_BIT_AX_MIN + 4)
#define SSC_STSBIT_AX_SALM			(STS_BIT_AX_MIN + 5)
#define SSC_STSBIT_AX_SWRN			(STS_BIT_AX_MIN + 6)
#define SSC_STSBIT_AX_ABSE			(STS_BIT_AX_MIN + 7)

#define SSC_STSBIT_AX_OP			(STS_BIT_AX_MIN + 8)
#define SSC_STSBIT_AX_CPO			(STS_BIT_AX_MIN + 9)
#define SSC_STSBIT_AX_PF			(STS_BIT_AX_MIN + 10)
#define SSC_STSBIT_AX_ZP			(STS_BIT_AX_MIN + 11)
#define SSC_STSBIT_AX_SMZ			(STS_BIT_AX_MIN + 12)
#define SSC_STSBIT_AX_OALM			(STS_BIT_AX_MIN + 13)
#define SSC_STSBIT_AX_OPF			(STS_BIT_AX_MIN + 14)
#define SSC_STSBIT_AX_PSW			(STS_BIT_AX_MIN + 15)

#define SSC_STSBIT_AX_AUTO			(STS_BIT_AX_MIN + 16)
#define SSC_STSBIT_AX_ZRNO			(STS_BIT_AX_MIN + 17)
#define SSC_STSBIT_AX_JO			(STS_BIT_AX_MIN + 18)
#define SSC_STSBIT_AX_SO			(STS_BIT_AX_MIN + 19)
#define SSC_STSBIT_AX_21			(STS_BIT_AX_MIN + 20)
#define SSC_STSBIT_AX_LIPO			(STS_BIT_AX_MIN + 21)
#define SSC_STSBIT_AX_DSTO			(STS_BIT_AX_MIN + 22)
#define SSC_STSBIT_AX_24			(STS_BIT_AX_MIN + 23)

#define SSC_STSBIT_AX_25			(STS_BIT_AX_MIN + 24)
#define SSC_STSBIT_AX_26			(STS_BIT_AX_MIN + 25)
#define SSC_STSBIT_AX_27			(STS_BIT_AX_MIN + 26)
#define SSC_STSBIT_AX_28			(STS_BIT_AX_MIN + 27)
#define SSC_STSBIT_AX_29			(STS_BIT_AX_MIN + 28)
#define SSC_STSBIT_AX_30			(STS_BIT_AX_MIN + 29)
#define SSC_STSBIT_AX_31			(STS_BIT_AX_MIN + 30)
#define SSC_STSBIT_AX_32			(STS_BIT_AX_MIN + 31)

#define SSC_STSBIT_AX_ISTP			(STS_BIT_AX_MIN + 32)
#define SSC_STSBIT_AX_RMRCH			(STS_BIT_AX_MIN + 33)
#define SSC_STSBIT_AX_POV			(STS_BIT_AX_MIN + 34)
#define SSC_STSBIT_AX_STO			(STS_BIT_AX_MIN + 35)
#define SSC_STSBIT_AX_37			(STS_BIT_AX_MIN + 36)
#define SSC_STSBIT_AX_38			(STS_BIT_AX_MIN + 37)
#define SSC_STSBIT_AX_ZREQ			(STS_BIT_AX_MIN + 38)
#define SSC_STSBIT_AX_40			(STS_BIT_AX_MIN + 39)

#define SSC_STSBIT_AX_SCF			(STS_BIT_AX_MIN + 40)
#define SSC_STSBIT_AX_TACF			(STS_BIT_AX_MIN + 41)
#define SSC_STSBIT_AX_TDCF			(STS_BIT_AX_MIN + 42)
#define SSC_STSBIT_AX_PCF			(STS_BIT_AX_MIN + 43)
#define SSC_STSBIT_AX_SCE			(STS_BIT_AX_MIN + 44)
#define SSC_STSBIT_AX_TACE			(STS_BIT_AX_MIN + 45)
#define SSC_STSBIT_AX_TDCE			(STS_BIT_AX_MIN + 46)
#define SSC_STSBIT_AX_PCE			(STS_BIT_AX_MIN + 47)

#define SSC_STSBIT_AX_49			(STS_BIT_AX_MIN + 48)
#define SSC_STSBIT_AX_50			(STS_BIT_AX_MIN + 49)
#define SSC_STSBIT_AX_51			(STS_BIT_AX_MIN + 50)
#define SSC_STSBIT_AX_52			(STS_BIT_AX_MIN + 51)
#define SSC_STSBIT_AX_53			(STS_BIT_AX_MIN + 52)
#define SSC_STSBIT_AX_54			(STS_BIT_AX_MIN + 53)
#define SSC_STSBIT_AX_55			(STS_BIT_AX_MIN + 54)
#define SSC_STSBIT_AX_56			(STS_BIT_AX_MIN + 55)

#define SSC_STSBIT_AX_PPIOP			(STS_BIT_AX_MIN + 56)
#define SSC_STSBIT_AX_PPIFIN		(STS_BIT_AX_MIN + 57)
#define SSC_STSBIT_AX_PPIERR		(STS_BIT_AX_MIN + 58)
#define SSC_STSBIT_AX_60			(STS_BIT_AX_MIN + 59)
#define SSC_STSBIT_AX_61			(STS_BIT_AX_MIN + 60)
#define SSC_STSBIT_AX_62			(STS_BIT_AX_MIN + 61)
#define SSC_STSBIT_AX_63			(STS_BIT_AX_MIN + 62)
#define SSC_STSBIT_AX_AUTLO			(STS_BIT_AX_MIN + 63)

#define SSC_STSBIT_AX_GAINO			(STS_BIT_AX_MIN + 64)
#define SSC_STSBIT_AX_FCLSO			(STS_BIT_AX_MIN + 65)
#define SSC_STSBIT_AX_TLSO			(STS_BIT_AX_MIN + 66)
#define SSC_STSBIT_AX_SPC			(STS_BIT_AX_MIN + 67)
#define SSC_STSBIT_AX_69			(STS_BIT_AX_MIN + 68)
#define SSC_STSBIT_AX_70			(STS_BIT_AX_MIN + 69)
#define SSC_STSBIT_AX_71			(STS_BIT_AX_MIN + 70)
#define SSC_STSBIT_AX_PRSMO			(STS_BIT_AX_MIN + 71)

#define SSC_STSBIT_AX_IWT			(STS_BIT_AX_MIN + 72)
#define SSC_STSBIT_AX_SINP			(STS_BIT_AX_MIN + 73)
#define SSC_STSBIT_AX_75			(STS_BIT_AX_MIN + 74)
#define SSC_STSBIT_AX_76			(STS_BIT_AX_MIN + 75)
#define SSC_STSBIT_AX_77			(STS_BIT_AX_MIN + 76)
#define SSC_STSBIT_AX_78			(STS_BIT_AX_MIN + 77)
#define SSC_STSBIT_AX_79			(STS_BIT_AX_MIN + 78)
#define SSC_STSBIT_AX_80			(STS_BIT_AX_MIN + 79)

#define SSC_STSBIT_AX_81			(STS_BIT_AX_MIN + 80)
#define SSC_STSBIT_AX_82			(STS_BIT_AX_MIN + 81)
#define SSC_STSBIT_AX_83			(STS_BIT_AX_MIN + 82)
#define SSC_STSBIT_AX_84			(STS_BIT_AX_MIN + 83)
#define SSC_STSBIT_AX_ZSF			(STS_BIT_AX_MIN + 84)
#define SSC_STSBIT_AX_ZSE			(STS_BIT_AX_MIN + 85)
#define SSC_STSBIT_AX_87			(STS_BIT_AX_MIN + 86)
#define SSC_STSBIT_AX_88			(STS_BIT_AX_MIN + 87)

#define SSC_STSBIT_AX_MKIF1			(STS_BIT_AX_MIN + 88)
#define SSC_STSBIT_AX_MKCF1			(STS_BIT_AX_MIN + 89)
#define SSC_STSBIT_AX_MKDO1			(STS_BIT_AX_MIN + 90)
#define SSC_STSBIT_AX_MKSEF1		(STS_BIT_AX_MIN + 91)
#define SSC_STSBIT_AX_MKIF2			(STS_BIT_AX_MIN + 92)
#define SSC_STSBIT_AX_MKCF2			(STS_BIT_AX_MIN + 93)
#define SSC_STSBIT_AX_MKDO2			(STS_BIT_AX_MIN + 94)
#define SSC_STSBIT_AX_MKSEF2		(STS_BIT_AX_MIN + 95)

#define SSC_STSBIT_AX_97			(STS_BIT_AX_MIN + 96)
#define SSC_STSBIT_AX_98			(STS_BIT_AX_MIN + 97)
#define SSC_STSBIT_AX_99			(STS_BIT_AX_MIN + 98)
#define SSC_STSBIT_AX_100			(STS_BIT_AX_MIN + 99)
#define SSC_STSBIT_AX_CTLMCF		(STS_BIT_AX_MIN + 100)
#define SSC_STSBIT_AX_CTLMCE		(STS_BIT_AX_MIN + 101)
#define SSC_STSBIT_AX_103			(STS_BIT_AX_MIN + 102)
#define SSC_STSBIT_AX_104			(STS_BIT_AX_MIN + 103)

#define SSC_STSBIT_AX_105			(STS_BIT_AX_MIN + 104)
#define SSC_STSBIT_AX_106			(STS_BIT_AX_MIN + 105)
#define SSC_STSBIT_AX_107			(STS_BIT_AX_MIN + 106)
#define SSC_STSBIT_AX_108			(STS_BIT_AX_MIN + 107)
#define SSC_STSBIT_AX_109			(STS_BIT_AX_MIN + 108)
#define SSC_STSBIT_AX_110			(STS_BIT_AX_MIN + 109)
#define SSC_STSBIT_AX_111			(STS_BIT_AX_MIN + 110)
#define SSC_STSBIT_AX_112			(STS_BIT_AX_MIN + 111)

#define SSC_STSBIT_AX_113			(STS_BIT_AX_MIN + 112)
#define SSC_STSBIT_AX_114			(STS_BIT_AX_MIN + 113)
#define SSC_STSBIT_AX_115			(STS_BIT_AX_MIN + 114)
#define SSC_STSBIT_AX_116			(STS_BIT_AX_MIN + 115)
#define SSC_STSBIT_AX_117			(STS_BIT_AX_MIN + 116)
#define SSC_STSBIT_AX_118			(STS_BIT_AX_MIN + 117)
#define SSC_STSBIT_AX_119			(STS_BIT_AX_MIN + 118)
#define SSC_STSBIT_AX_120			(STS_BIT_AX_MIN + 119)

#define SSC_STSBIT_AX_121			(STS_BIT_AX_MIN + 120)
#define SSC_STSBIT_AX_122			(STS_BIT_AX_MIN + 121)
#define SSC_STSBIT_AX_123			(STS_BIT_AX_MIN + 122)
#define SSC_STSBIT_AX_124			(STS_BIT_AX_MIN + 123)
#define SSC_STSBIT_AX_125			(STS_BIT_AX_MIN + 124)
#define SSC_STSBIT_AX_126			(STS_BIT_AX_MIN + 125)
#define SSC_STSBIT_AX_127			(STS_BIT_AX_MIN + 126)
#define SSC_STSBIT_AX_128			(STS_BIT_AX_MIN + 127)

#define SSC_STSBIT_AX_MOUT			(STS_BIT_AX_MIN + 128)
#define SSC_STSBIT_AX_MRCH			(STS_BIT_AX_MIN + 129)
#define SSC_STSBIT_AX_MER1			(STS_BIT_AX_MIN + 130)
#define SSC_STSBIT_AX_MER2			(STS_BIT_AX_MIN + 131)
#define SSC_STSBIT_AX_MER3			(STS_BIT_AX_MIN + 132)
#define SSC_STSBIT_AX_MER4			(STS_BIT_AX_MIN + 133)
#define SSC_STSBIT_AX_MESV			(STS_BIT_AX_MIN + 134)
#define SSC_STSBIT_AX_136			(STS_BIT_AX_MIN + 135)

#define SSC_STSBIT_AX_DCMEO			(STS_BIT_AX_MIN + 136)
#define SSC_STSBIT_AX_DCSEO			(STS_BIT_AX_MIN + 137)
#define SSC_STSBIT_AX_139			(STS_BIT_AX_MIN + 138)
#define SSC_STSBIT_AX_140			(STS_BIT_AX_MIN + 139)
#define SSC_STSBIT_AX_141			(STS_BIT_AX_MIN + 140)
#define SSC_STSBIT_AX_142			(STS_BIT_AX_MIN + 141)
#define SSC_STSBIT_AX_143			(STS_BIT_AX_MIN + 142)
#define SSC_STSBIT_AX_144			(STS_BIT_AX_MIN + 143)

#define SSC_STSBIT_AX_145			(STS_BIT_AX_MIN + 144)
#define SSC_STSBIT_AX_146			(STS_BIT_AX_MIN + 145)
#define SSC_STSBIT_AX_147			(STS_BIT_AX_MIN + 146)
#define SSC_STSBIT_AX_148			(STS_BIT_AX_MIN + 147)
#define SSC_STSBIT_AX_149			(STS_BIT_AX_MIN + 148)
#define SSC_STSBIT_AX_150			(STS_BIT_AX_MIN + 149)
#define SSC_STSBIT_AX_151			(STS_BIT_AX_MIN + 150)
#define SSC_STSBIT_AX_152			(STS_BIT_AX_MIN + 151)

#define SSC_STSBIT_AX_153			(STS_BIT_AX_MIN + 152)
#define SSC_STSBIT_AX_154			(STS_BIT_AX_MIN + 153)
#define SSC_STSBIT_AX_155			(STS_BIT_AX_MIN + 154)
#define SSC_STSBIT_AX_156			(STS_BIT_AX_MIN + 155)
#define SSC_STSBIT_AX_157			(STS_BIT_AX_MIN + 156)
#define SSC_STSBIT_AX_158			(STS_BIT_AX_MIN + 157)
#define SSC_STSBIT_AX_159			(STS_BIT_AX_MIN + 158)
#define SSC_STSBIT_AX_160			(STS_BIT_AX_MIN + 159)

#define SSC_STSBIT_AX_PWFIN1		(STS_BIT_AX_MIN + 160)
#define SSC_STSBIT_AX_PWEN1			(STS_BIT_AX_MIN + 161)
#define SSC_STSBIT_AX_PWED1			(STS_BIT_AX_MIN + 162)
#define SSC_STSBIT_AX_164			(STS_BIT_AX_MIN + 163)
#define SSC_STSBIT_AX_PWFIN2		(STS_BIT_AX_MIN + 164)
#define SSC_STSBIT_AX_PWEN2			(STS_BIT_AX_MIN + 165)
#define SSC_STSBIT_AX_PWED2			(STS_BIT_AX_MIN + 166)
#define SSC_STSBIT_AX_PSCHG			(STS_BIT_AX_MIN + 167)

#define SSC_STSBIT_AX_PRFIN1		(STS_BIT_AX_MIN + 168)
#define SSC_STSBIT_AX_PREN1			(STS_BIT_AX_MIN + 169)
#define SSC_STSBIT_AX_PRFIN2		(STS_BIT_AX_MIN + 170)
#define SSC_STSBIT_AX_PREN2			(STS_BIT_AX_MIN + 171)
#define SSC_STSBIT_AX_173			(STS_BIT_AX_MIN + 172)
#define SSC_STSBIT_AX_174			(STS_BIT_AX_MIN + 173)
#define SSC_STSBIT_AX_175			(STS_BIT_AX_MIN + 174)
#define SSC_STSBIT_AX_176			(STS_BIT_AX_MIN + 175)

#define SSC_STSBIT_AX_177			(STS_BIT_AX_MIN + 176)
#define SSC_STSBIT_AX_178			(STS_BIT_AX_MIN + 177)
#define SSC_STSBIT_AX_179			(STS_BIT_AX_MIN + 178)
#define SSC_STSBIT_AX_180			(STS_BIT_AX_MIN + 179)
#define SSC_STSBIT_AX_181			(STS_BIT_AX_MIN + 180)
#define SSC_STSBIT_AX_182			(STS_BIT_AX_MIN + 181)
#define SSC_STSBIT_AX_183			(STS_BIT_AX_MIN + 182)
#define SSC_STSBIT_AX_184			(STS_BIT_AX_MIN + 183)

#define SSC_STSBIT_AX_185			(STS_BIT_AX_MIN + 184)
#define SSC_STSBIT_AX_186			(STS_BIT_AX_MIN + 185)
#define SSC_STSBIT_AX_187			(STS_BIT_AX_MIN + 186)
#define SSC_STSBIT_AX_188			(STS_BIT_AX_MIN + 187)
#define SSC_STSBIT_AX_189			(STS_BIT_AX_MIN + 188)
#define SSC_STSBIT_AX_190			(STS_BIT_AX_MIN + 189)
#define SSC_STSBIT_AX_191			(STS_BIT_AX_MIN + 190)
#define SSC_STSBIT_AX_192			(STS_BIT_AX_MIN + 191)

#define SSC_STSBIT_AX_193			(STS_BIT_AX_MIN + 192)
#define SSC_STSBIT_AX_194			(STS_BIT_AX_MIN + 193)
#define SSC_STSBIT_AX_195			(STS_BIT_AX_MIN + 194)
#define SSC_STSBIT_AX_196			(STS_BIT_AX_MIN + 195)
#define SSC_STSBIT_AX_197			(STS_BIT_AX_MIN + 196)
#define SSC_STSBIT_AX_198			(STS_BIT_AX_MIN + 197)
#define SSC_STSBIT_AX_199			(STS_BIT_AX_MIN + 198)
#define SSC_STSBIT_AX_200			(STS_BIT_AX_MIN + 199)

#define SSC_STSBIT_AX_201			(STS_BIT_AX_MIN + 200)
#define SSC_STSBIT_AX_202			(STS_BIT_AX_MIN + 201)
#define SSC_STSBIT_AX_203			(STS_BIT_AX_MIN + 202)
#define SSC_STSBIT_AX_204			(STS_BIT_AX_MIN + 203)
#define SSC_STSBIT_AX_205			(STS_BIT_AX_MIN + 204)
#define SSC_STSBIT_AX_206			(STS_BIT_AX_MIN + 205)
#define SSC_STSBIT_AX_207			(STS_BIT_AX_MIN + 206)
#define SSC_STSBIT_AX_208			(STS_BIT_AX_MIN + 207)

#define SSC_STSBIT_AX_209			(STS_BIT_AX_MIN + 208)
#define SSC_STSBIT_AX_210			(STS_BIT_AX_MIN + 209)
#define SSC_STSBIT_AX_211			(STS_BIT_AX_MIN + 210)
#define SSC_STSBIT_AX_212			(STS_BIT_AX_MIN + 211)
#define SSC_STSBIT_AX_213			(STS_BIT_AX_MIN + 212)
#define SSC_STSBIT_AX_214			(STS_BIT_AX_MIN + 213)
#define SSC_STSBIT_AX_215			(STS_BIT_AX_MIN + 214)
#define SSC_STSBIT_AX_216			(STS_BIT_AX_MIN + 215)

#define SSC_STSBIT_AX_217			(STS_BIT_AX_MIN + 216)
#define SSC_STSBIT_AX_218			(STS_BIT_AX_MIN + 217)
#define SSC_STSBIT_AX_219			(STS_BIT_AX_MIN + 218)
#define SSC_STSBIT_AX_220			(STS_BIT_AX_MIN + 219)
#define SSC_STSBIT_AX_221			(STS_BIT_AX_MIN + 220)
#define SSC_STSBIT_AX_222			(STS_BIT_AX_MIN + 221)
#define SSC_STSBIT_AX_223			(STS_BIT_AX_MIN + 222)
#define SSC_STSBIT_AX_224			(STS_BIT_AX_MIN + 223)

#define SSC_STSBIT_AX_225			(STS_BIT_AX_MIN + 224)
#define SSC_STSBIT_AX_226			(STS_BIT_AX_MIN + 225)
#define SSC_STSBIT_AX_227			(STS_BIT_AX_MIN + 226)
#define SSC_STSBIT_AX_228			(STS_BIT_AX_MIN + 227)
#define SSC_STSBIT_AX_229			(STS_BIT_AX_MIN + 228)
#define SSC_STSBIT_AX_230			(STS_BIT_AX_MIN + 229)
#define SSC_STSBIT_AX_231			(STS_BIT_AX_MIN + 230)
#define SSC_STSBIT_AX_232			(STS_BIT_AX_MIN + 231)

#define SSC_STSBIT_AX_233			(STS_BIT_AX_MIN + 232)
#define SSC_STSBIT_AX_234			(STS_BIT_AX_MIN + 233)
#define SSC_STSBIT_AX_235			(STS_BIT_AX_MIN + 234)
#define SSC_STSBIT_AX_236			(STS_BIT_AX_MIN + 235)
#define SSC_STSBIT_AX_237			(STS_BIT_AX_MIN + 236)
#define SSC_STSBIT_AX_238			(STS_BIT_AX_MIN + 237)
#define SSC_STSBIT_AX_239			(STS_BIT_AX_MIN + 238)
#define SSC_STSBIT_AX_240			(STS_BIT_AX_MIN + 239)

#define SSC_STSBIT_AX_241			(STS_BIT_AX_MIN + 240)
#define SSC_STSBIT_AX_242			(STS_BIT_AX_MIN + 241)
#define SSC_STSBIT_AX_243			(STS_BIT_AX_MIN + 242)
#define SSC_STSBIT_AX_244			(STS_BIT_AX_MIN + 243)
#define SSC_STSBIT_AX_245			(STS_BIT_AX_MIN + 244)
#define SSC_STSBIT_AX_246			(STS_BIT_AX_MIN + 245)
#define SSC_STSBIT_AX_247			(STS_BIT_AX_MIN + 246)
#define SSC_STSBIT_AX_248			(STS_BIT_AX_MIN + 247)

#define SSC_STSBIT_AX_249			(STS_BIT_AX_MIN + 248)
#define SSC_STSBIT_AX_250			(STS_BIT_AX_MIN + 249)
#define SSC_STSBIT_AX_251			(STS_BIT_AX_MIN + 250)
#define SSC_STSBIT_AX_252			(STS_BIT_AX_MIN + 251)
#define SSC_STSBIT_AX_253			(STS_BIT_AX_MIN + 252)
#define SSC_STSBIT_AX_254			(STS_BIT_AX_MIN + 253)
#define SSC_STSBIT_AX_255			(STS_BIT_AX_MIN + 254)
#define SSC_STSBIT_AX_256			(STS_BIT_AX_MIN + 255)


/*------------------------------------------*/
/* unit command bits                        */
/*------------------------------------------*/
#define SSC_CMDBIT_UT_1				(CMD_BIT_UT_MIN + 0)
#define SSC_CMDBIT_UT_2				(CMD_BIT_UT_MIN + 1)
#define SSC_CMDBIT_UT_3				(CMD_BIT_UT_MIN + 2)
#define SSC_CMDBIT_UT_4				(CMD_BIT_UT_MIN + 3)
#define SSC_CMDBIT_UT_5				(CMD_BIT_UT_MIN + 4)
#define SSC_CMDBIT_UT_RURST			(CMD_BIT_UT_MIN + 5)
#define SSC_CMDBIT_UT_7				(CMD_BIT_UT_MIN + 6)
#define SSC_CMDBIT_UT_8				(CMD_BIT_UT_MIN + 7)

#define SSC_CMDBIT_UT_9				(CMD_BIT_UT_MIN + 8)
#define SSC_CMDBIT_UT_10			(CMD_BIT_UT_MIN + 9)
#define SSC_CMDBIT_UT_11			(CMD_BIT_UT_MIN + 10)
#define SSC_CMDBIT_UT_12			(CMD_BIT_UT_MIN + 11)
#define SSC_CMDBIT_UT_13			(CMD_BIT_UT_MIN + 12)
#define SSC_CMDBIT_UT_RCRST			(CMD_BIT_UT_MIN + 13)
#define SSC_CMDBIT_UT_15			(CMD_BIT_UT_MIN + 14)
#define SSC_CMDBIT_UT_16			(CMD_BIT_UT_MIN + 15)

#define SSC_CMDBIT_UT_17			(CMD_BIT_UT_MIN + 16)
#define SSC_CMDBIT_UT_18			(CMD_BIT_UT_MIN + 17)
#define SSC_CMDBIT_UT_19			(CMD_BIT_UT_MIN + 18)
#define SSC_CMDBIT_UT_20			(CMD_BIT_UT_MIN + 19)
#define SSC_CMDBIT_UT_21			(CMD_BIT_UT_MIN + 20)
#define SSC_CMDBIT_UT_22			(CMD_BIT_UT_MIN + 21)
#define SSC_CMDBIT_UT_23			(CMD_BIT_UT_MIN + 22)
#define SSC_CMDBIT_UT_24			(CMD_BIT_UT_MIN + 23)

#define SSC_CMDBIT_UT_25			(CMD_BIT_UT_MIN + 24)
#define SSC_CMDBIT_UT_26			(CMD_BIT_UT_MIN + 25)
#define SSC_CMDBIT_UT_27			(CMD_BIT_UT_MIN + 26)
#define SSC_CMDBIT_UT_28			(CMD_BIT_UT_MIN + 27)
#define SSC_CMDBIT_UT_29			(CMD_BIT_UT_MIN + 28)
#define SSC_CMDBIT_UT_30			(CMD_BIT_UT_MIN + 29)
#define SSC_CMDBIT_UT_31			(CMD_BIT_UT_MIN + 30)
#define SSC_CMDBIT_UT_32			(CMD_BIT_UT_MIN + 31)

#define SSC_CMDBIT_UT_MON			(CMD_BIT_UT_MIN + 32)
#define SSC_CMDBIT_UT_MONR			(CMD_BIT_UT_MIN + 33)
#define SSC_CMDBIT_UT_35			(CMD_BIT_UT_MIN + 34)
#define SSC_CMDBIT_UT_36			(CMD_BIT_UT_MIN + 35)
#define SSC_CMDBIT_UT_37			(CMD_BIT_UT_MIN + 36)
#define SSC_CMDBIT_UT_38			(CMD_BIT_UT_MIN + 37)
#define SSC_CMDBIT_UT_39			(CMD_BIT_UT_MIN + 38)
#define SSC_CMDBIT_UT_40			(CMD_BIT_UT_MIN + 39)

#define SSC_CMDBIT_UT_41			(CMD_BIT_UT_MIN + 40)
#define SSC_CMDBIT_UT_42			(CMD_BIT_UT_MIN + 41)
#define SSC_CMDBIT_UT_43			(CMD_BIT_UT_MIN + 42)
#define SSC_CMDBIT_UT_44			(CMD_BIT_UT_MIN + 43)
#define SSC_CMDBIT_UT_45			(CMD_BIT_UT_MIN + 44)
#define SSC_CMDBIT_UT_46			(CMD_BIT_UT_MIN + 45)
#define SSC_CMDBIT_UT_47			(CMD_BIT_UT_MIN + 46)
#define SSC_CMDBIT_UT_48			(CMD_BIT_UT_MIN + 47)

#define SSC_CMDBIT_UT_PWRT			(CMD_BIT_UT_MIN + 48)
#define SSC_CMDBIT_UT_50			(CMD_BIT_UT_MIN + 49)
#define SSC_CMDBIT_UT_51			(CMD_BIT_UT_MIN + 50)
#define SSC_CMDBIT_UT_52			(CMD_BIT_UT_MIN + 51)
#define SSC_CMDBIT_UT_53			(CMD_BIT_UT_MIN + 52)
#define SSC_CMDBIT_UT_54			(CMD_BIT_UT_MIN + 53)
#define SSC_CMDBIT_UT_55			(CMD_BIT_UT_MIN + 54)
#define SSC_CMDBIT_UT_56			(CMD_BIT_UT_MIN + 55)

#define SSC_CMDBIT_UT_PRD			(CMD_BIT_UT_MIN + 56)
#define SSC_CMDBIT_UT_58			(CMD_BIT_UT_MIN + 57)
#define SSC_CMDBIT_UT_59			(CMD_BIT_UT_MIN + 58)
#define SSC_CMDBIT_UT_60			(CMD_BIT_UT_MIN + 59)
#define SSC_CMDBIT_UT_61			(CMD_BIT_UT_MIN + 60)
#define SSC_CMDBIT_UT_62			(CMD_BIT_UT_MIN + 61)
#define SSC_CMDBIT_UT_63			(CMD_BIT_UT_MIN + 62)
#define SSC_CMDBIT_UT_64			(CMD_BIT_UT_MIN + 63)

#define SSC_CMDBIT_UT_65			(CMD_BIT_UT_MIN + 64)
#define SSC_CMDBIT_UT_66			(CMD_BIT_UT_MIN + 65)
#define SSC_CMDBIT_UT_67			(CMD_BIT_UT_MIN + 66)
#define SSC_CMDBIT_UT_68			(CMD_BIT_UT_MIN + 67)
#define SSC_CMDBIT_UT_69			(CMD_BIT_UT_MIN + 68)
#define SSC_CMDBIT_UT_70			(CMD_BIT_UT_MIN + 69)
#define SSC_CMDBIT_UT_71			(CMD_BIT_UT_MIN + 70)
#define SSC_CMDBIT_UT_72			(CMD_BIT_UT_MIN + 71)

#define SSC_CMDBIT_UT_73			(CMD_BIT_UT_MIN + 72)
#define SSC_CMDBIT_UT_74			(CMD_BIT_UT_MIN + 73)
#define SSC_CMDBIT_UT_75			(CMD_BIT_UT_MIN + 74)
#define SSC_CMDBIT_UT_76			(CMD_BIT_UT_MIN + 75)
#define SSC_CMDBIT_UT_77			(CMD_BIT_UT_MIN + 76)
#define SSC_CMDBIT_UT_78			(CMD_BIT_UT_MIN + 77)
#define SSC_CMDBIT_UT_79			(CMD_BIT_UT_MIN + 78)
#define SSC_CMDBIT_UT_80			(CMD_BIT_UT_MIN + 79)

#define SSC_CMDBIT_UT_81			(CMD_BIT_UT_MIN + 80)
#define SSC_CMDBIT_UT_82			(CMD_BIT_UT_MIN + 81)
#define SSC_CMDBIT_UT_83			(CMD_BIT_UT_MIN + 82)
#define SSC_CMDBIT_UT_84			(CMD_BIT_UT_MIN + 83)
#define SSC_CMDBIT_UT_85			(CMD_BIT_UT_MIN + 84)
#define SSC_CMDBIT_UT_86			(CMD_BIT_UT_MIN + 85)
#define SSC_CMDBIT_UT_87			(CMD_BIT_UT_MIN + 86)
#define SSC_CMDBIT_UT_88			(CMD_BIT_UT_MIN + 87)

#define SSC_CMDBIT_UT_89			(CMD_BIT_UT_MIN + 88)
#define SSC_CMDBIT_UT_90			(CMD_BIT_UT_MIN + 89)
#define SSC_CMDBIT_UT_91			(CMD_BIT_UT_MIN + 90)
#define SSC_CMDBIT_UT_92			(CMD_BIT_UT_MIN + 91)
#define SSC_CMDBIT_UT_93			(CMD_BIT_UT_MIN + 92)
#define SSC_CMDBIT_UT_94			(CMD_BIT_UT_MIN + 93)
#define SSC_CMDBIT_UT_95			(CMD_BIT_UT_MIN + 94)
#define SSC_CMDBIT_UT_96			(CMD_BIT_UT_MIN + 95)

#define SSC_CMDBIT_UT_97			(CMD_BIT_UT_MIN + 96)
#define SSC_CMDBIT_UT_98			(CMD_BIT_UT_MIN + 97)
#define SSC_CMDBIT_UT_99			(CMD_BIT_UT_MIN + 98)
#define SSC_CMDBIT_UT_100			(CMD_BIT_UT_MIN + 99)
#define SSC_CMDBIT_UT_101			(CMD_BIT_UT_MIN + 100)
#define SSC_CMDBIT_UT_102			(CMD_BIT_UT_MIN + 101)
#define SSC_CMDBIT_UT_103			(CMD_BIT_UT_MIN + 102)
#define SSC_CMDBIT_UT_104			(CMD_BIT_UT_MIN + 103)

#define SSC_CMDBIT_UT_105			(CMD_BIT_UT_MIN + 104)
#define SSC_CMDBIT_UT_106			(CMD_BIT_UT_MIN + 105)
#define SSC_CMDBIT_UT_107			(CMD_BIT_UT_MIN + 106)
#define SSC_CMDBIT_UT_108			(CMD_BIT_UT_MIN + 107)
#define SSC_CMDBIT_UT_109			(CMD_BIT_UT_MIN + 108)
#define SSC_CMDBIT_UT_110			(CMD_BIT_UT_MIN + 109)
#define SSC_CMDBIT_UT_111			(CMD_BIT_UT_MIN + 110)
#define SSC_CMDBIT_UT_112			(CMD_BIT_UT_MIN + 111)

#define SSC_CMDBIT_UT_113			(CMD_BIT_UT_MIN + 112)
#define SSC_CMDBIT_UT_114			(CMD_BIT_UT_MIN + 113)
#define SSC_CMDBIT_UT_115			(CMD_BIT_UT_MIN + 114)
#define SSC_CMDBIT_UT_116			(CMD_BIT_UT_MIN + 115)
#define SSC_CMDBIT_UT_117			(CMD_BIT_UT_MIN + 116)
#define SSC_CMDBIT_UT_118			(CMD_BIT_UT_MIN + 117)
#define SSC_CMDBIT_UT_119			(CMD_BIT_UT_MIN + 118)
#define SSC_CMDBIT_UT_120			(CMD_BIT_UT_MIN + 119)

#define SSC_CMDBIT_UT_121			(CMD_BIT_UT_MIN + 120)
#define SSC_CMDBIT_UT_122			(CMD_BIT_UT_MIN + 121)
#define SSC_CMDBIT_UT_123			(CMD_BIT_UT_MIN + 122)
#define SSC_CMDBIT_UT_124			(CMD_BIT_UT_MIN + 123)
#define SSC_CMDBIT_UT_125			(CMD_BIT_UT_MIN + 124)
#define SSC_CMDBIT_UT_126			(CMD_BIT_UT_MIN + 125)
#define SSC_CMDBIT_UT_127			(CMD_BIT_UT_MIN + 126)
#define SSC_CMDBIT_UT_128			(CMD_BIT_UT_MIN + 127)

/*------------------------------------------*/
/* unit status bits                         */
/*------------------------------------------*/
#define SSC_STSBIT_UT_RURDY			(STS_BIT_UT_MIN + 0)
#define SSC_STSBIT_UT_RUA			(STS_BIT_UT_MIN + 1)
#define SSC_STSBIT_UT_3				(STS_BIT_UT_MIN + 2)
#define SSC_STSBIT_UT_4				(STS_BIT_UT_MIN + 3)
#define SSC_STSBIT_UT_5				(STS_BIT_UT_MIN + 4)
#define SSC_STSBIT_UT_RUALM			(STS_BIT_UT_MIN + 5)
#define SSC_STSBIT_UT_RUWRN			(STS_BIT_UT_MIN + 6)
#define SSC_STSBIT_UT_8				(STS_BIT_UT_MIN + 7)

#define SSC_STSBIT_UT_9				(STS_BIT_UT_MIN + 8)
#define SSC_STSBIT_UT_10			(STS_BIT_UT_MIN + 9)
#define SSC_STSBIT_UT_11			(STS_BIT_UT_MIN + 10)
#define SSC_STSBIT_UT_12			(STS_BIT_UT_MIN + 11)
#define SSC_STSBIT_UT_13			(STS_BIT_UT_MIN + 12)
#define SSC_STSBIT_UT_RCALM			(STS_BIT_UT_MIN + 13)
#define SSC_STSBIT_UT_15			(STS_BIT_UT_MIN + 14)
#define SSC_STSBIT_UT_16			(STS_BIT_UT_MIN + 15)

#define SSC_STSBIT_UT_17			(STS_BIT_UT_MIN + 16)
#define SSC_STSBIT_UT_18			(STS_BIT_UT_MIN + 17)
#define SSC_STSBIT_UT_19			(STS_BIT_UT_MIN + 18)
#define SSC_STSBIT_UT_20			(STS_BIT_UT_MIN + 19)
#define SSC_STSBIT_UT_21			(STS_BIT_UT_MIN + 20)
#define SSC_STSBIT_UT_22			(STS_BIT_UT_MIN + 21)
#define SSC_STSBIT_UT_23			(STS_BIT_UT_MIN + 22)
#define SSC_STSBIT_UT_24			(STS_BIT_UT_MIN + 23)

#define SSC_STSBIT_UT_25			(STS_BIT_UT_MIN + 24)
#define SSC_STSBIT_UT_26			(STS_BIT_UT_MIN + 25)
#define SSC_STSBIT_UT_27			(STS_BIT_UT_MIN + 26)
#define SSC_STSBIT_UT_28			(STS_BIT_UT_MIN + 27)
#define SSC_STSBIT_UT_29			(STS_BIT_UT_MIN + 28)
#define SSC_STSBIT_UT_30			(STS_BIT_UT_MIN + 29)
#define SSC_STSBIT_UT_31			(STS_BIT_UT_MIN + 30)
#define SSC_STSBIT_UT_32			(STS_BIT_UT_MIN + 31)

#define SSC_STSBIT_UT_MOUT			(STS_BIT_UT_MIN + 32)
#define SSC_STSBIT_UT_MRCH			(STS_BIT_UT_MIN + 33)
#define SSC_STSBIT_UT_MER1			(STS_BIT_UT_MIN + 34)
#define SSC_STSBIT_UT_MER2			(STS_BIT_UT_MIN + 35)
#define SSC_STSBIT_UT_MER3			(STS_BIT_UT_MIN + 36)
#define SSC_STSBIT_UT_MER4			(STS_BIT_UT_MIN + 37)
#define SSC_STSBIT_UT_MERIO			(STS_BIT_UT_MIN + 38)
#define SSC_STSBIT_UT_40			(STS_BIT_UT_MIN + 39)

#define SSC_STSBIT_UT_41			(STS_BIT_UT_MIN + 40)
#define SSC_STSBIT_UT_42			(STS_BIT_UT_MIN + 41)
#define SSC_STSBIT_UT_43			(STS_BIT_UT_MIN + 42)
#define SSC_STSBIT_UT_44			(STS_BIT_UT_MIN + 43)
#define SSC_STSBIT_UT_45			(STS_BIT_UT_MIN + 44)
#define SSC_STSBIT_UT_46			(STS_BIT_UT_MIN + 45)
#define SSC_STSBIT_UT_47			(STS_BIT_UT_MIN + 46)
#define SSC_STSBIT_UT_48			(STS_BIT_UT_MIN + 47)

#define SSC_STSBIT_UT_PWFIN1		(STS_BIT_UT_MIN + 48)
#define SSC_STSBIT_UT_PWEN1			(STS_BIT_UT_MIN + 49)
#define SSC_STSBIT_UT_PWED1			(STS_BIT_UT_MIN + 50)
#define SSC_STSBIT_UT_52			(STS_BIT_UT_MIN + 51)
#define SSC_STSBIT_UT_PWFIN2		(STS_BIT_UT_MIN + 52)
#define SSC_STSBIT_UT_PWEN2			(STS_BIT_UT_MIN + 53)
#define SSC_STSBIT_UT_PWED2			(STS_BIT_UT_MIN + 54)
#define SSC_STSBIT_UT_56			(STS_BIT_UT_MIN + 55)

#define SSC_STSBIT_UT_PRFIN1		(STS_BIT_UT_MIN + 56)
#define SSC_STSBIT_UT_PREN1			(STS_BIT_UT_MIN + 57)
#define SSC_STSBIT_UT_PRFIN2		(STS_BIT_UT_MIN + 58)
#define SSC_STSBIT_UT_PREN2			(STS_BIT_UT_MIN + 59)
#define SSC_STSBIT_UT_61			(STS_BIT_UT_MIN + 60)
#define SSC_STSBIT_UT_62			(STS_BIT_UT_MIN + 61)
#define SSC_STSBIT_UT_63			(STS_BIT_UT_MIN + 62)
#define SSC_STSBIT_UT_64			(STS_BIT_UT_MIN + 63)

#define SSC_STSBIT_UT_65			(STS_BIT_UT_MIN + 64)
#define SSC_STSBIT_UT_66			(STS_BIT_UT_MIN + 65)
#define SSC_STSBIT_UT_67			(STS_BIT_UT_MIN + 66)
#define SSC_STSBIT_UT_68			(STS_BIT_UT_MIN + 67)
#define SSC_STSBIT_UT_69			(STS_BIT_UT_MIN + 68)
#define SSC_STSBIT_UT_70			(STS_BIT_UT_MIN + 69)
#define SSC_STSBIT_UT_71			(STS_BIT_UT_MIN + 70)
#define SSC_STSBIT_UT_72			(STS_BIT_UT_MIN + 71)

#define SSC_STSBIT_UT_73			(STS_BIT_UT_MIN + 72)
#define SSC_STSBIT_UT_74			(STS_BIT_UT_MIN + 73)
#define SSC_STSBIT_UT_75			(STS_BIT_UT_MIN + 74)
#define SSC_STSBIT_UT_76			(STS_BIT_UT_MIN + 75)
#define SSC_STSBIT_UT_77			(STS_BIT_UT_MIN + 76)
#define SSC_STSBIT_UT_78			(STS_BIT_UT_MIN + 77)
#define SSC_STSBIT_UT_79			(STS_BIT_UT_MIN + 78)
#define SSC_STSBIT_UT_80			(STS_BIT_UT_MIN + 79)

#define SSC_STSBIT_UT_81			(STS_BIT_UT_MIN + 80)
#define SSC_STSBIT_UT_82			(STS_BIT_UT_MIN + 81)
#define SSC_STSBIT_UT_83			(STS_BIT_UT_MIN + 82)
#define SSC_STSBIT_UT_84			(STS_BIT_UT_MIN + 83)
#define SSC_STSBIT_UT_85			(STS_BIT_UT_MIN + 84)
#define SSC_STSBIT_UT_86			(STS_BIT_UT_MIN + 85)
#define SSC_STSBIT_UT_87			(STS_BIT_UT_MIN + 86)
#define SSC_STSBIT_UT_88			(STS_BIT_UT_MIN + 87)

#define SSC_STSBIT_UT_89			(STS_BIT_UT_MIN + 88)
#define SSC_STSBIT_UT_90			(STS_BIT_UT_MIN + 89)
#define SSC_STSBIT_UT_91			(STS_BIT_UT_MIN + 90)
#define SSC_STSBIT_UT_92			(STS_BIT_UT_MIN + 91)
#define SSC_STSBIT_UT_93			(STS_BIT_UT_MIN + 92)
#define SSC_STSBIT_UT_94			(STS_BIT_UT_MIN + 93)
#define SSC_STSBIT_UT_95			(STS_BIT_UT_MIN + 94)
#define SSC_STSBIT_UT_96			(STS_BIT_UT_MIN + 95)

#define SSC_STSBIT_UT_97			(STS_BIT_UT_MIN + 96)
#define SSC_STSBIT_UT_98			(STS_BIT_UT_MIN + 97)
#define SSC_STSBIT_UT_99			(STS_BIT_UT_MIN + 98)
#define SSC_STSBIT_UT_100			(STS_BIT_UT_MIN + 99)
#define SSC_STSBIT_UT_101			(STS_BIT_UT_MIN + 100)
#define SSC_STSBIT_UT_102			(STS_BIT_UT_MIN + 101)
#define SSC_STSBIT_UT_103			(STS_BIT_UT_MIN + 102)
#define SSC_STSBIT_UT_104			(STS_BIT_UT_MIN + 103)

#define SSC_STSBIT_UT_105			(STS_BIT_UT_MIN + 104)
#define SSC_STSBIT_UT_106			(STS_BIT_UT_MIN + 105)
#define SSC_STSBIT_UT_107			(STS_BIT_UT_MIN + 106)
#define SSC_STSBIT_UT_108			(STS_BIT_UT_MIN + 107)
#define SSC_STSBIT_UT_109			(STS_BIT_UT_MIN + 108)
#define SSC_STSBIT_UT_110			(STS_BIT_UT_MIN + 109)
#define SSC_STSBIT_UT_111			(STS_BIT_UT_MIN + 110)
#define SSC_STSBIT_UT_112			(STS_BIT_UT_MIN + 111)

#define SSC_STSBIT_UT_113			(STS_BIT_UT_MIN + 112)
#define SSC_STSBIT_UT_114			(STS_BIT_UT_MIN + 113)
#define SSC_STSBIT_UT_115			(STS_BIT_UT_MIN + 114)
#define SSC_STSBIT_UT_116			(STS_BIT_UT_MIN + 115)
#define SSC_STSBIT_UT_117			(STS_BIT_UT_MIN + 116)
#define SSC_STSBIT_UT_118			(STS_BIT_UT_MIN + 117)
#define SSC_STSBIT_UT_119			(STS_BIT_UT_MIN + 118)
#define SSC_STSBIT_UT_120			(STS_BIT_UT_MIN + 119)

#define SSC_STSBIT_UT_121			(STS_BIT_UT_MIN + 120)
#define SSC_STSBIT_UT_122			(STS_BIT_UT_MIN + 121)
#define SSC_STSBIT_UT_123			(STS_BIT_UT_MIN + 122)
#define SSC_STSBIT_UT_124			(STS_BIT_UT_MIN + 123)
#define SSC_STSBIT_UT_125			(STS_BIT_UT_MIN + 124)
#define SSC_STSBIT_UT_126			(STS_BIT_UT_MIN + 125)
#define SSC_STSBIT_UT_127			(STS_BIT_UT_MIN + 126)
#define SSC_STSBIT_UT_128			(STS_BIT_UT_MIN + 127)

/*************************************************************************/
#endif /* mc2xxFuncUserDefH */
/*************************************************************************/


/* End of Files */


