#ifndef _POINTS_H_
#define _POINTS_H_

#include <cmath>
#include <fstream>
#include <cstdlib>
#include <iomanip>

class Point
{
private:
    double cx, cy, cz;
    double cw;
    double magnitude;

public:
    Point() : cx(0), cy(0), cz(0), cw(0){};

    Point(double cx, double cy, double cz)
    {
        this->cx = cx;
        this->cy = cy;
        this->cz = cz;
        this->cw = 1;
        this->magnitude = sqrt(pow(cx, 2.0) + pow(cy, 2.0) + pow(cz, 2.0));
    }

    Point(double cx, double cy, double cz, double cw)
    {
        this->cx = cx;
        this->cy = cy;
        this->cz = cz;
        this->cw = cw;
    }

    double getX()
    {
        return cx;
    }

    double getY()
    {
        return cy;
    }

    double getZ()
    {
        return cz;
    }

    double getW()
    {
        return cw;
    }

    void setX(double cx)
    {
        this->cx = cx;
    }

    void setY(double cy)
    {
        this->cy = cy;
    }

    void setZ(double cz)
    {
        this->cz = cz;
    }

    void setW(double cw)
    {
        this->cw = cw;
    }

    Point operator+(const Point &p)
    {
        return Point(cx + p.cx, cy + p.cy, cz + p.cz);
    }

    Point operator-(const Point &p)
    {
        return Point(cx - p.cx, cy - p.cy, cz - p.cz);
    }

    Point operator*(const Point &p)
    {
        return Point(cx * p.cx, cy * p.cy, cz * p.cz);
    }

    void operator=(const Point &p)
    {
        this->cx = p.cx;
        this->cy = p.cy;
        this->cz = p.cz;
        this->cw = p.cw;
    }

    Point cross(const Point &u, const Point &v)
    {
        return Point(u.cy * v.cz - u.cz * v.cy, u.cz * v.cx - u.cx * v.cz, u.cx * v.cy - u.cy * v.cx);
    }

    Point normalize()
    {
        return Point(cx / magnitude, cy / magnitude, cz / magnitude);
    }

    Point scale()
    {
        return Point(cx / cw, cy / cw, cz / cw);
    }

    friend std::ifstream &operator>>(std::ifstream &in, Point &p)
    {
        in >> p.cx >> p.cy >> p.cz >> p.cw;
        return in;
    }

    friend std::ofstream &operator<<(std::ofstream &out, const Point &p)
    {
        out << fixed << setprecision(5) << p.cx << " " << p.cy << " " << p.cz;
        return out;
    }
};

class Triangle
{
    Point cords[3];
    struct Color
    {
        int r, g, b;
    } color;

public:
    void setCords(Point a, Point b, Point c)
    {
        cords[0] = a;
        cords[1] = b;
        cords[2] = c;
    }

    void setColor(int a, int b, int c)
    {
        color.r = a;
        color.g = b;
        color.b = c;
    }

    double maxYOfAllCords()
    {
        return std::max(cords[0].getY(), std::max(cords[1].getY(), cords[2].getY()));
    }

    double minYOfAllCords()
    {
        return std::min(cords[0].getY(), std::min(cords[1].getY(), cords[2].getY()));
    }

    Point getSide(int index)
    {
        return cords[index];
    }

    int getColor(char a)
    {
        return a == 'b' ? color.b : (a == 'r' ? color.r : color.g);
    }
};

#endif // _POINTS_H_
