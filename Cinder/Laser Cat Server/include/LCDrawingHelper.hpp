//
//  LCDrawingHelper.hpp
//  LCTracker
//
//  Created by William Lindmeier on 10/13/12.
//
//

#ifndef LCTracker_LCDrawingHelper_hpp
#define LCTracker_LCDrawingHelper_hpp

#include "cinder/Vector.h"

using namespace ci;

#define DegreesToRadians(x) (M_PI * x / 180.0)

#define MIN3(x,y,z)  ((y) <= (z) ? \
((x) <= (y) ? (x) : (y)) \
: \
((x) <= (z) ? (x) : (z)))

#define MAX3(x,y,z)  ((y) >= (z) ? \
((x) >= (y) ? (x) : (y)) \
: \
((x) >= (z) ? (x) : (z)))

#define DegreesToRadians(x) (M_PI * x / 180.0)
#define RadiansToDegrees(x) (x * (180.0/M_PI))

static inline const Vec2f RotatePointAroundCenter(const Vec2f &point, const Vec2f &center, float degrees)
{
    // We'll just pivot for the time being
    float theta = DegreesToRadians(degrees);
    float newX = cos(theta) * (point.x-center.x) - sin(theta) * (point.y-center.y) + center.x;
    float newY = sin(theta) * (point.x-center.x) + cos(theta) * (point.y-center.y) + center.y;
    return Vec2f(newX, newY);
}

struct ColorRGB {
    unsigned char r, g, b;    /* Channel intensities between 0 and 255 */
};

struct ColorHSV {
    unsigned char hue;        /* Hue degree between 0 and 255 */
    unsigned char sat;        /* Saturation between 0 (gray) and 255 */
    unsigned char val;        /* Value between 0 (black) and 255 */
};

const static inline Vec2f RaiansToVec2f(float rads)
{
    return Vec2f(cos(rads), sin(rads));
}

static struct ColorHSV RGBtoHSV(struct ColorRGB rgb) {
    
    struct ColorHSV hsv;
    
    unsigned char rgb_min, rgb_max;
    rgb_min = MIN3(rgb.r, rgb.g, rgb.b);
    rgb_max = MAX3(rgb.r, rgb.g, rgb.b);
    
    hsv.val = rgb_max;
    if (hsv.val == 0) {
        hsv.hue = hsv.sat = 0;
        return hsv;
    }
    hsv.sat = 255*long(rgb_max - rgb_min)/hsv.val;
    if (hsv.sat == 0) {
        hsv.hue = 0;
        return hsv;
    }
    if (rgb_max == rgb.r) {
        hsv.hue = 0 + 43*(rgb.g - rgb.b)/(rgb_max - rgb_min);
    } else if (rgb_max == rgb.g) {
        hsv.hue = 85 + 43*(rgb.b - rgb.r)/(rgb_max - rgb_min);
    } else /* rgb_max == rgb.b */ {
        hsv.hue = 171 + 43*(rgb.r - rgb.g)/(rgb_max - rgb_min);
    }
    return hsv;
    
}

#endif
