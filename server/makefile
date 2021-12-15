#made by shon

#- in case of header files, add them to the changed files, but no complining
#  needed.
#- dependencies mean that if the file has been modified (even if its using the
#  "touch" command) then it will go into the target's action.

serverOutput: server.o
	g++ server.o -o server -lpthread

server.o: server.cpp 
	g++ -c server.cpp -I/home/sourcer/repos/udpFileTransfer/3rd_parties/boost_1_77_0/ 

clean:
	rm *.o server

