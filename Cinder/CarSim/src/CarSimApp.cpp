#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "Car.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class CarSimApp : public AppBasic {
  public:
	void setup();
	void mouseDrag( MouseEvent event );
    void mouseDown( MouseEvent event );
    void keyDown( KeyEvent event );
	void update();
	void draw();
    void resetCar();
    void startTrick();
    void stopTricks();
    void doTrick0();
    void doTrick1();
    
    Car _car;
    Vec2f _posLaser;
    
    bool _isDoingTrick;
    int  _trickFrame;
    int _trickNum;

};

static const float TrickDistance = 150.0f;
static const int TrickFrames = 60;
static const int NumTricks = 2;

void CarSimApp::setup()
{
    setWindowSize(640, 480);
    
    _posLaser = Vec2f(320, 240);
    
    resetCar();
}

void CarSimApp::resetCar()
{
    console() << "creating new car\n";
    
    Vec2f randStart((arc4random() % 500) + 70.0f, (arc4random() % 340) + 70.0);
    Vec2f randVec(((int)(arc4random() % 200) - 100) * 0.01f, ((int)(arc4random() % 200) - 100) * 0.01f);
    
    _car = Car(randStart, randVec);
        
}

void CarSimApp::mouseDrag( MouseEvent event )
{
    if(!_isDoingTrick){
        _posLaser = event.getPos();
    }
}

void CarSimApp::mouseDown( MouseEvent event )
{
    if(!_isDoingTrick){
        _posLaser = event.getPos();
    }
}

void CarSimApp::keyDown( KeyEvent event )
{
    if(event.getCode() == KeyEvent::KEY_SPACE){
        resetCar();
    }else if(event.getCode() == KeyEvent::KEY_t){
        startTrick();
    }
}

void CarSimApp::startTrick()
{
    _trickFrame = 0;
    _trickNum = arc4random() % 2;
    _isDoingTrick = true;

    console() << "starting trick " << _trickNum << "\n";
}

void CarSimApp::stopTricks()
{
    _car.setLightsOn(_trickFrame % 4 < 2);
    _trickFrame = 0;
    _isDoingTrick = false;
}

void CarSimApp::update()
{
    // Tell the car to go somewhere...
    if(_isDoingTrick){
        if(_trickFrame < TrickFrames){
            switch (_trickNum) {
                case 0:
                    doTrick0();
                    break;
                case 1:
                    doTrick1();
                    break;
                default:
                    console() << "couldnt find trick " << _trickNum << "\n";
                    break;
            }
            _trickFrame++;
        }else{
            stopTricks();
        }
    }
    
    _car.update(_posLaser);
    
}

void CarSimApp::doTrick0()
{
    // Blink Lights
    _car.setLightsOn(_trickFrame % 4 < 2);

}

void CarSimApp::doTrick1()
{
    // Drive in a circle
    Vec2f center = _car.getCenter();
    float pointerX = center.x + cos(_trickFrame * ((M_PI*2)/(float)TrickFrames)) * TrickDistance;
    float pointerY = center.y + sin(_trickFrame * ((M_PI*2)/(float)TrickFrames)) * TrickDistance;
    _posLaser = Vec2f(pointerX, pointerY);
}

void CarSimApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0.9, 0.9, 0.9 ) );
    
    gl::color(1, 0, 0);
    gl::drawSolidCircle(_posLaser, 10);
    
    _car.draw();
}


CINDER_APP_BASIC( CarSimApp, RendererGl )
