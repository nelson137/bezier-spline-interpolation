#ifndef DRAW_H
#define DRAW_H


#include <float.h>
#include <stdlib.h>

#include <gtk/gtk.h>

#include "util.h"


typedef struct {
    double r, g, b;
} rgb_t;


void init_animation_data();

void free_animation_data();

int get_num_knots();

gboolean has_hover_knot();

void update_hover_knot(double x, double y);

void move_hover_knot(double x, double y, double win_width, double win_height);

void try_remove_hover_knot();

int try_add_knot(double x, double y);

gboolean draw(GtkWidget *widget, cairo_t *cr, gpointer arg);


#endif
