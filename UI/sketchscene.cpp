#include "sketchscene.h"
#include <QDebug>
#include <QtSvg/QSvgGenerator>

//// Box2D header(s)
#include <Box2D/Box2D.h>
#include <Box2D/Dynamics/b2Body.h>
#include <QTimer>

namespace CDI
{
SketchScene::SketchScene(QObject* parent) :
    QGraphicsScene(parent)
{
    mouse_mode_enabled = false;	// Keep only for the testing stages.

    brushWidth = 3.0;

    defaultPen = QPen(QColor(0,0,0));
    defaultPen.setStyle(Qt::SolidLine);
    defaultPen.setWidth(brushWidth);

    highlightPen = QPen(QColor(0.5,0,1.0));
    highlightPen.setStyle(Qt::SolidLine);
    highlightPen.setWidth(brushWidth+3);

    marqueeSelectPen = QPen(QColor(0.25,0.25,0.25));
    marqueeSelectPen.setStyle(Qt::DashLine);
    marqueeSelectPen.setWidth(2);

    defaultBrush = QBrush(QColor(.75,0.75,0.75),Qt::BDiagPattern/* Qt::NoBrush*/);
    fillBrush = QBrush(QColor(0.75,0.75,0.75), Qt::SolidPattern);

    searchResults = QList<SearchGraphicsItem*>();
    freeStrokes = QList<GraphicsPathItem*>();

    current_mode = Draw;

    parent_item = NULL;

    drawing_right_now = false;

    current_stroke = NULL;

    setBackgroundBrush(fillBrush);

    searchManager = new SearchManager(this);

    // On stroke end
    QObject::connect(this, SIGNAL(signalBrushReleased(SketchScene*)),
                     this, SLOT(OnBrushRelease()));

    /* -----------------------------------------------------------------
     * Initializing the physics engine as well as testing out the simulation aspect
     * See http://www.box2d.org/manual.html for tutorial for more
    */
    if (true){
        b2Vec2 gravity(0.0, -2.0);
//        bool doSleep = true;
        physicsWorld = new b2World(gravity);

        // Create ground
        b2BodyDef groundBodyDef;    // Create as object so that it automatically gets deleted at the end
        groundBodyDef.position.Set(0.0,-10.0);

        b2Body* groundBody = physicsWorld->CreateBody(&groundBodyDef);
        b2PolygonShape groundBox;
        groundBox.SetAsBox(50.0,10.0);
        groundBody->CreateFixture(&groundBox, 0.0);

        QTimer* timer = new QTimer();
        connect(timer, SIGNAL(timeout()),
                this, SLOT(PhysicsStep()));
        timer->start(10);
    }
}

SketchScene::~SketchScene()
{
    clear();
}

void SketchScene::drawBackground(QPainter* painter, const QRectF &rect)
{
    // No freaky stuff here right now
    QGraphicsScene::drawBackground(painter, rect);
}

void SketchScene::clear()
{
    // Delete all items and clear the scene
    size_t n = searchResults.size();
    for (int i = 0; i < n; i++)
    {
        SearchGraphicsItem* item = searchResults[i];
        delete item;
    }
    searchResults.clear();

    n = freeStrokes.size();
    for (int i=0; i<n; i++)
    {
        GraphicsPathItem* item = freeStrokes[i];
        delete item;
    }
    freeStrokes.clear();

}

void SketchScene::BrushPress(QPointF scenePos, float pressure)
{
    current_stroke = new GraphicsPathItem(parent_item, scenePos, pressure, 0);
	current_stroke->parentStroke->setThickness(defaultPen.width());
	current_stroke->parentStroke->setColor(defaultPen.color());
    addItem(current_stroke);
    current_stroke->setPen(defaultPen);
    current_stroke->setBrush(defaultBrush);
    freeStrokes.push_back(current_stroke);

}

void SketchScene::BrushMove(QPointF scenePos, float pressure)
{
    if (!(current_stroke == NULL))
        current_stroke->push_back(scenePos, pressure);
	update();
}

void SketchScene::BrushRelease(QPointF scenePos, float pressure)
{
    if (!(current_stroke == NULL))
        current_stroke->push_back(scenePos, pressure);
	current_stroke->ApplySmoothing(2);
	update();
	// Physics engine test code
//	{
//		b2BodyDef bodyDef;
//		bodyDef.type = b2_dynamicBody;
//		bodyDef.position.Set(0.0,0.0);
//		bodyDef.angle = 25;
//		current_stroke->physicsBody = physicsWorld->CreateBody(&bodyDef);
//		b2ChainShape polygon;
//		b2Vec2* vec = new b2Vec2[current_stroke->parentStroke->points.size()];
//		int vertexCount =0;
//		for (int i=0; i < current_stroke->parentStroke->points.size();)
//		{
//			Point2D* pt = current_stroke->parentStroke->points[i];
//			vec[i] = b2Vec2(pt->x*current_stroke->physicsDivider,pt->y*current_stroke->physicsDivider);
//			vertexCount++;
//			i++;
//		}
//		polygon.CreateChain(vec, vertexCount);
//		b2FixtureDef fixtureDef;
//		fixtureDef.shape = &polygon;
//		fixtureDef.density = 0.4f;
//		fixtureDef.friction = 0.25f;
//		fixtureDef.restitution = 0.75f;
//		current_stroke->physicsBody->CreateFixture(&fixtureDef);

//	}
    // Trigger signal in order to update related/connected components
    current_stroke = NULL;

    emit signalBrushReleased(this);

}

void SketchScene::SelectSearchResult(SearchGraphicsItem *searchItem)
{
	Q_UNUSED(searchItem)
	// IMPORTANT - Implement loading of search results
	// TODO - Get the results file. Load the image and replace it
    // with all strokes with search results in a way that it fits
    // neatly in the same area
//    QString imageSourceFile = searchItem->sourceFilePath;
//    QPixmap pix = QPixmap(imageSourceFile); // Skip sanity checks since its coming from search image

//    GraphicsItemGroup *itemGroup = new GraphicsItemGroup(NULL, NULL);
//    addItem(itemGroup);
//    // NOTES - In here we want to allow create all items as ItemGroup of give type.. would that be better?
//    // Is it better to keep parent object = NULL if we are planning to reparent object during the workflow?
//    PixmapItem* sceneItem = new PixmapItem(pix, imageSourceFile, NULL, NULL);
//    itemGroup->addToGroup(sceneItem);
//    // Add all the strokes but set the invisible
//    for (QList<GraphicsPathItem*>::iterator it = freeStrokes.begin();
//         it != freeStrokes.end();
//         ++it)
//    {
//        itemGroup->addToGroup(*it);
//        (*it)->setVisible(false);
//    }
//    freeStrokes.clear();
}

void SketchScene::SaveScene(QString file)
{
    Q_UNUSED(file);
}

SketchScene* SketchScene::Clone()
{
    return this;
}

void SketchScene::PhysicsStep()
{
//// Disable the whole physics engine runtime for now
//    // Set up simulation settings
//    float timeStep = 1.0/60.0;
//    int velocityIterations = 6;
//    int positionIterations = 4;

//    physicsWorld->Step(timeStep, velocityIterations, positionIterations);
//    GraphicsPathItem* item = NULL;

//    foreach(item, freeStrokes)
//    {
//        if (item->physicsBody!= NULL)
//        {
//            b2Body* body = item->physicsBody;
//            b2Vec2 position = body->GetPosition();
//            float angle = body->GetAngle();
//            item->setPos(position.x*item->physicsMultiplier,position.y*item->physicsMultiplier);
//            item->setRotation(angle);
//            update(item->boundingRect());
////            qDebug() << angle << position.x << position.y;
//        }
//    }
//    if (item != NULL)
//    if (item->physicsBody!= NULL)
//    {
//        qDebug() << item->physicsBody->GetAngle();
//    }
}

// Protected function
void SketchScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    mouseEvent->accept();
    if (!mouse_mode_enabled) return;

    switch (current_mode)
    {
    case MODE::Draw :
        BrushPress(mouseEvent->scenePos());
        break;
    case MODE::Erase :

        break;
    case MODE::Transform :
        break;
    case MODE::Edit :
        break;
    }
}

void SketchScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    mouseEvent->accept();
    if (!mouse_mode_enabled) return;

    switch (current_mode)
    {
    case MODE::Draw :
        BrushMove(mouseEvent->scenePos());
        break;
    case MODE::Erase :
        break;
    case MODE::Transform :
        break;
    case MODE::Edit :
        break;
    }
}

void SketchScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    mouseEvent->accept();
    if (!mouse_mode_enabled) return;

    switch (current_mode)
    {
    case MODE::Draw :
        BrushRelease(mouseEvent->scenePos());
        break;
    case MODE::Erase :
        break;
    case MODE::Transform :
        break;
    case MODE::Edit :
        break;
    }
}


void SketchScene::DrawAction(QTabletEvent *event, QPointF scenePos)
{
    switch (event->type())
    {
    case QEvent::TabletPress :
        BrushPress(scenePos,event->pressure());
        break;
    case QEvent::TabletMove :
        BrushMove(scenePos, event->pressure());
        break;
    case QEvent::TabletRelease :
        BrushRelease(scenePos, event->pressure());
        break;
    }
}

void SketchScene::EraseAction(QTabletEvent *event, QPointF scenePos)
{
    switch (event->type())
    {
    case QEvent::TabletPress :
    case QEvent::TabletMove :
    case QEvent::TabletRelease :
        QList<int> ids = QList<int>();
        // iterate through each stroke
        for (int i=0;i<freeStrokes.size();i++)
        {
            if (freeStrokes[i] != NULL)
                if (freeStrokes[i]->Selected(scenePos, defaultPen.width()))
                {
                    removeItem(freeStrokes[i]);
                    // Remove from the free strokes list
                    ids.push_back(i);
					update();
                }
        }
        // Remove item from the freeStrokes list
        for (int i=0;i<ids.size();i++)
        {
            freeStrokes.removeAt(ids[i]);
        }

        break;
    }
}

void SketchScene::SelectAction(QTabletEvent *event, QPointF scenePos)
{
    Q_UNUSED(scenePos);
    switch (event->type())
    {
    case QEvent::TabletPress :
        break;
    case QEvent::TabletMove :
        break;
    case QEvent::TabletRelease :
        break;
    }
}

void SketchScene::OnBrushRelease()
{
    return;

    // Disable search at the end of each stroke
    OnSearchTrigger();
}

void SketchScene::slotTabletEvent(QTabletEvent* event, QPointF scenePos)
{
    // Temporarily disable mouse event because we don't know how to prevent trigger of mouse events
    if (event->pointerType() == QTabletEvent::Pen && current_mode == MODE::Draw)
        DrawAction(event, scenePos);
    if (event->pointerType() == QTabletEvent::Eraser)
        EraseAction(event, scenePos);
}

void SketchScene::OnSearchTrigger()
{
    // Saving for surface
    if (freeStrokes.size())
    {
        int x_min = 0;  int y_min = 0;
        int x_max = 0; int y_max = 0;

        clearSelection();
        setSceneRect(itemsBoundingRect());
        QRectF rect = sceneRect();
        x_max = rect.x() + rect.width();
        y_max = rect.y() + rect.height();
        //            QImage image(item->boundingRect().size().toSize(), QImage::Format_ARGB32);
        QImage image(x_max,y_max, QImage::Format_ARGB32);
        image.fill(Qt::transparent);

        GraphicsPathItem* item = freeStrokes[freeStrokes.size()-1];

        QPainter painter(&image);
        painter.setRenderHint(QPainter::Antialiasing);
        x_min = x_max; y_min = y_max; x_max = 0; y_max = 0;
        for (int i=0; i<freeStrokes.size(); i++)
        {
            item = freeStrokes[i];
            /*painter.setBrush(item->brush());
            painter.setPen(item->pen());
            painter.drawPath(item->path())*/;
            item->paint(&painter, NULL);
            QRectF rectF = item->boundingRect();
            QRect rect = QRect(rectF.x(),rectF.y(),rectF.width(),rectF.height());
            x_min = (x_min < rect.x() ? x_min : rect.x());
            y_min = (y_min < rect.y() ? y_min : rect.y());

            x_max = (rect.x()+rect.width() > x_max) ? rect.x()+rect.width() : x_max;
            y_max = (rect.y()+rect.height() > y_max) ? rect.y()+rect.height() : y_max;
        }
        QImage croppedImage = image.copy(QRect(x_min,y_min,
                                               x_max-x_min, y_max-y_min));
        if (searchManager->search(croppedImage, 10))
        {
            OnSearchComplete();
        }
    }
}

void SketchScene::OnSearchComplete()
{

    /*{
        // In here we will try to save as svg
        // Works but need to fine tune the settings
        QSvgGenerator svgGen;

        svgGen.setFileName( "scene2svg.svg" );
        svgGen.setSize(QSize(200, 200));
        svgGen.setViewBox(QRect(0, 0, 200, 200));
        svgGen.setTitle(tr("SVG Generator Example Drawing"));
        svgGen.setDescription(tr("An SVG drawing created by the SVG Generator "
                                    "Example provided with Qt."));

        QPainter painter( &svgGen );
        render( &painter );

    }*/
    // Read file from SearchManager
    for (int i=0; i< searchResults.size(); i++)
    {
        SearchGraphicsItem* item = searchResults[i];
        removeItem(item);
        delete (item);
    }
    searchResults.clear();
    int searchItemSize = 150;
    int margin = 10;
    int offset = 0;
    int maxIndex = searchManager->localFileList.size();

    // Iterators work fine as well. Just not needed.
    for (int index=0;index < maxIndex; index++)
    {
        QString imagePath = searchManager->localFileList[index];
        qDebug() << "Loading search result image at " << imagePath;
        QPixmap pix(imagePath);
        if (pix.height() > searchItemSize)
            pix = pix.scaledToHeight(searchItemSize, Qt::SmoothTransformation);
        if (pix.width() > searchItemSize)
            pix = pix.scaledToWidth(searchItemSize, Qt::SmoothTransformation);
        SearchGraphicsItem* searchItem = new SearchGraphicsItem(pix,imagePath, NULL);
        addItem(searchItem);
        searchItem->setOffset(offset, 0);
        offset += searchItemSize + margin;
        searchResults.append(searchItem);
    }

}

void SketchScene::setBrushWidth(int size)
{
    defaultPen.setWidth(size);
    highlightPen.setWidth(size+3);
}

void SketchScene::setBrushColor(QString name, QColor color)
{
    defaultPen.setColor(color);
}

void SketchScene::slotSetSceneMode(MODE newMode)
{
    if (current_mode == newMode) return;
    current_mode = newMode;
    emit signalModeChanged(current_mode);
}

void SketchScene::setToNone()
{
    current_mode = None;
    emit signalModeChanged(current_mode);
}

void SketchScene::setToDraw()
{
    current_mode = Draw;
    emit signalModeChanged(current_mode);
}

void SketchScene::setToErase()
{
    current_mode = Erase;
    emit signalModeChanged(current_mode);
}

void SketchScene::setToTransform()
{
    current_mode = Transform;
    emit signalModeChanged(current_mode);
}

void SketchScene::setToEdit()
{
    current_mode = Edit;
    emit signalModeChanged(current_mode);
}

void SketchScene::setToSelect()
{
    current_mode = Select;
    emit signalModeChanged(current_mode);
}

void SketchScene::setToSearch()
{
    current_mode = Search;
}
}
