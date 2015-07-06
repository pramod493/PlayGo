#include "modelviewtreewidget.h"
#include "commonfunctions.h"
#include <QDebug>

namespace CDI
{
	ModelViewTreeWidget::ModelViewTreeWidget(PlayGo *root)
	{
		playgo = root;
		setColumnCount(4);
		rootItem = NULL;
		populate();

		QTimer *timer = new QTimer(this);
		connect(timer, SIGNAL(timeout()),
				this, SLOT(populate()));
		timer->start(5000);
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
			tmpItem->setText(3, getItemNameByType(page->type()));

			QList<Assembly*> assemblies = page->getAssemblies();
			foreach (Assembly* assembly, assemblies)
			{
				updateAssembly(tmpItem, assembly);
			}

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

//		component->setTransform(component->transform().translate(20,20));
//		component->setTransform(component->transform().rotate(5));
		QTreeWidgetItem* compItem = new QTreeWidgetItem(parent);
		compItem->setText(0, "Component");
		compItem->setText(1, component->id().toString());
		updateTransform(2, compItem, component);

		QList<QGraphicsItem*> modelItems = component->values();
		foreach (QGraphicsItem* modelItem, modelItems)
		{
			updateAbstractModelItem(compItem, modelItem);
		}
	}

	void ModelViewTreeWidget::updateAbstractModelItem(QTreeWidgetItem* parent, QGraphicsItem *modelItem)
	{
		QTreeWidgetItem* item = new QTreeWidgetItem(parent);
		updateTransform(2,item, modelItem);
		if (modelItem->type() == Stroke::Type)
		{
			Stroke* s = qgraphicsitem_cast<Stroke*>(modelItem);
			QTreeWidgetItem* vectorChild = new QTreeWidgetItem(item);
			vectorChild->setText(0, "Stroke");
			vectorChild->setText(1, QString::number(s->size()) + " Points");
		}
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
