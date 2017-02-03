#!/bin/bash
APPDIR="$( cd "$(dirname "${BASH_SOURCE[0]}" )" && pwd)"

echo "restarting gunicorn..."
$APPDIR/stop_server.sh
$APPDIR/start_server.sh

echo "restarting nginx..."
exec sudo service nginx restart
