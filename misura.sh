#!/bin/env bash

CLI_NUM=$1

declare -a clients
declare -a correct
declare -a estimated
declare -a sum_value
declare -a max_value
declare -a min_value
for i in $(seq 1 $CLI_NUM);
do
    val="$(head -n 1 "client-$i.log" | awk -F' ' '{print $2 }{print $4}')"
    clients+=($val)
done

for i in $(seq 0 $(($CLI_NUM-1)));
do
    correct[i]=0
    estimated[i]=0
    sum_value[i]=0
    max_value[i]=0
    min_value[i]=1000000
done

OLDIFS=$IFS;
IFS=$'\n';

for line in $(cat supervisor.log);
do
    if [ $(echo "$line" | wc -w ) -eq 8 ]; then
        key="$(echo /"$line/" | awk -F' ' '{ print $5}')"
        val="$(echo /"$line/" | awk -F' ' '{ print $3}')"
        for index in "${!clients[@]}"
        do
            if [ $((index%2)) -eq 0 ]; then
                if [ "$key" == "${clients[index]}" ]; then
                    sum_value[$index/2]=$((${sum_value[$index/2]} + ${clients[index+1]}))
                    max_value[$index/2]=$(( $val > ${max_value[$index/2]} ? $val : ${max_value[$index/2]} ))
                    min_value[$index/2]=$(( $val < ${min_value[$index/2]} ? $val : ${min_value[$index/2]} ))
                    if [ $val -lt $((${clients[index+1]} + 25 )) ] && [ $val -gt $((${clients[index+1]} - 25 )) ]; then
                        echo "$key : la stima $val è corretta "
                    else
                        echo "$key : la stima corretta è ${clients[index+1]} non $val . Errore del $(($val / ${clients[index+1]} * 100))% "
                    fi
                    break
                fi
            fi
        done
    else 
        if [ $(echo "$line" | wc -w ) -eq 7 ]; then
            key="$(echo /"$line/" | awk -F' ' '{ print $5}')"
            val="$(echo /"$line/" | awk -F' ' '{ print $3}')"
            for index in "${!clients[@]}"
            do
                if [ $((index%2)) -eq 0 ]; then
                    if [ "$key" == "${clients[index]}" ]; then
                        sum_value[$index/2]=$((${sum_value[$index/2]} + ${clients[index+1]}))
                        max_value[$index/2]=$(( $val > ${max_value[$index/2]} ? $val : ${max_value[$index/2]} ))
                        min_value[$index/2]=$(( $val < ${min_value[$index/2]} ? $val : ${min_value[$index/2]} ))
                        if [ $val -lt $((${clients[index+1]} + 25 )) ] && [ $val -gt $((${clients[index+1]} - 25 )) ]; then
                            ((correct[$index/2]++))
                            ((estimated[$index/2]++))
                        else
                            ((estimated[$index/2]++))
                        fi
                        break
                    fi
                fi
            done
        fi
    fi
done

for i in $(seq 0 $(($CLI_NUM-1)));
do
    echo -e "\n${clients[i*2]} STATISTICHE : "
    echo -e "\n - Il valore corretto è ${clients[$i*2 + 1]}"
    echo " - La stima media è $((${sum_value[i]}/${estimated[i]}))"
    echo " - Il valore massimo è ${max_value[i]}"
    echo " - Il valore minimo è ${min_value[i]}"
    eass=$(( $(( ${max_value[i]} + ${min_value[i]} )) / 2 ))
    echo " - L'errore assoluto è $eass"
    percentage=$( echo "scale=2;$eass/(${sum_value[i]}/${estimated[i]})" | bc )
    echo " - L'errore relativo è $percentage"
    percentage=$( echo "scale=1;${correct[$i]}/${estimated[$i]}*100" | bc )
    echo " - E' stato stimato correttamente il $percentage% (${correct[$i]} , ${estimated[$i]})"
done

IFS=$OLDIFS
