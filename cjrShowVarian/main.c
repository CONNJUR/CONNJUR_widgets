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
#include <math.h>
#include "connjur.h"

/*
 * 
 */
int main(int argc, char** argv) {

    if (argc != 2) {printf("Usage: %s filename\n", argv[0]); return 1;}
    
    GtkBuilder *builder;
    GError *error = NULL;

    gtk_init(&argc, &argv);

    builder = gtk_builder_new();
	
    if (!gtk_builder_add_from_file(builder, "/home/nmrbox/mgryk/CONNJUR_spike/cjrShowVarian/varian.glade", &error)) 
	{
		g_warning("%s", error->message);
		g_free(error);
		return(1);
	}

    GtkWidget *window;
    window = GTK_WIDGET(gtk_builder_get_object(builder, "varianWindow"));

    gtk_builder_connect_signals(builder, NULL);
    
    GtkListStore *procparStore;
    GtkTreeView *procparView;
    procparStore = parseProcpar(concatStrings(argv[1],"/procpar"));
    procparView = GTK_TREE_VIEW(gtk_builder_get_object(builder,"procparTreeView"));
    gtk_tree_view_set_model(procparView,GTK_TREE_MODEL(procparStore));
    gtk_tree_view_set_enable_search(procparView,TRUE);
    gtk_tree_view_set_search_entry(procparView,GTK_ENTRY(gtk_builder_get_object(builder,"procparSearchEntry")));
    
    buildLogWindow(builder, concatStrings(argv[1],"/log"));
    
    ReqVarianPar varianPars;
    varianPars = extractVarPar(procparStore);
    guessDataDims(&varianPars);
    guessArrayDims(&varianPars);
    guessPSDims(&varianPars);
    setMaxDimensions(&varianPars);
    varianPars.fidSize = getFidSize(concatStrings(argv[1],"/fid"));
    if (buildSamplingWindow(builder,concatStrings(argv[1],"/sampling.sch"))) varianPars.sampling = 0;
    
    buildMappingWindow(builder, varianPars);
    
    AtomColor myColor = getAtomColors(varianPars.seqfil);
    AtomColor *myColorPtr;
    myColorPtr = &myColor;
    
    // Drawing code for protein backbone
    GtkWidget *drawArea;
    drawArea = GTK_WIDGET(gtk_builder_get_object(builder,"drawingarea1"));
    g_signal_connect (G_OBJECT(drawArea), "draw", G_CALLBACK(backbone_callback), myColorPtr);
    g_signal_connect (G_OBJECT(gtk_builder_get_object(builder,"cst_button")),"clicked", G_CALLBACK(nbx_button_clicked), argv[1]);
    
    g_object_unref(G_OBJECT(builder));

    gtk_widget_show(window);

    gtk_main();
	
    return (EXIT_SUCCESS);
}

