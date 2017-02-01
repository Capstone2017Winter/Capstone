#!/bin/bash
APPDIR="$( cd "$(dirname "${BASH_SOURCE[0]}" )" && pwd)"

$APPDIR/stop_server.sh
$APPDIR/start_server.sh
