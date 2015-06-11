#include <QApplication>
#include <iostream>
#include <QPainter>
#include <QPixmap>
#include <QImage>
#include <QFile>

#include "playgocore.h"

using namespace CDI;

bool enableDebugDetails;

void testPageCreation()
{
	std::cout << "Testing Page creation" << std::endl;
	PlayGo *playgo = new PlayGo();
	Page* page = playgo->addNewPage();
	playgo->setCurrentPage(page);
	for (int i=0; i< 10; i++)
	{
		playgo->addNewPage();
	}
	if (enableDebugDetails)
	{
		qDebug() << "Added 11 pages to PlayGo object"
				 << "Attemptig to print";
		qDebug() << "\n\n";
		qDebug() << "PlayGo current page" << playgo->currentPage()->id();
	}
	if (playgo->currentPage()->id() != page->id())
		qDebug() << "ERROR! Page ID does not match.";
	else
		qDebug() << "INFO! SUCCESS";
	if (enableDebugDetails)
	{
		qDebug() << "Printing all Page details";
		int i=1;
		QList<Page*> pages = playgo->values();
		foreach (Page* _page, pages)
		{
			qDebug() << i << _page->id().toString();
			i++;
		}
	}

}

// Test create assembly
void testAssemblyCreation()
{
	std::cout << "Testing Assembly and Component creation" << std::endl;
	PlayGo *playgo = new PlayGo();
	Page* page = playgo->addNewPage();
	playgo->setCurrentPage(page);

	Component* component = NULL;
	Assembly* assem = NULL;

	int num_assems = 0;
	int num_comps = 0;
	for (int i=1; i< 10; i++)
	{
		assem = page->createAssembly();
		num_assems ++;
		for (int j=i; j < 2*i; j++)
		{
			component = page->createComponent();
			assem->addItem(component);
			num_comps ++;
		}
	}
	if (enableDebugDetails)
	{
		qDebug() << num_assems << "assemblies created";
		qDebug() << num_comps << "components created";
	}

	if (page->getAssemblies().size() == num_assems &&
			page->getComponents().size()  == num_comps)
		qDebug() << "INFO! SUCCESS";
	else {
		qDebug() << "ERROR! Number of components and assemblies do not match";
	}
}

void testItemInsertion()
{
	std::cout << "Testing Assembly and Component creation" << std::endl;
	PlayGo *playgo = new PlayGo();
	Page* page = playgo->addNewPage();
	playgo->setCurrentPage(page);

	Component* component = NULL;
	Assembly* assem = NULL;

	int num_assems = 0;
	int num_comps = 0;
	for (int i=1; i< 10; i++)
	{
		assem = page->createAssembly();
		num_assems ++;
		for (int j=i; j < 2*i; j++)
		{
			component = page->createComponent();
			assem->addItem(component);
			num_comps ++;
		}
	}
	if (enableDebugDetails)
	{
		qDebug() << num_assems << "assemblies created";
		qDebug() << num_comps << "components created";
	}

	if (page->getAssemblies().size() == num_assems &&
			page->getComponents().size()  == num_comps)
		qDebug() << "INFO! Component and assemblies match";
	else {
		qDebug() << "ERROR! Number of components and assemblies do not match";
	}

	Stroke* stroke = component->addStroke(Qt::blue, 3.5f);
	Polygon2D* polygon = new Polygon2D();
	component->addItem(polygon);

	if (!(component->containsItem(polygon) && component->containsItem(stroke)))
	{
		qDebug() << "ERROR!@search_by_ptr Items not found in component";
	} else
	{
		qDebug() << "INFO!@search_by_ptr SUCCESS. Items found in component";
	}
	if (!(component->containsItem(polygon->id()) && component->containsItem(stroke->id())))
	{
		qDebug() << "ERROR!@search_by_id Items not found in component";
	} else
	{
		qDebug() << "INFO!@search_by_id SUCCESS. Items found in component";
	}
	if (!(assem->containsItem(polygon->id(), true) && assem->containsItem(stroke->id(), true)))
	{
		qDebug() << "ERROR!@search_by_id_recursive Items not found in assembly";
	} else
	{
		qDebug() << "INFO!@search_by_id_recurisve SUCCESS. Items found in assembly";
	}

	if (!(assem->containsItem(polygon, false) && assem->containsItem(stroke, false)))
	{
		qDebug() << "INFO!@search_by_id Items not found in assembly. Expected.";
	} else
	{
		qDebug() << "ERROR!@search_by_id. Failure";
	}

	if (page->destroyAssembly(assem) == false)
	{
		qDebug() << "Error in deletion of assembly";
	}

	qDebug() << "INFO! Component and assembly destroyed";
	qDebug() << page->getAssemblies().size() << num_assems
			 << page->getComponents().size() << num_comps;
	return;
	if (page->getAssemblies().size() == (num_assems-1) &&
			page->getComponents().size()  == (num_comps-1))
		qDebug() << "INFO! Component and assembly deleted";
	else
		qDebug() << "ERROR! Component and assemblies match. UNEXPECTED";

}

void testCoreDataStruct()
{
	//testPageCreation();
	//	testAssemblyCreation();
	testItemInsertion();
}

int main(int argc, char** argv)
{
	enableDebugDetails = false;
	QApplication app(argc, argv);

	testCoreDataStruct();

	return 1;
}
