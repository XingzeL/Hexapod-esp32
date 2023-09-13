#pragma once
#include "base_test.h"

/*
    MovementClass定义了整个机器人的姿态，控制六条腿的落脚点位置
    定义机器人运动的模式，加载运动模式相应的table，重载位置点和运动模式的运算符
    机器人的运动按照三维点表格进行推进，MovementClass对象也会记录当前运动到table中的index
    能够表示机器人是否在切换运动，config中设置了机器人的切换延时：150ms
*/

namespace hexapod_test {

    enum MovementMode {
        MOVEMENT_STANDBY = 0,
        MOVEMENT_FORWARD,
        MOVEMENT_FORWARDFAST,
        MOVEMENT_BACKWARD,
        MOVEMENT_TURNLEFT,
        MOVEMENT_TURNRIGHT,
        MOVEMENT_SHIFTLEFT,
        MOVEMENT_SHIFTRIGHT,
        MOVEMENT_CLIMB,
        MOVEMENT_ROTATEX,
        MOVEMENT_ROTATEY,
        MOVEMENT_ROTATEZ,
        MOVEMENT_TWIST,

        MOVEMENT_TOTAL,
    };

    inline MovementMode operator++ (MovementMode& m, int) {
        if (m < MOVEMENT_TOTAL) {
            m = static_cast<MovementMode>(static_cast<int>(m) + 1);
        }
        return m;
    }

    struct  MovementTable 
    {
        /* data */
        const Locations* table;
        int length;
        int stepDuration;
        const int* entries;
        int extriesCount;
    };

    class Movement {
    public:
        Movement(MovementMode mode); //构造函数

        void setMode(MovementMode newMode); //得到mode command的时候的操作

        const Locations& next(int elapsed); //接着执动作,动作对象的变化

    private:
        MovementMode mode_;
        Locations position_;
        int index_;
        bool transiting_;
        int remainTime_;
    };
    
}