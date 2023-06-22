/*********************************************************************
  * @author  Vince Keiper AstrodyneTDI
  * @version V1.0.0
  * @date    18-Sept-2015
  * @brief   ICCP GUI Profile Manager screen 
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 AstrodyneTDI</center></h2>
  *
  *
  ******************************************************************************  
  */ 

#include "DIALOG.h"
#include "stdio.h"
#include "k_module.h"
#include "globals.h"
#include "stdlib.h"
#include "iccpfuncs.h"
#include "iccp_canfuncs.h"
#include "string.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_FRAMEWIN_0 (GUI_ID_USER + 0x00)
#define ID_LISTVIEW_0 (GUI_ID_USER + 0x01)
#define ID_BUTTON_SSL (GUI_ID_USER + 0x02)
#define ID_DROPDOWN_ML (GUI_ID_USER + 0x03)
#define ID_BUTTON_CN (GUI_ID_USER + 0x05)
#define ID_BUTTON_SND (GUI_ID_USER + 0x06)
#define ID_BUTTON_HM (GUI_ID_USER + 0x07)
#define ID_DROPDOWN_MS (GUI_ID_USER + 0x08)//memory save select
#define ID_TEXT_ML (GUI_ID_USER + 0x09)//memory load label
#define ID_TEXT_MS (GUI_ID_USER + 0x0A)//memory save label
//#define ID_GROUP_ML (GUI_ID_USER + 0x0B)//group label
//#define ID_GROUP_MS (GUI_ID_USER + 0x0C)//group label
//#define ID_GROUP_ML2 (GUI_ID_USER + 0x0D)//group label
//#define ID_GROUP_MS2 (GUI_ID_USER + 0x0E)//group label
#define ID_BUTTON_LD (GUI_ID_USER + 0x14)
#define ID_BUTTON_SV (GUI_ID_USER + 0x15)
#define ID_FRAMEWIN_LS (GUI_ID_USER + 0x0F)//group label
#define ID_LISTVIEW_LS (GUI_ID_USER + 0x13)
#define ID_DROPDOWN_LS (GUI_ID_USER + 0x10)//group label
#define ID_BUTTON_LS_OK (GUI_ID_USER + 0x11)//group label
#define ID_BUTTON_LS_CN (GUI_ID_USER + 0x12)//group label

#define GUI_EXTRALIGHTGRAY     0x00E0E0E0

//#define XSCN 480  // x dimension screen
//#define YSCN 240  // y dimension screen
//#define XDDPD 170 // x dimension dropdown
//#define YDDPD 30  // y dimension dropdown
//#define XDBTN 80  // x dimension buttons
//#define YDBTN 35  // y dimension buttons
//#define XDSPC10 10// x dimension space between btn & other objects
//#define YDSPC10 10// y dimension space between btn & other objects

#define XSCN 480  // x dimension screen
#define YSCN 245  // y dimension screen //240
#define XDDPD 170 // x dimension dropdown //170
#define YDDPD 20  // y dimension dropdown
#define XDBTN 80-10  // x dimension buttons
#define YDBTN 35  // y dimension buttons
#define XDSPC10 10// x dimension space between btn & other objects
#define YDSPC10 10// y dimension space between btn & other objects

#define WLS XSCN/2
#define HLS (YSCN*3)/4
#define XLS XSCN/4
#define YLS YSCN/8


/**
*
*Local Variables
*
**/
uint8_t slct_memidx;//selected memory index
uint8_t slct_memsvidx;//selected memory save to index

FRAMEWIN_Handle hFrameLS;
WM_HWIN hFramePMgr;
char LS_ACTIVE = 0;
char strStatus[32];
char strProfileName[32];
/**
*
*    External resources
*
**/
extern K_ModuleItem_Typedef  iccp_main;
extern WM_HWIN iccpmain_hdl;
extern WM_HWIN Createiccp_main(void);
extern WM_HWIN Createlcdkeypad(uint8_t alpha_ena);
extern void CopyProfileStructRam2Ram(IccpPrfParam_t *pflsrc,IccpPrfParam_t *pfldst);


// from iccpfunc.c
extern IccpPrfParam_t s_profile_ram[4]; //Default Iccp Profile Parameters to send if PSU is not programmed

WM_HWIN Createiccp_pm_ldsv(char LDSV);
static void _cbDialog2(WM_MESSAGE * pMsg);

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static void  LoadListview(WM_HWIN hItem,IccpPrfParam_t *pfl);
static void  LoadListviewLS(WM_HWIN hItem);

static char* pBinFill(uint32_t x,char *so, int width, char fillChar);

//		static const char _acListStr[14]  [2]   [20] =  //EAJ 1/14/2016 was 10,2,20
//		{// Col0-Name         Col1-Description    
//			{ {"ref.lo"},      {"Electrode low Limit"} },  //ROW 0
//			{ {"ref.hi"},      {"Electrode hi limit"} },   //ROW 1
//			{ {"mode"},        {"PSU MODE"} },        		 //ROW 2
//			{ {"v.set"},		     {"CV Mode set point"} },  //ROW 3
//			{ {"i.set"},        {"CI mode set point"} },   //ROW 4
//			{ {"tst.int"},     {"Test interval Min"} },		 //ROW 6
//			{ {"ctl.int"},     {"Rect Upd Int"} },     		 //ROW 5
//			{ {"adc.int"},     {"ADC sample interval"} },	 //ROW 7
//			{ {"tst.ena"},     {"Tests enabled"} },	       //ROW 8
//			{ {"elc.ena"},     {"Electrodes enabled"} },	 //ROW 9
//			{ {"elc.1.desc"},     {"Electrode 1 Desc."} },	 //ROW 10 //EAJ 1/14/2016
//			{ {"elc.2.desc"},     {"Electrode 2 Desc."} },	 //ROW 11 //EAJ 1/14/2016
//			{ {"elc.3.desc"},     {"Electrode 3 Desc."} },	 //ROW 12 //EAJ 1/14/2016
//			{ {"elc.4.desc"},     {"Electrode 4 Desc."} },	 //ROW 13 //EAJ 1/14/2016
//		};
		
//		static const char _acListStr[14]  [2]   [20] =  //EAJ 1/14/2016 was 10,2,20
//		{// Col0-Property         Col1-Value    
//			{ {"Electrode low Limit"},	{"TBD"} },  //ROW 0
//			{ {"Electrode hi limit"},		{"TBD"} },   //ROW 1
//			{ {"PSU MODE"},							{"TBD"} },        		 //ROW 2
//			{ {"CV Mode set point"},		{"TBD"} },  //ROW 3
//			{ {"CI mode set point"},		{"TBD"} },   //ROW 4
//			{ {"Test interval Min"},		{"TBD"} },		 //ROW 6
//			{ {"Rect Upd Int"},					{"TBD"} },     		 //ROW 5
//			{ {"ADC sample interval"},	{"TBD"} },	 //ROW 7
//			{ {"Tests enabled"},				{"TBD"} },	       //ROW 8
//			{ {"Electrodes enabled"},		{"TBD"} },	 //ROW 9
//			{ {"Electrode 1 Desc."},		{"TBD"} },	 //ROW 10 //EAJ 1/14/2016
//			{ {"Electrode 2 Desc."},		{"TBD"} },	 //ROW 11 //EAJ 1/14/2016
//			{ {"Electrode 3 Desc."},		{"TBD"} },	 //ROW 12 //EAJ 1/14/2016
//			{ {"Electrode 4 Desc."},		{"TBD"} },	 //ROW 13 //EAJ 1/14/2016
//		};
		
		static const char _acListStr[21]  [2]   [32] =  //EAJ 1/14/2016 was 10,2,20
		{// Col0-Property         Col1-Value
			{ {"PSU MODE"},											{"TBD"} },        		 //ROW 2			
			{ {"CV: Set point"},								{"TBD"} },  //ROW 3
			{ {"CV: Low threshold"},						{"TBD"} },  //ROW 3
			{ {"CV: High threshold"},						{"TBD"} },  //ROW 3
			{ {"CV: Full Scale"},								{"TBD"} },  //ROW 3
			{ {"CI: Set point"},								{"TBD"} },   //ROW 4
			{ {"CI: Low threshold"},						{"TBD"} },  //ROW 3
			{ {"CI: Hi threshold"},							{"TBD"} },  //ROW 3
			{ {"CI: Full Scale"},								{"TBD"} },  //ROW 3
			{ {"CP: Demand set point"},					{"TBD"} },  //ROW 0			
			{ {"CP: Electrode low Limit"},			{"TBD"} },  //ROW 0
			{ {"CP: Electrode hi limit"},				{"TBD"} },   //ROW 1
			{ {"CP: Electrodes enabled"},				{"TBD"} },	 //ROW 9
			{ {"CP: Electrode 1 Description"},	{"TBD"} },	 //ROW 10 //EAJ 1/14/2016
			{ {"CP: Electrode 2 Description"},	{"TBD"} },	 //ROW 11 //EAJ 1/14/2016
			{ {"CP: Electrode 3 Description"},	{"TBD"} },	 //ROW 12 //EAJ 1/14/2016
			{ {"CP: Electrode 4 Description"},	{"TBD"} },	 //ROW 13 //EAJ 1/14/2016
			{ {"Tests enabled"},								{"TBD"} },	       //ROW 8
			{ {"Test interval (Minutes)"},			{"TBD"} },		 //ROW 6
			{ {"Rectifier update interval"},		{"TBD"} },     		 //ROW 5
			{ {"ADC sample interval"},					{"TBD"} },	 //ROW 7
		};
/*********************************************************************
*
*       _aDialogCreate
*/
//static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
//  { FRAMEWIN_CreateIndirect, "   Profile Manager", ID_FRAMEWIN_0,  0,            25,  XSCN,   YSCN, 0, 0x0, 0 },
//  { TEXT_CreateIndirect, "Selected Profile", ID_TEXT_ML,           5,             0,XDDPD, YDDPD, 0, 0x0, 0 },
//	{ DROPDOWN_CreateIndirect, "Dropdown", ID_DROPDOWN_ML,           5,            30, XDDPD, YDBTN, 0, 0x0, 0 },
//  { BUTTON_CreateIndirect, "Cancel", ID_BUTTON_CN,                 5,35*2+YDSPC10*1, 80, 35, 0, 0x0, 0 },
//	{ BUTTON_CreateIndirect, "Burn 2 Node", ID_BUTTON_SND,  XDDPD/2+10,35*2+YDSPC10*1, 80, 35, 0, 0x0, 0 },
//  { LISTVIEW_CreateIndirect, "Listview", ID_LISTVIEW_0,            3,35*3+YDSPC10*2, 375, YSCN-(35*3+YDSPC10*2)-5, 0, 0x0, 0 }, //Width was 250
//  
//	{ TEXT_CreateIndirect, "Save 2 Presets", ID_TEXT_MS,          XSCN-(XDDPD+15),      0,   XDDPD,YDDPD,  0,  0x0, 0 },
//  { DROPDOWN_CreateIndirect, "Dpdn Mem.Save", ID_DROPDOWN_MS, XSCN-(XDDPD+15),     30, XDDPD, YDDPD, 0, 0x0, 0 },
//	{ BUTTON_CreateIndirect, "Burn 2 MEMx", ID_BUTTON_SSL,              XSCN-95,35*2+YDSPC10*1, 80, 35, 0, 0x0, 0 },
//	
//	{ BUTTON_CreateIndirect, "Home", ID_BUTTON_HM,                      XSCN-95, YSCN-60, 80, 35, 0, 0x0, 0 },
//};
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { FRAMEWIN_CreateIndirect, "   Profile Manager", ID_FRAMEWIN_0,  0,            25,  XSCN,   YSCN, 0, 0x0, 0 },
//	{ TEXT_CreateIndirect, "", ID_GROUP_ML,           		 3,             15,XDDPD+4, YDDPD*4, 0, 0x0, 0 },
//	{ TEXT_CreateIndirect, "", ID_GROUP_ML2,           		 4,             16,XDDPD+2, YDDPD*4-2, 0, 0x0, 0 },
  { TEXT_CreateIndirect, " Profile: Node Live", ID_TEXT_ML,           3,             3,375+10, YDDPD, 0, 0x0, 0 },
//	{ DROPDOWN_CreateIndirect, "Dropdown", ID_DROPDOWN_ML,           5+5,            30, XDDPD-10, YDDPD, 0, 0x0, 0 },
//  { BUTTON_CreateIndirect, "Cancel", ID_BUTTON_CN,                 5+5,35*2+YDSPC10*1-5, 75, 35, 0, 0x0, 0 },
//	{ BUTTON_CreateIndirect, "Burn 2 Node", ID_BUTTON_SND,  XDDPD/2+10,35*2+YDSPC10*1-5, 75, 35, 0, 0x0, 0 },
  { LISTVIEW_CreateIndirect, "Listview", ID_LISTVIEW_0,            3,YDDPD+3, 375+10, YSCN-50-YDDPD, 0, 0x0, 0 }, //Width was 250
  

	{ TEXT_CreateIndirect, "Save 2 Presets", ID_TEXT_MS,          3,      YSCN-50+3,   375+10,YDDPD,  0,  0x0, 0 },
  //{ DROPDOWN_CreateIndirect, "Dpdn Mem.Save", ID_DROPDOWN_MS, XSCN-(XDDPD+15)+5,     30, XDDPD-10, YDDPD, 0, 0x0, 0 },
	//{ BUTTON_CreateIndirect, "Burn 2 MEMx", ID_BUTTON_SSL,              XSCN-95,35*2+YDSPC10*1-5, 75, 30, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, "Load", ID_BUTTON_LD,              				XSCN-95+10,3, 75-10, 35, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, "Save", ID_BUTTON_SV,              				XSCN-95+10,3+35+10, 75-10, 35, 0, 0x0, 0 },	
	{ BUTTON_CreateIndirect, "Home", ID_BUTTON_HM,                      XSCN-95+10, YSCN-60, 75-10, 35, 0, 0x0, 0 },
};

static const GUI_WIDGET_CREATE_INFO _aDialogCreate2[] = {
  { FRAMEWIN_CreateIndirect, "LOAD/SAVE", ID_FRAMEWIN_LS,  			XLS, 			YLS,  			WLS,  HLS, 0, 0x0, 0 }, //WM_CF_STAYONTOP
	{ LISTVIEW_CreateIndirect, "ID_LISTVIEW_LS", ID_LISTVIEW_LS, 	25, 			10, 				160, 	HLS-(35+10)-15-10-10, 	0, 0x0, 0 },
	//{ DROPDOWN_CreateIndirect, "ID_DROPDOWN_LS", ID_DROPDOWN_LS, 	25, 			10, 			160, 	35, 	0, 0x0, 0 },
	{ BUTTON_CreateIndirect, "OK", ID_BUTTON_LS_OK,              	25, 			HLS-(35+10)-15, 75, 	35, 	0, 0x0, 0 },
	{ BUTTON_CreateIndirect, "Cancel", ID_BUTTON_LS_CN,          	25+75+10, HLS-(35+10)-15, 75, 	35, 	0, 0x0, 0 },
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

/*********************************************************************
*
*       _cbDialog
*/
static void _cbDialog(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;
	char titlestr[34];
	uint8_t lclcanbuff[8];
	//int winxsz,winysz;
		switch (pMsg->MsgId) 
		{
					case WM_INIT_DIALOG:
								GuiState.cfgsrcn_state = G_CFG_INIT;
								slct_memidx =0;// the default profile is the one running in the PSU
								
								//
								// Initialization of 'Framewin'
								//
					sprintf(titlestr,"Profile Manager Screen for NODE: %i",GuiState.psu_viewing);
								hItem = pMsg->hWin;
								hFramePMgr = hItem;
								FRAMEWIN_SetFont(hItem, GUI_FONT_16B_1);
								FRAMEWIN_SetText(hItem, titlestr );
								FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
								FRAMEWIN_SetClientColor(hItem,GUI_WHITE);  
								FRAMEWIN_SetSkin(hItem,FRAMEWIN_SKIN_FLEX);
					
//			sprintf(titlestr,"Constant Potential Mode for NODE: %i",GuiState.psu_viewing);
//				hItem = pMsg->hWin;
//				FRAMEWIN_SetFont(hItem, GUI_FONT_16B_1);
//				FRAMEWIN_SetText(hItem, titlestr );
//				FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
//				FRAMEWIN_SetClientColor(hItem,GUI_WHITE);  
//				FRAMEWIN_SetSkin(hItem,FRAMEWIN_SKIN_FLEX);
					
								//hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_ML);
								
					
//								//
//								//
//								// Initialization of 'Load Select Label_txt'
//								//
//								hItem = WM_GetDialogItem(pMsg->hWin, ID_GROUP_ML);
//								//FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
//								FRAMEWIN_SetBarColor(hItem,0,GUI_WHITE);
//                FRAMEWIN_SetClientColor(hItem,GUI_WHITE);
//								FRAMEWIN_SetSkinClassic(hItem);								
//								//FRAMEWIN_SetSkin(hItem,FRAMEWIN_SKIN_FLEX);
//								
//								hItem = WM_GetDialogItem(pMsg->hWin, ID_GROUP_MS);
//								//FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
//								FRAMEWIN_SetBarColor(hItem,0,GUI_WHITE);
//								FRAMEWIN_SetClientColor(hItem,GUI_WHITE);
//								FRAMEWIN_SetSkinClassic(hItem);								
//								//FRAMEWIN_SetSkin(hItem,FRAMEWIN_SKIN_FLEX);

//									hItem = WM_GetDialogItem(pMsg->hWin, ID_GROUP_ML);
//									TEXT_SetBkColor(hItem, GUI_BLACK);
//									hItem = WM_GetDialogItem(pMsg->hWin, ID_GROUP_ML2);
//									TEXT_SetBkColor(hItem, GUI_WHITE);

//									hItem = WM_GetDialogItem(pMsg->hWin, ID_GROUP_MS);
//									TEXT_SetBkColor(hItem, GUI_BLACK);
//									hItem = WM_GetDialogItem(pMsg->hWin, ID_GROUP_MS2);
//									TEXT_SetBkColor(hItem, GUI_WHITE);
								
								//
								//
								// Initialization of 'Load Select Label_txt'
								//
								hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_ML);
								TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
								TEXT_SetFont(hItem, GUI_FONT_20_1);
								TEXT_SetTextColor(hItem, GUI_BLACK);
								TEXT_SetBkColor(hItem, GUI_EXTRALIGHTGRAY);
								//TEXT_SetBkColor(hItem, GUI_WHITE);
								//
								//
								// Initialization of 'Save Select Label_txt'
								//
//								hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_MS);
//								TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
//								TEXT_SetFont(hItem, GUI_FONT_20_1);
//								TEXT_SetTextColor(hItem, GUI_DARKBLUE);
//								TEXT_SetBkColor(hItem, GUI_WHITE);
								hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_MS);
								TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
								TEXT_SetFont(hItem, GUI_FONT_20_1);
								TEXT_SetTextColor(hItem, GUI_BLUE);
								TEXT_SetText(hItem, ""); //Clear Saved Status
								//TEXT_SetBkColor(hItem, GUI_EXTRALIGHTGRAY);
								
								//Intialization of buttons
								hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_LD);
								BUTTON_SetFont(hItem, GUI_FONT_20_1);
								
								hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_SV);
								BUTTON_SetFont(hItem, GUI_FONT_20_1);

								hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_HM);
								BUTTON_SetFont(hItem, GUI_FONT_20_1);

								//
								// Initialization of 'Listview'
								//
								LoadListview(WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0),&GuiState.SysDataPSU[GuiState.psu_viewing].pflparams);
								
//								//
//								// Initialization of 'Dropdown Load profile from Memory'
//								//
//								hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ML);
//								DROPDOWN_SetListHeight(hItem, 90);
//								DROPDOWN_AddString(hItem, "Live Profile");
//								DROPDOWN_SetFont(hItem, GUI_FONT_16B_1);
//								DROPDOWN_AddString(hItem, "MEM #1");
//								DROPDOWN_AddString(hItem, "MEM #2");
//								DROPDOWN_AddString(hItem, "MEM #3");
//								DROPDOWN_AddString(hItem, "Default");
//								DROPDOWN_SetTextHeight(hItem,30); //was 35
//								DROPDOWN_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
//								//
//								// Initialization of 'Dropdown Load profile from Memory'
//								//
//								hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_MS);
//								DROPDOWN_SetListHeight(hItem, 80);
//								DROPDOWN_AddString(hItem, "Live Profile");
//								DROPDOWN_SetItemDisabled(hItem,0,0);// You cannot save the Live Profile to Flash, it is RAM only in LCD
//								DROPDOWN_SetFont(hItem, GUI_FONT_16B_1);
//								DROPDOWN_AddString(hItem, "MEM #1");
//								DROPDOWN_AddString(hItem, "MEM #2");
//								DROPDOWN_AddString(hItem, "MEM #3");
//								
//								DROPDOWN_SetTextHeight(hItem,30); //was 35
//								DROPDOWN_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
					break;  //end case WM_INIT_DIALOG:
					
					case WM_NOTIFY_PARENT:
								Id    = WM_GetId(pMsg->hWinSrc);
								NCode = pMsg->Data.v;
								switch(Id)
								{
										case ID_FRAMEWIN_0: // Notifications sent by 'ID_FRAMEWIN_0'
																if(LS_ACTIVE > 0)
																{
																	FRAMEWIN_SetActive(hFrameLS,1);
																}

										break;
									
										case ID_BUTTON_HM: // Notifications sent by 'Home_Btn'
															switch(NCode) 
															{
																	case WM_NOTIFICATION_RELEASED:
																			if(LS_ACTIVE > 0)
																			 {    //FRAMEWIN_Delete(hFrameLS);
																						GUI_EndDialog(hFrameLS,NULL);
																				    LS_ACTIVE = 0;
																			 }
																			 
																			 if(GuiState.cfgsrcn_state == G_CFG_INIT)
																			 {
																						WM_InvalidateWindow(iccpmain_hdl);
																						WM_Paint(iccpmain_hdl);
																						GUI_EndDialog(pMsg->hWin,NULL);
																						GuiState.gui_scn_state = G_RTSTA;
																						GuiState.cfgsrcn_state = G_CFG_INIT;
																			 }
																			break;
															}//end switch NCode
										break;
														
										case ID_LISTVIEW_0: // Notifications sent by 'Listview'
													switch(NCode) {
															case WM_NOTIFICATION_CLICKED:
															break;
														
															case WM_NOTIFICATION_RELEASED:
															break;

															case WM_NOTIFICATION_SEL_CHANGED:
															break;
													}
										break;
													
										case ID_DROPDOWN_MS: // Notifications sent by 'Dropdown Save profile to Memory '
												switch(NCode) {
														case WM_NOTIFICATION_CLICKED:
																	hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_MS);
																	if(!GuiState.cfgsrcn_state == G_CFG_LDMSEL)
																	{
																			DROPDOWN_Expand(hItem);
																	}
														break;
														
														case WM_NOTIFICATION_RELEASED:
																	hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_MS);
																	if(!GuiState.cfgsrcn_state == G_CFG_LDMSEL)
																	{
																			GuiState.cfgsrcn_state = G_CFG_LDMSEL;
																	}
																	else
																	{
																			GuiState.cfgsrcn_state = G_CFG_INIT;
																	}
														break;
														
														case WM_NOTIFICATION_SEL_CHANGED:
															
																hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_MS);
																slct_memsvidx = DROPDOWN_GetSelExp(hItem);
																
														break;
												}
										break;			
													
										case ID_BUTTON_SSL: // Notifications sent by 'Save Selected Profile to MEMx location in LCD flash'
													switch(NCode) {
																case WM_NOTIFICATION_CLICKED:
																			if(slct_memsvidx==0)// default profile is index zero of the struct, this never gets writen to if idx zero of dropdown is selected
																			{										// we write the Live Profile which is the profile of the selected PSU in RAM of the LCD
																													// controller. 
																				//	WriteProfileStructToNor(&GuiState.SysDataPSU[GuiState.psu_viewing].pflparams,slct_memsvidx);
																				//	CopyProfileStructRam2Ram(&GuiState.SysDataPSU[GuiState.psu_viewing].pflparams,&s_profile_ram[slct_memsvidx]);
																				//	LoadListview(WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0),&GuiState.SysDataPSU[GuiState.psu_viewing].pflparams);
																			}
																			else if( slct_memsvidx !=4){//save MEM1 thru MEM3
																					CopyProfileStructRam2Ram(&GuiState.SysDataPSU[GuiState.psu_viewing].pflparams,&s_profile_ram[slct_memsvidx]);
																				  //s_profile_ram[slct_memsvidx]=GuiState.SysDataPSU[GuiState.psu_viewing].pflparams;//EAJ 1/14/2016
																					WriteProfileStructToNor(&s_profile_ram[slct_memsvidx],slct_memsvidx);// default profile is idx zero of struct so
																					LoadListview(WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0),&s_profile_ram[slct_memidx]);
																			}
																			
																break;
													
																case WM_NOTIFICATION_RELEASED:
																break;
													}
										break;
										
										case ID_DROPDOWN_ML: // Notifications sent by 'Dropdown Load profile from Memory '
												switch(NCode) {
														case WM_NOTIFICATION_CLICKED:
																	hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ML);
																	if(!GuiState.cfgsrcn_state == G_CFG_LDMSEL)
																	{
																			DROPDOWN_Expand(hItem);
																	}
														break;
														
														case WM_NOTIFICATION_RELEASED:
																	hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ML);
																	if(!GuiState.cfgsrcn_state == G_CFG_LDMSEL)
																	{
																			GuiState.cfgsrcn_state = G_CFG_LDMSEL;
																	}
																	else
																	{
																			GuiState.cfgsrcn_state = G_CFG_INIT;
																	}
														break;
														
														case WM_NOTIFICATION_SEL_CHANGED:
															
																hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ML);
																slct_memidx = DROPDOWN_GetSelExp(hItem);
																if(slct_memidx ==0){// load Live profile
																		LoadListview(WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0),&GuiState.SysDataPSU[GuiState.psu_viewing].pflparams);
																}else if (slct_memidx !=4){  // load MEM1 thru MEM3
																		ReadProfileStructFromNor(&s_profile_ram[slct_memidx],slct_memidx);// pulls profile from flash
																		LoadListview(WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0),&s_profile_ram[slct_memidx]);
																		CopyProfileStructRam2Ram(&s_profile_ram[slct_memidx],&GuiState.SysDataPSU[GuiState.psu_viewing].pflparams);
																	  //GuiState.SysDataPSU[GuiState.psu_viewing].pflparams=s_profile_ram[slct_memidx];//EAJ 1/14/2016
																}else{ //load default profile AKA MEM0
																		LoadListview(WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0),&s_profile_ram[0]);
																		CopyProfileStructRam2Ram(&s_profile_ram[0],&GuiState.SysDataPSU[GuiState.psu_viewing].pflparams);
																	  //GuiState.SysDataPSU[GuiState.psu_viewing].pflparams=s_profile_ram[0];//EAJ 1/14/2016
																}
														break;
												}
										break;
												
//										case ID_BUTTON_SD: // Notifications sent by 'Load Default'
//													switch(NCode) {
//															case WM_NOTIFICATION_CLICKED:
//																		CopyProfileStructRam2Ram(&s_profile_ram[0],&GuiState.SysDataPSU[GuiState.psu_viewing].pflparams);
//																		LoadListview(WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0),&s_profile_ram[slct_memidx]);
//																		WriteProfileStructToNor(&s_profile_ram[0],slct_memidx);
//															break;
//													}
//										break;
													
										case ID_BUTTON_CN: // Notifications sent by 'Cancel'
													switch(NCode) {
															case WM_NOTIFICATION_CLICKED:
															break;
															case WM_NOTIFICATION_RELEASED:
															break;
													}
										break;
													
										case ID_BUTTON_LD: // Notifications sent by 'Cancel'
													switch(NCode) {
															case WM_NOTIFICATION_CLICKED:
															break;
															case WM_NOTIFICATION_RELEASED:
																if(LS_ACTIVE==0)
																{
																	hFrameLS = Createiccp_pm_ldsv(1);
																}
																else
																{
																	FRAMEWIN_SetActive(hFrameLS,1);
																	//FRAMEWIN_Restore(hFrameLS);
																}
															break;
													}
										break;
													
										case ID_BUTTON_SV: // Notifications sent by 'Cancel'
													switch(NCode) {
															case WM_NOTIFICATION_CLICKED:
															break;
															case WM_NOTIFICATION_RELEASED:
																if(LS_ACTIVE==0)
																{
																	hFrameLS = Createiccp_pm_ldsv(2);
																}
																else
																{
																	FRAMEWIN_SetActive(hFrameLS,1);
																	//FRAMEWIN_Restore(hFrameLS);
																}
															break;
													}
										break;
													
										case ID_BUTTON_SND: // Notifications sent by 'Send Selected'
														switch(NCode) {
																case WM_NOTIFICATION_CLICKED:
																			CANWrite(GuiState.psu_viewing,0x00,CMD_CAN_203,lclcanbuff,0);// send profile to PSU FLASH
																			//CANWrite(GuiState.psu_viewing,0x00,CMD_CAN_203,(uint8_t*)dbgstr,0);// send profile to PSU FLASH
																break;
														}
										break;
														
						    }// end switch case notify parent
					break;// end CASE WM_NOTIFY_PARENT:
								
					default:
						WM_DefaultProc(pMsg);
					break;
					
		}//end swicth pmsg->
}// end function

static void _cbDialog2(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;
	uint8_t lclcanbuff[8];

//	char strOK[32];

		switch (pMsg->MsgId) 
		{
					case WM_INIT_DIALOG:
								slct_memidx=0;
								slct_memsvidx=0;
								hItem = pMsg->hWin;
								if(LS_ACTIVE==1)//LOAD
								{
									FRAMEWIN_SetText(hItem, "LOAD");
								}
								else
								{
									FRAMEWIN_SetText(hItem, "SAVE");
								}
								//FRAMEWIN_SetText(hItem, titlestr );
								FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
								FRAMEWIN_SetClientColor(hItem,GUI_WHITE);  
								FRAMEWIN_SetSkin(hItem,FRAMEWIN_SKIN_FLEX);
					
								LoadListviewLS(WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_LS));

					break;  //end case WM_INIT_DIALOG:
					
					case WM_NOTIFY_PARENT:
								Id    = WM_GetId(pMsg->hWinSrc);
								NCode = pMsg->Data.v;
								switch(Id)
								{
									
										case ID_BUTTON_LS_OK: // Notifications sent by 'Home_Btn'
															switch(NCode) 
															{
																	case WM_NOTIFICATION_RELEASED:
																	//{
																	if(LS_ACTIVE==1) //Load
																	{
																		if(slct_memidx ==0)
																		{// load Live profile
																			LoadListview(WM_GetDialogItem(hFramePMgr, ID_LISTVIEW_0),&GuiState.SysDataPSU[GuiState.psu_viewing].pflparams);
																		}
																		else if (slct_memidx !=4)
																		{  // load MEM1 thru MEM3
																			ReadProfileStructFromNor(&s_profile_ram[slct_memidx],slct_memidx);// pulls profile from flash
																			LoadListview(WM_GetDialogItem(hFramePMgr, ID_LISTVIEW_0),&s_profile_ram[slct_memidx]);
																			CopyProfileStructRam2Ram(&s_profile_ram[slct_memidx],&GuiState.SysDataPSU[GuiState.psu_viewing].pflparams);
																			//GuiState.SysDataPSU[GuiState.psu_viewing].pflparams=s_profile_ram[slct_memidx];//EAJ 1/14/2016
																		}
																		else
																		{ //load default profile AKA MEM0
																			LoadListview(WM_GetDialogItem(hFramePMgr, ID_LISTVIEW_0),&s_profile_ram[0]);
																			CopyProfileStructRam2Ram(&s_profile_ram[0],&GuiState.SysDataPSU[GuiState.psu_viewing].pflparams);
																			//GuiState.SysDataPSU[GuiState.psu_viewing].pflparams=s_profile_ram[0];//EAJ 1/14/2016
																		}
																		//Update Loaded Profile Name
																		hItem = WM_GetDialogItem(hFramePMgr, ID_TEXT_ML);
																		sprintf(strStatus," Profile: %s",strProfileName); //Add space at beginning of status string
																		TEXT_SetText(hItem, strStatus);
																		
																		//Update Save Status
																		hItem = WM_GetDialogItem(hFramePMgr, ID_TEXT_MS);
																		TEXT_SetText(hItem, ""); //Clear Save status text
																	}
																	else //Save
																	{

																		if(slct_memsvidx==0)// default profile is index zero of the struct, this never gets writen to if idx zero of dropdown is selected
																		{										// we write the Live Profile which is the profile of the selected PSU in RAM of the LCD
																		// controller. 
																		//	WriteProfileStructToNor(&GuiState.SysDataPSU[GuiState.psu_viewing].pflparams,slct_memsvidx);
																		//	CopyProfileStructRam2Ram(&GuiState.SysDataPSU[GuiState.psu_viewing].pflparams,&s_profile_ram[slct_memsvidx]);
																		//	LoadListview(WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0),&GuiState.SysDataPSU[GuiState.psu_viewing].pflparams);
																			CANWrite(GuiState.psu_viewing,0x00,CMD_CAN_203,lclcanbuff,0);// send profile to PSU FLASH
																		}
																		else//( slct_memsvidx !=4)
																		{//save MEM1 thru MEM3
																			CopyProfileStructRam2Ram(&GuiState.SysDataPSU[GuiState.psu_viewing].pflparams,&s_profile_ram[slct_memsvidx]);
																			//s_profile_ram[slct_memsvidx]=GuiState.SysDataPSU[GuiState.psu_viewing].pflparams;//EAJ 1/14/2016
																			WriteProfileStructToNor(&s_profile_ram[slct_memsvidx],slct_memsvidx);// default profile is idx zero of struct so
																			LoadListview(WM_GetDialogItem(hFramePMgr, ID_LISTVIEW_0),&s_profile_ram[slct_memidx]);
																		}
																		
																		//Update Save Status
																		hItem = WM_GetDialogItem(hFramePMgr, ID_TEXT_MS);
																		sprintf(strStatus,"Saved to: %s",strProfileName);
																		TEXT_SetText(hItem, strStatus);
																	}
																				    //FRAMEWIN_Delete(hFrameLS);
//																						hItem = WM_GetDialogItem(hFramePMgr, ID_TEXT_ML);
//																						TEXT_SetText(hItem, strOK);
																	
																						LS_ACTIVE = 0;
																						GUI_EndDialog(pMsg->hWin,NULL);
																			 //}
																			break;
															}//end switch NCode
										break;
															

															
										case ID_BUTTON_LS_CN: // Notifications sent by 'Home_Btn'
												switch(NCode) 
												{	
														case WM_NOTIFICATION_RELEASED:
																//FRAMEWIN_Delete(hFrameLS);
//																hItem = WM_GetDialogItem(hFramePMgr, ID_TEXT_ML);
//																TEXT_SetText(hItem, strOK);

																LS_ACTIVE = 0;
																GUI_EndDialog(pMsg->hWin,NULL);
														break;
												}//end switch NCode
										break;
															
										case ID_DROPDOWN_LS: // Notifications sent by 'Dropdown Load profile from Memory '
												switch(NCode) {
														case WM_NOTIFICATION_CLICKED:
//																	hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ML);
//																	if(!GuiState.cfgsrcn_state == G_CFG_LDMSEL)
//																	{
//																			DROPDOWN_Expand(hItem);
//																	}
														break;
														
														case WM_NOTIFICATION_RELEASED:
//																	hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ML);
//																	if(!GuiState.cfgsrcn_state == G_CFG_LDMSEL)
//																	{
//																			GuiState.cfgsrcn_state = G_CFG_LDMSEL;
//																	}
//																	else
//																	{
//																			GuiState.cfgsrcn_state = G_CFG_INIT;
//																	}
														break;
														
														case WM_NOTIFICATION_SEL_CHANGED:
															
																hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_LS);
																slct_memidx = DROPDOWN_GetSelExp(hItem);

														DROPDOWN_GetItemText(hItem,slct_memidx,strProfileName,32);
														
														hItem = WM_GetDialogItem(hFramePMgr, ID_TEXT_ML);
														TEXT_SetText(hItem, strProfileName);
														

														break;
												}
												
										case ID_LISTVIEW_LS: // Notifications sent by 'Dropdown Load profile from Memory '
												switch(NCode) {
														case WM_NOTIFICATION_CLICKED:
//																	hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ML);
//																	if(!GuiState.cfgsrcn_state == G_CFG_LDMSEL)
//																	{
//																			DROPDOWN_Expand(hItem);
//																	}
														break;
														
														case WM_NOTIFICATION_RELEASED:
//																	hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ML);
//																	if(!GuiState.cfgsrcn_state == G_CFG_LDMSEL)
//																	{
//																			GuiState.cfgsrcn_state = G_CFG_LDMSEL;
//																	}
//																	else
//																	{
//																			GuiState.cfgsrcn_state = G_CFG_INIT;
//																	}
														break;
														
														case WM_NOTIFICATION_SEL_CHANGED:
															
																hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_LS);

														slct_memidx = LISTVIEW_GetSel(hItem);
														slct_memsvidx = slct_memidx;
														LISTVIEW_GetItemText(hItem,0,slct_memidx,strProfileName,32);
														
//														hItem = WM_GetDialogItem(hFramePMgr, ID_TEXT_ML);
//														TEXT_SetText(hItem, strOK);

														break;
												}
//void             LISTVIEW_GetItemText         (LISTVIEW_Handle hObj, unsigned Column, unsigned Row, char * pBuffer, unsigned MaxSize);
//unsigned         LISTVIEW_GetItemTextLen      (LISTVIEW_Handle hObj, unsigned Column, unsigned Row);
//void             LISTVIEW_GetItemTextSorted   (LISTVIEW_Handle hObj, unsigned Column, unsigned Row, char * pBuffer, unsigned MaxSize);
//unsigned         LISTVIEW_GetLBorder          (LISTVIEW_Handle hObj);
//unsigned         LISTVIEW_GetNumColumns       (LISTVIEW_Handle hObj);
//unsigned         LISTVIEW_GetNumRows          (LISTVIEW_Handle hObj);
//unsigned         LISTVIEW_GetRBorder          (LISTVIEW_Handle hObj);
//int              LISTVIEW_GetSel              (LISTVIEW_Handle hObj);
										break;
						    }// end switch case notify parent
					break;// end CASE WM_NOTIFY_PARENT:
								
					default:
						WM_DefaultProc(pMsg);
					break;
					
		}//end swicth pmsg->
}// end function

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       CreateProfile Manager
*/
WM_HWIN Createiccp_pm(void);
WM_HWIN Createiccp_pm(void) {
  WM_HWIN hWin;

  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
	GuiState.gui_scn_state = G_CFG;
  return hWin;
}

//WM_HWIN Createiccp_pm_ldsv(void);
WM_HWIN Createiccp_pm_ldsv(char LDSV) {
  WM_HWIN hWin;
   
	LS_ACTIVE = LDSV;
  hWin = GUI_CreateDialogBox(_aDialogCreate2, GUI_COUNTOF(_aDialogCreate2), _cbDialog2, hFramePMgr, 0, 0);

  return hWin;
}



void LoadListview(WM_HWIN hItem,IccpPrfParam_t *pfl)
{
	int8_t row,col;
	char strDescription[32];
	char strBinary[33];
	char strValue[64];
		
	unsigned MaxCols = 2;
	unsigned MaxRows = 21;
	//static char ListViewLoaded = 0;
				HEADER_SetFont(LISTVIEW_GetHeader(hItem), GUI_FONT_20_1);
				HEADER_SetBkColor(LISTVIEW_GetHeader(hItem), GUI_LIGHTGRAY); //was GUI_EXTRALIGHTGRAY
				HEADER_SetTextColor(LISTVIEW_GetHeader(hItem), GUI_BLACK);
				HEADER_SetSkin(LISTVIEW_GetHeader(hItem),HEADER_SKIN_FLEX);
				
				LISTVIEW_SetFont(hItem, GUI_FONT_20_1); //Was GUI_FONT_20_1,GUI_FONT_20_ASCII
				LISTVIEW_SetGridVis(hItem, 1);
				//LISTVIEW_SetDefaultGridColor(GUI_LIGHTGRAY);
	
//		LISTVIEW_AddColumn(hItem, 75, "Name", GUI_TA_HCENTER | GUI_TA_VCENTER); //Width was 50
//		LISTVIEW_AddColumn(hItem, 150, "Value", GUI_TA_HCENTER | GUI_TA_VCENTER);//Width was 50
//		LISTVIEW_AddColumn(hItem, 150, "Description", GUI_TA_HCENTER | GUI_TA_VCENTER);//Width was 150
	
	if(LISTVIEW_GetNumColumns(hItem) < MaxCols)
	{
		LISTVIEW_AddColumn(hItem, 190, "Property", GUI_TA_LEFT | GUI_TA_VCENTER); //Width was 50
		LISTVIEW_AddColumn(hItem, 170, "Value", GUI_TA_LEFT | GUI_TA_VCENTER);//Width was 50
	}
	
    LISTVIEW_SetAutoScrollV(hItem,1);

		for(row=0;row<MaxRows;row++)  //EAJ 1/14/2016 was 10
		{
//				if(LISTVIEW_GetNumRows(hItem) < MaxRows){LISTVIEW_AddRow(hItem, NULL);}
//				for(col=0;col<2; col++)
//				{
//						// columns 0 and 2 filled from const array, column 1 is from profile params struct
//						if(col ==0){
//								LISTVIEW_SetItemText(hItem, col, row, _acListStr[row][col]); //put col0 array data in col 0 of list
//						}else{
//								LISTVIEW_SetItemText(hItem, col+1, row, _acListStr[row][col]);// put col-1 array data in col 2 of list 
//						}
//				}
			
//			{ {"Electrode low Limit"},	{"TBD"} },  //ROW 0
//			{ {"Electrode hi limit"},		{"TBD"} },   //ROW 1
//			{ {"PSU MODE"},							{"TBD"} },        		 //ROW 2
//			{ {"CV Mode set point"},		{"TBD"} },  //ROW 3
//			{ {"CI mode set point"},		{"TBD"} },   //ROW 4
//			{ {"Test interval Min"},		{"TBD"} },		 //ROW 6
//			{ {"Rect Upd Int"},					{"TBD"} },     		 //ROW 5
//			{ {"ADC sample interval"},	{"TBD"} },	 //ROW 7
//			{ {"Tests enabled"},				{"TBD"} },	       //ROW 8
//			{ {"Electrodes enabled"},		{"TBD"} },	 //ROW 9
//			{ {"Electrode 1 Desc."},		{"TBD"} },	 //ROW 10 //EAJ 1/14/2016
//			{ {"Electrode 2 Desc."},		{"TBD"} },	 //ROW 11 //EAJ 1/14/2016
//			{ {"Electrode 3 Desc."},		{"TBD"} },	 //ROW 12 //EAJ 1/14/2016
//			{ {"Electrode 4 Desc."},		{"TBD"} },	 //ROW 13 //EAJ 1/14/2016

//			{ {"PSU MODE"},											{"TBD"} },        		 //ROW 2			
//			{ {"CV: Set point"},								{"TBD"} },  //ROW 3
//			{ {"CV: Low threshold"},						{"TBD"} },  //ROW 3
//			{ {"CV: High threshold"},						{"TBD"} },  //ROW 3
//			{ {"CI: Full Scale"},								{"TBD"} },  //ROW 3
//			{ {"CI: Set point"},								{"TBD"} },   //ROW 4
//			{ {"CI: Low threshold"},						{"TBD"} },  //ROW 3
//			{ {"CI: Hi threshold"},							{"TBD"} },  //ROW 3
//			{ {"CI: Full Scale"},								{"TBD"} },  //ROW 3
//			{ {"CP: Demand set point"},					{"TBD"} },  //ROW 0			
//			{ {"CP: Electrode low Limit"},			{"TBD"} },  //ROW 0
//			{ {"CP: Electrode hi limit"},				{"TBD"} },   //ROW 1
//			{ {"CP: Electrodes enabled"},				{"TBD"} },	 //ROW 9
//			{ {"CP: Electrode 1 Description"},	{"TBD"} },	 //ROW 10 //EAJ 1/14/2016
//			{ {"CP: Electrode 2 Description"},	{"TBD"} },	 //ROW 11 //EAJ 1/14/2016
//			{ {"CP: Electrode 3 Description"},	{"TBD"} },	 //ROW 12 //EAJ 1/14/2016
//			{ {"CP: Electrode 4 Description"},	{"TBD"} },	 //ROW 13 //EAJ 1/14/2016
//			{ {"Tests enabled"},								{"TBD"} },	       //ROW 8
//			{ {"Test interval (Minutes)"},			{"TBD"} },		 //ROW 6
//			{ {"Rectifier update interval"},		{"TBD"} },     		 //ROW 5
//			{ {"ADC sample interval"},					{"TBD"} },	 //ROW 7
			
				if(LISTVIEW_GetNumRows(hItem) < MaxRows){LISTVIEW_AddRow(hItem, NULL);}
				
//				LISTVIEW_SetItemText(hItem, 0, row, _acListStr[row][0]); //put col0 array data in col 0 of list
//				LISTVIEW_SetItemText(hItem, 1, row, _acListStr[row][1]); //put col1 array data in col 1 of list
				// fill col 1 of list from PSU profile struct
				switch (row)
				{

							case 0:
									if(pfl->psu_mode == P_CV){sprintf(strValue,"CV");}
									else if(pfl->psu_mode == P_CI){sprintf(strValue,"CI");}
									else if(pfl->psu_mode == P_CPOT){sprintf(strValue,"CPOT");}
									else{sprintf(strValue,"OFF");}
							break;
							case 1:
									sprintf(strValue,"%2.1f",(float)pfl->rec_vset/10);
							break;
							case 2:
									sprintf(strValue,"%2.1f",(float)pfl->rec_vset_min_th/10);
							break;
							case 3:
									sprintf(strValue,"%2.1f",(float)pfl->rec_vset_max_th/10);
							break;
							case 4:
									sprintf(strValue,"%2.1f",(float)pfl->rec_fs_vset/10);
							break;
							case 5:
									sprintf(strValue,"%2.1f",(float)pfl->rec_iset/10);
							break;
							case 6:
									sprintf(strValue,"%2.1f",(float)pfl->rec_iset_min_th);
							break;
							case 7:
									sprintf(strValue,"%2.1f",(float)pfl->rec_iset_max_th);
							break;
							case 8:
									sprintf(strValue,"%2.1f",(float)pfl->rec_fs_iset);
							break;
							case 9:
									sprintf(strValue,"%0.2f",(float)pfl->refelec_dmd_setpt/1000);
							break;
							case 10:
									sprintf(strValue,"%0.2f",(float)pfl->refelec_min_th/1000);
							break;
							case 11:
									sprintf(strValue,"%0.2f",(float)pfl->refelec_max_th/1000);
							break;
							case 12:
								//pBinFill((pfl->refelec_active & 0x0f),strBinary,8,'0');
									sprintf(strValue,"0x%02X",pfl->refelec_active & 0x0f);
									//sprintf(strValue,"0x%02X:0b%s",pfl->refelec_active & 0x0f,strBinary);
							break;
							case 13:
									sprintf(strValue,"%s",pfl->refeleclongname[0]); //EAJ 1/14/2016
							break;
							case 14:
									sprintf(strValue,"%s",pfl->refeleclongname[1]); //EAJ 1/14/2016
							break;
							case 15:
									sprintf(strValue,"%s",pfl->refeleclongname[2]); //EAJ 1/14/2016
							break;
							case 16:
									sprintf(strValue,"%s",pfl->refeleclongname[3]); //EAJ 1/14/2016
							break;
							case 17:
									sprintf(strValue,"0x%04x",pfl->tsts_active & 0x0000FFFF);
							//pBinFill((pfl->tsts_active & 0x0000FFFF),strBinary,16,'0');
							//sprintf(strValue,"0x%04X:0b%s",pfl->tsts_active & 0x0000FFFF,strBinary);
							break;
							case 18:
									sprintf(strValue,"%i",(unsigned int)pfl->self_test_int);
							break;
							case 19:
									sprintf(strValue,"%i",(int)pfl->control_int);
							break;
							case 20:
									sprintf(strValue,"%i",(int)pfl->adc_smp_int);
							break;
				}
				LISTVIEW_SetItemText(hItem, 0, row, _acListStr[row][0]); //put col0 array data in col 0 of list
				LISTVIEW_SetItemText(hItem, 1, row, strValue);
		}
		
		//ListViewLoaded = 1;
		
}

void LoadListviewLS(WM_HWIN hItem)
{
//	int8_t row,col;
	//char strval[5];
//	unsigned MaxRows = 14;
	//static char ListViewLoaded = 0;
				HEADER_SetFont(LISTVIEW_GetHeader(hItem), GUI_FONT_16B_1);
				HEADER_SetBkColor(LISTVIEW_GetHeader(hItem), GUI_LIGHTGRAY); //was GUI_EXTRALIGHTGRAY
				HEADER_SetTextColor(LISTVIEW_GetHeader(hItem), GUI_BLACK);
				HEADER_SetSkin(LISTVIEW_GetHeader(hItem),HEADER_SKIN_FLEX);
				
				LISTVIEW_SetFont(hItem, GUI_FONT_20_1); //Was GUI_FONT_20_1
	
		//LISTVIEW_AddColumn(hItem, 150, "Source", GUI_TA_HCENTER | GUI_TA_VCENTER); //Width was 50
//		LISTVIEW_AddColumn(hItem, 150, "Value", GUI_TA_HCENTER | GUI_TA_VCENTER);//Width was 50
//		LISTVIEW_AddColumn(hItem, 150, "Description", GUI_TA_HCENTER | GUI_TA_VCENTER);//Width was 150
    LISTVIEW_SetAutoScrollV(hItem,1);
	
		if(LS_ACTIVE==1)//LOAD
		{
			LISTVIEW_AddColumn(hItem, 150, "Source", GUI_TA_LEFT | GUI_TA_VCENTER); //Width was 50
		}
		else
		{
			LISTVIEW_AddColumn(hItem, 150, "Destination", GUI_TA_LEFT | GUI_TA_VCENTER); //Width was 50
		}

//		for(row=0;row<MaxRows;row++)  //EAJ 1/14/2016 was 10
//		{
//			LISTVIEW_AddRow(hItem, NULL);
////				if(LISTVIEW_GetNumRows(hItem) < MaxRows){LISTVIEW_AddRow(hItem, NULL);}
////				for(col=0;col<2; col++)
////				{
////						// columns 0 and 2 filled from const array, column 1 is from profile params struct
////						if(col ==0){
////								LISTVIEW_SetItemText(hItem, col, row, _acListStr[row][col]); //put col0 array data in col 0 of list
////						}else{
////								LISTVIEW_SetItemText(hItem, col+1, row, _acListStr[row][col]);// put col-1 array data in col 2 of list 
////						}
////				}
//				// fill col 1 of list from PSU profile struct
////				switch (row)
////				{
////							case 0:
////									sprintf(strval,"%0.2f",(float)pfl->refelec_min_th/1000);
////							break;
////							case 1:
////									sprintf(strval,"%0.2f",(float)pfl->refelec_max_th/1000);
////							break;
////							case 2:
////									if(pfl->psu_mode == P_CV){sprintf(strval,"CV");}
////									else if(pfl->psu_mode == P_CI){sprintf(strval,"CI");}
////									else if(pfl->psu_mode == P_CPOT){sprintf(strval,"CPOT");}
////									else{sprintf(strval,"OFF");}
////							break;
////							case 3:
////									sprintf(strval,"%2.1f",(float)pfl->rec_vset/10);
////							break;
////							case 4:
////									sprintf(strval,"%2.1f",(float)pfl->rec_iset/10);
////							break;
////							case 5:
////									sprintf(strval,"%i",(unsigned int)pfl->self_test_int);
////							break;
////							case 6:
////									sprintf(strval,"%i",(int)pfl->control_int);
////							break;
////							case 7:
////									sprintf(strval,"%i",(int)pfl->adc_smp_int);
////							break;
////							case 8:
////									sprintf(strval,"0x%04x",pfl->tsts_active & 0x0000FFFF);
////							break;
////							case 9:
////									sprintf(strval,"0x%02x",pfl->refelec_active & 0x0f);
////							break;
////							case 10:
////									sprintf(strval,"%s",pfl->refeleclongname[0]); //EAJ 1/14/2016
////							break;
////							case 11:
////									sprintf(strval,"%s",pfl->refeleclongname[1]); //EAJ 1/14/2016
////							break;
////							case 12:
////									sprintf(strval,"%s",pfl->refeleclongname[2]); //EAJ 1/14/2016
////							break;
////							case 13:
////									sprintf(strval,"%s",pfl->refeleclongname[3]); //EAJ 1/14/2016
////							break;
////				}
//				sprintf(strval,"PROFILE: %d",row+1);
//				LISTVIEW_SetItemText(hItem, 0, row, strval);
//		}
		if(LS_ACTIVE==1)//Load
		{
			LISTVIEW_AddRow(hItem, NULL);LISTVIEW_SetItemText(hItem, 0, 0, "Node Live");
		}
		else //Save
		{
			LISTVIEW_AddRow(hItem, NULL);LISTVIEW_SetItemText(hItem, 0, 0, "Node Preset");
		}
		LISTVIEW_AddRow(hItem, NULL);LISTVIEW_SetItemText(hItem, 0, 1, "Local Preset 1");
		LISTVIEW_AddRow(hItem, NULL);LISTVIEW_SetItemText(hItem, 0, 2, "Local Preset 2");
		LISTVIEW_AddRow(hItem, NULL);LISTVIEW_SetItemText(hItem, 0, 3, "Local Preset 3");
		//LISTVIEW_AddRow(hItem, NULL);sprintf(strval,"Default %d",LS_ACTIVE);LISTVIEW_SetItemText(hItem, 0, 4, strval);
		if(LS_ACTIVE==1)//LOAD
		{
			LISTVIEW_AddRow(hItem, NULL);LISTVIEW_SetItemText(hItem, 0, 4, "Default");
		}
		
		
		//ListViewLoaded = 1;
		
}

char* pBinFill(uint32_t x,char *so, int width, char fillChar)
{ // fill in array from right to left
 //int    width=16;
// char s[width+1];
// int    i=width;
//	s[width]=0x00;   // terminate string

	 char s[width+1];
 int    i=width-1;
	s[width]=0x00;   // terminate string
	
// while(i>=0)
// {	 
//	 s[i--]=fillChar;    // fill with fillChar 
// }
 //s[i--]=0x00;   // terminate string
//i=width;
 do
 {
  s[i--]=(x & 1) ? '1':'0';
  x>>=1;  // shift right 1 bit
 } while( x > 0);
 //while(i>0) s[i--]=fillChar;    // fill with fillChar 
 while(i>=0)
 {	 
	 s[i--]=fillChar;    // fill with fillChar 
 }
 sprintf(so,"%s",s);
 return so;
}

/*************************** End of file ****************************/
