#include <Arduino.h>
#include "normal_mode.h"
#include "calibration.h"
#include "debug.h"
#include "hexapod.h"

#include <std_msgs/msg/int32.h>
#include <micro_ros_platformio.h>
#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <std_msgs/msg/int32.h>  //pub-sub的消息接口
#include <string.h>
#include <vector>
#include <functional>

//#include <esp32-hal-cpu.h>
#include "timer.h"
#include "rtosQueue.h"
#define LED_PIN 2
#define WIFI_ADDRESS "192.168.150.184"
#define SSID "SSID-A7FB6D"
#define PASSWD "NDSXrNf6"
#define NODE_NAME "hexapod_control1"

static int _mode = 1;
volatile unsigned long ulHighFrequencyTimerTicks;

rclc_executor_t exc;
rclc_support_t sup;
rcl_allocator_t allc;
rcl_node_t node;

// 声明话题订阅者
rcl_subscription_t test_sub; 
rcl_subscription_t scale_sub;
rcl_subscription_t offset_sub;
rcl_subscription_t move_sub;

// 声明消息文件
std_msgs__msg__Int32 test_msg;
std_msgs__msg__Int32 scale_msg;
std_msgs__msg__Int32 offset_msg;
std_msgs__msg__Int32 move_msg;

void error_loop(){
  while(1){
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    delay(100);
  }
}

void callback_subscription_offset(const void *msgin)
{
  const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32 *)msgin; 
  int offset_ = msg->data; //msg->data的类型为rosidl_runtime_c__int32__Sequence，是一个结构体，里面的data元素是int型
  if (DEBUG) {
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
  }
  //int scale = msg->data.data[1]; 
  calibrate(offset_, 0);  //msg的第一个和第二个data就是offset和scale
  //normal_loop(12); //for test
}
void callback_subscription_test(const void *msgin) {
  const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32*) msgin;
  //normal_loop(12);
  calibrate_test(msg->data);
  if (DEBUG) {
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
  }

}

void callback_subscription_scale(const void *msgin) {
  const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32*) msgin;
  int scale = msg->data;
  calibrate(0, scale); 
  if (DEBUG) {
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
  }
  //normal_loop(12);  
}

void callback_subscription_move(const void *msgin) {
    const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32*) msgin;
    //normal_loop(msg->data); //单任务使用
    //hexapod::Hexapod.setMode(msg->data);
    xQueueSend(controlMsg, &msg->data, 5);
    
    if (DEBUG) {
      Serial.printf("Send %d\n", msg->data);
      digitalWrite(LED_PIN, HIGH);
      delay(200);
      digitalWrite(LED_PIN, LOW);
    }
}

static void log_output(const char* log) {  //log: 串口信息，用Serial对象的println打印
  Serial.println(log);
}

//这些放到.h文件中会出现multipudefine 问题，因为main文件中也引入了头文件
//RosControl rc(srv, exc, sup, allc, node, req, res, service_callback); //全局的ros控制对象

void microRosControl_wifi(void *pvParam) {
 

    Serial.begin(115200);
    //delay(2000);
    pinMode(LED_PIN, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.printf("Task running on core %d\n", xPortGetCoreID());  
    Serial.print("Started, mode=");
    Serial.println(_mode);

    /* 初始化timer */
    // ConfigureTimerForWiFi();

    /* 初始化WiFi通信 */
    IPAddress agent_ip;
    agent_ip.fromString(WIFI_ADDRESS);
    set_microros_wifi_transports(SSID, PASSWD, agent_ip, 8888);
    delay(2000);


    allc = rcl_get_default_allocator();
    // 创建初始化选项
    RCCHECK(rclc_support_init(&sup, 0, NULL, &allc));
    // 创建节点 hello_microros
    RCCHECK(rclc_node_init_default(&node, NODE_NAME, "", &sup)); 

    /* 初始化校正订阅节点 */
    
    RCCHECK(rclc_subscription_init_default(
        &offset_sub,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32), //这里的展开是rosidl_typesupport_c__get_message_type_support_handle__<package name>__msg__<message name>
        "/servo_offset"));

    /* 初始化运动订阅节点 */
    
    RCCHECK(rclc_subscription_init_default(
        &scale_sub,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
        "/servo_scale"));
    
    /* 初始化测试订阅节点 */

    RCCHECK(rclc_subscription_init_default(
        &test_sub,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
        "/servo_test"));

    RCCHECK(rclc_subscription_init_default(
        &move_sub,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
        "/servo_move"));

    /* 初始化 */
    RCCHECK(rclc_executor_init(&exc, &sup.context, 4, &allc));  //rclc_excutor_init, able to handle 4 topics
    
    RCCHECK(rclc_executor_add_subscription(&exc, &test_sub, &test_msg, &callback_subscription_test, ON_NEW_DATA));
    RCCHECK(rclc_executor_add_subscription(&exc, &offset_sub, &offset_msg, &callback_subscription_offset, ON_NEW_DATA));
    RCCHECK(rclc_executor_add_subscription(&exc, &scale_sub, &scale_msg, &callback_subscription_scale, ON_NEW_DATA));
    RCCHECK(rclc_executor_add_subscription(&exc, &move_sub, &move_msg, &callback_subscription_move, ON_NEW_DATA));
    // char InfoBuffer[100];
    while(1) {
      flag_wifi = true;
      // vTaskGetRunTimeStats(InfoBuffer);
      // Serial.printf("%s/r/n", InfoBuffer);
      RCCHECK(rclc_executor_spin_some(&exc, RCL_MS_TO_NS(100)));
      if (DEBUG) {
        Serial.printf("core %d, task wifi, time ", xPortGetCoreID());
        Serial.println(millis());
        Serial.println("");
      }
      // if (WiFiRunTimeTicks > 0) {
      //     Serial.printf("wifi task:");
      //     Serial.println(WiFiRunTimeTicks);
      // }
      // // Serial.printf("wifi task:");
      // // Serial.println(WiFiRunTimeTicks);

      // flag_wifi = false;
    }
}
