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

char *concatStrings(char *string1, char *string2);

void readFileToString_alt(char *fileName, char **fileString, GError **error);

char *readFileToString(char*fileName, GError **error);

#endif /* CJRUTIL_H */
