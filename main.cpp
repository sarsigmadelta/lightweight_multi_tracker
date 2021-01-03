#include "data.hpp"
#include <cstdlib> 
#include <ctime>
#include "centroidtracker.h"
#include "trackableobject.h"


std::map<int, std::shared_ptr<Trackableobject>> trackableObjects;
std::shared_ptr<centroidtracker> ct = std::shared_ptr<centroidtracker>(new centroidtracker(20, 140));


void tracker_function(std::vector<rect>& rects, const cv::Mat& frame){
    if (!rects.empty()) {
        ct->update(rects);
        std::map<int, centroid> objects = ct->objects;
        std::map<int, rect> objectsRec = ct->objectsOfRect;

        std::shared_ptr<Trackableobject> to;
        for (auto iter = objects.begin(); iter != objects.end(); ++iter) {
            int objectID = iter->first;
            //centroid _centroid = iter->second;
            centroid _centroid;
            rect rr = objectsRec[objectID];
            _centroid.x = 0.5 * ( rr.x1 + rr.x2 );
            _centroid.y = 0.5 * ( rr.y1 + rr.y2 );
            auto iter_to = trackableObjects.find(objectID);
            if (iter_to == trackableObjects.end()) {
                to = std::shared_ptr<Trackableobject>(new Trackableobject(objectID, _centroid, rr));
            }
            else{
                to = iter_to->second;
                to->append(_centroid, rr);
            }
            trackableObjects.insert(std::pair<int, std::shared_ptr<Trackableobject>>(objectID, to));
        }
        for(auto iter = trackableObjects.begin(); iter != trackableObjects.end(); ++iter)
        {
            int ObjId = iter->first;
            auto data = iter->second;
            cv::putText(frame, std::to_string(ObjId), cv::Point(data->locations.back().first.x, data->locations.back().first.y), 
            cv::FONT_HERSHEY_TRIPLEX, 0.8, cv::Scalar(0, 255, 0), 1, cv::LINE_AA);

            if(!iter->second->speeds.empty()){
                auto speeds = iter->second->speeds.back();
                cv::putText(frame, std::string("x:") + std::to_string(speeds.first) + std::string("  y:") + std::to_string(speeds.second), 
                cv::Point(data->locations.back().first.x + 10, data->locations.back().first.y + 10), 
                cv::FONT_HERSHEY_TRIPLEX, 0.4, cv::Scalar(0, 255, 0), 1, cv::LINE_AA);
            }

            rect r = iter->second->predRect;

            cv::rectangle(frame, cv::Point(r.x1, r.y1), cv::Point(r.x2, r.y2), 
            cv::Scalar(0, 255, 0), 2, 8, 0);
        }
    }
}


int main()
{
    WalkerManager* manager = new WalkerManager(3);

    for(int i=0; i=100; ++i)
    {
        cv::Mat image = cv::Mat::zeros(600, 600, CV_8UC3);
        manager->UpdateAllWalkers();
        manager->ShowAllWalkers(image);

        std::vector<rect> rs = manager->GetRectFromManager();
        tracker_function(rs, image);

        cv::imshow("image ", image);
        cv::waitKey(50);
    }

    delete manager;
}