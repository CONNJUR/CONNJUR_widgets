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
 * This program is for testing / illustrating GTK for widget design.
 * It imports a custom file-format and displays the contents along with a picture.
 * Later abandoned this approach for a "glade" design.
 *
 * Created on May 31, 2018, 3:59 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <libxml/parser.h>
#include <libxml/xmlreader.h>
#include <gtk/gtk.h>
#include "connjur.h"

GdkPixbuf *create_pixbuf(const gchar * filename) {
    
   GdkPixbuf *pixbuf;
   GError *error = NULL;
   pixbuf = gdk_pixbuf_new_from_file(filename, &error);
   
   if (!pixbuf) {
       
      fprintf(stderr, "%s\n", error->message);
      g_error_free(error);
   }

   return pixbuf;
}

/*
 * 
 */
int main(int argc, char** argv) {

    if (argc != 2) { printf("Usage: %s filename\n", argv[0]); return 1;}
	
	FILE *fp;
	int dim, i, j, row, column;
	int maxDimensions = 4;
	int maxRecords = 8;
	int maxCellLength = 32;
	int windowWidth = 400;
	int windowLength = 600;
	char records[maxRecords][maxDimensions][maxCellLength];  //array of strings for metadata table

	fp = fopen(argv[1], "r");
	fscanf(fp, "%d", &dim);
	int numrecords = 0;
	while (fscanf(fp, "%s", records[numrecords][0]) == 1) {
		for (column=1; column<(dim+1); column++) {
			fscanf(fp, "%s", records[numrecords][column]);
		}
		numrecords++;
	}
	fclose(fp);
//	for (i=1; i<=dim; i++) sprintf(id[i], "%d", i);

	gchar *pangostr;
	pangostr = "|| <b>top left</b> ||";

	GtkWidget *window;		// GtkWidget class for "window" object
	GtkWidget *vbox;		// VBox is a vertical box for text and close button
	GtkWidget *button;
	GtkWidget *halign;		//Halign is a container for the button
	GtkWidget *label, *labelb;
	GtkWidget *notebook;
	GtkWidget *table1, *table2;
	GtkWidget *cell, *cell2;
	GtkWidget *pulseImage;

	GdkPixbuf *pixBuf;
	GdkPixbuf *pixScaleBuf;
	GError 	*error = NULL;		

	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);		
	gtk_window_set_title(GTK_WINDOW(window), argv[1]);
	gtk_window_set_default_size(GTK_WINDOW(window), windowWidth, windowLength);
	gtk_container_set_border_width(GTK_CONTAINER(window), 15);

	if (dim == 2) 
	{ pixBuf = gdk_pixbuf_new_from_file("/home/nmrbox/mgryk/CONNJUR_spike/images/nhsqc.png", &error); }
	else	
	{ pixBuf = gdk_pixbuf_new_from_file("/home/nmrbox/mgryk/CONNJUR_spike/images/hnco.png", &error); }

   	if (!pixBuf) {
       
      		fprintf(stderr, "%s\n", error->message);
      		g_error_free(error);
   	}

	pixScaleBuf = gdk_pixbuf_copy(pixBuf);
	pixScaleBuf = gdk_pixbuf_scale_simple(pixBuf, 300, 180, GDK_INTERP_BILINEAR);
	pulseImage = gtk_image_new_from_pixbuf(pixScaleBuf);

	vbox = gtk_vbox_new(FALSE, 1);
	button = gtk_button_new_from_stock(GTK_STOCK_CLOSE);	
	label = gtk_label_new(NULL);
	labelb = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(label), "Dimensions");
	gtk_label_set_markup(GTK_LABEL(labelb), "Channels");
//	button = gtk_button_new_with_mnemonic("_Button");	//Mnemonic means keyboard shortcut

	g_signal_connect(button, "clicked", G_CALLBACK(gtk_main_quit), NULL);
//	g_signal_connect(button, "clicked", G_CALLBACK(print_msg), NULL);

	table1 = gtk_table_new (1,dim+1,TRUE);
	table2 = gtk_table_new (1,dim+1,TRUE);
	gtk_table_set_row_spacings(GTK_TABLE(table1), 12);
	
	for (row=0; row<numrecords; row++) {
		for (column=0; column<(dim+1); column++) {
			cell = gtk_label_new(NULL);
			cell2 = gtk_label_new(NULL);
			gtk_label_set_markup(GTK_LABEL(cell), records[row][column]);
			gtk_label_set_markup(GTK_LABEL(cell2), records[numrecords-row-1][dim-column]);
			gtk_table_attach_defaults(GTK_TABLE(table1), cell, column, column+1, row, row+1);
			gtk_table_attach_defaults(GTK_TABLE(table2), cell2, column, column+1, row, row+1);
		}
	}

	notebook = gtk_notebook_new ();
	gtk_notebook_set_tab_pos (GTK_NOTEBOOK(notebook), GTK_POS_TOP);
	gtk_notebook_insert_page (GTK_NOTEBOOK(notebook), table1, label, 0);
	gtk_notebook_append_page (GTK_NOTEBOOK(notebook), table2, labelb);

	halign = gtk_alignment_new(1, 1, 0, 0);
	gtk_container_add(GTK_CONTAINER(halign), button);
	gtk_box_pack_start(GTK_BOX(vbox), pulseImage, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 10);
	gtk_box_pack_start(GTK_BOX(vbox), halign, FALSE, FALSE, 0); 
	gtk_container_add(GTK_CONTAINER(window), vbox);

	gtk_widget_show_all(window);				//must explicitly show window


	g_signal_connect(G_OBJECT(window), "destroy",		//must explicitly kill window
		G_CALLBACK(gtk_main_quit), NULL);

	g_object_unref(pixBuf);					//must explicitly release pixBufs
	g_object_unref(pixScaleBuf);

	gtk_main();						//loops until window closed
        
    return (EXIT_SUCCESS);
}

