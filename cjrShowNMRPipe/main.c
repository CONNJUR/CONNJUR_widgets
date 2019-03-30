/*
 * Copyright (C) 2018 Michael R. Gryk
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

/* 
 * File:   main.c
 * Author: mgryk
 *
 * Created on May 31, 2018, 3:59 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include "../lib/cjrutil.h"
#include "../lib/widgets.h"
#include "../lib/sparky.h"
#include "../lib/nmrview.h"

/*
 *  main
 * 
 *  This program expects one filename argument: namely an NMRView (.nv) file
 *  The program parses the header for display on a GTK widget
 *  This utilizes Gtk Builder with a Glade produced graphical interface.
 *  If the NMRView header is invalid, an error Dialog is displayed.
 */

int main(int argc, char** argv) {

    nmrviewHeaderStruct nmrviewHeader;
    char nmrviewOverall[256], nmrviewDimensions[256];
    GtkBuilder *builder;
    GtkWidget *window, *nmrviewLabel1, *nmrviewLabel2;
    int i;
    GError *error = NULL;
    
    if (argc != 2) {printf("Usage: %s filename\n", argv[0]); return 1;}

    gtk_init(&argc, &argv);
    
    nmrviewHeader = readNMRViewHeader(argv[1], &error);
    if (error != NULL) {
                g_warning("%s", error->message);
                showDialog(error->message, NULL);
		g_error_free(error);
		return(1);
    }
    
    printf("%s\n",nmrviewHeaderAsString(&nmrviewHeader));
    
//   printf("%ld\t%ld\t%ld\n", sizeof(nmrviewFileHeaderStruct), sizeof(nmrviewDimHeaderStruct), sizeof(nmrviewHeaderStruct));
    
    sprintf(nmrviewOverall, "<b>%dD FILE</b>\n", nmrviewHeader.file.nDim);
    strcpy(nmrviewDimensions,"");
    for(i=0; i<nmrviewHeader.file.nDim; i++) {
        sprintf(&nmrviewDimensions[strlen(nmrviewDimensions)],"%s: %d points, %.2f MHz, %.1f Hz, %.2f ppm\n", 
                nmrviewHeader.axis[i].label, nmrviewHeader.axis[i].size,
                nmrviewHeader.axis[i].sf, nmrviewHeader.axis[i].sw,
                nmrviewHeader.axis[i].refval);
    } 

    builder = gtk_builder_new();    // NEED A SOLUTION FOR GLADE PATH
    if (!gtk_builder_add_from_file(builder, "/home/nmrbox/mgryk/CONNJUR_spike/cjrShowSparky/glades/sparky.glade", &error)) {
		g_warning("%s", error->message);
                showDialog(error->message, NULL);
		g_error_free(error);
		return(1);
    }
    window = GTK_WIDGET(gtk_builder_get_object(builder, "sparkyWindow"));
    gtk_builder_connect_signals(builder, NULL);
    
    nmrviewLabel1 = GTK_WIDGET(gtk_builder_get_object(builder, "sparkyLabel1"));
    nmrviewLabel2 = GTK_WIDGET(gtk_builder_get_object(builder, "sparkyLabel2"));
    gtk_label_set_markup(GTK_LABEL(nmrviewLabel1), nmrviewOverall);
    gtk_label_set_text(GTK_LABEL(nmrviewLabel2), nmrviewDimensions);
    
    g_object_unref(G_OBJECT(builder));
    gtk_widget_show(window);

    gtk_main(); 
	
    return (EXIT_SUCCESS);
}

