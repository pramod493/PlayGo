#include "sketchscene.h"
#include <QDebug>

namespace CDI
{
SketchScene::SketchScene(QObject* parent) :
    QGraphicsScene(parent)
{
    qDebug() << "Creating sketch scene widget\n";

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

    defaultBrush = QBrush(QColor(1.0,1.0,1.0), Qt::NoBrush);
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

void SketchScene::BrushPress(QPointF scenePos)
{
    qDebug() << "Brush down at " << scenePos;
    if (0) {
        QGraphicsLineItem* line = new QGraphicsLineItem();

        line->setPen(marqueeSelectPen);
        line->setLine(QLineF(scenePos, QPointF(500,300)));
        addItem(line);
        //QGraphicsLineItem* line = addLine(QLineF(scenePos, QPointF(500,300)), defaultPen);
    }

    current_stroke = new GraphicsPathItem(parent_item, scenePos);
    current_stroke->parentStroke->thickness = defaultPen.width();
    addItem(current_stroke);
    current_stroke->setPen(defaultPen);
    freeStrokes.push_back(current_stroke);
}

void SketchScene::BrushMove(QPointF scenePos)
{
    qDebug() << "Brush Move to " << scenePos;
    if (!(current_stroke == NULL))
        current_stroke->push_back(scenePos);
}

void SketchScene::BrushRelease(QPointF scenePos)
{
    if (!(current_stroke == NULL))
        current_stroke->push_back(scenePos);
    qDebug() << "Apply smoothing to the stroke";
    current_stroke->ApplySmoothing(1);

    // Trigger signal in order to update related/connected components
    current_stroke = NULL;

    emit signalBrushReleased(this);
}

void SketchScene::SaveScene(QString file)
{
    Q_UNUSED(file);
}

SketchScene* SketchScene::Clone()
{
    return this;
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
        BrushPress(scenePos);
        break;
    case QEvent::TabletMove :
        BrushMove(scenePos);
        break;
    case QEvent::TabletRelease :
        BrushRelease(scenePos);
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
    // Saving for surface
    if (freeStrokes.size())
    {
        qDebug() << "Free strokes = " << freeStrokes.size();

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
            painter.setBrush(item->brush());
            painter.setPen(item->pen());
            painter.drawPath(item->path());
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
            qDebug() << "Seach complete. Loading images now";
            OnSearchComplete();
        }
    }
}

void SketchScene::slotTabletEvent(QTabletEvent* event, QPointF scenePos)
{
    // Temporarily disable mouse event because we don't know how to prevent trigger of mouse events
    if (event->pointerType() == QTabletEvent::Pen && current_mode == MODE::Draw)
        DrawAction(event, scenePos);
    if (event->pointerType() == QTabletEvent::Eraser)
        EraseAction(event, scenePos);
}

void SketchScene::OnSearchComplete()
{
    // Read file from SearchManager
    for (int i=0; i< searchResults.size(); i++)
    {
        SearchGraphicsItem* item = searchResults[i];
        removeItem(item);
        delete (item);
    }
    searchResults.clear();
    int searchItemSize = 100;
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
    qDebug() << "Brush width: " << size;
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
