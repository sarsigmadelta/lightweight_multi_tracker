#include "data.hpp"
#include <cstdlib> 
#include <ctime>
#include "centroidtracker.h"
#include "trackableobject.h"


std::map<int, std::shared_ptr<Trackableobject>> trackableObjects;
std::shared_ptr<centroidtracker> ct = std::shared_ptr<centroidtracker>(new centroidtracker(20, 50));


void tracker_function(std::vector<rect>& rects, const cv::Mat& frame){
    if (!rects.empty()) {
        ct->update(rects);
        std::map<int, centroid> objects = ct->objects;
        std::shared_ptr<Trackableobject> to;
        for (auto iter = objects.begin(); iter != objects.end(); ++iter) {
            int objectID = iter->first;
            centroid _centroid = iter->second;
            auto iter_to = trackableObjects.find(objectID);

            if (iter_to == trackableObjects.end()) {
                to = std::shared_ptr<Trackableobject>(new Trackableobject(objectID, _centroid));
            }
            else{
                to = iter_to->second;
                to->append(_centroid);
            }
            trackableObjects.insert(std::pair<int, std::shared_ptr<Trackableobject>>(objectID, to));
        }
        for(auto iter = trackableObjects.begin(); iter != trackableObjects.end(); ++iter)
        {
            int ObjId = iter->first;
            auto data = iter->second;

            cv::putText(frame, std::to_string(ObjId), cv::Point(data->centroids.back().x, data->centroids.back().y), 
            cv::FONT_HERSHEY_TRIPLEX, 0.8, cv::Scalar(0, 255, 0), 1, cv::LINE_AA);

        }
    }
}


int main()
{
    WalkerManager* manager = new WalkerManager(5);

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