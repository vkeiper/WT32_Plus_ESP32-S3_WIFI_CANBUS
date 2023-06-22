/**
  ******************************************************************************
  * @file    k_menu.c
  * @author  Vince Keiper AstrodyneTDI
  * @version V1.0.0
  * @date    24-June-2015
  * @brief   This file provide kernal menu funcitons    
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 AstrodyneTDI</center></h2>
  *
  *
  ******************************************************************************  
  */ 
  
/* Includes ------------------------------------------------------------------*/
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "dialog.h"
#include "messagebox.h"
#include "k_rtc.h"
#include "k_log.h"
#include "k_storage.h"
#include "k_module.h"

#include "k_res.c"
#include "cpu_utils.h"
#include "globals.h"
#include "iccpmain.h"
#include "iccpfuncs.h"

/** @addtogroup CORE
  * @{
  */

/** @defgroup KERNEL_MENU
  * @brief Kernel menu routines
  * @{
  */

/* External Functions --------------------------------------------------------*/
extern WM_HWIN GetWinHandleiccp_gph(void);
extern WM_HWIN GetWinHandleiccp_kybd(void);
extern WM_HWIN GetWinHandleiccp_cvDLG(void);
extern WM_HWIN GetWinHandleiccp_ciDLG(void);
extern WM_HWIN GetWinHandleiccp_cpDLG(void);
extern WM_HWIN GetWinHandleIccpmain(void);


/* External variables --------------------------------------------------------*/
extern char dbgstrip[32];  //from app_ethernet.c
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#define ID_TIMER_TIME                  (GUI_ID_USER + 0x00)
#define ID_SPRITE_TIME                 (GUI_ID_USER + 0xA0)
#define ID_BUTTON_BKGND                (GUI_ID_USER + 0x01)
#define ID_ICONVIEW_MENU               (GUI_ID_USER + 0x02)
#define ID_MENU_LOG                    (GUI_ID_USER + 0x03)
#define ID_MENU_PMGR                   (GUI_ID_USER + 0x04)
#define ID_MENU_EXIT                   (GUI_ID_USER + 0x05)
#define ID_MENU_PERF                   (GUI_ID_USER + 0x06)


#define ID_FRAMEWIN_PERFORMANCE       (GUI_ID_USER + 0x10)
#define ID_GRAPH_CPU                  (GUI_ID_USER + 0x11)
#define ID_BUTTON_HIDE                (GUI_ID_USER + 0x12)

#define ID_FRAMEWIN_KERNELLOG         (GUI_ID_USER + 0x20)
#define ID_BUTTON_CANCEL_KERNELLOG    (GUI_ID_USER + 0x21)
#define ID_MULTIEDIT_KERNELLOG        (GUI_ID_USER + 0x22)

#define ID_FRAMEWIN_PROCESSMANAGER       (GUI_ID_USER + 0x30)
#define ID_BUTTON_CANCEL_PROCESSMANAGER  (GUI_ID_USER + 0x31)
#define ID_LISTVIEW_PROCESSMANAGER       (GUI_ID_USER + 0x32)

ICONVIEW_Handle hIcon = 0;
 
static GRAPH_DATA_Handle hData = 0;
static GRAPH_SCALE_Handle hScale = 0;
static WM_HWIN  hPerformance = 0;
static WM_HWIN  hProcess = 0;
static WM_HWIN  hLog = 0;
uint8_t SpriteDisabled = 0;
xTaskStatusType  ProcessStatus[16] ={{0}};
static SystemSettingsTypeDef    settings;
uint8_t strMonth[][12] = {"January",
                           "February",
                           "March",
                           "April",
                           "May",
                           "June",
                           "July",
                           "August",
                           "September",
                           "October",
                           "November",
                           "December"};

/* Array of menu items */
static MENU_ITEM _aMenuItems[] = 
{
  {"Kernel Log"         , ID_MENU_LOG,  0},
  {"Process Viewer"    , ID_MENU_PMGR, 0},
  {"CPU Usage", ID_MENU_PERF, 0},  
  {0                    , 0           ,  MENU_IF_SEPARATOR},
  {"Cancel"             , ID_MENU_EXIT, 0},
};

/* Sprite array */
static SPRITE _aSprite[] = 
{
  { 480, 150, -8, -2, GUI_COUNTOF(apbmbutterfly), apbmbutterfly},
};

/* Dialog Create */
static const GUI_WIDGET_CREATE_INFO _aPerformanceDialogCreate[] = {
  { FRAMEWIN_CreateIndirect, "CPU Usage", ID_FRAMEWIN_PERFORMANCE, 60, 5, 320, 180, 0, 0x0, 0 },
  { GRAPH_CreateIndirect, "", ID_GRAPH_CPU, 6, 9, 298, 130, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Hide", ID_BUTTON_HIDE, 116, 142, 80, 30, 0, 0x0, 0 },
};

static const GUI_WIDGET_CREATE_INFO _aKernelLogDialogCreate[] = {
  { FRAMEWIN_CreateIndirect, "Kernel Log", ID_FRAMEWIN_KERNELLOG, 60, 5, 320, 180, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Cancel", ID_BUTTON_CANCEL_KERNELLOG, 116, 142, 80, 30, 0, 0x0, 0 },
  { MULTIEDIT_CreateIndirect, "Multiedit", ID_MULTIEDIT_KERNELLOG, 6, 9, 298, 130, 0, 0x0, 0 },
};

static const GUI_WIDGET_CREATE_INFO _aProcessManagerDialogCreate[] = {
  { FRAMEWIN_CreateIndirect, "Process viewer", ID_FRAMEWIN_PROCESSMANAGER, 60, 5, 320, 180, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Cancel", ID_BUTTON_CANCEL_PROCESSMANAGER, 116, 142, 80, 30, 0, 0x0, 0 },
  { LISTVIEW_CreateIndirect, "Listview", ID_LISTVIEW_PROCESSMANAGER, 5, 5, 298, 130, 0, 0x0, 0 },
};

/**
  * @brief  Callback function of the CPU window
  * @param  pMsg: pointer to data structure of type WM_MESSAGE
  * @retval None
  */
static void _cbCpuWindow(WM_MESSAGE * pMsg) {
  
  static WM_HTIMER hTimerTime; 
  switch (pMsg->MsgId) 
  {
  case WM_CREATE:
    /* Create timer */
    hTimerTime = WM_CreateTimer(pMsg->hWin, 0, 400, 0);        
    break;
    
  case WM_TIMER:
    GRAPH_DATA_YT_AddValue(hData , osGetCPUUsage());
    WM_InvalidateWindow(pMsg->hWin);
    WM_RestartTimer(pMsg->Data.v, 400);
    break; 
    
  case WM_DELETE:
    WM_DeleteTimer(hTimerTime);
    break;
    
  default:
    WM_DefaultProc(pMsg);
  }
}

/**
  * @brief  Callback function of the Task window
  * @param  pMsg: pointer to data structure of type WM_MESSAGE
  * @retval None
  */
static void _cbTaskWindow(WM_MESSAGE * pMsg) {
  
  static WM_HTIMER hTimerTime; 

  switch (pMsg->MsgId) 
  {
  case WM_CREATE:
    /* Create timer */
    hTimerTime = WM_CreateTimer(pMsg->hWin, 0, 1000, 0);        
    break;
    
  case WM_TIMER:
    WM_InvalidateWindow(WM_GetParent(pMsg->hWin));
    WM_RestartTimer(pMsg->Data.v, 1000);
    break; 
    
  case WM_DELETE:
    WM_DeleteTimer(hTimerTime);
    break;
    
  default:
    WM_DefaultProc(pMsg);
  }
}


/**
  * @brief  Update Log Viewer
  * @param  Msg: pointer to new message to be logged
  * @retval None
  */
void k_UpdateLog(char *Msg)
{
  WM_HWIN hItem;
  
  if(hLog != 0)
  {
    hItem = WM_GetDialogItem(hLog, ID_MULTIEDIT_KERNELLOG);
    MULTIEDIT_SetText(hItem, Msg);
    MULTIEDIT_SetCursorOffset(hItem, LOG_IN_ptr);    
  }
}
/**
  * @brief  Callback function of the kernel log dialog
  * @param  pMsg: pointer to data structure of type WM_MESSAGE
  * @retval None
  */
static void _cbKernelLogDialog(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    
    hItem = pMsg->hWin;
    FRAMEWIN_SetTitleVis(hItem, 0);
    
    hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIEDIT_KERNELLOG);
    MULTIEDIT_SetFont(hItem, GUI_FONT_8_1);
    MULTIEDIT_SetTextColor(hItem, MULTIEDIT_CI_READONLY, GUI_GREEN); 
    MULTIEDIT_SetBkColor(hItem, MULTIEDIT_CI_READONLY, GUI_BLACK); 
    MULTIEDIT_SetReadOnly(hItem, 1);
    MULTIEDIT_SetText(hItem, (char *)pLOG_CacheBuffer);
    MULTIEDIT_SetCursorOffset(hItem, LOG_IN_ptr);
    MULTIEDIT_SetAutoScrollV(hItem, 1);
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case ID_BUTTON_CANCEL_KERNELLOG: /* Notifications sent by 'Cancel' button */
      switch(NCode) {
      case WM_NOTIFICATION_RELEASED:
        GUI_EndDialog(pMsg->hWin, 0); 
        hLog = 0;
        break;
      }
      break;
    case ID_MULTIEDIT_KERNELLOG: /* Notifications sent by 'Multiedit' */
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        break;
      case WM_NOTIFICATION_RELEASED:
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
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

/**
  * @brief  Update process manager view.
  * @param  hItem: pointer to window handle
  * @retval None
  */
static void _UpdateProcessManagerView(WM_HWIN  hItem) {
  int      Idx;
  char     str[3];
  int16_t  tasks_nbr;
  
  tasks_nbr = uxTaskGetSystemState( ProcessStatus, 16, NULL );
  
  /*Limit view size */
  if(tasks_nbr > 16)
  {
    tasks_nbr = 16;
  }
  
  for (Idx = 0; Idx < tasks_nbr; Idx ++)
  {
    LISTVIEW_SetItemText(hItem, 0, Idx, (char *)(ProcessStatus[Idx].pcTaskName)); 
    sprintf(str, "%lu", ProcessStatus[Idx].uxCurrentPriority);
    LISTVIEW_SetItemText(hItem, 1, Idx, str);
    
    switch (ProcessStatus[Idx].eCurrentState)
    {
    case eReady:
      LISTVIEW_SetItemText(hItem, 2, Idx, "Ready"); 
      break;
      
    case eBlocked:
      LISTVIEW_SetItemText(hItem, 2, Idx, "Blocked"); 
      break;
      
    case eDeleted:
      LISTVIEW_SetItemText(hItem, 2, Idx, "Deleted"); 
      break;
      
    case eSuspended:
      LISTVIEW_SetItemText(hItem, 2, Idx, "Suspended"); 
      break;
      
    case eRunning:
      LISTVIEW_SetItemText(hItem, 2, Idx, "Running"); 
      break;        
      
    default:
      LISTVIEW_SetItemText(hItem, 2, Idx, "Unknown"); 
      break;        
    }
  }
  LISTVIEW_SetSort(hItem, 1, 0);
}
  
/**
  * @brief  Callback function of the process manager dialog
  * @param  pMsg: pointer to data structure of type WM_MESSAGE
  * @retval None
  */
static void _cbProcessManagerDialog(WM_MESSAGE * pMsg) {
  WM_HWIN  hItem;
  int      NCode;
  int      Id, Idx;

  switch (pMsg->MsgId) {
    
  case WM_INIT_DIALOG:

    hItem = pMsg->hWin;
    FRAMEWIN_SetTitleVis(hItem, 0);
    
    hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_PROCESSMANAGER);
    LISTVIEW_AddColumn(hItem, 170, "Tasks", GUI_TA_HCENTER | GUI_TA_VCENTER);
    LISTVIEW_AddColumn(hItem, 60, "Priority", GUI_TA_HCENTER | GUI_TA_VCENTER);
    LISTVIEW_AddColumn(hItem, 70, "State", GUI_TA_HCENTER | GUI_TA_VCENTER);
    
    LISTVIEW_SetGridVis(hItem, 1);
    LISTVIEW_SetTextAlign(hItem, 0, GUI_TA_LEFT);
    LISTVIEW_SetTextAlign(hItem, 1, GUI_TA_HCENTER);
    LISTVIEW_SetTextAlign(hItem, 2, GUI_TA_HCENTER);
    HEADER_SetDragLimit(LISTVIEW_GetHeader(hItem), 1);
    LISTVIEW_SetCompareFunc(hItem, 1, LISTVIEW_CompareDec);
    LISTVIEW_SetTextColor(hItem, LISTVIEW_CI_SELFOCUS, GUI_LIGHTBLUE);  
  
    for (Idx = 0; Idx < 16; Idx ++)
    {
      LISTVIEW_AddRow(hItem, NULL);
    }
  
    _UpdateProcessManagerView(hItem);
    
    WM_CreateWindowAsChild(470, 0, 10, 10, pMsg->hWin, WM_CF_SHOW | WM_CF_HASTRANS, _cbTaskWindow , 0); 
    break;

  case WM_PAINT:
    hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_PROCESSMANAGER);
    _UpdateProcessManagerView(hItem);
    break;

  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case ID_BUTTON_CANCEL_PROCESSMANAGER: /* Notifications sent by 'Cancel' button */
      switch(NCode) {
      case WM_NOTIFICATION_RELEASED:
        GUI_EndDialog(pMsg->hWin, 0);
        hProcess = 0;       
        break;
      }
      break;
    case ID_LISTVIEW_PROCESSMANAGER: /* Notifications sent by 'Listview' */
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        break;
      case WM_NOTIFICATION_RELEASED:
        break;
      case WM_NOTIFICATION_SEL_CHANGED:
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

/**
  * @brief  Callback function of the performance dialog
  * @param  pMsg: pointer to data structure of type WM_MESSAGE
  * @retval None
  */
static void _cbPerformanceDialog(WM_MESSAGE * pMsg) {
  int NCode;
  int Id;
  WM_HWIN hGraph;

  GUI_RECT Rect;

  switch (pMsg->MsgId) 
  {
    
  case WM_INIT_DIALOG:

    FRAMEWIN_SetTitleVis(pMsg->hWin, 0);
    hGraph = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_CPU);
    hData = GRAPH_DATA_YT_Create(GUI_LIGHTGREEN, 500, 0, 20);
    GRAPH_SetGridVis(hGraph, 1);
    GRAPH_SetBorder(hGraph, 30, 2, 2, 2); 
    GRAPH_AttachData(hGraph, hData);
    hScale = GRAPH_SCALE_Create(20, GUI_TA_RIGHT, GRAPH_SCALE_CF_VERTICAL, 25);
    GRAPH_AttachScale(hGraph, hScale);  
    GRAPH_SCALE_SetTextColor(hScale, GUI_YELLOW);
    GRAPH_SetGridDistX(hGraph, 25);
    GRAPH_SetGridDistY(hGraph, 25);
    WM_GetClientRect(&Rect);
    WM_CreateWindowAsChild(470, 0, 10, 10, pMsg->hWin, WM_CF_SHOW | WM_CF_HASTRANS, _cbCpuWindow , 0); 
    
    break;
    
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
      
    case ID_BUTTON_HIDE: /* Notifications sent by Hide Button */
      switch(NCode) 
      {
      case WM_NOTIFICATION_RELEASED:
        
        if(hPerformance != 0)
        {
          WM_HideWindow(hPerformance);
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

/**
  * @brief  Move the sprite
  * @param  pSprite: pointer to data structure of sprite
  * @retval None
  */
static void _MoveSprite(SPRITE * pSprite) {
  /* Set new index */
  pSprite->Index += 1;
  if (pSprite->Index >= pSprite->NumImages) {
    pSprite->Index = 0;
  }
  /* Adjust X-position */
  pSprite->xPos += pSprite->dx;
  if (pSprite->xPos > 480) {
    pSprite->xPos = -pSprite->apImages[pSprite->Index]->XSize;
  }
  if (pSprite->xPos < -pSprite->apImages[pSprite->Index]->XSize) {
    pSprite->xPos = 480;
  }
  /* Adjust Y-position */
  pSprite->yPos += pSprite->dy;
  if (pSprite->yPos < 0) {
    pSprite->dy = -pSprite->dy;
  }
  if (pSprite->yPos > (272 - pSprite->apImages[pSprite->Index]->YSize)) {
    pSprite->dy = -pSprite->dy;
  }
  /* Change sprite */
  GUI_SPRITE_SetBitmapAndPosition(pSprite->hSprite, pSprite->apImages[pSprite->Index], pSprite->xPos, pSprite->yPos);
}

/**
  * @brief  show the sprite
  * @param  None
  * @retval None
  */
 void _ShowSprites(void) {
  /* Create the sprites */
   _aSprite[0].hSprite = GUI_SPRITE_CreateEx(_aSprite[0].apImages[0], _aSprite[0].xPos, _aSprite[0].yPos, 0);

}

/**
  * @brief  Adds one menu item to the given menu
  * @param  hMenu:    pointer to the handle of menu
  * @param  hSubmenu: pointer to the handle of Sub menu
  * @param  pText:    pointer to menu item description
  * @param  Id:       ID of the menu item
  * @param  Flags:    window creation flags
  * @retval None
  */
static void _AddMenuItem(MENU_Handle hMenu, MENU_Handle hSubmenu, const char* pText, U16 Id, U16 Flags) 
{
  MENU_ITEM_DATA Item;
  Item.pText    = pText;
  Item.hSubmenu = hSubmenu;
  Item.Flags    = Flags;
  Item.Id       = Id;
  MENU_AddItem(hMenu, &Item);
}

/**
  * @brief  The function opens a popup menu at the given position. It returns
  *         immediately after creation. On the first call it creates the menu.
  * @param  hParent:    pointer to the handle of the parent
  * @param  pMenuItems: pointer to menu items 
  * @param  NumItems:   number of menu items 
  * @param  x:          x position of the popup
  * @param  y:          y position of the popup 
  * @retval None
  */
static void _OpenPopup(WM_HWIN hParent, MENU_ITEM * pMenuItems, int NumItems, int x, int y) 
{
  static MENU_Handle hMenu;
  if (!hMenu) {
    int i;
    /* Create the popup window only one time */
    hMenu = MENU_CreateEx(0, 0, 0, 0, WM_UNATTACHED, 0, MENU_CF_VERTICAL, 0);
    MENU_SetBkColor(hMenu, MENU_CI_SELECTED, GUI_LIGHTBLUE);
    
    for (i = 0; i < NumItems; i++) {
      _AddMenuItem(hMenu, 0, pMenuItems[i].sText, pMenuItems[i].Id, pMenuItems[i].Flags);
    }
  }

  MENU_Popup(hMenu, hParent, x, y, 0, 0, 0);
}

/**
  * @brief  Callback routine of desktop window.
  * @param  pMsg: pointer to data structure of type WM_MESSAGE
  * @retval None
  */
static void _cbBk(WM_MESSAGE * pMsg) {
  
  MENU_MSG_DATA* pData;
  uint32_t NCode, Id;
  static uint8_t sel = 0;
  
  switch (pMsg->MsgId) 
  {

  case WM_MENU:
    /* Process the menu message */
    pData = (MENU_MSG_DATA*)pMsg->Data.p;
    switch (pData->MsgType) 
    {
    case MENU_ON_ITEMSELECT:
      /* A menu item has been selected */
      switch (pData->ItemId) 
      {  
        
      case ID_MENU_LOG:
        if (hLog == 0)
        {
          hLog = GUI_CreateDialogBox(_aKernelLogDialogCreate, 
                                     GUI_COUNTOF(_aProcessManagerDialogCreate), 
                                     _cbKernelLogDialog, 
                                     pMsg->hWin, 
                                     FRAMEWIN_CF_MOVEABLE | WM_CF_FGND | WM_CF_STAYONTOP, 
                                     50);  
        }
        else
        {
          WM_ShowWindow(hLog);
          WM_BringToTop(hLog);
        }        
        break;
        
      case ID_MENU_PMGR:
        if(hProcess == 0)
        {
          hProcess = GUI_CreateDialogBox(_aProcessManagerDialogCreate, 
                                         GUI_COUNTOF(_aProcessManagerDialogCreate), 
                                         _cbProcessManagerDialog, 
                                         pMsg->hWin, 
                                         FRAMEWIN_CF_MOVEABLE | WM_CF_FGND | WM_CF_STAYONTOP, 
                                         50);  
        }
        else
        {
          WM_ShowWindow(hProcess);
          WM_BringToTop(hProcess);
        }
        break;
        
      case ID_MENU_PERF:
        if(hPerformance == 0)
        {
          hPerformance = GUI_CreateDialogBox(_aPerformanceDialogCreate, 
                                             GUI_COUNTOF(_aPerformanceDialogCreate), 
                                             _cbPerformanceDialog, 
                                             pMsg->hWin, 
                                             FRAMEWIN_CF_MOVEABLE | WM_CF_FGND | WM_CF_STAYONTOP, 
                                             50);
        }
        else
        {
          WM_ShowWindow(hPerformance);
          WM_BringToTop(hPerformance);
        }
        break;
        
      case ID_MENU_EXIT:
        break;
      }
    }
    break;
      
  case WM_PAINT:
    GUI_SetBkColor(GUI_TRANSPARENT);
    GUI_Clear();
    if(hIcon)
    {
      WM_BringToBottom(hIcon);
    }
    break;
    
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);     
    NCode = pMsg->Data.v;  
    
    switch (NCode) 
    {  
      
    case WM_NOTIFICATION_CHILD_DELETED:
      SpriteDisabled = 0;
      GUI_SPRITE_Show(_aSprite[0].hSprite);
      break;
      
    case WM_NOTIFICATION_CLICKED:   
      if (Id == ID_ICONVIEW_MENU)
      { 
        
        sel = ICONVIEW_GetSel(pMsg->hWinSrc);
        
//        if(hPerformance != 0)
//        {
//          WM_HideWindow(hPerformance);
//        } 
        
        if(sel < k_ModuleGetNumber())
        {    
          if(sel < 8)
          {
            ICONVIEW_SetSel(pMsg->hWinSrc, -1);
          }
          else
          {
            ICONVIEW_SetSel(pMsg->hWinSrc, k_ModuleGetNumber() + 1); 
          }
        }
      }    
      break;
      
    case WM_NOTIFICATION_RELEASED:
      if (Id == ID_ICONVIEW_MENU)
      {
        
        if(sel < k_ModuleGetNumber())
        { 
          SpriteDisabled = 1;
          GUI_SPRITE_Hide(_aSprite[0].hSprite);
          module_prop[sel].module->startup(pMsg->hWin, 0, 26);
          sel = 0;
        }
      }
      else if (Id == ID_BUTTON_BKGND)
      {
        /* Create popup menu after touching the display */
        _OpenPopup(WM_HBKWIN, _aMenuItems, GUI_COUNTOF(_aMenuItems),0 , 25);  
      }
      break;
    
    default:
      break;
    }
    break;
  default:
    WM_DefaultProc(pMsg);
  }
}

/**
  * @brief  Callback routine of desktop window status.
  * @param  pMsg: pointer to data structure of type WM_MESSAGE
  * @retval None
  */
static void _cbStatus(WM_MESSAGE * pMsg) {
  int xSize, ySize;
  static uint8_t TempStr[50];
	static uint8_t TempStrDate[50];
	static uint8_t TempStrTime[50];
  static WM_HTIMER hTimerTime;
  static WM_HTIMER hSpriteTime;  
	int8_t DATEX;
		
  RTC_TimeTypeDef   RTC_Time;
  RTC_DateTypeDef   RTC_DateStructure;

  WM_HWIN hWin;

  hWin = pMsg->hWin;
	
	int ScreenTimeout = 30;
  switch (pMsg->MsgId) 
  {
  case WM_CREATE:
    hTimerTime = WM_CreateTimer(hWin, ID_TIMER_TIME, 1000, 0); //Was 1000
    if(settings.b.enable_sprite)
    {
      hSpriteTime = WM_CreateTimer(hWin, ID_SPRITE_TIME, 50, 0);    
    }
    break;
    
  case WM_DELETE:
    WM_DeleteTimer(hTimerTime);
    if(settings.b.enable_sprite)
    {    
      WM_DeleteTimer(hSpriteTime);    
    }
    break;
    
  case WM_TIMER:
    if( WM_GetTimerId(pMsg->Data.v) == ID_TIMER_TIME)
    {
      WM_InvalidateWindow(hWin);
			if(GuiState.gui_scn_state == G_RTSTA)
			{
					WM_InvalidateWindow(GetWinHandleIccpmain());// invoke repaint of iccpmain
			}
			else if(GuiState.gui_scn_state == G_GRAPHS)			
			{
				//		WM_InvalidateWindow(GetWinHandleiccp_gph());// invoke repaint of iccp_gph
			}
			
				if(GuiState.gui_scn_state == G_CV)
				{
						GuiState.ScreenTimeout++;
						if(GuiState.ScreenTimeout>=ScreenTimeout)
						{
							if(GuiState.cvsrcn_state == G_CV_KYBUP){GUI_EndDialog(GetWinHandleiccp_kybd(), 1);}
							WM_InvalidateWindow(GetWinHandleIccpmain());
							WM_Paint(GetWinHandleIccpmain());
							GUI_EndDialog(GetWinHandleiccp_cvDLG(),NULL);
							GuiState.gui_scn_state = G_RTSTA;
							GuiState.cvsrcn_state = G_CV_INIT;
						}
				}
				else if(GuiState.gui_scn_state == G_CI)
				{
						GuiState.ScreenTimeout++;
						if(GuiState.ScreenTimeout>=ScreenTimeout)
						{
							if(GuiState.cisrcn_state == G_CI_KYBUP){GUI_EndDialog(GetWinHandleiccp_kybd(), 1);}
							WM_InvalidateWindow(GetWinHandleIccpmain());
							WM_Paint(GetWinHandleIccpmain());
							GUI_EndDialog(GetWinHandleiccp_ciDLG(),NULL);
							GuiState.gui_scn_state = G_RTSTA;
							GuiState.cisrcn_state = G_CI_INIT;
						}
				}
				else if(GuiState.gui_scn_state == G_CP)
				{
						GuiState.ScreenTimeout++;
						WM_InvalidateWindow(GetWinHandleiccp_cpDLG()); //To Refresh (WM_PAINT) Window
						if(GuiState.ScreenTimeout>=ScreenTimeout)
						{
							if(GuiState.cpsrcn_state == G_CP_KYBUP){GUI_EndDialog(GetWinHandleiccp_kybd(), 1);}
							WM_InvalidateWindow(GetWinHandleIccpmain());
							WM_Paint(GetWinHandleIccpmain());
							GUI_EndDialog(GetWinHandleiccp_cpDLG(),NULL);
							GuiState.gui_scn_state = G_RTSTA;
							GuiState.cpsrcn_state = G_CP_INIT;
						}
				}
				else
				{
					GuiState.ScreenTimeout = 0;
				}
			
			WM_RestartTimer(pMsg->Data.v, 1000);
    }
    else if( WM_GetTimerId(pMsg->Data.v) == ID_SPRITE_TIME)
    {
      if(settings.b.enable_sprite)
      {    
        if(SpriteDisabled == 0)
        {
          _MoveSprite(&_aSprite[0]);
        }
      }
      WM_RestartTimer(pMsg->Data.v, 100);
    }
    break;
    
  case WM_PAINT:
    xSize = WM_GetWindowSizeX(hWin);
    ySize = WM_GetWindowSizeY(hWin);
    
    /* Draw background */
    GUI_SetColor(0x40303030);
    GUI_FillRect(0, 0, xSize , ySize - 3);
    GUI_SetColor(0x808080);
    GUI_DrawHLine(ySize - 2, 0, xSize );
    GUI_SetColor(0x404040);
    GUI_DrawHLine(ySize - 1, 0, xSize );

    /* Draw time & Date */
    GUI_SetTextMode(GUI_TM_TRANS);
    GUI_SetColor(GUI_WHITE);
    GUI_SetFont(GUI_FONT_13B_ASCII);
    
	
		/* IS  USB comnnexcted? yes draw it*/
		DATEX=0;// 
		if(k_StorageGetStatus(USB_DISK_UNIT))
		{
			 GUI_DrawBitmap(&bmusbdisk, xSize - 80, 0);
			 DATEX = DATEX + 80;
		}
		
		/* Is uSD connected if yes draw it */
		if(k_StorageGetStatus(MSD_DISK_UNIT))
		{
			 GUI_DrawBitmap(&bmmicrosd, xSize - 80 - 35 * k_StorageGetStatus(USB_DISK_UNIT) , 0);
			 DATEX = DATEX + 35;
		}    
		
		
    k_GetTime(&RTC_Time);
    GuiState.sec    =  RTC_Time.Seconds;
	  GuiState.min    =  RTC_Time.Minutes;
    GuiState.hour   =  RTC_Time.Hours;
//    sprintf((char *)TempStr, "%02d:%02d:%02d", GuiState.hour, GuiState.min, GuiState.sec);
//    GUI_DispStringAt((char *)TempStr, xSize - 50, 4);

		
//		//EAJ: Debug Information (includes screen timeout and Active Electrodes)
//		sprintf((char *)TempStr, "%d%d%d%d %02d %02d:%02d:%02d",
//		GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.refelec_active >>0 &0x01,
//		GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.refelec_active >>1 &0x01,
//		GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.refelec_active >>2 &0x01,
//		GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.refelec_active >>3 &0x01,
//		GuiState.ScreenTimeout, GuiState.hour	, GuiState.min, GuiState.sec);
//		GUI_DispStringAt((char *)TempStr, xSize - 95, 4);
		
		if(GuiState.hour < 12)
		{
			if(GuiState.hour == 0){GuiState.hour = GuiState.hour+12;} //Turn 0 AM into 12 AM
				
			sprintf((char *)TempStrTime, "%02d:%02d:%02d AM", GuiState.hour, GuiState.min, GuiState.sec);
		}
		else
		{
			sprintf((char *)TempStrTime, "%02d:%02d:%02d PM", (GuiState.hour-12), GuiState.min, GuiState.sec);
		}
    
		
    k_GetDate(&RTC_DateStructure);
    
    
    GuiState.year =  RTC_DateStructure.Year + 2015;
    GuiState.month =  RTC_DateStructure.Month;
    GuiState.day =  RTC_DateStructure.Date;
    
//		if((GuiState.day > 0) && (GuiState.day <= 31) && 
//				 (GuiState.month > 0)&& (GuiState.month <= 12) && 
//					 (GuiState.year >= 1900))
//		{
//			sprintf((char *)TempStr, "%02d, %s, %04d", GuiState.day , strMonth[GuiState.month-1], GuiState.year);
//		}
//		else
//		{
//			sprintf((char *)TempStr, "01, January, 1900");
//		}
		
		if((GuiState.day > 0) && (GuiState.day <= 31) && 
				 (GuiState.month > 0)&& (GuiState.month <= 12) && 
					 (GuiState.year >= 1900))
		{
			sprintf((char *)TempStrDate, "%02d, %s, %04d", GuiState.day , strMonth[GuiState.month-1], GuiState.year);
		}
		else
		{
			sprintf((char *)TempStrDate, "01, January, 1900");
		}
		
		sprintf((char *)TempStr, "%s %s",(char *)TempStrDate,(char *)TempStrTime);
		
		GUI_DispStringAt((char *)TempStr, (xSize - (strlen((char*)TempStr)* 5) - 30), 4);
		
		
//		GUI_DispStringHCenterAt((char *)TempStr, (xSize - (strlen((char*)TempStr)* 10) + DATEX), 4);
		
		if((GuiState.gui_scn_state == G_CV)||(GuiState.gui_scn_state == G_CI)||(GuiState.gui_scn_state == G_CP))
		{
			sprintf((char *)TempStr,"Screen Timeout:%02d",30-GuiState.ScreenTimeout);
		}
		else
		{
			if(GuiState.demo==1)
			{
				sprintf((char *)TempStr,"DEMO             ");
			}
			else
			{
				sprintf((char *)TempStr,"");
			}
		}
		GUI_DispStringHCenterAt((char *)TempStr, (xSize - (strlen((char*)TempStr)* 10) - 80), 4);
		
		
		// if IP address is all 0's "not connected" then show x y cord of touch instead of IP
		if(strlen(dbgstrip) <14)
		{
////			sprintf((char *)TempStr, "Free:%lu Used:%lu",
////				GUI_ALLOC_GetNumFreeBytes(),GUI_ALLOC_GetNumUsedBytes()); 
			sprintf((char *)TempStr, "%s","");
			
			GUI_DispStringAt( (char *)TempStr, 40, 4);

		}
		else
		{
				/****************************************************/
			
				
			
			  /****************************************************/
			
////				sprintf((char *)TempStr, "CPU:%d%% %s", osGetCPUUsage(),dbgstrip);
			  sprintf((char *)TempStr, "%s", dbgstrip);
				
				GUI_DispStringAt( (char *)TempStr, 40, 4);
		}
    
    
    
    break;
    
  default:
    WM_DefaultProc(pMsg);
  }
}

/**
  * @brief  Paints of the owner drawn button.
  * @param  hObj: pointer to button handle
  * @retval None
  */
static void _OnPaint(BUTTON_Handle hObj) {
  int Index;

  GUI_RECT Rect;
  Index = (WIDGET_GetState(hObj) & BUTTON_STATE_PRESSED) ? 1 : 0;
  
  GUI_SetBkColor(FRAMEWIN_GetDefaultClientColor());
  GUI_Clear();  
  
  WM_GetClientRect(&Rect);
  
  /* Draw button background */
  if(Index)
  {
    GUI_SetColor(0x40000000 | GUI_GRAY);
  }
  else
  {
    GUI_SetColor(0x40000000 | GUI_DARKGRAY);
  }

  GUI_FillRectEx(&Rect);

  /* Draw black shape */
  GUI_SetColor(GUI_DARKGRAY);
  GUI_DrawRectEx(&Rect);
  
  //GUI_DrawBitmap(&bmatdi_vsmall,0,0);//(&bmSTLogo40x20, 0, 0);
}

/**
  * @brief  callback for background button
  * @param  pMsg: pointer to data structure of type WM_MESSAGE
  * @retval None
  */
static void _cbButton(WM_MESSAGE * pMsg) {
  switch (pMsg->MsgId) {
    case WM_PAINT:
      _OnPaint(pMsg->hWin);
      break;
    default:
      /* The original callback */
      BUTTON_Callback(pMsg); 
      break;
  }
}

/**
  * @brief  set the GUI profile
  * @param  None.
  * @retval None
  */
void k_SetGuiProfile(void) 
{
 
  settings.d32 = k_BkupRestoreParameter(CALIBRATION_GENERAL_SETTINGS_BKP);

  if(settings.b.disable_flex_skin == 0)
  {
    FRAMEWIN_SetDefaultSkin(FRAMEWIN_SKIN_FLEX);  
    PROGBAR_SetDefaultSkin(PROGBAR_SKIN_FLEX);
    RADIO_SetDefaultSkin(RADIO_SKIN_FLEX);
    SCROLLBAR_SetDefaultSkin(SCROLLBAR_SKIN_FLEX);
    SLIDER_SetDefaultSkin(SLIDER_SKIN_FLEX);
    SPINBOX_SetDefaultSkin(SPINBOX_SKIN_FLEX);
    BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);
    DROPDOWN_SetDefaultSkin(DROPDOWN_SKIN_FLEX);
    
    FRAMEWIN_SetDefaultTextColor(0, GUI_DARKGRAY);
    FRAMEWIN_SetDefaultTextColor(1, GUI_DARKGRAY);      
  }
  else
  {
    FRAMEWIN_SetDefaultTextColor(0, GUI_WHITE);
    FRAMEWIN_SetDefaultTextColor(1, GUI_WHITE);   
  }

  FRAMEWIN_SetDefaultFont(GUI_FONT_10_ASCII);

  FRAMEWIN_SetDefaultClientColor(GUI_LIGHTGRAY | 0x20F50000);  
  FRAMEWIN_SetDefaultTitleHeight(15);
  
  TREEVIEW_SetDefaultBkColor(TREEVIEW_CI_SEL, GUI_LIGHTGRAY | 0x20E50000);
  TREEVIEW_SetDefaultBkColor(TREEVIEW_CI_UNSEL, GUI_LIGHTGRAY | 0x20E50000);
  TREEVIEW_SetDefaultTextColor(TREEVIEW_CI_SEL, GUI_BLUE);
  
  WINDOW_SetDefaultBkColor(GUI_LIGHTGRAY | 0x20E50000);
  MULTIPAGE_SetDefaultBkColor(GUI_LIGHTGRAY | 0x20E50000, 0);
  MULTIPAGE_SetDefaultBkColor(GUI_LIGHTGRAY | 0x20E50000, 1);    
  MULTIPAGE_SetDefaultFont(GUI_FONT_10_ASCII); 
  CHECKBOX_SetDefaultBkColor(GUI_LIGHTGRAY | 0x20E50000);
  
  CALENDAR_SetDefaultSize(CALENDAR_SI_HEADER, 20 );
  CALENDAR_SetDefaultSize(CALENDAR_SI_CELL_X, 25 );
  CALENDAR_SetDefaultSize(CALENDAR_SI_CELL_Y, 21 ); 
  
  MULTIPAGE_SetDefaultBkColor (GUI_WHITE, 1);
    
  SPINBOX_SetDefaultButtonSize(15);
  LISTVIEW_SetDefaultGridColor(GUI_WHITE);
  SCROLLBAR_SetDefaultWidth(16);

  HEADER_SetDefaultBkColor(0x00C5903E);
  HEADER_SetDefaultTextColor(GUI_WHITE);
  SCROLLBAR_SetDefaultColor(GUI_LIGHTGRAY | 0x20F50000, SCROLLBAR_CI_THUMB);  
  SCROLLBAR_SetDefaultColor(GUI_LIGHTGRAY | 0x20F50000, SCROLLBAR_CI_SHAFT);  
  SCROLLBAR_SetDefaultColor(GUI_LIGHTGRAY | 0x20F50000, SCROLLBAR_CI_ARROW);    
  CHOOSEFILE_SetDelim('/');
}

/**
  * @brief  Initializes the menu.
  * @param  None.
  * @retval None
  */
void k_InitMenu(void) 
{

  WM_HWIN  hItem;
  uint8_t i = 0;
  GUI_SetLayerVisEx (0, 0);
  GUI_DrawBitmap(&bmatdi_bg_greywood, 0,0);     
  
  settings.d32 = k_BkupRestoreParameter(CALIBRATION_GENERAL_SETTINGS_BKP);
  
  GUI_SetLayerVisEx (0, 1);
  GUI_SelectLayer(1);
  GUI_Clear();

  WM_SetCallback(WM_HBKWIN, _cbBk);
  
  WM_CreateWindowAsChild(0,
                         0,
                         LCD_GetXSize(),
                         25,
                         WM_HBKWIN, 
                         WM_CF_SHOW | WM_CF_HASTRANS , 
                         _cbStatus, 
                         0);
     
  hIcon = ICONVIEW_CreateEx(0, 
                            26, 
                            LCD_GetXSize(), 
                            LCD_GetYSize()- 26, 
                            WM_HBKWIN, 
                            WM_CF_SHOW | WM_CF_HASTRANS | WM_CF_BGND ,
                            ICONVIEW_CF_AUTOSCROLLBAR_V,
                            ID_ICONVIEW_MENU, 
                            112, 
                            105);
  
  ICONVIEW_SetFont(hIcon, &GUI_Font13B_ASCII);
  
  ICONVIEW_SetBkColor(hIcon, ICONVIEW_CI_SEL, GUI_LIGHTBLUE |GUI_TRANSPARENT);
  
  ICONVIEW_SetSpace(hIcon, GUI_COORD_Y, 10);
  
  ICONVIEW_SetFrame(hIcon, GUI_COORD_Y, 10);
    
  for (i = 0; i < k_ModuleGetNumber(); i++)
  {
    ICONVIEW_AddBitmapItem(hIcon,module_prop[i].module->icon, (char *)module_prop[i].module->name);
  }

  if(settings.b.enable_sprite)
  {
    _ShowSprites();
  }
  
  hItem = BUTTON_CreateEx(0, 0, 40, 23, WM_HBKWIN, WM_CF_SHOW, 0, ID_BUTTON_BKGND);
  WM_SetCallback(hItem, _cbButton);
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
