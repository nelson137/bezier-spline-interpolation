#include "draw.h"


static double knots_x[] = {60, 220, 420, 700};
static double knots_y[] = {60, 300, 300, 240};
#define N_KNOTS ( sizeof(knots_x) / sizeof(knots_x[0]) )
#define N_SEGS  ( N_KNOTS - 1 )
static int hover_knot_index = -1;

static rgb_t seg_colors[] = {
    { 1, 0, 0 },
    { 0, 1, 0 },
    { 0, 0, 1 },
};

#define KNOT_RADIUS       16
#define KNOT_BORDER_WIDTH 6
#define SEG_LINE_WIDTH    8


/**
 * Set hover_knot_index to the index of the knot that intersects (x,y).
 * If no knots intersect the given coordinates, set the value to -1.
 */
void update_hover_knot(double x, double y) {
    for (int i=0; i<N_KNOTS; ++i) {
        if (distance(x, y, knots_x[i], knots_y[i]) <= KNOT_RADIUS) {
            hover_knot_index = i;
            return;
        }
    }

    hover_knot_index = -1;
}


/**
 * Move the knot given by index such that it is centered at (x,y).
 * If no knot is hovered over (hover_knot_index==-1), do nothing.
 * The new x and y positions are clamped on the intervals
 *   [0,window_width] and [0,window_height], respectively.
 */
void move_hover_knot(double x, double y, double win_width, double win_height) {
    if (hover_knot_index < 0 || hover_knot_index > N_KNOTS-1)
        return;

    knots_x[hover_knot_index] = clamp(x, KNOT_RADIUS, win_width-KNOT_RADIUS);
    knots_y[hover_knot_index] = clamp(y, KNOT_RADIUS, win_height-KNOT_RADIUS);
}


/**
 * Perform spline interpolation on one dimension of knot coordinates.
 * For example, given the x coordinates for all knots, calculate the
 *   x coordinate for each knot's first and second control points.
 */
static void calculate_controls(
    double knots[],      // x or y values for knots
    double controls1[],  // x or y values for first control points
    double controls2[]   // x or y values for second control points
) {
    static double a[N_SEGS], b[N_SEGS], c[N_SEGS], r[N_SEGS];
    static int i;

    // Left-most segment
    a[0] = 1;
    b[0] = 2;
    c[0] = 1;
    r[0] = knots[0] + 2.0*knots[1];

    // Internal segments
    for (i=1; i<N_SEGS; ++i) {
        a[i] = 1;
        b[i] = 4;
        c[i] = 1;
        r[i] = 4.0*knots[i] + 2.0*knots[i+1];
    }

    // Right-most segment
    a[N_SEGS-1] = 2;
    b[N_SEGS-1] = 7;
    c[N_SEGS-1] = 0;
    r[N_SEGS-1] = 8.0*knots[N_SEGS-1] + knots[N_SEGS];

    // Thomas algorithm
    static double m;
    for (i=1; i<N_SEGS; ++i) {
        m = a[i] / b[i-1];
        b[i] = b[i] - m*c[i-1];
        r[i] = r[i] - m*r[i-1];
    }

    // Calculate x or y value for first control points
    controls1[N_SEGS-1] = r[N_SEGS-1] / b[N_SEGS-1];
    for (i=N_SEGS-2; i>=0; --i)
        controls1[i] = (r[i] - c[i]*controls1[i+1]) / b[i];

    // Calculate x or y value for second control points
    for (i=0; i<N_SEGS-1; ++i)
        controls2[i] = 2.0*knots[i+1] - controls1[i+1];
    controls2[N_SEGS-1] = (knots[N_SEGS] + controls1[N_SEGS-1]) / 2.0;
}


/**
 * Draw a frame.
 */
gboolean draw(GtkWidget *widget, cairo_t *cr, gpointer arg) {
    // Paint background color

    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);

    // Calculate bezier control points for segments

    static double ctls1_x[N_SEGS], ctls1_y[N_SEGS];
    static double ctls2_x[N_SEGS], ctls2_y[N_SEGS];

    calculate_controls(knots_x, ctls1_x, ctls2_x);
    calculate_controls(knots_y, ctls1_y, ctls2_y);

    // Draw segments

    cairo_move_to(cr, knots_x[0], knots_y[0]);
    cairo_set_line_width(cr, SEG_LINE_WIDTH);

    double x, y;
    for (int i=0; i<N_SEGS; ++i) {
        cairo_set_source_rgb(
            cr,
            seg_colors[i].r,
            seg_colors[i].g,
            seg_colors[i].b);

        // Draw segment, current point becomes the endpoint
        cairo_curve_to(
            cr,
            ctls1_x[i], ctls1_y[i],
            ctls2_x[i], ctls2_y[i],
            knots_x[i+1], knots_y[i+1]);

        // Save the current point because cairo_stroke() will clear it
        cairo_get_current_point(cr, &x, &y);
        cairo_stroke(cr);
        cairo_move_to(cr, x, y);
    }

    // Draw knots

    cairo_set_line_width(cr, KNOT_BORDER_WIDTH);

    for (int i=0; i<N_KNOTS; ++i) {
        cairo_new_sub_path(cr);

        cairo_arc(cr, knots_x[i], knots_y[i], KNOT_RADIUS, 0, 2*G_PI);

        cairo_set_source_rgb(cr, 1, .84, 0);
        cairo_fill_preserve(cr);

        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_stroke(cr);
    }

    return TRUE;
}
