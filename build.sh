#!/bin/sh

MD="markdown"
SD="sed"

echo "Building content..."
$MD -v >/dev/null 2>&1 || echo "Cannot find markdown!"
$MD content.markdown > content.html || echo "Content building failed."

echo "Compiling index page..."
$SD --version >/dev/null 2>&1 || echo "Cannot find sed!"
$SD '/content.html/ r content.html' < template.html > index.html || exit 1

echo "All done."

