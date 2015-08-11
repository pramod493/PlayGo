#include <QtCore/QCoreApplication>

#include"wbbice.h"
#include"wbsearchengine.h"
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <vector>
#include <QDebug>
#include <QString>

using namespace std;

int main(int argc, char *argv[])
{
    std::cout << "Initialized...\n";
    namespace po = boost::program_options;
    std::string file;

    QCoreApplication a(argc, argv);

    std::cout << "Running correctly\n";

#ifdef Q_OS_LINUX
    path data("/home/pramod/Junks/database/");
#else
    //WINDOWS
    path data("C:/Database/");
#endif

    wbBICE * bicedescriptor = new wbBICE();
    wbSearchEngine * engine = new wbSearchEngine(data,bicedescriptor);

#ifdef Q_OS_LINUX
    file = "/home/pramod/Junks/database/Input.png";
#else
    file = "C:/Database/Input.png";
#endif

    engine->Index();
    engine->Load();
	vector<string> names = engine->Query(file,40);
	for(int i =0; i< names.size(); i++)
	{
		qDebug() << QString::fromStdString(names[i]);
	}
    return EXIT_SUCCESS;
}


