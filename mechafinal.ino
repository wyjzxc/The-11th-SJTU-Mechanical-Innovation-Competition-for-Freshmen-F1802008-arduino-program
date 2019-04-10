#include <PS2X_lib.h>
#include <Motor_L298N.h>
#include <LobotServoController.h>
#include <math.h>

LobotServoController Controller;
Motor_L298N MotorLeft(6,11,3);
Motor_L298N MotorRight(7,10,5);
PS2X ps2x;
#define pi 3.141592654
struct PS2DATA
{
  boolean UPPRESSED;
  boolean DOWMPRESSED;
  boolean LEFTPRESSED;
  boolean RIGHTPRESSED;
  boolean SELECTPRESSED;
  boolean STARTPRESSED;
  boolean GREENDELTAPRESSED;
  boolean REDSQUAREPRESSED;
  boolean REDCIRCLEPRESSED;
  boolean BLUECROSSPRESSED;
  boolean L1PRESSED;
  boolean L2PRESSED;
  boolean R1PRESSED;
  boolean R2PRESSED;
  boolean L3PRESSED;
  boolean R3PRESSED;
  int LX;
  int LY;
  int RX;
  int RY;
};

PS2DATA joystick;
PS2DATA *ps2 = &joystick;

#define PS2_DAT 8
#define PS2_CMD 9
#define PS2_ATT 2
#define PS2_CLK 4

const double l0 = 1.0;
const double l1 = 10.3;
const double l2 = 19.4;
const double l3 = 10.0;

int error=0;
int vibrate = 0;

boolean servobusy = false;
boolean servobusy_reset = false;
bool handleTimePeriod( unsigned long *ptr_time, unsigned int time_period) {
    if((millis() - *ptr_time) < time_period) {
        return 1;  
    }else{
         *ptr_time = millis();
         return 0;
    }
}

void ReadPS2( PS2DATA *ps2 )
{
  static unsigned long systick_ms_bak = 0;
  if(handleTimePeriod(&systick_ms_bak, 10))return;  
  ps2x.read_gamepad(false, vibrate);
  vibrate = ps2x.Analog(PSAB_BLUE);
  ps2->SELECTPRESSED = ps2x.Button(PSB_START);
  ps2->STARTPRESSED = ps2x.Button(PSB_SELECT);
  ps2->UPPRESSED = ps2x.Button(PSB_PAD_UP);
  ps2->RIGHTPRESSED = ps2x.Button(PSB_PAD_RIGHT);
  ps2->LEFTPRESSED = ps2x.Button(PSB_PAD_LEFT);
  ps2->DOWMPRESSED = ps2x.Button(PSB_PAD_DOWN);
  ps2->L3PRESSED = ps2x.Button(PSB_L3);
  ps2->R3PRESSED = ps2x.Button(PSB_R3);
  ps2->L2PRESSED = ps2x.Button(PSB_L2);
  ps2->R2PRESSED = ps2x.Button(PSB_R2);
  ps2->L1PRESSED = ps2x.Button(PSB_L1);
  ps2->R1PRESSED = ps2x.Button(PSB_R1);
  ps2->GREENDELTAPRESSED = ps2x.ButtonPressed(PSB_TRIANGLE);
  ps2->REDCIRCLEPRESSED = ps2x.ButtonPressed(PSB_CIRCLE);
  ps2->REDSQUAREPRESSED = ps2x.ButtonPressed(PSB_SQUARE);
  ps2->BLUECROSSPRESSED = ps2x.Button(PSB_CROSS);
  ps2->LX = ps2x.Analog(PSS_LX);
  ps2->RX = ps2x.Analog(PSS_RX);
  ps2->LY = ps2x.Analog(PSS_LY);
  ps2->RY = ps2x.Analog(PSS_RY);
}

void DriveMotor(PS2DATA *ps2)
{
  double tmp=0;
  double speedy=0,speedx=0,speedl=0,speedr=0;
  boolean flag=false;
  if(ps2->LY>=160)
  {
    speedy = ps2->LY;
    flag = true;
  }
  if(ps2->LY<=96)
  {
    speedy = ps2->LY-255;
    flag = true;
  }

  if(ps2->LX>=160)
  {
    speedx = ps2->LX;
    flag = true;
  }
  if(ps2->LX<=96)
  {
    speedx = ps2->LX - 255;
    flag = true;
  }
if(flag){
  speedl = speedy - speedx;
  if( speedl > 255 ) speedl = 255;
  if( speedl < -255 ) speedl = -255;
  speedr = speedy + speedx;
  if( speedr > 255 ) speedr = 255;
  if( speedr < -255 ) speedr = -255;
  if(speedl > 0)
  MotorLeft.L298N_run(1,speedl);
  if(speedl < 0)
  MotorLeft.L298N_run(2,-speedl);
  if(speedl == 0)
  MotorLeft.L298N_run(0,0);
  if(speedr > 0)
  MotorRight.L298N_run(1,speedr);
  if(speedr < 0)
  MotorRight.L298N_run(2,-speedr);
  if(speedr == 0)
  MotorRight.L298N_run(0,0);
}
else
{
  MotorLeft.L298N_run(0,0);
  MotorRight.L298N_run(0,0);
}
}

void ControlShovel(PS2DATA *ps2)
{
  static boolean waiting = false;
  static unsigned long systick_ms_bak = 0;
  if(handleTimePeriod(&systick_ms_bak, 50))return; 
  if(waiting)
  {
  static unsigned long systick_ms_bak2 = 0;
  if(handleTimePeriod(&systick_ms_bak2, 100))return;   
  waiting = false;
  }
  static int mode = 0;
  if(ps2->R1PRESSED) {
    mode += 100;
    if(mode>1100)mode=1100;
    waiting = true;
  }
  if(ps2->R2PRESSED) {
    mode -=100;
    if(mode<0)mode=0;
    waiting = true;
  }
  Controller.moveServo(0,500+mode,100);
}


void ControlServo(PS2DATA *ps2,int speedms)
{
  static unsigned long systick_ms_bak = 0;
  if(handleTimePeriod(&systick_ms_bak, 10))return;  
  static double prex = 25;
  static double prey = 0;
  static double prez = 0;
  static double prea = 90;
  static boolean pos_flag=0;
  static boolean state=true;
  double x;
  double y;
  double z;
  double a;
  if(ps2->REDSQUAREPRESSED){
    pos_flag=~pos_flag;
      if(pos_flag){
        prex=15;
        prez=15;
        }
      if(!pos_flag){
        prex=5;
        prez=10;
        }
    }
  if(!ps2->R3PRESSED)
  {
    x = prex + (127-ps2->RY)*0.005;
    y = prey + (128-ps2->RX)*0.05;
    z = prez;
    a = prea;
  }
  else
  {
    x = prex;
    y = prey;
    z = prez + (127 - ps2->RY)*0.002;
    a = prea + (ps2->RX-128)*0.01;
  }

  if((x<10)||(z<-10)||(a<=0)||(a>=180)||(x > 30))
  {
    x = prex;
    y = prey;
    z = prez;
    a = prea;
    return;
  }

  
  double a11,a12,a21,a22,atmp;
  double tmp,tmpa,tmpb;
  if(y>90||y<-180)
  {
    x = prex;
    y = prey;
    z = prez;
    a = prea;
    return;
  }
  tmpa = x + l0 + l3 * cos(a/180*pi);
  tmpb = z + l3 * sin(a/180*pi);
  tmp = ( tmpa * tmpa + tmpb * tmpb - l1 * l1 + l2 * l2 )/( 2 * l2 * sqrt( tmpa * tmpa + tmpb * tmpb ));

double tritmp;

  atmp = 180.0/pi * atan2(tmpa,tmpb);
  if(tmp>=0){
    if(tmp<=1)
    {
  a21 = 180.0/pi * asin(tmp) - atmp;
  a22 = 180-180.0/pi * asin(tmp)-atmp;
    }
  else {
    a21 = 90 - atmp;
    a22 = 90 - atmp;
  }
  
  tritmp = (tmpa - l2 * cos(a21/180*pi))/l1;
  if(tritmp>1)
  a11 = 0;
  else if(tritmp<-1)
  a11 = 180;
  else
  a11 = 180/pi*acos(tritmp);
  tritmp = (tmpa - l2 * cos(a22/180*pi))/l1;
  if(tritmp>1)
  a12 = 0;
  else if(tritmp<-1)
  a12 = 180;
  else
  a12 = 180/pi*acos(tritmp);
}
  else
  {
    if(tmp>=-1)
    {
    a21 = 180/pi *asin(tmp) -atmp;
    a22 = -180/pi *asin(tmp) -180 -atmp;
    }
    else 
    {
      a21 = -90-atmp;
      a22 = -270 - atmp;
    }
    
  tritmp = (tmpa - l2 * cos(a21/180*pi))/l1;
  if(tritmp>1)
  a11 = 0;
  else if(tritmp<-1)
  a11 = 180;
  else
  a11 = 180 / pi * acos(tritmp);
  
  tritmp = (tmpa - l2 * cos(a22/180*pi))/l1;
  if(tritmp>1)
  a12 = 0;
  else if(tritmp<-1)
  a12 = 180;
  else
  a12 = 180 / pi *acos(tritmp);
  }
  boolean flag1,flag2;
  flag1 = ((a11>5) && (a11<175) && ((a11-a21-9)>-135)&&((a11-a21-9)<135)&&((a - a21 - 90)>-90)&&((a-a21-90)<90));
  flag2 = ((a12>5) && (a12<175) && ((a12-a22-9)>-135)&&((a12-a22-9)<135)&&((a - a22 - 90)>-90)&&((a-a22-90)<90));
  if(flag1&&!flag2)
  servogopos(y,a11,a21,a,speedms);
  if(!flag1&&flag2)
  servogopos(y,a12,a22,a,speedms);
  if(flag1&&flag2)
    if(a21<=a22)
      servogopos(y,a11,a21,a,speedms);
     else
      servogopos(y,a21,a22,a,speedms);
  if(!flag1&&!flag2)
  {
    x = prex;
    y = prey;
    z = prez;
    a = prea;
    return;
  }
  prex = x;
  prey = y;
  prez = z;
  prea = a;
  }

void gopos(int num,double angle,int speedms){
  int a = 1500+200.0/27*angle;
  Controller.moveServo(num,a,speedms);
}
void servogopos(const double &a,const double &b,const double &c,const double &d, int speedms){
  gopos(1,a+45,speedms);
  gopos(2,b - 90,speedms);
  gopos(3,b-c-9,speedms);
  gopos(4,180+c-d,speedms);
}


void setup() {
    Serial.begin(9600);
    delay(300);
    error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_ATT, PS2_DAT);
}

void loop() {
  if(error == 1) //skip loop if no controller found
  {
    error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_ATT, PS2_DAT);
    return;
  }
  ReadPS2(ps2);
  DriveMotor(ps2);
  ControlShovel(ps2);
  if(!servobusy)ControlServo(ps2,5);
  catcha(ps2);
  storage();
}
void FastReset(PS2DATA *ps2){
  static bool res_pos=0;
  static int stepn_1 = 0;
  static boolean flag_1 = false;
  static unsigned long systick_ms_bak1_1 = 0;
  if(ps2->REDSQUAREPRESSED&&(!flag_1)){
    flag_1 = true;
    servobusy_reset = 1;
    systick_ms_bak1_1 = millis();
  }
  if(flag_1&&!res_pos)
  {
    if(stepn_1 == 0){
    Controller.moveServo(5,1400,200);
    Controller.moveServo(12,1500,200);
  if(handleTimePeriod(&systick_ms_bak1_1, 500))return;
  ++stepn_1;
  }
  if(stepn_1 == 1)
  {
   Controller.moveServo(2,2000,200);
   Controller.moveServo(3,2366,200);
  if(handleTimePeriod(&systick_ms_bak1_1, 600))return;
  ++stepn_1;
  }
  if(stepn_1 ==2){
    Controller.moveServo(12,2000,200);
    if(handleTimePeriod(&systick_ms_bak1_1, 600))return;
  ++stepn_1;
}
  if(stepn_1 == 3)
  {
    //if(handleTimePeriod(&systick_ms_bak1_1, 300))return;
    flag_1 = false;
    servobusy_reset = false;
    stepn_1 = 0;
    res_pos=1;
  }
  }
  if(flag_1&&res_pos)
  {
    if(stepn_1 == 0){
    //Controller.moveServo(5,1400,200);
    Controller.moveServo(12,1500,200);
  if(handleTimePeriod(&systick_ms_bak1_1, 300))return;
  ++stepn_1;
  }
  if(stepn_1 == 1)
  {
   Controller.moveServo(2,1300,200);
   Controller.moveServo(3,2000,200);
  if(handleTimePeriod(&systick_ms_bak1_1, 600))return;
  ++stepn_1;  
  }
  if(stepn_1 ==2){
    Controller.moveServo(12,2000,200);
    if(handleTimePeriod(&systick_ms_bak1_1, 1000))return;
  ++stepn_1;
}
  if(stepn_1 == 3)
  {
    //if(handleTimePeriod(&systick_ms_bak1_1, 300))return;
    flag_1 = false;
    servobusy_reset = false;
    stepn_1 = 0;
    res_pos=0;
  }
  }
}
void catcha(PS2DATA *ps2){
  static int t=0;
  static int stepn = 0;
  static boolean flag = false;
  static unsigned long systick_ms_bak1 = 0;
  if(ps2->REDCIRCLEPRESSED&&(!flag)){
    flag = true;
    servobusy = 1;
  t=(t+1)%3;
   systick_ms_bak1 = millis();
  }
  if(flag)
  {
    if(stepn == 0){
   Controller.moveServo(5,1700,200);
  if(handleTimePeriod(&systick_ms_bak1, 400))return;
  ++stepn;
  }
  if(stepn == 1)
  {
  Controller.moveServo(2,1500,400);
  Controller.moveServo(3,1490,400);
  Controller.moveServo(4,2250,400);
  if(handleTimePeriod(&systick_ms_bak1, 1000))return;
  ++stepn;  
  }
  if(stepn ==2){
  switch(t){
    case 0:Controller.moveServo(1,750,800);break;
    case 1:Controller.moveServo(1,650,800);break;
    case 2:Controller.moveServo(1,550,800);break;
  }
    if(handleTimePeriod(&systick_ms_bak1, 1500))return;
  ++stepn;
}
  if(stepn==3)
  {
    Controller.moveServo(5,1200,200);
    if(handleTimePeriod(&systick_ms_bak1, 400))return;
    ++stepn;
  }
  if(stepn == 4)
  {
    Controller.moveServo(1,2000,200);
    if(handleTimePeriod(&systick_ms_bak1, 400))return;
    ++stepn;
  }
  if(stepn == 5)
  {
    ControlServo(ps2,1000);
    if(handleTimePeriod(&systick_ms_bak1, 400))return;
    ++stepn;
    flag = false;
    servobusy = false;
    stepn = 0;
  }
  }
}

void storage(){
  if(!ps2x.Button(PSB_CROSS)) Controller.moveServo(6,1300,100);
  else Controller.moveServo(6,1800,100);
}
