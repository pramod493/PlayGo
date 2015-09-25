#include "commonfunctions.h"
#include "LoadModel.h"
#include "stroke.h"
#include "QsLog.h"

namespace CDI
{
	LoadModel::LoadModel(PlayGoController *playgocontroller)
		: QObject (playgocontroller)
	{
		//
		controller = playgocontroller;
		page = controller->_page;
	}

	void LoadModel::createComponents()
	{
		auto updateComponentScale = [](Component* c, float newScale)
		{
			c->previousScale = c->scale();
			c->setScale(newScale);
			c->requiresRegeneration = true;
			c->pendingPositionUpdate = true;
			emit c->onTransformChange(c);
		};
		// Do not try setting density to zero... doesn't turn out to be good
		Component *c1 = page->createComponent();
		c1->setPos(100,200);
		c1->setStatic(true);
		c1->setLayerIndex(LAYER_01);
		{
			QVector<QPointF> points;
			points.reserve(4);
			points.push_back(QPointF(0,0));
			points.push_back(QPointF(0,20));
			points.push_back(QPointF(200,20));
			points.push_back(QPointF(200,0));
			createPolygon(c1, QPointF(0,0), points);
		}
		Component *c2 = page->createComponent();
		c2->setPos(100,200);
		c2->setLayerIndex(LAYER_02);
		{
			QVector<QPointF> points;
			points.reserve(4);
			points.push_back(QPointF(0,0));
			points.push_back(QPointF(20,20));
			points.push_back(QPointF(120,-80));
			points.push_back(QPointF(100,-100));
			createPolygon(c2, QPointF(0,0), points);
		}
		Component *c3 = page->createComponent();
		c3->setPos(200,100);
		c3->setLayerIndex(LAYER_03);
		{	// same as component c1
			QVector<QPointF> points;
			points.reserve(4);
			points.push_back(QPointF(0,0));
			points.push_back(QPointF(0,20));
			points.push_back(QPointF(200,20));
			points.push_back(QPointF(200,0));
			createPolygon(c3, QPointF(0,0), points);
		}
		Component *c4 = page->createComponent();
		c4->setPos(280,200);
		c4->setLayerIndex(LAYER_02);
		{
			QVector<QPointF> points;
			points.reserve(4);
			points.push_back(QPointF(0,0));
			points.push_back(QPointF(20,20));
			points.push_back(QPointF(120,-80));
			points.push_back(QPointF(100,-100));
			createPolygon(c4, QPointF(0,0), points);
		}

		// create joints
		{
		page->getPhysicsManager()->createPinJoint
				(c1, c2, QPointF(100,200),
				 true, false, 100, 50, 0, 720);

		page->getPhysicsManager()->createPinJoint
				(c2, c3, QPointF(200,100),
				 false, false, 100, 50, 0, 720);
		page->getPhysicsManager()->createPinJoint
				(c3, c4, QPointF(400,100),
				 false, false, 100, 50, 0, 720);
		page->getPhysicsManager()->createPinJoint
				(c4, c1, QPointF(300,200),
				 false, false, 100, 50, 0, 720);
		}
		updateComponentScale(c1, 0.90f);
		updateComponentScale(c4, 1.25f);

		return;	// stop at 4-bar
		// 1
		QVector<Component*> componentList;
		int num_componnets = 10;

		// create 4 components
		QPointF pos(100,100);
		QPointF offset(50,50);
		for (int i=0; i < num_componnets; i++)
		{
			Component* component = page->createComponent();
			componentList.push_back(component);
			component->setPos(pos.x(), pos.y());
			component->setDensity(abs(sin(i))*10 + 0.1f);
			qDebug() << "Density" << component->getDensity();
			pos = pos + offset;
		}
		for (auto component : componentList)
		{
			int squareDim = 100;
			QVector<QPointF> points;
			points.reserve(4);
			points.push_back(QPointF(0,0));
			points.push_back(QPointF(squareDim,0));
			points.push_back(QPointF(squareDim,squareDim));
			points.push_back(QPointF(0,squareDim));
			createPolygon(component, QPointF(0,0), points);
			// try with polygons
			//QGraphicsPolygonItem* polygon = new QGraphicsPolygonItem(QPolygonF(points), component);

		}

//		// Fix the first one
		componentList[0]->setStatic(true);
		{
			// create a pin joint w/ motor
		page->getPhysicsManager()->createPinJoint(componentList[0], componentList[1],
												componentList[1]->mapToScene(QPointF(0,0)),
												  true, true, 100, 50, 0, 720);
		}
		{
			// Create a slider joint
			auto sliderStartPos = componentList[2]->mapToScene(QPointF(0,0));
			auto slider = page->getPhysicsManager()->createPrismaticJoint
					(componentList[1], componentList[2], sliderStartPos, sliderStartPos+QPointF(100,-150),
					 true, true, 10, 50);
			slider->joint()->EnableMotor(true);

			componentList[1]->physicsBody()->SetActive(true);
			componentList[1]->physicsBody()->SetAwake(true);

			componentList[2]->physicsBody()->SetActive(true);
			componentList[2]->physicsBody()->SetAwake(true);
		}
	}

	Polygon2D *LoadModel::createPolygon(Component* parent, QPointF pos, QVector<QPointF> points)
	{
		auto polygon = new Polygon2D(parent);
		//auto polygon = new QGraphicsPolygonItem(parent);
		parent->addToComponent(polygon);
		polygon->setPos(pos.x(), pos.y());

		polygon->setPolygon(QPolygonF(points));

		QPen pen = QPen(Qt::blue);
		pen.setWidth(3);
		polygon->setPen(pen);

		QColor brushColor = QColor(200,250,200, 150);
		QBrush brush = QBrush(brushColor);
		polygon->setBrush(brush);

		parent->recalculateBoundingRect();
		return polygon;
	}

	Pixmap* LoadModel::loadImage(QPointF pos, QString imagePath)
	{
		return nullptr;
	}

	Pixmap* LoadModel::loadImage(QPointF pos, QPixmap image)
	{
		return nullptr;
	}

	void LoadModel::runTest()
	{
		createComponents();
	}
}
