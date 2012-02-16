#!/bin/bash

echo "
\documentclass{minimal}
\usepackage{tikz}
\usepackage{amsmath}
\usepackage{amsfonts}
\usepackage{amsthm}
\usepackage{dsfont}
\usepackage{txfonts}
\usetikzlibrary{backgrounds,positioning,shapes,shadows,arrows,calc,fit}
\def\huge{\fontsize{15pt}{17pt}\selectfont} 
\setlength{\textwidth}{40cm}
\setlength{\textheight}{40cm}
\setlength{\pdfpagewidth}{40cm}
\setlength{\pdfpageheight}{40cm}
\begin{document}
\definecolor{awhite}{RGB}{254,254,254}
\definecolor{acgorange}{RGB}{240,112,48}
\tikzstyle{blob}=[circle,draw=awhite,fill=awhite,inner sep=2pt]
\tikzstyle{bedge}=[very thick,draw=awhite,bend left]
\tikzstyle{edge}=[very thick,draw=awhite]
\def\v#1{\mathbf{#1}}
\def\R{\mathds{R}}
" > tempfile.tex

cat $1 >> tempfile.tex

echo "\end{document}" >> tempfile.tex

#pdffile=`echo $1 | cut -f1 -d. -`.txt

name=${1%\.*}

pdflatex tempfile.tex && pdfcrop tempfile.pdf ${name}.pdf

rm -f tempfile.*
