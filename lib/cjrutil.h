/*
 * Copyright (C) 2018 Michael R. Gryk
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

/* 
 * File:   cjrutil.h
 * Author: mgryk
 *
 * Created on July 17, 2018, 9:37 AM
 */

#ifndef CJRUTIL_H
#define CJRUTIL_H

#define CJR_PARSE_ERROR 1722
#define MAX_FILE_STRING_SIZE 100000

char *concatManyStrings(int numStrings, char *str, ...);

char *concatStrings(char *string1, char *string2);

void readFileToString_alt(char *fileName, char **fileString, GError **error);

char *readFileToString(char*fileName, GError **error);

float FLOAT_FROM_BE(float swapFloat);

void FLOAT_FROM_BE_INPLACE(float *swapFloat);

/*
 *  This function swaps the bytes of a 32-bit float, from Big Endian to Little Endian or vice versa.
 *      It makes use of the Glib function GUINT32_SWAP_LE_BE which byte swaps ints.
 *      There is no guarantee the float is encoded properly, just that the 4 bytes are reversed in order.
 */

void BYTESWAP_FLOAT(float *swapFloat);

char *cjr_terminate_string(char *string, int index);

#endif /* CJRUTIL_H */
