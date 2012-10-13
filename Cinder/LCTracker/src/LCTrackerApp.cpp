#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"
#include "cinder/Channel.h"
#include "cinder/Surface.h"
#include "cinder/Area.h"
// Boost
#include <boost/lexical_cast.hpp>
// Block
#include "CinderOpenCv.h"
// LC
#include "LCDrawingHelper.hpp"

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
    void replaceContoursInCollection(vector< vector<cv::Point> > *contours, vector<Vec3f> *contourCollection);
    void drawContours(vector<Vec3f> *contourCollection);
    
private:
    
    gl::Texture _texTrack;
    cv::Mat     _cvMat;
    Vec2f       _posRed;
    Vec2f       _posGreen;
    Vec2f       _posBlue;
    Surface8u   _surfTracking;
    
    vector<Vec3f> _blobContoursR;
    vector<Vec3f> _blobContoursG;
    vector<Vec3f> _blobContoursB;
};

LCTrackerApp::LCTrackerApp()
{

};

void LCTrackerApp::setup()
{
    setWindowSize(640, 640);
    _surfTracking = loadImage(loadResource("sample_track_iphone_4.jpg"));
    _texTrack = gl::Texture(_surfTracking);
    _blobContoursR.clear();
    _blobContoursG.clear();
    _blobContoursB.clear();

}

void LCTrackerApp::mouseDown( MouseEvent event )
{
}

void LCTrackerApp::update()
{

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

            // HSV hue mapped to 0-255 (not 0-360)
            static const int MIN_R = 226;
            static const int MAX_R = 255;
            
            static const int MIN_G = 58;
            static const int MAX_G = 102;
            
            static const int MIN_B = 162;
            static const int MAX_B = 184;
            
            // Check if it's bright enough and saturated enough to be a tracking pixel
            if(hsv.val > 100 && hsv.sat > 100){
                
                // Check Red
                if(hsv.hue > MIN_R && hsv.hue < MAX_R){

                    chR.setValue(px, 255);

                // Check Green
                }else if(hsv.hue > MIN_G && hsv.hue < MAX_G){

                    chG.setValue(px, 255);
                    
                // Check Blue
                }else if(hsv.hue > MIN_B && hsv.hue < MAX_B){

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

    replaceContoursInCollection(&contoursR, &_blobContoursR);
    replaceContoursInCollection(&contoursG, &_blobContoursG);
    replaceContoursInCollection(&contoursB, &_blobContoursB);

    _texTrack = gl::Texture(_surfTracking);

}

void LCTrackerApp::replaceContoursInCollection(vector< vector<cv::Point> > *contours, vector<Vec3f> *contourCollection)
{
    contourCollection->clear();
    
    static const float BlobSizeMin = 2.0f;
    static const float BlobSizeMax = 640.0f;
   
    for (vector<vector<cv::Point> >::iterator it=contours->begin() ; it < contours->end(); it++ ){
        
        // center abd radius for current blob
        cv::Point2f center;
        float radius;
        
        // convert the cuntour point to a matrix
        vector<cv::Point> pts = *it;
        cv::Mat pointsMatrix = cv::Mat(pts);
        cv::minEnclosingCircle(pointsMatrix, center, radius);

        if (radius > BlobSizeMin && radius < BlobSizeMax) {
            
            // Store the location and radius
            contourCollection->push_back(Vec3f(center.x, center.y, radius));
            
        }
        
    }

}

void LCTrackerApp::drawContours(vector<Vec3f> *contourCollection)
{
    // Only draw the biggest one we find
    Vec3f largestContour = Vec3f::zero();
    
    for (vector<Vec3f>::iterator it=contourCollection->begin() ; it < contourCollection->end(); it++ ){
        Vec3f blob = *it;
        if(blob.z > largestContour.z){
            largestContour = blob;
        }
    }
    
    gl::drawStrokedCircle(Vec2f(largestContour.x, largestContour.y), largestContour.z);
}

void LCTrackerApp::draw()
{
	gl::clear( Color( 0.25, 0.25, 0.25 ) );
    
    gl::color(1.0, 1.0, 1.0);
    gl::draw(_texTrack);
    
    glLineWidth(2.0f);
    
    // Draw white circles around the blobs
    gl::color(1.0, 0.0, 0.0);
    drawContours(&_blobContoursR);
    
    gl::color(0.0, 1.0, 0.0);
    drawContours(&_blobContoursG);

    gl::color(0.0, 0.0, 1.0);
    drawContours(&_blobContoursB);
    
    float fps = getAverageFps();
    string fpsString = "fps: ";
    fpsString += boost::lexical_cast<string>((int)fps);
    
    gl::drawString(fpsString, Vec2f(500.0, 10.0f), Color(0.6,0.6,0.6), Font("Helvetica", 24.0f));
}


CINDER_APP_BASIC( LCTrackerApp, RendererGl )
