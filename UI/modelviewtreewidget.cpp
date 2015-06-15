#include "modelviewtreewidget.h"
#include <QDebug>

namespace CDI
{
	ModelViewTreeWidget::ModelViewTreeWidget(PlayGo *root)
	{
		playgo = root;
		setColumnCount(3);
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
		qDebug() << "Updating root items";
		if (rootItem!= NULL) delete rootItem;
		rootItem = new QTreeWidgetItem(this);
		rootItem->setText(0, "PlayGo");

		QList<Page*> pages = playgo->values();
		foreach(Page* page, pages)
		{
			QTreeWidgetItem* tmpItem = new QTreeWidgetItem(rootItem);
			tmpItem->setText(0, "Page");
			tmpItem->setText(1, page->id().toString());

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

		updateTransform(2, assemItem, assembly->transform());

		QList<Component*> components = assembly->values();
		foreach (Component* component, components)
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
		updateTransform(2, compItem, component->transform());

		QList<AbstractModelItem*> modelItems = component->values();
		foreach (AbstractModelItem* modelItem, modelItems)
		{
			updateAbstractModelItem(compItem, modelItem);
		}
	}

	void ModelViewTreeWidget::updateAbstractModelItem(QTreeWidgetItem* parent, AbstractModelItem* modelItem)
	{
		QTreeWidgetItem* item = new QTreeWidgetItem(parent);
		item->setText(0, getItemNameByType(modelItem->type()));
		item->setText(1, modelItem->id().toString());
		updateTransform(2,item, modelItem->transform());

		if (modelItem->type() == ItemType::STROKE)
		{
			Stroke* s = static_cast<Stroke*>(modelItem);
			QTreeWidgetItem* vectorChild = new QTreeWidgetItem(item);
			vectorChild->setText(0, "QVector<Point2DPT>");
			vectorChild->setText(1, QString::number(s->size()) + " Points");
		}
	}

	void ModelViewTreeWidget::updateTransform(int column, QTreeWidgetItem* item, QTransform &t)
	{
		QPointF pos = t.map(QPointF(0,0));
		QString msg = "(";
		msg = msg + QString::number(pos.x()) + "," + QString::number(pos.y()) + ")";
		item->setText(column, msg);
	}
}
