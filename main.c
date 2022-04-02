#include "main.h"

#define APP_ID "com.nelsonearle.bezier-spline-interpolation"

// Width and height for the window and drawing area
static int WIDTH = 1024;
static int HEIGHT = 600;

// Thresholds for distinguishing a Simple Click from a Click & Drag
#define MAX_CLICK_DURATION_MS 450.0
#define MAX_CLICK_DIST_DELTA 4.0

// Primary click data for adding a knot
static double click1_start_x, click1_start_y;
static struct timeval click1_start_time;

// Secondary click data for removing a knot
static double click2_start_x, click2_start_y;
static struct timeval click2_start_time;

// Click & drag data for moving a knot
static double drag_start_x, drag_start_y;
static int drag_knot_index;

/**
 * Callback for primary mouse button press to add a knot to the spline.
 *
 * Store information for determining if this event is a Simple Click or a Click
 * & Drag. The determination and potential knot add will occur on release.
 *
 * All click events are Click & Drag, but for the purposed of
 * this app, we will consider an event to be a Simple Click if:
 *
 * - the click duration (time difference between press and release) does not
 *   exceed a threshold; or
 * - the click region (euclidean distance between start and end positions) does
 *   not exceed a threshold.
 */
void on_press_primary(GtkGestureClick *g, gint n, gdouble x, gdouble y, gpointer arg)
{
    click1_start_x = x;
    click1_start_y = y;
    gettimeofday(&click1_start_time, NULL);
}

/**
 * Callback for primary mouse button release to add a knot to the spline.
 *
 * Attempt to add a knot to the spline at the current position. A knot will only
 * be added if the click was a Simple Click and the mouse is hovering over the
 * spline.
 *
 * See `on_press_primary()` for the criteria for a Simple Click.
 */
void on_release_primary(GtkGestureClick *g, gint n, gdouble x, gdouble y, gpointer arg)
{
    static struct timeval now;
    gettimeofday(&now, NULL);

    if (elapsed_ms(&click1_start_time, &now) < MAX_CLICK_DURATION_MS &&
        distance(click1_start_x, click1_start_y, x, y) < MAX_CLICK_DIST_DELTA)
    {
        try_add_knot_at(x, y);
    }
}

/**
 * Callback for secondary mouse button press to remove a knot from the spline.
 *
 * Store information for determining if this event is a Simple Click or a Click
 * & Drag. The determination and potential knot removal will occur on release.
 *
 * See `on_press_primary()` for the criteria for a Simple Click.
 */
void on_press_secondary(GtkGestureClick *g, gint n, gdouble x, gdouble y, gpointer arg)
{
    click2_start_x = x;
    click2_start_y = y;
    gettimeofday(&click2_start_time, NULL);
}

/**
 * Callback for secondary mouse button release to remove a knot from the spline.
 *
 * Attempt to remove a knot from the spline at the current position. A knot will
 * only be removed if the click was a Simple Click, there is a knot at the
 * current position, and there are currently more than 2 knots in the spline.
 *
 * See `on_press_primary()` for the criteria for a Simple Click.
 */
void on_release_secondary(GtkGestureClick *g, gint n, gdouble x, gdouble y, gpointer arg)
{
    static struct timeval now;
    gettimeofday(&now, NULL);

    if (elapsed_ms(&click2_start_time, &now) < MAX_CLICK_DURATION_MS &&
        distance(click2_start_x, click2_start_y, x, y) < MAX_CLICK_DIST_DELTA &&
        get_num_knots() > 2)
    {
        try_remove_knot(get_knot_index_at(x, y));
    }
}

/**
 * Callback for mouse drag begin to move a knot.
 */
void drag_begin(GtkGestureDrag *g, gdouble start_x, gdouble start_y, gpointer arg)
{
    drag_start_x = start_x;
    drag_start_y = start_y;
    drag_knot_index = get_knot_index_at(start_x, start_y);
}

/**
 * Callback for mouse drag update to move a knot.
 */
void drag_update(GtkGestureDrag *g, gdouble offset_x, gdouble offset_y, gpointer arg)
{
    double x = drag_start_x + offset_x;
    double y = drag_start_y + offset_y;
    move_knot(drag_knot_index, x, y, (double)WIDTH, (double)HEIGHT);
}

/**
 * Callback for mouse drag end to move a knot.
 */
void drag_end(GtkGestureDrag *g, gdouble offset_x, gdouble offset_y, gpointer arg)
{
    drag_knot_index = -1;
}

/**
 * Callback for tick event.
 *
 * Queue a draw event for the widget passed as `arg`. The app will be
 * continuously re-drawing while running, but this is okay since state changes
 * and draws are fast because this app doesn't do much. This is also easier than
 * integrating many draw queues into the logic wherever the state changes.
 */
gboolean tick_callback(GtkWidget *w, GdkFrameClock *fc, gpointer arg)
{
    gtk_widget_queue_draw(GTK_WIDGET(arg));
    return TRUE;
}

/**
 * Return a drawing area for this app.
 *
 * Create the drawing area and connect all of the necessary event controllers.
 */
GtkWidget *create_drawing_area()
{
    GtkWidget *drawing_area = gtk_drawing_area_new();

    // Configure drawing area
    gtk_drawing_area_set_content_width(GTK_DRAWING_AREA(drawing_area), WIDTH);
    gtk_drawing_area_set_content_height(GTK_DRAWING_AREA(drawing_area), HEIGHT);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(drawing_area), draw, NULL, NULL);

    // Setup primary click gesture -- add knot
    GtkGesture *click1_gesture = gtk_gesture_click_new();
    gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(click1_gesture), GDK_BUTTON_PRIMARY);
    g_signal_connect(click1_gesture, "pressed", G_CALLBACK(on_press_primary), NULL);
    g_signal_connect(click1_gesture, "released", G_CALLBACK(on_release_primary), NULL);
    gtk_widget_add_controller(drawing_area, GTK_EVENT_CONTROLLER(click1_gesture));

    // Setup secondary click gesture -- remove knot
    GtkGesture *click2_gesture = gtk_gesture_click_new();
    gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(click2_gesture), GDK_BUTTON_SECONDARY);
    g_signal_connect(click2_gesture, "pressed", G_CALLBACK(on_press_secondary), NULL);
    g_signal_connect(click2_gesture, "released", G_CALLBACK(on_release_secondary), NULL);
    gtk_widget_add_controller(drawing_area, GTK_EVENT_CONTROLLER(click2_gesture));

    // Setup click & drag gesture -- move knot
    GtkGesture *drag_gesture = gtk_gesture_drag_new();
    gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(drag_gesture), GDK_BUTTON_PRIMARY);
    g_signal_connect(drag_gesture, "drag-begin", G_CALLBACK(drag_begin), NULL);
    g_signal_connect(drag_gesture, "drag-update", G_CALLBACK(drag_update), NULL);
    g_signal_connect(drag_gesture, "drag-end", G_CALLBACK(drag_end), NULL);
    gtk_widget_add_controller(drawing_area, GTK_EVENT_CONTROLLER(drag_gesture));

    return drawing_area;
}

/**
 * Setup the main window and its widgets for the app.
 */
void activate(GtkApplication *app, gpointer arg)
{
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Bezier Spline Interpolation");

    GtkWidget *drawing_area = create_drawing_area();
    gtk_window_set_child(GTK_WINDOW(window), drawing_area);

    gtk_window_set_default_size(GTK_WINDOW(window), WIDTH, HEIGHT);

    gtk_widget_add_tick_callback(
        drawing_area,
        tick_callback,
        drawing_area,
        NULL);

    gtk_widget_show(window);
}

int main(int argc, char *argv[])
{
    srand(time(NULL));

    init_animation_data();

    GtkApplication *app = gtk_application_new(
        APP_ID,
        G_APPLICATION_FLAGS_NONE);

    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    int code = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    free_animation_data();

    return code;
}
