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
    Car _car;
    Vec2f _posLaser;
};

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
    _posLaser = event.getPos();
}

void CarSimApp::mouseDown( MouseEvent event )
{
    _posLaser = event.getPos();
}

void CarSimApp::keyDown( KeyEvent event )
{
    if(event.getCode() == KeyEvent::KEY_SPACE){
        resetCar();
    }
}

void CarSimApp::update()
{
    // Tell the car to go somewhere...
    
    _car.update(_posLaser);

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
