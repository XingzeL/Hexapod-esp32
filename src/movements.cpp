#include "movement.h"
#include "config.h"
// #include "movement_table.h"  //table头文件在这里会出现Locations,MovementTable等在namespace hexapod中的类型被报错为未定义

using namespace hexapod::config;
using namespace hexapod;

#define SIN30   0.5
#define COS30   0.866
#define SIN45   0.7071
#define COS45   0.7071
#define SIN15   0.2588
#define COS15   0.9659

#define STANDBY_Z   (kLegJoint3ToTip*COS15-kLegJoint2ToJoint3*SIN30)
#define LEFTRIGHT_X (kLegMountLeftRightX+kLegRootToJoint1+kLegJoint1ToJoint2+(kLegJoint2ToJoint3*COS30)+kLegJoint3ToTip*SIN15)
#define OTHER_X (kLegMountOtherX + (kLegRootToJoint1+kLegJoint1ToJoint2+(kLegJoint2ToJoint3*COS30)+kLegJoint3ToTip*SIN15)*COS45)
#define OTHER_Y (kLegMountOtherY + (kLegRootToJoint1+kLegJoint1ToJoint2+(kLegJoint2ToJoint3*COS30)+kLegJoint3ToTip*SIN15)*SIN45)

#define P1X     OTHER_X
#define P1Y     OTHER_Y
#define P1Z     -STANDBY_Z

#define P2X     LEFTRIGHT_X
#define P2Y     0
#define P2Z     -STANDBY_Z

#define P3X     OTHER_X
#define P3Y     -OTHER_Y
#define P3Z     -STANDBY_Z

#define P4X     -OTHER_X
#define P4Y     -OTHER_Y
#define P4Z     -STANDBY_Z

#define P5X     -LEFTRIGHT_X
#define P5Y     0
#define P5Z     -STANDBY_Z

#define P6X     -OTHER_X
#define P6Y     OTHER_Y
#define P6Z     -STANDBY_Z

namespace hexapod {
    #include "movement_table.h"  //放到这里可以运行

    namespace {  //匿名命名空间，仅在当前文件中有效，不会影响其他文件
        const Locations kStandby {
            {P1X, P1Y, P1Z}, {P2X, P2Y, P2Z}, {P3X, P3Y, P3Z}, {P4X, P4Y, P4Z}, {P5X, P5Y, P5Z}, {P6X, P6Y, P6Z}
        };
        const int zero = 0;

        const MovementTable standay_table {&kStandby, 1, 20, &zero, 1};
    }

    const MovementTable& standbyTable() {
        return standay_table;
    }

    // #include "movement_table.h"

    // const MovementTable& backwardTable() { //定义查表函数放在这里（#include之后）可以通过编译
    //     return backward_table; 
    // }
    //movement_table中返回表地址的函数定义在namespace之外，
    // 因为在这个文件中引入之后，这些函数会存在于namespace hexapod之中，
    // 而不是movement_table中的匿名namespace中
}
