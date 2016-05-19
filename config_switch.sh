#!/bin/bash

# USAGE NOTES
# You'll want to copy and paste this output into a serial
# terminal after issuing the `conf t` command
# 
# If you want to blank the configuration prior to reconf,
# you can use the write erase command and reboot

usage () {
    echo "USAGE: $1 [ CENTRAL | RACK ]" 1>&2
    echo "See comments in file for details" 1>&2
    exit 1
}

output_trunk () {
    echo interface faste$1
    echo no shutdown
    echo switchport mode trunk
    echo end
}

output_access () {
    echo interface faste$1
    echo no shutdown
    echo switchport mode access
    echo switchport access vlan $2
    echo end
}

if [ $# != 1 ] ; then
    usage $0
fi
if [ "x$1" != xCENTRAL ] && [ "x$1" != xRACK ] ; then
    usage $0
fi

OUTSIDE_VLAN=201
WARROOM_VLAN=202
SERVER_VLAN=203

for i in `seq 0 2 11` ; do
    # Top row, far left group
    output_trunk $i
done

for i in `seq 1 2 11` ; do
    # Bottom row, far left group
    output_access $i $OUTSIDE_VLAN
done

for i in `seq 12 2 23` ; do
    # Top row, center left group
    output_access $i $WARROOM_VLAN
done

for i in `seq 13 2 23` ; do
    # Bottom row, center left group
    output_access $i $SERVER_VLAN
done

for i in `seq 24 35` ; do
    # Center right group
    if [ "x$1" != xCENTRAL ] ; then
        # RACK configuration
        output_access $i $SERVER_VLAN
    else
        # CENTRAL configuration
        output_access $i $WARROOM_VLAN
    fi
done

# Ports 36-47 are reserved for CDX
if [ "x$1" != xCENTRAL ] ; then
    # Put CDX configuration for RACK switch here
    echo # Blank line to keep bash from choking
else
    # Put CDX configuration for CENTRAL switch here
    echo # Blank line to keep bash from choking
fi
