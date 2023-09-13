#include <Arduino.h>

hw_timer_t *timer0 = NULL;
hw_timer_t *timer1 = NULL;

volatile unsigned long long WiFiRunTimeTicks = 0;
volatile unsigned long long MovementRunTimeTicks = 0;
volatile bool flag_wifi = false;
volatile bool flag_move = false;


// 定义定时器中断处理函数
void IRAM_ATTR timer0ISR()
{
  if (flag_wifi) WiFiRunTimeTicks++;
  else {
    //Serial.println(WiFiRunTimeTicks);
    WiFiRunTimeTicks = 0;
  }
}

void IRAM_ATTR timer1ISR()
{
  if (flag_move) MovementRunTimeTicks++;
  else {
    MovementRunTimeTicks = 0;
  }
}

void ConfigureTimerForWiFi(void) {
  // 初始化定时器0
  timer0 = timerBegin(0, 80, true); // Timer0，预分频80，计时器模式为上升沿计数
  timerAttachInterrupt(timer0, &timer0ISR, true); // 绑定定时器0中断处理函数
  timerAlarmWrite(timer0, 100, true); // 定时器0每秒触发一次中断
  timerAlarmEnable(timer0); // 启用定时器0中断
}

void ConfigureTimerForMovement(void) {
  // 初始化定时器1
  timer1 = timerBegin(1, 80, true); // Timer1，预分频80，计时器模式为上升沿计数
  timerAttachInterrupt(timer1, &timer1ISR, true); // 绑定定时器1中断处理函数
  timerAlarmWrite(timer1, 100, true); // 定时器1每秒触发一次中断
  timerAlarmEnable(timer1); // 启用定时器1中断
}


