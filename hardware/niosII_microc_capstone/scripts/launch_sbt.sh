#Nancy Minderman
# nancy.minderman@ualberta.ca
# nancy.minderman@ece.ualberta.ca
# Use this script to launch the tools. 
# Sample usage ./scripts/launch_sbt.sh
#!/bin/bash

NIOS2_IDE=/OPT/altera/12.1/nios2eds/bin/eclipse-nios2
WORKSPACE_PATH=software
CONFIGURATION_PATH=$HOME/.eclipse/nios2-eclipse-12.1
# export SOPC_KIT_NIOS2=/OPT/altera/12.1/tools/nios2eds

if [ -d $WORKSPACE_PATH ]; then
$NIOS2_IDE -data $WORKSPACE_PATH -configuration $CONFIGURATION_PATH &
else 
echo -e "Could not find software directory...\n"
$NIOS2_IDE -configuration $CONFIGURATION_PATH &
fi
