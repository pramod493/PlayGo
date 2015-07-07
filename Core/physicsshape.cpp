#include "physicsshape.h"
#include "image.h"
#include <QPixmap>

namespace CDI
{
	PhysicsShape::PhysicsShape()
	{
		_id = uniqueHash();
		_currentscale = 1.0f;
		scaledToPhysics = false;
	}

	PhysicsShape::PhysicsShape(vector<p2t::Triangle *> p2tTrias)
	{
		_id = uniqueHash();
		_currentscale = 1.0f;
		scaledToPhysics = false;

		int num_trias = p2tTrias.size();
		for (int i=0; i < num_trias; i++)
		{
			Triangle* tria = new Triangle(p2tTrias[i]);
			trias.push_back(tria);
		}
	}

	PhysicsShape::~PhysicsShape()
	{
		for (int i=0; i< trias.size(); i++)
		{
			delete trias[i];
			trias[i] = 0;
		}
	}

	ItemType PhysicsShape::type() const
	{
		return PHYSICSSHAPE;
	}

	QUuid PhysicsShape::id() const
	{
		return _id;
	}

	QDataStream& PhysicsShape::serialize(QDataStream &stream) const
	{
		stream << _id;
		stream << _currentscale;
		int size = trias.size();
		stream << size;
		for (int i=0; i< size; i++)
		{
			Triangle* t = trias[i];
			t->serialize(stream);
		}
		return stream;
	}

	QDataStream& PhysicsShape::deserialize(QDataStream &stream)
	{
		// NOTE - Do I need to clear all the triangles in the shape before
		// reading data?
		stream >> _id;
		stream >> _currentscale;
		int size =0;
		stream >> size;
		if (size!= 0)
			for (int i=0; i< size; i++)
			{
				Triangle* t = new Triangle();
				t->deserialize(stream);
				trias.push_back(t);
			}
		return stream;
	}

	bool PhysicsShape::addShapeToBody(b2Body *body, float density)
	{
		if (trias.size() ==0) return false;
		b2PolygonShape shape;
		for(int i=0; i< trias.size(); i++)
		{
			Triangle* t = trias[i];
			int num_points = 3;
			b2Vec2 vertices[3];
			for (int j=0; j < num_points; j++)
			{
				vertices[j] = b2Vec2(t->points[j].x()/_currentscale,
									 t->points[j].y()/_currentscale);
			}
			shape.Set(vertices, num_points);
			body->CreateFixture(&shape, density);
		}
		return true;
	}

	float PhysicsShape::scale()
	{
		return _currentscale;
	}

	void PhysicsShape::setScale(float val)
	{
		if (val > 0)
			_currentscale = val;
	}

//	PhysicsShape* PhysicsShape::createShapeFromImage(Image *image)
//	{
//		QString f = "tmp_physics.png";
//		image->pixmap()->save(f);
//		vector<p2t::Triangle*> p2tTrias = generatePolygonFromImage(f, 5, 5);
//		if (p2tTrias.size()== 0) return NULL;
//		PhysicsShape* physicsShape =
//				PhysicsShape::createShapeFromTriangles(p2tTrias);

//		return physicsShape;
//	}

	PhysicsShape* PhysicsShape::createShapeFromPixmap(QPixmap &pixmap)
	{
		QString f = "tmp_physics.png";
		pixmap.save(f);

		vector<p2t::Triangle*> p2tTrias = generatePolygonFromImage(f, 5, 5);
		if (p2tTrias.size()== 0) return NULL;
		PhysicsShape* physicsShape =
				PhysicsShape::createShapeFromTriangles(p2tTrias);

		return physicsShape;
	}

	PhysicsShape* PhysicsShape::createShapeFromTriangles(std::vector<p2t::Triangle*> triangles)
	{
		PhysicsShape *physicsShape = new PhysicsShape();
		for (int i=0; i< triangles.size(); i++)
		{
			p2t::Triangle* p2tTria = triangles[i];
			Triangle* t = new Triangle(p2tTria);
			physicsShape->trias.push_back(t);
		}
		return physicsShape;
	}
}
