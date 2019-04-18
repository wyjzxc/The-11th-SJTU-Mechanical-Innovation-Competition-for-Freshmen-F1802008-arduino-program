/*
   Motor_L298N.h ����ʹ��Arduino uno ͨ��L298N��������壨��壩��������ֱ�����
   2017.2.13 �����������ߣ�Daivs2016,nowshow
   ��������
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
