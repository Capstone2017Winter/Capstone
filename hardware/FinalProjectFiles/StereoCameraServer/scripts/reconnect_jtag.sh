#Nancy Minderman
# nancy.minderman@ualberta.ca
# nancy.minderman@ece.ualberta.ca
# This script kills all jtagd daemons and reconnects using jtaconfig

JTAGCONFIG=/opt/altera/10.1sp1/quartus/bin/jtagconfig

sudo killall jtagd
sudo $JTAGCONFIG
