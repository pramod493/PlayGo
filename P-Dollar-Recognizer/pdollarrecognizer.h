#ifndef PDOLLARRECOGNIZER
#define PDOLLARRECOGNIZER

// A Simple Class That Load Template Files
// And recognize strokes as one of them
// For convinience, templates should be saved in
// different files
// So we might as well provide the folder
// containing all templates at the beginning


#include <QObject>
#include <vector>
#include <string>
#include <algorithm>
#include <QString>
#include <math.h>

#include <QDataStream>
#include <QFile>
#include <QIODevice>
#include <QDir>
#include <QStringList>

#define NUMBER_OF_POINTS_IN_TEMPLATE 32

/* Usage:
 * Initialize the recognizer using pdrRecognizer->loadPDRTemplates(<gesture directory>);
 * Add stroke using pdrRecognizer->addStroke(QVector<points>)
 *
 * Use pdrRecognizer->gbRecognize() to recognize the gesture pushed into it
 *  - Returns the gesture name
 *  - emits onGestureDetect(<gesture name>, <score>) with gesture name and score values
 * To save gesture use: pdrRecognizer->savePDRTemplate(<file path>, <gesture name>);
 *
 * Once detection/saving over, use pdrRecognizer->clean() to reset the current values
 *
 * Once detection is complete, clear the recognizer using pdrRecognizer->clean()
 *
*/

struct pdrPoint
{
	double x;
	double y;
	int strokeID;
};
typedef pdrPoint pdrPoint;

struct pdrTemplate
{
	int n;  // number of stroke
	std::vector<pdrPoint> points;
	QString gestureClass;   // To avoid some convertion

	//std::string gestureClass;

};
typedef pdrTemplate pdrTemplate;


class PDollarRecognizer : public QObject
{
	Q_OBJECT

public:
	std::vector<pdrTemplate> currentTemplates;
	// Gesture to be recognized and saved will be stored in this one. So i need to create one
	pdrTemplate toBeRecognize;

public:
	// Constructor
	PDollarRecognizer(QObject *parent = 0);

	// Clean Up
	void clean();

	// Load existing templates
	// void loadPDRTemplates(std::string tmpDir);
	void loadPDRTemplates(QString tmpDir);

	// Add new template (probably will be done automatically)
	// Add to the current memory(currentTemplate);
	void addPDRTemplate(QString gestureClass);   // save to default path

	// Save new template
	// Save to file
	// void savePDRTemplate(std::string tmpDir, std::string gestureClass);
	void savePDRTemplate(QString tmpDir, QString gestureClass);

	void addStroke(QVector<QPointF> points);

	// The most important part
	QString gbRecognize();

protected:
	// I dont see any needed

private:
	// Normalize point cloud
	void Normalize();   // will normalize toBeRecognize
	double GreedyCloudMatch(pdrTemplate pnt,pdrTemplate tmpl);
	double CloudDistance(pdrTemplate pnt,pdrTemplate tmpl,int start);

	// Help Functions
	double EuclideanDistance(pdrPoint p1, pdrPoint p2);

signals:
	void onGestureDetect(QString gestureName, float gestureScore);

};



#endif // PDOLLARRECOGNIZER

