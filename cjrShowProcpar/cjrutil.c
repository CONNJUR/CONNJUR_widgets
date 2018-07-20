/*
 * Copyright (C) 2018 Michael R. Gryk
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

/* 
 * File:   cjrutil.c
 * Author: mgryk
 * 
 * Created on July 17, 2018, 9:37 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <sys/stat.h>
#include "cjrutil.h"

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

void readFileToString_alt(char *fileName, char **fileString, GError **error){
    
    struct stat fileStat;
    
    if (stat(fileName, &fileStat) != 0)  {
        g_set_error(error, 0, 0, "Stat failure on %s", fileName);
        return;
    }
    (*fileString) = (char *)malloc(fileStat.st_size)+1;
    if (*fileString == NULL) {
        g_set_error(error, 0, 0, "Malloc failure for %s", fileName);
        return;
    }
    g_file_get_contents(fileName, fileString, NULL, error);  
    return;
} 

char *readFileToString(char*fileName, GError **error){
    
    struct stat fileStat;
    char *fileString;
    
    if (stat(fileName, &fileStat) != 0)  {
        g_set_error(error, 0, 0, "Stat failure on %s", fileName);
        return NULL;
        // Need to also check that it is a file and not directory
    }
    fileString = (char *)malloc(fileStat.st_size)+1;     
    if (fileString == NULL) {
        g_set_error(error, 0, 0, "Malloc failure for %s", fileName);
        return NULL;
    }
    g_file_get_contents(fileName, &fileString, NULL, error);  
    return fileString;
}