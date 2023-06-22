#ifndef __ICCP_FUNCS_H
#define __ICCP_FUNCS_H


#include <stdint.h>

#define tickspersec  20 //was 10
#define psufail_cnt_max 10 //Was 10

#define BITCHK(Variable, Position) (((Variable)>>(Position)) & 1UL)

#define QTYNODES (4u)
#define FIRSTNODE (0u)


// Data mapping of NOR flash for ICCP funcs


typedef struct {
	uint8_t dhcp; //1= dhcp ON 0=static ip
	uint8_t ip[4]; // ip address if static mode
	uint8_t mac[6]; // mac address 
	uint8_t nm[4]; // net mask default 255,255,255,0
	uint8_t gwy[4];// gateway default 192.168.1.0
	char    hn[15];// host name default "tdi_iccp_xxx" xxx equal last octet IP address
	char    alias[16]; //net alias
	uint16_t canbd; // can baudrate
	uint8_t  canlpb; // CAN loop back 1= loop back on 0= off default OFF ( loopback whatever tx = rx )
} NetworkParam_t; //Network related Parameters


typedef enum {P_CV,P_CI,P_CPOT,P_OFF} psu_mode_t;
//ICCP Run Time Profile Struct
typedef struct {
	psu_mode_t psu_mode;// P_CV,P_CI,P_CPOT,P_OFF
	psu_mode_t last_psu_mode;// P_CV,P_CI,P_CPOT,P_OFF
	int16_t refelec_min_th;   // resolution 1mv/bit  -2048 = -.5Vdc  +2048= +.5Vdc
	int16_t refelec_dmd_setpt;// EAJ Added per VK 11/11/2015 - resolution 1mv/bit  -2048 = -.5Vdc  +2048= +.5Vdc
	int16_t refelec_max_th;   // resolution 1mv/bit  -2048 = -.5Vdc  +2048= +.5Vdc
	uint16_t rec_vset;        // resolution 1mv/bit 4096 = Full Scale
	uint16_t rec_vset_max_th; // resolution 100mv/bit 4096 = Full Scale
	uint16_t rec_vset_min_th; // resolution 100mv/bit
	uint16_t rec_fs_vset;     // resolution = vset_fs / 4096 ex 63V/4096 = 15mA/bit for a 54V rect. this would be the OV point or 63v= 630bits
	uint16_t rec_iset; 				// resolution = iset_fs/4096 ex. 125A/4096 = 30mA/bit
	uint16_t rec_iset_max_th; // resolution 1A/bit
	uint16_t rec_iset_min_th; // resolution 1A/bit
	uint16_t rec_fs_iset;     // resolution 1A/bit 
	uint16_t alarm_reg_mask;  // 0= bit masked 1= active, same bit mapping as alarm register
	uint16_t self_test_int;// interval self test is run at in minutes/bit 0= disbaled
	uint8_t adc_smp_int;// interval adc samples are placed into 128 sample array mS/bit 0=uses real time values for debug purposes
	uint8_t control_int;// interval the rectifer output current is updated based on the ref elec AVG buffers. second/bit 
	uint32_t tsts_active; // bit mapped to each test in the self test routine, to allow us to select which tests will be run
	uint8_t  refelec_active; //
	char  refeleclongname[4][16]; // 16 character long name descriptor for each reference electrode sensor   
			
} IccpPrfParam_t; //Iccp Profile Parameters


/* Enumerations -------------------------------------------------------------*/
typedef enum {LBLD, IPSM, RECT_END} Rect_t;
typedef enum {I_OFF,I_HLLO,I_PFL,I_STAQRY,I_STARSP,I_FAIL } init_state_t;

/* Structures ---------------------------------------------------------------*/
typedef struct {
	IccpPrfParam_t    pflparams;
	init_state_t      init_state;// {I_OFF,I_HLLO,I_PFL,I_STA,I_FAIL } init_state_t;
	Rect_t		        Rectifier;
	uint16_t		      AlmReg;		// bit  0: output on\off 0=off
	                            // bit  1: DCOK 
											        // bit  2: ACOK
	                            // bit  3: RECT SUM FLT
															// bit  4: Manual Mode 0= auto 1= manual
															// bit  5: output on\off 0=off
															// bit  6: ref_elec 0=passing 1=at least one refelec is failed
															// bit  7: communications fault CAN stopped for >2 sec
															// bit  8: surge protector flt detected
															// bit  9: Current Imbalance
															// bit 10: Door Status 0= closed 1=open
															// bit 11: Last Self Test 0=failed 1=passed
	
	uint32_t         TestStaReg;// bit  0: test1
	                            // bit  1: test2 
											        // bit  2: 
	                            // bit  3: 
															// bit  4-31: 				
	uint8_t		        PsuTemp;
	uint8_t		        ModelNumber[16];
	uint8_t		        SerialNumber[16];
	uint8_t		        FirmwareVers[16];
	uint16_t	        RefCells[4];
	uint8_t             RfClsGd;// Qty Ref Cells measuring within bounds "good"
	uint16_t	        Irdbk;
	uint16_t	        Vrdbk;
	uint16_t 			rec_vset; // resolution 1mv/bit 4096 = Full Scale
	uint16_t 			rec_iset; // resolution 100mA/bit
	uint16_t	        PSU_ADDR;
	uint16_t	        DC_ADDR;
	uint16_t  Status;   //  bit 0  "present" (answered CNHELLO) 
						//  bit 1  "active" (answered 10 sec ping) 
						//  bit 2  "PROFILE DATA RXd" succesfully
                     	//  bit 3  "status rxd" 
	                    //  bit 4   in fault, no comms, no profile will load
						//  bit 5   RECT output SET on  
						//  bit 6   "PROFILE DATA TXd" succesfully
						//  bit 7   "NOR ERR" NOR Flash write or read error"
	psu_mode_t 			psu_mode;// P_CV,P_CI,P_CPOT,P_OFF	
	uint8_t		        fail_lockout;
	uint8_t		        psufail_cnt;
} SysData_t;


typedef enum {INIT_M, RUN, FAULT, CONFIG, DBG, MANUAL_M, SAFE_M } modestate_t;
typedef enum {INIT, ON, OFF, MANUAL, SAFE } rectcntl_state_t;
typedef enum {G_SPLASH, G_MENU, G_RTSTA,G_CV, G_CI, G_CP, G_CFG, G_DBG, G_GRAPHS } gui_scn_state_t;
typedef enum {G_CV_INIT, G_CV_KYBUP, G_CV_KYBDN,G_CV_SAVE,G_CV_HM } cv_scn_state_t;
typedef enum {G_CI_INIT, G_CI_KYBUP, G_CI_KYBDN,G_CI_SAVE,G_CI_HM } ci_scn_state_t;
typedef enum {G_CP_INIT, G_CP_KYBUP, G_CP_KYBDN,G_CP_SAVE,G_CP_HM } cp_scn_state_t; 
typedef enum {G_CFG_INIT, G_CFG_KYBUP, G_CFG_KYBDN,G_CFG_LDMSEL,G_CFG_SVMSEL,G_CFG_SAVE,G_CFG_HM } cfg_scn_state_t; 
typedef enum {G_GPH_INIT, G_GPH_KYBUP, G_GPH_KYBDN,G_GPH_SAVE,G_GPH_HM } gph_scn_state_t; 

/* Structures ---------------------------------------------------------------*/
typedef struct {
	NetworkParam_t    netpms;
	gph_scn_state_t   gph_scn_state;
	cfg_scn_state_t   cfgsrcn_state;
	cp_scn_state_t    cpsrcn_state;
	ci_scn_state_t    cisrcn_state;
	cv_scn_state_t    cvsrcn_state;
	gui_scn_state_t   gui_scn_state;
	SysData_t         SysDataPSU[4];
    uint8_t		        ModelNumber[16];
	uint8_t		        SerialNumber[16];
	uint8_t		        FirmwareVers[16];
	uint16_t	        CAN_ADDR;
	uint8_t     			psu_viewing;
	uint16_t          statusreg;  // BIT0 HWINT OK ( HAL, RCC,I2C,etc etc)
								// BIT 1 RTOS OK
								// BIT 2 GUI OK
								// BIT 3 TCPIP OK
								// BIT 4 CAN OK
								// BIT 5 SDRAM OK
								// BIT 6 SRAM  OK
								// BIT 7 NOR FALSH OK
								// BIT 8 CFG VARS OK
								// BIT10 LCD OK ( touch, frame buffer)
								// BIT11 USB OK
								// BIT12 SDCARD OK
								// BIT13-15 spare
	uint8_t  					 sec, min, hour, day, month;// Realtime clock populates these in k_Menu timer functoin
	uint16_t           year;
	uint16_t           ScreenTimeout;
	uint8_t            style;
	uint8_t			   demo;

} GuiState_t;
extern GuiState_t GuiState;


void Init_Iccp(void);
void IccpDoIccp(void);
void WriteProfileStructToNor(IccpPrfParam_t *pfl, uint8_t prflidx);
void ReadProfileStructFromNor(IccpPrfParam_t *pfl, uint8_t prflidx);
void CopyProfileStructRam2Ram(IccpPrfParam_t *pflsrc,IccpPrfParam_t *pfldst);
void SetIPflash(uint32_t ipaddress32 );
uint32_t GetIPflash(void);
void SetMacflash(uint64_t mac64);
uint64_t GetMacflash(void);

#endif