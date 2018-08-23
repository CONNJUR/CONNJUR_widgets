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
#include <sys/stat.h>
#include <string.h>
#include <gtk/gtk.h>
#include "../lib/varian.h"
#include "../lib/cjrutil.h"
#include "../lib/widgets.h"

/*
 * scrollToInvalidLine
 * 
 * This function is used to scroll the text view to the appropriate error line.
 * 
 * Inputs:  GtkNotebook where the handler is set.
 *          Page of notebook which sends signal
 *          Page number of notebook which sends singal
 *          *user_data in the form of the text-view with an "invalid" mark on error line.
 */

void on_notebook_switch_page (GtkNotebook *notebook, GtkWidget *page, guint page_num, gpointer user_data);

/*
 * inValidProcparWindow
 * 
 * This function rebuilds the interface to highlight errors in parsing the procpar.
 * 
 * Inputs: GtkBuilder for the normal procpar window
 *         procpar file (loaded into a string);
 *         line of file which threw the exception.
 */

void inValidProcparWindow(GtkBuilder *builder, char *procparString, int badLine);

/*
 * scrollToInvalidLine
 * 
 * This function is used to scroll the text view to the appropriate error line.
 * 
 * Inputs:  GtkNotebook where the handler is set.
 *          Page of notebook which sends signal
 *          Page number of notebook which sends singal
 *          *user_data in the form of the text-view with an "invalid" mark on error line.
 */

void on_notebook_switch_page (GtkNotebook *notebook, GtkWidget *page, guint page_num, gpointer user_data) {
    
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(user_data));
    GtkTextMark *mark = gtk_text_buffer_get_mark(buffer, "invalid");
    gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(user_data), mark, 0, TRUE, 0, 0.5);
    return;
}

/*
 * inValidProcparWindow
 * 
 * This function rebuilds the interface to highlight errors in parsing the procpar.
 * 
 * It retrieves the normal interface which has a notebook including one page for 
 *      the normal interface, and a second page highlighting the error. This is done 
 *      by presenting the procpar file as a text-view, and highlighting the error line.
 * 
 * Inputs: GtkBuilder for the normal procpar window
 *         procpar file (loaded into a string);
 *         line of file which threw the exception.
 */

void inValidProcparWindow(GtkBuilder *builder, char *procparString, int badLine) {
    
    GtkWidget *notebook, *procparTextView, *procparScrolledWindow, *textScrolledWindow;
    GtkTextBuffer *buffer;
    GtkTextIter startIter, endIter;
    GtkTextMark *startMark = gtk_text_mark_new("invalid", TRUE);
    
    // Retrieve interface from builder / glade
    procparTextView = GTK_WIDGET(gtk_builder_get_object(builder, "procparTextView"));
    notebook = GTK_WIDGET(gtk_builder_get_object(builder, "notebook"));
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (procparTextView));
    // Set text, highlight error line and set mark.
    gtk_text_buffer_set_text (buffer, (gchar *)procparString, -1);
    gtk_text_buffer_get_iter_at_line(buffer, &startIter, badLine-1);    // lines start at 0
    gtk_text_buffer_add_mark(buffer, startMark, &startIter);
    gtk_text_buffer_get_iter_at_line(buffer, &endIter, badLine);
    gtk_text_buffer_create_tag(buffer, "highlight", "background", "green", "foreground", "white", NULL);
    gtk_text_buffer_apply_tag_by_name(buffer, "highlight", &startIter, &endIter); 
    // If error is early, show the raw file.  If error is late, show parsed procpar
    if (badLine < 35) gtk_notebook_set_current_page (GTK_NOTEBOOK(notebook),-1);
    // Switch-page signal: auto scrolls raw file to the error line
    g_signal_connect(G_OBJECT(notebook), "switch-page", G_CALLBACK(on_notebook_switch_page), procparTextView);
}

/*
 *  main
 * 
 *  This program expects one filename argument: namely a Varian procpar file
 *  The program parses the procpar into a GTK Tree View for display on a GTK widget
 *  This utilizes Gtk Builder with a Glade produced graphical interface.
 *  If the procpar file is invalid, a notebook interface is used with the helper functions above.
 */

int main(int argc, char** argv) {

    char *procparString; 
    GtkBuilder *builder;
    GtkWidget *window, *notebook, *procparScrolledWindow, *procparBox;
    GtkTreeStore *procparTreeStore;
    GtkTreeView *procparView;
    GError *error = NULL;
    
    if (argc != 2) {printf("Usage: %s filename\n", argv[0]); return 1;}

    gtk_init(&argc, &argv);
    
    procparString = readFileToString(argv[1], &error);
    if (error != NULL) {
                g_warning("%s", error->message);
                showDialog(error->message, NULL);
		g_error_free(error);
		return(1);
    }

    builder = gtk_builder_new();    // NEED A SOLUTION FOR GLADE PATH
    if (!gtk_builder_add_from_file(builder, "/public/cjr_widgets/glades/procpar.glade", &error)) {
		g_warning("%s", error->message);
                showDialog(error->message, NULL);
		g_error_free(error);
		return(1);
    }
    window = GTK_WIDGET(gtk_builder_get_object(builder, "procparWindow"));
    gtk_builder_connect_signals(builder, NULL);
    
    procparView = GTK_TREE_VIEW(gtk_builder_get_object(builder,"procparTreeView"));
    procparTreeStore = parseProcparTree(procparString, &error);
    if ((error != NULL) && (error->domain == CJR_PARSE_ERROR)) {
        g_warning("%s", error->message);
        inValidProcparWindow(builder, procparString, error->code); 
    } else { 
        notebook = GTK_WIDGET(gtk_builder_get_object(builder, "notebook"));
        procparScrolledWindow = GTK_WIDGET(gtk_builder_get_object(builder, "procparScrolledWindow"));
        procparBox = GTK_WIDGET(gtk_builder_get_object(builder, "procparBox"));
        gtk_container_remove(GTK_CONTAINER(notebook), procparScrolledWindow);
        gtk_container_remove(GTK_CONTAINER(procparBox), notebook);
        gtk_box_pack_start(GTK_BOX(procparBox), procparScrolledWindow, TRUE, TRUE, 0);
    }
    
    gtk_tree_view_set_model(procparView,GTK_TREE_MODEL(procparTreeStore));
    gtk_tree_view_set_enable_search(procparView,TRUE);
    gtk_tree_view_set_search_entry(procparView,GTK_ENTRY(gtk_builder_get_object(builder,"procparSearchEntry")));
    
    g_object_unref(G_OBJECT(builder));
    gtk_widget_show(window);

    gtk_main(); 
	
    return (EXIT_SUCCESS);
}

