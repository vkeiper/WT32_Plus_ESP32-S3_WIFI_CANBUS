/*********************************************************************
  * @author  Vince Keiper AstrodyneTDI
  * @version V1.0.0
  * @date    24-June-2015
  * @brief   ICCP CI dialog  
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 AstrodyneTDI</center></h2>
  *
  *
  ******************************************************************************  
  */ 

/**
*
* Includes
*
**/
#include "DIALOG.h"
#include "stdio.h"
#include "k_module.h"
#include "globals.h"
#include "stdlib.h"
#include "iccpfuncs.h"
#include "iccp_canfuncs.h"
#include "string.h"

/**
*
*  External Variables
*
**/
extern K_ModuleItem_Typedef  iccp_main;
extern WM_HWIN iccpmain_hdl;
extern WM_HWIN Createiccp_main(void);
extern WM_HWIN Createlcdkeypad(uint8_t alpha_ena);

WM_HWIN hWin_ciDLG;


/**
*
*  Local Variables
*/
uint16_t orig_cival;


/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_FRAMEWIN_0 (GUI_ID_USER + 0x00)
#define ID_BUTTON_0 (GUI_ID_USER + 0x01)
#define ID_BUTTON_1 (GUI_ID_USER + 0x02)
#define ID_TEXT_0 (GUI_ID_USER + 0x03)
#define ID_TEXT_1 (GUI_ID_USER + 0x04)
#define ID_BTN_CANCEL (GUI_ID_USER + 0x05)
#define ID_EDIT_0 (GUI_ID_USER + 0x06)

#define BTNX 66
#define BTNY 66
#define EDX  140
#define EDY  50
#define MGN  30
#define MGNX 24  //X axis Margin
#define SCNX 470  
#define SCNY 270  

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { FRAMEWIN_CreateIndirect, "iccp_ci", ID_FRAMEWIN_0,             0,             0,SCNX+10,  SCNY,  0, 0x64, 0 },
  { BUTTON_CreateIndirect, "Home_Btn", ID_BUTTON_0,  SCNX-(BTNX+MGNX),(EDY+MGN)-BTNY,  BTNX,  BTNY,  0,  0x0, 0 },
  { BUTTON_CreateIndirect, "Set", ID_BUTTON_1,           (MGNX*2)+EDX,(EDY+MGN)-BTNY,  BTNX,  BTNY,  0,  0x0, 0 },
  { TEXT_CreateIndirect, "Current Set", ID_TEXT_1,               MGNX,            10,   125,    20,  0,  0x0, 0 },
  { EDIT_CreateIndirect,     0,        ID_EDIT_0,                MGNX,           MGN,   EDX,   EDY,  0,    5},
  { BUTTON_CreateIndirect,"Discard_Btn",ID_BTN_CANCEL,(MGNX*3)+EDX+BTNX,(EDY+MGN)-BTNY,  BTNX,  BTNY,  0,  0x0, 0 },
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
static void _cbDialog(WM_MESSAGE * pMsg) 
{
  WM_HWIN hItem;
  int     NCode;
  int     Id;
  char titlestr[44];
	char dbgstr[8];
	static int editcnt=0;//setcnt=0
	
	if(GuiState.cisrcn_state == G_CI_KYBDN)
	{
		WM_ShowWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1));
		WM_ShowWindow(WM_GetDialogItem(pMsg->hWin, ID_BTN_CANCEL));
	}


  switch (pMsg->MsgId)
	{
		case WM_INIT_DIALOG:
		{
			GuiState.cisrcn_state = G_CI_INIT;
			
			//printf("Init CI Dialog");
			//               
			// Initialization of 'iccp_ci'
			//               
			sprintf(titlestr,"Constant Current Mode Screen for NODE %i",GuiState.psu_viewing);
			hItem = pMsg->hWin;
			FRAMEWIN_SetText(hItem, titlestr );
			FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
			FRAMEWIN_SetClientColor(hItem,GUI_WHITE);  
			FRAMEWIN_SetSkin(hItem,FRAMEWIN_SKIN_FLEX);
			//
			// Initialization of 'Home_Btn'
			//
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
			BUTTON_SetText(hItem, "Home");
			BUTTON_SetFont(hItem, GUI_FONT_13B_ASCII);
			BUTTON_SetSkin(hItem,BUTTON_SKIN_FLEX);
			//
			// Initialization of 'Discard_Btn'
			//
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BTN_CANCEL);
			BUTTON_SetText(hItem, "Cancel");
			BUTTON_SetFont(hItem, GUI_FONT_13B_ASCII);
			BUTTON_SetSkin(hItem,BUTTON_SKIN_FLEX);
			//
			//
			// Initialization of 'Save'
			//
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
			BUTTON_SetFont(hItem, GUI_FONT_16B_ASCII);
			BUTTON_SetSkin(hItem,BUTTON_SKIN_FLEX);
			BUTTON_SetText(hItem, "Set");
			//
			//
			// Initialization of 'Amps_txt'
			//
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
			TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
			TEXT_SetFont(hItem, GUI_FONT_D24X32);
			//                                    
			//                                                                                    
			// Initialization of 'Amps Label'                                                          
			//                                                                                    
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
			TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
			TEXT_SetFont(hItem, GUI_FONT_20_1);
			TEXT_SetTextColor(hItem, 0x00FF0000);

//
			// Initialization of 'Slider'
			//
//			hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_0);
//			SLIDER_SetWidth(hItem, 10);
//			SLIDER_SetFocusColor(hItem, GUI_RED);
//			SLIDER_SetValue(hItem,GuiState.SysDataPSU[GuiState.psu_viewing].rec_iset); 
//			SLIDER_SetSkin(hItem,SLIDER_SKIN_FLEX);
//			SLIDER_SetRange(hItem, 50, 650);
//			SLIDER_SetNumTicks(hItem, 60);
//			
			// Set Edit Box
			hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_0);  /* Get the handle of the edit widget */
			EDIT_SetFont(hItem, GUI_FONT_D24X32);
			EDIT_SetTextColor(hItem,EDIT_CI_ENABLED, GUI_BLUE);//0x000303E7);
			EDIT_SetTextAlign(hItem,GUI_TA_CENTER | GUI_TA_VCENTER);
			sprintf(dbgstr,"%2.1f",(float)GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.rec_iset/10);
			EDIT_SetText(hItem, dbgstr);                   /* Set text */
			
			orig_cival = GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.rec_iset;
			
			WM_HideWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1));
			WM_HideWindow(WM_GetDialogItem(pMsg->hWin, ID_BTN_CANCEL));
			
			WM_InvalidateWindow(pMsg->hWin);
			break;
		}
		case WM_NOTIFY_PARENT:
		{
				Id    = WM_GetId(pMsg->hWinSrc);
				NCode = pMsg->Data.v;
				//printf("ID:%i  NCode:%i \r\n",Id,NCode);
			
				GuiState.ScreenTimeout=0;
							
				switch(Id) 
				{
						case ID_BUTTON_0: // Notifications sent by 'Home_Btn'
						{
							switch(NCode) 
							{
									case WM_NOTIFICATION_CLICKED:
											 if(GuiState.cisrcn_state == G_CI_INIT)
											 {
														WM_InvalidateWindow(iccpmain_hdl);
														WM_Paint(iccpmain_hdl);
														GUI_EndDialog(pMsg->hWin,NULL);
														GuiState.gui_scn_state = G_RTSTA;
														GuiState.cisrcn_state = G_CI_INIT;
											 }
											break;
							}//end switch NCode
							break;
						}
						case ID_BTN_CANCEL: // Notifications sent by 'Discard_Btn'
						{
							switch(NCode) 
							{
									case WM_NOTIFICATION_CLICKED:
											 if(GuiState.cisrcn_state != G_CI_INIT &&
												 GuiState.cisrcn_state != G_CI_KYBUP)
											 {
														// Set Edit Box back to Value we entered this screen with
														hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_0);  /* Get the handle of the edit widget */
														EDIT_SetTextColor(hItem,EDIT_CI_ENABLED, GUI_BLUE);//0x000303E7);
														sprintf(dbgstr,"%2.1f",(float)orig_cival/10);
														EDIT_SetText(hItem, dbgstr); 
														GuiState.cisrcn_state = G_CI_INIT;
														WM_HideWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1));
														WM_HideWindow(WM_GetDialogItem(pMsg->hWin, ID_BTN_CANCEL));
														WM_ShowWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0));
											 }
											break;
							}//end switch NCode
							break;
						}
						case ID_BUTTON_1: // Notifications sent by 'Set' or 'Save' btn
						{
							switch(NCode) 
							{
									case WM_NOTIFICATION_CLICKED:
										hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
										
										editcnt++;
										if(editcnt>0 && (GuiState.cisrcn_state == G_CI_INIT  || 
												GuiState.cisrcn_state == G_CI))
										{
												editcnt=0;
												BUTTON_SetText(hItem, "Send");
												hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_0);
												EDIT_SetTextColor(hItem,1,GUI_RED);
												EDIT_EnableBlink(hItem, 500, 1);
												EDIT_SetText(hItem,"");	
												EDIT_SetCursorAtChar(hItem, 0);
												
												Createlcdkeypad(0);//num only, alpha disabled
										}
									

										if(GuiState.cisrcn_state == G_CI_KYBDN)
										{
													GuiState.cisrcn_state = G_CI_SAVE;
													
													BUTTON_SetText(hItem, "Set");
													hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_0);
													EDIT_GetText(hItem,dbgstr,sizeof(dbgstr));
													//convert str contents of edit box to int to be saved in volts var
													GuiState.SysDataPSU[GuiState.psu_viewing].rec_iset   = (unsigned int)(atof(dbgstr)*10);
													GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.rec_iset = (unsigned int)(atof(dbgstr)*10);
													GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.rec_vset = 500;//Set to 50.V when in CI mode
													GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.psu_mode   = P_CI;
													
													EDIT_SetTextColor(hItem,1,GUI_BLUE);
													EDIT_EnableBlink(hItem, 500, 0);
													//printf("CItmp:%2.1f  CI:%2.1f \r\n",(float)GuiState.SysDataPSU[GuiState.psu_viewing].rec_iset/10,(float)cimode_iset/10);

													GuiState.cisrcn_state = G_CI_INIT;
													WM_HideWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1));
											    WM_HideWindow(WM_GetDialogItem(pMsg->hWin, ID_BTN_CANCEL));
													WM_ShowWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0));
													if(!GuiState.demo){while(!(BITCHK(GuiState.SysDataPSU[GuiState.psu_viewing].Status,3)));}//Wait for status update to complete
													CANWrite(GuiState.psu_viewing,0x00,CMD_CAN_204,(uint8_t*)dbgstr,0);// send profile to PSU RAM
										}
										break;
							}
							break;//break ID_BUTTON_1
						}

						case ID_EDIT_0: // Notifications sent by 'Edit Volts Display'
						{
									switch(NCode) 
									{
											case WM_NOTIFICATION_CLICKED:
														if(GuiState.cisrcn_state == G_CI_INIT)
														{
																editcnt++;
																if(editcnt>0 && GuiState.cisrcn_state != G_CI_KYBUP)
																{
																		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
												            BUTTON_SetText(hItem, "Send");
																		editcnt=0;
																		Createlcdkeypad(0);// num only, alpha disabled
																		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_0);
																		EDIT_SetTextColor(hItem,1,GUI_RED);
																		EDIT_EnableBlink(hItem, 500, 1);
																		EDIT_SetText(hItem,"");
																		EDIT_SetCursorAtChar(hItem, 0);
																	
																		WM_HideWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0));
																		WM_HideWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1));
																		WM_HideWindow(WM_GetDialogItem(pMsg->hWin, ID_BTN_CANCEL));
																}
														}
											break;
									}
							// USER END
							break;
						}
						//GuiState.ScreenTimeout=0;
				}//end switch Id
				default:
						WM_DefaultProc(pMsg);
				break;
  }// end NOTIFY PARENT MSG
	}//end switch MSGID
//	printf("CI MemFree:%lu MemUsed:%lu \r\n",GUI_ALLOC_GetNumFreeBytes(),GUI_ALLOC_GetNumUsedBytes());
}//end func	

/**
*******************************************************************
*
*       Public code
*
*******************************************************************
**/
/**
*       Createiccp_ci
**/
WM_HWIN Createiccp_ci(void);
WM_HWIN Createiccp_ci(void) {
  WM_HWIN hWin;

  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 26);
  GuiState.gui_scn_state = G_CI;
	hWin_ciDLG = hWin;
		return hWin;
}

WM_HWIN GetWinHandleiccp_ciDLG(void);
WM_HWIN GetWinHandleiccp_ciDLG(void)
{
	return hWin_ciDLG;
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/
