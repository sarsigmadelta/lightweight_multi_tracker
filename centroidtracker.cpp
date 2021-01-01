#include "centroidtracker.h"


centroidtracker::centroidtracker(int _maxDisappeared, int _maxDistance) {
	this->nextObjectID = 0;
	this->maxDisappeared = _maxDisappeared;
	this->maxDistance = _maxDistance;
}

void centroidtracker::regster(centroid c) {
	this->objects.insert(std::pair<int, centroid>(this->nextObjectID, c));
	this->disappeared.insert(std::pair<int, int>(this->nextObjectID, 0));
	nextObjectID++;
}

void centroidtracker::deregster(int objectID) {
	this->objects.erase(objectID);
	this->disappeared.erase(objectID);
}


std::vector<int> centroidtracker::argsort(const std::vector<double>& a) {
	int Len = a.size();
	std::vector<int> idx(Len, 0);
	for (int i = 0; i < Len; i++) {
		idx[i] = i;
	}
	std::sort(idx.begin(), idx.end(), [&a](int i1, int i2) {return a[i1] < a[i2]; });
	return idx;
}

inline double distanceOfPts(const centroid pts1, const centroid pts2) {
	return sqrt((pts1.x - pts2.x) * (pts1.x - pts2.x) + (pts1.y - pts2.y) * (pts1.y - pts2.y));
}

std::vector<std::vector<double>> centroidtracker::cdist(const std::vector<centroid> data_1, const std::vector<centroid> data_2) {
	std::vector<std::vector<double>> result;
	for (int i = 0; i < data_1.size(); ++i) {
		std::vector<double> result_row;
		for (int j = 0; j < data_2.size(); ++j) {
			result_row.push_back(distanceOfPts(data_1.at(i), data_2.at(j)));
		}
		result.push_back(result_row);
	}
	return result;
}

dataStrip centroidtracker::stripForCentroid(const std::map<int, centroid> data) {
	dataStrip result;
	for (auto iter = data.begin(); iter != data.end(); ++iter) {
		result.keys.push_back(iter->first);
		result.values.push_back(iter->second);
	}
	return result;
}

rowCol centroidtracker::getRowsCols(const std::vector<std::vector<double>> D) {
	std::vector<int> rowResult;
	std::vector<double> minValue;
	std::vector<int> colTmp;
	std::vector<int> colResult;
	for (auto iter = D.begin(); iter != D.end(); ++iter) {
		minValue.push_back(*std::min_element(iter->begin(), iter->end()));
		colTmp.push_back(std::min_element(iter->begin(), iter->end()) - iter->begin());
	}
	rowResult = argsort(minValue);

	for (int i = 0; i < colTmp.size(); ++i) {
		colResult.push_back(colTmp.at(rowResult.at(i)));
	}
	rowCol result;
	result.cols = colResult;
	result.rows = rowResult;

	return result;
}

void centroidtracker::update(const std::vector<rect>& rects) {
	if (rects.empty()) {
		for (auto iter = this->disappeared.begin(); iter != this->disappeared.end(); ++iter) {
			++(iter->second);
			if ((iter->second) > (this->maxDisappeared))
				deregster(iter->second);
		}
		return;
	}
	std::vector<centroid> inputCentroids;

	int i = 0;
	for (auto iter = rects.begin(); iter != rects.end(); ++iter) {
		int cX = (int)((iter->x1 + iter->x2) / 2);
		int cY = (int)((iter->y1 + iter->y2) / 2);
		centroid tmpCentroid;
		tmpCentroid.x = cX;
		tmpCentroid.y = cY;
		inputCentroids.push_back(tmpCentroid);
		++i;
	}

	if (this->objects.empty()) {
		for (auto iter = inputCentroids.begin(); iter != inputCentroids.end(); ++iter) {
			regster(*iter);
		}
	}
	else {
		dataStrip dataStripObject = stripForCentroid(this->objects);
		std::vector<int> objectIDs = dataStripObject.keys;
		std::vector<centroid> objectCentroids = dataStripObject.values;
		std::vector<std::vector<double>> D = cdist(objectCentroids, inputCentroids);
		rowCol rowsCols = getRowsCols(D);
		std::vector<int> rows = rowsCols.rows;
		std::vector<int> cols = rowsCols.cols;

		std::unordered_set<int> usedRows;
		std::unordered_set<int> usedCols;
		int objectID;

		for (int i = 0; i < rows.size(); ++i) {
			int row = rows.at(i);
			int col = cols.at(i);
			if ((usedRows.find(row) != usedRows.end()) || (usedCols.find(col) != usedCols.end())) {
				continue;
			}
			if (D.at(row).at(col) > maxDistance) {
				continue;
			}

			objectID = objectIDs.at(row);
			//this->objects.insert(std::pair<int, centroid>(objectID, inputCentroids.at(col)));
			this->objects[objectID] = inputCentroids.at(col);
			disappeared[objectID] = 0;
			usedRows.insert(row);
			usedCols.insert(col);
		}

		std::unordered_set<int> unusedRows;
		std::unordered_set<int> unusedCols;
		for (int i = 0; i < D.size(); ++i) {
			if (usedRows.find(i) != usedRows.end())
				continue;
			unusedRows.insert(i);
		}

		for (int j = 0; j < D.at(0).size(); ++j) {
			if (usedCols.find(j) != usedCols.end())
				continue;
			unusedCols.insert(j);
		}

		if (D.size() > D.at(0).size()) {
			for (auto row = unusedRows.begin(); row != unusedRows.end(); ++row) {
				objectID = objectIDs.at(*row);
				++disappeared[objectID];
			}
			if (disappeared[objectID] > maxDisappeared)
				deregster(objectID);
		}
		else {
			for (auto col = unusedCols.begin(); col != unusedCols.end(); ++col) {
				regster(inputCentroids.at(*col));
			}
		}

	}

}