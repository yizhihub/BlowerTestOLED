#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
//#include "oled.h"
#include "boled.h"

//Mini STM32开发板范例代码9
//OLED显示 实验
//正点原子@ALIENTEK
//技术论坛:www.openedv.com	
static u8 GBrightness = 0xFF;

 int main(void)
 {
 
	delay_init();	     //延时初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2
 	LED_Init();
    msDelay(200);
    OLED_Init();			//初始化OLED    
   // LCD_Init();     
//  OLED_ShowString(0,0,"ALIENTEK",24);  
//	OLED_ShowString(0,24, "0.96' OLED TEST",16);  
// 	OLED_ShowString(0,40,"ATOM 2014/3/7",12);  
// 	OLED_ShowString(0,52,"ASCII:",12);  
// 	OLED_ShowString(64,52,"CODE:",12);  
//	OLED_Refresh_Gram();//更新显示到OLED	 
//	t=' '; 
//    OLED_PutStr(0,OLED_LINE0, (uint8_t *)"abcdefghijklmnopqrstuvwxyz!@#$%^&*()_123456789~!", 6, RED);
     OLED_Fill(BLUE);
     OLED_Fill(BLACK);
     OLED_Fill(WHITE);
     OLED_Fill(BLACK);
 //    LCD_Fill(0,0,240,280,BLUE);
 //     LCD_Fill(0,0,240,280,RED);
    OLED_PutStr(0, OLED_LINE0, (uint8_t *)"123456789~!{}|AaBbCc", 6, GREEN);
    OLED_PutStr(0, OLED_LINE1, (uint8_t *)"Aa123456789~!{}|", 8, BLUE);
//    OLED_PutStr(0, OLED_LINE2, (uint8_t *)"Am12345", 16, BLUE);
    OLED_Print(64,  OLED_LINE3, "驱动测试", RED);
//    OLED_PutNumber(0, OLED_LINE4, 0, 2, 1, "℃", 6, GREEN);  // 92
//    OLED_PutNumber(0, OLED_LINE5, 0, 2, 1, "℃", 8, YELLOW); // 64
//    OLED_PutNumber(0, OLED_LINE6, 0, 2, 0, "℃", 16,GREEN);
    while(1) 
    {
        static float t = 3.14f;
//        OLED_ShowChar(36,52,t,12,1);//显示ASCII字符	
//        OLED_ShowNum(94,52,t,3,12);	//显示ASCII字符的码值    
//        OLED_Refresh_Gram();//更新显示到OLED
//        t++;
//        if(t>'~')t=' ';
        OLED_PutNumber(32, OLED_LINE2, t, 2, 1, 0, 6, GREEN);  // 92
        OLED_PutNumber(64, OLED_LINE2, t, 2, 1, "RPM", 8, YELLOW); // 64
        OLED_PutNumber(0, OLED_LINE2, t, 2, 0, 0, 16,GREEN);
        LED0=!LED0;
        t += 0.1;
        if (t > 160)  t = 0;
           
    }
}

       /*SysTick->LOAD = 0xFFFFFF;
        SysTick->VAL  = 0x00;
        SysTick->CTRL|= SysTick_CTRL_ENABLE_Msk ;
        OLED_Fill(BLACK);
        SysTick->CTRL&= ~SysTick_CTRL_ENABLE_Msk; 
        OLED_PutNumber(0, OLED_LINE7, 9000000.0f / (0xFFFFFF - SysTick->VAL), 3, 2, "FPS", 6, YELLOW);*/
