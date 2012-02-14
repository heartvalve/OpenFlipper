#!/bin/bash

name=${1%\.*}
convert -density 300x300 ${name}.pdf -resize ${2}x -density 300x300 -quality 90 ${name}.png
