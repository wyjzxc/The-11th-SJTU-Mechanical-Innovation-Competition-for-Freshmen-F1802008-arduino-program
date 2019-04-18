/*
   Motor_L298N.h 用于使用Arduino uno 通过L298N电机驱动板（红板）控制两个直流电机
   2017.2.13 创建，创建者：Daivs2016,nowshow
   公共领域
*/

#ifndef Motor_L298N_h
#define Motor_L298N_h

#include "Arduino.h"

class Motor_L298N
{
  public:
    Motor_L298N(int pin1,int pin2,int pwmpin);
    void L298N_run(int state,int val);
  private:
    int _pin1;
    int _pin2;
    int _pwmpin;
  };
#endif
