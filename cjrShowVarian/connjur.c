/*
 * Copyright (C) 2018 Michael R. Gryk
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <math.h>
#include "connjur.h" 

const int FID_HEADER = 32;
const int BLOCK_HEADER = 28;

const GdkRGBA Color0 = {.red = 1.000, .blue = 1.000, .green = 1.000 }; //white
const GdkRGBA Color1 = {.red = 0.852, .blue = 0.906, .green = 0.984 }; //greenish
const GdkRGBA Color2 = {.red = 0.855, .blue = 0.988, .green = 0.910 }; //blueish
const GdkRGBA Color3 = {.red = 0.957, .blue = 0.806, .green = 0.804 }; //reddish
const GdkRGBA Color4 = {.red = 1.000, .blue = 0.800, .green = 0.949 }; //yellowish
const GdkRGBA ColorB = {.red = 0.000, .blue = 0.000, .green = 0.000 }; //black

void getColors(GdkRGBA **colorArray, char *pulseSequence)
{
    *colorArray = malloc(sizeof(GdkRGBA)*160);
    *colorArray[0] = Color2;
   // *colorArray[1] = Color3; //*colorArray[1] = Color2;// *colorArray[2] = Color3; *colorArray[3] = Color0; 
    //printf("%lf", *colorArray[0].red);
    
}

char *concatStrings(char *string1, char *string2) 
{ 
    char *catStrings = malloc(strlen(string1)+strlen(string2)+1);
    if (catStrings == NULL) {
        fprintf(stderr, "concatStrings: malloc failed\n");
        exit(1);
    }
    strcpy(catStrings,string1);
    strcat(catStrings,string2);
    return(catStrings);
}

GtkListStore *parseProcpar(char *filename)
{
    GtkListStore *store = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT);
    FILE *procpar;
    int filesize;
    VarianPar parArray[1000];    // Hardcode max number of parameters!!!
    int i=0, j;
    char junk[10000]; // Metadata intended to be discarded
    char c1, c2;
    
   // gtk_list_store_insert_with_values(store, NULL, -1, 0, "ni", 1, "128", -1);
   // gtk_list_store_insert_with_values(store, NULL, -1, 0, "sfrq", 1, "599.56", -1);
    
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
  //      if (parArray[i].number_of_values > 1) printf("%s: %d\n", parArray[i].name, parArray[i].number_of_values);
        if (parArray[i].basictype < 2) 
        {
            // This fscanf command reads multivalue parameters into one field
            for (j=0; j<parArray[i].number_of_values; j++) fscanf(procpar, "%[^\n]", parArray[i].value);
        } else {
            for (j=0; j<parArray[i].number_of_values; j++) {
                fscanf(procpar, "%[\n ]", junk);  //This line skips spaces and newlines
                c1 = fgetc(procpar);            // This kludge is to handle consecutive double quotes.
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
        if ((strcmp(parArray[i].name,"ap")!=0) && (strncmp(parArray[i].name,"dg",2)!=0) ) 
            gtk_list_store_insert_with_values(store, NULL, -1, 0, parArray[i].name, 1, parArray[i].value, 
                                                2, parArray[i].basictype, 3, parArray[i].number_of_values, -1);
 //       gtk_list_store_insert_with_values(store, NULL, -1, 0, parArray[i].name, 1, parArray[i].value, -1);
        i++;
        }
    
    fclose(procpar);
    
    return (store);
}

ReqVarianPar extractVarPar(GtkListStore *VarianList)
{
    ReqVarianPar parameters = { .channelName[0] = "X", .channelName[1] = "X", .channelName[2] = "X", .channelName[3] = "X",
                                .arrayedParam = "NONE", .seqfil = "NONE", .solvent = "NONE", .sampling = 1 };
    GtkTreeIter iter;
    gboolean valid;
    gchar *param, *value;
    
    valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(VarianList), &iter);
    while (valid){
        gtk_tree_model_get(GTK_TREE_MODEL(VarianList), &iter, 0, &param, 1, &value, -1);
        if (strcmp(param,"np")==0) sscanf(value,"%d", &parameters.points[0]);
        if (strcmp(param,"ni")==0) sscanf(value,"%d", &parameters.points[1]);
        if (strcmp(param,"ni2")==0) sscanf(value,"%d", &parameters.points[2]);
        if (strcmp(param,"ni3")==0) sscanf(value,"%d", &parameters.points[3]);
        if (strcmp(param,"sw")==0) sscanf(value,"%f", &parameters.sweepWidth[0]);
        if (strcmp(param,"sw1")==0) sscanf(value,"%f", &parameters.sweepWidth[1]);
        if (strcmp(param,"sw2")==0) sscanf(value,"%f", &parameters.sweepWidth[2]);
        if (strcmp(param,"sw3")==0) sscanf(value,"%f", &parameters.sweepWidth[3]);
        if (strcmp(param,"sfrq")==0) sscanf(value,"%f", &parameters.freq[0]);
        if (strcmp(param,"dfrq")==0) sscanf(value,"%f", &parameters.freq[1]);
        if (strcmp(param,"dfrq2")==0) sscanf(value,"%f", &parameters.freq[2]);
        if (strcmp(param,"dfrq3")==0) sscanf(value,"%f", &parameters.freq[3]);
        if (strcmp(param,"tn")==0) sscanf(value,"%s", parameters.channelName[0]);
        if (strcmp(param,"dn")==0) sscanf(value,"%s", parameters.channelName[1]);
        if (strcmp(param,"dn2")==0) sscanf(value,"%s", parameters.channelName[2]);
        if (strcmp(param,"dn3")==0) sscanf(value,"%s", parameters.channelName[3]);
        if (strcmp(param,"seqfil")==0) sscanf(value,"%s", parameters.seqfil);
        if (strcmp(param,"solvent")==0) sscanf(value,"%s", parameters.solvent);
        if (strcmp(param,"array")==0) sscanf(value,"%s", parameters.arrayedParam);
        if ((strcmp(param,"TROSY")==0) && strcmp(value,"y") == 0) parameters.TROSY = 1;
        valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(VarianList), &iter);
    }
    return (parameters);
}

void guessDataDims(struct _ReqVarianPar *varPar)
{
    int i=0;
    
    if (varPar->points[0] > 1) i++;
    if (varPar->points[1] > 1) i++;
    if (varPar->points[2] > 1) i++;
    if (varPar->points[3] > 1) i++;
    
    varPar->dataDims = i;
   // printf("%d\n Data Dimensions", varPar->dimensions);
}

void guessPSDims(struct _ReqVarianPar *varPar)
{
    if (strcmp(varPar->seqfil, "ghn_co")==0) varPar->psDims = 3;
    if (strcmp(varPar->seqfil, "gCNhsqc")==0) varPar->psDims = 3;
    if (strcmp(varPar->seqfil, "ghn_cacb")==0) varPar->psDims = 3;
    if (strcmp(varPar->seqfil, "gNhsqc")==0) varPar->psDims = 2;
    if (strcmp(varPar->seqfil, "mgNhsqc")==0) varPar->psDims = 2;
    if (strcmp(varPar->seqfil, "gnoesyNhsqc")==0) varPar->psDims = 3;
}

void guessArrayDims(struct _ReqVarianPar *varPar)
{
    int i, dimCount=1;  // By default, dimCount is 1
    if (strlen(varPar->arrayedParam) > 1) dimCount = 2; //if array is not empty, at least two dimensions
    for (i=0; i<strlen(varPar->arrayedParam); i++) if (varPar->arrayedParam[i]==',') dimCount++; //extra dim per comma
    varPar->arrayDims = dimCount;
   // printf("%d array dimensions\n", varPar->arrayDims);
}

void setMaxDimensions(struct _ReqVarianPar *varPar)
{
    varPar->dimensions = varPar->dataDims;
    if (varPar->psDims > varPar->dimensions) varPar->dimensions = varPar->psDims;
    if (varPar->arrayDims > varPar->dimensions) varPar->dimensions = varPar->arrayDims;
}

int oldParseProcpar(char *filename, struct _ReqVarianPar *varPar, GtkListStore **list){
    FILE *procpar;
    int filesize;
    VarianPar parArray[600];    // Hardcode max number of parameters!!!
    int i=0, j;
    char junk[10000]; // Data designed to be discarded
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
                fscanf(procpar, "%[\n ]", junk);  //This line skips spaces and newlines
                c1 = fgetc(procpar);            // This kludge is to handle consecutive double quotes.
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
        if ((strcmp(parArray[i].name,"ap")!=0) && (strncmp(parArray[i].name,"dg",2)!=0) ) gtk_list_store_insert_with_values(*list, NULL, -1, 0, parArray[i].name, 1, parArray[i].value, -1);
        /* Alternate ways of copying string from one structure to another
        if (strcmp(parArray[i].name,"seqfil")==0) sprintf(varPar->seqfil,"%s",parArray[i].value);
        if (strcmp(parArray[i].name,"seqfil")==0) strcpy(varPar->seqfil,parArray[i].value); 
        */
        if (strcmp(parArray[i].name,"date")==0) strcpy(varPar->date,parArray[i].value); 
        if (strcmp(parArray[i].name,"arraydim")==0) sscanf(parArray[i].value, "%d", &varPar->arraydim);
        if (strcmp(parArray[i].name,"array")==0) strcpy(varPar->arrayedParam,parArray[i].value); 
        if (strcmp(parArray[i].name,"at")==0) sscanf(parArray[i].value, "%f", &varPar->at);
        if (strcmp(parArray[i].name,"gain")==0) sscanf(parArray[i].value, "%f", &varPar->gain);
        if (strcmp(parArray[i].name,"arrayelemts")==0) sscanf(parArray[i].value, "%d", &varPar->arrayelemts);
        if (strcmp(parArray[i].name,"nt")==0) sscanf(parArray[i].value, "%d", &varPar->transients);
        if (strcmp(parArray[i].name,"rof1")==0) sscanf(parArray[i].value, "%f", &varPar->rof1);
        if (strcmp(parArray[i].name,"rof2")==0) sscanf(parArray[i].value, "%f", &varPar->rof2);
        if (strcmp(parArray[i].name,"seqfil")==0) strcpy(varPar->seqfil,parArray[i].value); 
        if (strcmp(parArray[i].name,"solvent")==0) strcpy(varPar->solvent,parArray[i].value); 
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

int getFidSize(char *fid_path)
{
    struct stat myFidFile;
    GError *error;
    int fidSize;
    
    if (stat(fid_path, &myFidFile) != 0) fidSize=-1; else fidSize=myFidFile.st_size;
    //printf("Fid %d\n", fidSize);
    return (fidSize);
}

int buildSamplingWindow(GtkBuilder *builder, char *path)
{
    struct stat sampleSchedule;
    gchar **scheduleContent;
    GError *error;
    GtkNotebook *windowNotebook;
    
    if (stat(path, &sampleSchedule) != 0) {
        windowNotebook = GTK_NOTEBOOK(gtk_builder_get_object(builder,"windowNotebook"));
        gtk_notebook_remove_page(windowNotebook, -1);
        return 0;
    }
    scheduleContent = (gchar **)malloc(sampleSchedule.st_size); 
    g_file_get_contents(path, scheduleContent, NULL, &error);
    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,"samplingContentLabel")), *scheduleContent);
    return 1;
}

void buildLogWindow(GtkBuilder *builder, char *logpath) 
{
    struct stat myLogFile;
    gchar **logContent;
    GError *error;
    
    stat(logpath, &myLogFile);
    logContent = (gchar **)malloc(myLogFile.st_size); 
    g_file_get_contents(logpath, logContent, NULL, &error);
    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,"logContentLabel")), *logContent);
}

void buildMappingWindow(GtkBuilder *builder, ReqVarianPar varPar)
{
    int dimensions = 3;
    int i;
    GtkGrid *axisGrid;
    GtkComboBoxText *myTextCombo0, *myTextCombo1, *myTextCombo2;
    char tempString[128];
    char expString[2048];
    
    axisGrid = GTK_GRID(gtk_builder_get_object(builder,"grid1"));
    for (i=3; i>(varPar.dataDims-1); i--) gtk_grid_remove_row(axisGrid,i);
    
    sprintf(tempString,"%d points", varPar.points[0]);
    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,"label0_0")), tempString);
    sprintf(tempString,"%.2f Hertz", varPar.sweepWidth[0]);
    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,"label0_1")), tempString);
    sprintf(tempString,"%d points", varPar.points[1]);
    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,"label1_0")),tempString);
    sprintf(tempString,"%.2f Hertz", varPar.sweepWidth[1]);
    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,"label1_1")), tempString);
    sprintf(tempString,"%d points", varPar.points[2]);
    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,"label2_0")),tempString);
    sprintf(tempString,"%.2f Hertz", varPar.sweepWidth[2]);
    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,"label2_1")), tempString);
    myTextCombo0 = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder,"comboboxtext0"));
    myTextCombo1 = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder,"comboboxtext1"));
    myTextCombo2 = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder,"comboboxtext2"));
    sprintf(tempString,"%s: %.2f MHz", varPar.channelName[0], varPar.freq[0]);
    gtk_combo_box_text_append(myTextCombo0,varPar.channelName[0],tempString);
    gtk_combo_box_text_append(myTextCombo1,varPar.channelName[0],tempString);
    gtk_combo_box_text_append(myTextCombo2,varPar.channelName[0],tempString);
    sprintf(tempString,"%s: %.2f MHz", varPar.channelName[1], varPar.freq[1]);
    gtk_combo_box_text_append(myTextCombo0,varPar.channelName[1],tempString);
    gtk_combo_box_text_append(myTextCombo1,varPar.channelName[1],tempString);
    gtk_combo_box_text_append(myTextCombo2,varPar.channelName[1],tempString);
    sprintf(tempString,"%s: %.2f MHz", varPar.channelName[2], varPar.freq[2]);
    gtk_combo_box_text_append(myTextCombo0,varPar.channelName[2],tempString);
    gtk_combo_box_text_append(myTextCombo1,varPar.channelName[2],tempString);
    gtk_combo_box_text_append(myTextCombo2,varPar.channelName[2],tempString);
    sprintf(tempString,"%s: %.2f MHz", varPar.channelName[3], varPar.freq[3]);
    gtk_combo_box_text_append(myTextCombo0,varPar.channelName[3],tempString);
    gtk_combo_box_text_append(myTextCombo1,varPar.channelName[3],tempString);
    gtk_combo_box_text_append(myTextCombo2,varPar.channelName[3],tempString); 
    gtk_combo_box_set_active_id(GTK_COMBO_BOX(myTextCombo0),varPar.channelName[0]);  //sets top combo box to 1H
    gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(builder,"cb0_0")), varPar.channelName[0]);
    gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(builder,"cb0_1")), varPar.channelName[1]);
    gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(builder,"cb0_2")), varPar.channelName[2]);
    gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(builder,"cb0_3")), varPar.channelName[3]);
    gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(builder,"cb1_0")), varPar.channelName[0]);
    gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(builder,"cb1_1")), varPar.channelName[1]);
    gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(builder,"cb1_2")), varPar.channelName[2]);
    gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(builder,"cb1_3")), varPar.channelName[3]);
    gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(builder,"cb2_0")), varPar.channelName[0]);
    gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(builder,"cb2_1")), varPar.channelName[1]);
    gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(builder,"cb2_2")), varPar.channelName[2]);
    gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(builder,"cb2_3")), varPar.channelName[3]);
    gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(builder,"cb3_0")), varPar.channelName[0]);
    gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(builder,"cb3_1")), varPar.channelName[1]);
    gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(builder,"cb3_2")), varPar.channelName[2]);
    gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(builder,"cb3_3")), varPar.channelName[3]);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"cb0_0")), varPar.channelName[0]); //checks tn
    // Build Experimental Description String
    memset(expString, '\0', sizeof(expString));
    if (!varPar.sampling) strcat(expString, "non-");
    strcat(expString, "uniformly sampled ");
    if (varPar.arrayDims > varPar.dataDims) strcat(expString, "pseudo-");
    sprintf(tempString, "%dD ", varPar.dimensions);
    strcat(expString, tempString);
    if (varPar.TROSY) strcat(expString, "TROSY ");
    sprintf(tempString, "%s experiment\n", varPar.seqfil);
    strcat(expString, tempString);
    sprintf(tempString, "FID file size: %d Bytes\n", varPar.fidSize);
    strcat(expString, tempString);
    
    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,"expLabel")), expString);
    
    if (varPar.psDims > varPar.dataDims) {
        gtk_grid_remove_column(axisGrid,2);
    } else {
        gtk_grid_remove_column(axisGrid,6);
        gtk_grid_remove_column(axisGrid,5);
        gtk_grid_remove_column(axisGrid,4);
        gtk_grid_remove_column(axisGrid,3);
    }
}

gboolean draw_circle (cairo_t *cr, char *text, int x, int y, int r, GdkRGBA color)
{   
    cairo_new_path(cr);
    cairo_set_line_width(cr,0.6);
    cairo_arc(cr,x,y,r,0,2*G_PI);
    cairo_close_path(cr);
    if (&color != NULL) cairo_set_source_rgb(cr, color.red, color.green, color.blue);
    cairo_fill_preserve(cr);
    cairo_set_source_rgb(cr,0,0,0);
    cairo_stroke(cr);
    cairo_move_to(cr, x-6*((int) strlen(text)), y+5);
    cairo_set_font_size(cr,16);
    cairo_show_text(cr,text);
}

AtomColor getAtomColors(char *pulseSequence)
{
    AtomColor myColors = {.hni=0, .ni=0, .coi=0, .cai=0, .hai=0, .cbi=0, .hb2i=0, .hb3i=0, .cgi=0, .hg2i=0, .hg3i=0,
                                 .np=0, .cop=0, .cap=0, .hap=0, .cbp=0, .hb2p=0, .hb3p=0, .cgp=0, .hg2p=0, .hg3p=0 };
    if (strcmp(pulseSequence, "ghn_co")==0) { myColors.hni=1; myColors.ni=2; myColors.cop=3; }
    if (strcmp(pulseSequence, "ghn_co_ca")==0) { myColors.hni=1; myColors.ni=2; myColors.cap=3; }
    if (strcmp(pulseSequence, "gCNhsqc")==0) { myColors.hni=1; myColors.ni=2; myColors.hai=1; myColors.hap=1; 
            myColors.cai=3; myColors.cap=3; myColors.cbi=3; myColors.cbp=3; myColors.cgi=3; myColors.cgp=3;
            myColors.hb2i=1; myColors.hb2p=1; myColors.hg2i=1; myColors.hg2p=1;
            myColors.hb3i=1; myColors.hb3p=1; myColors.hg3i=1; myColors.hg3p=1;}
    if (strcmp(pulseSequence, "ghn_cacb")==0) { myColors.hni=1; myColors.ni=2; 
            myColors.cai=3; myColors.cap=3; myColors.cbi=3; myColors.cbp=3; }
    if (strcmp(pulseSequence, "ghn_ca_co")==0) { myColors.hni=1; myColors.ni=2; myColors.coi=3; myColors.cop=3; }
    if (strcmp(pulseSequence, "gNhsqc")==0) { myColors.hni=1; myColors.ni=2; }
    if (strcmp(pulseSequence, "mgNhsqc")==0) { myColors.hni=1; myColors.ni=2; }
    if (strcmp(pulseSequence, "gnoesyNhsqc")==0) { myColors.hni=1; myColors.ni=2; myColors.hai=4; myColors.hap=4; 
            myColors.hb2i=4; myColors.hb2p=4; myColors.hg2i=4; myColors.hg2p=4;
            myColors.hb3i=4; myColors.hb3p=4; myColors.hg3i=4; myColors.hg3p=4;}
    return (myColors);
}

gboolean backbone_callback (GtkWidget *widget, cairo_t *cr, gpointer data)
{
    typedef struct _atom {
           int x_coord; 
           int y_coord; 
           char element[2];
           char type[8];
    } atom;
    
    atom hni, ni, cai, coi, cbi, hai, hb2i, hb3i, cgi, hg2i, hg3i;
    atom hnp, np, cap, cop, cbp, hap, hb2p, hb3p, cgp, hg2p, hg3p;
    int h_atom_rad = 20;
    int x_atom_rad = 30;
    int h_x_length = h_atom_rad+x_atom_rad+10;
    int x_x_length = 2*x_atom_rad+15;
    double hn_n_angle = 45*G_PI/180;
    double co_n_angle = 45*G_PI/180;
    double co_ca_angle = 22.5*G_PI/180;
    double hn_c_angle = 35*G_PI/180;
    double ca_n_angle = 22.5*G_PI/180;
    
    
    //AtomColor myColor = {.hni=1, .ni=2, .coi=3, .cai=0, .hai=0, .cbi=0, .hb2i=0, .hb3i=0, .cgi=0, .hg2i=0, .hg3i=0,
    //                             .np=0, .cop=3, .cap=0, .hap=0, .cbp=0, .hb2p=0, .hb3p=0, .cgp=0, .hg2p=0, .hg3p=0 };
    
    //Define Labels for atoms
    sprintf(hni.element,"H");
    sprintf(hni.type,"N");
    sprintf(ni.element,"N");
    ni.type[0] = '\0';
    sprintf(cop.element,"C");
    sprintf(cop.type,"O");
    sprintf(cap.element,"C");
    sprintf(cap.type,"A");
    sprintf(hap.element,"H");
    sprintf(hap.type,"A");
    sprintf(np.element,"N");
    np.type[0] = '\0';
    sprintf(cbp.element,"C");
    sprintf(cbp.type,"B");
    sprintf(hb2p.element,"H");
    sprintf(hb2p.type,"B2");
    sprintf(hb3p.element,"H");
    sprintf(hb3p.type,"B3");
    sprintf(cgp.element,"C");
    sprintf(cgp.type,"G");
    sprintf(hg2p.element,"H");
    sprintf(hg2p.type,"G2");
    sprintf(hg3p.element,"H");
    sprintf(hg3p.type,"G3");
    sprintf(cai.element,"C");
    sprintf(cai.type,"A");
    sprintf(hai.element,"H");
    sprintf(hai.type,"A");
    sprintf(coi.element,"C");
    sprintf(coi.type,"O");
    sprintf(cbi.element,"C");
    sprintf(cbi.type,"B");
    sprintf(hb2i.element,"H");
    sprintf(hb2i.type,"B2");
    sprintf(hb3i.element,"H");
    sprintf(hb3i.type,"B3");
    sprintf(cgi.element,"C");
    sprintf(cgi.type,"G");
    sprintf(hg2i.element,"H");
    sprintf(hg2i.type,"G2");
    sprintf(hg3i.element,"H");
    sprintf(hg3i.type,"G3");
    
    //Calculate coordinates for atoms relative to HN(i) in the middle
    hni.x_coord=0;
    hni.y_coord=0;
    ni.x_coord = hni.x_coord + floor(h_x_length*cos((double) hn_n_angle));
    ni.y_coord = hni.y_coord + floor(h_x_length*sin((double) hn_n_angle));
    cop.x_coord = ni.x_coord - floor(x_x_length*cos((double) co_n_angle));
    cop.y_coord = ni.y_coord + floor(x_x_length*sin((double) co_n_angle));
    cap.x_coord = cop.x_coord - floor(x_x_length*cos((double) co_ca_angle));
    cap.y_coord = cop.y_coord - floor(x_x_length*sin((double) co_ca_angle));
    hap.x_coord = cap.x_coord - floor(h_x_length*cos((double) hn_c_angle));
    hap.y_coord = cap.y_coord + floor(h_x_length*sin((double) hn_c_angle));
    np.x_coord = cap.x_coord - floor(x_x_length*cos((double) ca_n_angle));
    np.y_coord = cap.y_coord - floor(x_x_length*sin((double) ca_n_angle));
    cbp.x_coord = cap.x_coord - floor(x_x_length*sin((double) 0));
    cbp.y_coord = cap.y_coord - floor(x_x_length*cos((double) 0));
    hb2p.x_coord = cbp.x_coord - floor(h_x_length*cos((double) hn_c_angle));
    hb2p.y_coord = cbp.y_coord - floor(h_x_length*sin((double) hn_c_angle));
    hb3p.x_coord = cbp.x_coord + floor(h_x_length*cos((double) hn_c_angle));
    hb3p.y_coord = cbp.y_coord - floor(h_x_length*sin((double) hn_c_angle));
    cgp.x_coord = cbp.x_coord - floor(x_x_length*sin((double) 0));
    cgp.y_coord = cbp.y_coord - floor(x_x_length*cos((double) 0));
    hg2p.x_coord = cgp.x_coord - floor(h_x_length*cos((double) hn_c_angle));
    hg2p.y_coord = cgp.y_coord - floor(h_x_length*sin((double) hn_c_angle));
    hg3p.x_coord = cgp.x_coord + floor(h_x_length*cos((double) hn_c_angle));
    hg3p.y_coord = cgp.y_coord - floor(h_x_length*sin((double) hn_c_angle));
    cai.x_coord = ni.x_coord + floor(x_x_length*cos((double) ca_n_angle));
    cai.y_coord = ni.y_coord + floor(x_x_length*sin((double) ca_n_angle));
    coi.x_coord = cai.x_coord + floor(x_x_length*cos((double) co_ca_angle));
    coi.y_coord = cai.y_coord + floor(x_x_length*sin((double) co_ca_angle));
    hai.x_coord = cai.x_coord - floor(h_x_length*cos((double) hn_c_angle));
    hai.y_coord = cai.y_coord + floor(h_x_length*sin((double) hn_c_angle));
    
    
    cbi.x_coord = cai.x_coord - floor(x_x_length*sin((double) 0));
    cbi.y_coord = cai.y_coord - floor(x_x_length*cos((double) 0));
    hb2i.x_coord = cbi.x_coord - floor(h_x_length*cos((double) hn_c_angle));
    hb2i.y_coord = cbi.y_coord - floor(h_x_length*sin((double) hn_c_angle));
    hb3i.x_coord = cbi.x_coord + floor(h_x_length*cos((double) hn_c_angle));
    hb3i.y_coord = cbi.y_coord - floor(h_x_length*sin((double) hn_c_angle));
    cgi.x_coord = cbi.x_coord - floor(x_x_length*sin((double) 0));
    cgi.y_coord = cbi.y_coord - floor(x_x_length*cos((double) 0));
    hg2i.x_coord = cgi.x_coord - floor(h_x_length*cos((double) hn_c_angle));
    hg2i.y_coord = cgi.y_coord - floor(h_x_length*sin((double) hn_c_angle));
    hg3i.x_coord = cgi.x_coord + floor(h_x_length*cos((double) hn_c_angle));
    hg3i.y_coord = cgi.y_coord - floor(h_x_length*sin((double) hn_c_angle));
    
    //Translate frame to center of drawing area
    cairo_translate(cr,190,150);
    
    //Draw bond scaffold
    cairo_move_to (cr, cgp.x_coord, cgp.y_coord);
    cairo_line_to (cr, hg2p.x_coord, hg2p.y_coord);
    cairo_move_to (cr, cgp.x_coord, cgp.y_coord);
    cairo_line_to (cr, hg3p.x_coord, hg3p.y_coord);
    cairo_move_to (cr, cgp.x_coord, cgp.y_coord);
    cairo_line_to (cr, cbp.x_coord, cbp.y_coord);
    cairo_line_to (cr, hb2p.x_coord, hb2p.y_coord);
    cairo_move_to (cr, cbp.x_coord, cbp.y_coord);
    cairo_line_to (cr, hb3p.x_coord, hb3p.y_coord);
    cairo_move_to (cr, cbp.x_coord, cbp.y_coord);
    cairo_line_to (cr, cap.x_coord, cap.y_coord);
    cairo_line_to (cr, np.x_coord, np.y_coord);
    cairo_move_to (cr, cap.x_coord, cap.y_coord);
    cairo_line_to (cr, hap.x_coord, hap.y_coord);
    cairo_move_to (cr, cap.x_coord, cap.y_coord);
    cairo_line_to (cr, cop.x_coord, cop.y_coord);
    cairo_line_to (cr, ni.x_coord, ni.y_coord);
    cairo_line_to (cr, hni.x_coord, hni.y_coord);
    cairo_move_to (cr, ni.x_coord, ni.y_coord);
    cairo_line_to (cr, cai.x_coord, cai.y_coord);
    cairo_line_to (cr, coi.x_coord, coi.y_coord);
    cairo_move_to (cr, cgi.x_coord, cgi.y_coord);
    cairo_line_to (cr, hg2i.x_coord, hg2i.y_coord);
    cairo_move_to (cr, cgi.x_coord, cgi.y_coord);
    cairo_line_to (cr, hg3i.x_coord, hg3i.y_coord);
    cairo_move_to (cr, cgi.x_coord, cgi.y_coord);
    cairo_line_to (cr, cbi.x_coord, cbi.y_coord);
    cairo_line_to (cr, hb2i.x_coord, hb2i.y_coord);
    cairo_move_to (cr, cbi.x_coord, cbi.y_coord);
    cairo_line_to (cr, hb3i.x_coord, hb3i.y_coord);
    cairo_move_to (cr, cbi.x_coord, cbi.y_coord);
    cairo_line_to (cr, cai.x_coord, cai.y_coord);
    cairo_move_to (cr, cai.x_coord, cai.y_coord);
    cairo_line_to (cr, hai.x_coord, hai.y_coord);
    cairo_stroke(cr);
    
    GdkRGBA Color[] = { Color0, Color1, Color2, Color3, Color4, ColorB };
    AtomColor *myColor;
    myColor = data;
    //Draw atoms
    GdkRGBA *colors = data;
    draw_circle(cr,concatStrings(hni.element, hni.type), hni.x_coord, hni.y_coord, h_atom_rad, Color[myColor->hni]);
    draw_circle(cr,concatStrings(ni.element, ni.type), ni.x_coord, ni.y_coord, x_atom_rad, Color[myColor->ni]);
    draw_circle(cr,concatStrings(cop.element, cop.type), cop.x_coord, cop.y_coord, x_atom_rad, Color[myColor->cop]);
    draw_circle(cr,concatStrings(cap.element, cap.type), cap.x_coord, cap.y_coord, x_atom_rad, Color[myColor->cap]);
    draw_circle(cr,concatStrings(hap.element, hap.type), hap.x_coord, hap.y_coord, h_atom_rad, Color[myColor->hap]);
    draw_circle(cr,concatStrings(np.element, np.type), np.x_coord, np.y_coord, x_atom_rad, Color[myColor->np]);
    draw_circle(cr,concatStrings(cbp.element, cbp.type), cbp.x_coord, cbp.y_coord, x_atom_rad, Color[myColor->cbp]);
    draw_circle(cr,concatStrings(hb2p.element, hb2p.type), hb2p.x_coord, hb2p.y_coord, h_atom_rad, Color[myColor->hb2p]);
    draw_circle(cr,concatStrings(hb3p.element, hb3p.type), hb3p.x_coord, hb3p.y_coord, h_atom_rad, Color[myColor->hb3p]);
    draw_circle(cr,concatStrings(cgp.element, cgp.type), cgp.x_coord, cgp.y_coord, x_atom_rad, Color[myColor->cgp]);
    draw_circle(cr,concatStrings(hg2p.element, hg2p.type), hg2p.x_coord, hg2p.y_coord, h_atom_rad, Color[myColor->hg2p]);
    draw_circle(cr,concatStrings(hg3p.element, hg3p.type), hg3p.x_coord, hg3p.y_coord, h_atom_rad, Color[myColor->hg3p]);
    draw_circle(cr,concatStrings(cai.element, cai.type), cai.x_coord, cai.y_coord, x_atom_rad, Color[myColor->cai]);
    draw_circle(cr,concatStrings(hai.element, hai.type), hai.x_coord, hai.y_coord, h_atom_rad, Color[myColor->hai]);
    draw_circle(cr,concatStrings(coi.element, coi.type), coi.x_coord, coi.y_coord, x_atom_rad, Color[myColor->coi]);
    draw_circle(cr,concatStrings(cbi.element, cbi.type), cbi.x_coord, cbi.y_coord, x_atom_rad, Color[myColor->cbi]);
    draw_circle(cr,concatStrings(hb2i.element, hb2i.type), hb2i.x_coord, hb2i.y_coord, h_atom_rad, Color[myColor->hb2i]);
    draw_circle(cr,concatStrings(hb3i.element, hb3i.type), hb3i.x_coord, hb3i.y_coord, h_atom_rad, Color[myColor->hb3i]);
    draw_circle(cr,concatStrings(cgi.element, cgi.type), cgi.x_coord, cgi.y_coord, x_atom_rad, Color[myColor->cgi]);
    draw_circle(cr,concatStrings(hg2i.element, hg2i.type), hg2i.x_coord, hg2i.y_coord, h_atom_rad, Color[myColor->hg2i]);
    draw_circle(cr,concatStrings(hg3i.element, hg3i.type), hg3i.x_coord, hg3i.y_coord, h_atom_rad, Color[myColor->hg3i]);
}

void nbx_button_clicked (GtkWidget *widget, gpointer data)
{
    /* Code from GTK documentation: https://developer.gnome.org/gtk3/stable/GfkFileChooserDialog.html  */
    GtkWidget *dialog;
    GtkFileChooser *chooser;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
    int res;
    
    dialog = gtk_file_chooser_dialog_new ("Save .nbx file", GTK_WINDOW(gtk_widget_get_toplevel (widget)), 
            action, "_Cancel", GTK_RESPONSE_CANCEL, "_Save", GTK_RESPONSE_ACCEPT, NULL);
    chooser = GTK_FILE_CHOOSER (dialog);
    gtk_file_chooser_set_do_overwrite_confirmation (chooser, TRUE);
    
    res = gtk_dialog_run (GTK_DIALOG (dialog));
    
    if (res == GTK_RESPONSE_ACCEPT)
    {
        char *output;
        char command[128];
        char cst_command[256];
        
        output = gtk_file_chooser_get_filename (chooser);
        // Do something here
        sprintf(cst_command, "/usr/software/bin/spectrum-translator -st varian -dt nbx -sd %s -df %s", data, output);
        GtkWidget *spinner = gtk_spinner_new();
        gtk_spinner_start(spinner);
        printf("%s\n", cst_command);
        system(cst_command);
        gtk_spinner_stop(spinner);
        g_free (output);
        gtk_widget_destroy(spinner);
    }
    
    gtk_widget_destroy (dialog);
    gtk_main_quit();
}