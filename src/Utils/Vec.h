#pragma once
#include <math.h> 
#include <stdlib.h> 
#include <stdio.h>
#include <BWAPI.h>

struct Vec {
    friend std::ostream& operator <<(std::ostream& os, const Vec& v);
    double x, y;
    Vec(double x_=0.0, double y_=0.0){ x=x_; y=y_; }
    Vec(const Vec& v){ x=v.x; y=v.y; }
    Vec(const BWAPI::Position& p){ x=p.x(); y=p.y(); }
    Vec operator+(const Vec &b) const { return Vec(x+b.x,y+b.y); }
    Vec operator+(const BWAPI::Position& p) { return BWAPI::Position(p.x() + (int)x, p.y() + (int)y); }
    Vec operator+=(const Vec &b) { x += b.x; y += b.y; return *this; }
    Vec operator+=(const BWAPI::Position& p) { x += p.x(); y += p.y(); return *this; }
    Vec operator-(const Vec &b) const { return Vec(x-b.x,y-b.y); }
    Vec operator*(double b) const { return Vec(x*b,y*b); }
    Vec operator*=(const double b) { x *= b; y *= b; return *this; }
    Vec operator/(double b) const { return Vec(x/b,y/b); }
    Vec operator/=(const double s) { x /= s; y /= s; return *this; }
    bool operator==(const Vec &b) { return (x==b.x && y==b.y); }
    Vec mult(const Vec &b) const { return Vec(x*b.x,y*b.y); }
    Vec& normalize() { return (x == 0 && y == 0) ? *this : *this = *this * (1/sqrt(x*x+y*y)); }
    double norm() const { return sqrt(x*x+y*y); } 
    double dot(const Vec &b) const { return x*b.x+y*b.y; } // cross:
    BWAPI::Position toPosition() const { return BWAPI::Position( (int)x, (int)y);};
    Vec operator%(Vec&b){return Vec(-y*b.x-x*b.y, x*b.y-y*b.x);}
    BWAPI::Position translate(const BWAPI::Position& p) const { return BWAPI::Position(p.x() + (int)x, p.y() + (int)y); }
    std::ostream& operator <<(std::ostream& os) const { os << x << " " << y; return os; }
};

inline std::ostream& operator <<(std::ostream& os, const Vec& v) { os << v.x << " " << v.y; return os; }
