#include <Arduino.h>

#include "debug_test.h"
#include "config_test.h"
#include "hexapod_test.h"

#include "hexapod_move.h"

#define REACT_DELAY hexapod_test::config::movementInterval
//取自config文件中定义的常数

static void log_output(const char* log) {
    Serial.println(log);
}

void hexapod_setup(void) {}

void hexapod_move(int _mode) {

    auto t0 = millis();  //得到当前的时间
    digitalWrite(2,1);
    auto mode = hexapod_test::MOVEMENT_STANDBY;

    for (auto i = _mode; i > 0; i--) {
        mode++; //这个++运算符时经过了重载的，用于这个枚举类型的累加
    }

    hexapod_test::Hexapod.processMovement(mode, REACT_DELAY);
    auto spent = millis() - t0;
    if (spent < REACT_DELAY) {
        delay(REACT_DELAY-spent);
    }
    else {
        Serial.println(spent);
    }
}
