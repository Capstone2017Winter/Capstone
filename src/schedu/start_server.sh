#!/bin/bash
APPNAME=schedu
#APPDIR=/home/ubuntu/Capstone/src/$APPNAME/
APPDIR="$( cd "$(dirname "${BASH_SOURCE[0]}" )" && pwd)"

LOGFILE=$APPDIR'gunicorn.log'
ERRORFILE=$APPDIR'gunicorn-error.log'

NUM_WORKERS=3

ADDRESS=127.0.0.1:8000

cd $APPDIR

exec gunicorn $APPNAME.wsgi:application \
-w $NUM_WORKERS --bind=$ADDRESS \
--log-level=debug \
--log-file=$LOGFILE 2>>$LOGFILE 1>>$ERRORFILE &

