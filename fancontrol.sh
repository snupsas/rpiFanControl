#!/bin/bash
#

# Some things that run always
set -e
FANCONTROLDIR=<PATH_GOES_HERE>

# Carry out specific functions when asked to by the system
case "$1" in
  start)
    echo "Starting <fanControl>"
    $FANCONTROLDIR/./fanControl $FANCONTROLDIR/fanControl.cfg &>/dev/null &
    ;;
  stop)
    echo "Stopping <fanControl>"
    killall fanControl
    ;;
  *)
    echo "Usage: /etc/init.d/fancontrol {start|stop}"
    exit 1
    ;;
esac

exit 0
