# $Copyright
# Copyright 1992, 1993, 1994, 1995 Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
:
#  senscode.sh
#
#set -x

if [ $# -eq 0 ]; then
  cat ./senscode.lst | nawk -f ./senscode.awk 2>&1 | pg1
else
  cat ./senscode.lst | nawk -f ./senscode.awk > senscode._c
fi
