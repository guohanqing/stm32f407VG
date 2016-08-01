/*
*********************************************************************************************************
*	                                  
*	模块名称 : GUI界面主函数
*	文件名称 : MainTask.c
*	版    本 : V1.0
*	说    明 : Shows how to use a numpad as input device on a touch screen
*	修改记录 :
*		版本号    日期          作者                 说明
*		v1.0    2013-04-22    Eric2013      ST固件库版本 V1.0.2版本。
*
*	Copyright (C), 2013-2014
*   QQ超级群：216681322
*   BLOG: http://blog.sina.com.cn/u/2565749395 
*********************************************************************************************************
*/

#include "MainTask.h"


int i;
char acText[] = "www.armfly.com  www.armfly.taobao.com Eric2013";
GUI_RECT Rect = {10, 10, 59, 59};
GUI_WRAPMODE aWm[] = {GUI_WRAPMODE_NONE,
                      GUI_WRAPMODE_CHAR,
                      GUI_WRAPMODE_WORD};
/*
*********************************************************************************************************
*	函 数 名: MainTask
*	功能说明: GUI主函数
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void MainTask(void) 
{
    GUI_Init();
    GUI_SetTextMode(GUI_TM_TRANS);
    for (i = 0; i < 3; i++) 
    {
        GUI_SetColor(GUI_BLUE);
        GUI_FillRectEx(&Rect);
        GUI_SetColor(GUI_WHITE);
        GUI_DispStringInRectWrap(acText, &Rect, GUI_TA_LEFT, aWm[i]);
        Rect.x0 += 60;
        Rect.x1 += 60;
    }
    while (1)
    {
        GUI_Delay(10);
    }
}

