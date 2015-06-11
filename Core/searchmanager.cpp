#include "searchmanager.h"
#include <QDir>
#include <QDebug>
#include <vector>
#include "page.h"
#include "searchresult.h"

using namespace std;
namespace CDI
{
    SearchManager::SearchManager(QObject *parent)
        : QObject(parent)
    {
		databaseDir     = QString("C:/Database/");
		inputFilePath   = databaseDir + QString("Input.png");
		resultFilePath  = databaseDir + QString("results/results.yml");
		origImagePath   = databaseDir + QString("trans");

        localFileList = QList<QString>();

        namespace po = boost::program_options;
        path datapasePath = path(databaseDir.toStdString());
        wbBICE *biceDescriptor = new wbBICE();
        searchEngine = new wbSearchEngine(datapasePath, biceDescriptor);
        // TODO - Enable indexing for test
        searchEngine->Index();
        searchEngine->Load();
    }

	SearchManager* SearchManager::_instance = NULL;

	SearchManager* SearchManager::instance()
	{
		if (_instance == NULL) _instance = new SearchManager();
		return _instance;
	}

    SearchManager::~SearchManager()
    {
        if (searchEngine!= NULL) delete searchEngine;
        localFileList.clear();
    }

	void SearchManager::refresh()
	{
		if (searchEngine!= NULL) delete searchEngine;
		namespace po = boost::program_options;
		path datapasePath = path(databaseDir.toStdString());
		wbBICE *biceDescriptor = new wbBICE();
		searchEngine = new wbSearchEngine(datapasePath, biceDescriptor);
		// TODO - Enable indexing for test
		searchEngine->Index();
		searchEngine->Load();
	}

    bool SearchManager::search(QImage &image, int numResults)
    {
        if (image.isNull()) return false;
		image.save(inputFilePath);
        //ConvertResultsToLocalPath(numResults);
		return search(inputFilePath, numResults);
    }

	bool SearchManager::search(QString filePath, int numResults)
	{
		vector<string> results = searchEngine->Query(filePath.toStdString(), numResults);

		localFileList.clear();
		for (vector<string>::iterator it = results.begin();
			 it != results.begin()+numResults; ++it)
		{
			string name = *it;
			localFileList.push_back(QString::fromStdString(name));
		}
		//ConvertResultsToLocalPath(numResults);
		return true;
	}

    void SearchManager::ConvertResultsToLocalPath(int numResults)
    {   // Populates the file list vector
        // No longer needed since we are directly querying the file name from
        // searchEngine
        localFileList.clear();      // Cleanup the search results
        QFile file(resultFilePath);
        if (file.open(QFile::ReadOnly))
        {
            int lineNumber = 1;
            QTextStream in(&file);
            while (!in.atEnd())
            {
                QString line = in.readLine();
                if ((lineNumber > 1) && (lineNumber <= numResults))
                {
                    QStringList list1 = line.split("/");
                    QString result(databaseDir);
                    QString fileName = list1.at(list1.size()-1);
                    fileName.remove('"');
                    fileName.replace(".jpg", ".png");
                    result.append(fileName);
                    localFileList.push_back(result);
                }
                lineNumber++;
            }
        }
    }
}
