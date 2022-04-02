#include "util.h"

void array_insert(double *array, int size, int index, double value)
{
    for (int i = size - 2; i >= 0; --i)
    {
        if (i >= index)
            array[i + 1] = array[i];
        if (i == index)
            array[i] = value;
    }
}

void array_remove(double *array, int size, int index)
{
    for (int i = 0; i < size - 1; ++i)
        if (i >= index)
            array[i] = array[i + 1];
}

double bezier_func(double p0, double p1, double p2, double p3, double t)
{
    return pow(1.0 - t, 3) * p0 +
           3.0 * (t - 2.0 * t * t + t * t * t) * p1 +
           3.0 * (t * t - t * t * t) * p2 +
           t * t * t * p3;
}

/**
 * Clamp x on the interval [min,max].
 */
double clamp(double x, double min, double max)
{
    if (x < min)
        return min;
    else if (x > max)
        return max;
    else
        return x;
}

/**
 * https://cboard.cprogramming.com/c-programming/155809-find-closest-point-line.html#post1158165
 */
void closest_point_on_line(
    double Ax, double Ay,
    double Bx, double By,
    double Px, double Py,
    double *x, double *y)
{
    double APx = Px - Ax;
    double APy = Py - Ay;
    double ABx = Bx - Ax;
    double ABy = By - Ay;
    double magAB2 = ABx * ABx + ABy * ABy;
    double ABdotAP = ABx * APx + ABy * APy;
    double t = ABdotAP / magAB2;

    if (t < 0)
    {
        *x = Ax;
        *y = Ay;
    }
    else if (t > 1)
    {
        *x = Bx;
        *y = By;
    }
    else
    {
        *x = Ax + ABx * t;
        *y = Ay + ABy * t;
    }
}

/**
 * Return the distance between the points (x1,y1) and (x2,y2).
 */
double distance(double x1, double y1, double x2, double y2)
{
    static double dx, dy;
    dx = x2 - x1;
    dy = y2 - y1;
    return sqrt(dx * dx + dy * dy);
}

/**
 * Return the difference between times `a` and `b` in milliseconds.
 */
double elapsed_ms(struct timeval *a, struct timeval *b)
{
    int elapsed = 1000.0 * ((double)b->tv_sec - (double)a->tv_sec);
    elapsed += 0.001 * ((double)b->tv_usec - (double)a->tv_usec);
    return elapsed;
}
