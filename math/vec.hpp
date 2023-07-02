#pragma once

#include "math_base.hpp"

template <typename T>
class Vec2 : public VectorExtension<Vec2<T>, T> {
  public:
  Vec2(int x = 0, int y = 0) : x{x}, y{y} {}
  
  Vec2 operator+(const Vec2& other) const { return Vec2{x + other.x, y + other.y}; }
  Vec2 operator*(const T& scalar)   const { return Vec2{x * scalar, y * scalar}; }
  Vec2 operator-()                  const { return Vec2{-x, -y}; }

  T LengthSquared() const { return x * x + y * y; }

  T X() { return x; }
  T Y() { return y; }

  Vec2 XY() { return Vec2{x, y}; }
  Vec2 YX() { return Vec2{y, x}; }

  private:
  T x, y;
};


template <typename T>
class Vec3 : public VectorExtension<Vec3<T>, T> {
  public:
  Vec3(int x = 0, int y = 0, int z = 0) : x{x},        y{y},        z{z}        {}
  Vec3(const Vec2<T>& vec2, int z = 0)  : x{vec2.X()}, y{vec2.Y()}, z{z}        {}
  Vec3(int x, const Vec2<T>& vec2)      : x{x},        y{vec2.X()}, z{vec2.Y()} {}

  Vec3 operator+(const Vec3& other) const { return Vec3{x + other.x, y + other.y, z + other.z}; }
  Vec3 operator*(const T& scalar)   const { return Vec3{x * scalar, y * scalar, z * scalar}; }
  Vec3 operator-()                  const { return Vec3{-x, -y, -z}; }

  T LengthSquared() const { return x * x + y * y + z * z; }

  T X() { return x; }
  T Y() { return y; }
  T Z() { return z; }

  Vec2 XY() { return Vec2{x, y}; }
  Vec2 XZ() { return Vec2{x, z}; }
  Vec2 YX() { return Vec2{y, x}; }
  Vec2 YZ() { return Vec2{y, z}; }
  Vec2 ZX() { return Vec2{z, x}; }
  Vec2 ZY() { return Vec2{z, y}; }

  Vec3 XYZ() { return Vec3{x, y, z}; }
  Vec3 XZY() { return Vec3{x, z, y}; }
  Vec3 YXZ() { return Vec3{y, x, z}; }
  Vec3 YZX() { return Vec3{y, z, x}; }
  Vec3 ZXY() { return Vec3{z, x, y}; }
  Vec3 ZYX() { return Vec3{z, y, x}; }

  private:
  T x, y, z;
};

template <typename T>
class Vec4 : public VectorExtension<Vec4<T>, T> {
  public:
  Vec4(int x = 0, int y = 0, int z = 0, int w = 0) : x{x},        y{y},        z{z},        w{w}        {}
  Vec4(const Vec2<T>& vec2, int z = 0, int w = 0)  : x{vec2.X()}, y{vec2.Y()}, z{z},        w{w}        {}
  Vec4(int x, const Vec2<T>& vec2, int w = 0)      : x{x},        y{vec2.X()}, z{vec2.Y()}, w{w}        {}
  Vec4(int x, int y, const Vec2<T>& vec2)          : x{x},        y{y},        z{vec2.X()}, w{vec2.Y()} {}
  Vec4(const Vec2<T>& v1, const Vec2<T>& v2)       : x{v1.X()},   y{v1.Y()},   z{v2.X()},   w{v2.Y()}   {}
  Vec4(const Vec3<T>& vec3, int w = 0)             : x{vec3.X()}, y{vec3.Y()}, z{vec3.Z()}, w{w}        {}
  Vec4(int x, const Vec3<T>& vec3)                 : x{x},        y{vec3.X()}, z{vec3.Y()}, w{vec3.Z()} {}


  Vec4 operator+(const Vec4& other) const { return Vec4{x + other.x, y + other.y, z + other.z, w + other.w}; }
  Vec4 operator*(const T& scalar) const { return Vec4{x * scalar, y * scalar, z * scalar, w * scalar}; }
  Vec4 operator-() const { return Vec4{-x, -y, -z, -w}; }

  T LengthSquared() const { return x * x + y * y + z * z + w * w; }

  T X() { return x; }
  T Y() { return y; }
  T Z() { return z; }
  T W() { return w; }

  Vec2 XY() { return Vec2{x, y}; }
  Vec2 XZ() { return Vec2{x, z}; }
  Vec2 XW() { return Vec2{x, w}; }
  Vec2 YX() { return Vec2{y, x}; }
  Vec2 YZ() { return Vec2{y, z}; }
  Vec2 YW() { return Vec2{y, w}; }
  Vec2 ZX() { return Vec2{z, x}; }
  Vec2 ZY() { return Vec2{z, y}; }
  Vec2 ZW() { return Vec2{z, w}; }
  Vec2 WX() { return Vec2{w, x}; }
  Vec2 WY() { return Vec2{w, y}; }
  Vec2 WZ() { return Vec2{w, z}; }

  Vec3 XYZ() { return Vec3{x, y, z}; }
  Vec3 XYW() { return Vec3{x, y, w}; }
  Vec3 XZY() { return Vec3{x, z, y}; }
  Vec3 XZW() { return Vec3{x, z, w}; }
  Vec3 XWY() { return Vec3{x, w, y}; }
  Vec3 XWZ() { return Vec3{x, w, z}; }
  Vec3 YXZ() { return Vec3{y, x, z}; }
  Vec3 YXW() { return Vec3{y, x, w}; }
  Vec3 YZX() { return Vec3{y, z, x}; }
  Vec3 YZW() { return Vec3{y, z, w}; }
  Vec3 YWX() { return Vec3{y, w, x}; }
  Vec3 YWZ() { return Vec3{y, w, z}; }
  Vec3 ZXY() { return Vec3{z, x, y}; }
  Vec3 ZXW() { return Vec3{z, x, w}; }
  Vec3 ZYX() { return Vec3{z, y, x}; }
  Vec3 ZYW() { return Vec3{z, y, w}; }
  Vec3 ZWX() { return Vec3{z, w, x}; }
  Vec3 ZWY() { return Vec3{z, w, y}; }
  Vec3 WXY() { return Vec3{w, x, y}; }
  Vec3 WXZ() { return Vec3{w, x, z}; }
  Vec3 WYX() { return Vec3{w, y, x}; }
  Vec3 WYZ() { return Vec3{w, y, z}; }
  Vec3 WZX() { return Vec3{w, z, x}; }
  Vec3 WZY() { return Vec3{w, z, y}; }

  Vec4 XYZW() { return Vec4{x, y, z, w}; }
  Vec4 XYWZ() { return Vec4{x, y, w, z}; }
  Vec4 XZYW() { return Vec4{x, z, y, w}; }
  Vec4 XZWY() { return Vec4{x, z, w, y}; }
  Vec4 XWYZ() { return Vec4{x, w, y, z}; }
  Vec4 XWZY() { return Vec4{x, w, z, y}; }
  Vec4 YXZW() { return Vec4{y, x, z, w}; }
  Vec4 YXWZ() { return Vec4{y, x, w, z}; }
  Vec4 YZXW() { return Vec4{y, z, x, w}; }
  Vec4 YZWX() { return Vec4{y, z, w, x}; }
  Vec4 YWXZ() { return Vec4{y, w, x, z}; }
  Vec4 YWZX() { return Vec4{y, w, z, x}; }
  Vec4 ZXYW() { return Vec4{z, x, y, w}; }
  Vec4 ZXWY() { return Vec4{z, x, w, y}; }
  Vec4 ZYXW() { return Vec4{z, y, x, w}; }
  Vec4 ZYWX() { return Vec4{z, y, w, x}; }
  Vec4 ZWXY() { return Vec4{z, w, x, y}; }
  Vec4 ZWYX() { return Vec4{z, w, y, x}; }
  Vec4 WXYZ() { return Vec4{w, x, y, z}; }
  Vec4 WXZY() { return Vec4{w, x, z, y}; }
  Vec4 WYXZ() { return Vec4{w, y, x, z}; }
  Vec4 WYZX() { return Vec4{w, y, z, x}; }
  Vec4 WZXY() { return Vec4{w, z, x, y}; }
  Vec4 WZYX() { return Vec4{w, z, y, x}; }

  private:
  T x, y, z, w;
};