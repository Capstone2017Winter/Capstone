#!/bin/bash
APPNAME=schedu
#APPDIR=/home/ubuntu/Capstone/src/$APPNAME/
APPDIR="$( cd "$(dirname "${BASH_SOURCE[0]}" )" && pwd)"

LOGFILE=$APPDIR'gunicorn.log'
ERRORFILE=$APPDIR'gunicorn-error.log'

NUM_WORKERS=3

ADDRESS=0.0.0.0:8000

cd $APPDIR

echo "starting server..."

exec gunicorn $APPNAME.wsgi:application \
-w $NUM_WORKERS --bind=$ADDRESS \
--log-level=debug \
--log-file=$LOGFILE 2>>$LOGFILE 1>>$ERRORFILE &

