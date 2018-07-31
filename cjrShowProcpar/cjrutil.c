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
#include <stdarg.h>
#include "cjrutil.h"

char *concatManyStrings(int numStrings, char *str, ...){
    
    va_list strList;
    char *catStrings;
    int i;
    int strLength=strlen(str);    // Length of first string (str)
    
    va_start(strList, str);
    for (i=2; i<=numStrings; i++) strLength += strlen(va_arg(strList, char *));
    va_end(strList);
    
    catStrings = malloc((strLength*sizeof(char))+1); // Sum of string lengths plus \0
    
    va_start(strList, str);
    strcpy(catStrings, str);
    for (i=2; i<=numStrings; i++) strcat(catStrings,va_arg(strList, char*)); 
    va_end(strList);
    
    return(catStrings);
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

void readFileToString_alt(char *fileName, char **fileString, GError **error){
    
    struct stat fileStat;
    
    if (stat(fileName, &fileStat) != 0)  {
        (*error) = g_error_new(G_FILE_ERROR, G_FILE_ERROR_NOENT, "Stat failure on %s", fileName);
        return;
    }
    (*fileString) = (char *)malloc(fileStat.st_size)+1;
    if (*fileString == NULL) {
        (*error) = g_error_new(G_FILE_ERROR, G_FILE_ERROR_NOENT, "malloc failure for %s", fileName);
        return;
    }
    g_file_get_contents(fileName, fileString, NULL, error);  
    return;
} 

char *readFileToString(char* fileName, GError **error){
    
    struct stat fileStat;
    char *fileString;
    
    if (stat(fileName, &fileStat) != 0)  {
        (*error) = g_error_new(G_FILE_ERROR, G_FILE_ERROR_NOENT, "Stat failure on %s", fileName);
        return NULL;
    }
    if (fileStat.st_size > MAX_FILE_STRING_SIZE)  {
        (*error) = g_error_new(G_FILE_ERROR, G_FILE_ERROR_NOENT, "%s too large to parse", fileName);
        return NULL;
    }
    fileString = (char *)malloc(fileStat.st_size)+1;     
    if (fileString == NULL) {
        (*error) = g_error_new(G_FILE_ERROR, G_FILE_ERROR_NOENT, "malloc failure for %s", fileName);
        return NULL;
    }
    g_file_get_contents(fileName, &fileString, NULL, error);  
    return fileString;
}