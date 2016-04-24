#include "pixmap.h"
#include <QsLog.h>
namespace CDI
{
	Pixmap::Pixmap(QGraphicsItem *parent)
		:QGraphicsPixmapItem(parent)
	{
		_id = uniqueHash();
		_highlighted = false;
		_physicsShape = nullptr;

		//setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
		setShapeMode(QGraphicsPixmapItem::MaskShape);

		setZValue(Z_IMAGEVIEW);

#ifdef CDI_DEBUG_DRAW_SHAPE
		QGraphicsEllipseItem *ellipse = new QGraphicsEllipseItem(this);
		ellipse->setPen(QPen(Qt::blue));
		ellipse->setBrush(QBrush(Qt::green, Qt::SolidPattern));
		ellipse->setRect(QRectF(-10,-10,20,20));
		ellipse->setTransform(QTransform());
		ellipse->setZValue(1.0f);
#endif //CDI_DEBUG_DRAW_SHAPE
	}

	Pixmap::Pixmap(QString filepath, QGraphicsItem* parent)
		: Pixmap(parent)
	{
		_filename = filepath;
		QFile f(_filename);
		if (f.exists())
		{
			QPixmap p = QPixmap();
			p.load(_filename);
			setPixmap(p);
		}
	}

	Pixmap::Pixmap(const QPixmap &pixmap, QString filepath, QGraphicsItem* parent)
		: Pixmap(parent)
	{
		setPixmap(pixmap);
		_filename = filepath;
	}

	Pixmap::Pixmap(const Pixmap& copy)
		: Pixmap(copy.parentItem())
	{
		setPixmap(copy.pixmap());
		_physicsShape = new PhysicsShape(*copy._physicsShape);
		_filename = copy.filename();
		setShapeMode(copy.shapeMode());
		setZValue(copy.zValue());
		setTransform(copy.transform());
	}

	void Pixmap::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
	{
		QGraphicsPixmapItem::paint(painter, option, widget);
#ifdef CDI_DEBUG_DRAW_SHAPE
		painter->setPen(QPen(Qt::red));
		painter->drawRect(boundingRect());
#endif
	}

	PhysicsShape* Pixmap::physicsShape() const
	{
		return _physicsShape;
	}

	void Pixmap::resetPhysicsShape()
	{
		initializePhysicsShape();
	}

	QString Pixmap::filename() const
	{
		return _filename;
	}

	void Pixmap::setFilename(QString filepath)
	{
		_filename = filepath;
	}

	bool Pixmap::isHighlighted() const
	{
		return _highlighted;
	}

	void Pixmap::highlight(bool value)
	{
		if (_highlighted == value) return;  // Do nothing
		_highlighted = value;
	}

	QDataStream& Pixmap::serialize(QDataStream& stream) const
	{
		stream << _id;
		return stream;
	}

	QDataStream& Pixmap::deserialize(QDataStream& stream)
	{
		stream >> _id;
		return stream;
	}

	void Pixmap::initializePhysicsShape()
	{
		// Recreate physics shapes
		if(_physicsShape) delete _physicsShape;
		_physicsShape = NULL;

		QPixmap tmpPixmap =	pixmap();
		if (tmpPixmap.isNull()) return;
		QString filepath = getHomeDirectory() + "/tmp.png";
		tmpPixmap.save(filepath);
		vector<p2t::Triangle*> p2tTrias = generatePolygonFromImage(filepath);
		_physicsShape  = new PhysicsShape(p2tTrias);

		for (int i=0; i < p2tTrias.size(); i++)
		{
			p2t::Triangle* tria = p2tTrias[i];
			if (tria) delete tria;
			p2tTrias[i] = 0;
		}
		p2tTrias.clear();
	}
}
