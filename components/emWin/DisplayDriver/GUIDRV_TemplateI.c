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
----------------------------------------------------------------------
File        : GUIDRV_TemplateI.c
Purpose     : Display driver
---------------------------END-OF-HEADER------------------------------
*/

#include <stddef.h>

#include "GUI_Private.h"
#include "LCD_SIM.h"

#include "GUIDRV_TemplateI_Private.h"

#if (!defined(WIN32) | defined(LCD_SIMCONTROLLER))

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#if defined(WIN32) && defined(LCD_SIMCONTROLLER)
  //
  // TBD: Add prototypes of simulation routines
  //
#endif

//
// Register definitions
//

// TBD: Add hardware dependent defines/macros/register definitions

#define REG_LUT_WRITE_B    0x08
#define REG_LUT_WRITE_G    0x09
#define REG_LUT_WRITE_R    0x0A
#define REG_LUT_WRITE_ADDR 0x0B

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _InitOnce
*
* Purpose:
*   Allocates a fixed block for the context of the driver
*
* Return value:
*   0 on success, 1 on error
*/
static int _InitOnce(GUI_DEVICE * pDevice) {
  if (pDevice->u.pContext == NULL) {
    pDevice->u.pContext = GUI_ALLOC_GetFixedBlock(sizeof(DRIVER_CONTEXT));
    GUI__memset((U8 *)pDevice->u.pContext, 0, sizeof(DRIVER_CONTEXT));
  }
  if (pDevice->u.pContext) {
    return 0; // Ok
  } else {
    return 1; // Error
  }
}

/*********************************************************************
*
*       _SetVSize
*/
static void _SetVSize(GUI_DEVICE * pDevice, int xSize, int ySize) {
  DRIVER_CONTEXT * pContext;

  _InitOnce(pDevice);
  if (pDevice->u.pContext) {
    pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
    pContext->vxSize = xSize;
    pContext->vySize = ySize;
  }
}

/*********************************************************************
*
*       _SetSize
*/
static void _SetSize(GUI_DEVICE * pDevice, int xSize, int ySize) {
  DRIVER_CONTEXT * pContext;

  _InitOnce(pDevice);
  if (pDevice->u.pContext) {
    pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
    pContext->xSize = xSize;
    pContext->ySize = ySize;
    if (pContext->vxSize == 0) {
      pContext->vxSize = pContext->xSize;
    }
    if (pContext->vySize == 0) {
      pContext->vySize = pContext->ySize;
    }
  }
}

/*********************************************************************
*
*       _Check_XXX_BitBus
*/
static void _Check_XXX_BitBus(GUI_DEVICE * pDevice) {
  DRIVER_CONTEXT * pContext;

  pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
  if ((pContext->HW_API.pfWrite16_A0  == NULL) ||
      (pContext->HW_API.pfWrite16_A1  == NULL) ||
      (pContext->HW_API.pfWriteM16_A0 == NULL) ||
      (pContext->HW_API.pfWriteM16_A1 == NULL)
     ) {
    GUI_DEBUG_ERROROUT("Error in " __FILE__ ". Missing hardware routines.");
    while (1); // Error
  }
  if ((pContext->UseCache == 0) && (pContext->HW_API.pfRead16_A1 == NULL)) {
    GUI_DEBUG_ERROROUT("Error in " __FILE__ ". Missing 16 bit hardware routines.");
    while (1); // Error
  }
}

/*********************************************************************
*
*       _Dummy
*/
static void _Dummy(void) {}

/*********************************************************************
*
*       Static code: Initialization
*
**********************************************************************
*/
/*********************************************************************
*
*       _Init_XXX
*/
static void _Init_XXX(GUI_DEVICE * pDevice) {
  DRIVER_CONTEXT * pContext;

  pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
  if (pContext == NULL) {
    GUI_DEBUG_ERROROUT("Error in " __FILE__ ". Unknown error.");
    while (1); // Error
  }
  if (pContext->HW_API.pfFlushBuffer == NULL) {
    pContext->HW_API.pfFlushBuffer = _Dummy;
  }
  #if defined(WIN32) && defined(LCD_SIMCONTROLLER)
    //
    // TBD: Route function pointers of pContext->HW_API to simulation routines
    //
  #endif
  //
  // Check if hardware functions are available
  //
  if (pContext->pfCheck) {
    pContext->pfCheck(pDevice);
  }
  //
  // Check color depth
  //
  if (pContext->BitsPerPixel != 8) {
    GUI_DEBUG_ERROROUT("Error in " __FILE__ ". Selected color depth currently not supported for this controller.");
    while (1); // Error
  }
}

/*********************************************************************
*
*       _Init
*/
static int _Init(GUI_DEVICE * pDevice) {
  DRIVER_CONTEXT * pContext;
  int xSize, ySize;

  if (_InitOnce(pDevice)) {
    GUI_DEBUG_ERROROUT("Error in " __FILE__ ". No memory available.");
    while (1); // Error
  }
  if (pDevice->u.pContext) {
    pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
    //
    // Calculate BytesPerLine and BitsPerPixel
    //
    if (pDevice->pDeviceAPI->pfGetDevProp(pDevice, LCD_DEVCAP_SWAP_XY)) {
      xSize = pContext->ySize;
      ySize = pContext->xSize;
    } else {
      xSize = pContext->xSize;
      ySize = pContext->ySize;
    }
    pContext->BitsPerPixel = (int)pDevice->pDeviceAPI->pfGetDevProp(pDevice, LCD_DEVCAP_BITSPERPIXEL);
    pContext->BytesPerLine = ((xSize + pContext->FirstSEG) * pContext->BitsPerPixel + 7) / 8;
    //
    // Allocate cache
    //
    if (pContext->UseCache) {
      pContext->MemSize = pContext->BytesPerLine * (ySize + pContext->FirstCOM);
      pContext->pVMEM   = (U8 *)GUI_ALLOC_GetFixedBlock(pContext->MemSize);
      if (pContext->pVMEM == NULL) {
        GUI_DEBUG_ERROROUT("Error in " __FILE__ ". No memory available.");
        while (1); // Error
      }
    }
    //
    // Controller specific initialization
    //
    if (pContext->pfInit) {
      pContext->pfInit(pDevice);
    } else {
      GUI_DEBUG_ERROROUT("Error in " __FILE__ ". No display controller selected.");
      while (1); // Error
    }
  }
  return LCD_X_DisplayDriver(pDevice->LayerIndex, LCD_X_INITCONTROLLER, NULL);
}

/*********************************************************************
*
*       Private code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUIDRV__TemplateI_SetOrg
*/
void GUIDRV__TemplateI_SetOrg(GUI_DEVICE * pDevice, int x, int y) {
  LCD_X_SETORG_INFO Data = {0};

  #ifdef WIN32
    LCDSIM_SetOrg(x, y, pDevice->LayerIndex);
  #else
    Data.xPos = x;
    Data.yPos = y;
    LCD_X_DisplayDriver(pDevice->LayerIndex, LCD_X_SETORG, (void *)&Data);
  #endif
}

/*********************************************************************
*
*       GUIDRV__TemplateI_GetDevFunc
*/
void (* GUIDRV__TemplateI_GetDevFunc(GUI_DEVICE ** ppDevice, int Index))(void) {
  GUI_USE_PARA(ppDevice);
  switch (Index) {
  case LCD_DEVFUNC_SET_VSIZE:
    return (void (*)(void))_SetVSize;
  case LCD_DEVFUNC_SET_SIZE:
    return (void (*)(void))_SetSize;
  case LCD_DEVFUNC_INIT:
    return (void (*)(void))_Init;
  }
  return NULL;
}

/*********************************************************************
*
*       GUIDRV__TemplateI_GetDevProp
*/
I32 GUIDRV__TemplateI_GetDevProp(GUI_DEVICE * pDevice, int Index) {
  DRIVER_CONTEXT * pContext;

  pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
  switch (Index) {
  case LCD_DEVCAP_XSIZE:
    return pContext->xSize;
  case LCD_DEVCAP_YSIZE:
    return pContext->ySize;
  case LCD_DEVCAP_VXSIZE:
    return pContext->vxSize;
  case LCD_DEVCAP_VYSIZE:
    return pContext->vySize;
  case LCD_DEVCAP_XMAG:
    return 1;
  case LCD_DEVCAP_YMAG:
    return 1;
  }
  return -1;
}

/*********************************************************************
*
*       GUIDRV__TemplateI_GetRect
*/
void GUIDRV__TemplateI_GetRect(GUI_DEVICE * pDevice, LCD_RECT * pRect) {
  DRIVER_CONTEXT * pContext;

  pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
  pRect->x0 = 0;
  pRect->y0 = 0;
  pRect->x1 = pContext->vxSize - 1;
  pRect->y1 = pContext->vySize - 1;
}

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
//
// TBD: Add functions to be used for function pointers of MANAGE_VMEM_API
//

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUIDRV_TemplateI_Config
*/
void GUIDRV_TemplateI_Config(GUI_DEVICE * pDevice, CONFIG_TEMPLATE_I * pConfig) {
  DRIVER_CONTEXT * pContext;

  if (pDevice) {
    pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
    if (pContext) {
      pContext->FirstCOM  = pConfig->FirstCOM;
      pContext->FirstSEG  = pConfig->FirstSEG;
      pContext->pfInit    = _Init_XXX;
    }
  }
}

/*********************************************************************
*
*       GUIDRV_TemplateI_SetBus_XXX
*/
void GUIDRV_TemplateI_SetBus_XXX(GUI_DEVICE * pDevice, GUI_PORT_API * pHW_API) {
  DRIVER_CONTEXT * pContext;

  if (pDevice) {
    pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
    if (pContext) {
      //
      // Get function pointers
      //
      pContext->HW_API = *pHW_API;
      //
      // Set routine for checking interface routines
      //
      pContext->pfCheck = _Check_XXX_BitBus;
      //
      // TBD: Set up function pointers of MANAGE_VMEM_API
      //
    }
  }
}


#else

void GUIDRV_TemplateI_C(void);   // Avoid empty object files
void GUIDRV_TemplateI_C(void) {}

#endif

/*************************** End of file ****************************/
