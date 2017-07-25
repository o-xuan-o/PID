#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "adc.h"
#include "pwm.h"
#include "PID.h"
#include "remote.h"
#include <math.h>

u8 flag,key_count,key=0;
u8 key_val[3];

float speed_Angle=30;//���õĽǶ�
extern pid_InitTypeDef pid;

void key_display(u8);
void Remote_display(void);

 int main(void)

 {	 
	u16 adcx;//ADC
	u32 PWM;//PWMֵ
	float Tar_val;//���ص�ʵ�ʵ�ѹֵ
	float Voltage;//��ѹֵ
	u32 Vol_display;//��ʾ��ѹ
	float Actual;//ʵ�ʽǶ�
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(9600);	 	//���ڳ�ʼ��Ϊ9600
 	LED_Init();			     //LED�˿ڳ�ʼ��
	LCD_Init();			 	
 	Adc_Init();		  		//ADC��ʼ��
	Remote_Init();			//������ճ�ʼ��
	PID_init();
	pwm_init();
	//��ʾ��ʾ��Ϣ
	POINT_COLOR=BLUE;//��������Ϊ��ɫ
	LCD_ShowString(30,110,200,16,16,"ADC_CH0_VAL:");	      
	LCD_ShowString(30,130,200,16,16,"ADC_CH0_VOL:0.000V");	

//	LCD_ShowString(30,130,200,16,16,"KEYVAL:");	 
	LCD_ShowString(30,150,200,16,16,"TARBOL:");	
	LCD_ShowString(30,170,200,16,16,"SYMBOL:");
	
	LCD_ShowString(30,190,200,16,16,"SET_Angle:000");	 
	LCD_ShowString(30,210,200,16,16,"REAL_Angle:000");	 
	while(1)
	{
		//�Ӻ��������ж�ȡ�û����õ�ֵ
		Remote_display();
	
		//ADC��ȡ��ֵ����ɽǶ�
		adcx=Get_Adc_Average(ADC_Channel_1,1);
		LCD_ShowxNum(126,110,adcx,4,16,0);//��ʾADC��ֵ
		Voltage=(float)adcx*(3.3/4095);
		
		//����ѹת���ɽǶ�
		
		//Actual=acos((2.58-Voltage)/0.75)*180/3.1415-20;//90-(asin((2.6-Voltage)/0.7))*180/3.1416;
		Actual=90*Voltage-72;
		//��ʼPID��ȡ��Ҫ�Ľ���ֵ
		Tar_val=PID_realize(speed_Angle,Actual);//�����������Ƕ�
		LCD_ShowxNum(30,230,Tar_val,4,16,0x80);
		PWM=Tar_val*150;//100-50��  120-55��  
		//���Ľ���ֵ�����PWM���
		TIM_SetCompare2(TIM3,20000-PWM);//����TIMx����Ƚ�2�Ĵ���ֵ
		LCD_ShowxNum(118,230,PWM,5,16,0x80);
		//��ʾ��ѹֵ
		Vol_display=Voltage*1000;
		LCD_ShowxNum(126,130,Vol_display/1000,1,16,0x80);//��ʾADC��ֵ
		LCD_ShowxNum(126+16,130,Vol_display%1000,3,16,0x80);//��ʾADC��ֵ
		//��ʾ�趨�Ƕ�ֵ
		LCD_ShowxNum(110,190,speed_Angle,3,16,0x80);
		
		//��ʾʵ�ʽǶ�ֵͨ���������ĵ�ѹֵ���л���
		LCD_ShowxNum(118,210,Actual,3,16,0x80);
			delay_ms(50);
			
	}
 }
 

void key_display(u8 key)
{
	switch(key)
	{	
//		case 162:str="CH-";flag=1;break;//POWER  
//		case 98:str="CH";flag=1;break;//UP    
//		case 2:str="RIGHT";flag=1;break;//PLAY 
//		case 226:str="CH+";flag=1;break;//ALIENTEK
		case 194:flag=10;break;//RIGHT
//		case 34:str="LEFT";flag=1;break;	//	  
//		case 224:str="VOL-";flag=1;break;//		  
//		case 168:str="VOL+";flag=1;break;//DOWN	   
		case 144:flag=11;break;//VOL+    
		case 104:flag=0;break;//1  
//		case 152:str="100+";flag=1;break;//2  
//		case 176:str="200+";flag=1;break;//3  
		case 48:flag=1;break;//4
		case 24:flag=2;break;//5
		case 122:flag=3;break;//6
		case 16:flag=4;break;//7					
		case 56:flag=5;break;//8
		case 90:flag=6;break;//9
		case 66:flag=7;break;//0
		case 74:flag=8;break;//ERROR 
		case 82:flag=9;break;//DELETE 
//		default :flag=1; break; 
	}
}

void Remote_display(void)
{
	u8 i=0,j=0;
	key=Remote_Scan();
	//�а�������
	if(key)
	{
		key_display(key);
		if(flag==10)//PLAY��
		{
			for(j=0;j<key_count;j++)
			{
				LCD_ShowNum(86+j*8,150,key_val[j],1,16);
			}	
			speed_Angle=key_val[0]*100+key_val[1]*10+key_val[2];//�û����õĽǶ�ֵ
			key_count=0;
		}
		if(flag==11)//EQ��
		{
			LCD_Fill(86,150,116+8*8,150+16,WHITE);	//���֮ǰ����ʾ
			LCD_Fill(86,170,116+8*8,170+16,WHITE);	//���֮ǰ����ʾ
			key_count=0;
		}
		if((flag>=0)&&(flag<=9))//���ּ�
		{
			if(key_count==3) key_count=0;
			key_val[key_count++]=flag;
			for(i=0;i<key_count;i++)
			{
				LCD_ShowNum(86+i*8,170,key_val[i],1,16);
			}
		}
	}
	else delay_ms(10);
//	while(Remote_Scan());
//		delay(10);
	while(Remote_Scan());
	delay_ms(10);
}

