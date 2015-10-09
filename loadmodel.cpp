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
		//		updateComponentScale(c2, 1.25f);
		//		updateComponentScale(c1, 0.90f);
		//		updateComponentScale(c4, 1.25f);
		// Testing out some of the graph examples
		if (false) {
			customPlot = new QCustomPlot();
			customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
			QCPGraph *graph = customPlot->addGraph();
			int n = 500;
			double phase = 0;
			double k = 3;
			QVector<double> x(n), y(n);
			for (int i=0; i<n; ++i)
			{
			  x[i] = i/(double)(n-1)*34 - 17;
			  y[i] = qExp(-x[i]*x[i]/20.0)*qSin(k*x[i]+phase);
			}
			graph->setData(x, y);
			graph->setPen(QPen(Qt::blue));
			graph->rescaleKeyAxis();
			customPlot->yAxis->setRange(-1.45, 1.65);
			customPlot->xAxis->grid()->setZeroLinePen(Qt::NoPen);

			// add the bracket at the top:
			QCPItemBracket *bracket = new QCPItemBracket(customPlot);
			customPlot->addItem(bracket);
			bracket->left->setCoords(-8, 1.1);
			bracket->right->setCoords(8, 1.1);
			bracket->setLength(13);

			// add the text label at the top:
			QCPItemText *wavePacketText = new QCPItemText(customPlot);
			customPlot->addItem(wavePacketText);
			wavePacketText->position->setParentAnchor(bracket->center);
			wavePacketText->position->setCoords(0, -10); // move 10 pixels to the top from bracket center anchor
			wavePacketText->setPositionAlignment(Qt::AlignBottom|Qt::AlignHCenter);
			wavePacketText->setText("Wavepacket");
//			wavePacketText->setFont(QFont(font().family(), 10));

			// add the phase tracer (red circle) which sticks to the graph data (and gets updated in bracketDataSlot by timer event):
			QCPItemTracer *phaseTracer = new QCPItemTracer(customPlot);
			customPlot->addItem(phaseTracer);
//			itemDemoPhaseTracer = phaseTracer; // so we can access it later in the bracketDataSlot for animation
			phaseTracer->setGraph(graph);
			phaseTracer->setGraphKey((M_PI*1.5-phase)/k);
			phaseTracer->setInterpolating(true);
			phaseTracer->setStyle(QCPItemTracer::tsCircle);
			phaseTracer->setPen(QPen(Qt::red));
			phaseTracer->setBrush(Qt::red);
			phaseTracer->setSize(7);

			// add label for phase tracer:
			QCPItemText *phaseTracerText = new QCPItemText(customPlot);
			customPlot->addItem(phaseTracerText);
			phaseTracerText->position->setType(QCPItemPosition::ptAxisRectRatio);
			phaseTracerText->setPositionAlignment(Qt::AlignRight|Qt::AlignBottom);
			phaseTracerText->position->setCoords(1.0, 0.95); // lower right corner of axis rect
			phaseTracerText->setText("Points of fixed\nphase define\nphase velocity vp");
			phaseTracerText->setTextAlignment(Qt::AlignLeft);
			//phaseTracerText->setFont(QFont(font().family(), 9));
			phaseTracerText->setPadding(QMargins(8, 0, 0, 0));

			// add arrow pointing at phase tracer, coming from label:
			QCPItemCurve *phaseTracerArrow = new QCPItemCurve(customPlot);
			customPlot->addItem(phaseTracerArrow);
			phaseTracerArrow->start->setParentAnchor(phaseTracerText->left);
			phaseTracerArrow->startDir->setParentAnchor(phaseTracerArrow->start);
			phaseTracerArrow->startDir->setCoords(-40, 0); // direction 30 pixels to the left of parent anchor (tracerArrow->start)
			phaseTracerArrow->end->setParentAnchor(phaseTracer->position);
			phaseTracerArrow->end->setCoords(10, 10);
			phaseTracerArrow->endDir->setParentAnchor(phaseTracerArrow->end);
			phaseTracerArrow->endDir->setCoords(30, 30);
			phaseTracerArrow->setHead(QCPLineEnding::esSpikeArrow);
			phaseTracerArrow->setTail(QCPLineEnding(QCPLineEnding::esBar, (phaseTracerText->bottom->pixelPoint().y()-phaseTracerText->top->pixelPoint().y())*0.85));

			// add the group velocity tracer (green circle):
			QCPItemTracer *groupTracer = new QCPItemTracer(customPlot);
			customPlot->addItem(groupTracer);
			groupTracer->setGraph(graph);
			groupTracer->setGraphKey(5.5);
			groupTracer->setInterpolating(true);
			groupTracer->setStyle(QCPItemTracer::tsCircle);
			groupTracer->setPen(QPen(Qt::green));
			groupTracer->setBrush(Qt::green);
			groupTracer->setSize(7);

			// add label for group tracer:
			QCPItemText *groupTracerText = new QCPItemText(customPlot);
			customPlot->addItem(groupTracerText);
			groupTracerText->position->setType(QCPItemPosition::ptAxisRectRatio);
			groupTracerText->setPositionAlignment(Qt::AlignRight|Qt::AlignTop);
			groupTracerText->position->setCoords(1.0, 0.20); // lower right corner of axis rect
			groupTracerText->setText("Fixed positions in\nwave packet define\ngroup velocity vg");
			groupTracerText->setTextAlignment(Qt::AlignLeft);
			//groupTracerText->setFont(QFont(font().family(), 9));
			groupTracerText->setPadding(QMargins(8, 0, 0, 0));

			// add arrow pointing at group tracer, coming from label:
			QCPItemCurve *groupTracerArrow = new QCPItemCurve(customPlot);
			customPlot->addItem(groupTracerArrow);
			groupTracerArrow->start->setParentAnchor(groupTracerText->left);
			groupTracerArrow->startDir->setParentAnchor(groupTracerArrow->start);
			groupTracerArrow->startDir->setCoords(-40, 0); // direction 30 pixels to the left of parent anchor (tracerArrow->start)
			groupTracerArrow->end->setCoords(5.5, 0.4);
			groupTracerArrow->endDir->setParentAnchor(groupTracerArrow->end);
			groupTracerArrow->endDir->setCoords(0, -40);
			groupTracerArrow->setHead(QCPLineEnding::esSpikeArrow);
			groupTracerArrow->setTail(QCPLineEnding(QCPLineEnding::esBar, (groupTracerText->bottom->pixelPoint().y()-groupTracerText->top->pixelPoint().y())*0.85));

			// add dispersion arrow:
			QCPItemCurve *arrow = new QCPItemCurve(customPlot);
			customPlot->addItem(arrow);
			arrow->start->setCoords(1, -1.1);
			arrow->startDir->setCoords(-1, -1.3);
			arrow->endDir->setCoords(-5, -0.3);
			arrow->end->setCoords(-10, -0.2);
			arrow->setHead(QCPLineEnding::esSpikeArrow);

			// add the dispersion arrow label:
			QCPItemText *dispersionText = new QCPItemText(customPlot);
			customPlot->addItem(dispersionText);
			dispersionText->position->setCoords(-6, -0.9);
			dispersionText->setRotation(40);
			dispersionText->setText("Dispersion with\nvp < vg");
			//dispersionText->setFont(QFont(font().family(), 10));

			// setup a timer that repeatedly calls MainWindow::bracketDataSlot:
			//connect(&dataTimer, SIGNAL(timeout()), this, SLOT(bracketDataSlot()));
			//dataTimer.start(0); // Interval 0 means to refresh as fast as possible

			customPlot->show();
		}
		if (false) {
			customPlot = new QCustomPlot();
			customPlot->legend->setVisible(true);
			customPlot->legend->setFont(QFont("Helvetica", 9));
			customPlot->legend->setRowSpacing(-3);
			QVector<QCPScatterStyle::ScatterShape> shapes;
			shapes << QCPScatterStyle::ssCross;
			shapes << QCPScatterStyle::ssPlus;
			shapes << QCPScatterStyle::ssCircle;
			shapes << QCPScatterStyle::ssDisc;
			shapes << QCPScatterStyle::ssSquare;
			shapes << QCPScatterStyle::ssDiamond;
			shapes << QCPScatterStyle::ssStar;
			shapes << QCPScatterStyle::ssTriangle;
			shapes << QCPScatterStyle::ssTriangleInverted;
			shapes << QCPScatterStyle::ssCrossSquare;
			shapes << QCPScatterStyle::ssPlusSquare;
			shapes << QCPScatterStyle::ssCrossCircle;
			shapes << QCPScatterStyle::ssPlusCircle;
			shapes << QCPScatterStyle::ssPeace;
			shapes << QCPScatterStyle::ssCustom;

			QPen pen;
			// add graphs with different scatter styles:
			for (int i=0; i<shapes.size(); ++i)
			{
				customPlot->addGraph();
				pen.setColor(QColor(qSin(i*0.3)*100+100, qSin(i*0.6+0.7)*100+100, qSin(i*0.4+0.6)*100+100));
				// generate data:
				QVector<double> x(10), y(10);
				for (int k=0; k<10; ++k)
				{
					x[k] = k/10.0 * 4*3.14 + 0.01;
					y[k] = 7*qSin(x[k])/x[k] + (shapes.size()-i)*5;
				}
				customPlot->graph()->setData(x, y);
				customPlot->graph()->rescaleAxes(true);
				customPlot->graph()->setPen(pen);
				customPlot->graph()->setName(QCPScatterStyle::staticMetaObject.enumerator(QCPScatterStyle::staticMetaObject.indexOfEnumerator("ScatterShape")).valueToKey(shapes.at(i)));
				customPlot->graph()->setLineStyle(QCPGraph::lsLine);
				// set scatter style:
				if (shapes.at(i) != QCPScatterStyle::ssCustom)
				{
					customPlot->graph()->setScatterStyle(QCPScatterStyle(shapes.at(i), 10));
				}
				else
				{
					QPainterPath customScatterPath;
					for (int i=0; i<3; ++i)
						customScatterPath.cubicTo(qCos(2*M_PI*i/3.0)*9, qSin(2*M_PI*i/3.0)*9, qCos(2*M_PI*(i+0.9)/3.0)*9, qSin(2*M_PI*(i+0.9)/3.0)*9, 0, 0);
					customPlot->graph()->setScatterStyle(QCPScatterStyle(customScatterPath, QPen(Qt::black, 0), QColor(40, 70, 255, 50), 10));
				}
			}
			// set blank axis lines:
			customPlot->rescaleAxes();
			customPlot->xAxis->setTicks(false);
			customPlot->yAxis->setTicks(false);
			customPlot->xAxis->setTickLabels(false);
			customPlot->yAxis->setTickLabels(false);
			// make top right axes clones of bottom left axes:
			customPlot->axisRect()->setupFullAxesBox();
			customPlot->show();
		}
		if (false) {
			// Do some plotting stuff
			customPlot = new QCustomPlot();
			customPlot->addGraph();
			// Add initial values
			vecXaxis.push_back(0);
			//vecXaxis.push_back(c2->physicsBody()->GetAngularVelocity());
			vecYaxis.push_back(c3->physicsBody()->GetAngularVelocity());
			customPlot->graph(0)->setData(vecXaxis, vecYaxis);
			customPlot->xAxis->setLabel("x-axis");
			customPlot->yAxis->setLabel("y-axis");
			customPlot->rescaleAxes();
			customPlot->replot();

			auto addPlotPoint = [=]()
			{
				//qDebug() << c1->rotation() << c2->rotation();
				//qDebug() << vecXaxis << vecYaxis;
				//vecXaxis.push_back(c2->physicsBody()->GetAngularVelocity());
				vecXaxis.push_back(vecXaxis.last()+1);
				//vecYaxis.push_back(fmod(c2->rotation(), 360));
				// Lets see what all we can plot
				vecYaxis.push_back(c3->physicsBody()->GetAngularVelocity());

				// Clip the vectors beyond a given length
				customPlot->graph(0)->setData(vecXaxis, vecYaxis);
				customPlot->rescaleAxes();
				customPlot->replot();
			};

			connect(page->getPhysicsManager(), &PhysicsManager::physicsStepComplete,
					addPlotPoint);
			customPlot->show();
		}

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
