#pragma once
#include <QtAlgorithms>
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
#include "asmopencv.h"
#include "searchresult.h"
namespace CDI
{
    /**
     * @brief Initializes the search engine as well as manages
     * the tags used to filter search results.
     * @todo Use the search tags to populate the search results or read
     * from another database.
     */
    class SearchManager : public QObject
    {
        Q_OBJECT
    public:
		QString databaseDir;
		QString inputFilePath;
        QString resultFilePath;
		QString origImagePath;

		bool limitSearchByTag;

		QList<QString> tags;

        wbSearchEngine* searchEngine;

	protected:
		bool _databaseIndexed;

	public:
		/**
         * @brief Constructor of SearchManager to maintain uniqueness
		 * @param parent Parent object(defaults to NULL)
		 */
        SearchManager(QObject *parent=0);

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
         * @return List of SearchResult objects
		 */
        QList<SearchResult *> search(QImage &image, int numResults=10);

		/**
		 * @brief Searches based on image store at filePath
		 * @param filePath Input image path
		 * @param numResults Number of resulsts to store
		 * @return True on complete. False on error
		 */
        QList<SearchResult *> search(QString filePath, int numResults= 20);

    };
}
