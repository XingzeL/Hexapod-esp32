#include "calibration.h"
#include "debug.h"
#include "hexapod.h"
#include <Arduino.h>
#include <cmath>

#define SERVO_ONE_LEG 3
#define LEG_COUNT(m) abs(m) / SERVO_ONE_LEG
#define SERVO_INDEX(m) (abs(m) - 1) % SERVO_ONE_LEG
#define SERVO_COUNT(m) abs(m) % SERVO_ONE_LEG
#define LEG_INDEX(m) (SERVO_COUNT(m) == 0) ? LEG_COUNT(m) - 1 : LEG_COUNT(m)

#define TEST_ANGLE(m) 45 * (m / abs(m))

static int _cur_offset = 0;
static int _cur_scale = 0;
static int _test_angle[3*6] = {0};

//协议：offset的设置控制
void calibrate(int offset, int scale) {
    int valid = offset + scale;  //目前offset和scale同时只有一个非0
    if (offset == 0 && scale == 0) {
        hexapod::Hexapod.calibrationSave();
        return;
    }

    int set_offset = 0;
    if (offset != 0) {
        set_offset = (offset > 0) ? 1 : -1;
    }
    
    int set_scale = 0;
    if (scale != 0) {
        set_scale = (scale > 0) ? 1 : -1;
    }

    int legIndex = (SERVO_COUNT(valid) == 0) ? LEG_COUNT(valid) - 1 : LEG_COUNT(valid);

    hexapod::Hexapod.calibrationGet(legIndex, SERVO_INDEX(valid), _cur_offset, _cur_scale);
    hexapod::Hexapod.calibrationSet(legIndex, SERVO_INDEX(valid), _cur_offset + set_offset, _cur_scale + set_scale);
    Serial.printf("leg %d, servo %d, offset %d, scale %d\n", legIndex, SERVO_INDEX(valid),
                  _cur_offset+set_offset, _cur_scale + set_scale);
}

void calibrate_test(int msg) {
    if (msg == 0) {
        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < 3; j++) {
                hexapod::Hexapod.calibrationTest(i, j, 0);
            }
        }
        return;  //杜绝把0送到下面的宏里面，避免除0问题
    }
    hexapod::Hexapod.calibrationTest(LEG_INDEX(msg), SERVO_INDEX(msg), TEST_ANGLE(msg));
}

/*
    void HexapodClass::calibrationTest(int legIndex, int partIndex, float angle) {
        legs_[legIndex].get(partIndex)->setAngle(angle);
    }
*/