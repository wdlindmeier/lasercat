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
    serial.writeString("hotdog\n");
}

void SerialTestApp::mouseDrag( MouseEvent event )
{
    mousePos = event.getPos();
    Vec2f offset = mousePos - getWindowCenter();

    float speed = offset.length();
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
    string directions = "" + boost::lexical_cast<string>((int)lw) + "," +
                             boost::lexical_cast<string>((int)rw) + "," +
                             boost::lexical_cast<string>((int)sp) + ",\n";
    serial.writeString(directions);
}

void SerialTestApp::mouseUp( MouseEvent event )
{
    serial.writeString("hamburger\n");
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
}


CINDER_APP_BASIC( SerialTestApp, RendererGl )
