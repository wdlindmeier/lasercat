//
//  Car.cpp
//  CarSim
//
//  Created by William Lindmeier on 10/15/12.
//
//

#include "Car.h"
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"

Car::Car(const Vec2f &initialPosition, const Vec2f &initialDirection)
{
    app::console() << "initialPosition: " << initialPosition << " initialDirection: " << initialDirection << "\n";
    setPositionAndDirection(initialPosition, initialDirection);
}

void Car::setPositionAndDirection(const Vec2f &initialPosition, const Vec2f &initialDirection)
{
    _center = initialPosition;
    
    _n = initialDirection.normalized();
    
    float theta = DegreesToRadians(-90.0f);
    float vecX = cos(theta) * _n.x - sin(theta) * _n.y;
    float vecY = sin(theta) * _n.x + cos(theta) * _n.y;
    
    _v = Vec2f(vecX, vecY);
    _v.normalize();
    
    // Set the posA and posB
    float distBetweenLights = 100.0f;
    _posTrackerA = _center + (_v * (distBetweenLights * 0.5));
    _posTrackerB = _center - (_v * (distBetweenLights * 0.5));
    
}

void Car::draw()
{
    
    gl::color(0, 1, 0);
    gl::drawSolidCircle(_posTrackerA, 10.0f);
    
    gl::color(0, 0, 1);
    gl::drawSolidCircle(_posTrackerB, 10.0f);
    
    glLineWidth(1);

    // Vec
    gl::color(0,0,0);
    gl::drawLine(_posTrackerA, _posTrackerB);

    // Norm
    gl::color(1,0,0);
    gl::drawLine(_center, Vec2f(_center + (_n * _posTrackerA.distance(_posTrackerB))));
    
}

Vec2f Car::normalBetweenTrackingPoints()
{
    Vec2f posA = _posTrackerA;
    Vec2f posB = _posTrackerB;
    
    Vec2f vecCar(posA.x - posB.x, posA.y - posB.y);
    vecCar.normalize();
    
    float theta = DegreesToRadians(90.0f);
    float normX = cos(theta) * vecCar.x - sin(theta) * vecCar.y;
    float normY = sin(theta) * vecCar.x + cos(theta) * vecCar.y;
    return Vec2f(normX, normY);
}

void Car::update(const Vec2f &posLaser)
{
    
    // Whats the vector between the _center and the green laser?
    Vec2f vecToLaser = posLaser - _center;
    Vec2f vecLaserUnit = vecToLaser.normalized();
    
    float normRads = atan2(_n.y, _n.x);
    float laserRads = atan2(vecLaserUnit.y, vecLaserUnit.x);

    float deltaRads = laserRads-normRads;
    int degrees = RadiansToDegrees(deltaRads);

    if(degrees < 0) degrees = 360 + degrees;
    
    // Simulate a max/min turning radius
    if(degrees > 180 && degrees < 350) degrees = 350;
    if(degrees < 180 && degrees > 10) degrees = 10;
    
    // ...........
    // NOTE:
    // EVERYTHING BELOW will have to be translated into car-speak
    
    // NOTE:
    // We obviously can't pivot the car, we just have turn as far as we can and move forward a minimal amount
    
    // Rotate
    
    Vec2f newA = RotatePointAroundCenter(_posTrackerA, _center, degrees);
    Vec2f newB = RotatePointAroundCenter(_posTrackerB, _center, degrees);
    _posTrackerA = newA;
    _posTrackerB = newB;
    
    // Then move forward a little
    
    float distanceToFrontOfCar = _posTrackerA.distance(_posTrackerB); // Make this real
    Vec2f newNormal = normalBetweenTrackingPoints();
    float goForwardDist = (_center.distance(posLaser) - distanceToFrontOfCar) * 0.01;
    Vec2f newCenter = _center + (newNormal * goForwardDist);
    setPositionAndDirection(newCenter, newNormal);
    
}

