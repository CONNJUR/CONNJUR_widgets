#!/bin/csh

#
# Processing with Linear Prediction (LP):
#  Use -yLP ... to set LP options.
#  Results are rescaled to max 100.0 for easier comparison.

basicFT2.com -in test.fid -out lp.ft2 \
             -xP0 25.42 -xP1 0 -yP0 0 -yP1 0 -scaleTo 100.0 \
             -yLP fb,ord=10 -title US_LP


