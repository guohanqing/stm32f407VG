#include <rthw.h>
#include "GUI.h"
#include "GUIDEMO.h"
#include "MainTask.h"
#include "drv_lcd_st7735.h"
#define RT_GUI_NUMBYTES 1024*4
static struct rt_thread emwin_thread;
static char emWin_thread_stack[RT_GUI_NUMBYTES];
#if 1
void emWin_thread_entry(void *parameter)
{
	//WM_SetCreateFlags(WM_CF_MEMDEV);
	
	//st7735_Init();
	//Lcd_Clear(YELLOW);
	
	//GUI_Init();

	rt_kprintf("\nGUIINIT\n");
	
	while(1)
	{
		
		//GUIDEMO_Main();
		//rt_thread_delay(1000);
		Lcd_Clear(RED);
	//GUI_SetBkColor(GUI_BLUE);
	//GUI_Clear();
	//GUI_SetColor(GUI_WHITE); 
	//GUI_SetFont(GUI_FONT_8X16_ASCII);
	//GUI_DispStringAt("11012",0,0);
		MainTask();
		//rt_thread_delay(10);
	}
}

int emwin_system_init(void)
{
    rt_err_t result;
	  //rt_device_t device;

		//GUI_X_InitOS();
	 /* device = rt_device_find("lcd");
		if(device == RT_NULL)
		{
		 rt_kprintf("no graphic device in the system. \n");
		
		}*/
	  st7735_Init();
	  Lcd_Clear(RED);
	  GUI_Init();
    result = rt_thread_init(&emwin_thread,
        "emwin",
        emWin_thread_entry, RT_NULL,
        &emWin_thread_stack[0], sizeof(emWin_thread_stack),
        18, 30);
				
			/*emwin_thread = rt_thread_create("emwin",
                            emWin_thread_entry,
                             RT_NULL,
                             RT_GUI_NUMBYTES,
                             21,
                             30);*/

    if (result == RT_EOK)
        rt_thread_startup(&emwin_thread);
    return 0;
}
INIT_COMPONENT_EXPORT(emwin_system_init);

#endif