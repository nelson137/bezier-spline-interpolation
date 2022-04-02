#include "draw.h"

static double *knots_x, *knots_y;
static double *ctrls1_x, *ctrls1_y, *ctrls2_x, *ctrls2_y;
static int n_knots, n_segs;

static rgb_t seg_colors[] = {
    {1, 0, 0},
    {0, 1, 0},
    {0, 0, 1},
};

static int n_colors = sizeof(seg_colors) / sizeof(seg_colors[0]);

#define KNOT_DOT_RADIUS 16
#define KNOT_BORDER_WIDTH 6
#define KNOT_RADIUS (KNOT_DOT_RADIUS + KNOT_BORDER_WIDTH / 2)
#define SEG_LINE_WIDTH 8

void init_animation_data()
{
    n_knots = 4;
    n_segs = n_knots - 1;

    knots_x = malloc(sizeof(double) * n_knots);
    knots_y = malloc(sizeof(double) * n_knots);

    knots_x[0] = 60;
    knots_x[1] = 220;
    knots_x[2] = 420;
    knots_x[3] = 700;

    knots_y[0] = 60;
    knots_y[1] = 300;
    knots_y[2] = 300;
    knots_y[3] = 240;

    ctrls1_x = malloc(sizeof(double) * n_segs);
    ctrls1_y = malloc(sizeof(double) * n_segs);
    ctrls2_x = malloc(sizeof(double) * n_segs);
    ctrls2_y = malloc(sizeof(double) * n_segs);
}

void free_animation_data()
{
    free(knots_x);
    free(knots_y);

    free(ctrls1_x);
    free(ctrls1_y);
    free(ctrls2_x);
    free(ctrls2_y);
}

/**
 * Return the number of knots.
 */
int get_num_knots()
{
    return n_knots;
}

/**
 * Return the index of the knot that intersects (x,y), otherwise -1.
 */
int get_knot_index_at(double x, double y)
{
    for (int i = 0; i < n_knots; ++i)
        if (distance(x, y, knots_x[i], knots_y[i]) <= KNOT_RADIUS)
            return i;
    return -1;
}

/**
 * Move knot `index` such that it is centered at (x,y), do nothing if `index`
 * is out of bounds.
 *
 * The new x and y positions are clamped on the intervals
 *   [0,window_width] and [0,window_height], respectively.
 */
void move_knot(int index, double x, double y, double win_width, double win_height)
{
    if (index < 0 || index > n_knots - 1)
        return;

    knots_x[index] = clamp(x, KNOT_RADIUS, win_width - KNOT_RADIUS);
    knots_y[index] = clamp(y, KNOT_RADIUS, win_height - KNOT_RADIUS);
}

/**
 * Try to remove knot `index`, do nothing if out of bounds.
 */
void try_remove_knot(int index)
{
    if (index < 0 || index > n_knots - 1)
        return;

    array_remove(knots_x, n_knots, index);
    array_remove(knots_y, n_knots, index);

    --n_knots;
    --n_segs;

    knots_x = realloc(knots_x, sizeof(double) * n_knots);
    knots_y = realloc(knots_y, sizeof(double) * n_knots);

    ctrls1_x = realloc(ctrls1_x, sizeof(double) * n_segs);
    ctrls1_y = realloc(ctrls1_y, sizeof(double) * n_segs);
    ctrls2_x = realloc(ctrls2_x, sizeof(double) * n_segs);
    ctrls2_y = realloc(ctrls2_y, sizeof(double) * n_segs);
}

/**
 * Insert knot with center (x,y) at `index`.
 */
static void _insert_knot(int index, double x, double y)
{
    if (index < 0 || index > n_knots - 1)
        return;

    ++n_knots;
    ++n_segs;

    knots_x = realloc(knots_x, sizeof(double) * n_knots);
    knots_y = realloc(knots_y, sizeof(double) * n_knots);

    array_insert(knots_x, n_knots, index, x);
    array_insert(knots_y, n_knots, index, y);

    ctrls1_x = realloc(ctrls1_x, sizeof(double) * n_segs);
    ctrls1_y = realloc(ctrls1_y, sizeof(double) * n_segs);
    ctrls2_x = realloc(ctrls2_x, sizeof(double) * n_segs);
    ctrls2_y = realloc(ctrls2_y, sizeof(double) * n_segs);
}

/**
 * Add a knot at (x,y) if it is on a segment of the spline.
 */
int try_add_knot_at(double x, double y)
{
    int min_seg_i;
    double min_dist = DBL_MAX, min_x, min_y;
    // Change in t
    static double dt = .1;
    // Point on bezier curve
    double bAx, bAy, bBx, bBy;
    // Point closest to line segment
    double cx, cy;
    // Distance from cursor to closest point on line segment
    double dist;

    // for each spline segment
    for (int i = 0; i < n_segs; ++i)
    {
        // split spline segment into 1/dt line segments
        for (double t = 0; t < 1 - dt; t += dt)
        {
            // Calculate endpoints of line segment
            bAx = bezier_func(
                knots_x[i], ctrls1_x[i], ctrls2_x[i], knots_x[i + 1], t);
            bAy = bezier_func(
                knots_y[i], ctrls1_y[i], ctrls2_y[i], knots_y[i + 1], t);
            bBx = bezier_func(
                knots_x[i], ctrls1_x[i], ctrls2_x[i], knots_x[i + 1], t + dt);
            bBy = bezier_func(
                knots_y[i], ctrls1_y[i], ctrls2_y[i], knots_y[i + 1], t + dt);
            // Calculate closest point on line segment to (x,y)
            closest_point_on_line(bAx, bAy, bBx, bBy, x, y, &cx, &cy);
            dist = distance(x, y, cx, cy);
            if (dist < min_dist || min_dist == DBL_MAX)
            {
                min_seg_i = i;
                min_dist = dist;
                min_x = cx;
                min_y = cy;
            }
        }
    }

    // If (x,y) is on a line segment
    if (min_dist <= SEG_LINE_WIDTH)
        // Create a new knot at (x,y)
        _insert_knot(min_seg_i + 1, min_x, min_y);

    return 0;
}

/**
 * Perform spline interpolation on one dimension of knot coordinates.
 * For example, given the x coordinates for all knots, calculate the
 * x coordinate for each knot's first and second control points.
 */
static void calculate_controls(
    double knots[],     // x or y values for knots
    double controls1[], // x or y values for first control points
    double controls2[]  // x or y values for second control points
)
{
    double *a, *b, *c, *r;
    static int i;

    double **ptrs[] = {&a, &b, &c, &r};
    for (int i = 0; i < 4; ++i)
        *ptrs[i] = malloc(sizeof(double) * n_segs);

    // Left-most segment
    a[0] = 1;
    b[0] = 2;
    c[0] = 1;
    r[0] = knots[0] + 2.0 * knots[1];

    // Internal segments
    for (i = 1; i < n_segs; ++i)
    {
        a[i] = 1;
        b[i] = 4;
        c[i] = 1;
        r[i] = 4.0 * knots[i] + 2.0 * knots[i + 1];
    }

    // Right-most segment
    a[n_segs - 1] = 2;
    b[n_segs - 1] = 7;
    c[n_segs - 1] = 0;
    r[n_segs - 1] = 8.0 * knots[n_segs - 1] + knots[n_segs];

    // Thomas algorithm
    static double m;
    for (i = 1; i < n_segs; ++i)
    {
        m = a[i] / b[i - 1];
        b[i] = b[i] - m * c[i - 1];
        r[i] = r[i] - m * r[i - 1];
    }

    // Calculate x or y value for first control points
    controls1[n_segs - 1] = r[n_segs - 1] / b[n_segs - 1];
    for (i = n_segs - 2; i >= 0; --i)
        controls1[i] = (r[i] - c[i] * controls1[i + 1]) / b[i];

    // Calculate x or y value for second control points
    for (i = 0; i < n_segs - 1; ++i)
        controls2[i] = 2.0 * knots[i + 1] - controls1[i + 1];
    controls2[n_segs - 1] = (knots[n_segs] + controls1[n_segs - 1]) / 2.0;

    for (int i = 0; i < 4; ++i)
        free(*ptrs[i]);
}

/**
 * Draw a frame.
 */
void draw(
    GtkDrawingArea *widget,
    cairo_t *cr,
    int width,
    int height,
    gpointer arg)
{
    // Paint background color

    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);

    // Calculate bezier control points for segments

    calculate_controls(knots_x, ctrls1_x, ctrls2_x);
    calculate_controls(knots_y, ctrls1_y, ctrls2_y);

    // Draw segments

    cairo_move_to(cr, knots_x[0], knots_y[0]);
    cairo_set_line_width(cr, SEG_LINE_WIDTH);

    double x, y;
    for (int i = 0; i < n_segs; ++i)
    {
        cairo_set_source_rgb(
            cr,
            seg_colors[i % n_colors].r,
            seg_colors[i % n_colors].g,
            seg_colors[i % n_colors].b);

        // Draw segment, current point becomes the endpoint
        cairo_curve_to(
            cr,
            ctrls1_x[i], ctrls1_y[i],
            ctrls2_x[i], ctrls2_y[i],
            knots_x[i + 1], knots_y[i + 1]);

        // Save the current point because cairo_stroke() will clear it
        cairo_get_current_point(cr, &x, &y);
        cairo_stroke(cr);
        cairo_move_to(cr, x, y);
    }

    // Draw knots

    cairo_set_line_width(cr, KNOT_BORDER_WIDTH);

    for (int i = 0; i < n_knots; ++i)
    {
        cairo_new_sub_path(cr);

        cairo_arc(cr, knots_x[i], knots_y[i], KNOT_DOT_RADIUS, 0, 2 * G_PI);

        cairo_set_source_rgb(cr, 1, .84, 0);
        cairo_fill_preserve(cr);

        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_stroke(cr);
    }
}
