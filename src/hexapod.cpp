
#include "hexapod.h"
#include "servo.h"
#include "debug.h"

#define EEPROM_READ 1
#define LED_PIN 2
// TBD: move to hal

namespace hexapod {

    HexapodClass Hexapod; //这条在该hexapod中可以放在任意位置
    //std::unordered_map<int, MovementMode> moveHashMap;

    HexapodClass::HexapodClass(): 
        legs_{{0}, {1}, {2}, {3}, {4}, {5}}, 
        movement_{MOVEMENT_STANDBY},
        mode_{MOVEMENT_STANDBY}
    {
        //std::unordered_map<int, MovementMode> moveHashMap; //这一句跟外面的重复，出现只能设置动作0的问题，注释掉后出现core panic：Guru Meditation Error
        Serial.println("Hexapod Created!"); //这一句没看到输出


    }

    

    void HexapodClass::init(bool setting) {
        
        auto value = MovementMode::MOVEMENT_STANDBY;
        for (int i = MovementMode::MOVEMENT_STANDBY; i < MovementMode::MOVEMENT_TOTAL; i++) {
            this->moveMap_[i] = value;
            value++;
            if (DEBUG) {
                Serial.printf("movement mode %d, num %d\n", value, i);
            }
        }
        Serial.println("private moveMap insert success!");

        Servo::init();

        calibrationLoad();

        // default to standby mode
        if (!setting)
            processMovement(MOVEMENT_STANDBY);
        //下面是进行0角度测试
        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < 3; j++) {
                hexapod::Hexapod.calibrationTest(i, j, 0);
            }
        }
        
        LOG_INFO("Hexapod init done.");
    }


    void HexapodClass::setMode(int num) {
        this->mode_ = this->moveMap_[num];
        if(1) {
            Serial.printf("任务成功设置机器人运动模式！%d\n", this->moveMap_[num]);
        }
    }

    MovementMode HexapodClass::getMode() {
        return this->mode_;
    }

    void HexapodClass::processMovement_RTOS(int elapsed) {  //用于任务中循环使机器人移动t
        //if (mode_ >= MovementMode::MOVEMENT_TOTAL) return;
        movement_.setMode(mode_); //这一行出现不正常的摆动     
        //movement_.setMode(MOVEMENT_TWIST); //用这一行直接不动
        auto& location = movement_.next(elapsed);

        Serial.printf("In move RTOS ");
        Serial.println(location.get(0).x_);

        for(int i=0;i<6;i++) {
            legs_[i].moveTip(location.get(i));
            //if (DEBUG) {
            //    Serial.printf("move mode %d core %d\n", mode_, xPortGetCoreID());
            //}

        }
    } 

    void HexapodClass::processMovement(MovementMode mode, int elapsed) {
        if (mode >= MovementMode::MOVEMENT_TOTAL) return;
        if (mode_ != mode) {
            mode_ = mode;
            movement_.setMode(mode_);
        }

        auto& location = movement_.next(elapsed);

        if (DEBUG) {
            Serial.printf("set movement x ");
            Serial.println(location.get(0).x_);
        }

        for(int i=0;i<6;i++) {
            legs_[i].moveTip(location.get(i));
            if (PRINT) {
                Serial.print("done leg");
                Serial.println(i);
                Serial.print("\n");
            }

        }
    }

    void HexapodClass::calibrationSave() {
        
        // int address = 0;
        // for (int i = 0; i < 6; i++) {
        //     for (int j = 0; j < 3; j++) {
        //         int offset, scale;
        //         legs_[i].get(j)->getParameter(offset, scale);
        //         EEPROM.put(address, offset);
        //         EEPROM.put(address + sizeof(int32_t), scale);
        //         address += 2 * sizeof(int32_t);
        //     }
        // }

        //新式存放EEPROM
        EEPROM.begin(36);
        int address = 0;
        for(int i = 0; i < 6; i++) {
            for (int j = 0; j < 3; j++) {
                int offset;
                int scale;
                legs_[i].get(j)->getParameter(offset, scale);
                EEPROM.write(address, offset);
                EEPROM.write(address + 1, scale);
                address += 2;
            }
        }
        EEPROM.commit(); //注意写完之后要commit
        Serial.println("Commited");

        //闪烁提示
        for (int i = 0; i < 10; i++) {
            digitalWrite(LED_PIN, HIGH);
            delay(50);
            digitalWrite(LED_PIN, LOW);
            delay(50);
        }

        //保存时读取eeprom
        
        #ifdef EEPROM_READ
        Serial.println("");
        Serial.println("开始读取offset和scale");
        EEPROM.begin(36);
        address = 0;
        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < 3; j++) {
                int offset = EEPROM.read(address);
                int scale = EEPROM.read(address+1);
                legs_[i].get(j)->setParameter(offset, scale, false);
                Serial.printf("offset: %d, scale: %d\n", offset, scale);
                address += 2;
            }
        }
        #endif

        delay(1000);
/*
        short data[6*3*2];

        for(int i=0;i<6;i++) {
            for(int j=0;j<3;j++) {
                int offset, scale;
                legs_[i].get(j)->getParameter(offset, scale);
                data[2*3*i + 2*j] = (short)offset;
                data[2*3*i + 2*j + 1] = (short)scale;
            }
        }
*/
    }

    void HexapodClass::calibrationGet(int legIndex, int partIndex, int& offset, int& scale) {
        legs_[legIndex].get(partIndex)->getParameter(offset, scale);
    }

    void HexapodClass::calibrationSet(int legIndex, int partIndex, int offset, int scale) {
        legs_[legIndex].get(partIndex)->setParameter(offset, scale);
    }

    void HexapodClass::calibrationTest(int legIndex, int partIndex, float angle) {
        legs_[legIndex].get(partIndex)->setAngle(angle);
    }

    void HexapodClass::calibrationLoad() {
        uint32_t size = sizeof(int32_t);
        int address = 0;

// //1
//         for (int i = 0; i < EEPROM.length(); i++) { //initial the value of EEPROM
//            EEPROM.write(i, 1);
//         }
//         for(int i=0;i<6;i++) {
//             for(int j=0;j<3;j++) {
//                 int32_t offset = 0;
//                 int32_t scale = 5;
//                 EEPROM.write(address, offset);
//                 EEPROM.write(address + size, scale);
//                 address += 2 * size;  //之前没写这里
//             }
//         }
//         address = 0;
// //1
//         for(int i=0;i<6;i++) {
//             for(int j=0;j<3;j++) {
//                 int32_t offset;
//                 int32_t scale;
//                 EEPROM.get(address, offset);
//                 EEPROM.get(address + size, scale);
//                 legs_[i].get(j)->setParameter(offset, scale, false);
//                 Serial.printf("offset: %d, scale: %d\n", offset, scale);
//                 address += 2 * size;  //之前没写这里
//             }
//         }

//EEPROM操作，esp32和teensy之间有一些区别，用原来的方法时会出现奇怪的数字
        // EEPROM.begin(36);
        // address = 0;
        // for(int i = 0; i < 6; i++) {
        //     for (int j = 0; j < 3; j++) {
        //         int offset = 0;
        //         int scale = 0;
        //         EEPROM.write(address, offset);
        //         EEPROM.write(address + 1, scale);
        //         address += 2;
        //     }
        // }
        // EEPROM.commit(); //注意写完之后要commit
        // Serial.println("Commited");

        Serial.println("");
        Serial.println("开始读取offset和scale");
        EEPROM.begin(36);
        address = 0;
        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < 3; j++) { 
                uint8_t offset = EEPROM.read(address);
                uint8_t scale = EEPROM.read(address+1);
                legs_[i].get(j)->setParameter(int(offset), int(scale), false);
                Serial.printf("offset: %d, scale: %d\n", offset, scale);
            }
        }
        //结束EEPROM操作

        for (int i = 0; i < 10; i++) {
            digitalWrite(LED_PIN, HIGH);
            delay(50);
            digitalWrite(LED_PIN, LOW);
            delay(50);
        }
        //delay(2000);
    }
    //HexapodClass Hexapod;
}