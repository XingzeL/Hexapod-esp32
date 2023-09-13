#pragma once

#include <stdexcept>

namespace hexapod_test {

    class Point3D {
    public:
        Point3D() = default;
        Point3D(float x, float y, float z): x_{x}, y_{y}, z_{z} {
        }
    
    public:
        float x_;
        float y_;
        float z_;
    };

    inline Point3D operator-(const Point3D &lhs, const Point3D &rhs) {
        return Point3D(lhs.x_ - rhs.x_, lhs.y_ - rhs.y_, lhs.z_ - rhs.z_);
    }

        inline Point3D operator*(const Point3D &lhs, const float& b) {
        return Point3D(lhs.x_ * b, lhs.y_ * b, lhs.z_ * b);
    };

    inline Point3D& operator +=(Point3D& a, const Point3D& b) {
        a.x_ += b.x_;
        a.y_ += b.y_;
        a.z_ += b.z_;
        return a;
    }

    inline bool operator ==(const Point3D &a, const Point3D &b) {
        return (a.x_ == b.x_) && (a.y_ == b.y_) && (a.z_ == b.z_);
    }

    class Locations {
    public:
        Locations() = default;
        Locations(const Point3D& foreRight, const Point3D& right, const Point3D& hindRight, const Point3D& hindLeft, const Point3D& left, const Point3D& foreLeft):
            points_{foreRight, right, hindRight, hindLeft, left, foreLeft} {
        }

        const Point3D& get(int index) const {
            return points_[index];
        }
        /*
        这段代码是一个类的成员函数，
        返回一个 const Point3D 的引用，它获取了 points_ 数组中指定 index 索引的元素。
        const 关键字表示该函数不会修改类的成员变量。这是因为该函数被声明为 const，
        这意味着它不能修改对象的状态。同时，返回值也是一个 const 引用，这意味着返回的对象不能被修改。
        */

        Locations operator-(const Locations &b) const {
            return Locations{
                points_[0] - b.points_[0], points_[1] - b.points_[1], points_[2] - b.points_[2],
                points_[3] - b.points_[3], points_[4] - b.points_[4], points_[5] - b.points_[5],  
            };
        }

        Locations operator*(const float& b) const {
            return Locations{
                points_[0] * b, points_[1] * b, points_[2] * b,
                points_[3] * b, points_[4] * b, points_[5] * b,
            };
        }

        Locations& operator +=(const Locations& b) {
            for(auto i=0;i<6;i++)
                points_[i] += b.points_[i];
            return *this;
        }

    private:
        Point3D points_[6];
    };
}