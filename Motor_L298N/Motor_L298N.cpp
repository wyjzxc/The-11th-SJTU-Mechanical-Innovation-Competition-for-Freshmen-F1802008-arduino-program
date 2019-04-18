/*
   Motor_L298N.cpp 用于使用Arduino uno 通过L298N电机驱动板（红板）控制两个直流电机
   2017.2.13 创建，创建者：Daivs2016,nowshow
   公共领域
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
    if(state==1)                             //当state为1时正转
    {
      analogWrite(_pwmpin,val);
      digitalWrite(_pin1,1);
      digitalWrite(_pin2,0);
    }
    else if(state==2)                     //当state为2时反转
    {
      analogWrite(_pwmpin,val);
      digitalWrite(_pin1,0);
      digitalWrite(_pin2,1);
    }
    else if(state==0)                     //当state为0时停止
    {
      analogWrite(_pwmpin,0);
      digitalWrite(_pin1,0);
      digitalWrite(_pin2,0);
    }
}

