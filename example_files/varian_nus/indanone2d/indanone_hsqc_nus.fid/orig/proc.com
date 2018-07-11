#!/bin/csh

#
# Processing NUS data with conventional FT, IST, and SMILE.
#  Results are rescaled to max 100.0 for easier comparison.
#  The following processing parameters are common to each scheme:

set procArgs = "-xP0 25.42 -xP1 0 -yP0 0 -yP1 0 -scaleTo 100.0"

#
# Process by regular FT for comparison; 
# Extra Zero Fill is used so that all results have the same size.
#
# Process by IST, including NUS Zero-Fill extrapolation (-nusZF);
# IST processing uses -mask input data to specify the NUS schedule.

# Process by SMILE (NUS Zero-Fill extrapolation is default for SMILE);
# SMILE processing uses -sample input data to specify the NUS schedule.

basicFT2.com $procArgs -title NUS_25pct_FT          -in test.fid -out test.ft2  -yZFARG zf=2,auto
ist2D.com    $procArgs -title NUS_25pct_NUSZF_IST   -in test.fid -out ist.ft2   -mask mask.fid -istMaxRes Auto -nusZF
smile2D.com  $procArgs -title NUS_25pct_NUSZF_SMILE -in test.fid -out smile.ft2 -sample sampling.sch
