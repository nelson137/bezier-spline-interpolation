#ifndef UTIL_H
#define UTIL_H

#include <math.h>

#include <sys/time.h>

void array_insert(double *array, int size, int index, double value);

void array_remove(double *array, int size, int index);

double bezier_func(double p0, double p1, double p2, double p3, double t);

double clamp(double x, double min, double max);

void closest_point_on_line(
    double Ax, double Ay,
    double Bx, double By,
    double Px, double Py,
    double *x, double *y);

double distance(double x1, double y1, double x2, double y2);

double elapsed_ms(struct timeval *a, struct timeval *b);

#endif
