#made by shon

#- in case of header files, add them to the changed files, but no complining
#  needed.
#- dependencies mean that if the file has been modified (even if its using the
#  "touch" command) then it will go into the target's action.

INCLUDE = -I/home/sourcer/repos/udpFileTransfer/3rd_parties/boost_1_77_0/
OBJ = server.o server_main.o

serverOutput: $(OBJ)
	g++ -std=c++17 server_main.cpp server.cpp -o udpServer -lpthread

server.o: server.cpp server_main.cpp 
	g++ -std=c++17 -c server.cpp server_main.cpp $(INCLUDE)  

clean:
	rm *.o server server.cpp

