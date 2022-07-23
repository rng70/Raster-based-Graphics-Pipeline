#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include <vector>
#include "points.h"

#define vd std::vector<double>
#define vvd std::vector<std::vector<double>>

#define PI (2.0 * acos(0.0))

class Transform
{
private:
    std::vector<std::vector<double>> matrix;

    Point cross(Point u, Point v)
    {
        return Point(u.getY() * v.getZ() - u.getZ() * v.getY(), u.getZ() * v.getX() - u.getX() * v.getZ(), u.getX() * v.getY() - u.getY() * v.getX());
    }

    Point scale(Point x, double val)
    {
        return Point(x.getX() * val, x.getY() * val, x.getZ() * val);
    }

    Point multiply(Point x, Point y)
    {
        return Point(x.getX() * y.getX(), x.getY() * y.getY(), x.getZ() * y.getZ());
    }

    Point RodriguesFormula(Point x, Point a, double theta)
    {
        return (scale(x, cos(theta * PI / 180.0)) + multiply(a, scale(multiply(a, x), (1 - cos(theta * PI / 180.0)))) + scale((cross(a, x)), sin(theta * PI / 180.0)));
    }

public:
    Transform(int n)
    {
        matrix.resize(n, std::vector<double>(n, 0.0));
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                if (i == j)
                    matrix[i][j] = 1;
                else
                {
                    matrix[i][j] = 0;
                }
            }
        }
    }

    void generateTranslationMatrix(double tx, double ty, double tz)
    {
        matrix[0][3] = tx;
        matrix[1][3] = ty;
        matrix[2][3] = tz;
    }

    void generateScaleMatrix(double sx, double sy, double sz)
    {
        matrix[0][0] = sx;
        matrix[1][1] = sy;
        matrix[2][2] = sz;
    }

    void insertMatrix(vvd &matrix, Point &a, Point &b, Point &c)
    {
        /* generating corresponding rotation matrix R */
        matrix[0][0] = a.getX();
        matrix[1][0] = a.getY();
        matrix[2][0] = a.getZ();

        matrix[0][1] = b.getX();
        matrix[1][1] = b.getY();
        matrix[2][1] = b.getZ();

        matrix[0][2] = c.getX();
        matrix[1][2] = c.getY();
        matrix[2][2] = c.getZ();
    }

    void generateRotationMatrix(double angle, Point &a)
    {
        a = a.normalize();

        Point x = Point(1.0, 0, 0);
        Point y = Point(0, 1.0, 0);
        Point z = Point(0, 0, 1.0);

        Point c1 = RodriguesFormula(x, a, angle);
        Point c2 = RodriguesFormula(y, a, angle);
        Point c3 = RodriguesFormula(z, a, angle);

        insertMatrix(matrix, c1, c2, c3);
    }

    Transform generateViewMatrix(Point eye, Point look, Point up)
    {
        Point l = look - eye;
        l = l.normalize();

        Point r = cross(l, up);
        r = r.normalize();

        Point u = cross(r, l);
        // u = u.normalize();

        Transform t(4);
        t.generateTranslationMatrix(-eye.getX(), -eye.getY(), -eye.getZ());

        matrix[0][0] = r.getX();
        matrix[0][1] = r.getY();
        matrix[0][2] = r.getZ();

        matrix[1][0] = u.getX();
        matrix[1][1] = u.getY();
        matrix[1][2] = u.getZ();

        matrix[2][0] = -l.getX();
        matrix[2][1] = -l.getY();
        matrix[2][2] = -l.getZ();

        return t;
    }

    void generateProjectionMatrix(double fovY, double aspectRatio, double near, double far)
    {
        /* determining parameters fovX, t & r */
        double fovX = fovY * aspectRatio;
        double t = near * tan(fovY / 2.0 * PI / 180.0);
        double r = near * tan(fovX / 2.0 * PI / 180.0);

        /* generating corresponding projection matrix P */
        matrix[0][0] = near / r;
        matrix[1][1] = near / t;
        matrix[2][2] = -(far + near) / (far - near);
        matrix[2][3] = -(2.0 * far * near) / (far - near);
        matrix[3][2] = -1.0;
        matrix[3][3] = 0.0;
    }

    std::vector<std::vector<double>> getMatrix()
    {
        return this->matrix;
    }

    void setMatrix(std::vector<std::vector<double>> matrix)
    {
        this->matrix = matrix;
    }
};

#endif // _TRANSFORM_H_