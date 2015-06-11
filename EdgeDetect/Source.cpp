#include "FindContours.h"

using namespace std;

int main()
{
	// Dir
	char* imgDir = "database/1240215_04.png";
	vector<vector<cv::Point> > outerContours;
	vector<vector<vector<cv::Point> > > innerContours;

	bool savefortesting = true;
	gbFindContours(imgDir, outerContours, innerContours, savefortesting);

	std::cin.get();
	return(0);
}

