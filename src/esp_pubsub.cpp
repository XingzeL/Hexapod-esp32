#include <Arduino.h>

#include <std_msgs/msg/int32.h>
#include <micro_ros_platformio.h>
#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <std_msgs/msg/int32.h>  //pub-sub的消息接口
#include <string.h>

#define WIFI_ADDRESS "192.168.150.184"
#define SSID "SSID-A7FB6D"
#define PASSWD "NDSXrNf6"
#define NODE_NAME "hexapod_control1"
#define ESPTOPICNAME "/esp_topic1"
#define PCTOPICNAME "/pc_topic"

rclc_executor_t executor;

rcl_publisher_t publisher; //esp的发布者
rcl_subscription_t test_sub;

std_msgs__msg__Int32 test_msg;
std_msgs__msg__Int32 msg;

rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;

#define LED_PIN 13

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}


void error_loop(){
  while(1){
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    delay(100);
  }
}

void timer_callback(rcl_timer_t * timer, int64_t last_call_time)
{
  RCLC_UNUSED(last_call_time);
  if (timer != NULL) {
    RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));
    msg.data++;
  }
}

void esp_publiser(void * p) {
    IPAddress agent_ip;
    agent_ip.fromString(WIFI_ADDRESS);
    set_microros_wifi_transports(SSID, PASSWD, agent_ip, 8888);

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);

    delay(2000);

    
    allocator = rcl_get_default_allocator();

    //create init_options
    RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

    // create node
    RCCHECK(rclc_node_init_default(&node, NODE_NAME, "", &support));

    // create publisher
    RCCHECK(rclc_publisher_init_best_effort(
    &publisher,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
    ESPTOPICNAME));

    RCCHECK(rclc_executor_init(&executor, &support.context, 4, &allocator)); //需要在publisher之前初始化executor

    msg.data = 0;

    while(1) {
        RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));
        msg.data++;
        delay(2000);
    }
}

void callback_subscription_test(const void *msgin) {
    const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32*) msgin;
    //normal_loop(12);
    Serial.println(msg->data);
}

void esp_subscriber(void *pvParam) {
 

    Serial.begin(115200);
    delay(5000);  //这里需要延时一下
    pinMode(LED_PIN, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);

    // IPAddress agent_ip;
    // agent_ip.fromString(WIFI_ADDRESS);
    // set_microros_wifi_transports(SSID, PASSWD, agent_ip, 8888);
    // delay(2000);


    // allocator = rcl_get_default_allocator();
    // // 创建初始化选项
    // RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));
    // // 创建节点 hello_microros
    // RCCHECK(rclc_node_init_default(&node, NODE_NAME, "", &support)); 

    /* 初始化校正订阅节点 */
    
    
    /* 初始化测试订阅节点 */

    RCCHECK(rclc_subscription_init_default(
        &test_sub,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
        PCTOPICNAME));


    /* 初始化 */
    // RCCHECK(rclc_executor_init(&executor, &support.context, 4, &allocator));  //rclc_excutor_init, able to handle 4 topics
    
    RCCHECK(rclc_executor_add_subscription(&executor, &test_sub, &test_msg, &callback_subscription_test, ON_NEW_DATA));
    Serial.println("subscribe running");
    while(1) {

      // vTaskGetRunTimeStats(InfoBuffer);
      // Serial.printf("%s/r/n", InfoBuffer);
      RCCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100)));
      //Serial.println("subscribe running");
      
    }

}


void setup() {
    Serial.begin(115200);
    
    xTaskCreate(esp_publiser,
                "esp publishing esp_topics",
                1024 * 4,
                NULL,
                1,
                NULL);
    xTaskCreate(esp_subscriber,
                "esp subscribing pc_topic",
                1024 * 4,
                NULL, 
                1, 
                NULL);
}

void loop() {
    
}