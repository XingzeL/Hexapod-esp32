#include <Arduino.h>
#include <micro_ros_platformio.h>
#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>  //excutor需要进行include

#include <std_msgs/msg/int32.h>
#include <string.h>
#include <vector>
#include <functional>

#include "hexapod_test.h"
#include "hexapod_move.h"

#define LED_PIN 2
static int _mode = 1;

rclc_executor_t exc;
rclc_support_t sup;

rcl_allocator_t allc;
rcl_node_t node;

rcl_subscription_t sub;
std_msgs__msg__Int32 msg;

void callback_subscription(const void *msgin) {
    const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32 *)msgin;
    int move = msg->data;
    hexapod_move(move);
}


void setup() {
    pinMode(LED_PIN, OUTPUT);
    Serial.begin(11520);
    Serial.println("Srarting...");
    pinMode(LED_BUILTIN, OUTPUT); //这里直接映射到了esp32的按键引脚

    Serial.print("Started, mode = ");
    Serial.println(_mode);

    //robot init

    //transportation
    set_microros_serial_transports(Serial);
    delay(5000);

    allc = rcl_get_default_allocator();

}

void loop() {
    hexapod_move(12);
}