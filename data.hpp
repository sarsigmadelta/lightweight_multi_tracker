#ifndef DATA_HPP
#define DATA_HPP


#include <opencv2/opencv.hpp>
#include <vector>
#include <memory>
#include <string>
#include "centroidtracker.h"

struct Mrect;
typedef struct Mrect
{
    int x1;
    int y1;
    int x2;
    int y2;

    Mrect(int _x1, int _y1, int _x2, int _y2):
    x1(_x1), y1(_y1), x2(_x2), y2(_y2){}

}Mrect;

struct WalkSpeed;
typedef struct WalkSpeed
{
    float sX;
    float sY;

}WalkSpeed;



struct MWakler;
typedef struct MWakler
{
    int idx;
    std::vector<Mrect> mRects;
    WalkSpeed mSpeed;

}MWakler;


inline void UpdateRandSpeed(std::shared_ptr<MWakler> walker, const float baseX = 4., const float baseY = 4.)
{
    walker->mSpeed.sX = baseX + 10. - std::rand() % 1000 / 200.;
    walker->mSpeed.sY = baseY + 10. - std::rand() % 1000 / 200.;
}


inline void RandWalk(std::shared_ptr<MWakler> walker)
{
    if(walker->mRects.empty())
    {
        int randX = std::rand() % 100;
        int randY = std::rand() % 100;

        Mrect r{10 + randX,10 + randY,50 + randX,50 + randY};
        walker->mRects.push_back(r);
        return ;
    }

    Mrect r = walker->mRects.back();
    r.x1 += walker->mSpeed.sX;
    r.x2 += walker->mSpeed.sX;
    r.y1 += walker->mSpeed.sY;
    r.y2 += walker->mSpeed.sY;
    walker->mRects.push_back(r);
    return;
}

class WalkerManager
{

public:
    explicit WalkerManager(int NumWalkers)
    {
        nextID = 0;
        srand(time(NULL));

        for(int i=0; i<NumWalkers; ++i)
        {
            GenNewWaler();
            //++nextID;
        }
    };

    void GenNewWaler()
    {
        std::shared_ptr<MWakler> walker = std::make_shared<MWakler>();
        UpdateRandSpeed(walker);
        walker->idx = nextID++;

        walkers.push_back(walker);
    }

    void UpdateAllWalkers()
    {
        for(auto iter=walkers.begin(); iter != walkers.end(); ++iter)
        {
            UpdateRandSpeed(*iter);
            RandWalk(*iter);
        }
    }

    void ShowAllWalkers(const cv::Mat& frame)
    {
        for(auto iter=walkers.begin(); iter != walkers.end(); ++iter)
        {
            Mrect r = iter->get()->mRects.back();
            cv::rectangle(frame, cv::Point(r.x1, r.y1), cv::Point(r.x2, r.y2), 
            cv::Scalar(0, 255, 255), 2, 8, 0);

            cv::putText(frame, std::string("id:") + std::to_string(iter->get()->idx), 
            cv::Point(r.x1, r.y1), cv::FONT_HERSHEY_COMPLEX, 0.5, cv::Scalar(0, 0, 255), 2, 2, 0);
        }
    }

    std::vector<rect> GetRectFromManager()
    {
        std::vector<rect> retcts;
        //retcts.resize(walkers.size());

        for(int i=0; i<walkers.size(); ++i)
        {
            Mrect r = walkers.at(i)->mRects.back();
            rect r_t{r.x1, r.y1, r.x2, r.y2};
            retcts.push_back(r_t);
        }
        return retcts;
    }

private:
    std::vector<std::shared_ptr<MWakler>> walkers;
    int nextID;
};





#endif