lasercat
========

PComp Midterm Project for ITP
Authors: William Lindmeier, Harry How and Max Ma

More information on this project can be found here:
http://itp.nyu.edu/~wdl225/work/?p=232

FILES:
    
    The desktop app is in Cinder/Laser Cat Server/xcode
    
    The final Arduino sketch (for the car) is in Arduino/XBeeSerialCar
    
    uvc-ctrl is included in the root directory to control the webcam exposure and focus.
    http://www.dm9.se/?tag=uvc-ctrl

APP CONTROLS:
    
    Press 'esc' to get the configuration interface. This lets you define the color ranges.
    Press 'r' 'g' or 'b' to view the color detection for red, green or blue respectively.
    Press 'space' to get back to full color view.
    The above configurations will be saved to ~/Documents/lasercat_settings.csv

GETTING STARTED w/ CINDER:

    Install Cinder: http://libcinder.org/docs/welcome/GitSetup.html

    Install the OpenCV Cinder block. (instructions are at the bottom of the Cinder install page)

    Set your CINDER_PATH at the bottom of the Build Settings tab of Cinder/LCTracker/xcode/LCTracker.xcodeproj
        NOTE: replace "harry" with whatever your system username is. Mine is "bill".

    The app should run at this point
        
   