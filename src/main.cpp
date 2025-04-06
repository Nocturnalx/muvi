#include <iostream>
#include <gtk/gtk.h>
#include "Visualiser.h"

void parseArgs(int argc, char **argv){

    if (argc > 1){

        if (strcmp(argv[1], "-d") == 0){
            inputDevice = strtol(argv[2], nullptr, 0);

            if (inputDevice < 0 || inputDevice > 20) inputDevice = 0;
        }
    }
}

int main (int argc, char **argv) {

    std::terminate_handler(quitProgram);

    parseArgs(argc, argv);

    int status;
    
    GtkApplication * app = gtk_application_new ("com.utladal.visualiser", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);

    status = g_application_run (G_APPLICATION (app), 0, NULL);
    
    g_object_unref (app);

    return status;
}