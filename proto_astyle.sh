#!/bin/bash

if [ $# -gt 0 ]
then
gcc -E $1 -P -nostdinc | astyle
else
gcc -E *.h -P -nostdinc | astyle
fi

