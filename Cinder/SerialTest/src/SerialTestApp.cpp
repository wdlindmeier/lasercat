#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Serial.h"
#include <boost/lexical_cast.hpp>

using namespace ci;
using namespace ci::app;
using namespace std;

class SerialTestApp : public AppBasic {
  public:
	void setup();
	void mouseDown( MouseEvent event );
    void mouseUp( MouseEvent event );
    void mouseDrag( MouseEvent event );
	void update();
	void draw();
    Serial serial;
    Vec2f mousePos;
    
    int _ardLDir;
    int _ardRDir;
    int _ardLVal;
    int _ardRVal;
};

void SerialTestApp::setup()
{
    // print the devices
	const vector<Serial::Device> &devices( Serial::getDevices() );
	for( vector<Serial::Device>::const_iterator deviceIt = devices.begin(); deviceIt != devices.end(); ++deviceIt ) {
		console() << "Device: " << deviceIt->getName() << endl;
	}
	
	try {
		Serial::Device dev = Serial::findDeviceByNameContains("tty.usbmodemfa141");
		serial = Serial( dev, 9600);
        serial.flush();
	}
	catch( ... ) {
		console() << "There was an error initializing the serial device!" << std::endl;
		exit( -1 );
	}

}

void SerialTestApp::mouseDown( MouseEvent event )
{
//    serial.writeString("hotdog\n");
}

void SerialTestApp::mouseDrag( MouseEvent event )
{
    mousePos = event.getPos();
    Vec2f offset = mousePos - getWindowCenter();

    float speed = offset.length();
    offset.normalize();
    float amtLeftWheel = 0;
    float amtRightWheel = 0;
    
    // Turning scheme:
    // 0-90°
    // 0 == other wheel moves forward @ speed
    // 90 == other wheel moves backwards @ speed
    
    // Never account for moving backwards.
    // Hard left or right is all we can do.
    float yRange = (MAX((offset.y*-1), 0.0)*2.0f) - 1.0f; // -1..1
    //yRange*=-1;
    
    // Always having one wheel moving forward ensures we're
    // driving forward. We can't drive backwards.
    if(offset.x < 0){
        amtRightWheel = 1;
        amtLeftWheel = yRange;// -1..1 //offset.y*-1;
    }else{
        amtLeftWheel = 1;
        amtRightWheel = yRange;// -1..1 //offset.y*-1;
    }
    
    // Making the lw / rw amount a function of the speed
    const static int MAX_SPEED = 200;
    float speedScalar = MIN((speed/(float)MAX_SPEED), 1.0);
    amtLeftWheel *= speedScalar;
    amtRightWheel *= speedScalar;
    
    int lw = 255+(amtLeftWheel*255); // 0..255..500
    int rw = 255+(amtRightWheel*255); // 0..255..500
//    int sp = (int)speed;
    string directions = "" + boost::lexical_cast<string>((int)lw) + "," +
                             boost::lexical_cast<string>((int)rw) + ",\n";// +
//                             boost::lexical_cast<string>((int)sp) + ",\n";
    console() << directions << "\n";
    serial.writeString(directions);
    
    // SIM Arduino code
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

}

void SerialTestApp::mouseUp( MouseEvent event )
{
//    serial.writeString("hamburger\n");
}

void SerialTestApp::update()
{
    
}

void SerialTestApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
    gl::color(1, 1, 1);
    gl::drawSolidCircle(getWindowCenter(), 10);
    gl::color(1, 0, 0);
    glLineWidth(5);
    gl::drawLine(getWindowCenter(), mousePos);
    
    glLineWidth(2.0);

    // Draw the wheels
    // L
    if(_ardLDir < 0){
        // Backward
        gl::color(Color8u(_ardLVal, 0, 0));
        gl::drawSolidCircle(Vec2f(540,440), 10);
    }else{
        // Forward
        gl::color(Color8u(_ardLVal, 0, 0));
        gl::drawSolidCircle(Vec2f(540,400), 10);
    }
    gl::color(255, 255, 255);
    gl::drawStrokedCircle(Vec2f(540,400), 10);
    gl::color(255, 255, 255);
    gl::drawStrokedCircle(Vec2f(540,440), 10);

    // R
    if(_ardRDir < 0){
        gl::color(Color8u(_ardRVal, 0, 0));
        gl::drawSolidCircle(Vec2f(580,440), 10);
    }else{
        // Forward
        gl::color(Color8u(_ardRVal, 0, 0));
        gl::drawSolidCircle(Vec2f(580,400), 10);
    }
    // Bakward
    gl::color(255, 255, 255);
    gl::drawStrokedCircle(Vec2f(580,400), 10);
    gl::color(255, 255, 255);
    gl::drawStrokedCircle(Vec2f(580,440), 10);

}


CINDER_APP_BASIC( SerialTestApp, RendererGl )
