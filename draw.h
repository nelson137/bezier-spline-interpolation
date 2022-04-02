#ifndef DRAW_H
#define DRAW_H

#include <float.h>
#include <stdlib.h>

#include <gtk/gtk.h>

#include "util.h"

typedef struct
{
    double r, g, b;
} rgb_t;

void init_animation_data();

void free_animation_data();

int get_knot_index_at(double x, double y);

int get_num_knots();

void move_knot(
    int index,
    double x,
    double y,
    double win_width,
    double win_height);

void try_remove_knot(int index);

int try_add_knot_at(double x, double y);

void draw(
    GtkDrawingArea *widget,
    cairo_t *cr,
    int width,
    int height,
    gpointer arg);

#endif
