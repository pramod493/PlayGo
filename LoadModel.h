#ifndef __LOADMODEL_H__
#define __LOADMODEL_H__

#include "playgocontroller.h"
#include "polygon2d.h"

namespace CDI
{
	class LoadModel
	{
		PlayGoController* 	controller;
		Page* 				page;

	public:
		LoadModel(PlayGoController* playgocontroller)
		{
			//
			controller = playgocontroller;
			page = controller->_page;
		}

		void createComponents()
		{
			// 1
			vector<Component*> componentList;
			int num_componnets = 10;

			// create 4 components
			for (int i=0; i < num_componnets; i++)
			{
				Component* component = page->createComponent();
				componentList.push_back(component);
				component->setPos(0,0);
			}
			for (Component* component : componentList)
			{
				//
				Polygon2D* polygon = new Polygon2D(component);
				component->addToComponent(polygon);
			}
		}

		void createPolygon(QPointF pos, vector<QPointF> points)
		{

		}

		void loadImage(QPointF pos, QString imagePath)
		{

		}

		void loadImage(QPointF pos, QPixmap image)
		{

		}
	};
}
#endif //__LOADMODEL_H__
