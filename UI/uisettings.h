#pragmna once
#include <QString>
#include <QDataStream>

//// Use this file to store the values which can be accessed from anywhere
namespace CDI
{

	//todo -- singleton class for loading the uisettings... pref  from user area
	// do not implement now. for very later into future
	// main window settings
	bool MainWindowFullscreen = true;
	bool MainWindowTransparent = false;

	float MainWindowAlpha = 0.75f;

	//// Search related variables
	QString SketchSaveFile("Imput.png");
	QString DatabaseDirectory("../../Database/Images/");
	QString LogFilePath("./Debug/LogFile.txt");

	// Save settings using this file
	QDataStream& writeSettingsData(QDataStream& stream)
	{
		QString title("Settings data");
		stream << title;
		stream << MainWindowFullscreen;
		stream << MainWindowTransparent;
		stream << MainWindowAlpha;
		stream << SketchSaveFile;
		stream << DatabaseDirectory;
		stream << LogFilePath;
		return stream;
	}

	// Read setting back here
	QDataStream& readSettingsData(QDataStream& stream)
	{
		QString title;
		stream >> title;
		stream >> MainWindowFullscreen;
		stream >> MainWindowTransparent;
		stream >> MainWindowAlpha;
		stream >> SketchSaveFile;
		stream >> DatabaseDirectory;
		stream >> LogFilePath;
		return stream;
	}
}
