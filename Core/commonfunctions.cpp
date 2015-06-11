/*
 * Author: Pramod Kumar
 * Created on: May 21, 2015
 * pramod493@gmail.com
*/
#include <math.h>
#include <algorithm>
#include "commonfunctions.h"
#include "polygon2d.h"
#include "ramerdouglaspeucker.h"
#include "poly2tri.h"
#include <vector>
#include "FindContours.h"

using namespace std;

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
		case ItemType::POLYGON2D:
			return ItemType::POLYGON2D;
		case ItemType::SEARCHRESULT :
			return ItemType::SEARCHRESULT;
		case ItemType::PHYSICSBODY :
			return ItemType::PHYSICSBODY;
		case ItemType::PHYSICSJOINT :
			return ItemType::PHYSICSJOINT;
		case ItemType::COMPONENT :
			return ItemType::COMPONENT;
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

	bool isConvexPolygon(Point2D *points, int numPoints)
	{
		if (numPoints <= 3) return true;	// triangle are always convex
		// Assuming all the points form a polygon
		// reference
		// https://github.com/pramod493/box2d-editor/blob/eb5aec8cfb26ceeda811398200d680d9baeceb86/editor/src/aurelienribon/bodyeditor/maths/earclipping/ewjordan/Polygon.java
		bool isPositive = false;
		for (int i =0; i < numPoints; i++)
		{
			int lower = (i==0) ? (numPoints-1) : (i-1);
			int middle = i;
			int upper = (i== numPoints-1) ? (0) : (i+1);

			float dx0 = points[middle].x() - points[lower].x();
			float dy0 = points[middle].y() - points[lower].y();

			float dx1 = points[upper].x() - points[middle].y();
			float dy1 = points[upper].y() - points[middle].y();

			float cross = dx0*dy1 - dx1*dy0;

			// Cross product of all adjacent sides should have same value
			bool newIsP = (cross > 0) ? true : false;
			if (i==0)
				isPositive = newIsP;	// set first one as initial value
			else if (isPositive != newIsP)
				return false;
		}
		return true;
	}

	/**
	 * @brief uniqueHash returns a QUuid for identification. Assumed to be unique
	 * @return Unique QUuid identifier
	 */
	QUuid uniqueHash()
	{
		return QUuid::createUuid();
	}

	vector<p2t::Triangle*> triangularizeImage(QString imagePath, float deltaOutside, float deltaInside)
	{
		vector<p2t::Triangle*> triangles;
		vector<vector<cv::Point> > outerContours;
		vector<vector<vector<cv::Point> > > allInnerContours;

		gbFindContours(imagePath.toStdString(), outerContours, allInnerContours, false/*NO DEBUG*/);

		// Sanity check
		if (outerContours.size()!= allInnerContours.size())
		{
			qDebug() << "Mismatch in the number of loops @generatePolygonFromImage";
			return triangles;
		}

		// Initialize RDP object
		CDI::RamerDouglas rdp = CDI::RamerDouglas();

		// Iterate for each outer contour
		for (int i=0; i< outerContours.size(); i++)
		{
			// 1. Get outer loop and corresponding inner loop
			vector<cv::Point> outerContour = outerContours[i];
			vector<vector<cv::Point>> innerContours = allInnerContours[i];

			// 2. Convert into format accepted by RDP algo
			vector<p2t::Point> tmp_contour;
			tmp_contour.reserve(outerContour.size());
			size_t max_points = outerContour.size();
			for (size_t j=0; j< max_points ; j++)
			{
				cv::Point pt = outerContour[i];
				tmp_contour.push_back(p2t::Point(pt.x, pt.y));
			}

			// 3. Simplify outer loop with RDp
			vector<p2t::Point> simplied_outerContour = rdp.simplifyWithRDP(tmp_contour, deltaOutside);

			// prepare the smoothed polyline for poly2tri
			vector<p2t::Point*> p2t_polyline;
			p2t_polyline.reserve(simplied_outerContour.size());
			max_points = simplied_outerContour.size();
			for (size_t j = 0; j < max_points; j++)
			{
				p2t::Point pt = simplied_outerContour[j];
				p2t_polyline.push_back(new p2t::Point(pt.x,pt.y));
			}

			// Initialize CDT with simplified contour
			p2t::CDT cdt = p2t::CDT(p2t_polyline);

			// Create a hole for each of the inner loop
			size_t max_inner_loops = allInnerContours.size();
			for (size_t j=0; j < max_inner_loops; j++)
			{
				vector<cv::Point> innerContour = innerContours[j];
				// Apply RDG
				vector<p2t::Point> tmp_contour;
				tmp_contour.reserve(innerContour.size());
				size_t max_points = innerContour.size();
				for (size_t j=0; j< max_points ; j++)
				{
					cv::Point pt = innerContour[i];
					tmp_contour.push_back(p2t::Point(pt.x, pt.y));
				}

				// Apply RDP on inner loop
				vector<p2t::Point> simplified_innerContour = rdp.simplifyWithRDP(tmp_contour, deltaInside);

				vector<p2t::Point*> p2t_hole;
				p2t_hole.reserve(simplied_outerContour.size());
				size_t max_points_in_hole = simplied_outerContour.size();
				for (size_t k = 0; k < max_points_in_hole; k++)
				{
					p2t::Point pt = simplified_innerContour[k];
					p2t_hole.push_back(new p2t::Point(pt.x,pt.y));
				}
				cdt.AddHole(p2t_hole);
			}

			// Triangulate
			cdt.Triangulate();

			// Append the traingulated results to the vector
			vector<p2t::Triangle*> tmp_trias = cdt.GetTriangles();
			for(int m =0; m < tmp_trias.size(); m++)
			{
				triangles.push_back(tmp_trias[m]);
			}
		}

		return triangles;
	}

	QList<Polygon2D*> generatePolygonFromImage(QString imagePath, float deltaOutside, float deltaInside)
	{
		// Initialize the list
		QList<Polygon2D*> polygons;
		vector<p2t::Triangle*> triangles = triangularizeImage(imagePath, deltaOutside, deltaInside);
		for (int i=0; i <triangles.size(); i++)
		{
			p2t::Triangle* tria = triangles[i];

			Polygon2D* polygon = new Polygon2D();
			p2t::Point *pt = 0;
			pt = tria->GetPoint(0);	polygon->push_back(Point2D(pt->x,pt->y));
			pt = tria->GetPoint(1);	polygon->push_back(Point2D(pt->x,pt->y));
			pt = tria->GetPoint(2);	polygon->push_back(Point2D(pt->x,pt->y));

			polygons.push_back(polygon);
		}
		return polygons;
	}
}
