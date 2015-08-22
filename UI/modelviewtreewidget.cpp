#include "modelviewtreewidget.h"
#include "commonfunctions.h"
#include <QDebug>
#include <QPushButton>
#include "stroke.h"
#include "pixmap.h"
#include "polygon2d.h"
#include "physicsjoint.h"

namespace CDI
{
	// 0 - Type
	// 1 - ID
	// 2 - Transform
	// 3 - Extra details
	ModelViewTreeWidget::ModelViewTreeWidget(PlayGo *root)
	{
		playgo = root;
		setColumnCount(4);
		rootItem = NULL;
		populate();

		id_type_hash.insert(Stroke::Type, "Stroke");
		id_type_hash.insert(Polygon2D::Type, "Polygon 2D");
		id_type_hash.insert(Pixmap::Type, "Pixmap");
		id_type_hash.insert(PhysicsJoint::Type, "Pin joint");

// Manual update
//		QTimer *timer = new QTimer(this);
//		connect(timer, SIGNAL(timeout()),
//				this, SLOT(populate()));
//		timer->start(5000);

		QPushButton *button = new QPushButton(this);
		button->setText("Refresh tree");
		connect(button, SIGNAL(clicked()),
				 this, SLOT(populate()));
	}

	ModelViewTreeWidget::~ModelViewTreeWidget()
	{

	}

	void ModelViewTreeWidget::populate()
	{
		if (rootItem!= NULL) delete rootItem;
		rootItem = new QTreeWidgetItem(this);
		rootItem->setText(0, "PlayGo");

		QList<Page*> pages = playgo->values();
		foreach(Page* page, pages)
		{
			QTreeWidgetItem* tmpItem = new QTreeWidgetItem(rootItem);
			tmpItem->setText(0, "Page");
			tmpItem->setText(1, page->id().toString());

//			QList<Assembly*> assemblies = page->getAssemblies();
//			foreach (Assembly* assembly, assemblies)
//			{
//				updateAssembly(tmpItem, assembly);
//			}

			QList<Component*> components = page->getComponents();
			foreach (Component* component, components)
			{
				updateComponent(tmpItem, component);
			}
		}

		expandAll();
	}


	void ModelViewTreeWidget::updateAssembly(QTreeWidgetItem* parent, Assembly* assembly)
	{
		QTreeWidgetItem* assemItem = new QTreeWidgetItem(parent);
		assemItem->setText(0, "Assembly");
		assemItem->setText(1, assembly->id().toString());
		updateTransform(2, assemItem, assembly);
		QList<Component*> comps = assembly->components();
		foreach (Component* component, comps)
		{
			updateComponent(assemItem, component);
		}
	}

	void ModelViewTreeWidget::updateComponent(QTreeWidgetItem* parent, Component* component)
	{

		QTreeWidgetItem* compItem = new QTreeWidgetItem(parent);
		compItem->setText(0, "Component");
		compItem->setText(1, component->id().toString());
		updateTransform(2, compItem, component);

		QList<QGraphicsItem*> modelItems = component->childItems();
		foreach (QGraphicsItem* modelItem, modelItems)
		{
			updateAbstractModelItem(compItem, modelItem);
		}
	}

	void ModelViewTreeWidget::updateAbstractModelItem(QTreeWidgetItem* parent, QGraphicsItem *modelItem)
	{
		QTreeWidgetItem* item = new QTreeWidgetItem(parent);
		item->setText(0, id_type_hash.value(modelItem->type()));
		updateTransform(2,item, modelItem);
	}

	void ModelViewTreeWidget::updateTransform(int column, QTreeWidgetItem* item, QGraphicsItem *t)
	{
		QPointF pos = t->pos();
		float rot = t->rotation();
		float scale = t->scale();
		QString msg = "POS:(";
		msg = msg + QString::number(pos.x()) + ", " + QString::number(pos.y()) + ")";
		msg = msg + QString(" ROT:") + QString::number(rot);
		msg = msg + QString(" SCA:") + QString::number(scale);
		item->setText(column, msg);
	}
}
