#pragmna once
#include <QString>

//// Use this file to store the values which can be accessed from anywhere
namespace CDI
{
	// main window settings
	bool MainWindowFullscreen = true;
	bool MainWindowTransparent = false;

	float MainWindowAlpha = 0.75f;

	//// Search related variables
	QString SketchSaveFile("Imput.png");
	QString DatabaseDirectory("../../Database/Images/");
	QString LogFilePath("./Debug/LogFile.txt");


}
