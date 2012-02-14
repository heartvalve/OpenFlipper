#!/bin/bash

name=${1%\.*}
convert -density 600x600 ${name}.pdf -resize ${2}x  -quality 90 ${name}.png
