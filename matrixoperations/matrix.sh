#!/bin/bash

# Kenny Ngo
# CS 344 - Operating Systems
# Program 1 - Matrix Operations
# This program performs calculations on one or two matrices.


# prints out the dimensions (LxW) of a matrix
function dims
{
	#check num arguments
	if [ $# -gt 1 ]
	then
		echo "invalid number of arguments" >&2
		exit 1
	fi

	#check file readable
	if [ "$1" != "" ] && [ ! -r "$1" ]
	then
		echo "not a valid file">&2
		exit 1
	fi

	wcOut=($(wc $1))
	rowNum=${wcOut[0]}
	colNum=$((${wcOut[1]}/$rowNum))
	
	echo "$rowNum $colNum" >&1
}

# reflects the elements of a matrix across the diagonal
function transpose
{
	#check num arguments
	if [ $# -gt 1 ]
	then
		echo "invalid number of arguments" >&2
		exit 1
	fi

	#check file readable
	if [ "$1" != "" ] && [ ! -r "$1" ]
	then
		echo "not a valid file">&2
		exit 1
	fi

	wcOut=($(wc $1))
	rowNum=${wcOut[0]}
	numRows=$((${wcOut[1]}/$rowNum))

	#doing '\c' was a lifesaver
	for ((i=1;i<="$numRows";i++))
	do
		cut -f$i $1 | paste -s >> "temp1$$"
		echo -e '\c' >> "temp1$$"
	done
	cat "temp1$$"
	rm -f "temp1$$"
}

# calculates the average of each column
function mean
{
	#check num arguments
	if [ $# -gt 1 ]
	then
		echo "invalid number of arguments" >&2
		exit 1
	fi

	#check file readable
	if [ "$1" != "" ] && [ ! -r "$1" ]
	then
		echo "not a valid file">&2
		exit 1
	fi
	
	wcOut=($(wc $1))
	rowNum=${wcOut[0]}
	colNum=$((${wcOut[1]}/$rowNum))

	for (( i=1; i<=colNum; i++ ))
	do
		cut -f$i $1 > "tempCol$$"
		sum=0
		while read n
		do
			if [ "$n" != "" ]
			then
				sum=$(($sum + $n))
			fi
		done < "tempCol$$"

		mean=$((($sum + ($rowNum/2) * (($sum>0)*2-1)) / $rowNum))
		echo -n "$mean" >> "tempRow$$"
		if [ $i -ne $colNum ]
		then
			echo -e -n '\t' >> "tempRow$$"
		else
			echo -e "" >> "tempRow$$"
		fi
	done
	cat "tempRow$$"
	rm -f "tempRow$$"
	rm -f "tempCol$$"
}

# adds one matrix to another
function add
{
	#check arguments
	if [ $# -lt 2 ]
	then
		echo "invalid number of arguments" >&2
		exit 1
	fi

	#check readibility
	if ([ "$1" != "" ] && [ ! -r "$1" ]) || ([ "$2" != "" ] && [ ! -r "$2" ])
	then
		echo "not a valid file">&2
		exit 1
	fi

	#checks compatibility
	dims1=$(dims $1)
	dims2=$(dims $2)
	if [ "$dims1" != "$dims2" ]
	then
		echo "adding incompatible matrices" >&2
		exit 1
	fi

	wcOut=($(wc $1))
	rowNum=${wcOut[0]}
	colNum=$((${wcOut[1]}/$rowNum))

	for (( i=1; i<=rowNum; i++ ))
	do
		head -$i $1 | tail -1 > "temp1$$"
		head -$i $2 | tail -1 > "temp2$$"
		
		num1=0
		num2=0 
		sum=0
		for (( j=1; j<=colNum; j++ ))
		do
			num1=$(cut -f$j "temp1$$")
			num2=$(cut -f$j "temp2$$")
			sum=$(( $num1 + $num2 ))
			echo -n "$sum" >> "temp3$$"
			if [ $j -ne $colNum ]
			then
				echo -e -n '\t' >> "temp3$$"
			else
				echo -e "" >> "temp3$$"
			fi
		done
	done
	cat "temp3$$"
	rm -f "temp1$$"
	rm -f "temp2$$"
	rm -f "temp3$$"
}

# multiplies one matrix by another
function multiply
{
	#check num arguments
	if [ $# -lt 2 ]
	then
		echo "invalid number of arguments" >&2
		exit 1
	fi

	#checks readability
	if ([ "$1" != "" ] && [ ! -r "$1" ]) || ([ "$2" != "" ] && [ ! -r "$2" ])
	then
		echo "not a valid file">&2
		exit 1
	fi

	#checks compatibility
	wcOut1=($(wc $1))
	row1Num=${wcOut1[0]}
	col1Num=$((${wcOut1[1]}/${wcOut1[0]}))
	wcOut2=($(wc $2))
	row2Num=${wcOut2[0]}
	col2Num=$((${wcOut2[1]}/${wcOut2[0]}))
	if [ "$col1Num" -ne "$row2Num" ]
	then
		echo "multiplying incompatible matrices" >&2
		exit 1
	fi

	for (( i=1; i<=$row1Num; i++ ))
	do
		head -n$i $1 | tail -n1 > "temp1$$"
		sum=0
		for (( j=1; j<=$row1Num; j++ ))
		do
			cut -f$j $2 | paste -s > "temp2$$"

			for (( k=1; k<=$row2Num; k++ ))
			do
				num1=$(cut -f$k "temp1$$")
				num2=$(cut -f$k "temp2$$")
				if [ -z "$num1" ]
				then	
					num1=0
				elif [ -z "$num2" ]
				then
					num2=0
				fi
				product=$(( $num1 * $num2 ))

				sum=$(($sum + $product))
			done
			echo -n "$sum" >> "temp3$$"
			if [ $j -ne $row1Num ]
			then
				echo -e -n '\t' >> "temp3$$"
			else
				echo -e "" >> "temp3$$"
			fi
			sum=0
		done
	done
	cat "temp3$$"
	rm -f "temp1$$"
	rm -f "temp2$$"
	rm -f "temp3$$"
}

# helps read stdin
function readStdin
{
	while read line
	do
		echo "$line" >> "$1"
	done < "/dev/stdin"
}

# checks for bad commands
case $1 in
    dims|transpose|mean|add|multiply)
    	#do nothing
    	;;
    *)             
    	echo "bad command" >&2
    	exit 1
    	;;
esac

# checks number of arguments and what to do per amount of args
if [ $# -gt 3 ] || [ $# -eq 0 ]
then 
	echo "invalid number of arguments" >&2
	exit 1
elif [ $# -eq 1 ]
then
	case $1 in
	    add|multiply)
	    	echo "bad args" >&2
	    	exit 1
			;;
	    *)	#particularly proud of the way i handled stdin input by passing it as an argument
			fileTemp="file$$"
			readStdin "$fileTemp"
			if [ "$fileTemp" != "" ] && [ ! -r "$fileTemp" ]
			then
				echo "not a valid file">&2
				rm -f "$fileTemp"
				exit 1
			fi
	    	;;
	esac
fi

# no matter if reading from a file or stdin, the functions called are unaffected
if [ $# -eq 1 ]
then
	$1 "$fileTemp"
	rm -f "$fileTemp"
else
	$1 "${@:2}"
fi
exit 0

# remove temp files
trap 'rm -f $fileTemp' INT HUP TERM
trap 'rm -f *$$' INT HUP TERM