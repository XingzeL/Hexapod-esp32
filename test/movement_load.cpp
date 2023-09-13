/*
    这个文件用来定义和配置和定义运动相关的宏和参数
    并且使用了上面定义完成的参数在文件后include movementTable的代码
    这样就可以在movementTable中直接用这个文件里定义的宏
*/

#include "movement_test.h"
#include "config_test.h"

using namespace hexapod_test::config;
using namespace hexapod_test;

#define SIN30   0.5
#define COS30   0.866
#define SIN45   0.7071
#define COS45   0.7071
#define SIN15   0.2588
#define COS15   0.9659

/*
    STANDBY_Z:在standby模式下机身平台的z轴高度
    LEFTRIGHT_X
*/
#define STANDBY_Z (kLegJoint3ToTip*COS15-kLegJoint2ToJoint3*SIN30)
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

namespace hexapod_test {

    namespace {
        const Locations KStandby {
            {P1X, P1Y, P1Z}, {P2X, P2Y, P2Z}, {P3X, P3Y, P3Z},{P4X, P4Y, P4Z}, {P5X, P5Y, P5Z}, {P6X, P6Y, P6Z}    
        };
        const int zero = 0;
/*
    struct  MovementTable 
    {
        const Locations* table;
        int length;
        int stepDuration;
        const int* entries;
        int extriesCount;
    };
*/
        const MovementTable standay_table {&KStandby, 1, 20, &zero, 1}; //注意这里是大括号，是初始化这个结构体
    }
    const MovementTable& standbyTable() {
        return standay_table;
    }

    #include "movement_table.h"

}