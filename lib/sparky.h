/* 
 * File:   sparky.h
 * Author: mgryk: see Sparky copyright, license disclaimer below
 *
 * Created on August 20, 2018, 12:07 PM
 */

#ifndef SPARKY_H
#define SPARKY_H

#define SPARKY_HEADER_SIZE      180  
#define SPARKY_AXIS_SIZE        128 
#define SPARKY_HEADER_SIZE_1    129    
#define SPARKY_HEADER_SIZE_2    51     
#define SPARKY_AXIS_SIZE_1      74     
#define SPARKY_AXIS_SIZE_2      10
#define SPARKY_AXIS_SIZE_3      32
#define SPARKY_AXIS_SIZE_4      12
#define SPARKY_MAX_AXES         4       // Min and max defined in Manual
#define SPARKY_MIN_AXES         2       //      https://www.cgl.ucsf.edu/home/sparky/manual/files.html

/*  mgryk: Note that the header sizes of 180 and 128 are different than the sizeof the structures (192, 136) (64-bit).
 *  This is because of alignment padding in the structures 
 *  64-bit:
 *      In NMR_AXIS, there are 2 bytes of padding for each LP_EXTEND and 4 bytes of padding before the void pointer.
 *      In NMR_HEADER, there are 7 bytes of padding before seek_pos.
 *  32-bit:
 *      In NMR_AXIS, there are 4 bytes of padding after the void pointer.
 *      In NMR_HEADER, there are 3 bytes of padding before seek_pos.
 *  Need to byte shift if you really want those values: see sparky.c
 */

/*
 *  The following copyright, license and disclaimer applies to the distributed Sparky source code, documentation and binaries. 
 *  Copyright (c) 2009, The Regents of the University of California.
 *  All rights reserved.
 * 
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions, and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions, and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *    3. Redistributions must acknowledge that this software was
 *       originally developed by the UCSF Resource for Biocomputing,
 *       Visualization, and Informatics under support by the NIH
 *       National Center for Research Resources, grant P41-RR01081.
 * 
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER "AS IS" AND ANY
 *  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 *  OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 *  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 *  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

/*
 *  The following code comes from the Sparky source code: ucsffile.cc
 *  These code blocks define the data structures used for the two headers of .ucsf files
 *  *** BEGIN ucsffile.cc ***
 */

/*
 * data structure for solvent removal via convolution of fid.
 */
typedef struct {
	short		width;		/* half width of window */
	short		extrapolation;	/* npoints at ends to extrapolate */
} NMR_CONVOLUTION;

/*
 * parameters for apodization.
 */
typedef struct {
	union {
		float	shift;
		float	line_broad;
	}	p1;
	union {
		float	gaussian;
	}	p2;
} NMR_APO_PARAMS;

/*
 * parameters for polynomial fitting of transformed baseline.
 */
typedef struct {
	unsigned	solvent_start;
	short		poly_order;
	short		solvent_width;
} NMR_BASE_FIT;

/*
 * region for calculating offest of transformed baseline.
 */
typedef struct {
	unsigned	start, end;
} NMR_BASE_OFFSET;

/*
 * Processing flags / small bitfields
 */
typedef struct {
	unsigned	fid_convolution : 2;
	unsigned	dc_offset	: 1;
	unsigned	forward_extend	: 1;
	unsigned	backwards_extend: 1;
	unsigned	replace		: 2;
	unsigned	apodization	: 4;
	unsigned	ft_code		: 2;
	unsigned	nfills		: 4;
	unsigned	absolute_value	: 1;
	unsigned	spectrum_reverse: 1;
	unsigned	baseline_offset	: 1;
	unsigned	baseline_fit	: 2;
	unsigned	reserved	: 10;
} NMR_FLAG;

/*
 * results flag.
 */
typedef struct {
	unsigned	transformed	: 1;
	unsigned	base_corrected	: 1;
	unsigned	reserved	: 14;
} NMR_PROCESSED;

#define LP_REPLACE_NONE		0
#define LP_REPLACE_BEFORE	1
#define LP_REPLACE_AFTER	2

typedef struct {
	unsigned	start;		/* 1st pt. in coeffs. calculation */
	unsigned short	poly_order;	/* polynomial order */
	unsigned short	npredicted;	/* # of additional points */
	unsigned short	npoints;	/* # of point in coeffs region */
} LP_EXTEND;

typedef struct {
	unsigned	before_start;	/* 1st pt. in downfield coeffs. calc. */
	unsigned	after_start;	/* 1st pt. in upfield coeffs calc. */
	unsigned	first;		/* first pt to be replaced */
	unsigned short	npredicted;	/* length of region replacement. */
	unsigned short	poly_order;	/* polynomial order */
	unsigned short	before_npoints;	/* # of point in coeffs region */
	unsigned short	after_npoints;	/* # of point in coeffs region */
} LP_REPLACE;

typedef struct {
	char		nucleus[6];
	short		spectral_shift;		/* to left or right shift */
	unsigned	npoints;		/* # of active data points */
	unsigned	size;			/* total size of axis */
	unsigned	bsize;			// # of points per cache block
	float		spectrometer_freq;	// MHz
	float		spectral_width;		// Hz
	float		xmtr_freq;		/* transmitter offset (ppm) */
	float		zero_order;		/* phase corrections */
	float		first_order;
	float		first_pt_scale;		/* scaling for first point */
	NMR_PROCESSED	status;			/* completion flags */
	NMR_FLAG	flags;			/* processing options */
	NMR_CONVOLUTION	conv;			/* FID convolution parameters */
	NMR_APO_PARAMS	apo;			/* apodization parameters */        // mgryk: don't use after here.  see above
	LP_EXTEND	forward;		/* FID extension */
	LP_EXTEND	backwards;		/* FID beginning correction */
	LP_REPLACE	replace;		/* FID replacement */
	NMR_BASE_OFFSET	base_offset;		/* baseline offset correction */
	NMR_BASE_FIT	base_fit;
	void		*unused;                
} NMR_AXIS;

typedef struct {                        // THESE COMMENTS ARE FROM mgryk, NMRbox, CONNJUR
	char ident[10]; 		// Should read "UCSF NMR\0"
	char naxis;  			// These are not '1','2','3' but hhu 1, 2, 3
	char ncomponents;
	char encoding;
	char version;                   // Version 2 is what these code blocks are for
	char owner[9];
	char date[26];
	char comment[80];               // mgryk: don't use after here.  see above about padding  129
	long seek_pos;                  // 144
	char scratch[40];               // 184
	NMR_AXIS *axis;                 // 192
} NMR_HEADER;

/*
 *  *** END ucsffile.cc ***
 */

typedef struct _sparkyHeaderStruct {
    NMR_HEADER file;
    NMR_AXIS axis[SPARKY_MAX_AXES];
} sparkyHeaderStruct;

/*
 * sparkHeaderAsString
 * 
 * This function converts a sparky header struct into a string.  
 * This function is created for debugging purposes only.  No care has been taken
 *      for print formatting.
 */

char *sparkyHeaderAsString(sparkyHeaderStruct *header);

sparkyHeaderStruct *readSparkyHeader(char* fileName, GError **error);

/*
 * printSparkyStructSizes
 * 
 * This procedure prints the sizes of all of the sparky structs..  
 * This procedure is for debugging/informational purposes only.  
 */

void printSparkyStructSizes();

#endif /* SPARKY_H */
