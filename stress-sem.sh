#!/bin/bash
#
# Stress benchmark
#
# This benchmark is run as a oneshot systemd.service and outputs to the tty
echo "## Starting stress tests"
echo "stress-ng: semaphores"
stress-ng --sem 4 --sem-ops 100000

echo "## Testing complete, shutting down"
shutdown -h now
