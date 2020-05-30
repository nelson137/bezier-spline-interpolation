#include "util.h"


/**
 * Clamp x on the interval [min,max].
 */
double clamp(double x, double min, double max) {
    if (x < min)
        return min;
    else if (x > max)
        return max;
    else
        return x;
}


/**
 * Return the distance between the points (x1,y1) and (x2,y2).
 */
double distance(double x1, double y1, double x2, double y2) {
    static double dx, dy;
    dx = x2 - x1;
    dy = y2 - y1;
    return sqrt(dx*dx + dy*dy);
}
