/*
   Motor_L298N.cpp ����ʹ��Arduino uno ͨ��L298N��������壨��壩��������ֱ�����
   2017.2.13 �����������ߣ�Daivs2016,nowshow
   ��������
*/

#include "Arduino.h"
#include "Motor_L298N.h"

Motor_L298N::Motor_L298N(int pin1,int pin2,int pwmpin)
{
    _pin1=pin1;
    _pin2=pin2;
    _pwmpin=pwmpin;
}

void Motor_L298N::L298N_run(int state,int val)
{
    pinMode(_pin1,OUTPUT);
    pinMode(_pin2,OUTPUT);
    if(state==1)                             //��stateΪ1ʱ��ת
    {
      analogWrite(_pwmpin,val);
      digitalWrite(_pin1,1);
      digitalWrite(_pin2,0);
    }
    else if(state==2)                     //��stateΪ2ʱ��ת
    {
      analogWrite(_pwmpin,val);
      digitalWrite(_pin1,0);
      digitalWrite(_pin2,1);
    }
    else if(state==0)                     //��stateΪ0ʱֹͣ
    {
      analogWrite(_pwmpin,0);
      digitalWrite(_pin1,0);
      digitalWrite(_pin2,0);
    }
}

