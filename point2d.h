#pragma once
namespace CDI
{
    class Point2D
    {
    public:
        float x;
        float y;

        Point2D();

        Point2D(float X,float Y);

        Point2D(Point2D& orig);
    };
}
