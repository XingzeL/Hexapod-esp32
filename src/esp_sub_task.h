#pragma once

#define LED_PIN 2
#define WIFI_ADDRESS "192.168.150.184"
#define SSID "SSID-A7FB6D"
#define PASSWD "NDSXrNf6"
#define NODE_NAME "hexapod_control1"
#define TOPICNAME "/pc_topic"

static int _mode = 1;
volatile unsigned long ulHighFrequencyTimerTicks;

rclc_executor_t exc;
rclc_support_t sup;
rcl_allocator_t allc;
rcl_node_t node;

// 声明话题订阅者
rcl_subscription_t test_sub; 
std_msgs__msg__Int32 test_msg;


void error_loop(){
  while(1){
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    delay(100);
  }
}

void callback_subscription_test(const void *msgin) {
    const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32*) msgin;
    //normal_loop(12);
    Serial.println(msg->data);
}

static void log_output(const char* log) {  //log: 串口信息，用Serial对象的println打印
  Serial.println(log);
}

void esp_subscriber(void *pvParam) {
 

    Serial.begin(115200);
    //delay(2000);
    pinMode(LED_PIN, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);

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
    
    
    /* 初始化测试订阅节点 */

    RCCHECK(rclc_subscription_init_default(
        &test_sub,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
        TOPICNAME));


    /* 初始化 */
    RCCHECK(rclc_executor_init(&exc, &sup.context, 4, &allc));  //rclc_excutor_init, able to handle 4 topics
    
    RCCHECK(rclc_executor_add_subscription(&exc, &test_sub, &test_msg, &callback_subscription_test, ON_NEW_DATA));

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
    }
    rclc_executor_fini(&exc);
    rclc_support_fini(&sup);
}
