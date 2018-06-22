all: server client

server: Server/server.c Server/main.c Server/sqlite3.c
	gcc -o server Server/server.c Server/main.c Server/sqlite3.c -lpthread -ldl -Wall -pedantic

client: Client/client.c Client/main.c Client/flight.c
	gcc -o client Client/client.c Client/main.c Client/flight.c -Wall -pedantic

clean:
	rm server client
