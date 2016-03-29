## SHUTDOWN TEST ###
echo "SHUTDOWN TEST"
curl -X GET http://localhost:9090/2 # not found
echo ""
curl -X POST http://localhost:9090/shutdown # shutdown
echo ""
echo ""
sleep 1

### PUT TEST ###
echo "PUT TEST"
curl -X PUT http://localhost:9090/3/4 # put
echo ""
curl -X GET http://localhost:9090/3 # {key: 3, value: 4}
echo ""
curl -X POST http://localhost:9090/shutdown # shutdown
echo ""
echo ""
sleep 1

### UPDATE TEST ###
echo "UPDATE TEST"
curl -X PUT http://localhost:9090/3/4 # put
echo ""
curl -X PUT http://localhost:9090/3/5 # put
echo ""
curl -X GET http://localhost:9090/3 # not found
echo ""
curl -X POST http://localhost:9090/shutdown # shutdown
echo ""
echo ""
sleep 1



### DELETE TEST ###
echo "DELETE TEST"
curl -X PUT http://localhost:9090/3/4 # put
echo ""
curl -X DELETE http://localhost:9090/3 # delete
echo ""
curl -X GET http://localhost:9090/3 # not found
echo ""
curl -X POST http://localhost:9090/shutdown # shutdown
echo ""
echo ""
sleep 1


### TEST HEAD ###
echo "HEAD TEST"
curl -I http://localhost:9090/3 # -X HEAD has strange bug in curl 
curl -X POST http://localhost:9090/shutdown # shutdown
echo ""
echo ""
sleep 1

### TEST MEMSIZE ###
echo "MEMSIZE TEST"
curl -X POST http://localhost:9090/memsize/10 # changed maxmem
echo ""
curl -X PUT http://localhost:9090/3/4  # put
echo ""
curl -X POST http://localhost:9090/memsize/5 # bad request
echo ""
curl -X POST http://localhost:9090/shutdown # shutdown
echo ""
echo ""
sleep 1
