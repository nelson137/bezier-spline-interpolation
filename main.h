#ifndef MAIN_H
#define MAIN_H


#include <pthread.h>
#include <time.h>
#include <unistd.h>

#include <gtk/gtk.h>

#include "draw.h"


#define CONNECT_SIMPLE_SIGNAL(widget, sig, callback) \
    g_signal_connect(G_OBJECT(widget), (sig), G_CALLBACK(callback), NULL)


#endif
