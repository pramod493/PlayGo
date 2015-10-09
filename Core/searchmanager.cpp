#include "searchmanager.h"
#include <QDir>
#include "QsLog.h"
#include <vector>
#include "page.h"
#include "searchresult.h"
#include <QFileInfo>
#include <QFile>
#include <QDir>

#include <QMessageBox>

using namespace std;
using namespace CDI;
vector<string> getDummyResults()
{
	vector<string> ret;
	ret.push_back(getHomeDirectory().toStdString() +"/trans/GEAR32_15.png"                    );
	ret.push_back(getHomeDirectory().toStdString() +"/trans/GEAR32_15.png"                    );
	ret.push_back(getHomeDirectory().toStdString() +"/trans/GEAR32_15.png"                    );
	ret.push_back(getHomeDirectory().toStdString() +"/trans/GEAR32_15.png"                    );
	ret.push_back(getHomeDirectory().toStdString() +"/trans/GEAR32_15.png"                    );
	ret.push_back(getHomeDirectory().toStdString() +"/trans/GEAR32_15.png"                    );
	ret.push_back(getHomeDirectory().toStdString() +"/trans/GEAR32_15.png"                    );
	ret.push_back(getHomeDirectory().toStdString() +"/trans/GEAR32_15.png"                    );
	ret.push_back(getHomeDirectory().toStdString() +"/trans/GEAR32_15.png"                    );
	ret.push_back(getHomeDirectory().toStdString() +"/trans/GEAR32_15.png"                    );
	ret.push_back(getHomeDirectory().toStdString() +"/trans/GEAR32_15.png"                    );
	ret.push_back(getHomeDirectory().toStdString() +"/trans/GEAR32_15.png"                    );
	ret.push_back(getHomeDirectory().toStdString() +"/trans/GEAR32_15.png"                    );
	ret.push_back(getHomeDirectory().toStdString() +"/trans/GEAR32_15.png"                    );
	ret.push_back(getHomeDirectory().toStdString() +"/trans/GEAR32_15.png"                    );
	ret.push_back(getHomeDirectory().toStdString() +"/trans/GEAR32_15.png"                    );
	ret.push_back(getHomeDirectory().toStdString() +"/trans/GEAR32_15.png"                    );
	ret.push_back(getHomeDirectory().toStdString() +"/trans/GEAR32_15.png"                    );
	ret.push_back(getHomeDirectory().toStdString() +"/trans/GEAR32_15.png"                    );
	ret.push_back(getHomeDirectory().toStdString() +"/trans/GEAR32_15.png"                    );
	return ret;
}

namespace CDI
{
	SearchManager::SearchManager(QObject *parent)
		: QObject(parent)
	{
		databaseDir = getHomeDirectory();
		inputFilePath   = databaseDir + QString("Input.png");
		resultFilePath  = databaseDir + QString("results/results.yml");
		origImagePath   = databaseDir + QString("trans");

		namespace po = boost::program_options;
		path datapasePath = path(databaseDir.toStdString());
		wbBICE *biceDescriptor = new wbBICE();
		searchEngine = new wbSearchEngine(datapasePath, biceDescriptor);

		_databaseIndexed = false;
		//searchEngine->Index();
		//searchEngine->Load();
	}

	SearchManager::~SearchManager()
	{
		if (searchEngine!= NULL) delete searchEngine;
	}

	void SearchManager::reload()
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

	QList<SearchResult*> SearchManager::search(QImage &image, int numResults)
	{
		if (image.isNull()) return QList<SearchResult*>();
		image.save(inputFilePath);
		return search(inputFilePath, numResults);
	}

	QList<SearchResult*> SearchManager::search(QString filePath, int numResults)
	{
		vector<string> results;
		if(false)	// TODO - Eliminate for now
		{
			results = getDummyResults();
		} else
		{
			if (_databaseIndexed == false)
			{
				_databaseIndexed = true;
				searchEngine->Index();
				searchEngine->Load();
			}
			results = searchEngine->Query(filePath.toStdString(), numResults);
		}
		QDir originialImageDir(origImagePath);
		QList<SearchResult*> searchResults;
		for(auto filepath : results)
		{
			QString str = QString::fromStdString(filepath);
			QFileInfo fileinfo = QFileInfo(str);
			QString justFilename(fileinfo.fileName());
			QString newFilepath = originialImageDir.filePath(justFilename);
			auto searchResult = new SearchResult(newFilepath);
			searchResults.push_back(searchResult);
		}
		return searchResults;
	}
}
