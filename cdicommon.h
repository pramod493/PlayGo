#pragma once

#include <math.h>
#include <algorithm>
#include <vector>
#include "point2d.h"

#define CDI_MAX_FLOAT  100000000
#define CDI_MIN_FLOAT -100000000

#define CDI_TRANSFORM_DIRTY     0x01
#define CDI_SHAPE_DIRTY         0x02
#define CDI_HIDE_IN_DISPLAY     0x04
#define CDI_HIDE_IN_PHYSICS     0x08


struct DeleteFromVector
{
    template <class T>
    void operator() ( T* ptr) const
    {
        delete ptr;
    }
};

namespace CDI
{
    bool FuzzyIsNull(float f);

    float SqrEuclideanDistance(Point2D* a, Point2D* b);

    float EuclideanDistance(Point2D* a, Point2D* b);

    class Item
    {
    public:
        enum Type {
            EMPTY,
            POINT,        /*primitives*/
            STROKE,
            LINE,           // UNUSED
            POLYGON,        // UNUSED
            ELLIPSE,        // UNUSED
            COMPOUND_SHAPE, // UNUSED
            BLOB,               // Closed stroke
            IMAGE,              // Image
            POLYGON_COLLIDER,   // Physics engine
            RIGID_BODY,
            HINGE_CONNECTOR,
            SLIDER_CONNECTOR,
            HINGE_MOTOR,
            BODY_FORCE,
            BODY_TORQUE,
            PHYSICS_MATERIAL
        };

        virtual Type GetType() {return Type::EMPTY;}
    };

    class Shape : public Item
    {
    public:
        virtual Shape* Clone()=0;

    };
}
