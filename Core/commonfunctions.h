#pragma once
#include <QString>
#include <QColor>
#include <QPointF>
#include <QTransform>
#include <QUuid>
#include <QList>
#include <QVector>
#include "FindContours.h"
#include "poly2tri.h"

#define MASK_BIT0	1
#define MASK_BIT1	2
#define MASK_BIT2	4
#define MASK_BIT3	8
#define MASK_BIT4	16
#define MASK_BIT5	32
#define MASK_BIT6	64
#define MASK_BIT7	128
#define _PI_ 3.14159265

namespace CDI
{
	class Polygon2D;

	// Program constant
	#define CDI_MAX_FLOAT FLT_MAX;
	#define CDI_MIN_FLOAT FLT_MIN;

	// Mask values
	const unsigned int isModified		= MASK_BIT0;
	const unsigned int isHidden			= MASK_BIT1;
	const unsigned int isShared			= MASK_BIT2;
	const unsigned int isTransformed	= MASK_BIT3;
	const unsigned int isLocked			= MASK_BIT4;
	// Add as per requirement

	// IMPORTANT - Do not change from CAPS. It creates conflict with class name
	// Also, MUST update the getItemType() and getSelectionType() function to reflect the changes
    enum ItemType : int {
		STROKE = 10,	// geometry
		IMAGE,
		POLYGON2D,		// Nothing but objects with holes. Maybe triangulated
		SEARCHRESULT,	// display items
		PHYSICSBODY,	// physics items
		PHYSICSJOINT,
		PHYSICSSHAPE,
		COMPONENT,		// core components
		ASSEMBLY,
		PAGE,
		ROOT,
		NONE
    };

    enum SelectionType : int {
		OnItem,
		Inside,
		Outside,
		Nearby
	};

    ItemType getItemType(int i);
	SelectionType getSelectionType(int i);

    QString getItemNameByType(ItemType i);

    QString getHomeDirectory();

    typedef QPointF Point2D;

	float dotProduct(Point2D* p1, Point2D* p2);

    // Though cross product is a vector, it will result in only along z- direction.
    // So return type is a scalar
	float crossProduct(Point2D* p1, Point2D* p2);

	float angleBetweenPoints(Point2D* p1, Point2D* p2);

	float magnitude(Point2D* p);

	float sqrMagnitude(Point2D* p);

	bool pointFuzzyCompare(Point2D* p1, Point2D* p2);

	float sqrEuclideanDistance(Point2D* a, Point2D* b);

	float euclideanDistance(Point2D* a, Point2D* b);

    float DistancePointLine(Point2D* p, Point2D* lineStart, Point2D* lineEnd);

    bool DistanceFromLineSegment(Point2D* p, Point2D* lineStart, Point2D* lineEnd, float* distance);

	bool extractTransformComponents(QTransform &t, float* rotation, float* scale, Point2D* translation);

	bool colorCompare(QColor c1, QColor c2);

	bool isConvexPolygon(Point2D* points, int numPoints);

	QUuid uniqueHash();

	/**
	 * @brief generatePolygonFromImage creates contour from given image, simplies using RDG
	 * algorith and triangulates the resultant polygon
	 * @param imagePath Full path to image
	 * @param deltaOutside epsilon value for outer polygons (RDG algo)
	 * @param deltaInside epsilon value for internal polygons (RDG algo)
	 * @return List of convex polygons
	 * @see http://en.wikipedia.org/wiki/Ramer%E2%80%93Douglas%E2%80%93Peucker_algorithm
	 * @see http://karthaus.nl/rdp/
	 * @remarks - In the current implementation, all polygons are simply triangles.
	 * @todo - Implement decomposition to generate convex polygons with num of vertices > 3
	 */
	vector<p2t::Triangle*>/*QList<Polygon2D*>*/ generatePolygonFromImage(QString imagePath, float deltaOutside, float deltaInside);
}
