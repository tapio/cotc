#!/bin/sh
# Script name: generate-version.sh
# Description: Generates version.h file that has a proper version string
# ***
# Note: This is run automatically by CMake, don't do it manually.
# ***

MAJOR=1  # Major release git tag should point to commit updating this
VERSION_H="version.h"

BASECOMMIT=`git show $MAJOR.0 | head -1 | cut -d " " -f 2`
BASECOMMITS=`git log $BASECOMMIT --pretty=oneline | wc -l`""
TOTALCOMMITS=`git log --pretty=oneline | wc -l`""

MINOR=$(($TOTALCOMMITS - $BASECOMMITS))

echo "#pragma once" > $VERSION_H.new
echo "#define VERSION_STRING \"$MAJOR.$MINOR\"" >> $VERSION_H.new

if [ -f $VERSION_H ] ; then
	# Check if they differ (to avoid changing time stamps if not necessary)
	cmp -s $VERSION_H $VERSION_H.new
	if [ $? -eq 1 ] ; then
		mv $VERSION_H.new $VERSION_H
	fi
else
	# Not yet generated
	mv $VERSION_H.new $VERSION_H
fi
rm -f $VERSION_H.new

