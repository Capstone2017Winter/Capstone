#Nancy Minderman
# nancy.minderman@ualberta.ca
# This script kills all jtagd daemons and reconnects using jtaconfig

JTAGCONFIG=/OPT/altera/12.1/quartus/bin/jtagconfig

/usr/bin/killall jtagd
$JTAGCONFIG
