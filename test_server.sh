## SHUTDOWN TEST ###
echo "SHUTDOWN TEST"
curl -X GET http://localhost:9090/2
echo ""
curl -X POST http://localhost:9090/shutdown
echo ""
echo ""
sleep 1

### PUT TEST ###
echo "PUT TEST"
curl -X PUT http://localhost:9090/3/4
echo ""
curl -X GET http://localhost:9090/3
echo ""
curl -X POST http://localhost:9090/shutdown
echo ""
echo ""
sleep 1

### DELETE TEST ###
echo "PUT TEST"
curl -X PUT http://localhost:9090/3/4
echo ""
curl -X DELETE http://localhost:9090/3
echo ""
curl -X GET http://localhost:9090/3 # should be error page
echo ""
curl -X POST http://localhost:9090/shutdown
echo ""
echo ""
sleep 1


### TEST HEAD ###
echo "HEAD TEST"
curl -I http://localhost:9090/3 # -X HEAD has strange bug in curl
echo ""
curl -X POST http://localhost:9090/shutdown
echo ""
echo ""
sleep 1

