#!/usr/bin/perl
# cleanup.pl
# removes left-over semaphores, msg queues, and shared memory segments
# weide chang, copy-fee

# get rid of leftover semaphores
`/bin/rm -f /dev/shm/*`; # force-rm leftover semaphores

# Perl uses $_ as default built-in

# delelte msg q's
for ( `ipcs -q` ) {  # generates lines of of strings, each $_
   print;          # $_ is printed, that's 'each'
   chop;           # $_ is chopped (last char off - \n)
   split /[ \t]+/; # $_ is being split into array @_ (use $_[?])

   if ( $_[0] =~ /^0x/ ) { # if 1st token starts with 0x
      print "*** removing msg q id $_[1]...\n"; # 2nd token is qid
      `ipcrm -q $_[1]`;                         # shell cmd to remove
   }
}

# get rid of leftover shared memory segments
for ( `ipcs -m` )  { # generates lines of of strings, each $_
   print;          # $_ is printed, that's 'each'
   chop;           # $_ is chopped (last char off - \n)
   split /[ \t]+/; # $_ is being split into array @_

   if ( $_[0] =~ /^0x/ ) { # if 1st token starts with 0x
      print "*** removing shm id $_[1]...\n"; # 2nd token is mid
      `ipcrm -m $_[1]`;                       # shell cmd to remove
   }
}

