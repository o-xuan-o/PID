#ifndef __PID_H
#define __PID_H	
#include "sys.h"
#include "usart.h"
#include "math.h"
#include "stdlib.h"
typedef struct _pid
{
    float SetSpeed;            //�����趨ֵ
    float ActualSpeed;        //����ʵ��ֵ
    float err;                //����ƫ��ֵ
    float err_next;            //������һ��ƫ��ֵ
		float err_last;            //��������ǰ��ƫ��ֵ
    float Kp,Ki,Kd;            //������������֡�΢��ϵ��
    float voltage; //�����ѹֵ������ִ�����ı�����
		float integral;         
 
}pid_InitTypeDef;

void PID_init(void);
float PID_realize(float,float);


#endif
