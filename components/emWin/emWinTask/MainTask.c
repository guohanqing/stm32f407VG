#include <rthw.h>
#include "GUI.h"
#include "GUIDEMO.h"

#define RT_GUI_NUMBYTES 1024*20 
static struct rt_thread emwin_thread;
static char emWin_thread_stack[RT_GUI_NUMBYTES];

void emWin_thread_entry(void *parameter)
{
	WM_SetCreateFlags(WM_CF_MEMDEV);
	while(1)
	{
		GUIDEMO_Main();
	}
}

int emwin_system_init(void)
{
    rt_err_t result;


    result = rt_thread_init(&emwin_thread,
        "emwin",
        emWin_thread_entry, RT_NULL,
        &emWin_thread_stack[0], sizeof(emWin_thread_stack),
        19, 10);

    if (result == RT_EOK)
        rt_thread_startup(&emwin_thread);
    return 0;
}
INIT_COMPONENT_EXPORT(emwin_system_init);