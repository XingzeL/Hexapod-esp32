#pragma once

#include "base_test.h"
#include "servo_test.h"

#include <functional>

namespace hexapod_test {

    class Leg {
    public:

        Leg(int legIndex);  //构造函数，腿的重要参数：index
        virtual ~Leg();
        /*
        当一个类被用作基类时，通常需要将其析构函数声明为虚函数。
        如果类没有虚函数，当在指向子类的基类指针上删除对象时，
        只会调用基类的析构函数，而不会调用子类的析构函数。
        这可能会导致内存泄漏和其他未定义行为。通过将析构函数声明为虚函数，
        可以确保在删除指向子类的基类指针时，将调用正确的析构函数。
        */

        //坐标转换的方法，待研究
        void translateToLocal(const Point3D& world, Point3D& local);
        void translateToWorld(const Point3D& local, Point3D& world);

        void setJointAngle(float angle[3]); //传入舵机的角度，直接设置角度
        //项目中并没有使用这个函数，先将角度转换成了3D点坐标，然后进行一次逆运动学计算转换成角度，然后setAngle()

        void moveTip(const Point3D& to); //得到3维点形式的姿态信息，设置机器人的姿态
        //内部的move_函数设定好机器人，之后记录这次的位姿保存在Leg的对象里以供下一个时间点的姿态设置计算

        const Point3D& getTipPosition(void);

        void moveTipLocal(const Point3D& to);
        const Point3D& getTipPositionLocal(void);

        Servo* get(int partIndex) {
            return servos_[partIndex];
        }

    private:

        //正运动学计算：从angle转到落脚点
        //逆运动学计算：从3D落脚点到舵机的angle(3个)
        static void _forwardKinematics(float angle[3], Point3D& out);
        static void _inverseKinematics(const Point3D& to, float angles[3]);
        void _move(const Point3D& to);

    private:
        int index_;  //leg的index，在构造函数中初始化
        Servo* servos_[3];  //leg中的3个舵机
        Point3D mountPosition_;  //重要：关节的3D位置

        /*
            对于每个对象需要不同的方法的时候用函数指针
            在构造函数中初始化，需要实现定义需要指向的函数
        */
        std::function<void(const Point3D& src, Point3D& dest)> localConv_;
        std::function<void(const Point3D& src, Point3D& dest)> worldConv_;

        /*
            tip：落脚点的3D坐标点，并不在构造的时候定义，在moveTip的时候设置，
            相当于一个锁存器，存住上一个时刻的落脚点

            Local：以机器人的物理中点为原点
            普通：以出发点为原点(世界坐标系)
        */
        Point3D tipPos_;
        Point3D tipPosLocal_;

    };
}