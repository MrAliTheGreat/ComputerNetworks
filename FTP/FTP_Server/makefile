CA4_Ali.out : mainServer.o Server.o client.o System.o User.o
	      g++ -std=c++11 -o mainServer mainServer.o Server.o User.o System.o
	      g++ -std=c++11 -o client client.o 

mainServer.o: mainServer.cpp Server.hpp
	g++ -c mainServer.cpp

Server.o: Server.cpp User.hpp System.hpp
	g++ -c Server.cpp

client.o: client.cpp
	g++ -c client.cpp

User.o: User.cpp User.hpp
	g++ -c User.cpp

System.o: System.cpp System.hpp User.hpp
	g++ -c System.cpp

clean:
	rm *.o
