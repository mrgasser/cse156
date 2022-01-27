# cse156
Projects for 156

ASGN 1:

asgn1 Readme

Design
1. Create socket from program args
    - check for socket formation errors
2. form request
3. send request over socket
    - check for send errors
4. receive response from server
    - check for errors in reponse
5. Write response to output file
 

Notes:
- For arguments, loop through argv, build up argument string, (ommiting -h), then using sscanf parse args into, host name, ip, etc..
- it should work with just the root directory (No resource was given)
    - looks life if no resource given, index.html is the resource
- Buffer size 256
- Errors printed to STDerr

ERROR CHECKING:
- Check IP address is valid
- Check for port
- check socket creation
- check connection 

WHEN SUBMitting
- Submit tar.gz compressed file
- format lab1-jbyron.tar.gz
- https://www.howtogeek.com/248780/how-to-compress-and-extract-files-using-the-tar-command-on-linux/

Resources:
- referenced for creating socket https://www.geeksforgeeks.org/socket-programming-cc/
- Response code taken from cse130 Lab1 


Questions: 
- Will inputs always be structured the same.
- dont need to check the URL

- it should work with just the root directory

- How should we handle output.dat? create it each time, or assume that the file already exists, should we truncate or overide


ssrc-nas-1.soe.ucsc.edu:8080

bin = empty
doc = README.md 
src = 
Makefile - should place executable in bin

type make
then bin/my
output.dat goes to pwd


Testing:

./bin/myweb www.neverssl.com 13.35.121.94:80

./bin/myweb ssrc-nas-1.soe.ucsc.edu 128.114.52.55:8080/

./bin/myweb google.com 132.251.32.46:80

./bin/myweb softwareqatest.com 216.92.49.183

./bin/myweb www.testingmcafeesites.com 23.45.205.250:80

all the above also done with -h 


ASGN 2:

Questions:
- Are the program arguments going to allways be in said order?


Things needed to add:
FORMAT
- (DONE) item

client:
- make sure packets are in correct order
- detect packet loss, exit with failure
- (DONE) add timeout on both reading and writing to socket
- client must not hang if server is down

server:

other:
- Documentation
    - Mechanism for splitting file into packets
    - protocol for reordering the orignal file


Test Cases:
- ./myclient 127.0.0.1 8888 25 testfiles/longfile.txt output.txt 
- ./myclient 127.0.0.1 8888 1500 testfiles/hello.txt output.txt 
- ./myclient 52.43.121.77 10001 1500 ./testfiles/hello.txt output.txt
- ./myclient 127.0.0.1 8080 22 testfiles/longfile.txt output2.txt and ./myclient 127.0.0.1 8080 1500 ./testfiles/hello.txt output1.txt ran simultaneously
- ./myclient 52.43.121.77 10001 24 ./testfiles/longfile.txt output.txt



