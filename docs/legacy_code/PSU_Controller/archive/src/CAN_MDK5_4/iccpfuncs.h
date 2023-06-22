#include <stdint.h>

#define tickspersec 10


/* Enumerations -------------------------------------------------------------*/
typedef enum  {LBLD, ESPM, RECT_END} Rect_t;
typedef enum  {I_OFF, I_HLLO, I_PFL, I_STAQRY, I_STARSP, I_FAIL } init_state_t;
typedef enum  {INIT_M, RUN, FAULT, CONFIG, DBG, MANUAL_M, SAFE_M } modestate_t;
typedef enum  {INIT, ON, OFF, MANUAL, SAFE } rectcntl_state_t;
        enum Maxima {M_ATZERO = 0, M_INDEX = 129};

/* Structures ---------------------------------------------------------------*/
typedef struct {
            uint16_t  avg;
            uint32_t  sum;
            uint16_t  collection[129];
} Record_t;

/* ICCP Run Time Profile Struct ---------------------------------------------*/
typedef enum {P_CV,P_CI,P_CPOT,P_OFF} psu_mode_t;
//ICCP Run Time Profile Struct
typedef struct {
			psu_mode_t psu_mode;// P_CV,P_CI,P_CPOT,P_OFF
			int16_t refelec_min_th;   // resolution 1mv/bit  -2048 = -.5Vdc  +2048= +.5Vdc
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
} IccpPrfParam_t; //Iccp Profile Parameters

typedef struct {
	IccpPrfParam_t    pflparams;
	init_state_t      init_state;             // {I_OFF,I_HLLO,I_PFL,I_STA,I_FAIL } init_state_t;
	uint16_t		      AlmReg;		              // bit  0: output on\off 0=off
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
	
	uint32_t          TestStaReg;             // bit  0: test1
                                            // bit  1: test2 
                                            // bit  2: 
                                            // bit  3: 
                                            // bit  4-31:
	uint8_t		        ModelNumber[16];
	uint8_t		        SerialNumber[16];
	uint8_t		        FirmwareVers[16];
  uint8_t           IndexCounter;
  uint8_t           Temperature;
  uint8_t	          DC_ADDR;
  Record_t          RefCell_0;
  Record_t          RefCell_1;
  Record_t          RefCell_2;
  Record_t          RefCell_3;
  Record_t          Vrdbk;
  Record_t          Irdbk_0;
  Record_t          Irdbk_1;
  Record_t          Irdbk_2;
  Record_t          Irdbk_3;
	uint16_t	        NODE_ADDR;
	uint16_t          Status;                 //  bit 0  "present" (answered CNHELLO) 
                                            //  bit 1  "active" (answered 10 sec ping) 
                                            //  bit 2  "PROFILE DATA RXd" succesfully
                                            //	bit 3  "status rxd" 
                                            //  bit 4   in fault, no comms, no profile will load
                                            //  bit 5   RECT output SET on  
                                            //  bit 6   "PROFILE DATA TXd" succesfully
                                            //  bit 7   "NOR ERR" NOR Flash write or read error"												
} SysData_t;

/* Structure instantiations -------------------------------------------------*/
extern SysData_t  SysData;


void Init_Iccp(void);
void IccpDoIccp(void);
void WriteProfileStructToNor(IccpPrfParam_t *pfl, uint8_t prflidx);
void ReadProfileStructFromNor(IccpPrfParam_t *pfl, uint8_t prflidx);
void CopyProfileStructRam2Ram(IccpPrfParam_t *pflsrc,IccpPrfParam_t *pfldst);
