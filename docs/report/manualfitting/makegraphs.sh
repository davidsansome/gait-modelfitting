#!/bin/sh

FILENAME=/tmp/manualerror-$$.g
SET=$1
ALG=$2

if [ -z $SET ]; then
	echo "Usage: $0 setNumber algorithmName"
	exit 1
fi

if [ -z $ALG ]; then
	echo "Usage: $0 setNumber algorithmName"
	exit 1
fi

if [ ! -e "set$SET-manual.dat" ]; then
	echo "set$SET-manual.dat doesn't exist"
	exit 1
fi

if [ ! -e "set$SET-$ALG.dat" ]; then
	echo "set$SET-$ALG.dat doesn't exist"
	exit 1
fi


rm -fv $FILENAME

echo "set term png" >> $FILENAME
echo "set xlabel 'Time'" >> $FILENAME

echo "set output '$SET-$ALG-leftthigh.png'" >> $FILENAME
echo "plot 'set$SET-$ALG.dat' using 1:3 lt 1 title 'Auto', 'set$SET-manual.dat' using 1:3 lt 3 title 'Manual'" >> $FILENAME

echo "set output '$SET-$ALG-leftlower.png'" >> $FILENAME
echo "plot 'set$SET-$ALG.dat' using 1:5 lt 1 title 'Auto', 'set$SET-manual.dat' using 1:5 lt 3 title 'Manual'" >> $FILENAME

echo "set output '$SET-$ALG-rightthigh.png'" >> $FILENAME
echo "plot 'set$SET-$ALG.dat' using 1:7 lt 1 title 'Auto', 'set$SET-manual.dat' using 1:7 lt 3 title 'Manual'" >> $FILENAME

echo "set output '$SET-$ALG-rightlower.png'" >> $FILENAME
echo "plot 'set$SET-$ALG.dat' using 1:9 lt 1 title 'Auto', 'set$SET-manual.dat' using 1:9 lt 3 title 'Manual'" >> $FILENAME

gnuplot $FILENAME

rm -f $FILENAME


cat <<End-of-latex
\begin{figure}[p]
	\centering
	\subfloat[Left thigh]{\label{ManualFit$SET:$ALG:LeftThigh}\includegraphics[width=\graphwidth]{manualfitting/$SET-$ALG-leftthigh.png}}
	\quad
	\subfloat[Right thigh]{\label{ManualFit$SET:$ALG:RightThigh}\includegraphics[width=\graphwidth]{manualfitting/$SET-$ALG-rightthigh.png}}
	\\
	\subfloat[Left lower leg]{\label{ManualFit$SET:$ALG:LeftLower}\includegraphics[width=\graphwidth]{manualfitting/$SET-$ALG-leftlower.png}}
	\quad
	\subfloat[Right lower leg]{\label{ManualFit$SET:$ALG:RightLower}\includegraphics[width=\graphwidth]{manualfitting/$SET-$ALG-rightlower.png}}
	
	\caption{Fitting error for Sample $SET using $ALG.}
	\label{ManualFit$SET:$ALG}
\end{figure}
End-of-latex
