ITERS=10
tgt_diff=1000000000 # in nanoseconds
sum=0

curl -X PUT http://localhost:8080/alex/ledger

# Time UDP
ts=$(date +%s%N); 
for i in `seq 1 $ITERS`;
do
    # not quite right
    start_time=$(date +%s%N); 
    echo -n "alex" | nc -4u -w0  localhost 8081
    time_taken=$(($(date +%s%N) - start_time))
    sum=$((sum + time_taken))

    # this seems wrong
    sleep_time=$((tgt_diff - time_taken))
    echo $sleep_time
    if [ "$sleep_time" -gt "0" ]
    then
        sleep_time=$(echo "scale=4; $sleep_time / 1000000000" | bc)
        sleep $sleep_time
        echo $sleep_time
    fi

done
avg_time=$(echo "scale=4; $sum / $ITERS" | bc)
echo "UDP Time ta+ diffken: $sum";
echo "UDP Mean Time taken: $avg_time"

# Time TCP
#ts=$(date +%s%N); 
#for i in `seq 1 $ITERS`;
#do
#    curl -s -X GET http://localhost:8080/alex > /dev/null
#done
#tt=$((($(date +%s%N) - $ts)/1000000000)); 
#echo "TCP Time taken: $tt"
#echo -n "TCP Mean Time taken:"
#echo $((tt / ITERS))


#curl -X POST http://localhost:8080/shutdown 



