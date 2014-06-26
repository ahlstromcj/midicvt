#!/bin/perl
#
#  The following Perl script changes note off messages to note on with
#  vol=0, deletes controller 3 changes, makes some note reassignments on
#  channel 10, and changes the channel numbers from channel 1 depending on
#  the track number. 

%drum = (62, 36, 63, 47, 65, 61, 67, 40, 68, 54);

while (<>)
{
   next if /c=3/;
   s/Off(.*)v=[0-9]*/On\1v=0/;
   if (/ch=10/ && /n=([0-9]*)/ && $drum{$1})
   {
      s/n=$1/"n=".$drum{$1}/e;
   }
   if (/^MTrk/)
   {
      ++$trknr ; print "track $trknr\n";
   }
   if ($trknr > 2)
   {
      s/ch=1\b/ch=3/;
   }
   else
   {
      s/ch=1\b/ch=4/;
   }
   print || die "Error: $!\n";
}   
