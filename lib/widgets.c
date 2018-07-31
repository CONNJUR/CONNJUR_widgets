/*
 * Copyright (C) 2018 Michael R. Gryk
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

/* 
 * File:   widgets.c
 * Author: mgryk
 * 
 * Created on July 17, 2018, 9:36 AM
 */

#include <gtk/gtk.h>
#include "widgets.h"

void showDialog(char *string, GtkWindow *parent)
{
    // Code adapted from https://developer.gnome.org/gtk3/stable/GtkMessageDialog.html
    GtkDialog *dialog;
    GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
    
    if (parent == NULL) parent=GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
    dialog = GTK_DIALOG(gtk_message_dialog_new (parent,
                                 flags,
                                 GTK_MESSAGE_ERROR,
                                 GTK_BUTTONS_CLOSE,
                                 "%s",
                                 string));
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (GTK_WIDGET(dialog));
}
