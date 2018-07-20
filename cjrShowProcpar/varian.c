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
#include "varian.h"
#include "cjrutil.h"

int parseNextToken (GScanner *scanner, GtkTreeStore *store, GtkTreeIter *iter, int *numValues, int *enumValues, int *basicType, int procparType, GError **error) {
    
    int negate = 1;
    char numberToString[32];
    
    g_scanner_get_next_token(scanner);
    switch (procparType){
        case PP_NAME :
            if (scanner->token == G_TOKEN_IDENTIFIER) {
                gtk_tree_store_insert(store, iter, NULL, -1); // New row for tree
                gtk_tree_store_set(store, iter, procparType, (char *) scanner->value.v_identifier, -1);
                return 0;
            } else { 
                g_set_error(error, CJR_PARSE_ERROR, scanner->line, "Invalid parameter name: line %d", scanner->line); 
                return 1; 
            }
        case PP_SUBTYPE    :
        case PP_GGROUP     :
        case PP_DGROUP     :
        case PP_PROTECTION :
        case PP_ACTIVE     :
        case PP_INTPTR     :
            if (scanner->token == G_TOKEN_INT) {
                gtk_tree_store_set(store, iter, procparType, (int) scanner->value.v_int, -1);
                return 0;
            } else { 
                g_set_error(error, CJR_PARSE_ERROR, scanner->line, "Expected Integer: line %d, character %d", scanner->line, scanner->position); 
                return 1; 
            }
        case PP_BASIC_TYPE :
            if (scanner->token == G_TOKEN_INT) {
                gtk_tree_store_set(store, iter, procparType, (int) scanner->value.v_int, -1);
                (*basicType) = scanner->value.v_int;
                return 0;
            } else { 
                g_set_error(error, CJR_PARSE_ERROR, scanner->line, "Invalid basictype: line %d", scanner->line); 
                return 1; 
            }
        case PP_MAX :
            scanner->config->int_2_float = TRUE;
            if (scanner->token =='-') {
                negate = -1;
                g_scanner_get_next_token(scanner);
            }
            if (scanner->token == G_TOKEN_FLOAT) {
                gtk_tree_store_set(store, iter, procparType, (float) negate*(scanner->value.v_float), -1);
                return 0;
            } else { 
                g_set_error(error, CJR_PARSE_ERROR, scanner->line, "Invalid Maximum Value: line %d, character %d", scanner->line, scanner->position); 
                return 1; 
            }
        case PP_MIN :
        case PP_STEP_SIZE :
            if (scanner->token =='-') {
                negate = -1;
                g_scanner_get_next_token(scanner);
            }
            if (scanner->token == G_TOKEN_FLOAT) {
                gtk_tree_store_set(store, iter, procparType, (float) negate*(scanner->value.v_float), -1);
                return 0;
            } else { 
                g_set_error(error, CJR_PARSE_ERROR, scanner->line, "Invalid Min/StepSize: line %d, character %d", scanner->line, scanner->position); 
                return 1; 
            } 
        case PP_NUM_VALUES :
            if (scanner->token == G_TOKEN_INT) {
                (*numValues) = scanner->value.v_int;
                return 0;
            } else { 
                g_set_error(error, CJR_PARSE_ERROR, scanner->line, "Invalid number of values: line %d", scanner->line); 
                return 1; 
            }
        case PP_ENUMS :
            if (scanner->token == G_TOKEN_INT) {
                (*enumValues) = scanner->value.v_int;
                return 0;
            } else { 
                g_set_error(error, CJR_PARSE_ERROR, scanner->line, "Invalid number of enumerated values: line %d", scanner->line); 
                return 1; 
            }
        case PP_VALUE :
            if (scanner->token =='-') {
                negate = -1;
                g_scanner_get_next_token(scanner);
            } 
            if ((scanner->token == G_TOKEN_INT) && (*basicType < 2)) {
                sprintf(numberToString, "%d", ((int) scanner->value.v_int)*negate);
                gtk_tree_store_set(store, iter, procparType, numberToString, -1);
                return 0;
            } else if ((scanner->token == G_TOKEN_FLOAT) && (*basicType < 2)) {
                sprintf(numberToString, "%f", ((double) scanner->value.v_float)*negate);
                gtk_tree_store_set(store, iter, procparType, numberToString, -1);
                return 0;
            } else if ((scanner->token == G_TOKEN_STRING) && (*basicType == 2)) {             
                gtk_tree_store_set(store, iter, procparType, (char *) scanner->value.v_string, -1);
                return 0;
            } else { 
                g_set_error(error, CJR_PARSE_ERROR, scanner->line, "Invalid Value: line %d", scanner->line); 
                return 1; 
            }
        case PP_ENUMVALUE :     // Enumerated values must be strings!!
            if ((scanner->token == G_TOKEN_STRING) && (*basicType == 2)) {             
            //    gtk_tree_store_set(store, iter, procparType, (char *) scanner->value.v_string, -1);  // commented out so no enum in tree
                return 0;
            } else { 
                g_set_error(error, CJR_PARSE_ERROR, scanner->line, "Invalid Enumerated Value: line %d", scanner->line); 
                return 1; 
            }
    }
}

GtkTreeStore *parseProcparTree(char *procparString, GError **error)
{
    GtkTreeStore *store;
    GtkTreeIter iterParent, iterChild;
    GScanner *scanner;
    int numValues =0, numEnumValues = 0;
    char testString[] = "ins 3 1 8190 0 1e-07 4 1 3 1 64\n1 1 \n0 \ndownsamp 7 1 999 1 1 3 1 1 0 64\n1 0 \n0 ";
//    GQuark CJR_PARSE_ERROR = g_quark_from_string("CONNJUR Parse Error");
    int i, basictype = 0;

    store = gtk_tree_store_new(13, 
            G_TYPE_STRING,              // parameter name
            G_TYPE_STRING,              // parameter value
            G_TYPE_INT,                 // subtype
            G_TYPE_INT,                 // basictype
            G_TYPE_FLOAT,               // maximum value
            G_TYPE_FLOAT,               // minimum value
            G_TYPE_FLOAT,               // step size
            G_TYPE_INT,                 // Ggroup
            G_TYPE_INT,                 // Dgroup
            G_TYPE_INT,                 // protection
            G_TYPE_INT,                 // active flag
            G_TYPE_INT,                 // intptr  (unused)
            G_TYPE_STRING);             // enumerated values
   
    scanner = g_scanner_new(NULL);
    scanner->config->numbers_2_int = TRUE;
    scanner->config->scan_string_dq = TRUE;
    g_scanner_input_text (scanner, procparString, strlen (procparString));
    
    g_scanner_peek_next_token (scanner);
    while (scanner->next_token != G_TOKEN_EOF) {
        if (parseNextToken (scanner, store, &iterParent, &numValues, &numEnumValues, &basictype, PP_NAME, error)) break;
        if (parseNextToken (scanner, store, &iterParent, &numValues, &numEnumValues, &basictype, PP_SUBTYPE, error)) break;
        if (parseNextToken (scanner, store, &iterParent, &numValues, &numEnumValues, &basictype, PP_BASIC_TYPE, error)) break;
        scanner->config->int_2_float = TRUE;
        if (parseNextToken (scanner, store, &iterParent, &numValues, &numEnumValues, &basictype, PP_MAX, error)) break;
        if (parseNextToken (scanner, store, &iterParent, &numValues, &numEnumValues, &basictype, PP_MIN, error)) break;
        if (parseNextToken (scanner, store, &iterParent, &numValues, &numEnumValues, &basictype, PP_STEP_SIZE, error)) break;
        scanner->config->int_2_float = FALSE;
        if (parseNextToken (scanner, store, &iterParent, &numValues, &numEnumValues, &basictype, PP_GGROUP, error)) break;
        if (parseNextToken (scanner, store, &iterParent, &numValues, &numEnumValues, &basictype, PP_DGROUP, error)) break;
        if (parseNextToken (scanner, store, &iterParent, &numValues, &numEnumValues, &basictype, PP_PROTECTION, error)) break;
        if (parseNextToken (scanner, store, &iterParent, &numValues, &numEnumValues, &basictype, PP_ACTIVE, error)) break;
        if (parseNextToken (scanner, store, &iterParent, &numValues, &numEnumValues, &basictype, PP_INTPTR, error)) break;
        if (parseNextToken (scanner, store, &iterParent, &numValues, &numEnumValues, &basictype, PP_NUM_VALUES, error)) break;
        for (i = 0; i < numValues; i++) {
            if (i == 0) {
                if (parseNextToken (scanner, store, &iterParent, &numValues, &numEnumValues, &basictype, PP_VALUE, error)) break;
            }
            else {
                gtk_tree_store_insert(store, &iterChild, &iterParent, -1);
                if (parseNextToken (scanner, store, &iterChild, &numValues, &numEnumValues, &basictype, PP_VALUE, error)) break;
            }
        }
        if (*error != NULL) break;
        if (parseNextToken (scanner, store, &iterParent, &numValues, &numEnumValues, &basictype, PP_ENUMS, error)) break;
        for (i = 0; i < numEnumValues; i++) {
            if (i == 0) {
                if (parseNextToken (scanner, store, &iterParent, &numValues, &numEnumValues, &basictype, PP_ENUMVALUE, error)) break;
            }
            else {
         //       gtk_tree_store_insert(store, &iterChild, &iterParent, -1);    // Commented out so no enum children in tree
                if (parseNextToken (scanner, store, &iterChild, &numValues, &numEnumValues, &basictype, PP_ENUMVALUE, error)) break;
            }
        }
        if (*error != NULL) break;
        g_scanner_peek_next_token (scanner);
    }
    g_scanner_destroy(scanner);    
    return(store);
}

GtkListStore *parseProcparList(char *filename)
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