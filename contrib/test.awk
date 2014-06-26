#!/bin/awk
#
#  The following awk script changes note off messages to note on with
#  vol=0, deletes controller 3 changes, makes some note reassignments on
#  channel 10, and changes the channel numbers from channel 1 depending on
#  the track number. 

BEGIN { drum[62] = 36; drum[63] = 47; drum[65] = 61; \
   drum[67] = 40; drum[68] = 54 }
/c=3/ { next }
($2 == "Off") { $2 = "On"; $5 = "v=0" }
/ch=10/ { n = substr($4, 3); if (n in drum) $4 = "n=" drum[n] }
/^MTrk/ { trknr++ }
/ch=1 / { if (trknr > 2) { $3 = "ch=2" } else { $3 = "ch=3" } } 
{ print }
