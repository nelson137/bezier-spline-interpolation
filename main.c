#include "main.h"


static gboolean primary_button_down = FALSE;


/**
 * Callback for mouse button press.
 */
gboolean on_button_press(GtkWidget *w, GdkEventButton *event, gpointer arg) {
    if (event->button == GDK_BUTTON_PRIMARY)
        primary_button_down = TRUE;
    return TRUE;
}


/**
 * Callback for mouse motion.
 */
gboolean on_motion_notify(GtkWidget *w, GdkEvent *event, gpointer arg) {
    if (event->type != GDK_MOTION_NOTIFY)
        return FALSE;

    GdkEventMotion *e = (GdkEventMotion*) event;

    GdkWindow *window = gtk_widget_get_window(w);
    int width = gdk_window_get_width(window);
    int height = gdk_window_get_height(window);

    if (primary_button_down == TRUE)
        move_hover_knot(e->x, e->y, width, height);
    else
        update_hover_knot(e->x, e->y);

    return TRUE;
}


/**
 * Callback for mouse button release.
 */
gboolean on_button_release(GtkWidget *w, GdkEventButton *event, gpointer arg) {
    if (event->button == GDK_BUTTON_PRIMARY)
        primary_button_down = FALSE;
    return TRUE;
}


/**
 * Callback executed for each tick.
 */
gboolean tick_callback(
    GtkWidget *widget,
    GdkFrameClock *frame_clock,
    gpointer arg
) {
    gtk_widget_queue_draw(GTK_WIDGET(arg));
    return TRUE;
}


int main(int argc, char *argv[]) {
    srand(time(NULL));

    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    GtkWidget *frame = gtk_frame_new(NULL);
    gtk_container_add(GTK_CONTAINER(window), frame);

    GtkWidget *drawing_area = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(frame), drawing_area);

    gtk_window_set_role(GTK_WINDOW(window), "pop-up");
    gtk_window_move(GTK_WINDOW(window), 100, 100);
    gtk_window_set_default_size(GTK_WINDOW(window), 1024, 800);

    CONNECT_SIMPLE_SIGNAL(drawing_area, "draw", draw);
    CONNECT_SIMPLE_SIGNAL(window, "button-press-event", on_button_press);
    CONNECT_SIMPLE_SIGNAL(window, "motion-notify-event", on_motion_notify);
    CONNECT_SIMPLE_SIGNAL(window, "button-release-event", on_button_release);
    CONNECT_SIMPLE_SIGNAL(window, "destroy", gtk_main_quit);

    // Request access to cursor motion events
    gtk_widget_set_events(
        drawing_area,
        gtk_widget_get_events(window) | GDK_POINTER_MOTION_MASK);

    gtk_widget_add_tick_callback(
        drawing_area,
        tick_callback,
        drawing_area,
        NULL);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}
