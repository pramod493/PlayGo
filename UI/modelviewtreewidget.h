#pragma once
#include <QTreeView>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QTransform>
#include "playgocore.h"

namespace CDI
{
	class ModelViewTreeWidget : public QTreeWidget
	{
		Q_OBJECT
	private:

		PlayGo *playgo;
		QTreeWidgetItem* rootItem;

	public:
		ModelViewTreeWidget(PlayGo *root);
		~ModelViewTreeWidget();

	public slots:
		void populate();

	private:
		void updateAssembly(QTreeWidgetItem* parent, Assembly* assembly);

		void updateComponent(QTreeWidgetItem* parent, Component* component);

		void updateAbstractModelItem(QTreeWidgetItem* parent, AbstractModelItem* modelItem);

        void updateTransform(int column, QTreeWidgetItem* item, QTransform t);
	};
}
