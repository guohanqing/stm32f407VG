/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2012  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.16 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The software has been licensed to  ARM LIMITED whose registered office
is situated at  110 Fulbourn Road,  Cambridge CB1 9NJ,  England solely
for  the  purposes  of  creating  libraries  for  ARM7, ARM9, Cortex-M
series,  and   Cortex-R4   processor-based  devices,  sublicensed  and
distributed as part of the  MDK-ARM  Professional  under the terms and
conditions  of  the   End  User  License  supplied  with  the  MDK-ARM
Professional. 
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
---Author-Explanation
* 
* 1.00.00 020519 JJL    First release of uC/GUI to uC/OS-II interface
* 
*
* Known problems or limitations with current version
*
*    None.
*
*
* Open issues
*
*    None
*********************************************************************************************************
*/
#include "GUI_Private.H"
#include "stdio.H"
#include <rtthread.h>

/*
*********************************************************************************************************
*                                         GLOBAL VARIABLES
*********************************************************************************************************
*/
/*指向信号量的指针*/ 

static rt_sem_t DispSem=RT_NULL; 
static rt_sem_t EventSem=RT_NULL; 
static rt_sem_t KeySem=RT_NULL; 

static  int     KeyPressed;
static  char    KeyIsInited;


/*
*********************************************************************************************************
*                                        TIMING FUNCTIONS
*
* Notes: Some timing dependent routines of uC/GUI require a GetTime and delay funtion. 
*        Default time unit (tick), normally is 1 ms.
*********************************************************************************************************
*/

int  GUI_X_GetTime (void) 
{
	return (int)(rt_tick_get());
}


void  GUI_X_Delay (int period) 
{
  rt_thread_delay(period);
}


/*
*********************************************************************************************************
*                                          GUI_X_ExecIdle()
*********************************************************************************************************
*/
void GUI_X_ExecIdle (void) 
{
    GUI_X_Delay(1);
}


/*
*********************************************************************************************************
*                                    MULTITASKING INTERFACE FUNCTIONS
*
* Note(1): 1) The following routines are required only if uC/GUI is used in a true multi task environment, 
*             which means you have more than one thread using the uC/GUI API.  In this case the #define 
*             GUI_OS 1   needs to be in GUIConf.h
*********************************************************************************************************
*/

void  GUI_X_InitOS (void)
{ 
	DispSem =rt_sem_create("DispSem" , 0,RT_IPC_FLAG_FIFO); 
	EventSem=rt_sem_create("EventSem", 0,RT_IPC_FLAG_FIFO); 
}

void  GUI_X_Lock (void)
{ 	   
	rt_sem_take(DispSem, 0);
}

void  GUI_X_Unlock (void)
{ 
	rt_sem_release(DispSem);
}


U32  GUI_X_GetTaskId (void) 
{ 
	return (0);
}

/*
*********************************************************************************************************
*                                        GUI_X_WaitEvent()
*                                        GUI_X_SignalEvent()
*********************************************************************************************************
*/


void GUI_X_WaitEvent (void) 
{
	rt_sem_take(EventSem, 0);
}


void GUI_X_SignalEvent (void) 
{
	rt_sem_release(EventSem);
}

/*
*********************************************************************************************************
*                                      KEYBOARD INTERFACE FUNCTIONS
*
* Purpose: The keyboard routines are required only by some widgets.
*          If widgets are not used, they may be eliminated.
*
* Note(s): If uC/OS-II is used, characters typed into the log window will be placed	in the keyboard buffer. 
*          This is a neat feature which allows you to operate your target system without having to use or 
*          even to have a keyboard connected to it. (useful for demos !)
*********************************************************************************************************
*/

void  CheckInit (void) 
{
	if (KeyIsInited == 0u) {
		KeyIsInited = 1u;
		GUI_X_Init();
	}
}

void GUI_X_Init (void) 
{
	KeySem=rt_sem_create("KeySem", 0,RT_IPC_FLAG_FIFO);
}

int  GUI_X_GetKey (void) 
{
	int r;

	r          = KeyPressed;
	CheckInit();
	KeyPressed = 0;
	return (r);
}

int  GUI_X_WaitKey (void) 
{
	int    r;

	CheckInit();
	if (KeyPressed == 0) {   
		rt_sem_take(KeySem, 0);
	}
	r          = KeyPressed;
	KeyPressed = 0;
	return (r);		   
}

void  GUI_X_StoreKey (int k) 
{
	KeyPressed = k;
	rt_sem_release(KeySem);
}

void GUI_X_Log     (const char *s) { GUI_USE_PARA(s); }
void GUI_X_Warn    (const char *s) { GUI_USE_PARA(s); }
void GUI_X_ErrorOut(const char *s) { GUI_USE_PARA(s); }
