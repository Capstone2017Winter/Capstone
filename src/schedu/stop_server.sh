#!/bin/bash
echo "stopping gunicorn server..."
`pkill gunicorn`

exec sudo service nginx stop
