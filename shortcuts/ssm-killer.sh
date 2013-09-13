#!/bin/bash

declare -i isalive
ret=`ps -a | grep "ssm-coordinator" | wc -l`
isalive=`expr $ret`
cnt=`expr 0`


echo "ssm-killer"
if [ $isalive -eq 0 ] ; then
	echo 'ssm-coordinator is not operating'
	echo 'Finish'
	exit 0
fi

while [ $cnt -le 5 ]
do
	cnt=`expr $cnt + 1`

	echo "killssm ... $cnt"
	killssm
	
	ret=`ps -a | grep "ssm-coordinator" | wc -l`
	isalive=`expr $ret`

	if [ $isalive -eq 0 ]; then
		echo "ssm-coordineter kill ... OK"
		break
	fi
	
	sleep 1
done


if [ $isalive -eq 0  ]; then
	echo ""
	while true
	do 
		echo "kill terminator? ... yes/no"
		read yn

		if [ -z $yn ]; then 
			continue
		elif [ $yn = YES -o $yn = Yes -o $yn = yes -o $yn = Y -o $yn = y ]; then
			killall bash
			break
		elif [ $yn = NO -o $yn = No -o $yn = no -o $yn = N -o $yn = n ]; then
			break
		fi
	done 
	
	echo "Finish"
else
	echo "Error: fail to killssm"
fi


exit 0

