#include <Arduino.h>
#include "normal_mode.h"
#include "calibration.h"
#include "debug.h"
#include "hexapod.h"
#include "config.h"

#include "rtosQueue.h"
//#include "timer.h"

#define REACT_DELAY hexapod::config::movementInterval
using namespace hexapod;

std::unordered_map<int, MovementMode> moveMap;
//extern QueueHandle_t controlMsg;

void moveLegTask(void * pvParam) {
    Serial.begin(115200);
    hexapod::Hexapod.init(false);
    //ConfigureTimerForMovement();
    // while (1)
    // {
    //     /* code */
    //     normal_loop(2); //正常运行，过程：传入int数据，调用hexapod::Hexapod.processMovement(mode_, elapse)
    // }

    // while(1) { //无法正常行走
    //     auto t0 = millis();
    //     auto mode = hexapod::MOVEMENT_TWIST;
    //     hexapod::Hexapod.processMovement_RTOS(REACT_DELAY);
    //     auto spent = millis() - t0;
    //     if(spent < REACT_DELAY) {
    // // Serial.println(spent);
    //         delay(REACT_DELAY-spent);
    //     }
    //     else {
    // //digitalWrite(13, 0);
    //         Serial.printf("spent: %d\n",spent);
    //     }
    //     //Serial.printf("core %d,task move, time ", xPortGetCoreID());
    //     //Serial.println(millis());
    //     //Serial.println("");
    // }

    //填充一个快速查找的哈希表
    auto value = MovementMode::MOVEMENT_STANDBY;
    for (int i = MovementMode::MOVEMENT_STANDBY; i < MovementMode::MOVEMENT_TOTAL; i++) {
        moveMap[i] = value;
        if (DEBUG) {
            Serial.printf("movement mode %d, num %d\n", value, i);
        }
        value++;
    }
    Serial.println("moveMap insert success!");
    // Hexapod.setMode(5);

    int flag = 0;

    while (1) {

        // flag_move = true;
        if (xQueueReceive(controlMsg, &flag, 5)) {
            Serial.printf("Recieve %d\n", flag);
        }
        auto t0 = millis();
        MovementMode mode = moveMap[flag];
        //auto mode = Hexapod.getMode();
        //Serial.printf("Proccess movement %d in %d\n",mode, millis());

        Hexapod.processMovement(mode, REACT_DELAY);
        auto spent = millis() - t0;
        if(spent < REACT_DELAY) {
    // Serial.println(spent);
           delay(REACT_DELAY-spent);
        }
        else {
    //digitalWrite(13, 0);
            Serial.printf("spent: %d\n",spent);
        }
        // if (MovementRunTimeTicks > 0) {
        //     Serial.printf("movement task:");
        //     Serial.println(MovementRunTimeTicks);
        // }
        // flag_move = false;
    }


}