#!/bin/bash
#Nancy Minderman
# nancy.minderman@ualberta.ca
# nancy.minderman@ece.ualberta.ca
# March 8, 2011
# This script lanches quartus without modifying the users path. 
# I'll probably need to add /opt/altera/10.1sp1/quartus/bin and others later
# March 7, 2012
# Added the environment variable for the new licensing

QUARTUS=/opt/altera/10.1sp1/quartus/bin/quartus
# Change the line below to match the name of the quartus project. It should also be the name of the top level entity as well. I also name  my working directory the same way.

TOP_LEVEL=
export LM_LICENSE_FILE=12000@lic.ece.ualberta.ca
if [ -f $TOP_LEVEL.qpf ]; then
 	PROJECT_FILE=$TOP_LEVEL.qpf
else
	echo -e "Could not find Quartus project file... \n"
	PROJECT_FILE=""
fi



$QUARTUS $PROJECT_FILE &
