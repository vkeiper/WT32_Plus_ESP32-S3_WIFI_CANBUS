/**
  ******************************************************************************
  * @file    aplha_kybd.c
  * @author  Vince Keiper AstrodyneTDI
  * @version V1.0.0
  * @date    24-June-2015
  * @brief   Virtual Alpha Numeric Keyboard, made by stacking buttons together with the button
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

//
// Array of keys
//                       +10               +11           +12            Not Used             
static int _aKey[] = {GUI_KEY_DELETE, GUI_KEY_ENTER, GUI_KEY_BACKSPACE, GUI_KEY_SPACE};

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
#define ID_BUTTON_14 (GUI_ID_USER + 0x0F)

#define ID_BTN_A_0 (GUI_ID_USER + 0x10)
#define ID_BTN_A_1 (GUI_ID_USER + 0x11)
#define ID_BTN_A_2 (GUI_ID_USER + 0x12)
#define ID_BTN_A_3 (GUI_ID_USER + 0x13)
#define ID_BTN_A_4 (GUI_ID_USER + 0x14)
#define ID_BTN_A_5 (GUI_ID_USER + 0x15)
#define ID_BTN_A_6 (GUI_ID_USER + 0x16)
#define ID_BTN_A_7 (GUI_ID_USER + 0x17)
#define ID_BTN_A_8 (GUI_ID_USER + 0x18)
#define ID_BTN_A_9 (GUI_ID_USER + 0x19)
#define ID_BTN_A_10 (GUI_ID_USER + 0x1A)
#define ID_BTN_A_11 (GUI_ID_USER + 0x1B)
#define ID_BTN_A_12 (GUI_ID_USER + 0x1C)
#define ID_BTN_A_13 (GUI_ID_USER + 0x1D)
#define ID_BTN_A_14 (GUI_ID_USER + 0x1E)
#define ID_BTN_A_15 (GUI_ID_USER + 0x1F)
#define ID_BTN_A_16 (GUI_ID_USER + 0x20)
#define ID_BTN_A_17 (GUI_ID_USER + 0x21)
#define ID_BTN_A_18 (GUI_ID_USER + 0x22)
#define ID_BTN_A_19 (GUI_ID_USER + 0x23)
#define ID_BTN_A_20 (GUI_ID_USER + 0x24)
#define ID_BTN_A_21 (GUI_ID_USER + 0x25)
#define ID_BTN_A_22 (GUI_ID_USER + 0x26)
#define ID_BTN_A_23 (GUI_ID_USER + 0x27)
#define ID_BTN_A_24 (GUI_ID_USER + 0x28)
#define ID_BTN_A_25 (GUI_ID_USER + 0x29)
#define ID_BTN_A_26 (GUI_ID_USER + 0x2A)
#define ID_BTN_A_27 (GUI_ID_USER + 0x2B)//ENT
#define ID_BTN_A_28 (GUI_ID_USER + 0x2C)//BSP
#define ID_BTN_A_29 (GUI_ID_USER + 0x2D)//123


#define BTNX 60
#define BTNY 60
#define LNGBTNX 120
#define LNGBTNY 60
#define ABTNX 48
#define ABTNY 48
#define ALNGBTNX 90
#define ALNGBTNY 48

/*********************************************************************
*
*       Private Vars
*
**********************************************************************
*/
 WM_HWIN hDlg_kbd;

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
	WM_HWIN Createlcdkeypad(uint8_t alpha_ena);
WM_HWIN CreateAlphakeypad(void);



typedef enum {K_NUMONLY,K_APLHAONLY,K_NUMandAPLHA} kybdtype_t;
typedef enum {K_KNUP,K_KNDN,K_KAUP,K_ADN} kybdstate_t;
typedef struct {
	kybdtype_t     nkybdtype;
	kybdstate_t    nkybdstate;
} keyboardstatus_t;

keyboardstatus_t kybdsta;


static const GUI_WIDGET_CREATE_INFO _aDialogAlphaPad[] = {
//                                                           |   Position   |    | dimensions |              
//  Function                 Text        Id                  Px            Py    Dx       Dy
//
	{ WINDOW_CreateIndirect,   0,          0,                  0, 270-(ABTNY*3),   480,   ABTNY*3, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "A",        ID_BTN_A_0,    ABTNX*0,        ABTNY*0,  ABTNX,     ABTNY, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "B", 	     ID_BTN_A_1,    ABTNX*1,        ABTNY*0,  ABTNX,     ABTNY, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "C",        ID_BTN_A_2,    ABTNX*2,        ABTNY*0,  ABTNX,     ABTNY, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "D",        ID_BTN_A_3,    ABTNX*3,        ABTNY*0,  ABTNX,     ABTNY, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "E",        ID_BTN_A_4,    ABTNX*4,        ABTNY*0,  ABTNX,     ABTNY, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "F",        ID_BTN_A_5,    ABTNX*5,        ABTNY*0,  ABTNX,     ABTNY, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "G",        ID_BTN_A_6,    ABTNX*6,        ABTNY*0,  ABTNX,     ABTNY, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "H",        ID_BTN_A_7,    ABTNX*7,        ABTNY*0,  ABTNX,     ABTNY, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "I",        ID_BTN_A_8,    ABTNX*8,        ABTNY*0,  ABTNX,     ABTNY, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "J",        ID_BTN_A_9,    ABTNX*9,        ABTNY*0,  ABTNX,     ABTNY, 0, 0x0, 0 },
  //line 2
	{ BUTTON_CreateIndirect,   "K",        ID_BTN_A_10,   ABTNX*0,        ABTNY*1,   ABTNX,     ABTNY, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "L",        ID_BTN_A_11,   ABTNX*1,        ABTNY*1,   ABTNX,     ABTNY, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "M",    		 ID_BTN_A_12,   ABTNX*2,        ABTNY*1,   ABTNX,     ABTNY, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "N",        ID_BTN_A_13,   ABTNX*3,        ABTNY*1,   ABTNX,     ABTNY, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect,   "O",        ID_BTN_A_14,    ABTNX*4,       ABTNY*1,  ABTNX,    ABTNY, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "P",        ID_BTN_A_15,    ABTNX*5,       ABTNY*1,  ABTNX,    ABTNY, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "Q",        ID_BTN_A_16,    ABTNX*6,       ABTNY*1,  ABTNX,    ABTNY, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "R",        ID_BTN_A_17,    ABTNX*7,       ABTNY*1,  ABTNX,    ABTNY, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "S",        ID_BTN_A_18,    ABTNX*8,       ABTNY*1,  ABTNX,    ABTNY, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "T",        ID_BTN_A_19,    ABTNX*9,       ABTNY*1,  ABTNX,    ABTNY, 0, 0x0, 0 },
  //line 3
	{ BUTTON_CreateIndirect,   "U",        ID_BTN_A_20,   ABTNX*0,        ABTNY*2,   ABTNX,     ABTNY, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "V",        ID_BTN_A_21,   ABTNX*1,        ABTNY*2,   ABTNX,     ABTNY, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "W",    		 ID_BTN_A_22,   ABTNX*2,        ABTNY*2,   ABTNX,  ABTNY, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "X",        ID_BTN_A_23,   ABTNX*3,        ABTNY*2,   ABTNX,  ABTNY, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect,   "Y",        ID_BTN_A_24,    ABTNX*4,        ABTNY*2,  ABTNX,    ABTNY, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "Z",        ID_BTN_A_25,    ABTNX*5,        ABTNY*2,  ABTNX,    ABTNY, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,   ".",        ID_BTN_A_26,    ABTNX*6,        ABTNY*2,  ABTNX,    ABTNY, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "SPC",      ID_BTN_A_27,    ABTNX*7,        ABTNY*2,  ABTNX,    ABTNY, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "BSP",      ID_BTN_A_28,    ABTNX*8,        ABTNY*2,  ABTNX,    ABTNY, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "123",      ID_BTN_A_29,    ABTNX*9,        ABTNY*2,  ABTNX,    ABTNY, 0, 0x0, 0 },
};

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
  { BUTTON_CreateIndirect,   "ENT",      ID_BUTTON_12,   BTNX*4,        BTNY*1,  LNGBTNX,  LNGBTNY, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "BSP",      ID_BUTTON_13,   BTNX*6,        BTNY*1,  BTNX,  BTNY, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect,   "ABC",      ID_BUTTON_14,   BTNX*7,        BTNY*1,  BTNX,  BTNY, 0, 0x0, 0 },
};

/*********************************************************************
*
*       _cbDialogAlphaPad
*
* Function description
*   Callback function of the alphapad.
*/
static void _cbDialogAlphaPad(WM_MESSAGE * pMsg) {
  GUI_RECT r;
  unsigned i; 
  int      NCode;
  unsigned Id;
  int      Pressed;
  WM_HWIN  hDlg;
  WM_HWIN  hItem;

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
    // Initialization of 'alpha pad framewin'
    //
    hItem = pMsg->hWin;
		WINDOW_SetBkColor(hItem,GUI_WHITE);
		
		for (i = GUI_COUNTOF(_aDialogNumPad); i <  GUI_COUNTOF(_aDialogNumPad) + GUI_COUNTOF(_aDialogAlphaPad); i++) 
		{
				hItem = WM_GetDialogItem(hDlg, GUI_ID_USER + i);
				BUTTON_SetFocussable(hItem, 0);                       /* Set all buttons non focussable */
				
				if(i+ GUI_ID_USER <= (ID_BTN_A_26)){
						BUTTON_SetFont(hItem,GUI_FONT_32B_1);	
				}
				else{		
					BUTTON_SetFont(hItem,GUI_FONT_16B_1);	
				}
				
				BUTTON_SetSkin(hItem,BUTTON_SKIN_FLEX);
	  }
    WM_GetDialogItem(hDlg, ID_BTN_A_29);
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);      /* Id of widget */
    NCode = pMsg->Data.v;                 /* Notification code */
    switch (NCode) {
    case WM_NOTIFICATION_CLICKED:
      Pressed = 1;
    case WM_NOTIFICATION_RELEASED:
			if ((Id >= GUI_ID_USER + 0x10) && (Id <= GUI_ID_USER + 0x2D ))// if its a alpha btn that was pressed
			{
					int Key;
					// if it is a letter period copy the button text to the character buffer, over write whats already in there
					if (Id < GUI_ID_USER + 0x2B) 
					{
						char acBuffer[10];
						BUTTON_GetText(pMsg->hWinSrc, acBuffer, sizeof(acBuffer)); // Get the text of the button
						Key = acBuffer[0];
					} 
					else // it is a key enter, or backspace 
					{
							//ID_BTN_A_27//ENT- ID_BTN_A_28//BSP - ID_BTN_A_29 //123
							if(Id == ID_BTN_A_27 || Id == ID_BTN_A_28)
							{
									if(Id == ID_BTN_A_27)
									{	
											Key = _aKey[3];// send GUI_KEY_SPACE to key 
									}else if( Id == ID_BTN_A_28){
											Key = _aKey[2];// send GUI_KEY_BACKSPACE
									}
							}
							else  //its the 123 key close alpha kybd and open num kybd
							{
									GUI_EndDialog(hDlg, 1);// end the alpha dlg and open the numdlg
									Createlcdkeypad(0);// open the num dlg, alpha disabled
								 break;
							}
					}
					GUI_SendKeyMsg(Key, Pressed);  // vk, here we pass the value of "key" as a message to which ever the focussed window and edit box
					GUI_Delay(10);
					
      }//end if Id is a keybd key
      break;
    }
  default:
    WM_DefaultProc(pMsg);
  }
	
}


/*********************************************************************
*
*       CreateAlphakeypad
*/
WM_HWIN CreateAlphakeypad(void) 
{
  WM_HWIN hAlphaPad;

  hAlphaPad = GUI_CreateDialogBox(_aDialogAlphaPad, 
                 GUI_COUNTOF(_aDialogAlphaPad), 
                 _cbDialogAlphaPad, WM_HBKWIN, 0, 0); /* Create the Alphapad dialog */
	
	WM_SetStayOnTop(hAlphaPad, 1);
	
	hDlg_kbd = hAlphaPad;
	return hAlphaPad;
}



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
		
		GuiState.ScreenTimeout=0;
	
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
						if(Id != ID_BUTTON_14)
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
										{			
												GuiState.cpsrcn_state = G_CP_KYBDN;
										}
								}
							}
						else{  // if the "ABC" was pressed
								if(kybdsta.nkybdtype == K_NUMandAPLHA || kybdsta.nkybdtype == K_APLHAONLY)
								{
										GUI_EndDialog(hDlg, 1);// kill the num dlg
										CreateAlphakeypad();// open the alpha pad
										break;
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
WM_HWIN Createlcdkeypad(uint8_t alpha_ena) 
{
  WM_HWIN hNumPad;
	
	if(alpha_ena ==0)
	{
			kybdsta.nkybdtype = K_NUMONLY;
	}
	else if(alpha_ena ==1)
	{
			kybdsta.nkybdtype = K_APLHAONLY;
	}
	else if(alpha_ena ==2)
	{
			kybdsta.nkybdtype = K_NUMandAPLHA;
	}
	
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
	
	hDlg_kbd = hNumPad;
	return hNumPad;
}

WM_HWIN GetWinHandleiccp_kybd(void);
WM_HWIN GetWinHandleiccp_kybd(void)
{
	return hDlg_kbd;
}




