#pragma once
#include <Arduino.h>
#include <EEPROM.h>
#include "movement.h"
#include "leg.h"
#include <unordered_map>

namespace hexapod {

    class HexapodClass {
    public:
        HexapodClass();

        // init

        void init(bool setting = false);

        // Movement API

        void processMovement(MovementMode mode, int elapsed = 0);
        void processMovement_RTOS(int elapsed); //用于RTOS

        // Calibration API

        void calibrationSave(); // write to flash
        void calibrationGet(int legIndex, int partIndex, int& offset, int& scale);  // read servo setting
        void calibrationSet(int legIndex, int partIndex, int offset, int scale);    // update servo setting
        void calibrationTest(int legIndex, int partIndex, float angle);             // test servo setting

        // setting API
        void setMode(int num);

        // getting API
        MovementMode getMode();

    private:
        void calibrationLoad(); // read from flash

    private:
        volatile MovementMode mode_ = MOVEMENT_TWIST;  //MovementMode, Movement的定义在movement.h
        Movement movement_;
        Leg legs_[6];
        std::unordered_map<int, MovementMode> moveMap_;
    };

    extern HexapodClass Hexapod;  //注释后会报错：'Hexapod' is not a member of 'hexapod'
}