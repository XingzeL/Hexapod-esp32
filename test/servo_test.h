#pragma once

namespace hexapod_test {

    class Servo {
    public:
        static void init(void);

    public:
        Servo(int legIndex, int partIndex); //舵机的index

        void setAngle(float angle);  //设置舵机的角度
        float getAngle(void); //控制时调用的接口

        void getParameter(int& offset, int& scale) {
            offset = offset_;
            scale = scale_;
        }

        void setParameter(int offset, int scale, bool update = true) {
            offset_ = offset;
            scale_ = scale;
            if (update)
                setAngle(angle_);
        }

    private:
        float angle_; //此时舵机的angle
        int pwmIndex_; //pwm的索引
        bool inverse_; //是否让输出取反
        int offset_; //舵机的offset，初始化的参数之一
        int scale_; 
        int range_;
        float adjust_angle_;
    };
}