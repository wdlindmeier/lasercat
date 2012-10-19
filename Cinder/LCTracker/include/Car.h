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
#include "LCDrawingHelper.hpp"

using namespace ci;

class Car {

public:
    
    Car() : _posTrackerA(0,0), _posTrackerB(0,0), _size(100.0f) { };
    Car(const Vec2f &initialPosition, const Vec2f &initialDirection);
    void setPositionAndDirection(const Vec2f &initialPosition, const Vec2f &initialDirection);
    void draw();
    void update(const Vec2f &posLaser, const float &relativeSpeed);
    const Vec2f getTrackerA(){ return _posTrackerA; };
    const Vec2f getTrackerB(){ return _posTrackerB; };
    const Vec2f getCenter(){ return _center; };
    void setSize(float newSize){ _size = newSize; };
    const float getSize(){ return _size; };
    
protected:
    
    Vec2f normalBetweenTrackingPoints();
    Vec2f _posTrackerA;
    Vec2f _posTrackerB;
    Vec2f _v;
    Vec2f _n;
    Vec2f _center;
    float _size;
};

#endif /* defined(__CarSim__Car__) */
