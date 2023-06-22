/*********************************************************************
  * @author  Vince Keiper AstrodyneTDI
  * @version V1.0.0
  * @date    18-Sept-2015
  * @brief   ICCP Constant Potential screen  
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
#include "stdlib.h"
#include "iccpfuncs.h"
#include "iccp_canfuncs.h"
#include "string.h"
#include "globals.h"

/**
*
*  External Variables
*
**/
extern K_ModuleItem_Typedef  iccp_main;
extern WM_HWIN iccpmain_hdl;
extern WM_HWIN Createiccp_main(void);
extern WM_HWIN Createlcdkeypad(uint8_t alpha_ena);

extern GUI_CONST_STORAGE GUI_BITMAP bmCheckBox_Checked_24_16x16;
extern GUI_CONST_STORAGE GUI_BITMAP bmCheckBox_Unchecked_24_16x16;

extern GUI_CONST_STORAGE GUI_BITMAP bmChkBox_1_24_16x16;
extern GUI_CONST_STORAGE GUI_BITMAP bmChkBox_0_24_16x16;

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
//Origins (i.e. X, Y or pX,pY) and sizes (i.e. W, H or dX,dY)
#define BTNX 66
#define BTNY 66
#define EDX  140
#define EDY  50
#define MGN  30
#define MGNX 24  //X axis Margin
#define SCNX 470  
#define SCNY 270
#define SCNY 270  

#define CHKPX 20 //175// distance from left for chkboxes
#define CHKDX 15 //130// len chkboxes
#define CHKDY 15 //130// len chkboxes
#define CHKDY2 27 // height checkboxes

#define LVHH 18 // ListView Header Height
#define LVRH 28 //27 // ListView Row Height 

#define LVDX 397 //392 //372
#define LVDY ((LVRH+1)*4)+LVHH //+1 //was 5

#define MGL	5 //10 //20	//Margin Left
#define MGR	MGL+5	//Margin Right
#define MGT	3		//Margin Top
#define MGB	MGT	//Margin Bottom
#define MGG	3		//Margin Global

//#define EDW	130 //EDIT (box) Width
#define EDW	(LVDX/3) //(LVDX-MGG-MGG)/3 //EDIT (box) Width
#define EDH	28 //30 //37	//EDIT (box) Height

#define TXW	EDW //TEXT (box) Width
#define TXH	18 //18 //17	//TEXT (box) Height

#define BTW	60	//BUTTON Width
#define BTH	((LVDY-(MGG*4))/3) //35 //40	//BUTTON Height

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_FRAMEWIN_0 (GUI_ID_USER + 0x00)
#define ID_CHECKBOX_REF0 (GUI_ID_USER + 0x01)
#define ID_TEXT_0 (GUI_ID_USER + 0x02)
#define ID_EDIT_MIN (GUI_ID_USER + 0x03)
#define ID_EDIT_MAX (GUI_ID_USER + 0x04)
#define ID_TEXT_1 (GUI_ID_USER + 0x05)
#define ID_TEXT_2 (GUI_ID_USER + 0x06)
#define ID_LISTVIEW_0 (GUI_ID_USER + 0x07)
#define ID_BUTTON_HOME (GUI_ID_USER + 0x08)
#define ID_BUTTON_SAVE (GUI_ID_USER + 0x09)
#define ID_BUTTON_CANCEL (GUI_ID_USER + 0x0A)
#define ID_CHECKBOX_REF1 (GUI_ID_USER + 0x0B)
#define ID_CHECKBOX_REF2 (GUI_ID_USER + 0x0C)
#define ID_CHECKBOX_REF3 (GUI_ID_USER + 0x0D)
#define ID_EDIT_DSC (GUI_ID_USER + 0x0E)// ref elec description 
#define ID_TEXT_DSC (GUI_ID_USER + 0x0F)// ref elec description 
#define ID_EDIT_DMD (GUI_ID_USER + 0x10)
#define ID_TEXT_LVW (GUI_ID_USER + 0x11)
#define ID_TEXT_SETPTS (GUI_ID_USER + 0x12)
#define ID_HDR_SETPTS (GUI_ID_USER + 0x13)


#define XSCN 480  // x dimension screen width
#define YSCN 245 // y dimension screen height

#define HSCN YSCN-30 //Visible screen height (subtracting title height and border widths)

#define GUI_EXTRALIGHTGRAY     0x00E0E0E0

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
#define MAXROWS 4
#define MAXCOLS 4
#define QTYCHARS 16  
#define DESCCOL 2
                     //Rows Cols  Strings 
//static char _acListStr[4]   [5]   [16] = 
//{// Col0   Col1     Col2     Col3
//	{ {" "},{"0"},{"Starboard-Bow"}	,{"-0.02"},{"PASS"}	},  //ROW 0
//	{ {" "},{"1"},{"Port-Bow"}			,{"0.31"}, {"PASS"}	},  //ROW 1
//	{ {" "},{"2"},{"Starboard-Aft"}	,{"0.46"}, {"PASS"}	},  //ROW 2
//	{ {" "},{"3"},{"Port-Aft"}			,{"1.23"}, {"FAIL"}	},  //ROW 3
//};


/**
*
* Locals
**/
uint16_t orig_cpminval,orig_cpmaxval,orig_cpdmdval;
typedef enum{CP_EDIT_INIT,CP_MAX_CHG,CP_DMD_CHG,CP_MIN_CHG,CP_DSC_CHG,CP_EDIT_SAVE} cp_edit_state_t;

cp_edit_state_t cp_edit_state;

WM_HWIN hWin_cpDLG;
WM_HWIN hWin_LV;

GUI_RECT hLvRect;

IccpPrfParam_t    orig_pflparams,edit_pflparams; //Local copy

static int EditLVRow;
	int Xpos;
	int Ypos;
	static uint8_t refelec_active[4];

//Prototypes

//void ResetLocalProfile(void);
//void ResetGuiStateProfile(void);

//void UpdateStatus(WM_HWIN hItem);
void UpdateStatus(WM_MESSAGE *pMsg);
void ShowControls(WM_MESSAGE *pMsg);
void ResetControlText(WM_MESSAGE *pMsg);

/*********************************************************************
*
*       _aDialogCreate
*/
//static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = 
//	{//                       p=position d-dimensions    pX pY  dX    dY
//  { FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_0, 0, 25, XSCN, YSCN, 0, 0x64, 0 },
//  
//  { EDIT_CreateIndirect, "Edit", ID_EDIT_MIN, 20, 25, 125, 40, 0, 0x5, 0 },
//  { TEXT_CreateIndirect, "Min Threshold Volts", ID_TEXT_1, 27, 70, 115, 15, 0, 0x0, 0 },
//	
//  { TEXT_CreateIndirect, "Active Electrodes", ID_TEXT_0,      164,  1,   169,    20,  0, 0x0, 0 },
//	{ CHECKBOX_CreateIndirect, "chk_ref_0", ID_CHECKBOX_REF0, CHKPX, 21,  CHKDX, CHKDY, 0, 0x0, 0 },
//  { CHECKBOX_CreateIndirect, "chk_ref_1", ID_CHECKBOX_REF1, CHKPX, 21+CHKDY*1, CHKDX, CHKDY, 0, 0x0, 0 }, 
//	{ CHECKBOX_CreateIndirect, "chk_ref_2", ID_CHECKBOX_REF2, CHKPX, 21+CHKDY*2, CHKDX, CHKDY, 0, 0x0, 0 }, 
//	{ CHECKBOX_CreateIndirect, "chk_ref_3", ID_CHECKBOX_REF3, CHKPX, 21+CHKDY*3, CHKDX, CHKDY, 0, 0x0, 0 }, 
//	
//	{ EDIT_CreateIndirect, "Edit", ID_EDIT_MAX, 330, 25, 125, 40, 0, 0x4, 0 },
//	{ TEXT_CreateIndirect, "Max Threshold Volts", ID_TEXT_2, 334, 70, 115, 15, 0, 0x0, 0 },
//  
//	{ LISTVIEW_CreateIndirect, "Listview", ID_LISTVIEW_0,  10, YSCN-100, 280, 80, 0, 0x0, 0 },
//  { BUTTON_CreateIndirect, "Home", ID_BUTTON_0, 409, YSCN-110, 54, 86, 0, 0x0, 0 },
//  { BUTTON_CreateIndirect, "Save", ID_BUTTON_1, 305, YSCN-110, 90, 40, 0, 0x0, 0 },
//  { BUTTON_CreateIndirect, "Cancel", ID_BUTTON_2, 305, YSCN-63, 90, 40, 0, 0x0, 0 },
//  
//};

//CONTROL_CreateIndirect, "TextString", ID_CONTROL, X, Y, W, H, ?, Max_Characters, ? },
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = 
	{//                       p=position d-dimensions    pX pY  dX    dY
  { FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_0, 0, 25, XSCN, YSCN, 0, 0x64, 0 },
  
	{ TEXT_CreateIndirect, "Setpoints",	ID_TEXT_SETPTS,		MGL, MGT, 		LVDX, TXH,  0, 0x0, 0 },
	{ HEADER_CreateIndirect, "Hdr Setpts", 	ID_HDR_SETPTS,  MGL, MGT+TXH, 		LVDX, TXH, 0, 0x0, 0 },
	
  //{ TEXT_CreateIndirect, "Minimum", ID_TEXT_1, 							MGL,	MGT+TXH,			TXW, TXH, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "Edit Min",ID_EDIT_MIN, 						MGL,	MGT+TXH+TXH,	EDW, EDH, 0, 0x5, 0 },	

	//{ TEXT_CreateIndirect, "Demand", 	ID_TEXT_0,	MGL+TXW,		MGT+TXH, 			TXW, TXH, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "Edit Dmd",ID_EDIT_DMD,MGL+EDW,		MGT+TXH+TXH, 	EDW, EDH, 0, 0x5, 0 },	

	//{ TEXT_CreateIndirect, "Maximum", ID_TEXT_2,	MGL+TXW+TXW, 	MGT+TXH,			TXW, TXH, 0, 0x0, 0 },
	{ EDIT_CreateIndirect, "Edit Max",ID_EDIT_MAX,MGL+EDW+EDW, 	MGT+TXH+TXH,	EDW, EDH, 0, 0x4, 0 }, 	

	{ TEXT_CreateIndirect, "Electrodes", 		ID_TEXT_LVW,		MGL, MGT+TXH+EDH+MGG+TXH, 		LVDX, TXH,  0, 0x0, 0 },	
	{ LISTVIEW_CreateIndirect, "Listview", 	ID_LISTVIEW_0,  MGL, MGT+TXH+EDH+MGG+TXH+TXH, LVDX, LVDY, 0, 0x0, 0 },

//  { BUTTON_CreateIndirect, "Set", 		ID_BUTTON_SAVE, XSCN-BTW-MGR, MGT+TXH+EDH+MGG+TXH+TXH, 							BTW, BTH, 0, 0x0, 0 },
//  { BUTTON_CreateIndirect, "Cancel", 	ID_BUTTON_CANCEL, XSCN-BTW-MGR, MGT+TXH+EDH+MGG+TXH+BTH+MGG+MGG+TXH,				BTW, BTH, 0, 0x0, 0 },
//	{ BUTTON_CreateIndirect, "Home", 		ID_BUTTON_HOME, XSCN-BTW-MGR, MGT+TXH+EDH+MGG+TXH+((BTH+MGG+MGG)*2)+TXH,	BTW, BTH, 0, 0x0, 0 },

	{ BUTTON_CreateIndirect, "Set", 		ID_BUTTON_SAVE, MGL+LVDX+MGG, MGT+TXH+EDH+MGG+TXH+TXH, 							BTW, BTH, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Cancel", 	ID_BUTTON_CANCEL, MGL+LVDX+MGG, MGT+TXH+EDH+MGG+TXH+BTH+MGG+MGG+TXH,				BTW, BTH, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, "Home", 		ID_BUTTON_HOME, MGL+LVDX+MGG, MGT+TXH+EDH+MGG+TXH+((BTH+MGG+MGG)*2)+TXH,	BTW, BTH, 0, 0x0, 0 },

		
//	{ CHECKBOX_CreateIndirect, "chk_ref_0", ID_CHECKBOX_REF0, CHKPX, 1,  	 CHKDX, CHKDY, 0, 0x0, 0 },
//  { CHECKBOX_CreateIndirect, "chk_ref_1", ID_CHECKBOX_REF1, CHKPX+30, 1, CHKDX, CHKDY, 0, 0x0, 0 }, 
//	{ CHECKBOX_CreateIndirect, "chk_ref_2", ID_CHECKBOX_REF2, CHKPX+60, 1, CHKDX, CHKDY, 0, 0x0, 0 }, 
//	{ CHECKBOX_CreateIndirect, "chk_ref_3", ID_CHECKBOX_REF3, CHKPX+90, 1, CHKDX, CHKDY, 0, 0x0, 0 },	
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

// USER START (Optionally insert additional static code)
// USER END

/*********************************************************************
*
*       _cbDialog
*/
static void _cbDialog(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;
	char titlestr[32];
//	static uint8_t refelec_active[4];
	//static uint8_t myrow;
	//static uint8_t mylastrow = -1;	
	//static int myrow;
	static int mylastrow = -1;	
	static int mycol;
	//static int LastYP;
//	int Xpos;
//	int Ypos;
	char edits_set = 0;
	
	ShowControls(pMsg);


  switch (pMsg->MsgId) 
	{
			case WM_INIT_DIALOG:
				
				//Populate names locally for now
			//uint16_t  Status;
//			if((GuiState.SysDataPSU[GuiState.psu_viewing].Status & 0x0004)== 0x0000) //Profile not received
//			{			
//				GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.refelec_active = 0;
//				GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.refelec_min_th = 0;
//				GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.refelec_dmd_setpt = 0;
//				GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.refelec_max_th = 0;
//				strcpy(GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.refeleclongname[0],"<PFL NOT RCVD>");
//				strcpy(GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.refeleclongname[1],"<PFL NOT RCVD>");
//				strcpy(GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.refeleclongname[2],"<PFL NOT RCVD>");
//				strcpy(GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.refeleclongname[3],"<PFL NOT RCVD>");
//			}
				
			 //Save pflparams for cancel
				orig_pflparams = GuiState.SysDataPSU[GuiState.psu_viewing].pflparams;

//				orig_cpminval = GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.refelec_min_th;
//				orig_cpmaxval = GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.refelec_max_th;
//				orig_cpdmdval = GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.refelec_dmd_setpt;

				orig_cpminval = orig_pflparams.refelec_min_th;				
				orig_cpmaxval = orig_pflparams.refelec_max_th;				
				orig_cpdmdval = orig_pflparams.refelec_dmd_setpt;
			
			//Save pflparams for editing
			edit_pflparams = GuiState.SysDataPSU[GuiState.psu_viewing].pflparams; 
				
				cp_edit_state = CP_EDIT_INIT;

				//
				// Initialization of 'Framewin'
				//
			sprintf(titlestr,"Constant Potential Mode for NODE: %i",GuiState.psu_viewing);
				hItem = pMsg->hWin;
				FRAMEWIN_SetFont(hItem, GUI_FONT_16B_1);
				FRAMEWIN_SetText(hItem, titlestr );
				FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
				FRAMEWIN_SetClientColor(hItem,GUI_WHITE);  
				FRAMEWIN_SetSkin(hItem,FRAMEWIN_SKIN_FLEX);
				
				//
				// Initialization of 'Setpoints' TEXT (Label)
				//
				hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_SETPTS);
				TEXT_SetFont(hItem, GUI_FONT_16B_1); //Was GUI_FONT_16B_1
				TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
				TEXT_SetBkColor(hItem, GUI_EXTRALIGHTGRAY);
				
				//
				// Initialization of 'Header Setpoints' (For EDIT boxes)
				//
				hItem = WM_GetDialogItem(pMsg->hWin, ID_HDR_SETPTS);

				HEADER_SetFont(hItem, GUI_FONT_16B_1);

				HEADER_SetBkColor(hItem, GUI_LIGHTGRAY); //was GUI_EXTRALIGHTGRAY
				HEADER_SetTextColor(hItem, GUI_BLACK);
				HEADER_SetSkin(hItem,HEADER_SKIN_FLEX);
				HEADER_SetHeight(hItem, TXH);//LVHH
				
				//NOTE: GUI_TA_TOP - Text hugs bottom, GUI_TA_BOTTOM - Text hugs top, WEIRD!!!

				HEADER_AddItem(hItem,TXW, "Minimum", GUI_TA_HCENTER | GUI_TA_BOTTOM);
				HEADER_AddItem(hItem,TXW, "Demand", GUI_TA_HCENTER | GUI_TA_BOTTOM);
				HEADER_AddItem(hItem,TXW, "Maximum", GUI_TA_HCENTER | GUI_TA_BOTTOM);
		
				//
				// Initialization of 'Edit' MIN
				//
				hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_MIN);
				EDIT_SetFont(hItem, GUI_FONT_24B_1); //Was GUI_FONT_D24X32 then GUI_FONT_32B_1
				EDIT_SetTextColor(hItem,EDIT_CI_ENABLED, GUI_BLUE);//0x000303E7);
				EDIT_SetTextAlign(hItem,GUI_TA_CENTER | GUI_TA_VCENTER);
					
				//
				// Initialization of 'Edit Demand'
				//
				hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_DMD);
				EDIT_SetFont(hItem, GUI_FONT_24B_1);//Was GUI_FONT_D24X32 then GUI_FONT_32B_1
				EDIT_SetTextColor(hItem,EDIT_CI_ENABLED, GUI_BLUE);//0x000303E7);
				EDIT_SetTextAlign(hItem,GUI_TA_CENTER | GUI_TA_VCENTER);

				//
				// Initialization of 'Edit' MAX
				//
				hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_MAX);
				EDIT_SetFont(hItem, GUI_FONT_24B_1);//Was GUI_FONT_D24X32 then GUI_FONT_32B_1
				EDIT_SetTextColor(hItem,EDIT_CI_ENABLED, GUI_BLUE);//0x000303E7);
				EDIT_SetTextAlign(hItem,GUI_TA_CENTER | GUI_TA_VCENTER);

				//
				// Initialization of 'Electrodes' TEXT (Label)
				//
				hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_LVW);
				TEXT_SetFont(hItem, GUI_FONT_16B_1);
				TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
				TEXT_SetBkColor(hItem, GUI_EXTRALIGHTGRAY);
				
				//
				// Initialization of 'Listview'
				//
				hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);

				HEADER_SetFont(LISTVIEW_GetHeader(hItem), GUI_FONT_16B_1);
				HEADER_SetBkColor(LISTVIEW_GetHeader(hItem), GUI_LIGHTGRAY); //was GUI_EXTRALIGHTGRAY
				HEADER_SetTextColor(LISTVIEW_GetHeader(hItem), GUI_BLACK);
				HEADER_SetSkin(LISTVIEW_GetHeader(hItem),HEADER_SKIN_FLEX);
				
				LISTVIEW_SetFont(hItem, GUI_FONT_24B_1); //Was GUI_FONT_20_1
				LISTVIEW_SetRowHeight(hItem, LVRH);
				LISTVIEW_SetHeaderHeight(hItem, LVHH);
				LISTVIEW_SetGridVis(hItem, 1);

				LISTVIEW_AddColumn(hItem, 45, "Active", GUI_TA_HCENTER | GUI_TA_VCENTER);
				LISTVIEW_AddColumn(hItem, 20, "ID", GUI_TA_HCENTER | GUI_TA_VCENTER);
				LISTVIEW_AddColumn(hItem, 195, "Description", GUI_TA_LEFT | GUI_TA_VCENTER);//Was 185 wide
				LISTVIEW_AddColumn(hItem, 70, "Reading", GUI_TA_HCENTER | GUI_TA_VCENTER);//Was 60 wide
				LISTVIEW_AddColumn(hItem, 62, "Status", GUI_TA_HCENTER | GUI_TA_VCENTER); //Was 57 wide
				
				//Can't set all header text align until we know how many columns
				HEADER_SetTextAlign(LISTVIEW_GetHeader(hItem), 0, GUI_TA_HCENTER | GUI_TA_BOTTOM);
				HEADER_SetTextAlign(LISTVIEW_GetHeader(hItem), 1, GUI_TA_HCENTER | GUI_TA_BOTTOM);
				HEADER_SetTextAlign(LISTVIEW_GetHeader(hItem), 2, GUI_TA_HCENTER | GUI_TA_BOTTOM);
				HEADER_SetTextAlign(LISTVIEW_GetHeader(hItem), 3, GUI_TA_HCENTER | GUI_TA_BOTTOM);
				HEADER_SetTextAlign(LISTVIEW_GetHeader(hItem), 4, GUI_TA_HCENTER | GUI_TA_BOTTOM);
				
				//Load control text with profile data
				ResetControlText(pMsg);
				
				//Set ListView Readings and Status
				UpdateStatus(pMsg);
					
//				//
//				// Initialization of 'chk_ref_0, chk_ref1,chk_ref2,chk_ref3'
//				//
				
			break;
			
	case WM_PAINT:
		UpdateStatus(pMsg);
		break;
  
	case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
	
		GuiState.ScreenTimeout=0;
		char strbuff[32];
	
		switch(Id)
		{
		
			case ID_BUTTON_HOME: // Notifications sent by 'Home_Btn'
					switch(NCode) 
					{
							case WM_NOTIFICATION_RELEASED:
										if((GuiState.cpsrcn_state == G_CP_INIT)||(cp_edit_state == CP_EDIT_SAVE))
										 {
													WM_InvalidateWindow(iccpmain_hdl);
													WM_Paint(iccpmain_hdl);
													GUI_EndDialog(pMsg->hWin,NULL);
													GuiState.gui_scn_state = G_RTSTA;
													GuiState.cpsrcn_state = G_CP_INIT;
										 }
								break;
					}
      break;	
			
			case ID_BUTTON_CANCEL: // Notifications sent by 'Cancel_Btn'
					switch(NCode) 
					{
							case WM_NOTIFICATION_RELEASED:
										if(GuiState.cpsrcn_state == G_CP_INIT)
										{
												ResetControlText(pMsg);
										}
												
										if(GuiState.cpsrcn_state != G_CP_INIT &&
									 GuiState.cpsrcn_state != G_CP_KYBUP)
									 {
												if(cp_edit_state == CP_MAX_CHG)
												{
														// Set Edit Box back to Value we entered this screen with
														hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_MAX);  /* Get the handle of the edit widget */
														sprintf(dbgstr,"%.2f",(float)orig_cpmaxval/1000);
														EDIT_SetTextColor(hItem,EDIT_CI_ENABLED, GUI_BLUE);//0x000303E7);
														EDIT_SetText(hItem, dbgstr);
												}
												else if(cp_edit_state == CP_DMD_CHG)
												{
														// Set Edit Box back to Value we entered this screen with
														hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_DMD);  /* Get the handle of the edit widget */
														sprintf(dbgstr,"%.2f",(float)orig_cpdmdval/1000);
														EDIT_SetTextColor(hItem,EDIT_CI_ENABLED, GUI_BLUE);//0x000303E7);
														EDIT_SetText(hItem, dbgstr);
												}												
												else if(cp_edit_state == CP_MIN_CHG)
												{
														hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_MIN);  /* Get the handle of the edit widget */
														sprintf(dbgstr,"%.2f",(float)orig_cpminval/1000);
														EDIT_SetTextColor(hItem,EDIT_CI_ENABLED, GUI_BLUE);//0x000303E7);
														EDIT_SetText(hItem, dbgstr);
												}
												else if(cp_edit_state == CP_DSC_CHG)
												{
													if(GuiState.cpsrcn_state == G_CP_KYBDN)
													{
														  EDIT_Delete(WM_GetDialogItem(pMsg->hWin, ID_TEXT_DSC));
															EDIT_Delete(WM_GetDialogItem(pMsg->hWin, ID_EDIT_DSC));
														
//														hItem = pMsg->hWin;
//														//ID_FRAMEWIN_0
//														WM_MoveTo(hItem, 0, LastYP);
//														WM_InvalidateWindow(iccpmain_hdl);
//														WM_Paint(iccpmain_hdl);
													}
												}
												else
												{
														//ResetControlText(pMsg);
												}
												
												GuiState.cpsrcn_state = G_CP_INIT;
												cp_edit_state = CP_EDIT_INIT;
												BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_SAVE), "Set");
									}
						break;
					}
      break;
					
			case ID_BUTTON_SAVE: // Notifications sent by 'Set' or 'Save' btn
						switch(NCode) 
						{
								case WM_NOTIFICATION_RELEASED:
									hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_SAVE);
									
									// If Button was pressed and is in INIT state 
									// then lets set button into SAVE mode
								//EAJ 12/08/2015: Why are we doing this?
//									if(GuiState.cpsrcn_state == G_CP_INIT  || 
//											GuiState.cpsrcn_state == G_CP)
//									{
//											BUTTON_SetText(hItem, "Save");
//										
//											if(cp_edit_state == CP_MAX_CHG)
//											{
//													hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_MAX);
//											}		
//											else
//											{
//													hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_MIN);
//											}
//											EDIT_SetTextColor(hItem,1,GUI_RED);
//											EDIT_EnableBlink(hItem, 500, 1);
//											EDIT_SetText(hItem,"");	
//											EDIT_SetCursorAtChar(hItem, 0);
//											Createlcdkeypad(0);// num only alpha disabled
//									}
								
								  
									// If Keyboard just went down then
									// Button button is pressed then "Save" was pressed
									// so lets save and go back to "Set" mode
									if(GuiState.cpsrcn_state == G_CP_KYBDN)
									{
												//GuiState.cpsrcn_state = G_CP_SAVE;
												//BUTTON_SetText(hItem, "Set"); //EAJ 12/08/2015: Why are we doing this?
										
												if(cp_edit_state == CP_MAX_CHG)
												{
														hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_MAX);
												}		
												else if( cp_edit_state == CP_DMD_CHG)
												{
														hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_DMD);
												}
												else if( cp_edit_state == CP_MIN_CHG)
												{
														hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_MIN);
												}
												else
												{
														hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_DSC);
												}
												
												memset(dbgstr,0x00,sizeof(dbgstr));
												EDIT_GetText(hItem,dbgstr,sizeof(dbgstr));
												
												// Save values from EDIT box into data struct
												if(cp_edit_state == CP_MAX_CHG)
												{
														edit_pflparams.refelec_max_th   = (signed int)(atof(dbgstr)*1000);
														hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_MAX);
														EDIT_SetTextColor(hItem,1,GUI_BLUE);
														EDIT_EnableBlink(hItem, 500, 0);
														sprintf(dbgstr,"%.2f",(float)edit_pflparams.refelec_max_th/1000); //Repopulate formatted data
														EDIT_SetText(hItem, dbgstr);                   /* Set text */
													  edits_set = 1;
												}
												else if( cp_edit_state == CP_DMD_CHG)
												{
														edit_pflparams.refelec_dmd_setpt   = (signed int)(atof(dbgstr)*1000);
														hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_DMD);
														EDIT_SetTextColor(hItem,1,GUI_BLUE);
														EDIT_EnableBlink(hItem, 500, 0);
														sprintf(dbgstr,"%.2f",(float)edit_pflparams.refelec_dmd_setpt/1000); //Repopulate formatted data
														EDIT_SetText(hItem, dbgstr);                   /* Set text */
													  edits_set = 1;
												}												
												else if( cp_edit_state == CP_MIN_CHG)
												{
														edit_pflparams.refelec_min_th   = (signed int)(atof(dbgstr)*1000);
														hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_MIN);
														EDIT_SetTextColor(hItem,1,GUI_BLUE);
														EDIT_EnableBlink(hItem, 500, 0);
														sprintf(dbgstr,"%.2f",(float)edit_pflparams.refelec_min_th/1000); //Repopulate formatted data
														EDIT_SetText(hItem, dbgstr);                   /* Set text */
													  edits_set = 1;
												}
												else if(cp_edit_state == CP_DSC_CHG)
												{
													  EDIT_Delete(WM_GetDialogItem(pMsg->hWin, ID_TEXT_DSC));
														EDIT_Delete(WM_GetDialogItem(pMsg->hWin, ID_EDIT_DSC));
													
														hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
														LISTVIEW_SetItemText(hItem, DESCCOL, EditLVRow, dbgstr);
													  strncpy(edit_pflparams.refeleclongname[EditLVRow],dbgstr,sizeof(edit_pflparams.refeleclongname[EditLVRow]));
														edits_set = 1;
												}
												else
												{
														cp_edit_state = CP_EDIT_SAVE; //Save whatever we have now in edit_pflparams
													  BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_SAVE), "Save");
												}
												
												if(cp_edit_state == CP_EDIT_SAVE)
												{
													//GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.psu_mode   = P_CPOT;
													edit_pflparams.psu_mode = P_CPOT;
													
													GuiState.SysDataPSU[GuiState.psu_viewing].pflparams = edit_pflparams;
													
													GuiState.cpsrcn_state = G_CP_INIT;
													cp_edit_state = CP_EDIT_INIT;

													if(!GuiState.demo){while(!(BITCHK(GuiState.SysDataPSU[GuiState.psu_viewing].Status,3)));}//Wait for status update to complete
													CANWrite(GuiState.psu_viewing,0x00,CMD_CAN_204,(uint8_t*)dbgstr,0);// send profile to PSU RAM
													//NOTE: EAJ 12/09/2015 - dbgstr placeholder only - not sent because len = 0. Notify that profile ready to send.
													BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_SAVE), "Set");
												}
												
												if(edits_set == 1)
												{
													cp_edit_state = CP_EDIT_SAVE;
													BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_SAVE), "Save");
												}									
									}
									break;
						}
		  break;//break ID_BUTTON_SAVE
					
//      case ID_CHECKBOX_REF0: // Notifications sent by 'Checkbox Reference Electrode 0'
//			case ID_CHECKBOX_REF1: // Notifications sent by 'Checkbox Reference Electrode 1'
//			case ID_CHECKBOX_REF2: // Notifications sent by 'Checkbox Reference Electrode 2'
//			case ID_CHECKBOX_REF3: // Notifications sent by 'Checkbox Reference Electrode 3'
//			
//					switch(NCode) {
//								case WM_NOTIFICATION_RELEASED:
//											switch (Id)
//											{
//														case ID_CHECKBOX_REF0:
//																	hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_REF0);
//																	if(!CHECKBOX_GetState(hItem))
//																	{
//																			GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.refelec_active &= 0xfe;//and off bit0
//																	}
//																	else
//																	{
//																			GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.refelec_active |= 0x01;//or on bit0
//																	}
//														break;
//													  
//														case ID_CHECKBOX_REF1:
//																	hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_REF1);
//																	if(!CHECKBOX_GetState(hItem))
//																	{
//																			GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.refelec_active &= 0xfd;//and off bit1
//																	}
//																	else
//																	{
//																			GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.refelec_active |= 0x02;//or on bit1
//																	}
//														break;
//																	
//														case ID_CHECKBOX_REF2:
//																	hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_REF2);
//																	if(!CHECKBOX_GetState(hItem))
//																	{
//																			GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.refelec_active &= 0xfb;//and off bit2
//																	}
//																	else
//																	{
//																			GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.refelec_active |= 0x04;//or on bit2
//																	}
//														break;			
//																	
//														case ID_CHECKBOX_REF3:
//																	hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_REF3);
//																	if(!CHECKBOX_GetState(hItem))
//																	{
//																			GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.refelec_active &= 0xf7;//and off bit3
//																	}
//																	else
//																	{
//																			GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.refelec_active |= 0x08;//or on bit3
//																	}
//														break;			
//											}
//								break;
//					}
//      break;
					
			case ID_EDIT_MIN: // Notifications sent by 'Edit'
						switch(NCode) 
						{
							case WM_NOTIFICATION_RELEASED:
											if((GuiState.cpsrcn_state == G_CP_INIT)||(cp_edit_state == CP_EDIT_SAVE))
											{
													if(GuiState.cpsrcn_state != G_CP_KYBUP)
													{
															cp_edit_state = CP_MIN_CHG;
															hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_SAVE);
															BUTTON_SetText(hItem, "Set");
															
															hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_MIN);
															EDIT_SetTextColor(hItem,1,GUI_RED);
															EDIT_EnableBlink(hItem, 500, 1);
															EDIT_SetText(hItem,"");
															EDIT_SetCursorAtChar(hItem, 0);
															
															Createlcdkeypad(0);// num only alpha disabled
													}
											}
								break;
						}
			break;
						
			case ID_EDIT_DMD: // Notifications sent by 'Edit'
						switch(NCode) 
						{
							case WM_NOTIFICATION_RELEASED:
											if((GuiState.cpsrcn_state == G_CP_INIT)||(cp_edit_state == CP_EDIT_SAVE))
											{
													if(GuiState.cpsrcn_state != G_CP_KYBUP)
													{
															cp_edit_state = CP_DMD_CHG;
															hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_SAVE);
															BUTTON_SetText(hItem, "Set");
															
															hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_DMD);
															EDIT_SetTextColor(hItem,1,GUI_RED);
															EDIT_EnableBlink(hItem, 500, 1);
															EDIT_SetText(hItem,"");
															EDIT_SetCursorAtChar(hItem, 0);
															
															Createlcdkeypad(0);// num only alpha disabled
													}
											}
								break;
						}
			break;
    
			case ID_EDIT_MAX: // Notifications sent by 'Edit'
					switch(NCode) 
					{
							case WM_NOTIFICATION_RELEASED:
										if((GuiState.cpsrcn_state == G_CP_INIT)||(cp_edit_state == CP_EDIT_SAVE))
										{
												if(GuiState.cpsrcn_state != G_CP_KYBUP)
												{
														cp_edit_state = CP_MAX_CHG;
														hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_SAVE);
														BUTTON_SetText(hItem, "Set");
														
														hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_MAX);
														EDIT_SetTextColor(hItem,1,GUI_RED);
														EDIT_EnableBlink(hItem, 500, 1);
														EDIT_SetText(hItem,"");
														EDIT_SetCursorAtChar(hItem, 0);
														
														Createlcdkeypad(0);// num only alpha disabled
												}
										}
							break;
					}
			break;
				
			case ID_LISTVIEW_0: // Notifications sent by 'Listview'
						switch(NCode) 
						{
								case WM_NOTIFICATION_CLICKED:
												hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
												hWin_LV = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
												EditLVRow = LISTVIEW_GetSel(hItem);
												mycol = LISTVIEW_GetSelCol(hItem);
												LISTVIEW_GetItemText(hItem,DESCCOL,EditLVRow,strbuff,16);
								
												//sprintf(dbgstr,"Electrode %i Description:",myrow);
												//TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0),dbgstr);
								
												if(mycol==0)
												{
													
										if((GuiState.cpsrcn_state == G_CP_INIT)||(cp_edit_state == CP_EDIT_SAVE))
										{
//												if(GuiState.cpsrcn_state != G_CP_KYBUP)
//												{
//														cp_edit_state = CP_MAX_CHG;
//														hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_SAVE);
//														BUTTON_SetText(hItem, "Set");
//														
//														hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_MAX);
//														EDIT_SetTextColor(hItem,1,GUI_RED);
//														EDIT_EnableBlink(hItem, 500, 1);
//														EDIT_SetText(hItem,"");
//														EDIT_SetCursorAtChar(hItem, 0);
//														
//														Createlcdkeypad(0);// num only alpha disabled
//												}
										}
//													if(myrow==0)
//													{
//														hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_REF0);
//													}
//													if(myrow==1)
//													{
//														hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_REF1);
//													}
//													if(myrow==2)
//													{
//														hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_REF2);
//													}
//													if(myrow==3)
//													{
//														hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_REF3);
//													}
//												
//													if(CHECKBOX_IsChecked(hItem))
//													{
//														CHECKBOX_SetState(hItem,0);
//														LISTVIEW_SetItemBitmap(hWin_LV, 0, myrow, 22, 3, &bmChkBox_0_24_16x16);
//													}
//													else
//													{
//														CHECKBOX_SetState(hItem,1);
//														LISTVIEW_SetItemBitmap(hWin_LV, 0, myrow, 22, 3, &bmChkBox_1_24_16x16);
//													}
													
													if(refelec_active[EditLVRow]==0x01)
													{
														refelec_active[EditLVRow]=0x00;
														LISTVIEW_SetItemBitmap(hItem, 0, EditLVRow, 15, 5, &bmChkBox_0_24_16x16);
													}
													else
													{
														refelec_active[EditLVRow]=0x01;
														LISTVIEW_SetItemBitmap(hItem, 0, EditLVRow, 15, 5, &bmChkBox_1_24_16x16);
													}
													
													
											switch (EditLVRow)
											{
														case 0:
																	if(refelec_active[EditLVRow]==0x00)
																	{
																			//GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.refelec_active &= 0xfe;//and off bit0
																		  edit_pflparams.refelec_active &= 0xfe;//and off bit0
																	}
																	else
																	{
																			edit_pflparams.refelec_active |= 0x01;//or on bit0
																	}
														break;
													  
														case 1:
																	if(refelec_active[EditLVRow]==0x00)
																	{
																			edit_pflparams.refelec_active &= 0xfd;//and off bit1
																	}
																	else
																	{
																			edit_pflparams.refelec_active |= 0x02;//or on bit1
																	}
														break;
																	
														case 2:
																	if(refelec_active[EditLVRow]==0x00)
																	{
																			edit_pflparams.refelec_active &= 0xfb;//and off bit2
																	}
																	else
																	{
																			edit_pflparams.refelec_active |= 0x04;//or on bit2
																	}
														break;			
																	
														case 3:
																	if(refelec_active[EditLVRow]==0x00)
																	{
																			edit_pflparams.refelec_active &= 0xf7;//and off bit3
																	}
																	else
																	{
																			edit_pflparams.refelec_active |= 0x08;//or on bit3
																	}
														break;			
												}

												}
								
												if((mycol == DESCCOL)&&(EditLVRow == mylastrow))
												{
//													if(cp_edit_state == CP_DSC_CHG)
//													{
//															hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_DSC);
//															EDIT_Delete(hItem);
//													}
												
											    if(cp_edit_state != CP_DSC_CHG)
													{
							//EDIT_Handle EDIT_CreateEx      (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id, int MaxLen);
													EDIT_CreateEx(235, 59,	185, 22,pMsg->hWin, WM_CF_SHOW,	0, ID_EDIT_DSC,16);
													hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_DSC);
													EDIT_SetFont(hItem, GUI_FONT_20_1);
													EDIT_SetTextAlign(hItem,GUI_TA_LEFT | GUI_TA_VCENTER);

													hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_DSC);
													EDIT_SetText(hItem,strbuff);
														
							//TEXT_Handle TEXT_CreateEx      (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id, const char * pText);
														sprintf(dbgstr,"Electrode %i Description:",EditLVRow);
													TEXT_CreateEx(5, 59,	230, 22,pMsg->hWin, WM_CF_SHOW,	0, ID_TEXT_DSC,dbgstr);
													hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_DSC);
													TEXT_SetFont(hItem, GUI_FONT_20_1);
													TEXT_SetTextAlign(hItem,GUI_TA_RIGHT | GUI_TA_VCENTER);													
													
														//Createlcdkeypad(2);// alpha enabled
														Createlcdkeypad(1);// alpha only

													  cp_edit_state = CP_DSC_CHG;
														BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_SAVE), "Set");
														
														//WM_MoveTo(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), 5, 100);
														WM_HideWindow(WM_GetDialogItem(pMsg->hWin, ID_TEXT_SETPTS));
														WM_HideWindow(WM_GetDialogItem(pMsg->hWin, ID_HDR_SETPTS));
														WM_HideWindow(WM_GetDialogItem(pMsg->hWin, ID_EDIT_MIN));
														WM_HideWindow(WM_GetDialogItem(pMsg->hWin, ID_EDIT_DMD));
														WM_HideWindow(WM_GetDialogItem(pMsg->hWin, ID_EDIT_MAX));
														WM_HideWindow(WM_GetDialogItem(pMsg->hWin, ID_TEXT_LVW));
														WM_HideWindow(WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0));
														WM_HideWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_SAVE));

														
														//LISTVIEW_GetItemRect(hWin_LV, DESCCOL, myrow, &hLvRect);
														
//														hItem = pMsg->hWin;
//														LastYP = WM_GetWindowOrgY(hItem);
//														WM_MoveTo(hItem, 0, LastYP-100);

													}
													
												}
												mylastrow = EditLVRow;
								break;
								
								case WM_NOTIFICATION_RELEASED:
									break;
								
								case WM_NOTIFICATION_LOST_FOCUS:
									if(cp_edit_state != CP_DSC_CHG)
									{
										hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
										LISTVIEW_SetSel(hItem,-1);
									}										
									break;
								
								case WM_NOTIFICATION_SEL_CHANGED:
									if(cp_edit_state == CP_DSC_CHG)
												{
													if(GuiState.cpsrcn_state == G_CP_KYBDN)
													{
														hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_DSC);
														EDIT_Delete(hItem);
														EDIT_Delete(WM_GetDialogItem(pMsg->hWin, ID_TEXT_DSC));
														GuiState.cpsrcn_state = G_CP_INIT;
														cp_edit_state = CP_EDIT_INIT;
													}
												}
								break;
						}
						break;

					}
			break;

		
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}

void ResetControlText(WM_MESSAGE *pMsg)
{
				WM_HWIN hItem;
				 //Save pflparams for cancel
				orig_pflparams = GuiState.SysDataPSU[GuiState.psu_viewing].pflparams;
	
				//Save pflparams for editing
			  edit_pflparams = orig_pflparams; 
				//
				// Initialization of 'Edit' MIN
				//
				hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_MIN);
				EDIT_SetTextColor(hItem,EDIT_CI_ENABLED, GUI_BLUE);//0x000303E7);
				sprintf(dbgstr,"%.2f",(float)orig_pflparams.refelec_min_th/1000);
				EDIT_SetText(hItem, dbgstr);                   /* Set text */
					
				//
				// Initialization of 'Edit Demand'
				//
				hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_DMD);
				EDIT_SetTextColor(hItem,EDIT_CI_ENABLED, GUI_BLUE);//0x000303E7);
				sprintf(dbgstr,"%.2f",(float)orig_pflparams.refelec_dmd_setpt/1000);
				EDIT_SetText(hItem, dbgstr);                   /* Set text */
				//
				// Initialization of 'Edit' MAX
				//
				hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_MAX);
				EDIT_SetTextColor(hItem,EDIT_CI_ENABLED, GUI_BLUE);//0x000303E7);
				sprintf(dbgstr,"%.2f",(float)orig_pflparams.refelec_max_th/1000);
				EDIT_SetText(hItem, dbgstr);                   /* Set text */
				
				hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
				
				int8_t row; //,col;
				for(row=0;row<4;row++)
				{
						LISTVIEW_AddRow(hItem, NULL);
						
						//Set Checkbox bitmaps in col(0) and local array
					//edit_pflparams
						//refelec_active[row]=GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.refelec_active >>row &0x01;
					  refelec_active[row]=orig_pflparams.refelec_active >>row &0x01;
						if(refelec_active[row]==0x01)
						{
							LISTVIEW_SetItemBitmap(hItem, 0, row, 15, 5, &bmChkBox_1_24_16x16);
						}
						else
						{
							LISTVIEW_SetItemBitmap(hItem, 0, row, 15, 5, &bmChkBox_0_24_16x16);
						}
						
						//Set ID string
//						sprintf(dbgstr,"%d",row+1);
//						LISTVIEW_SetItemText(hItem, 0, row, dbgstr);
						
						//Set ID string
						sprintf(dbgstr,"%d",row+1);
						LISTVIEW_SetItemText(hItem, 1, row, dbgstr);
						
						//Set Description string
						//GuiState.SysDataPSU[GuiState.psu_viewing].pflparams
						//sprintf(dbgstr,"%d",sizeof(orig_pflparams));
//						sprintf(dbgstr,"%d",sizeof(GuiState.SysDataPSU[GuiState.psu_viewing].pflparams));
//						LISTVIEW_SetItemText(hItem, 2, row, dbgstr);
						LISTVIEW_SetItemText(hItem, 2, row, orig_pflparams.refeleclongname[row]);					
				}
}

void ShowControls(WM_MESSAGE *pMsg)
{
	WM_HWIN hItem;
			if(cp_edit_state == CP_DSC_CHG)
		{
			if(GuiState.cpsrcn_state == G_CP_KYBDN)
			{

				WM_ShowWindow(WM_GetDialogItem(pMsg->hWin, ID_TEXT_SETPTS));
				WM_ShowWindow(WM_GetDialogItem(pMsg->hWin, ID_HDR_SETPTS));
				WM_ShowWindow(WM_GetDialogItem(pMsg->hWin, ID_EDIT_MIN));
				WM_ShowWindow(WM_GetDialogItem(pMsg->hWin, ID_EDIT_DMD));
				WM_ShowWindow(WM_GetDialogItem(pMsg->hWin, ID_EDIT_MAX));

				WM_ShowWindow(WM_GetDialogItem(pMsg->hWin, ID_TEXT_LVW));
				WM_ShowWindow(WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0));
				WM_ShowWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_SAVE));

				WM_HideWindow(WM_GetDialogItem(pMsg->hWin, ID_TEXT_DSC));
				
				hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);

				LISTVIEW_GetItemRect(hItem, DESCCOL, EditLVRow, &hLvRect);
				Xpos = WM_GetWindowOrgX(hItem);
				Ypos = WM_GetWindowOrgY(hItem);

				hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_DSC);
				//WM_MoveTo(hItem, 157, 55);
				EDIT_SetFont(hItem, GUI_FONT_20_1);
				//LISTVIEW_SetFont(hItem, GUI_FONT_20_1);
				WM_SetWindowPos(hItem, Xpos+hLvRect.x0, Ypos+hLvRect.y0, (hLvRect.x1-hLvRect.x0), (hLvRect.y1-hLvRect.y0));
			
			}
		}
}


			//if((GuiState.SysDataPSU[GuiState.psu_viewing].Status & 0x0004)== 0x0000) //Profile not received

//void UpdateStatus(WM_HWIN hItem)
void UpdateStatus(WM_MESSAGE *pMsg)
		{
			WM_HWIN hItem;
			hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);

				int8_t row,col;
				for(row=0;row<4;row++)
				{
					//Update latest reading
					sprintf(dbgstr,"%0.2f",(float)GuiState.SysDataPSU[GuiState.psu_viewing].RefCells[row]/1000);
					LISTVIEW_SetItemText(hItem, 3, row,dbgstr);
					
					//Check if in range and determine PASS/FAIL
					if((GuiState.SysDataPSU[GuiState.psu_viewing].RefCells[row] >= 
						  GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.refelec_min_th)&&
					   (GuiState.SysDataPSU[GuiState.psu_viewing].RefCells[row] <=
							GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.refelec_max_th))
					{//PASS
						for(col=0;col<5;col++)
						{
							if(col==4)
							{
								if((GuiState.SysDataPSU[GuiState.psu_viewing].Status & 0x0004)== 0x0000) //Profile not received
								{
									LISTVIEW_SetItemText(hItem,4,row,"TBD");
									LISTVIEW_SetItemBkColor(hItem,4,row,LISTVIEW_CI_UNSEL,GUI_YELLOW);
								}
								else
								{
									LISTVIEW_SetItemText(hItem,4,row,"PASS");
									LISTVIEW_SetItemBkColor(hItem,4,row,LISTVIEW_CI_UNSEL,GUI_GREEN);
								}
								LISTVIEW_SetItemTextColor(hItem,4,row,LISTVIEW_CI_UNSEL,GUI_BLACK);
							}
							else
							{
								LISTVIEW_SetItemBkColor(hItem,col,row,LISTVIEW_CI_UNSEL,GUI_WHITE);
								LISTVIEW_SetItemTextColor(hItem,col,row,LISTVIEW_CI_UNSEL,GUI_BLACK);
							}
						}
					}
					else
					{//FAIL
						for(col=0;col<5;col++)
						{
							if(col==4)
							{
								LISTVIEW_SetItemText(hItem,4,row,"FAIL");
								LISTVIEW_SetItemBkColor(hItem,4,row,LISTVIEW_CI_UNSEL,GUI_RED);
								LISTVIEW_SetItemTextColor(hItem,4,row,LISTVIEW_CI_UNSEL,GUI_WHITE);
							}
							else
							{
								LISTVIEW_SetItemBkColor(hItem,col,row,LISTVIEW_CI_UNSEL,GUI_WHITE);
								LISTVIEW_SetItemTextColor(hItem,col,row,LISTVIEW_CI_UNSEL,GUI_RED);
							}
						}
					}					
				}
		}

/*********************************************************************
*
*       Screen creation
*
**********************************************************************
*/
/*********************************************************************
*
*       CreateFramewin
*/
WM_HWIN Createiccp_cp(void);
WM_HWIN Createiccp_cp(void) {
  WM_HWIN hWin;

  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  GuiState.gui_scn_state = G_CP;
	hWin_cpDLG = hWin;
	return hWin;
}

WM_HWIN GetWinHandleiccp_cpDLG(void);
WM_HWIN GetWinHandleiccp_cpDLG(void)
{
	return hWin_cpDLG;
}

/*************************** End of file ****************************/
