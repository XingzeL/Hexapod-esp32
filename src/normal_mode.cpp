#include <Arduino.h>

/*
 include micro_ros packages
*/
#include "debug.h"
#include "config.h"
#include "hexapod.h"
//#include "subscriber_setup.h"
#include "normal_mode.h"


#define REACT_DELAY hexapod::config::movementInterval
/*
  取自micro_ros_publisher
*/

static void log_output(const char* log) {
  Serial.println(log);
}

void normal_setup(void) {
  return;
}
void normal_loop(int _mode) {

  auto t0 = millis();
  auto flag = 12;  //MOVEMENT_TWIST,让机器人一直扭动
  auto mode = hexapod::MOVEMENT_STANDBY;  //auto定义的变量需要进行初始化，自动变成初始化时的数据类型
  
  for(auto i = _mode; i > 0 ; i--){
    mode++;
  }
  
  //digitalWrite(13, 0); //在这里微亮；去掉时间相关代码，常亮
 //处理得到的动作指令,机器人动作控制的关键函数
  hexapod::Hexapod.processMovement(mode, REACT_DELAY);
  //！！！！  这里的mode改成hexapod::MOVEMENT_TWIST，程序才会一直循环执行(有可能是因为flag设成了13（不是一个具体动作而是动作的数量），mode一直是standby，之后就不会调整舵机)
  //delay(1000);
  //digitalWrite(13, 0);  //去掉时间相关代码，常亮； 还原，微亮
 
  auto spent = millis() - t0;
  if(spent < REACT_DELAY) {
    // Serial.println(spent);
    delay(REACT_DELAY-spent);
  }
  else {
    //digitalWrite(13, 0);
    Serial.printf("spent: %d\n",spent);
  }
  
}