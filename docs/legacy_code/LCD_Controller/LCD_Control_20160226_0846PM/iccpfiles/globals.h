




extern unsigned int cvmode_vset,cvmode_vsettmp;
extern unsigned int cimode_iset,cimode_isettmp;


//typedef enum {INIT_M, RUN, FAULT, CONFIG, DBG, MANUAL_M, SAFE_M } modestate_t;
//extern modestate_t modesstate_main_t;

//typedef enum {INIT_R, RECTON, RECTOFF, MANUAL_R, SAFE_R } rectcntl_state_t;
//extern rectcntl_state_t rect_ctl_state1_t;

//typedef enum {INIT_DIALOG, KEYBRDUP, SETVAL, SAVEVAL } gui_scn_state_t;
//extern gui_scn_state_t gui_scn_state_cv; //instance of cv screen enum
//extern gui_scn_state_t gui_scn_state_ci; //instance of ci screen enum
//extern gui_scn_state_t gui_scn_state_cpot; //instance of cont potential screen enum

//typedef enum {KYBRDUP, KYBRDDN } gui_kybrd_state_t;
//extern gui_kybrd_state_t gui_kybrd_state1; //instance of keyboard screen enum

//typedef struct  {
//				modestate_t modesstate_main_t;
//        rectcntl_state_t rect_ctl_state1_t;
//				gui_scn_state_t gui_scn_state_cv;
//				gui_scn_state_t gui_scn_state_ci;
//				gui_scn_state_t gui_scn_state_cpot;
//				gui_kybrd_state_t gui_kybrd_state1;
//				unsigned char spare;
//        unsigned char recfaultreg;
//        unsigned char rectfaultregmask;
//        unsigned int  rect_duty; // Iset OWM duty cycle
//        unsigned int  Vrd; //V readback
//        unsigned int  Ird; //I readback
//        unsigned int  Trd_rmt; //temp from remote mount sensor
//        unsigned int  Trd_pcb; //temp from pcb SMT sensor
//        unsigned char RectEnabled;
//        unsigned char recttimeon_ms;
//        unsigned char ton_minsec;// 0= off 1=sec 2=min
//        unsigned char timeoneremaining;
//        unsigned char recttimeoff_ms;
//        unsigned char toff_minsec;// 0= off 1=sec 2=min
//        unsigned char timeofferemaining;
//        unsigned int statusreg;// All the LEDs except the 7 segment leds
//} Status_s;
//extern Status_s systemstatus_s;

extern char dbgstr[32];
extern unsigned int gbl_xpos,gbl_ypos;
//extern unsigned int iccpmain_hdl;

