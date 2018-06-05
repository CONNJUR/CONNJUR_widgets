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

int ParseProcpar(char *filename){
    FILE *procpar;
    int filesize;
    VarianPar parArray[600];
    ReqVarianPar myPar;
    int i=0, j;
    char junk[100]; // Data designed to be discarded
    char c1, c2;
    
    procpar = fopen(filename, "r");
    // First find size of file;
    fseek(procpar, 0, SEEK_END);
    filesize = ftell(procpar);
    fseek(procpar, 0, SEEK_SET);
    //Parse into parArray
    while ((filesize - ftell(procpar)) > 10) {  
    // 10 is an arbitrary buffer in case extra white space in procpar
        fscanf(procpar, "%s",  parArray[i].name);
        fscanf(procpar, "%d",  &parArray[i].subtype);       
        fscanf(procpar, "%d",  &parArray[i].basictype);
        fscanf(procpar, "%lf", &parArray[i].max_value);
        fscanf(procpar, "%lf", &parArray[i].min_value);
        fscanf(procpar, "%lf", &parArray[i].step_size);
        fscanf(procpar, "%d",  &parArray[i].Ggroup);
        fscanf(procpar, "%d",  &parArray[i].Dgroup);
        fscanf(procpar, "%d",  &parArray[i].protection);
        fscanf(procpar, "%d",  &parArray[i].active);
        fscanf(procpar, "%d",  &parArray[i].intptr);            //unused
        fscanf(procpar, "%d",  &parArray[i].number_of_values);
        if (parArray[i].basictype < 2) 
        {
            for (j=0; j<parArray[i].number_of_values; j++) fscanf(procpar, "%s", parArray[i].value);
        } else {
            for (j=0; j<parArray[i].number_of_values; j++) {
                fscanf(procpar, "%[ ]", junk);
                c1 = fgetc(procpar);            // This kludge is to handle ""
                c2 = fgetc(procpar);
                if (c2 != '"') {
                    fseek(procpar,-1,SEEK_CUR);
                    fscanf(procpar, "%[^\"]\"", parArray[i].value);
                }
            }
        }
        fscanf(procpar, "%d", &parArray[i].enumerable_values);
        for (j=0; j<parArray[i].enumerable_values; j++) {
                fscanf(procpar, "%[ ]", junk);
                c1 = fgetc(procpar);            // This kludge is to handle ""
                c2 = fgetc(procpar);
                if (c2 != '"') {
                    fseek(procpar,-1,SEEK_CUR);
                    fscanf(procpar, "%[^\"]\"", junk);
                }
            }
        printf("%s: %s\n", parArray[i].name, parArray[i].value);
        if (strcmp(parArray[i].name,"seqfil")==0) strcpy(myPar.seqfil,parArray[i].value);
        i++;
        }
    fclose(procpar);
    printf("%s\n", myPar.seqfil);
    return i;
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
	int windowWidth = 400, windowLength = 600;
	char records[maxRecords][maxDimensions][maxCellLength];  //array of strings for metadata table
        char *procpar_path = (char *)malloc(strlen(argv[1])+8);  //** Need to check into extra / for directory???
        char *text_path = (char *)malloc(strlen(argv[1])+5);
        char *log_path = (char *)malloc(strlen(argv[1])+4);
        VarianPar myPar[500];
        int numberOfPars;
        
        GtkWidget *window;		// GtkWidget class for "window" object
	GtkWidget *vbox;		// VBox is a vertical box for text and close button
	GtkWidget *button;
        GtkWidget *halign;		//Halign is a container for the button
        GtkWidget *procpar_tab, *text_tab, *log_tab;
        GtkWidget *procpar_label, *text_label, *log_label;
	GtkWidget *notebook;
        GtkWidget *label1, *label2;
        GtkWidget *text_view;
        GtkTextBuffer *text_buffer;
        GtkWidget *scrolled;

        gchar *procpar_content, *text_content, *log_content;
        GError **err;

/*
 *  We expect the argv[1] to be a .fid directory containing fid, procpar, text, log and *.c files
 *      First we test that argv[1] is a directory.  Then we concatenate procpar, text etc to the directory name.
 *      Next we open each file and store the contents into the appropriate string.
 */
        struct stat buffer;
        stat(argv[1], &buffer);
  //      printf("%d\n", S_ISDIR(buffer.st_mode));
        if (!S_ISDIR(buffer.st_mode)) exit (1);
        
        strcpy(procpar_path,argv[1]);
        strcat(procpar_path, "/procpar");
        strcpy(log_path,argv[1]);
        strcat(log_path, "/log");
        strcpy(text_path,argv[1]);
        strcat(text_path, "/text");
	fp = fopen(argv[1], "r");
        g_file_get_contents(procpar_path, &procpar_content, NULL, err);
//            if (g_file_get_contents(procpar_path, &procpar_content, NULL, err)) printf("%s", procpar_content);
        fclose(fp);
        fp = fopen(log_path, "r");
        g_file_get_contents(log_path, &log_content, NULL, err);
        fclose(fp);
        fp = fopen(text_path, "r");
        g_file_get_contents(text_path, &text_content, NULL, err);
        fclose(fp);
        
        gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);		
	gtk_window_set_title(GTK_WINDOW(window), argv[1]);
	gtk_window_set_default_size(GTK_WINDOW(window), windowWidth, windowLength);
	gtk_container_set_border_width(GTK_CONTAINER(window), 15);

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
//        button = gtk_button_new_from_icon_name("window-close",GTK_ICON_SIZE_BUTTON);
	button = gtk_button_new_from_stock(GTK_STOCK_CLOSE);	
//	//button = gtk_button_new_with_mnemonic("_Button");	//Mnemonic means keyboard shortcut

	g_signal_connect(button, "clicked", G_CALLBACK(gtk_main_quit), NULL);
//	//g_signal_connect(button, "clicked", G_CALLBACK(print_msg), NULL);

        procpar_tab = gtk_label_new("procpar");
        text_tab = gtk_label_new("text");
        log_tab = gtk_label_new("log");
        log_label = gtk_label_new(log_content);
        text_label = gtk_label_new(text_content);
        procpar_label = gtk_label_new(procpar_content);
        label2 = gtk_label_new("Here is the content.");

        //gtk_notebook_append_page (GTK_NOTEBOOK(notebook), procpar_label, procpar_tab);
        
        text_view = gtk_text_view_new();
        gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD);
        text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
        gtk_text_buffer_set_text(text_buffer, procpar_content, -1);
        scrolled = gtk_scrolled_window_new(NULL, NULL);
        
        notebook = gtk_notebook_new ();
	gtk_notebook_set_tab_pos (GTK_NOTEBOOK(notebook), GTK_POS_TOP);
	gtk_notebook_insert_page (GTK_NOTEBOOK(notebook), scrolled, procpar_tab, 0);
        gtk_notebook_append_page (GTK_NOTEBOOK(notebook), log_label, log_tab);
	gtk_notebook_append_page (GTK_NOTEBOOK(notebook), text_label, text_tab);
        gtk_container_add(GTK_CONTAINER(scrolled), text_view);
        
        numberOfPars = ParseProcpar(procpar_path);
        printf("%d\n", numberOfPars);

//	halign = gtk_alignment_new(1, 1, 0, 0);
        halign = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
        gtk_widget_set_halign(halign, GTK_ALIGN_END);
        gtk_widget_set_valign(halign, GTK_ALIGN_END);
	gtk_container_add(GTK_CONTAINER(halign), button);
	gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 10);
	gtk_box_pack_start(GTK_BOX(vbox), halign, FALSE, FALSE, 0); 
	gtk_container_add(GTK_CONTAINER(window), vbox);

	gtk_widget_show_all(window);				//must explicitly show window

	g_signal_connect(G_OBJECT(window), "destroy",		//must explicitly kill window
		G_CALLBACK(gtk_main_quit), NULL);

//	g_object_unref(pixBuf);					//must explicitly release pixBufs
//	g_object_unref(pixScaleBuf);

	gtk_main();						//loops until window closed
        
    return (EXIT_SUCCESS);
}

