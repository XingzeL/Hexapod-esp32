#include <Arduino.h>

#include "leg_test.h"
#include "config_test.h"
#include "servo_test.h"

#include <cmath>
using namespace hexapod_test::config;

namespace hexapod_test {
    namespace {
        //下面会进行一些数学计算
        #define SIN45 0.7071
        #define COS45 0.7071

        void rotate0(const Point3D& src, Point3D& dest) {
            dest = src;
        }

        void rotate45(const Point3D& src, Point3D& dest) {  //沿着z轴顺时针旋转45度
            dest.x_ = src.x_ * COS45 - src.y_ * SIN45;
            dest.y_ = src.x_ * SIN45 + src.y_ * COS45;
            dest.z_ = src.z_;   
        }

        void rotate135(const Point3D& src, Point3D& dest) {
            dest.x_ = src.x_ * (-COS45) - src.y_ * SIN45;
            dest.y_ = src.x_ * SIN45 + src.y_ * (-COS45);
            dest.z_ = src.z_;
        }

        void rotate180(const Point3D& src, Point3D& dest) {
            dest.x_ = -src.x_;
            dest.y_ = -src.y_;
            dest.z_ = src.z_;
        }

        void rotate225(const Point3D& src, Point3D& dest) {
            dest.x_ = src.x_ * (-COS45) - src.y_ * (-SIN45);
            dest.y_ = src.x_ * (-SIN45) + src.y_ * (-COS45);
            dest.z_ = src.z_;
        }

        void rotate315(const Point3D& src, Point3D& dest) {
            dest.x_ = src.x_ * COS45 - src.y_ * (-SIN45);
            dest.y_ = src.x_ * (-SIN45) + src.y_ * COS45;
            dest.z_ = src.z_;
        }
    }

    Leg::Leg(int legIndex): index_(legIndex) {
        /*
            index_(legIndex) 是在 Leg 类的构造函数初始化列表中设置成员变量 index_ 
            的值为传入的 legIndex。这个初始化操作确保在对象被创建时成员变量就已经被正确地
            初始化了。这是 C++ 中一个提高效率和避免出现错误的技巧。如果不使用初始化列表，
            而是在构造函数体中给成员变量赋值，那么实际上会发生两次赋值，
            一次是成员变量的默认初始化，一次是在构造函数体中的赋值，
            这样就浪费了一定的时间和资源。
        */
        switch(legIndex) {
        /*
            接下来初始化关节位置，
        */
        case 0:
            mountPosition_ = Point3D(kLegMountOtherX, kLegMountOtherY, 0);
            localConv_ = rotate315;
            worldConv_ = rotate45;
            break;

        case 1:
            mountPosition_ = Point3D(kLegMountLeftRightX, 0, 0);
            localConv_ = rotate0;
            worldConv_ = rotate0;
            break;

        case 2: // -45 or 315 degree
            mountPosition_ = Point3D(kLegMountOtherX, -kLegMountOtherY, 0);
            localConv_ = rotate45;
            worldConv_ = rotate315;
            break;

        case 3: // -135 or 225 degree
            mountPosition_ = Point3D(-kLegMountOtherX, -kLegMountOtherY, 0);
            localConv_ = rotate135;
            worldConv_ = rotate225;
            break;
        case 4: // 180 degree
            mountPosition_ = Point3D(-kLegMountLeftRightX, 0, 0);
            localConv_ = rotate180;
            worldConv_ = rotate180;
            break;
        case 5: // 135 degree
            mountPosition_ = Point3D(-kLegMountOtherX, kLegMountOtherY, 0);
            localConv_ = rotate225;
            worldConv_ = rotate135;
            break;
        
       }
        for (int i = 0; i < 3; i++) {
            servos_[i] = new hexapod_test::Servo(legIndex, i);
        }
    }

    Leg::~Leg() {
        for (int i = 0; i < 3; i++) {
            delete servos_[i];   //删除对象和置空都要有
            servos_[i] = nullptr;
        }
    }

    /*
        这个项目中，world坐标系是以机器人身体中心点为原点（感觉应该是robot坐标系）
        Local坐标系是以第一关节点为原点，y轴朝向腿伸直的方向(leg坐标系)
    */
    void Leg::translateToLocal(const Point3D& world, Point3D& local) {
        localConv_(world - mountPosition_, local); 
    }

    void Leg::translateToWorld(const Point3D& local, Point3D& world) {
        worldConv_(local, world);
        world += mountPosition_;
    }

    void Leg::setJointAngle(float angle[3]) {
        Point3D to;
        _forwardKinematics(angle, to); //得到servo的angle转换成tip的点坐标
        moveTipLocal(to);
    }

    void Leg::moveTip(const Point3D& to) { 
        /*
            传入三维坐标，转换成local坐标，执行move
            原项目使用的是这个api，说明table中的坐标点是以robot的中心为原点的
        */

        if (to == tipPos_) {
            return;
        }

        Point3D local;
        translateToLocal(to, local); //得到的点转换成局部坐标系
        _move(local);
        tipPos_ = to;  //将当前设定的点存到对象的属性中
        tipPosLocal_ = local;
    }

    const Point3D& Leg::getTipPosition(void) { //外部访问内部属性的接口，原项目中没有用到
        return tipPos_;
    }

    void Leg::moveTipLocal(const Point3D& to) {
        if (to == tipPosLocal_)
            return;
        
        Point3D world;
        translateToWorld(to, world);
        _move(to);
        tipPos_ = world;
        tipPosLocal_ = to;
    }

    const Point3D& Leg::getTipPositionLocal(void) {
        return tipPosLocal_;
    }

    /*
        private函数 
    */
   const float pi = std::acos(-1);
   const float hpi = pi / 2;

   void Leg::_forwardKinematics(float angle[3], Point3D& out) {
        float radian[3];
        for (int i = 0; i < 3; i++) {
            radian[i] = pi * angle [i] / 180; //转换成弧度
        }

        float x = kLegJoint1ToJoint2 + std::cos(radian[1]) * kLegJoint2ToJoint3 + std::cos(radian[1] + radian[2] - hpi) * kLegJoint3ToTip;

        out.x_ = kLegRootToJoint1 + std::cos(radian[0]) * x;
        out.y_ = std::sin(radian[0]) * x;
        out.z_ = std::sin(radian[1]) * kLegJoint2ToJoint3 + std::sin(radian[1] + radian[2] - hpi) * kLegJoint3ToTip;
   }

    /*
        最重要的数学运算：逆运动学运算：补充视频参考b站上的视频
    */
    void Leg::_inverseKinematics(const Point3D& to, float angles[3]) {

        float x = to.x_ - kLegRootToJoint1;
        float y = to.y_;

        angles[0] = std::atan2(y, x) * 180 / pi;

        x = std::sqrt(x*x + y*y) - kLegJoint1ToJoint2;
        y = to.z_;
        float ar = std::atan2(y, x);
        float lr2 = x*x + y*y;
        float lr = std::sqrt(lr2);
        float a1 = std::acos((lr2 + kLegJoint2ToJoint3*kLegJoint2ToJoint3 - kLegJoint3ToTip*kLegJoint3ToTip)/(2*kLegJoint2ToJoint3*lr));
        float a2 = std::acos((lr2 - kLegJoint2ToJoint3*kLegJoint2ToJoint3 + kLegJoint3ToTip*kLegJoint3ToTip)/(2*kLegJoint3ToTip*lr));
        angles[1] = (ar + a1) * 180 / pi;
        angles[2] = 90 - ((a1 + a2)  * 180 / pi);
    }

    void Leg::_move(const Point3D& to) {
        float angle[3];
        _inverseKinematics(to, angle);
        for (int i = 0; i < 3; i++) {
            servos_[i]->setAngle(angle[i]);
        }
    }


}