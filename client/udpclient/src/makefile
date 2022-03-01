#made by shon

#- in case of header files, add them to the changed files, but no compiling
#  needed.
#- dependencies mean that if the file has been modified (even if its using the
#  "touch" command) then it will go into the target's action.

INCLUDE := -I/home/sourcer/repos/udpFileTransfer/AsioProject/3rd_parties/wirehair/include/
WIREHAIR_BUILD := -L/home/sourcer/repos/udpFileTransfer/AsioProject/3rd_parties/wirehair/build/
CXX := g++ -std=c++17 -g
LINK_LIBS := -lpthread -lboost_filesystem -lboost_system -lwirehair
OBJ := client.o client_main.o

client: $(OBJ)
	$(CXX) client.o client_main.o -o client $(WIREHAIR_BUILD) $(LINK_LIBS)

client.o client_main.o: client.cpp client_main.cpp 
	$(CXX) -c client.cpp client_main.cpp $(INCLUDE)  


