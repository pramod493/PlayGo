#include <vector>
#include <algorithm>
#include <iterator>
#include <cstdlib>
#include "poly2tri.h"
#include <QDebug>

using namespace p2t;
using namespace std;

void testPoly2Tri();

template <class C> void FreeClear( C & cntr ) {
	for ( typename C::iterator it = cntr.begin();
			  it != cntr.end(); ++it ) {
		delete * it;
	}
	cntr.clear();
}

QDebug operator <<(QDebug d, const p2t::Point& p)
{
	d.nospace() << "(" << p.x << ", " << p.y << ")\n";
	return d;
}

void testPoly2Tri()
{
	vector<vector<p2t::Point*>> polylines;
	vector<Triangle*> triangles;

	vector<p2t::Point*> polyline;

	polyline.push_back(new p2t::Point(1,2));
	polyline.push_back(new p2t::Point(2,2));
	polyline.push_back(new p2t::Point(7,6));
	polyline.push_back(new p2t::Point(2,9));
	polyline.push_back(new p2t::Point(5,6));
	polyline.push_back(new p2t::Point(0,9));

	qDebug() << "Created polyline with " << polyline.size() << "points";

	vector<p2t::Point*> hole;
	hole.push_back(new p2t::Point(1.5,2.1));
	hole.push_back(new p2t::Point(1.9,2.1));
	hole.push_back(new p2t::Point(4,5));
	hole.push_back(new p2t::Point(1,5));

	polylines.push_back(polyline);

	CDT* cdt = new CDT(polyline);
	cdt->AddHole(hole);

	cdt->Triangulate();
	triangles = cdt->GetTriangles();

	qDebug() << "Number of triangles after triangulation: "<< triangles.size();
	for (int i=0; i< triangles.size(); i++)
	{
		qDebug() << "Triangle " << i;
		triangles[i]->DebugPrint();
	}

	delete cdt;
	for(int i = 0; i < polylines.size(); i++)
	{
		vector<p2t::Point*> poly = polylines[i];
		FreeClear(poly);
	}
}
