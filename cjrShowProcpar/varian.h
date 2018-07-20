/*
 * Copyright (C) 2018 Michael R. Gryk
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

/* 
 * File:   connjur.h
 * Author: mgryk
 *
 * Created on May 31, 2018, 3:57 PM
 */

#ifndef NEWFILE_H
#define NEWFILE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	CJR_VARIAN	= 0,
	CJR_BRUKER	= 1
	} precessionSign;
        
typedef enum {
        PP_NAME         = 0,
        PP_VALUE        = 1,
        PP_SUBTYPE      = 2,
        PP_BASIC_TYPE   = 3,
        PP_MAX          = 4,
        PP_MIN          = 5,
        PP_STEP_SIZE    = 6,
        PP_GGROUP       = 7,
        PP_DGROUP       = 8,
        PP_PROTECTION   = 9,
        PP_ACTIVE       = 10,
        PP_INTPTR       = 11,
        PP_ENUMVALUE    = 12,
        PP_NUM_VALUES   = 13,
        PP_ENUMS        = 14
} procparColumns;

typedef struct _SpecAxis {
	char sign; 			// +/-, should be enum
	char negImag;  			// T/F, should be enum
	float sweepWidth;
	char sweepWidthUnits[8];	// Hz, ppm, should be enum
	float specFreq;
	char frepUnits[8];		// MHz, Tesla, should be enum
	char domain;			// T:time, F:freq, should be enum
	char pointType[8];		// real, complex, should be enum
	float cnstPhase;
	float linPhase;
	int firstScalePoint;		// may want to deprecate this
} SpecAxis;

struct _SpecAxis *ptr_SpecAxis;

typedef struct _VarianPar {
    char name[32];                      // Name of parameter
    int subtype;                        // 0:und, 1:real, 2:string, 3:delay, 4:flag, 5:freq, 6:pulse, 7:int
    int basictype;                      // 0:und(int), 1:real, 2:string
    double max_value;                   // Either min value or pointer to /vnmr/conpar
    double min_value;                   // Either max value or pointer to /vnmr/conpar
    double step_size;                   // Either step_size or pointer to /vnmr/conpar  (O is ignored).
    int Ggroup;                         // 0:ALL, 1:SAMPLE, 2:ACQ, 3:PROC, 4:DISPLAY, 5:SPIN
    int Dgroup;                         // See manual for enumeration
    int protection;                     // 32-bit word.  See manual for bit meanings.
    int active;                         // 0: not active, 1:active
    int intptr;                         // Not used - normally set to 64
    int number_of_values;               // Number of values for parameter.
    char value[2048];                   // Value of parameter.  Stored as string, see subtype and basictype
    int enumerable_values;              // Number of enumerated values possible for value.  Can add array for this later.
    char enum_value[2018];
} VarianPar;

struct _VarianPar *ptr_VarianPar;

typedef struct _ReqVarianPar {
    char date[16];
    int arraydim;
    char arrayedParam[32];
    float at;
    float gain;
    int arrayelemts;
    int transients;
    float rof1, rof2;
    char seqfil[32];
    char solvent[16];
    float temperature;
    float parVersion;
    float freq[5];                      // arbitrary allow up to 5 channels
    char channelName[5][8];
    float offset[5];
    float linearPhase[4];               // arbitrary allow up to 4 dimensions
    float constantPhase[4];
    int points[4];
    float sweepWidth[4];
} ReqVarianPar;

struct _ReqVarianPar *ptr_ReqVarianPar;

typedef struct _SpecParm {
	char ID[32];
	int dimensions;
	SpecAxis axis[8];		// should use pointers here.
} SpecParm;

struct _SpecParm *ptr_SpecParm;

int oldParseProcpar(char *filename, struct _ReqVarianPar *varPar, GtkListStore **list);

GtkListStore *parseProcparList(char *filename);

GtkTreeStore *parseProcparTree(char *newString, GError **error);



#ifdef __cplusplus
}
#endif

#endif /* NEWFILE_H */

