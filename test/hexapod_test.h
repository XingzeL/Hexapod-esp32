#pragma once
#include <Arduino.h>
#include <EEPROM.h>
#include "leg_test.h"
#include "movement_test.h"

namespace hexapod_test {
    
    class HexapodClass {
    public:
        HexapodClass();

        void init(bool setting = false);

        void processMovement(MovementMode mode, int elapsed = 0);

    private:
        void calibrationLoad();
    private:
        MovementMode mode_;
        Movement movement_;
        Leg legs_[6];
    };

    extern HexapodClass Hexapod;
}