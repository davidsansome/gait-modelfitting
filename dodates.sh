#!/bin/bash

F=dates
svn log . | grep '^r...' | awk '{print $5}' > $F


function count {
	d1=`egrep $1-$2-0?\`expr $3 \* 7 + 1\`\$ $F | wc -l`
	d2=`egrep $1-$2-0?\`expr $3 \* 7 + 2\`\$ $F | wc -l`
	d3=`egrep $1-$2-0?\`expr $3 \* 7 + 3\`\$ $F | wc -l`
	d4=`egrep $1-$2-0?\`expr $3 \* 7 + 4\`\$ $F | wc -l`
	d5=`egrep $1-$2-0?\`expr $3 \* 7 + 5\`\$ $F | wc -l`
	d6=`egrep $1-$2-0?\`expr $3 \* 7 + 6\`\$ $F | wc -l`
	d7=`egrep $1-$2-0?\`expr $3 \* 7 + 7\`\$ $F | wc -l`
	d8=0
	d9=0
	d10=0

	if [ $3 == 3 ]; then
		d8=`grep $1-$2-0?\`expr $3 \* 7 + 8\`\$ $F | wc -l`
		d9=`grep $1-$2-0?\`expr $3 \* 7 + 9\`\$ $F | wc -l`
		d10=`grep $1-$2-0?\`expr $3 \* 7 + 10\`\$ $F | wc -l`
	fi

	tot=`expr $d1 + $d2 + $d3 + $d4 + $d5 + $d6 + $d7 + $d8 + $d9 + $d10`
	echo -n $tot
}

function domonth {
	w1=`count $1 $2 0`
	w2=`count $1 $2 1`
	w3=`count $1 $2 2`
	w4=`count $1 $2 3`

	tot=`expr $w1 + $w2 + $w3 + $w4`
	echo -n "$tot,"
}

domonth 2007 08
domonth 2007 09
domonth 2007 10
domonth 2007 11
domonth 2007 12
domonth 2008 01
domonth 2008 02
domonth 2008 03
domonth 2008 04

echo
