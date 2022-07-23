#include <iostream>
#include <stack>
#include <vector>
#include <string>
#include <ctime>
#include <limits>

using namespace std;
#define INF std::numeric_limits<double>::infinity()

#include "transform.h"
#include "bitmap_image.hpp"

int triangleCount;

double dx, dy;
double topY, bottomY;
double leftX, rightX;

double upX, upY, upZ;
double eyeX, eyeY, eyeZ;
double lookX, lookY, lookZ;
int screenWidth, screenHeight;
double leftLimitX, rightLimitX;
double bottomLimitY, topLimitY;
double frontLimitZ, rearLimitZ;
double fovY, aspectRatio, near, far;

Point transformPoint(Transform &t, Point &p)
{
    vd v(4, 0.0);
    vvd matrix = t.getMatrix();

    double multiplier = 0.0;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (!j)
            {
                multiplier = p.getX();
            }
            else if (j == 1)
            {
                multiplier = p.getY();
            }
            else if (j == 2)
            {
                multiplier = p.getZ();
            }
            else
            {
                multiplier = p.getW();
            }

            v[i] += matrix[i][j] * multiplier;
        }
    }

    Point point(v[0], v[1], v[2], v[3]);

    point = point.scale();
    return point;
}

vvd matrixProduct(vvd v1, vvd v2)
{
    vvd v3(4, vd(4, 0.0));
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            for (int k = 0; k < 4; k++)
            {
                v3[i][j] += v1[i][k] * v2[k][j];
            }
        }
    }

    return v3;
}

Transform getProduct(Transform &t1, Transform &t2)
{
    Transform t(4);
    t.setMatrix(matrixProduct(t1.getMatrix(), t2.getMatrix()));

    return t;
}

void stage1(std::ifstream &in, std::ofstream &out)
{
    in >> eyeX >> eyeY >> eyeZ;
    in >> lookX >> lookY >> lookZ;
    in >> upX >> upY >> upZ;
    in >> fovY >> aspectRatio >> near >> far;

    /**
     * @brief Modeling Transformation
     * Display commands are four type
     * triangle, translate, rotate & scale
     * here a stack of transfomation class is maintained
     */
    std::stack<Transform> transformStack;

    /**
     * @brief Initialize with a identity matrix
     * of size 4
     */
    transformStack.push(Transform(4));
    double a, b, c;
    std::string command;
    while (true)
    {
        in >> command;
        std::cout << command << std::endl;
        if (command == "triangle")
        {
            std::cout << "inside here" << std::endl;

            in >> a >> b >> c;
            Point p1(a, b, c);
            in >> a >> b >> c;
            Point p2(a, b, c);
            in >> a >> b >> c;
            Point p3(a, b, c);
            std::cout << p1.getX() << " " << p1.getY() << " " << p1.getZ() << std::endl;
            std::cout << p2.getX() << " " << p2.getY() << " " << p2.getZ() << std::endl;
            std::cout << p3.getX() << " " << p3.getY() << " " << p3.getZ() << std::endl;

            p1 = transformPoint(transformStack.top(), p1);
            p2 = transformPoint(transformStack.top(), p2);
            p3 = transformPoint(transformStack.top(), p3);

            out << p1 << std::endl;
            out << p2 << std::endl;
            out << p3 << std::endl;

            triangleCount++;
        }
        else if (command == "translate")
        {
            double tx, ty, tz;
            in >> tx >> ty >> tz;
            cout << tx << " " << ty << " " << tz << endl;
            Transform t(4);
            t.generateTranslationMatrix(tx, ty, tz);

            Transform newT = getProduct(transformStack.top(), t);
            transformStack.pop();
            transformStack.push(newT);
        }
        else if (command == "scale")
        {
            double sx, sy, sz;
            in >> sx >> sy >> sz;
            cout << sx << " " << sy << " " << sz << endl;

            Transform t(4);
            t.generateScaleMatrix(sx, sy, sz);

            Transform newT = getProduct(transformStack.top(), t);
            transformStack.pop();

            transformStack.push(newT);
        }
        else if (command == "rotate")
        {
            double angle, ax, ay, az;
            in >> angle >> ax >> ay >> az;
            cout << angle << " " << ax << " " << ay << " " << az << endl;

            Transform t(4);
            Point point(ax, ay, az);
            t.generateRotationMatrix(angle, point);

            Transform newT = getProduct(transformStack.top(), t);
            transformStack.pop();
            transformStack.push(newT);
        }
        else if (command == "push")
        {
            // Transform t(4);
            transformStack.push(transformStack.top());
        }
        else if (command == "pop")
        {
            if (transformStack.empty())
            {
                std::cout << "Stack is empty" << std::endl;
                exit(0);
            }
            transformStack.pop();
        }
        else if (command == "end")
        {
            break;
        }
        else
        {
            std::cout << "Invalid command" << std::endl;
            exit(0);
        }
    }

    in.close();
    out.close();
}

void stage2(std::ifstream &in, std::ofstream &out)
{
    Transform t(4), x(4);

    Point up(upX, upY, upZ);
    Point eye(eyeX, eyeY, eyeZ);
    Point look(lookX, lookY, lookZ);

    x = x.generateViewMatrix(eye, look, up);
    t = getProduct(t, x);

    double a, b, c;
    for (int i = 0; i < triangleCount; i++)
    {
        in >> a >> b >> c;
        Point p1(a, b, c);
        in >> a >> b >> c;
        Point p2(a, b, c);
        in >> a >> b >> c;
        Point p3(a, b, c);

        p1 = transformPoint(t, p1);
        p2 = transformPoint(t, p2);
        p3 = transformPoint(t, p3);

        out << p1 << std::endl;
        out << p2 << std::endl;
        out << p3 << std::endl;
        out << std::endl;
    }

    in.close();
    out.close();
}

void stage3(std::ifstream &in, std::ofstream &out)
{
    Transform t(4);
    t.generateProjectionMatrix(fovY, aspectRatio, near, far);

    double a, b, c;
    for (int i = 0; i < triangleCount; i++)
    {
        in >> a >> b >> c;
        Point p1(a, b, c);
        in >> a >> b >> c;
        Point p2(a, b, c);
        in >> a >> b >> c;
        Point p3(a, b, c);

        p1 = transformPoint(t, p1);
        p2 = transformPoint(t, p2);
        p3 = transformPoint(t, p3);

        out << p1 << std::endl;
        out << p2 << std::endl;
        out << p3 << std::endl;
        out << std::endl;
    }

    in.close();
    out.close();
}

struct Color
{
    /* data */
    int r, g, b;
};

void handleBMP(vvd &zBuffer, std::vector<std::vector<Color>> &refreshBuffer)
{
    bitmap_image bitmapImage(screenWidth, screenHeight);

    for (int i = 0; i < screenHeight; i++)
    {
        for (int j = 0; j < screenWidth; j++)
        {
            bitmapImage.set_pixel(j, i, refreshBuffer[i][j].r, refreshBuffer[i][j].g, refreshBuffer[i][j].b);
        }
    }
    bitmapImage.save_image("out.bmp");

    std::ofstream outZ("z-buffer.txt");
    if (!outZ.is_open())
    {
        exit(0);
    }

    for (int i = 0; i < screenHeight; i++)
    {
        for (int j = 0; j < screenWidth; j++)
        {
            if (zBuffer[i][j] < rearLimitZ)
            {
                outZ << zBuffer[i][j] << '\t';
            }
        }
        outZ << std::endl;
    }

    outZ.close();
}

void zBuffer(Triangle triangles[])
{
    vvd zBuffer(screenHeight, vd(screenWidth, 0.0));
    std::vector<std::vector<Color>> refreshBuffer(screenHeight, std::vector<Color>(screenWidth));

    for (int i = 0; i < screenHeight; i++)
    {
        for (int j = 0; j < screenWidth; j++)
        {
            zBuffer[i][j] = rearLimitZ;
            refreshBuffer[i][j].r = 0;
            refreshBuffer[i][j].g = 0;
            refreshBuffer[i][j].b = 0;
        }
    }

    int topS, bottomS;
    int lI, rI;
    double maxX, minX;

    for (int i = 0; i < triangleCount; i++)
    {
        if (triangles[i].maxYOfAllCords() >= topY)
        {
            topS = 0;
        }
        else
        {
            topS = (int)(round((topY - triangles[i].maxYOfAllCords()) / dy));
        }

        if (triangles[i].minYOfAllCords() <= bottomY)
        {
            bottomS = screenHeight - 1;
        }
        else
        {
            bottomS = screenHeight - (1 + ((int)round((triangles[i].minYOfAllCords() - bottomY) / dy)));
        }

        for (int r = topS; r <= bottomS; r++)
        {
            double midVal = topY - r * dy;

            Point intersect[3];
            intersect[0] = Point(INF, midVal, 0, 1);
            intersect[1] = Point(INF, midVal, 1, 2);
            intersect[2] = Point(INF, midVal, 2, 0);

            for (int j = 0; j < 3; j++)
            {
                Point p1 = triangles[i].getSide((int)intersect[j].getZ());
                Point p2 = triangles[i].getSide((int)intersect[j].getW());

                if (p1.getY() != p2.getY())
                {
                    intersect[j].setX(p1.getX() + (midVal - p1.getY()) * (p1.getX() - p2.getX()) / (p1.getY() - p2.getY()));
                }
            }

            for (int j = 0; j < 3; j++)
            {
                Point p1 = triangles[i].getSide((int)intersect[j].getZ());
                Point p2 = triangles[i].getSide((int)intersect[j].getW());

                if (intersect[j].getX() != INF)
                {
                    bool xMaxValTrue = intersect[j].getX() > std::max(p1.getX(), p2.getX());
                    bool xMinValTrue = intersect[j].getX() < std::min(p1.getX(), p2.getX());
                    bool yMaxValTrue = intersect[j].getY() > std::max(p1.getY(), p2.getY());
                    bool yMinValTrue = intersect[j].getY() < std::min(p1.getY(), p2.getY());

                    if ((xMaxValTrue || xMinValTrue) || (yMaxValTrue || yMinValTrue))
                    {
                        intersect[j].setX(INF);
                    }

                    int maxIndex = -1, minIndex = -1;

                    for (int j = 0; j < 3; j++)
                    {
                        if (maxIndex == -1 && minIndex == -1)
                        {
                            if (intersect[j].getX() != INF)
                            {
                                maxIndex = minIndex = j;
                                maxX = minX = intersect[j].getX();
                            }
                        }
                        else
                        {
                            if (intersect[j].getX() != INF)
                            {
                                if (intersect[j].getX() < minX)
                                {
                                    minIndex = j;
                                    minX = intersect[j].getX();
                                }
                                if (intersect[j].getX() > maxX)
                                {
                                    maxIndex = j;
                                    maxX = intersect[j].getX();
                                }
                            }
                        }
                    }

                    if (intersect[minIndex].getX() <= leftX)
                    {
                        lI = 0;
                    }
                    else
                    {
                        lI = (int)round((intersect[minIndex].getX() - leftX) / dx);
                    }
                    if (intersect[maxIndex].getX() >= rightX)
                    {
                        rI = screenWidth - 1;
                    }
                    else
                    {
                        rI = screenWidth - (1 + ((int)round((rightX - intersect[maxIndex].getX()) / dx)));
                    }

                    int index1 = (int)intersect[minIndex].getZ();

                    int index2 = (int)intersect[minIndex].getW();

                    Point p1 = triangles[i].getSide(index1);
                    Point p2 = triangles[i].getSide(index2);

                    double za = p1.getZ() + (intersect[minIndex].getY() - p1.getY()) * (p2.getZ() - p1.getZ()) / (p2.getY() - p1.getY());

                    index1 = (int)intersect[maxIndex].getZ();
                    index2 = (int)intersect[maxIndex].getW();

                    p1 = triangles[i].getSide(index1);
                    p2 = triangles[i].getSide(index2);

                    double zb = p1.getZ() + (intersect[maxIndex].getY() - p1.getY()) * (p2.getZ() - p1.getZ()) / (p2.getY() - p1.getY());

                    double zVal = 0.0;
                    for (int k = lI; k <= rI; k++)
                    {
                        if (k == lI)
                        {
                            zVal = za + ((leftX + lI * dx) - intersect[minIndex].getX()) * (zb - za) / (intersect[maxIndex].getX() - intersect[minIndex].getX());
                        }
                        else
                        {
                            zVal = zVal + (dx * (zb - za) / (intersect[maxIndex].getX() - intersect[minIndex].getX()));
                        }

                        /**
                         * @brief z value was calculated
                         * here
                         */
                        if (zVal > frontLimitZ && zVal < zBuffer[r][k])
                        {
                            zBuffer[r][k] = zVal;

                            refreshBuffer[r][k].r = triangles[i].getColor('r');
                            refreshBuffer[r][k].g = triangles[i].getColor('g');
                            refreshBuffer[r][k].b = triangles[i].getColor('b');
                        }
                    }
                }
            }
        }
    }

    handleBMP(zBuffer, refreshBuffer);
}

void stage4(std::ifstream &ins, std::ifstream &inc)
{
    inc >> screenWidth >> screenHeight;
    inc >> leftLimitX;
    inc >> bottomLimitY;
    inc >> frontLimitZ >> rearLimitZ;

    rightLimitX = -leftLimitX;
    topLimitY = -bottomLimitY;

    dx = (rightLimitX - leftLimitX) / screenWidth;
    dy = (topLimitY - bottomLimitY) / screenHeight;
    topY = topLimitY - dy / 2.0;
    bottomY = bottomLimitY + dy / 2.0;
    leftX = leftLimitX + dx / 2.0;
    rightX = rightLimitX - dx / 2.0;

    Triangle triangles[triangleCount];
    srand(time(0));
    Point s1, s2, s3;
    double a, b, c;

    for (int i = 0; i < triangleCount; i++)
    {
        ins >> a >> b >> c;
        Point p1(a, b, c);
        s1 = p1;
        ins >> a >> b >> c;
        Point p2(a, b, c);
        s3 = p2;
        ins >> a >> b >> c;
        Point p3(a, b, c);
        s3 = p3;
        /// inst >> s2;
        /// inst >> s3;
        triangles[i].setCords(s1, s2, s3);
        triangles[i].setColor(rand() % 256, rand() % 256, rand() % 256);
    }

    zBuffer(triangles);

    ins.close();
    inc.close();
}

int main(int argc, char **argv)
{
    srand(time(0));

    std::ifstream in("scene.txt");
    if (!in.is_open())
    {
        std::cout << "Error opening input file" << std::endl;
        exit(0);
    }

    std::ofstream out("stage1.txt");
    if (!out.is_open())
    {
        std::cout << "Error opening output file" << std::endl;
        exit(0);
    }

    stage1(in, out);

    in.open("stage1.txt");
    if (!in.is_open())
    {
        std::cout << "Error opening input file" << std::endl;
        exit(0);
    }

    out.open("stage2.txt");
    if (!out.is_open())
    {
        std::cout << "Error opening output file" << std::endl;
        exit(0);
    }
    stage2(in, out);

    in.open("stage2.txt");
    if (!in.is_open())
    {
        std::cout << "Error opening input file" << std::endl;
        exit(0);
    }

    out.open("stage3.txt");
    if (!out.is_open())
    {
        std::cout << "Error opening output file" << std::endl;
        exit(0);
    }
    stage3(in, out);

    std::ifstream ins("stage3.txt");
    if (!ins.is_open())
    {
        std::cout << "Error opening input file" << std::endl;
        exit(0);
    }

    std::ifstream inc("config.txt");
    if (!inc.is_open())
    {
        std::cout << "Error opening input file" << std::endl;
        exit(0);
    }

    // out.open("z_buffer.txt");
    // if (!out.is_open())
    // {
    //     std::cout << "Error opening output file" << std::endl;
    //     exit(0);
    // }
    stage4(ins, inc);

    return 0;
}