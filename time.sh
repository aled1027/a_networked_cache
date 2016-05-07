ITERS=5

curl -X PUT http://localhost:8080/alex/ledger
echo ""

for i in `seq 1 $ITERS`;
do
    #echo -n "alex" | nc -4u -w0.1  localhost 8081
    echo -n "alex" | nc -4u -w1  localhost 8081
    echo ""
done

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



