#!/bin/bash
#Nancy Minderman
# nancy.minderman@ualberta.ca
# March 8, 2011
# This script lanches quartus without modifying the users path. 
# March 7, 2012
# Added the environment variable for the new licensing

QUARTUS=/OPT/altera/12.1/quartus/bin/quartus
export QUARTUS_ROOTDIR_OVERRIDE=/OPT/altera/12.1/quartus
# Change the line below to match the name of the quartus project. 
# It should also be the name of the top level entity as well. 
# I also name  my working directory the same way.

TOP_LEVEL=
export LM_LICENSE_FILE=12000@lic.ece.ualberta.ca

if [ -f $TOP_LEVEL.qpf ]; then
 	PROJECT_FILE=$TOP_LEVEL.qpf
else
	echo -e "Could not find Quartus project file... \n"
	PROJECT_FILE=""
fi



$QUARTUS $PROJECT_FILE &
