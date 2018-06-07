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

void GetVarianContent(char *dir_name, gchar ***log, gchar ***text, gchar ***procpar){
    
    char *procpar_path = (char *)malloc(strlen(dir_name)+16);
    char *log_path = (char *)malloc(strlen(dir_name)+16);
    char *text_path = (char *)malloc(strlen(dir_name)+16);
    GError **err;
    struct stat varFile;
    
    sprintf(procpar_path, "%s/procpar", dir_name);
    stat(procpar_path, &varFile);
    *procpar = (gchar **)malloc(varFile.st_size); 
 //   printf("%s\n", procpar_path);
    sprintf(log_path, "%s/log", dir_name);
    stat(log_path, &varFile);
    *log = (gchar **)malloc(varFile.st_size); 
    sprintf(text_path, "%s/text", dir_name);
    stat(text_path, &varFile);
    *text = (gchar **)malloc(varFile.st_size); 
  
    if (*procpar == NULL) { printf("Malloc failed\n"); exit(2); } //should print this to stderr
    if (*log == NULL) { printf("Malloc failed\n"); exit(2); } 
    if (*text == NULL) { printf("Malloc failed\n"); exit(2); } 
    g_file_get_contents(procpar_path, *procpar, NULL, err);    
    g_file_get_contents(log_path, *log, NULL, err);
    g_file_get_contents(text_path, *text, NULL, err);
  //  printf("In Function: %s\n", **text);
    
    return;
}

int ParseProcpar(char *filename, struct _ReqVarianPar *varPar){
    FILE *procpar;
    int filesize;
    VarianPar parArray[600];    // Hardcode max number of parameters!!!
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
        /* Alternate ways of copying string from one structure to another
        if (strcmp(parArray[i].name,"seqfil")==0) sprintf(varPar->seqfil,"%s",parArray[i].value);
        if (strcmp(parArray[i].name,"seqfil")==0) strcpy(varPar->seqfil,parArray[i].value); 
        */
        if (strcmp(parArray[i].name,"date")==0) sscanf(parArray[i].value, "%s", varPar->date);
        if (strcmp(parArray[i].name,"arraydim")==0) sscanf(parArray[i].value, "%d", &varPar->arraydim);
        if (strcmp(parArray[i].name,"array")==0) sscanf(parArray[i].value, "%s", varPar->arrayedParam);
        if (strcmp(parArray[i].name,"at")==0) sscanf(parArray[i].value, "%f", &varPar->at);
        if (strcmp(parArray[i].name,"gain")==0) sscanf(parArray[i].value, "%f", &varPar->gain);
        if (strcmp(parArray[i].name,"arrayelemts")==0) sscanf(parArray[i].value, "%d", &varPar->arrayelemts);
        if (strcmp(parArray[i].name,"nt")==0) sscanf(parArray[i].value, "%d", &varPar->transients);
        if (strcmp(parArray[i].name,"rof1")==0) sscanf(parArray[i].value, "%f", &varPar->rof1);
        if (strcmp(parArray[i].name,"rof2")==0) sscanf(parArray[i].value, "%f", &varPar->rof2);
        if (strcmp(parArray[i].name,"seqfil")==0) sscanf(parArray[i].value, "%s", varPar->seqfil);
        if (strcmp(parArray[i].name,"solvent")==0) sscanf(parArray[i].value, "%s", varPar->solvent);
        if (strcmp(parArray[i].name,"temp")==0) sscanf(parArray[i].value, "%f", &varPar->temperature);
        if (strcmp(parArray[i].name,"parVersion")==0) sscanf(parArray[i].value, "%f", &varPar->parVersion);
        // channel information
        if (strcmp(parArray[i].name,"sfrq")==0)  sscanf(parArray[i].value, "%f", &varPar->freq[0]);
        if (strcmp(parArray[i].name,"dfrq")==0)  sscanf(parArray[i].value, "%f", &varPar->freq[1]);
        if (strcmp(parArray[i].name,"dfrq2")==0) sscanf(parArray[i].value, "%f", &varPar->freq[2]);
        if (strcmp(parArray[i].name,"dfrq3")==0) sscanf(parArray[i].value, "%f", &varPar->freq[3]);
        if (strcmp(parArray[i].name,"tof")==0)  sscanf(parArray[i].value, "%f", &varPar->offset[0]);
        if (strcmp(parArray[i].name,"dof")==0)  sscanf(parArray[i].value, "%f", &varPar->offset[1]);
        if (strcmp(parArray[i].name,"dof2")==0) sscanf(parArray[i].value, "%f", &varPar->offset[2]);
        if (strcmp(parArray[i].name,"dof3")==0) sscanf(parArray[i].value, "%f", &varPar->offset[3]);
        if (strcmp(parArray[i].name,"tn")==0)   sscanf(parArray[i].value, "%s", varPar->channelName[0]);
        if (strcmp(parArray[i].name,"dn")==0)   sscanf(parArray[i].value, "%s", varPar->channelName[1]);
        if (strcmp(parArray[i].name,"dn2")==0)  sscanf(parArray[i].value, "%s", varPar->channelName[2]);
        if (strcmp(parArray[i].name,"dn3")==0)  sscanf(parArray[i].value, "%s", varPar->channelName[3]);
        // Dimensions
        if (strcmp(parArray[i].name,"np")==0)   sscanf(parArray[i].value, "%d", &varPar->points[0]);
        if (strcmp(parArray[i].name,"ni")==0)   sscanf(parArray[i].value, "%d", &varPar->points[1]);
        if (strcmp(parArray[i].name,"ni2")==0)  sscanf(parArray[i].value, "%d", &varPar->points[2]);
        if (strcmp(parArray[i].name,"ni3")==0)  sscanf(parArray[i].value, "%d", &varPar->points[3]);
        if (strcmp(parArray[i].name,"sw")==0)   sscanf(parArray[i].value, "%f", &varPar->sweepWidth[0]);
        if (strcmp(parArray[i].name,"sw1")==0)  sscanf(parArray[i].value, "%f", &varPar->sweepWidth[1]);
        if (strcmp(parArray[i].name,"sw2")==0)  sscanf(parArray[i].value, "%f", &varPar->sweepWidth[2]);
        if (strcmp(parArray[i].name,"sw3")==0)  sscanf(parArray[i].value, "%f", &varPar->sweepWidth[3]);
        if (strcmp(parArray[i].name,"rp")==0)   sscanf(parArray[i].value, "%f", &varPar->constantPhase[0]);
        if (strcmp(parArray[i].name,"rp1")==0)  sscanf(parArray[i].value, "%f", &varPar->constantPhase[1]);
        if (strcmp(parArray[i].name,"rp2")==0)  sscanf(parArray[i].value, "%f", &varPar->constantPhase[2]);
        if (strcmp(parArray[i].name,"rp3")==0)  sscanf(parArray[i].value, "%f", &varPar->constantPhase[3]);
        if (strcmp(parArray[i].name,"lp")==0)   sscanf(parArray[i].value, "%f", &varPar->linearPhase[0]);
        if (strcmp(parArray[i].name,"lp1")==0)  sscanf(parArray[i].value, "%f", &varPar->linearPhase[1]);
        if (strcmp(parArray[i].name,"lp2")==0)  sscanf(parArray[i].value, "%f", &varPar->linearPhase[2]);
        if (strcmp(parArray[i].name,"lp3")==0)  sscanf(parArray[i].value, "%f", &varPar->linearPhase[3]);
        i++;
        }
    fclose(procpar);
 //   printf("%s %s\n", myPar.seqfil, varPar->seqfil);  //For Debug
    return i;
}

void BuildWindow(GtkWidget *window, ReqVarianPar varPar, char **log_string, char **text_string, char **procpar_string){
    
    GtkWidget *vbox_1;		// VBox is a vertical box for text and close button
    GtkWidget *close_button;
    GtkWidget *hbox_2;
    GtkWidget *label2;
    GtkWidget *notebook;
    GtkWidget *log_content = gtk_label_new(*log_string);
    GtkWidget *log_label = gtk_label_new("LOG");
    GtkWidget *text_label = gtk_label_new("TEXT");
    GtkWidget *procpar_label = gtk_label_new("PROCPAR");
    GtkWidget *text_content = gtk_label_new(*text_string);
    GtkWidget *procpar_content = gtk_label_new(*procpar_string);
    GtkWidget *procpar_view;
    GtkTextBuffer *procpar_buffer;
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    
    // Close Button widget.  Stock icon, quit on click.
    close_button = gtk_button_new_from_icon_name("window-close",GTK_ICON_SIZE_BUTTON);
    gtk_button_set_label(GTK_BUTTON(close_button), "Close");
    g_signal_connect(close_button, "clicked", G_CALLBACK(gtk_main_quit), NULL);
    
    //Procpar scrollable Text View
    procpar_view = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(procpar_view), GTK_WRAP_WORD);
    procpar_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(procpar_view));
    gtk_text_buffer_set_text(procpar_buffer, *procpar_string, -1);
    gtk_container_add(GTK_CONTAINER(scrolled), procpar_view);

    vbox_1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
    hbox_2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
        gtk_widget_set_halign(hbox_2, GTK_ALIGN_END);
        gtk_widget_set_valign(hbox_2, GTK_ALIGN_END);
    gtk_container_add(GTK_CONTAINER(hbox_2), close_button);
  
    notebook = gtk_notebook_new ();
    gtk_notebook_set_tab_pos (GTK_NOTEBOOK(notebook), GTK_POS_TOP);
    gtk_notebook_insert_page (GTK_NOTEBOOK(notebook), scrolled, procpar_label, 0);
    gtk_notebook_append_page (GTK_NOTEBOOK(notebook), log_content, log_label);
    gtk_notebook_append_page (GTK_NOTEBOOK(notebook), text_content, text_label);
    
    gtk_box_pack_start(GTK_BOX(vbox_1), notebook, TRUE, TRUE, 10);
    gtk_box_pack_start(GTK_BOX(vbox_1), hbox_2, FALSE, FALSE, 0); 
    gtk_container_add(GTK_CONTAINER(window), vbox_1);
    
    return;
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
        ReqVarianPar reqPar;
        ptr_ReqVarianPar = &reqPar;
        int a=100, b=200;
        
        GtkWidget *window;		// GtkWidget class for "window" object

        gchar **procpar_content, **text_content, **log_content;
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
        
        
        sprintf(procpar_path, "%s/procpar", argv[1]);
    //    sprintf(log_path, "%s/log", argv[1]);
      //  sprintf(text_path, "%s/text", argv[1]);
   //     log_content = malloc(5000);
   //     text_content = malloc(5000);
     //   procpar_content = malloc(5000);
  //      g_file_get_contents(procpar_path, procpar_content, NULL, err);
  //      g_file_get_contents(log_path, log_content, NULL, err);
  //      g_file_get_contents(text_path, text_content, NULL, err);   
        
        gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);		
	gtk_window_set_title(GTK_WINDOW(window), argv[1]);
	gtk_window_set_default_size(GTK_WINDOW(window), windowWidth, windowLength);
	gtk_container_set_border_width(GTK_CONTAINER(window), 15);
        
        numberOfPars = ParseProcpar(procpar_path, ptr_ReqVarianPar);
        printf("SEQFIL: %s\n", reqPar.seqfil);
        printf("freq[0]: %.0f\n", reqPar.freq[0]);
        printf("Total Pars in procpar: %d\n", numberOfPars);
        
        
        GetVarianContent(argv[1], &log_content, &text_content, &procpar_content);
   //     printf("Out of function: %s\n", *text_content);
        BuildWindow(window, reqPar, log_content, text_content, procpar_content);

	gtk_widget_show_all(window);				//must explicitly show window

	g_signal_connect(G_OBJECT(window), "destroy",		//must explicitly kill window
		G_CALLBACK(gtk_main_quit), NULL);

//	g_object_unref(pixBuf);					//must explicitly release pixBufs
//	g_object_unref(pixScaleBuf);

	gtk_main();						//loops until window closed
        
    return (EXIT_SUCCESS);
}

