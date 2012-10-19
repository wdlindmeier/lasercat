#include <fstream>

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

// LC
#include "Car.h"

#define USE_SIM_CAR 1

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace gl;
using namespace mowa::sgui;

#pragma mark - Data types

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

#pragma mark - Global Functions

static inline void LogColorConstraint(ColorConstraint c)
{
    console() << "minHue: " << c.hueMin << " maxHue: " << c.hueMax << " minSat: " << c.satMin << " maxSat: " << c.satMax << " minVal: " << c.valMin << " maxVal: " << c.valMax << "\n";
}

static string SerializedColorConstraints(ColorConstraint c)
{
    // This seems retarded
    string cc = boost::lexical_cast<string>((int)c.hueMin) + "," + boost::lexical_cast<string>((int)c.hueMax) + "," + boost::lexical_cast<string>((int)c.satMin) + "," + boost::lexical_cast<string>((int)c.satMax) + "," + boost::lexical_cast<string>((int)c.valMin) + "," + boost::lexical_cast<string>((int)c.valMax);
    return cc;
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

#pragma mark - LCTrackerApp

class LCTrackerApp : public AppBasic {
    
public:
    
    LCTrackerApp();
	void setup();
    void shutdown();
	void keyDown( KeyEvent event );
    void mouseDown( MouseEvent event);
    void mouseUp( MouseEvent event);
    void mouseDrag( MouseEvent event);
	void update();
	void draw();
    void updateLabels();
    Vec3f getLargestContour(vector< vector<cv::Point> > *contours);
    void outputColors();
    void saveColorSettings();
    void readColorSettings();
#if USE_SIM_CAR
    void resetCar();
#endif

private:
    
    gl::Texture _texTrack;
    Surface8u   _surfTracking;
    
    Vec3f       _blobR;
    Vec3f       _blobG;
    Vec3f       _blobB;

    Capture     _capture;

    char        _showTex;
    SimpleGUI   *gui;
    bool        _isShowingGUI;
    LaserMode   _laserMode;
    string      _colorPath;
    bool        _isMouseDown;
    Vec2f       _posMouse;
    
    ColorConstraint _ccRed;
    ColorConstraint _ccBlue;
    ColorConstraint _ccGreen;

    LabelControl *_labelFPS;
    
    Car _car;
    
};

LCTrackerApp::LCTrackerApp() :
_blobR(0,0,0),
_blobG(0,0,0),
_blobB(0,0,0),
_labelFPS(0),
_showTex(0),
_isMouseDown(0)
{

};

void LCTrackerApp::shutdown()
{
    saveColorSettings();
    outputColors();
}

void LCTrackerApp::setup()
{
    _colorPath = ci::getDocumentsDirectory().string() + "lasercat_colors.csv";
    
    setWindowSize(640, 480);
    _surfTracking = Surface8u(640, 480, false);
    _texTrack = gl::Texture(_surfTracking);
    _isShowingGUI = false;
    _laserMode = LaserModeGreen;
    
    // HSV hue mapped to 0-255 (not 0-360)
    readColorSettings();

    // GUI
    gui = new SimpleGUI(this);
    gui->lightColor = ColorA(1, 1, 0, 1);
	gui->addLabel("Color Ranges");

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
    
#if USE_SIM_CAR
    
    resetCar();
#else 
    
    _car = Car(Vec2f::zero(), Vec2f::zero());
    
#endif
    
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
    
}

#if USE_SIM_CAR

void LCTrackerApp::resetCar()
{
    
    console() << "creating new car\n";
    _car = Car(getWindowSize() * 0.5, Vec2f(0.0,1.0));
    
}

#endif 

void LCTrackerApp::saveColorSettings()
{
    // Get an ofstream which is what you'll use to write to your file.
    std::ofstream oStream( _colorPath.c_str() );
    
    // write the string.
    oStream << SerializedColorConstraints(_ccRed) << "\n";   // R
    oStream << SerializedColorConstraints(_ccGreen) << "\n"; // G
    oStream << SerializedColorConstraints(_ccBlue) << "\n";  // B
    
    oStream.close();
}

void LCTrackerApp::readColorSettings()
{

    fs::path testPath( _colorPath );
    if( fs::exists( testPath ) )
    {
        string myString = loadString( loadFile( _colorPath ) );
        vector<string> rgb = split(myString,"\n");

        for(int i=0;i<rgb.size();i++){
            vector<string> colorVals = split(rgb[i],",");
            
            if(colorVals.size() >= 6){
                int hueMin= boost::lexical_cast<int>(colorVals[0]);
                int hueMax= boost::lexical_cast<int>(colorVals[1]);
                int satMin= boost::lexical_cast<int>(colorVals[2]);
                int satMax= boost::lexical_cast<int>(colorVals[3]);
                int valMin= boost::lexical_cast<int>(colorVals[4]);
                int valMax= boost::lexical_cast<int>(colorVals[5]);
                ColorConstraint c = ColorConstraintMake(hueMin, hueMax, satMin, satMax, valMin, valMax);
                switch(i){
                    case 0: // R
                        _ccRed = c;
                        LogColorConstraint(_ccRed);
                        break;
                    case 1: // G
                        _ccGreen = c;
                        LogColorConstraint(_ccGreen);
                        break;
                    case 2: // B
                        _ccBlue = c;
                        LogColorConstraint(_ccBlue);
                        break;
                }
            }
        }
        
    }else{
        // TODO: Use defaults
        console() << "No file exists at " << _colorPath << ". Using default colors. \n";
        _ccRed = ColorConstraintMake(0, 24, 108, 255, 201, 255);
        _ccGreen = ColorConstraintMake(106, 130, 0, 255, 222, 255);
        _ccBlue = ColorConstraintMake(89, 150, 167, 255, 179, 255);
    }

}

void LCTrackerApp::outputColors()
{    
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
    
    }else if(c == KeyEvent::KEY_ESCAPE){
        
        _isShowingGUI = !_isShowingGUI;
        
    }else if(c == KeyEvent::KEY_RETURN){
    
        resetCar();
        
    }else if(c == KeyEvent::KEY_r || c == KeyEvent::KEY_g || c == KeyEvent::KEY_b || c == KeyEvent::KEY_SPACE){
        
        _showTex = event.getChar();
        
    }
}

void LCTrackerApp::mouseDown( MouseEvent event)
{
    _isMouseDown = true;
    _posMouse = event.getPos();
}

void LCTrackerApp::mouseUp( MouseEvent event)
{
    _isMouseDown = false;
    _posMouse = event.getPos();
}

void LCTrackerApp::mouseDrag( MouseEvent event)
{
    _posMouse = event.getPos();
}

void LCTrackerApp::update()
{
    
    if( _capture.checkNewFrame() ) {
        _surfTracking = _capture.getSurface();

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

#define USE_CV_BLUR 0
        // This doesn't seem to have a framerate hit
#if USE_CV_BLUR
        cv::Mat blurR, thresholdedR, blurG, thresholdedG, blurB, thresholdedB;
        int blurAmt = 5;
        cv::blur(threshR, blurR, cv::Size(blurAmt,blurAmt));
        cv::blur(threshG, blurG, cv::Size(blurAmt,blurAmt));
        cv::blur(threshB, blurB, cv::Size(blurAmt,blurAmt));
        cv::threshold( blurR, thresholdedR, 127, 255, CV_THRESH_BINARY);
        cv::threshold( blurG, thresholdedG, 127, 255, CV_THRESH_BINARY);
        cv::threshold( blurB, thresholdedB, 127, 255, CV_THRESH_BINARY);
        cv::findContours(thresholdedR, contoursR, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
        cv::findContours(thresholdedG, contoursG, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
        cv::findContours(thresholdedB, contoursB, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
#else
        cv::findContours(threshR, contoursR, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
        cv::findContours(threshG, contoursG, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
        cv::findContours(threshB, contoursB, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
#endif

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
    
    Vec2f posLaser;
    if(_isMouseDown){
        
        posLaser = _posMouse;
        
    }else{
        switch(_laserMode){
            case LaserModeRed:
                posLaser = _blobR.xy();
                break;
            case LaserModeGreen:
                posLaser = _blobG.xy();
                break;
            case LaserModeBlue:
                posLaser = _blobB.xy();
                break;
        }
    }
    
    if(posLaser != Vec2f::zero()){

        float speed = 1 * (1.0/getAverageFps());
        _car.update(posLaser, speed);
        
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
    
    if(_isShowingGUI){
        updateLabels();
        gui->draw();
        gl::disableDepthRead();
        gl::disableDepthWrite();
    }
    
    glLineWidth(2.0f);
    
    Vec2f posRed, posGreen, posBlue;
    
    // Draw laser blob
    if(_blobG != Vec3f::zero()){
        gl::color(0.0, 1.0, 0.0);
        posGreen = Vec2f(_blobG.x, _blobG.y);
        gl::drawStrokedCircle(Vec2f(posGreen.x, posGreen.y), _blobG.z);
    }
    
#if !USE_SIM_CAR
    
    // Draw circles around the car blobs
    
    if(_blobR != Vec3f::zero()){
        gl::color(1.0, 0.0, 0.0);
        posRed = Vec2f(_blobR.x, _blobR.y);
        gl::drawStrokedCircle(posRed, _blobR.z);
    }else{ drawVec = false; }
    
    if(_blobB != Vec3f::zero()){
        gl::color(0.0, 0.0, 1.0);
        posBlue = Vec2f(_blobB.x, _blobB.y);
        gl::drawStrokedCircle(Vec2f(posBlue.x, posBlue.y), _blobB.z);
    }else{ drawVec = false; }    
    
    // Drawing the car direction (a cross)
    gl::color(1.0f, 1.0f, 1.0f);
    
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
    
#endif
    
    _car.draw();
    
}


CINDER_APP_BASIC( LCTrackerApp, RendererGl )
