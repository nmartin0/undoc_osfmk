###############################################################################
#  The BYTE UNIX Benchmarks - Release 3
#          Module: fs.awk   SID: 3.4 5/15/91 19:30:24
#          
###############################################################################
# Bug reports, patches, comments, suggestions should be sent to:
#
#	Ben Smith or Tom Yager at BYTE Magazine
#	ben@bytepb.byte.com   tyager@bytepb.byte.com
#
###############################################################################
#  Modification Log:
#       **** modified to only do file copy results *****
#       added geometric mean 8/6/89 -ben
#       modified for new version of fstime 11/15/89 -ben
#       removed variance 10/23/90 -ben
#
###############################################################################
BEGIN   { c_product = 0.0000;
	iter=0;
	c_too_quick=0;
	}
/FLAVOR\|/ { split($0, junk,"|");
	flavor=junk[2] ;
		}
/real/	{ iter++; ok++; next; }
/user/	{ if (flavor == "SysV") {next;} }
/sys/	{ if (flavor == "SysV") {next;} }
/^$/	{ next; } 
/^#/	{ next; } 
/sample/ { sample = $1; next; }
/fstime/ {
         print "** Iteration ",iter," Failed: ",$0; 
	 ok--;
         fail=1;
         } 
/copy/  { if (!fail) {
	       c+=$1;
	       c2+=$1*$1;
	       c_product += log($1);
	       }
	}
END {
	if (ok > 0) {
# TestName|Sample(seconds)|Unit(KiloBytes/sec)|ArithMean|GeoMean|DataPoints
	    printf "File Copy  (%d seconds)|%d|KBps|%.0f|%.0f|%d\n",sample,sample,c/ok,exp(c_product/ok),ok;
	} else {
	    print "File Copy|  no measured results|"
	}
    }
