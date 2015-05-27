/*
 * Author: Pramod Kumar
 * Created on: May 21, 2015
 * pramod493@gmail.com
*/
#include <math.h>
#include <algorithm>
#include "commonfunctions.h"

namespace CDI
{
	////////////// ENUM conversion from int
	ItemType getItemType(int i)
	{
		switch (i)
		{
		case ItemType::STROKE :
			return ItemType::STROKE;
		case ItemType::IMAGE :
			return ItemType::IMAGE;
		case ItemType::SEARCHRESULT :
			return ItemType::SEARCHRESULT;
		case ItemType::PHYSICSBODY :
			return ItemType::PHYSICSBODY;
		case ItemType::PHYSICSJOINT :
			return ItemType::PHYSICSJOINT;
		case ItemType::COMPONENT :
			return ItemType::COMPONENT;
		case ItemType::PAGE :
			return ItemType::PAGE;
		case ItemType::ROOT :
			return ItemType::ROOT;
		}
		return ItemType::NONE;
	}

	SelectionType getSelectionType(int i)
	{
		// This function is not needed since we are not going to serialize the selection type
		switch (i)
		{
		case SelectionType::OnItem :
			return SelectionType::OnItem;
		case SelectionType::Inside :
			return SelectionType::Inside;
		case SelectionType::Outside :
			return SelectionType::Outside;
		case SelectionType::Nearby :
			return SelectionType::Nearby;
		}
		return SelectionType::Nearby;
	}

	////////////////////////////////////////

	float dotProduct(Point2D* p1, Point2D* p2)
    {
        return (p1->x() * (p2->x()) + p1->y() * (p2->y()));
    }

	float crossProduct(Point2D* p1, Point2D* p2)
    {
		return (p1->x() * (p2->y()) - p2->x() * (p1->y()) );
    }

    float AngleBetweenPoints(Point2D* p1, Point2D* p2)
    {
        return atan ((p2->y()-p1->y())/(p2->x()-p1->x()));
    }

	float magnitude(Point2D* p)
    {
        return sqrt(p->x() * (p->x()) + p->y() * (p->y()) );
    }

	float sqrMagnitude(Point2D* p)
	{
		return (p->x() * (p->x()) + p->y() * (p->y()) );
	}

	bool pointFuzzyCompare(Point2D* p1, Point2D* p2)
    {
		return qFuzzyCompare(p1->x(), p2->x()) && qFuzzyCompare(p1->y(), p2->y());
    }

	float sqrEuclideanDistance(Point2D* a, Point2D* b)
    {
        return (a->x() - b->x())*(a->x() - b->x()) +
                (a->y() - b->y())*(a->y() - b->y());
    }

	float euclideanDistance(Point2D* a, Point2D* b)
    {
		return sqrt(sqrEuclideanDistance(a,b));
    }

    float DistancePointLine(Point2D* p, Point2D* lineStart, Point2D* lineEnd)
    {
        Point2D V1 = Point2D(p->x()-lineStart->x(), p->y()-lineStart->y());
        Point2D V2 = Point2D(lineEnd->x()-lineStart->x(),lineEnd->y()-lineStart->y());
        float sqrMag = (V2.x()*V2.x() + V2.y()*V2.y());

        // dot product
        float t = (V1.x()*V2.x() + V1.y()*V2.y()) / sqrMag;
//		if (t < 0.0 || t > 1.0) continue;

        // check normal distance
        V2 *= t;	// set to coord of projection point
        float dist = (V1.x()-V2.x())*(V1.x()-V2.x()) + (V1.y()-V2.y())*(V1.y()-V2.y());
        return dist;
    }

    bool DistanceFromLineSegment(Point2D* p, Point2D* lineStart, Point2D* lineEnd, float* distance)
    {
        bool OnSegment = true;
        Point2D V1 = Point2D(p->x()-lineStart->x(), p->y()-lineStart->y());
        Point2D V2 = Point2D(lineEnd->x()-lineStart->x(),lineEnd->y()-lineStart->y());
        float sqrMag = (V2.x()*V2.x() + V2.y()*V2.y());

        // dot product
        float t = (V1.x()*V2.x() + V1.y()*V2.y()) / sqrMag;
        if (t < 0.0 || t > 1.0) OnSegment = false;


        if (t < 0.0)
        {
			*distance = euclideanDistance(p, lineStart);
        } else if (t > 1.0)
        {
			*distance = euclideanDistance(p, lineEnd);
        } else {
            // check normal distance
            V2 *= t;    // set to coord of projection point
			*distance = euclideanDistance(&V1, &V2);
        }
        return OnSegment;
    }

    bool colorCompare(QColor c1, QColor c2)
    {
         return (c1.red()== c2.red()) && (c1.green() == c2.green()) && (c1.blue() == c2.blue());
    }

	QUuid uniqueHash()
	{
		return QUuid::createUuid();
	}
}
