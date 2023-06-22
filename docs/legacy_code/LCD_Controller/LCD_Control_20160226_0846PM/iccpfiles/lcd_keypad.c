/**
  ******************************************************************************
  * @file    lcd_keyboard.c
  * @author  Vince Keiper AstrodyneTDI
  * @version V1.0.0
  * @date    24-June-2015
  * @brief   Virtual Keyboard, made by stacking buttons together with the button
	*					 text field set to its respective character. When the button is pressed
	*          the ON_RELEASE handler will read the text and saved as a variable 'key' 
	*          which then will be passed back to the GUI screen that called us by 
  *          invoking a GUI API command "GUI_SendKeyMsg(Key, Pressed)"
  *					 pressing Enter will close the keyboard and set the GUI state to 
  *					 to let caller know the value was changed.	
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 AstrodyneTDI</center></h2>
  *
  *
  ******************************************************************************  
  */ 


#include "DIALOG.h"
#include "iccpfuncs.h"
#include "iccp_canfuncs.h"

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

//
// Array of keys
//                       +10               +11           +12            Not Used             
static int _aKey[] = {GUI_KEY_DELETE, GUI_KEY_ENTER, GUI_KEY_BACKSPACE, GUI_KEY_TAB};

//
// Dialog resource of numpad
//
#define ID_FRAMEWIN_0 (GUI_ID_USER + 0x00)
#define ID_BUTTON_0 (GUI_ID_USER + 0x01)
#define ID_BUTTON_1 (GUI_ID_USER + 0x02)
#define ID_BUTTON_2 (GUI_ID_USER + 0x03)
#define ID_BUTTON_3 (GUI_ID_USER + 0x04)
#define ID_BUTTON_4 (GUI_ID_USER + 0x05)
#define ID_BUTTON_5 (GUI_ID_USER + 0x06)
#define ID_BUTTON_6 (GUI_ID_USER + 0x07)
#define ID_BUTTON_7 (GUI_ID_USER + 0x08)
#define ID_BUTTON_8 (GUI_ID_USER + 0x09)
#define ID_BUTTON_9 (GUI_ID_USER + 0x0A)
#define ID_BUTTON_10 (GUI_ID_USER + 0x0B)
#define ID_BUTTON_11 (GUI_ID_USER + 0x0C)
#define ID_BUTTON_12 (GUI_ID_USER + 0x0D)
#define ID_BUTTON_13 (GUI_ID_USER + 0x0E)

#define BTNX 60
#define BTNY 60
#define LNGBTNX 120
#define LNGBTNY 60
static const GUI_WIDGET_CREATE_INFO _aDialogNumPad[] = {
//                                                           |   Position   |    | dimensions |              
//  Function                 Text        Id                  Px            Py    Dx       Dy
//
	{ WINDOW_CreateIndirect,   0,          0,                  0,            150,   480,   BTNY*2, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "0",        ID_BUTTON_0,    BTNX*0,        BTNY*0,  BTNX,     BTNY, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "1", 	     ID_BUTTON_1,    BTNX*1,        BTNY*0,  BTNX,     BTNY, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "2",        ID_BUTTON_2,    BTNX*2,        BTNY*0,  BTNX,     BTNY, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "3",        ID_BUTTON_3,    BTNX*3,        BTNY*0,  BTNX,     BTNY, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "4",        ID_BUTTON_4,    BTNX*4,        BTNY*0,  BTNX,     BTNY, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "5",        ID_BUTTON_5,    BTNX*5,        BTNY*0,  BTNX,     BTNY, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "6",        ID_BUTTON_6,    BTNX*6,        BTNY*0,  BTNX,     BTNY, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "7",        ID_BUTTON_7,    BTNX*7,        BTNY*0,  BTNX,     BTNY, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "8",        ID_BUTTON_8,    BTNX*0,        BTNY*1,  BTNX,     BTNY, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "9",        ID_BUTTON_9,    BTNX*1,        BTNY*1,  BTNX,     BTNY, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,   ".",        ID_BUTTON_10,   BTNX*2,        BTNY*1,  BTNX,     BTNY, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "-",        ID_BUTTON_11,   BTNX*3,        BTNY*1,  BTNX,     BTNY, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "ENTER",    ID_BUTTON_12,   BTNX*4,        BTNY*1,  LNGBTNX,  LNGBTNY, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "BKSP",     ID_BUTTON_13,   BTNX*5+BTNX,   BTNY*1,  LNGBTNX,  LNGBTNY, 0, 0x0, 0 },
};


/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _cbDialogNumPad
*
* Function description
*   Callback function of the numpad.
*/
static void _cbDialogNumPad(WM_MESSAGE * pMsg) {
  GUI_RECT r;
  unsigned i; 
  int      NCode;
  unsigned Id;
  int      Pressed;
  WM_HWIN  hDlg;
  WM_HWIN  hItem;
  
  //BUTTON_SKINFLEX_PROPS BtnProps;

  Pressed = 0;
  hDlg = pMsg->hWin;
  switch (pMsg->MsgId) {
  case WM_PAINT:
    WM_GetClientRect(&r);
    GUI_SetColor(0x000000);
    GUI_DrawRect(r.x0, r.y0, r.x1, r.y1);          /* Draw rectangle around it */
    /* Draw the bright sides */
    GUI_SetColor(0xffffff);
    GUI_DrawHLine(r.y0 + 1, r.x0 + 1, r.x1 - 2);   /* Draw top line */
    GUI_DrawVLine(r.x0 + 1, r.y0 + 1, r.y1 - 2);
    /* Draw the dark sides */
    GUI_SetColor(0x555555);
    GUI_DrawHLine(r.y1-1, r.x0 + 1, r.x1 - 1);
    GUI_DrawVLine(r.x1-1, r.y0 + 1, r.y1 - 2);
    break;
  case WM_INIT_DIALOG:
  
	
	/************************************************/
  
    //
    // Initialization of 'lcd_keypad framewin'
    //
    hItem = pMsg->hWin;
		WINDOW_SetBkColor(hItem,GUI_WHITE);
		
		for (i = 0; i < GUI_COUNTOF(_aDialogNumPad); i++) 
		{
      hItem = WM_GetDialogItem(hDlg, GUI_ID_USER + i);
      BUTTON_SetFocussable(hItem, 0);                       /* Set all buttons non focussable */
			if(i <=12){BUTTON_SetFont(hItem,GUI_FONT_32B_1);}
			else{BUTTON_SetFont(hItem,GUI_FONT_16B_1);}
      BUTTON_SetSkin(hItem,BUTTON_SKIN_FLEX);
	  }
    WM_GetDialogItem(hDlg, GUI_ID_USER + 13);
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);      /* Id of widget */
    NCode = pMsg->Data.v;                 /* Notification code */
    switch (NCode) {
    case WM_NOTIFICATION_CLICKED:
      Pressed = 1;
    case WM_NOTIFICATION_RELEASED:
      if ((Id >= GUI_ID_USER) && (Id <= (GUI_ID_USER + GUI_COUNTOF(_aDialogNumPad) - 1))) {
        int Key;
				// if it is a number or decimal point
				if (Id < GUI_ID_USER + 13) {
          char acBuffer[10];
          BUTTON_GetText(pMsg->hWinSrc, acBuffer, sizeof(acBuffer)); // Get the text of the button
          Key = acBuffer[0];
        } 
				else // it is a key enter, or backspace 
				{
          Key = _aKey[Id - GUI_ID_USER - 12];                        // Get the text from the array 
					// vk, we will use the ENTER button press to close the keyboard
					if(Key == GUI_KEY_ENTER)
					{
							GUI_EndDialog(hDlg, 1);
							
							// vk, we need t set the gui state to the screen that called us
							if(GuiState.gui_scn_state == G_CV)
							{
									GuiState.cvsrcn_state = G_CV_KYBDN;
							}
							else if(GuiState.gui_scn_state == G_CI)
							{
									GuiState.cisrcn_state = G_CI_KYBDN;
							}
							else if(GuiState.gui_scn_state == G_CP)
							{								GuiState.cpsrcn_state = G_CP_KYBDN;
							}
					}

        }
        GUI_SendKeyMsg(Key, Pressed);  // vk, here we pass the value of "key" as a message to which ever the focussed window and edit box
				GUI_Delay(10);
				
      }
      break;
    }
  default:
    WM_DefaultProc(pMsg);
  }
	
}


/*********************************************************************
*
*       Createlcdkeypad
*/
WM_HWIN Createlcdkeypad(void);
WM_HWIN Createlcdkeypad(void) {
  WM_HWIN hNumPad;

  hNumPad = GUI_CreateDialogBox(_aDialogNumPad, 
                                GUI_COUNTOF(_aDialogNumPad), 
                                _cbDialogNumPad, WM_HBKWIN, 0, 0); /* Create the numpad dialog */
	
	WM_SetStayOnTop(hNumPad, 1);
	
	if(GuiState.gui_scn_state == G_CV)
	{
			GuiState.cvsrcn_state = G_CV_KYBUP;
	}
	else if(GuiState.gui_scn_state == G_CI)
	{
			GuiState.cisrcn_state = G_CI_KYBUP;
	}
	else if(GuiState.gui_scn_state == G_CP)
	{
			GuiState.cpsrcn_state = G_CP_KYBUP;
	}
	
	return hNumPad;
}



