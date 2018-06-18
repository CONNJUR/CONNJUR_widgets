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
#include "connjur.h"

char *concatStrings(char *string1, char *string2) 
{
   // char *finalString;
   // int finalStringLen;
    
   // finalStringLen = strlen(string1) + strlen(string2);
    
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
    GtkListStore *store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
    FILE *procpar;
    int filesize;
    VarianPar parArray[600];    // Hardcode max number of parameters!!!
    int i=0, j;
    char junk[10000]; // Data designed to be discarded
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
        if ((strcmp(parArray[i].name,"ap")!=0) && (strncmp(parArray[i].name,"dg",2)!=0) ) gtk_list_store_insert_with_values(store, NULL, -1, 0, parArray[i].name, 1, parArray[i].value, -1);
 //       gtk_list_store_insert_with_values(store, NULL, -1, 0, parArray[i].name, 1, parArray[i].value, -1);
        i++;
        }
    
    fclose(procpar);
    
    return (store);
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