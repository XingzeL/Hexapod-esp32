#include <Wire.h>
#include "Adafruit_PWMServoDriver.h"

#include "pwm_test.h"

/*
    这一层开始调用adafruit的驱动api
*/

namespace hexapod_test { namespace hal {
    PCA9685::PCA9685(int i2cAddress) {
        /*
            pca9685对象的private的可以指向任何对象
            这里要用到i2c传输的工作，所以在构造函数中指向了一个驱动对象
        */
        obj_ = (void*)new Adafruit_PWMServoDriver(i2cAddress); 
    }

    PCA9685::~PCA9685() {
        delete ((Adafruit_PWMServoDriver*)obj_);
    }

    void PCA9685::begin() {
        ((Adafruit_PWMServoDriver*)obj_)->begin();
    }

    void PCA9685::setPWMFreq(int freq) {
        ((Adafruit_PWMServoDriver*)obj_)->setPWMFreq(freq);
    }

    void PCA9685::setPWM(int index, int on, int off) {
        ((Adafruit_PWMServoDriver*)obj_)->setPWM(index, (uint16_t)on, (uint16_t)off);
    }
}

}