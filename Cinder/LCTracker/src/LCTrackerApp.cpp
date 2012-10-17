#define USE_KINECT  0

#include "cinder/app/AppBasic.h"
#include "cinder/app/KeyEvent.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"
#include "cinder/Channel.h"
#include "cinder/Surface.h"
#include "cinder/Area.h"
#include "cinder/Capture.h"
// Boost
#include <boost/lexical_cast.hpp>
// Block
#include "CinderOpenCv.h"
#include "SimpleGUI.h"

#if USE_KINECT
#include "Kinect.h"
#endif 

// LC
#include "LCDrawingHelper.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace gl;
using namespace mowa::sgui;

typedef enum LaserModes {
    LaserModeGreen,
    LaserModeRed,
    LaserModeBlue
} LaserMode;

typedef struct {
    int hueMin;
    int hueMax;
    int valMin;
    int valMax;
    int satMin;
    int satMax;
} ColorConstraint;

static inline void LogColorConstraint(ColorConstraint c)
{
    console() << "minHue: " << c.hueMin << " maxHue: " << c.hueMax << " minSat: " << c.satMin << " maxSat: " << c.satMax << " minVal: " << c.valMin << " maxVal: " << c.valMax << "\n";
}

static inline ColorConstraint ColorConstraintMake(int minHue, int maxHue, int minSat, int maxSat, int minVal, int maxVal)
{
    ColorConstraint c;
    c.hueMin = minHue;
    c.hueMax = maxHue;
    c.satMin = minSat;
    c.satMax = maxSat;
    c.valMin = minVal;
    c.valMax = maxVal;
    return c;
}


class LCTrackerApp : public AppBasic {
    
public:
    
    LCTrackerApp();
	void setup();
    void shutdown();
	void keyDown( KeyEvent event );
	void update();
	void draw();
    void updateLabels();
    Vec3f getLargestContour(vector< vector<cv::Point> > *contours);
    void outputColors();

private:
    
    gl::Texture _texTrack;
    Surface8u   _surfTracking;
    
    Vec3f       _blobR;
    Vec3f       _blobG;
    Vec3f       _blobB;

#if USE_KINECT
    Kinect      _kinect;
#else
    Capture     _capture;
#endif

    char        _showTex;
    SimpleGUI   *gui;
    
    LaserMode   _laserMode;
    
//    ColorConstraint _ccRedTop;
//    ColorConstraint _ccRedBottom;
    ColorConstraint _ccRed;
    ColorConstraint _ccBlue;
    ColorConstraint _ccGreen;

    LabelControl *_labelFPS;
    
};

LCTrackerApp::LCTrackerApp() :
_blobR(0,0,0),
_blobG(0,0,0),
_blobB(0,0,0),
_labelFPS(0),
_showTex(0)
{

};

void LCTrackerApp::shutdown()
{
    outputColors();
}

void LCTrackerApp::setup()
{
    setWindowSize(640, 480);
    _surfTracking = Surface8u(640, 480, false);
    _texTrack = gl::Texture(_surfTracking);

    _laserMode = LaserModeGreen;
    
#if USE_KINECT
    console() << "There are " << Kinect::getNumDevices() << " Kinects connected." << std::endl;
	_kinect = Kinect( Kinect::Device() ); // the default Device implies the first Kinect connected
#else
    
    // list out the devices
    Capture::DeviceRef useDevice;
    bool useDefaultDevice = true;
	std::vector<Capture::DeviceRef> devices( Capture::getDevices() );
	for( std::vector<Capture::DeviceRef>::const_iterator deviceIt = devices.begin(); deviceIt != devices.end(); ++deviceIt ) {
        Capture::DeviceRef device = *deviceIt;
        console() << "Found Device " << device->getName() << " ID: " << device->getUniqueId() << std::endl;
        
        if( device->checkAvailable() ) {
            useDevice = device;
            useDefaultDevice = false;
        }
	}
    if(useDefaultDevice){
        _capture = Capture( 640, 480 );
    }else{
        _capture = Capture( 640, 480, useDevice );
    }
	_capture.start();
#endif
    
    // HSV hue mapped to 0-255 (not 0-360)

    // TODO: Update w/ Orange / Green / Violet
//    _ccRedTop = ColorConstraintMake(238, 255, 108, 255, 201, 255);
//    _ccRedBottom = ColorConstraintMake(0, 24, 110, 255, 285, 255);
    _ccRed = ColorConstraintMake(0, 24, 108, 255, 201, 255);
    _ccGreen = ColorConstraintMake(106, 130, 0, 255, 222, 255);
    _ccBlue = ColorConstraintMake(89, 150, 167, 255, 179, 255);

    // GUI
    gui = new SimpleGUI(this);
    gui->lightColor = ColorA(1, 1, 0, 1);
	gui->addLabel("Color Ranges");

    // Red
    /*
    gui->addParam("Red Top Min Hue", &_ccRedTop.hueMin, 0, 255, _ccRedTop.hueMin);
    gui->addParam("Red Top Max Hue", &_ccRedTop.hueMax, 0, 255, _ccRedTop.hueMax);
    gui->addParam("Red Top Min Sat", &_ccRedTop.satMin, 0, 255, _ccRedTop.satMin);
    gui->addParam("Red Top Max Sat", &_ccRedTop.satMax, 0, 255, _ccRedTop.satMax);
    gui->addParam("Red Top Min Val", &_ccRedTop.valMin, 0, 255, _ccRedTop.valMin);
    gui->addParam("Red Top Max Val", &_ccRedTop.valMax, 0, 255, _ccRedTop.valMax);
    
    gui->addParam("Red Bottom Min Hue", &_ccRedBottom.hueMin, 0, 255, _ccRedBottom.hueMin);
    gui->addParam("Red Bottom Max Hue", &_ccRedBottom.hueMax, 0, 255, _ccRedBottom.hueMax);
    gui->addParam("Red Bottom Min Sat", &_ccRedBottom.satMin, 0, 255, _ccRedBottom.satMin);
    gui->addParam("Red Bottom Max Sat", &_ccRedBottom.satMax, 0, 255, _ccRedBottom.satMax);
    gui->addParam("Red Bottom Min Val", &_ccRedBottom.valMin, 0, 255, _ccRedBottom.valMin);
    gui->addParam("Red Bottom Max Val", &_ccRedBottom.valMax, 0, 255, _ccRedBottom.valMax);
    */
    
    gui->addParam("Red Min Hue", &_ccRed.hueMin, 0, 255, _ccRed.hueMin);
    gui->addParam("Red Max Hue", &_ccRed.hueMax, 0, 255, _ccRed.hueMax);
    gui->addParam("Red Min Sat", &_ccRed.satMin, 0, 255, _ccRed.satMin);
    gui->addParam("Red Max Sat", &_ccRed.satMax, 0, 255, _ccRed.satMax);
    gui->addParam("Red Min Val", &_ccRed.valMin, 0, 255, _ccRed.valMin);
    gui->addParam("Red Max Val", &_ccRed.valMax, 0, 255, _ccRed.valMax);

    gui->addParam("Green Min Hue", &_ccGreen.hueMin, 0, 255, _ccGreen.hueMin);
    gui->addParam("Green Max Hue", &_ccGreen.hueMax, 0, 255, _ccGreen.hueMax);
    gui->addParam("Green Min Sat", &_ccGreen.satMin, 0, 255, _ccGreen.satMin);
    gui->addParam("Green Max Sat", &_ccGreen.satMax, 0, 255, _ccGreen.satMax);
    gui->addParam("Green Min Val", &_ccGreen.valMin, 0, 255, _ccGreen.valMin);
    gui->addParam("Green Max Val", &_ccGreen.valMax, 0, 255, _ccGreen.valMax);

    gui->addParam("Blue Min Hue", &_ccBlue.hueMin, 0, 255, _ccBlue.hueMin);
    gui->addParam("Blue Max Hue", &_ccBlue.hueMax, 0, 255, _ccBlue.hueMax);
    gui->addParam("Blue Min Sat", &_ccBlue.satMin, 0, 255, _ccBlue.satMin);
    gui->addParam("Blue Max Sat", &_ccBlue.satMax, 0, 255, _ccBlue.satMax);
    gui->addParam("Blue Min Val", &_ccBlue.valMin, 0, 255, _ccBlue.valMin);
    gui->addParam("Blue Max Val", &_ccBlue.valMax, 0, 255, _ccBlue.valMax);

    _labelFPS = gui->addLabel("FPS");
    
    updateLabels();
    
}

void LCTrackerApp::outputColors()
{
/*
    console() << "ccRedTop: ";
    LogColorConstraint(_ccRedTop);
    
    console() << "ccRedBottom: ";
    LogColorConstraint(_ccRedBottom);
*/

    console() << "ccRed: ";
    LogColorConstraint(_ccRed);

    console() << "ccGreen: ";
    LogColorConstraint(_ccGreen);
    
    console() << "ccBlue: ";
    LogColorConstraint(_ccBlue);
}

void LCTrackerApp::updateLabels()
{

    if(_labelFPS){
        float fps = getAverageFps();
        string fpsString = "FPS: ";
        fpsString += boost::lexical_cast<string>((int)fps);
        _labelFPS->setText(fpsString);
    }
    
}

void LCTrackerApp::keyDown( KeyEvent event )
{
    char c = event.getChar();

    if(c == KeyEvent::KEY_c){
        
        outputColors();
        
    }else if(c == KeyEvent::KEY_r || c == KeyEvent::KEY_g || c == KeyEvent::KEY_b || c == KeyEvent::KEY_SPACE){
        
        _showTex = event.getChar();
        
    }
}

void LCTrackerApp::update()
{
    
#if USE_KINECT
    if( _kinect.checkNewVideoFrame() ){
		_surfTracking = _kinect.getVideoImage();
#else
    if( _capture.checkNewFrame() ) {
        _surfTracking = _capture.getSurface();
#endif

        // Find the contours
        Vec2i imgSize = _surfTracking.getSize();

        Channel8u chR(imgSize.x, imgSize.y), chG(imgSize.x, imgSize.y), chB(imgSize.x, imgSize.y);

        Area area( 0, 0, imgSize.x, imgSize.y );
        Surface::Iter iter = _surfTracking.getIter( area );
        int x=0;
        int y=0;
        while( iter.line() ) {
            while( iter.pixel() ) {
                Vec2i px(x,y);
                
                chR.setValue(px, 0);
                chG.setValue(px, 0);
                chB.setValue(px, 0);
                
                // Check for blobs yo
                ColorRGB rgb;
                rgb.r = iter.r();
                rgb.g = iter.g();
                rgb.b = iter.b();
                
                ColorHSV hsv = RGBtoHSV(rgb);

                /*
                // Check Red (top)
                if(hsv.hue >= _ccRedTop.hueMin && hsv.hue <= _ccRedTop.hueMax &&
                   hsv.val >= _ccRedTop.valMin && hsv.val <= _ccRedTop.valMax &&
                   hsv.sat >= _ccRedTop.satMin && hsv.sat <= _ccRedTop.satMax){

                    chR.setValue(px, 255);

                // Check Red (bottom)
                }else if(hsv.hue >= _ccRedBottom.hueMin && hsv.hue <= _ccRedBottom.hueMax &&
                         hsv.val >= _ccRedBottom.valMin && hsv.val <= _ccRedBottom.valMax &&
                         hsv.sat >= _ccRedBottom.satMin && hsv.sat <= _ccRedBottom.satMax){


                    chR.setValue(px, 255);
                */
                
                // Check Blue
                if(hsv.hue >= _ccBlue.hueMin && hsv.hue <= _ccBlue.hueMax &&
                     hsv.val >= _ccBlue.valMin && hsv.val <= _ccBlue.valMax &&
                     hsv.sat >= _ccBlue.satMin && hsv.sat <= _ccBlue.satMax){
                
                    chB.setValue(px, 255);
                    
                // Check Green
                }else if(hsv.hue >= _ccGreen.hueMin && hsv.hue <= _ccGreen.hueMax &&
                     hsv.val >= _ccGreen.valMin && hsv.val <= _ccGreen.valMax &&
                     hsv.sat >= _ccGreen.satMin && hsv.sat <= _ccGreen.satMax){
                
                
                    chG.setValue(px, 255);
                
                // Check Red
                }else if(hsv.hue >= _ccRed.hueMin && hsv.hue <= _ccRed.hueMax &&
                   hsv.val >= _ccRed.valMin && hsv.val <= _ccRed.valMax &&
                   hsv.sat >= _ccRed.satMin && hsv.sat <= _ccRed.satMax){
                    
                    chR.setValue(px, 255);
                }

                
                x++;
            }
            x=0;
            y++;
        }

        cv::Mat threshR(toOcv(chR)), threshG(toOcv(chG)), threshB(toOcv(chB));
        vector<vector<cv::Point> > contoursR, contoursG, contoursB;
        cv::findContours(threshR, contoursR, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
        cv::findContours(threshG, contoursG, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
        cv::findContours(threshB, contoursB, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
        
        _blobR = getLargestContour(&contoursR);
        _blobB = getLargestContour(&contoursB);
        _blobG = getLargestContour(&contoursG);

        // Choose the texture to show based on keyboard input.
        if(_showTex == KeyEvent::KEY_r){
            _texTrack = gl::Texture(chR);
        }else if(_showTex == KeyEvent::KEY_g){
            _texTrack = gl::Texture(chG);
        }else if(_showTex == KeyEvent::KEY_b){
            _texTrack = gl::Texture(chB);
        }else{
            _texTrack = gl::Texture(_surfTracking);
        }

    }

}

Vec3f LCTrackerApp::getLargestContour(vector< vector<cv::Point> > *contours)
{

    static const float BlobSizeMin = 2.0f;
    static const float BlobSizeMax = 640.0f;
   
    Vec3f largestContour = Vec3f::zero();
    
    for (vector<vector<cv::Point> >::iterator it=contours->begin() ; it < contours->end(); it++ ){
        
        // center abd radius for current blob
        cv::Point2f center;
        float radius;
        
        // convert the cuntour point to a matrix
        vector<cv::Point> pts = *it;
        cv::Mat pointsMatrix = cv::Mat(pts);
        cv::minEnclosingCircle(pointsMatrix, center, radius);

        if (radius > BlobSizeMin && radius < BlobSizeMax) {
            
            if(radius > largestContour.z){
                largestContour = Vec3f(center.x, center.y, radius);
            }

        }
        
    }
    
    return largestContour;

}

void LCTrackerApp::draw()
{
	gl::clear( Color( 0.25, 0.25, 0.25 ) );
    
    gl::color(1.0, 1.0, 1.0);
    gl::draw(_texTrack);
    
    updateLabels();
    gui->draw();
	gl::disableDepthRead();
	gl::disableDepthWrite();
    
    glLineWidth(2.0f);
    
    bool drawVec = true;
    Vec2f posRed, posGreen, posBlue;
    
    // Draw circles around the blobs
    
    if(_blobR != Vec3f::zero()){
        gl::color(1.0, 0.0, 0.0);
        posRed = Vec2f(_blobR.x, _blobR.y);
        gl::drawStrokedCircle(posRed, _blobR.z);
    }else{ drawVec = false; }
    
    if(_blobG != Vec3f::zero()){
        gl::color(0.0, 1.0, 0.0);
        posGreen = Vec2f(_blobG.x, _blobG.y);
        gl::drawStrokedCircle(Vec2f(posGreen.x, posGreen.y), _blobG.z);
    }

    if(_blobB != Vec3f::zero()){
        gl::color(0.0, 0.0, 1.0);
        posBlue = Vec2f(_blobB.x, _blobB.y);
        gl::drawStrokedCircle(Vec2f(posBlue.x, posBlue.y), _blobB.z);
    }else{ drawVec = false; }
    
    
    // Drawing the car direction (a cross)
    gl::color(1.0f, 1.0f, 1.0f);
    
    // Draw the vector
    if(drawVec){
        gl::drawLine(posRed, posBlue);
        
        Vec2f vecCar(posRed.x - posBlue.x, posRed.y - posBlue.y);
        float vecCarLength = vecCar.length();
        vecCar.normalize();
        
        float theta = DegreesToRadians(90.0f);
        float normX = cos(theta) * vecCar.x - sin(theta) * vecCar.y;
        float normY = sin(theta) * vecCar.x + cos(theta) * vecCar.y;
        Vec2f normCar(normX, normY);

        normCar *= vecCarLength;
        Vec2f halfVecCar = vecCar * (vecCarLength * 0.5);
        
        Vec2f normStart(posBlue.x + halfVecCar.x, posBlue.y + halfVecCar.y);
        normStart -= (normCar * 0.5);
        
        // Draw the normal
        gl::drawLine(normStart, normStart+normCar);
    }
}


CINDER_APP_BASIC( LCTrackerApp, RendererGl )
