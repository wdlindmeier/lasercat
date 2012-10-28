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
#include "cinder/Serial.h"

using namespace ci;
using namespace std;

class Car {

public:
    
    Car() : _serial(0), _posTrackerA(0,0), _posTrackerB(0,0), _size(100.0f) { };
    Car(const Vec2f &initialPosition, const Vec2f &initialDirection, Serial *serial);
    void draw();
    void update(const Vec2f &posLaser, const float &relativeSpeed, const Vec2f &windowSize);
    const Vec2f getTrackerA(){ return _posTrackerA; };
    const Vec2f getTrackerB(){ return _posTrackerB; };
    const Vec2f getCenter(){ return _center; };
    const Vec2f getVector(){ return _v; };
    void setSize(float newSize){ _size = newSize; };
    const float getSize(){ return _size; };
    void setPositionDirectionSize(const Vec2f &currentPosition,
                                  const Vec2f &currentDirection,
                                  const float &size);
    
protected:
    
    void updateSerialPosition(const Vec2f &posLaser,
                              const float &relativeSpeed,
                              const Vec2f &windowSize);
    void updateProjectedPosition(const Vec2f &posLaser,
                                 const float &relativeSpeed,
                                 const Vec2f &windowSize);
    Vec2f normalBetweenTrackingPoints();
    Vec2f _posTrackerA;
    Vec2f _posTrackerB;
    Vec2f _v;
    Vec2f _n;
    Vec2f _center;
    float _size;
    Serial *_serial;
    Vec2f _drawVec;
};

#endif /* defined(__CarSim__Car__) */
