#!/bin/bash
#
### BEGIN INIT INFO
# Provides:          fancontrol
# Required-Start:    $remote_fs $syslog
# Required-Stop:     $remote_fs $syslog
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: Example initscript
# Description:       This file should be used to construct scripts to be
#                    placed in /etc/init.d.  This example start a
#                    single forking daemon capable of writing a pid
#                    file.  To get other behavoirs, implemend
#                    do_start(), do_stop() or other functions to
#                    override the defaults in /lib/init/init-d-script.
### END INIT INFO

# Some things that run always
set -e
FANCONTROLDIR=/home/benas/gitSources/C/rpiFanControl

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
    echo "Usage: /etc/init.d/blah {start|stop}"
    exit 1
    ;;
esac

exit 0
