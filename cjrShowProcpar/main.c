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
#include "varian.h"
#include "cjrutil.h"
#include "widgets.h"

/*
 * 
 */
void inValidProcparWindow(GtkBuilder *builder, char *procparString, int badLine);

void inValidProcparWindow(GtkBuilder *builder, char *procparString, int badLine) {
    
    GtkWidget *notebook, *nb_label_1, *nb_label_2, *buttonBox;
    GtkWidget *procparBox, *procparTextView, *procparScrolledWindow, *textScrolledWindow;
    GtkTextBuffer *buffer;
    GtkTextIter startIter, endIter;
    GtkTextMark *startMark = gtk_text_mark_new("start", TRUE);
    
    procparTextView = gtk_text_view_new();
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (procparTextView));
    gtk_text_view_set_editable(GTK_TEXT_VIEW(procparTextView), FALSE);
    gtk_text_buffer_set_text (buffer, (gchar *)procparString, -1);
    gtk_text_buffer_get_iter_at_line(buffer, &startIter, badLine-1);    // lines start at 0
    gtk_text_buffer_get_iter_at_line(buffer, &endIter, badLine);
    gtk_text_buffer_create_tag(buffer, "highlight", "background", "green", "foreground", "white", NULL);
    gtk_text_buffer_remove_tag_by_name(buffer, "highlight", &startIter, &endIter); 
    gtk_text_buffer_apply_tag_by_name(buffer, "highlight", &startIter, &endIter);
    gtk_text_buffer_add_mark(buffer,startMark, &startIter);
    gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(procparTextView), startMark,0, TRUE, 0, 0.5);
    textScrolledWindow = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_show(procparTextView);
    gtk_container_add(GTK_CONTAINER(textScrolledWindow), procparTextView);
    gtk_widget_show(textScrolledWindow);
    
    procparBox = GTK_WIDGET(gtk_builder_get_object(builder, "procparBox"));
    buttonBox = GTK_WIDGET(gtk_builder_get_object(builder, "buttonBox"));
    procparScrolledWindow = GTK_WIDGET(gtk_builder_get_object(builder, "procparScrolledWindow"));
    gtk_container_remove(GTK_CONTAINER(procparBox), procparScrolledWindow);
    //gtk_container_remove(GTK_CONTAINER(procparBox), buttonBox);
    notebook = gtk_notebook_new();
    nb_label_1 = gtk_label_new("procpar");
    nb_label_2 = gtk_label_new("invalid procpar file");
    gtk_notebook_insert_page (GTK_NOTEBOOK(notebook), procparScrolledWindow, nb_label_1, 0);
    gtk_notebook_append_page (GTK_NOTEBOOK(notebook), textScrolledWindow, nb_label_2);
    if (badLine < 35) gtk_notebook_set_current_page (GTK_NOTEBOOK(notebook),-1);
    gtk_box_pack_start(GTK_BOX(procparBox), notebook, TRUE, TRUE, 10);
    //gtk_box_pack_start(GTK_BOX(buttonBox), notebook, TRUE, TRUE, 10);
    gtk_widget_show(notebook);
}

int main(int argc, char** argv) {

    char *procparString; 
    GtkBuilder *builder;
    GtkWidget *window;
    GtkListStore *procparListStore;
    GtkTreeStore *procparTreeStore;
    GtkTreeView *procparView;
    GError *error = NULL;
    
    if (argc != 2) {printf("Usage: %s filename\n", argv[0]); return 1;}

    gtk_init(&argc, &argv);

    builder = gtk_builder_new();    // NEED A SOLUTION FOR GLADE PATH
    if (!gtk_builder_add_from_file(builder, "/home/nmrbox/mgryk/CONNJUR_spike/cjrShowProcpar/procpar.glade", &error)) 
	{
		g_warning("%s", error->message);
                showDialog(error->message, NULL);
		g_error_free(error);
		return(1);
	}
    window = GTK_WIDGET(gtk_builder_get_object(builder, "procparWindow"));
    gtk_builder_connect_signals(builder, NULL);
    
    procparView = GTK_TREE_VIEW(gtk_builder_get_object(builder,"procparTreeView"));
    procparListStore = parseProcparList(argv[1]);
    procparString = readFileToString(argv[1], &error);
    if (error != NULL) {
                g_warning("%s", error->message);
                showDialog(error->message, NULL);
		g_error_free(error);
		return(1);
    }
    procparTreeStore = parseProcparTree(procparString, &error);
    
    gtk_tree_view_set_model(procparView,GTK_TREE_MODEL(procparTreeStore));
    gtk_tree_view_set_enable_search(procparView,TRUE);
    gtk_tree_view_set_search_entry(procparView,GTK_ENTRY(gtk_builder_get_object(builder,"procparSearchEntry")));
    
    if ((error != NULL) && (error->domain == CJR_PARSE_ERROR)) {
        g_warning("%s", error->message);
        inValidProcparWindow(builder, procparString, error->code); 
    } 
    
    g_object_unref(G_OBJECT(builder));
    gtk_widget_show(window);

    gtk_main();
	
    return (EXIT_SUCCESS);
}

