#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace gl;

class LCTrackerApp : public AppBasic {
    
public:
    
    LCTrackerApp();
	void setup();
	void mouseDown( MouseEvent event );
	void update();
	void draw();
private:
    
    gl::Texture _texCat;
    
};

LCTrackerApp::LCTrackerApp()
{

};

void LCTrackerApp::setup()
{
    
    _texCat = loadImage(loadResource("lc.png"));

}

void LCTrackerApp::mouseDown( MouseEvent event )
{
}

void LCTrackerApp::update()
{
    
}

void LCTrackerApp::draw()
{
	gl::clear( Color( 1, 0, 0 ) );
    gl::draw(_texCat);
}


CINDER_APP_BASIC( LCTrackerApp, RendererGl )
