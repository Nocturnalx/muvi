#include <iostream>
#include <gtk/gtk.h>
#include "Visualiser.h"

int main (int argc, char **argv) {

    int status;
    
    GtkApplication * app = gtk_application_new ("com.utladal.visualiser", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);

    status = g_application_run (G_APPLICATION (app), argc, argv);
    
    g_object_unref (app);

    return status;
}