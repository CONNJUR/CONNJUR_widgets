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

typedef struct _SpecParm {
	char ID[32];
	int dimensions;
	SpecAxis axis[8];		// should use pointers here.
} SpecParm;

struct _SpecParm *ptr_SpecParm;



#ifdef __cplusplus
}
#endif

#endif /* NEWFILE_H */

