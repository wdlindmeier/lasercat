//
//  LCDrawingHelper.hpp
//  LCTracker
//
//  Created by William Lindmeier on 10/13/12.
//
//

#ifndef LCTracker_LCDrawingHelper_hpp
#define LCTracker_LCDrawingHelper_hpp

#define MIN3(x,y,z)  ((y) <= (z) ? \
((x) <= (y) ? (x) : (y)) \
: \
((x) <= (z) ? (x) : (z)))

#define MAX3(x,y,z)  ((y) >= (z) ? \
((x) >= (y) ? (x) : (y)) \
: \
((x) >= (z) ? (x) : (z)))


struct ColorRGB {
    unsigned char r, g, b;    /* Channel intensities between 0 and 255 */
};

struct ColorHSV {
    unsigned char hue;        /* Hue degree between 0 and 255 */
    unsigned char sat;        /* Saturation between 0 (gray) and 255 */
    unsigned char val;        /* Value between 0 (black) and 255 */
};

struct ColorHSV RGBtoHSV(struct ColorRGB rgb) {
    
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
