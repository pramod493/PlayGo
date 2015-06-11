#include <QApplication>
#include "reconizerwindow.h"

int main (int argc, char* argv[])
{
	QApplication app(argc, argv);

	ReconizerWindow window;
	window.show();

	return app.exec();
}
