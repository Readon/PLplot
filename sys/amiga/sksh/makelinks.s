#!c:sksh

#*************************************************************************
# Makes links to all PLPLOT source files using the SKsh lnsoft script.
#
# I use soft links rather than hard links so that I can edit the original
# file but (a) only end up with one copy, and (b) have the time stamp on
# the link change.  If you use hard links and you change the original
# file, you end up with TWO files, and the timestamp on the copy on the
# tmp directory will not change (so make won't rebuild).
#
# There are some problems using soft links, however, including: most 
# directory listers see them as directories.  I've yet to find a good 'ls'
# that reports the link properly without generating enforcer hits.  Also,
# they can be difficult to remove at times.  I've found that you can
# always remove them from the workbench, however.
#
#*************************************************************************

lnsoft ../src/*.c .
lnsoft ../include/*.h .
lnsoft ../drivers/*.c .
lnsoft ../utils/*.c .
lnsoft ../examples/C/*.c .
lnsoft ../sys/amiga/*.m4 .
lnsoft ../sys/amiga/src/*.c .
lnsoft ../sys/amiga/src/*.h .
lnsoft ../sys/amiga/cf/*.h .
lnsoft ../lib/*.fnt .
lnsoft ../lib/*.map .
