all:  main

main: main.o cuckoo.o lsh.o nest.o similarity.o net_server.o net_utility.o net_wrap.o
	gcc -o  main main.o nest.o cuckoo.o lsh.o similarity.o net_server.o net_utility.o net_wrap.o -lm -lflann -lpthread

#client: net_client.o net_wrap.o
#	gcc -o  client net_client.o net_wrap.o

lsh.o: lsh.c lsh.h
	gcc -c lsh.c

cuckoo.o: cuckoo.c cuckoo.h lsh.h
	gcc -c cuckoo.c

nest.o: nest.c nest.h lsh.h cuckoo.h
	gcc -c nest.c

similarity.o: similarity.c similarity.h
	gcc -c similarity.c

net_wrap.o: net_wrap.c net_wrap.h 
	gcc -c net_wrap.c

net_utility.o: net_utility.c net_utility.h net_wrap.h net_headers.h
	gcc -c net_utility.c

net_server.o: net_server.c net_server.h net_utility.h net_wrap.h net_headers.h
	gcc -c net_server.c

main.o: main.c nest.h lsh.h cuckoo.h net_server.h
	gcc -c main.c

#net_client.o: net_client.c net_headers.h net_wrap.h
#	gcc -c net_client.c

clean:
	rm -r *.o
