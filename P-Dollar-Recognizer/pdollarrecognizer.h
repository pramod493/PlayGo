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
    PDollarRecognizer();

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

};



#endif // PDOLLARRECOGNIZER

