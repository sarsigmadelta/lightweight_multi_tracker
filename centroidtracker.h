#ifndef CENTROIDTRACKER_H
#define CENTROIDTRACKER_H

#include <map>
#include <vector>
#include <algorithm>
#include <math.h>
#include <unordered_set>
#include <opencv2/opencv.hpp>


struct centroid {
	int x;
	int y;
};

struct rect
{
	int x1;
	int y1;
	int x2;
	int y2;
	rect(int _x1, int _y1, int _x2, int _y2) :x1(_x1), y1(_y1), x2(_x2), y2(_y2) {}
	rect(const cv::Rect& r)
	{
		x1 = r.x;
		y1 = r.y;
		x2 = r.x + r.width;
		y2 = r.y + r.height;
	}
};

struct dataStrip {
	std::vector<int> keys;
	std::vector<centroid> values;
};

struct rowCol {
	std::vector<int> rows;
	std::vector<int> cols;
};


class centroidtracker {

public:
	centroidtracker(int _maxDisappeared, int _maxDistance);

	~centroidtracker() {}

	void regster(centroid c);
	void deregster(int objectID);
	void update(const std::vector<rect>& rects);
	dataStrip stripForCentroid(const std::map<int, centroid> data);
	std::vector<std::vector<double>> cdist(const std::vector<centroid> data_1, const std::vector<centroid> data_2);
	std::vector<int> argsort(const std::vector<double>& a);
	rowCol getRowsCols(const std::vector<std::vector<double>> D);


public:
	int nextObjectID = 0;
	std::map<int, centroid> objects;
	std::map<int, int> disappeared;
	int maxDisappeared;
	int maxDistance;
};

#endif

