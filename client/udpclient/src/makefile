#made by shon

#- in case of header files, add them to the changed files, but no complining
#  needed.
#- dependencies mean that if the file has been modified (even if its using the
#  "touch" command) then it will go into the target's action.

clientOutput: client.o
	g++ -std=c++17 client.o -o client -lpthread -lboost_filesystem -lboost_system

client.o: client.cpp
	g++ -std=c++17 -c client.cpp -I/home/sourcer/repos/udpFileTransfer/3rd_parties/boost_1_77_0/ 

clean:
	rm *.o client_main                                                                           
