## File Sharing

simple programs for sharing a file from the server to only one client


The server listens on the give port to send the specified file to the client,
and the client connects to that server by specifying the IP address of the server and the port number to get the file from the server and saving it to the specified file .

NOTE: server and client apps are configured to run on the same machine, in order to change it to run on a real server, uncomment line 52 and make the first argument of getaddrinfo() function NULL(in line 56 of server.c file).


### Compilation and Execution

* to compile the programs navigate to the directory containing the source code, then simply run :
```
gcc -o server server.c
gcc -o client client.c
```

* open two terminals and enter the below commands in seperate terminals:

* server:
```
./server [ Port ] [ File ]
```

* client:
```
./server [ Port ] [ File ]
```



## License

These programs are licensed under the MIT License
