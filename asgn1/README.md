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

