#!/bin/bash

# Variables which have to be defined by the CI
# CIHOME=
# P4_PORT=
# P4_USER=
# P4_PASS=
# P4_CLIENT=

cd $CIHOME/perforce
p4 set P4PORT=$P4_PORT
p4 set P4USER=$P4_USER
p4 set P4PASSWD=$P4_PASS
p4 set P4IGNORE=.p4ignore
p4 set P4CLIENT=$P4_CLIENT
p4 trust -y
p4 sync

cd thor
./extras/bin/win64/FBuild.exe -noprogress -summary dist-clean
./extras/bin/win64/FBuild.exe -noprogress -summary all
./_out/bin/tests.exe

