//
//  Car.cpp
//  CarSim
//
//  Created by William Lindmeier on 10/15/12.
//
//

#include "Car.h"
#include "cinder/Utilities.h"
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"

Car::Car(const Vec2f &initialPosition, const Vec2f &initialDirection, Serial *serial)
{
    _serial = serial;
    app::console() << "initialPosition: " << initialPosition << " initialDirection: " << initialDirection << "\n";
    setPositionDirectionSize(initialPosition, initialDirection, 50.0f);
}

// TODO:
// NOTE:
// IMPORTANT:
// If we're using serial, we'll have to call setPositionDirectionSize from the app
void Car::updateSerialPosition(const Vec2f &posLaser,
                               const float &relativeSpeed,
                               const Vec2f &windowSize)
{
    
    // From sim
    
    Vec2f vecToLaser = posLaser - _center;
    float laserDistance = vecToLaser.length();
    Vec2f vecLaserUnit = vecToLaser.normalized();
    
    float vecRads = atan2(_v.y, _v.x);
    float laserRads = atan2(vecLaserUnit.y, vecLaserUnit.x);
    float deltaRads = laserRads-vecRads;
    
    _drawVec = RaiansToVec2f(deltaRads) * laserDistance;
    _drawVec = RotatePointAroundCenter(_drawVec, Vec2f::zero(), -90);
    
    app::console() << _drawVec << "\n";
    
    Vec2f offset = _drawVec.normalized();
    float speed = _drawVec.length();
    
    // END

    /*
    Vec2f offset = posLaser - _center;
    float offsetLength = offset.length();
    float speed = offset.length();
    
    offset.normalize();
    
    // Make the offset relative to the car's vector
    offset -= _v;
    
    _drawVec = offset * offsetLength;
    _drawVec = RotatePointAroundCenter(_drawVec, Vec2f::zero(), -90);
    */
    
    float amtLeftWheel = 0;
    float amtRightWheel = 0;
    
    // Turning scheme:
    // 0-90°
    // 0 == other wheel moves forward @ speed
    // 90 == other wheel moves backwards @ speed
    
    // Never account for moving backwards.
    // Hard left or right is all we can do.
    float yRange = (MAX((offset.y*-1), 0.0)*2.0f) - 1.0f; // -1..1
    
    // Always having one wheel moving forward ensures we're
    // driving forward. We can't drive backwards.
    if(offset.x < 0){
        amtRightWheel = 1;
        amtLeftWheel = yRange;
    }else{
        amtLeftWheel = 1;
        amtRightWheel = yRange;
    }
    
    /*
    // Making the lw / rw amount a function of the speed
    // TODO: make this a product of the car size. (e.g. _car.getSize() * 5)
    const static int MAX_SPEED = 200; // This is in px. It will slow down w/in that range
    
    float speedScalar = MIN((speed/(float)MAX_SPEED), 1.0);
    // Always give it enough speed (50%) to move if the distance is greater than 20
    if(speed > 20) speedScalar = 0.5;
    
    amtLeftWheel *= speedScalar;
    amtRightWheel *= speedScalar;
    */
    
    int lw = 255+(amtLeftWheel*255); // 0..255..500
    int rw = 255+(amtRightWheel*255); // 0..255..500
    
#define WRITE_DIRECTLY_TO_CAR   1
    
#if WRITE_DIRECTLY_TO_CAR
    
    long val = (lw*(long)1000)+rw;
    int rVal = val % 1000;
    int lVal = (val - rVal) * 0.001;
    
    /*
    int lDirection = lVal >= 255 ? 1 : -1;
    int rDirection = rVal >= 255 ? 1 : -1;
    int lAbsVal = abs(lVal-255);
    int rAbsVal = abs(rVal-255);
    
    _ardLDir = lDirection;
    _ardRDir = rDirection;
    _ardLVal = lAbsVal;
    _ardRVal = rAbsVal;
    */

    long totalVal = (lVal*(long)1000)+rVal;
    string directions = "" + boost::lexical_cast<string>((long)totalVal) + "\n";
    _serial->writeString(directions);
    
#else
    
    // We have an arduino intermediary on the Serial port
    
    //    int sp = (int)speed;
    string directions = "" + boost::lexical_cast<string>((int)lw) + "," +
    boost::lexical_cast<string>((int)rw) + ",\n";// +
    //                             boost::lexical_cast<string>((int)sp) + ",\n";
    app::console() << directions << "\n";
    _serial->writeString(directions);

#endif
    
    // Serial code
    /*
     long val = (lw*(long)1000)+rw;
     int rVal = val % 1000;
     int lVal = (val - rVal) * 0.001;
     
     int lDirection = lVal >= 255 ? 1 : -1;
     int rDirection = rVal >= 255 ? 1 : -1;
     int lAbsVal = abs(lVal-255);
     int rAbsVal = abs(rVal-255);
     
     console() << "lw : " << lw << " rw: " << rw;
     console() << " lAbsVal : " << lAbsVal << " rAbsVal: " << rAbsVal << "\n";
     
     _ardLDir = lDirection;
     _ardRDir = rDirection;
     _ardLVal = lAbsVal;
     _ardRVal = rAbsVal;
     */

    /*
    Vec2f offset = posLaser - _center; //(windowSize * 0.5);
    
    float speed = offset.length() * relativeSpeed;
    offset.normalize();
    float amtLeftWheel = 0;
    float amtRightWheel = 0;
    
    // Always having one wheel moving forward ensures we're
    // driving forward. We can't drive backwards.
    if(offset.x < 0){
        amtRightWheel = 1;
        amtLeftWheel = offset.y*-1;
    }else{
        amtLeftWheel = 1;
        amtRightWheel = offset.y*-1;
    }
    
    int lw = amtLeftWheel*255; // -255..255
    int rw = amtRightWheel*255; // -255..255
    int sp = (int)speed;
    std::string directions = "" + boost::lexical_cast<string>((int)lw) + "," +
                            boost::lexical_cast<string>((int)rw) + "," +
                            boost::lexical_cast<string>((int)sp) + ",\n";
    _serial->writeString(directions);
    */
}

void Car::updateProjectedPosition(const Vec2f &posLaser,
                                  const float &relativeSpeed,
                                  const Vec2f &windowSize)
{
    // Whats the vector between the _center and the green laser?
    Vec2f vecToLaser = posLaser - _center;
    float laserDistance = vecToLaser.length();
    Vec2f vecLaserUnit = vecToLaser.normalized();
    
    float vecRads = atan2(_v.y, _v.x);
    float laserRads = atan2(vecLaserUnit.y, vecLaserUnit.x);
    float deltaRads = laserRads-vecRads;
    
    _drawVec = RaiansToVec2f(deltaRads) * laserDistance;
    _drawVec = RotatePointAroundCenter(_drawVec, Vec2f::zero(), -90);
    
    Vec2f offset = _drawVec.normalized();
    //float yRange = (MAX((offset.y*-1), 0.0)*2.0f) - 1.0f; // -1..1
    app::console() << offset << "\n";

    
    int degrees = RadiansToDegrees(deltaRads);
    
    if(degrees < 0) degrees = 360 + degrees;
    
    // Rotate
    
    Vec2f newA = RotatePointAroundCenter(_posTrackerA, _center, degrees);
    Vec2f newB = RotatePointAroundCenter(_posTrackerB, _center, degrees);
    
    // Then move forward a little
    
    Vec2f newVec = newA - newB; //normalBetweenTrackingPoints();
    newVec.normalize();
    
    float goForwardDist = (_center.distance(posLaser) - (_size*0.5)) * relativeSpeed;
    Vec2f newCenter = _center + (newVec * goForwardDist);
    if(Area(_size,_size,windowSize.x-(_size*2),windowSize.y-(_size*2)).contains(newCenter)){
        
        // Don't let the car leave the bounds, otherwise we cant track it
        setPositionDirectionSize(newCenter, newVec, _size);
    }
}

void Car::setPositionDirectionSize(const Vec2f &currentPosition,
                                   const Vec2f &currentDirection,
                                   const float &size)
{
    _size = size;

    _center = currentPosition;
    
    _v = currentDirection.normalized();
    
    float theta = DegreesToRadians(-90.0f);
    float vecX = cos(theta) * _v.x - sin(theta) * _v.y;
    float vecY = sin(theta) * _v.x + cos(theta) * _v.y;
    
    _n = Vec2f(vecX, vecY);
    _n.normalize();
    
    // Set the posA and posB
    _posTrackerA = _center + (_v * (_size * 0.5));
    _posTrackerB = _center - (_v * (_size * 0.5));

}

void Car::draw()
{
    
    glLineWidth(5);

    gl::color(0.2,0.2,0.2);

    // Vec
    gl::drawLine(_posTrackerA, _posTrackerB);

    // Norm
    Vec2f normalVec = _n * _posTrackerA.distance(_posTrackerB);
    gl::drawLine(_center - (normalVec*0.5), _center + (normalVec*0.5));
    
    // Tracking points
    gl::color(1, 0, 0);
    gl::drawSolidCircle(_posTrackerA, 10.0f);
    
    gl::color(0, 0, 1);
    gl::drawSolidCircle(_posTrackerB, 10.0f);
    
    // Draw the vector
    gl::color(1, 1, 1);
    glLineWidth(2);
    Vec2f vecStart = Vec2f(600, 100);
    gl::drawLine(vecStart, vecStart+_drawVec);
    
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

void Car::update(const Vec2f &posLaser,
                 const float &relativeSpeed,
                 const Vec2f &windowSize)
{
    
    if(_serial){
        
        // NOTE: Make sure the app has called setPositionDirectionSize        
        updateSerialPosition(posLaser,
                             relativeSpeed,
                             windowSize);
        
    }else{
        
        updateProjectedPosition(posLaser,
                                relativeSpeed,
                                windowSize);
        
    }

}

