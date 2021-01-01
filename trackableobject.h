#ifndef TRACKABLEOBJECT_H
#define TRACKABLEOBJECT_H

#include "centroidtracker.h"

class Trackableobject {

public:
	Trackableobject(int _objectID, centroid c) {
		this->centroids.push_back(c);
		this->objectID = _objectID;
	}
	Trackableobject() {
		this->centroids = std::vector<centroid>();
	}

	~Trackableobject() {}

	void append(centroid c) {
		this->centroids.push_back(c);
	}

public:
	std::vector<centroid> centroids;
	int objectID;
	bool counted;
	bool ready_to_counted;
};


#endif
