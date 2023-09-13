#include "hexapod_test.h"
//#include "movement_test.h"
#include "servo_test.h"

namespace hexapod_test {
    HexapodClass Hexapod;

    HexapodClass::HexapodClass(): 
        legs_{{0}, {1}, {2}, {3}, {4}, {5}}, 
        movement_{MOVEMENT_STANDBY},
        mode_{MOVEMENT_STANDBY}
    {

    }
    
    void HexapodClass::init(bool setting) {
        Servo::init();
        calibrationLoad();
        if (!setting) {
            processMovement(MOVEMENT_STANDBY);
        }

    }

    void HexapodClass::calibrationLoad() {
        uint32_t size = sizeof(int32_t);
        int address = 0;

        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < 3; j++) {
                int32_t offset;
                int32_t scale;
                EEPROM.get(address, offset);
                EEPROM.get(address + size, scale);
                legs_[i].get(j)->setParameter(offset, scale); //todo
                address += 2 * size;
            }
        }

        delay(2000);
    }

    void HexapodClass::processMovement(MovementMode mode, int elapsed) {
        if (mode_ != mode) {
            mode_ = mode;
            movement_.setMode(mode_);
        }

        auto& localtion = movement_.next(elapsed);
        for (int i = 0; i < 6; i++) {
            legs_[i].moveTip(localtion.get(i));
        }
    }

}