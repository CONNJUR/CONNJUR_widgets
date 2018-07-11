#!/bin/csh

#
# Directories
# Extensions
# Specific files.

set dList = (data sim dif dat ft lp mem ml ist mask nusmask nuszf inspect istmult istres istspec isttmp)
set fList = (ser_full fid_full mask_full pk.tcl test.tab test.tab.old hn.proj.tab nuslist_zf nmr.com)
set eList = (.dat .ft1 .ft2 .ft3 .ft4 .ft .fid .tmp %)

foreach d ($dList)
   if (-d $d) then
      echo $d
      /bin/rm -rf $d
   endif
end

foreach f ($fList)
   if (-e $f) then
      echo $f
      /bin/rm -f $f
   endif
end

foreach f (*)
   foreach e ($eList)
      set b = (`basename $f $e`)

      if (-e $b$e) then
         echo $b$e
         /bin/rm -f $f
      endif
   end
end
