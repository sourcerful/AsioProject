#made by shon

#- in case of header files, add them to the changed files, but no complining
#  needed.
#- dependencies mean that if the file has been modified (even if its using the
#  "touch" command) then it will go into the target's action.

clientOutput: client.o
	g++ client.o -o client -lpthread

client.o: client.cpp 
	g++ -c client.cpp -I/home/sourcer/repos/udpFileTransfer/3rd_parties/boost_1_77_0/ 

clean:
	rm *.o client



