#Nancy Minderman
# nancy.minderman@ualberta.ca
# nancy.minderman@ece.ualberta.ca
# January 2011
# Use this script to launch the tools. 
# Note that the software directory must exist at the same level as this script to function correctly.
#!/bin/bash

NIOS2_IDE=/opt/altera/10.1sp1/nios2eds/bin/nios2-ide
WORKSPACE_PATH=software
export SOPC_KIT_NIOS2=/opt/altera/10.1sp1/nios2eds

if [ -d $WORKSPACE_PATH ]; then
$NIOS2_IDE -data $WORKSPACE_PATH &
else 
echo -e "Could not find software directory...\n"
$NIOS2_IDE &
fi
