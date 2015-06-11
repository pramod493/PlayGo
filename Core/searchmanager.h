#pragma once
#include <QObject>
#include <QString>
#include <QList>
#include <QImage>
#include <QFile>
#include <QTextStream>

// Juxtapoze
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include "wbbice.h"
#include "wbsearchengine.h"

namespace CDI
{
    class SearchManager : public QObject
    {
        Q_OBJECT
    public:
		QString databaseDir;
		QString inputFilePath;
        QString resultFilePath;
		QString origImagePath;

		bool limitSearchByTag;

        QList<QString> localFileList;
		QList<QString> tags;

        wbSearchEngine* searchEngine;

	private:
		static SearchManager* _instance;

	protected:
		/**
		 * @brief Private constructor of SearchManager to maintain uniqueness
		 * @param parent Parent object(defaults to NULL)
		 */
        SearchManager(QObject *parent=0);

	public:
		/**
		 * @brief Returns instance of SearchEngine
		 * @return SearchEngine pointer
		 */
		static SearchManager* instance();

		/** @brief Deletes the wbSearchEngine instance and clears the file list
		  */
        ~SearchManager();

		/**
		 * @brief refresh creates new instance of search engine based on the new param values
		 */
		void refresh();

		/**
		 * @brief Searches based on the input image
		 * @param image Input Sketch
		 * @param numResults Number of results to store
		 * @return True on complete. False on error
		 */
        bool search(QImage &image, int numResults=10);

		/**
		 * @brief Searches based on image store at filePath
		 * @param filePath Input image path
		 * @param numResults Number of resulsts to store
		 * @return True on complete. False on error
		 */
		bool search(QString filePath, int numResults= 20);

        void ConvertResultsToLocalPath(int numResults);

	signals:
		void onSearchComplete(SearchManager* searchmanager);

    };
}
