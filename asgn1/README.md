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

