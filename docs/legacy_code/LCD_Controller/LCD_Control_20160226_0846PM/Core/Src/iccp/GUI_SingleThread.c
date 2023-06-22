
#include "cmsis_os.h"                   // CMSIS RTOS header file
#include "GUI.h"
#include "dialog.h"

#ifdef _RTE_
#include "RTE_Components.h"             // Component selection
#endif

extern int GUImode;
extern void ExecKeypad(void);
/*----------------------------------------------------------------------------
 *      GUIThread: GUI Thread for Single-Task Execution Model
 *---------------------------------------------------------------------------*/
 
void GUIThread (void const *argument);              // thread function, prototype thread function ( the function must be written below. It calls tasks pertinent to it and runs while(1)
osThreadId tid_GUIThread;                           // thread id, give thread a handle
osThreadDef (GUIThread, osPriorityIdle, 1, 2048);   // define thread object, define GUIThread function os thread if "0" passed in stacksz then Default value used in cfg wiz
																										// if stacksz not "0" then it uses some of the the stacksz allocated in RTX_Conf_CM

/*---------- Create the GUI thread -------------------------*/
// Called from main() 
int Init_GUIThread (void) {

  tid_GUIThread = osThreadCreate (osThread(GUIThread), NULL);
  if(!tid_GUIThread) return(-1);
  
  return(0);
}


extern WM_HWIN Createiccp_main(void);
void GUIThread (void const *argument) {
	//osKernelInitialize (); // Initialize RTX
  
	GUI_Init();                     /* Initialize the Graphics Component  Part of Compiled Library cant see source*/
	GUI_CURSOR_Show();
	WM_SetCreateFlags(WM_CF_MEMDEV);  // Use memory devices on all windows to avoid flicker
	/* Add GUI setup code here */
	
	Createiccp_main();
  while (1) {
    
    /* All GUI related activities might only be called from here */

    GUI_TOUCH_Exec();             /* Execute Touchscreen support */
    GUI_Exec();                   /* Execute all GUI jobs ... Return 0 if nothing was done. */
    GUI_X_ExecIdle();             /* Nothing left to do for the moment ... Idle processing */
		
//		if(GUImode ==1)
//		{
//				ExecKeypad();
//		}
  GUI_Delay(1000);
	}
}

