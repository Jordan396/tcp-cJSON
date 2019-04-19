all: client server

client: ./client.c ../cJSON.c ../tcp_cJSON.c
	gcc ./client.c ../cJSON.c ../tcp_cJSON.c -o client

server: ./server.c ../cJSON.c ../tcp_cJSON.c
	gcc ./server.c ../cJSON.c ../tcp_cJSON.c -o server