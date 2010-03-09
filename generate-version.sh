#!/bin/sh
# Script name: generate-version.sh
# Description: Generates version.h file that has a proper version string

VERSION_H="version.h"

echo "#define VERSION_STRING \"0.`git log --pretty=oneline | wc -l`\"" > $VERSION_H.new

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

