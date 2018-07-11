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
    VarianPar parArray[1000];    // Hardcode max number of parameters!!!
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
        if ((strcmp(parArray[i].name,"ap")!=0) && (strncmp(parArray[i].name,"dg",2)!=0) ) gtk_list_store_insert_with_values(store, NULL, -1, 0, parArray[i].name, 1, parArray[i].value, -1);
 //       gtk_list_store_insert_with_values(store, NULL, -1, 0, parArray[i].name, 1, parArray[i].value, -1);
        i++;
        }
    
    fclose(procpar);
    
    return (store);
}