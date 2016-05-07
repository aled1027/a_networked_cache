ITERS=5

curl -X PUT http://localhost:8080/alex/ledger

# Time UDP
ts=$(date +%s%N); 
for i in `seq 1 $ITERS`;
do
    echo -n "alex" | nc -4u -w1  localhost 8081
    echo ""
done
tt=$((($(date +%s%N) - $ts)/1000000)); 
echo "UDP Time taken: $tt";
echo "UDP Mean Time taken:"
echo $((tt / ITERS))

# Time TCP
ts=$(date +%s%N); 
for i in `seq 1 $ITERS`;
do
    curl -X GET http://localhost:8080/alex
    echo ""
done
tt=$((($(date +%s%N) - $ts)/1000000)); 
echo "TCP Time taken: $tt"
echo -n "TCP Mean Time taken:"
echo $((tt / ITERS))



