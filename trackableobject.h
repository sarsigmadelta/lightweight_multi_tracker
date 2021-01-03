#ifndef TRACKABLEOBJECT_H
#define TRACKABLEOBJECT_H

#include "centroidtracker.h"

class Trackableobject {

public:
	Trackableobject(int _objectID, centroid c, rect r) {
		this->objectID = _objectID;
		maxRemenbers = 100;
		this->locations.push(std::pair<centroid, rect>(c, r));

		lastlocation = std::pair<float, float>(c.x, c.y);
	}
	Trackableobject() {}

	~Trackableobject() {}

	void append(centroid c, rect r) {
		if(this->locations.size()==maxRemenbers){
			this->locations.pop();
		}
		if(this->speeds.size()==maxRemenbers){
			this->speeds.pop();
		}
		if(this->accelerations.size()==maxRemenbers){
			this->accelerations.pop();
		}
		this->locations.push(std::pair<centroid, rect>(c, r));

		// ----------------about the speeds and accelerations------------------------
		{
			//std::pair<float, float> lastlocation = speeds.back();
			float sX = c.x - lastlocation.first;
			float sY = c.y - lastlocation.second;
			
			speeds.push(std::pair<float, float>(sX, sY));

			lastlocation = std::pair<float, float>(c.x, c.y);
			lastspeed = std::pair<float, float>(sX, sY);

			predRect = rect(r.x1 + sX, r.y1 + sY, 
							r.x2 + sX, r.y2 + sY);
		}

		{

			float sX = c.x - lastlocation.first;
			float sY = c.y - lastlocation.second;

			float aX = sX - lastspeed.first;
			float aY = sY - lastspeed.second;

			accelerations.push(std::pair<float, float>(aX, aY));
		}
	}


public:
	std::queue<std::pair<centroid, rect>> locations;
	std::queue<std::pair<float,float>> speeds;
	std::queue<std::pair<float,float>> accelerations;

	rect predRect;

	std::pair<float,float> lastlocation;
	std::pair<float,float> lastspeed;
	int maxRemenbers;
	int objectID;
};


#endif
