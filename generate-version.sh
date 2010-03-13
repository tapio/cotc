#!/bin/sh
# Script name: generate-version.sh
# Description: Generates version.h file that has a proper version string
# ***
# Note: This is run automatically by CMake, don't do it manually.
# ***

VERSION_H="version.h"

V=`git log --pretty=oneline | wc -l`""
V=`echo "$V-139"|bc`
echo "#define VERSION_STRING \"1.$V\"" > $VERSION_H.new

if [ -f $VERSION_H ] ; then
	# Check if they differ
	cmp -s $VERSION_H $VERSION_H.new
	if [ $? -eq 1 ] ; then
		mv $VERSION_H.new $VERSION_H
	fi
else
	# Not yet generated
	mv $VERSION_H.new $VERSION_H
fi
rm -f $VERSION_H.new

