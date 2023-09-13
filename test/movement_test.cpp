/*
    movement.cpp使用了table中的数据
    实现动作模式的设定
    动作的执行，动作连贯性的机制
*/

#include "movement_test.h"
#include "config_test.h"
#include "debug_test.h"

#include <cstdlib>

namespace hexapod_test { //不加这个namespace，下面的extern全部都要报错

    extern const MovementTable& standbyTable();  //extern声明的函数，表明其在其他源文件中具有定义
    extern const MovementTable& forwardTable();
    extern const MovementTable& forwardfastTable();
    extern const MovementTable& backwardTable();
    extern const MovementTable& turnleftTable();
    extern const MovementTable& turnrightTable();
    extern const MovementTable& shiftleftTable();
    extern const MovementTable& shiftrightTable();
    extern const MovementTable& climbTable();
    extern const MovementTable& rotatexTable();
    extern const MovementTable& rotateyTable();
    extern const MovementTable& rotatezTable();
    extern const MovementTable& twistTable();

    const MovementTable kTable[MOVEMENT_TOTAL] { //定义一个整合表，方便再执行动作的时候调用
        standbyTable(),
        forwardTable(),
        forwardfastTable(),
        backwardTable(),
        turnleftTable(),
        turnrightTable(),
        shiftleftTable(),
        shiftrightTable(),
        climbTable(),
        rotatexTable(),
        rotateyTable(),
        rotatezTable(),
        twistTable(),
    };

    Movement::Movement(MovementMode mode):
        mode_{mode}, transiting_{false} {}

    void Movement::setMode(MovementMode newMode) {

        if (!kTable[newMode].entries) {
            LOG_INFO("Error: null movement of mode(%d)!", newMode);
            return;
        }

        mode_ = newMode;

        const MovementTable& table = kTable[mode_];

        index_ = table.entries[std::rand() % table.extriesCount];

        /*
            判断延时的时间是否大于动作周期：谁大就设置成谁的
        */
        remainTime_ = config::movementSwitchDuration > table.stepDuration ? config::movementSwitchDuration : table.stepDuration;
    }

    const Locations& Movement::next(int elapsed) {
        const MovementTable& table = kTable[mode_];

        if (elapsed <= 0) {
            elapsed = table.stepDuration;
        }

        if (remainTime_ <= 0) {
            index_ = (index_ + 1) % table.length;
            remainTime_ = table.stepDuration;
        }

        if (elapsed >= remainTime_) {
            elapsed = remainTime_;
        }

        auto ratio = (float)elapsed / remainTime_;
        position_ += (table.table[index_] - position_) * ratio;
        remainTime_ -= elapsed;

        return position_;
    }

}