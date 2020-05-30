#ifndef DRAW_H
#define DRAW_H


#include <gtk/gtk.h>

#include "util.h"


typedef struct {
    double r, g, b;
} rgb_t;


void update_hover_knot(double x, double y);

void move_hover_knot(double x, double y, double win_width, double win_height);

gboolean draw(GtkWidget *widget, cairo_t *cr, gpointer arg);


#endif
