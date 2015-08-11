#include "searchmanager.h"
#include <QDir>
#include "QsLog.h"
#include <vector>
#include "page.h"
#include "searchresult.h"
#include <QFileInfo>
#include <QFile>
#include <QDir>

using namespace std;
using namespace CDI;
vector<string> getDummyResults()
{
	vector<string> ret;
	ret.push_back(getHomeDirectory().toStdString() +"/trans/REV_SHAFT_15.png"                );
	ret.push_back(getHomeDirectory().toStdString() +"/trans/BEARING_SHAFT_57_08.png"         );
	ret.push_back(getHomeDirectory().toStdString() +"/trans/SINUSOID.png"                    );
	ret.push_back(getHomeDirectory().toStdString() +"/trans/0_1875_BALL_CUTTER_14.png"       );
	ret.push_back(getHomeDirectory().toStdString() +"/trans/ASHAFT_PRT_06.png"               );
	ret.push_back(getHomeDirectory().toStdString() +"/trans/ARIES63_01.png"                  );
	ret.push_back(getHomeDirectory().toStdString() +"/trans/32T_4D_05W_14.png"               );
	ret.push_back(getHomeDirectory().toStdString() +"/trans/SHAFT_62_08.png"                 );
	ret.push_back(getHomeDirectory().toStdString() + "/trans/NUT_244_BACK_SHORTS_INNER_04.png");
	ret.push_back(getHomeDirectory().toStdString() + "/trans/84TEETH2_14.png"                 );
	ret.push_back(getHomeDirectory().toStdString() + "/trans/GEAR32_15.png"                   );
	ret.push_back(getHomeDirectory().toStdString() + "/trans/8MM_FLATHEAD_SCREWDRIVER_03.png" );
	ret.push_back(getHomeDirectory().toStdString() + "/trans/88T_106D_05W_02.png"             );
	ret.push_back(getHomeDirectory().toStdString() + "/trans/PIVOT_PIN.png"                   );
	ret.push_back(getHomeDirectory().toStdString() + "/trans/TRACK_01.png"                    );
	ret.push_back(getHomeDirectory().toStdString() + "/trans/1250195_02.png"                  );
	ret.push_back(getHomeDirectory().toStdString() + "/trans/54TEETH2_16.png"                 );
	ret.push_back(getHomeDirectory().toStdString() + "/trans/0_1875_BALL_CUTTER_PRT_14.png"   );
	ret.push_back(getHomeDirectory().toStdString() + "/trans/BEARING_SHAFT_57_05.png"         );
	ret.push_back(getHomeDirectory().toStdString() + "/trans/CLUSTER_COUNTERSHAFT_PRT_14.png" );
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
		if(true)
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
		for(vector<string>::iterator it = results.begin();
			it != results.end(); ++it)
		{
			QString str = QString::fromStdString(*it);
			QFileInfo fileinfo = QFileInfo(str);
			QString justFilename(fileinfo.fileName());
			QString newFilepath = originialImageDir.filePath(justFilename);

			SearchResult *searchResult = new SearchResult(newFilepath);
			searchResults.push_back(searchResult);
		}
		return searchResults;
	}
}
