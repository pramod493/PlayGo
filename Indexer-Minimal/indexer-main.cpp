#include <QtCore/QCoreApplication>

#include"wbbice.h"
#include"wbsearchengine.h"
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>


using namespace std;

int main(int argc, char *argv[])
{
    std::cout << "Initialized...\n";
    namespace po = boost::program_options;
    std::string file;

    QCoreApplication a(argc, argv);

    std::cout << "Running correctly\n";

    //WINDOWS
    path data("C:/Database/");

    wbBICE * bicedescriptor = new wbBICE();
    wbSearchEngine * engine = new wbSearchEngine(data,bicedescriptor);

    file = "C:/Database/Input.png";

    engine->Index();
    engine->Load();
    engine->Query(file,40);
    return EXIT_SUCCESS;
}


