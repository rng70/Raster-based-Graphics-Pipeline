#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include <vector>
#include "Points.h"

#define PI (2.0 * acos(0.0))

class Transform
{
private:
    std::vector<std::vector<double>> matrix;

    Point cross(const Point &u, const Point &v)
    {
        return Point(u.cy * v.cz - u.cz * v.cy, u.cz * v.cx - u.cx * v.cz, u.cx * v.cy - u.cy * v.cx);
    }

    Point RodriguesFormula(Point x, Point a, double theta)
    {
        return (x * cos(theta * PI / 180.0) + a * (a * x) * (1 - cos(theta * PI / 180.0)) + (cross(a, x)) * sin(theta * PI / 180.0));
    }

public:
    Transform(int n)
    {
        matrix.resize(n, std::vector<double>(n, 0));
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                if (i == j)
                    matrix[i][j] = 1;
            }
        }
    }

    void generateTranslationMatrix(double tx, double ty, double tz){
        matrix[0][3] = tx;
        matrix[1][3] = ty;
        matrix[2][3] = tz;
    }

    void generateScaleMatrix(double sx, double sy, double sz){
        matrix[0][0] = sx;
        matrix[1][1] = sy;
        matrix[2][2] = sz;
    }

    void generateRotationMatrix(double angle, const Point& a){
        a = a.normalize();

        Point x = Point(1, 0, 0);
        Point y = Point(0, 1, 0);
        Point z = Point(0, 1, 0);

        Point c1 = RodriguesFormula(x, a, angle);
        Point c2 = RodriguesFormula(y, a, angle);
        Point c3 = RodriguesFormula(z, a, angle);

        /* generating corresponding rotation matrix R */
        matrix[0][0] = c1.getX();
        matrix[1][0] = c1.getY();
        matrix[2][0] = c1.getZ();

        matrix[0][1] = c2.getX();
        matrix[1][1] = c2.getY();
        matrix[2][1] = c2.getZ();

        matrix[0][2] = c3.getX();
        matrix[1][2] = c3.getY();
        matrix[2][2] = c3.getZ();
    }

    Point operator*(const Point point)
    {
        /* 4x4 & 4x1 matrices multiplication */
        double temp[4];

        for (int i = 0; i < 4; i++)
        {
            temp[i] = 0.0;

            for (int j = 0; j < 4; j++)
            {
                temp[i] += matrix[i][j] * ((j == 0) ? point.getX() : ((j == 1) ? point.getY() : ((j == 2) ? point.getZ() : point.getW())));
            }
        }
        return Point(temp[0], temp[1], temp[2], temp[3]);
    }
};

#endif // _TRANSFORM_H_