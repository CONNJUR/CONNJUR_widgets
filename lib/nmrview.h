/*
 * Copyright (C) 2018 Michael R. Gryk
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

/* 
 * File:   nmrview.h
 * Author: mgryk
 *
 * Created on August 23, 2018, 2:20 PM
 */

#ifndef NMRVIEW_H
#define NMRVIEW_H

#define NMRVIEW_MAX_AXES         8              // Max defined in Manual; Min is a guess
#define NMRVIEW_MIN_AXES         1              //      http://docs.nmrfx.org/processor/files/fformat
#define NMRVIEW_HEADER_SIZE      2048
#define NMRVIEW_MAGIC            874032077
#define NMRVIEW_CIGAM            -844425164     // Byte swapped MAGIC number

typedef struct _nmrviewFileHeaderStruct {
    unsigned int magic;         // Should be equal to 874032077 if this is an NMRView file.
    int version;                // Currently set to 0
    int unused_1;               // Unused, but may be used in subsequent versions.
    int fileHeaderSize;         // The size of the header at beginning of file. Typically 2048 bytes for NMRViewJ
    int blockHeaderSize;        // The size of the header at the beginning of each data block (sub-matrix tile). Typically 0.
    int blockElements;          // The number of elements in each sub-matrix
    int nDim;                   // The number of dimensions in the file
    char unused_2[996];         // Unused, but may be used in subsequent versions.
} nmrviewFileHeaderStruct;

typedef struct _nmrviewDimHeaderStruct {
    int size;                   // The number of data points along this dimension
    int blockSize;              // The number of data points in a block, along this dimension
    int nBlocks;                // The total number of blocks in the file, the value is ignored on reading and calculated from the above sizes
    int unused_1;               // Unused, but reserved for future use.
    int unused_2;               // Unused, but reserved for future use.
    int unused_3;               // Unused, but reserved for future use.
    float sf;                   // The spectrometer frequency for this dimension (Mhz)
    float sw;                   // The sweep width for this dimension (Hz)
    float refpt;                // The data point at which the reference is specified. Typically the center of spectrum.
    float refval;               // The reference value at the above reference point.
    int refunits;               // The units the reference is specified in, typically the integer 3, indicating ppm.
    float foldUp;               // Unused at present
    float foldDown;             // Unused at present
    char label[16];             // The label for this dimension of the axis.  IMPORTANT.  Can have 16 chars without \0
    int complex;                // 0 if the data along this dimension is real, 1 if it is complex
    int freqdomain;             // 0 if the data along this dimension is in the time domain, 1 if it is in the frequency domain
    float ph0;                  // The cumulateive zero-order phase correction applied along this dimension.
    float ph1;                  // The cumulateive first-order phase correction applied along this dimension.
    int vsize;                  // The number of data points along this dimension that have valid data. Used during processing.
    char unused_4[40];          // Unused, but reserved for future use.
} nmrviewDimHeaderStruct;

typedef struct _nmrviewHeaderStruct {
    nmrviewFileHeaderStruct file;
    nmrviewDimHeaderStruct axis[NMRVIEW_MAX_AXES];
} nmrviewHeaderStruct;

/*
 * nmrviewHeaderAsString
 * 
 * This function converts an NMRView header struct into a string.  
 * This function is created for debugging purposes only.  
 */

char *nmrviewHeaderAsString(nmrviewHeaderStruct *header);

/*
 * readNMRViewHeader
 * 
 * This function parses individual fields of a binary NMRView (.nv) file.  
 * 
 * How it works:  This function just uses fread to read the binary header directly into the NMRViewHeader struct.
 * NMRView files can be either endian.  This function does NOT correct endianess.  That should be done elsewhere.
 * 
 * Inputs: NMRView filename
 *         GError for error callbacks.
 * Output: nmrViewHeader (success or failure).  If failure, GError should not be NULL.
 */

nmrviewHeaderStruct readNMRViewHeader(char* fileName, GError **error);

#endif /* NMRVIEW_H */
