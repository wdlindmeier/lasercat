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
#include "Kinect.h"
// LC
#include "LCDrawingHelper.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace gl;
using namespace mowa::sgui;

#define USE_KINECT  0

class LCTrackerApp : public AppBasic {
    
public:
    
    LCTrackerApp();
	void setup();
	void keyDown( KeyEvent event );
	void update();
	void draw();
    void updateLabels();
    Vec3f getLargestContour(vector< vector<cv::Point> > *contours);

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
    
    // Color Ranges
    int MIN_R_TOP;
    int MAX_R_TOP;
    int MIN_R_BOTTOM;
    int MAX_R_BOTTOM;
    int MIN_G;
    int MAX_G;
    int MIN_B;
    int MAX_B;
    int MIN_VAL;
    int MIN_SAT;
    
    LabelControl *_labelRangeRed;
    LabelControl *_labelRangeBlue;
    LabelControl *_labelRangeGreen;
    LabelControl *_labelValSat;
    LabelControl *_labelFPS;
    
};

LCTrackerApp::LCTrackerApp() :
_blobR(0,0,0),
_blobG(0,0,0),
_blobB(0,0,0),
_labelRangeRed(0),
_labelRangeBlue(0),
_labelRangeGreen(0),
_labelValSat(0),
_labelFPS(0),
_showTex(0)
{

};

void LCTrackerApp::setup()
{
    setWindowSize(640, 480);
    _surfTracking = Surface8u(640, 480, false);
    _texTrack = gl::Texture(_surfTracking);

    
#if USE_KINECT
    console() << "There are " << Kinect::getNumDevices() << " Kinects connected." << std::endl;
	_kinect = Kinect( Kinect::Device() ); // the default Device implies the first Kinect connected
#else
    _capture = Capture( 640, 480 );
	_capture.start();
#endif
    
    // HSV hue mapped to 0-255 (not 0-360)
    MIN_R_TOP = 242;
    MAX_R_TOP = 255;
    MIN_R_BOTTOM = 0;
    MAX_R_BOTTOM = 18;
    MIN_G = 58;
    MAX_G = 102;
    MIN_B = 124;
    MAX_B = 179;
    MIN_VAL = 142;
    MIN_SAT = 140;

    // GUI
    gui = new SimpleGUI(this);
    gui->lightColor = ColorA(1, 1, 0, 1);
	gui->addLabel("Color Ranges");

    gui->addParam("MIN_R_TOP", &MIN_R_TOP, 0, 255, MIN_R_TOP);
    gui->addParam("MAX_R_TOP", &MAX_R_TOP, 0, 255, MAX_R_TOP);
    gui->addParam("MIN_R_BOTTOM", &MIN_R_BOTTOM, 0, 255, MIN_R_BOTTOM);
    gui->addParam("MAX_R_BOTTOM", &MAX_R_BOTTOM, 0, 255, MAX_R_BOTTOM);
    
    gui->addParam("MIN_G", &MIN_G, 0, 255, MIN_G);
    gui->addParam("MAX_G", &MAX_G, 0, 255, MAX_G);
    
    gui->addParam("MIN_B", &MIN_B, 0, 255, MIN_B);
    gui->addParam("MAX_B", &MAX_B, 0, 255, MAX_B);
    
    gui->addParam("MIN_VAL", &MIN_VAL, 0, 255, MIN_VAL);
    gui->addParam("MIN_SAT", &MIN_SAT, 0, 255, MIN_SAT);

    _labelRangeRed = gui->addLabel("Red Range");
    _labelRangeGreen = gui->addLabel("Green Range");
    _labelRangeBlue = gui->addLabel("Blue Range");
    _labelValSat = gui->addLabel("Val Sat");
    _labelFPS = gui->addLabel("FPS");
    
    updateLabels();
    
}

void LCTrackerApp::updateLabels()
{
    if(_labelValSat){
        string valsat = "Val: ";
        valsat  += boost::lexical_cast<string>(MIN_VAL);
        valsat  += " Sat: ";
        valsat  += boost::lexical_cast<string>(MIN_SAT);
        _labelValSat->setText(valsat);
    }

    if(_labelRangeRed){
        string redRange = string("Red Range: ")+ boost::lexical_cast<string>(MIN_R_TOP) + string(" - ") + boost::lexical_cast<string>(MAX_R_BOTTOM);
        _labelRangeRed->setText(redRange);
    }
    
    if(_labelRangeBlue){
        string blueRange = string("Blue Range: ")+ boost::lexical_cast<string>(MIN_B) + string(" - ") + boost::lexical_cast<string>(MAX_B);
        _labelRangeBlue->setText(blueRange);
    }

    if(_labelRangeGreen){
        string greenRange = string("Green Range: ")+ boost::lexical_cast<string>(MIN_G) + string(" - ") + boost::lexical_cast<string>(MAX_G);
        _labelRangeGreen->setText(greenRange);
    }
    
    if(_labelFPS){
        float fps = getAverageFps();
        string fpsString = "FPS: ";
        fpsString += boost::lexical_cast<string>((int)fps);
        _labelFPS->setText(fpsString);
    }
    
}

void LCTrackerApp::keyDown( KeyEvent event )
{
    _showTex = event.getChar();
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

                // Check if it's bright enough and saturated enough to be a tracking pixel
                if(hsv.val >= MIN_VAL && hsv.sat >= MIN_SAT){
                    
                    // Check Red (top)
                    if(hsv.hue >= MIN_R_TOP && hsv.hue <= MAX_R_TOP){

                        chR.setValue(px, 255);

                    // Check Red (bottom)
                    }else if(hsv.hue >= MIN_R_BOTTOM && hsv.hue <= MAX_R_BOTTOM){

                        chR.setValue(px, 255);

                    // Check Green
                    }else if(hsv.hue >= MIN_G && hsv.hue <= MAX_G){

                        chG.setValue(px, 255);
                        
                    // Check Blue
                    }else if(hsv.hue >= MIN_B && hsv.hue <= MAX_B){

                        chB.setValue(px, 255);
                    }
                    
                    
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
    
    // Draw circles around the blobs
    gl::color(1.0, 0.0, 0.0);
    Vec2f posRed = Vec2f(_blobR.x, _blobR.y);
    gl::drawStrokedCircle(posRed, _blobR.z);
    
    gl::color(0.0, 1.0, 0.0);
    Vec2f posGreen = Vec2f(_blobG.x, _blobG.y);
    gl::drawStrokedCircle(Vec2f(posGreen.x, posGreen.y), _blobG.z);

    gl::color(0.0, 0.0, 1.0);
    Vec2f posBlue = Vec2f(_blobB.x, _blobB.y);
    gl::drawStrokedCircle(Vec2f(posBlue.x, posBlue.y), _blobB.z);
    
    
    // Drawing the car direction (a cross)
    gl::color(1.0f, 1.0f, 1.0f);
    
    // Draw the vector
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


CINDER_APP_BASIC( LCTrackerApp, RendererGl )
