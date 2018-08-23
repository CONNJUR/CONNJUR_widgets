/*
 * Copyright (C) 2018 Michael R. Gryk
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

/* 
 * File:   sparky.c
 * Author: mgryk
 * 
 * Created on August 20, 2018, 12:07 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <gtk/gtk.h>
#include "sparky.h"
#include "cjrutil.h"

/*
 * sparkHeaderAsString
 * 
 * This function converts a sparky header struct into a string.  
 * This function is created for debugging purposes only.  No care has been taken
 *      for print formatting.
 */

char *sparkyHeaderAsString(sparkyHeaderStruct *header){
    
    int i;
    static char sparkyString[1000];
    
    sprintf(sparkyString,"ident:%s\tnaxis:%hhu\tncomponents:%hhu\tencoding:%hhu\tversion:%hhu\n", 
            header->file.ident, header->file.naxis, header->file.ncomponents, header->file.encoding, header->file.version);
    sprintf(&sparkyString[strlen(sparkyString)],"owner:\t%s\n", header->file.owner);
    sprintf(&sparkyString[strlen(sparkyString)],"date:\t%s\n", header->file.date);
    sprintf(&sparkyString[strlen(sparkyString)],"comment:\t%s\n", header->file.comment);
    sprintf(&sparkyString[strlen(sparkyString)],"seek_pos:\t%d\n", GINT_FROM_BE(header->file.seek_pos));
    sprintf(&sparkyString[strlen(sparkyString)],"scratch:\t%s\n", header->file.scratch);
    for (i=0; i<header->file.naxis; i++) {
        sprintf(&sparkyString[strlen(sparkyString)],
                "Nucleus: %s\tshift: %hd\tnpoints: %u\tsize: %u\tbsize: %u\tsfreq: %.2f\tswidth: %.2f\tcenter: %.2f\n",
                header->axis[i].nucleus, GINT16_FROM_BE(header->axis[i].spectral_shift), 
                GUINT_FROM_BE(header->axis[i].npoints), GUINT_FROM_BE(header->axis[i].size), GUINT_FROM_BE(header->axis[i].bsize), 
                FLOAT_FROM_BE(header->axis[i].spectrometer_freq), FLOAT_FROM_BE(header->axis[i].spectral_width),
                FLOAT_FROM_BE(header->axis[i].xmtr_freq)); 
        sprintf(&sparkyString[strlen(sparkyString)],
                "p0: %.2f\tp1: %.2f\tfp: %.2f\n", FLOAT_FROM_BE(header->axis[i].zero_order),
                FLOAT_FROM_BE(header->axis[i].first_order), FLOAT_FROM_BE(header->axis[i].first_pt_scale)); 
        sprintf(&sparkyString[strlen(sparkyString)],
                "Conv.width: %hd\tConv.ext: %hd\tApo.p1.shift: %.2f\tApo.p1.lb: %.2f\n",
                GINT16_FROM_BE(header->axis[i].conv.width), GINT16_FROM_BE(header->axis[i].conv.extrapolation),
                FLOAT_FROM_BE(header->axis[i].apo.p1.shift), FLOAT_FROM_BE(header->axis[i].apo.p1.line_broad));
        sprintf(&sparkyString[strlen(sparkyString)],
                "Forward Start: %u\tOrder: %hu\tPred: %hu\tPoints: %hu\n",
                GUINT_FROM_BE(header->axis[i].forward.start),GUINT16_FROM_BE(header->axis[i].forward.poly_order),
                GUINT16_FROM_BE(header->axis[i].forward.npredicted), GUINT16_FROM_BE(header->axis[i].forward.npoints));
        sprintf(&sparkyString[strlen(sparkyString)],
                "Backwards Start: %u\tOrder: %hu\tPred: %hu\tPoints: %hu\n",
                GUINT_FROM_BE(header->axis[i].backwards.start),GUINT16_FROM_BE(header->axis[i].backwards.poly_order),
                GUINT16_FROM_BE(header->axis[i].backwards.npredicted), GUINT16_FROM_BE(header->axis[i].backwards.npoints));
        sprintf(&sparkyString[strlen(sparkyString)],
                "Replace BS: %u\tAS: %u\tfirst: %u\tnpred: %hu\tpo: %hu\tbnp: %hu\tanp: %hu\n",
                GUINT_FROM_BE(header->axis[i].replace.before_start), GUINT_FROM_BE(header->axis[i].replace.after_start),
                GUINT_FROM_BE(header->axis[i].replace.first), GUINT16_FROM_BE(header->axis[i].replace.npredicted),
                GUINT16_FROM_BE(header->axis[i].replace.poly_order), GUINT16_FROM_BE(header->axis[i].replace.before_npoints),
                GUINT16_FROM_BE(header->axis[i].replace.after_npoints));
        sprintf(&sparkyString[strlen(sparkyString)],
                "Base off_start: %u\toff_end %u\tsol_start %u\tpoly_order: %hu\tsolvent-width: %hu\n",
                GUINT_FROM_BE(header->axis[i].base_offset.start), GUINT_FROM_BE(header->axis[i].base_offset.end),
                GUINT_FROM_BE(header->axis[i].base_fit.solvent_start), GUINT16_FROM_BE(header->axis[i].base_fit.poly_order),
                GUINT16_FROM_BE(header->axis[i].base_fit.solvent_width));
    } 
    
    return sparkyString;
}

/*
 * readSparkyHeader
 * 
 * This function parses individual fields of a binary Sparky (.ucsf) file.  
 * 
 * How it works:  This function just uses fread to read the binary header directly into the sparkyHeader struct.
 *      The sparkyHeader struct uses the NMR_HEADER and NMR_AXIS structures defined by Sparky.
 *      A small nuance, because of some alignment padding done by the compiler, we don't read the entire header
 *          in one shot.  Rather, we read segments up until a known pad point.
 *      The header field naxis tells us how many NMR_AXIS structures to read.  Sparky restricts between 2 and 4.
 * 
 * Inputs: sparky filename
 *         GError for error callbacks.
 * Output: sparkyHeader (success) or NULL (failure).  If NULL, GError should not be NULL.
 */

sparkyHeaderStruct *readSparkyHeader(char* fileName, GError **error){
    
    struct stat fileStat;
    sparkyHeaderStruct *sparkyBuffer;
    FILE *fp;
    int axis;

    if (stat(fileName, &fileStat) != 0)  {
        (*error) = g_error_new(G_FILE_ERROR, G_FILE_ERROR_NOENT, "Stat failure on %s", fileName);
        return NULL;
    }
    fp=fopen(fileName,"r+");
    if (fp == NULL) {
        (*error) = g_error_new(G_FILE_ERROR, G_FILE_ERROR_NOENT, "Could not open %s", fileName);
        return NULL;
    }
    if (fileStat.st_size < (SPARKY_HEADER_SIZE + 2*SPARKY_AXIS_SIZE))  {
        (*error) = g_error_new(G_FILE_ERROR, G_FILE_ERROR_NOENT, "%s too small to be ucsf file", fileName);
        return NULL;
    } 
    sparkyBuffer = (sparkyHeaderStruct *)malloc(sizeof(sparkyHeaderStruct));
    if (sparkyBuffer == NULL) {
        (*error) = g_error_new(G_FILE_ERROR, G_FILE_ERROR_NOENT, "malloc failure for %s", fileName);
        return NULL;
    }
    
    /*  These multiple freads are due to struct alignment issues with the c compiler.  There bytes
     *      padded after the comment field.  32-bit has 3 bytes of padding.  64-bit has 7 bytes.
     *      We just take care of overall size of header, the padding is handled by struct/compiler
     */
    fread(sparkyBuffer,1,SPARKY_HEADER_SIZE_1,fp);
    fread(&sparkyBuffer->file.seek_pos,1,SPARKY_HEADER_SIZE_2,fp);
    if ((strcmp(sparkyBuffer->file.ident,"UCSF NMR") != 0) || (sparkyBuffer->file.version != 2)
            || (sparkyBuffer->file.naxis < 2) || (sparkyBuffer->file.version > 4)) {
        (*error) = g_error_new(G_FILE_ERROR, G_FILE_ERROR_NOENT, "%s is not valid version 2 ucsf file", fileName);
        return NULL;
    }
    for (axis = 0; axis < sparkyBuffer->file.naxis; axis++) {
        /*  These multiple freads are due to struct alignment issues with the c compiler.  There are 3 places
         *      in the NMR_AXIS structure where a few bytes are padded.  2 bytes after each LP_EXTEND and 4 bytes
         *      before the unused void pointer.  Since nothing after the first 32 bytes is typically used, this 
         *      is overkill.  But just in case we ever want to support the 'superfluous junk' discussed in Sparky
         *      documentation.  ** THIS PADDING IS 64-BIT SPECIFIC!!!  For 32-bit, no padding required. **
         */
        fread(&sparkyBuffer->axis[axis],1,SPARKY_AXIS_SIZE_1,fp);
        fread(&sparkyBuffer->axis[axis].forward,1,SPARKY_AXIS_SIZE_2,fp);
        fread(&sparkyBuffer->axis[axis].backwards,1,SPARKY_AXIS_SIZE_3,fp);
        fread(&sparkyBuffer->axis[axis].unused,1,SPARKY_AXIS_SIZE_4,fp);
    }
    
    fclose(fp);
    
    return sparkyBuffer;
}

/*
 * printSparkyStructSizes
 * 
 * This procedure prints the sizes of all of the sparky structs..  
 * This procedure is for debugging/informational purposes only.  
 */

void printSparkyStructSizes(){
    
    printf("size of short: %lu\tsize of unsigned: %lutsize of float: %lu\n", 
            sizeof(short), sizeof(unsigned), sizeof(float));
    printf("size of NMR_PROCESSED: %lu\n", sizeof(NMR_PROCESSED));
    printf("size of NMR_FLAG: %lu\n", sizeof(NMR_FLAG));
    printf("size of NMR_CONVOLUTION: %lu\n", sizeof(NMR_CONVOLUTION));
    printf("size of NMR_APO_PARAMS: %lu\n", sizeof(NMR_APO_PARAMS));
    printf("size of LP_EXTEND: %lu\n", sizeof(LP_EXTEND));
    printf("size of LP_REPLACE: %lu\n", sizeof(LP_REPLACE));
    printf("size of NMR_BASE_OFFSET: %lu\n", sizeof(NMR_BASE_OFFSET));
    printf("size of NMR_BASE_FIT: %lu\n", sizeof(NMR_BASE_FIT)); 
    printf("size of char: %lu\tsize of long: %lu\tsize of void*: %lu\n", sizeof(char), sizeof(long), sizeof(void *));
    printf("header structure size: %lu\n",sizeof(NMR_HEADER));
    printf("axis structure size: %lu\n",sizeof(NMR_AXIS));
}