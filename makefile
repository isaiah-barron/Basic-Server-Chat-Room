all: ./bin/server_chat ./bin/client_chat

bin/server_chat: bin/server.o bin/wrapnetwork.o bin/LinkedList.o
	gcc -o bin/server_chat bin/server.o bin/wrapnetwork.o bin/LinkedList.o -lpthread -g

bin/client_chat: bin/client.o bin/wrapnetwork.o
	gcc -o bin/client_chat bin/client.o bin/wrapnetwork.o -lpthread -lreadline -g

bin/client.o: src/client.c src/client.h
	gcc -c src/client.c -g -o bin/client.o

bin/server.o: src/server.c src/server.h
	gcc -c src/server.c -g -o bin/server.o

bin/LinkedList.o: src/LinkedList.c src/LinkedList.h
	gcc -c src/LinkedList.c -g -o bin/LinkedList.o

bin/wrapnetwork.o: src/wrapnetwork.c src/wrapnetwork.h
	gcc -c src/wrapnetwork.c -g -o bin/wrapnetwork.o

clean:
	rm ./bin/*.o
	rm ./bin/server_chat
	rm ./bin/client_chat

memcheck:
	valgrind -v --show-leak-kinds=all --leak-check=full --track-origins=yes ./bin/server_chat
	valgrind -v --show-leak-kinds=all --leak-check=full --track-origins=yes ./bin/client_chat
