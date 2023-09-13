#pragma once

#include "base.h"
#include <unordered_map> //用于RTOS

namespace hexapod {

    enum MovementMode {  //枚举变量，表示该变量的取值范围只有这几个，增加代码的可读性
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

    // inline关键字是一种编译器优化技术，让这个函数直接在用到这个的地方展开，而不是调用函数进来，减少调用函数的开销
    // inline指定的内联展开函数适用于轻量级且多次调用的函数，不然会使代码冗长
    // 像这种重载的运算符就适合直接内联展开

    inline MovementMode operator++ (MovementMode& m, int) {
        if (m < MOVEMENT_TOTAL)
            m = static_cast<MovementMode>(static_cast<int>(m) + 1);
        return m;
    }

    struct MovementTable {
        const Locations* table; //指向位姿table的指针，位姿为Location类
        int length;
        int stepDuration; //这一步需要走的时间
        const int* entries;
        int entriesCount;
    };

    class Movement {
    public:
        Movement(MovementMode mode);

        void setMode(MovementMode newMode);

        const Locations& next(int elapsed);

    private:
        MovementMode mode_;
        Locations position_;
        int index_;             // index in mode position table
        bool transiting_;       // if still in transiting to new mode
        int remainTime_;  //剩余时间
    };

}