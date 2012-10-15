//
//  Car.h
//  CarSim
//
//  Created by William Lindmeier on 10/15/12.
//
//

#ifndef __CarSim__Car__
#define __CarSim__Car__

#include <iostream>
#include "cinder/Vector.h"
using namespace ci;

#define DegreesToRadians(x) (M_PI * x / 180.0)
#define RadiansToDegrees(x) (x * (180.0/M_PI))

static inline const Vec2f RotatePointAroundCenter(const Vec2f &point, const Vec2f &center, float degrees)
{
    // We'll just pivot for the time being
    float theta = DegreesToRadians(degrees);
    float newX = cos(theta) * (point.x-center.x) - sin(theta) * (point.y-center.y) + center.x;
    float newY = sin(theta) * (point.x-center.x) + cos(theta) * (point.y-center.y) + center.y;
    return Vec2f(newX, newY);
}


class Car {
public:
    Car() : _posTrackerA(0,0), _posTrackerB(0,0) { };
    Car(const Vec2f &initialPosition, const Vec2f &initialDirection);
    void setPositionAndDirection(const Vec2f &initialPosition, const Vec2f &initialDirection);
    void draw();
    void update(const Vec2f &posLaser);
    const Vec2f getTrackerA(){ return _posTrackerA; };
    const Vec2f getTrackerB(){ return _posTrackerB; };
    
protected:
    Vec2f normalBetweenTrackingPoints();
    Vec2f _posTrackerA;
    Vec2f _posTrackerB;
    Vec2f _v;
    Vec2f _n;
    Vec2f _center;
};

#endif /* defined(__CarSim__Car__) */
