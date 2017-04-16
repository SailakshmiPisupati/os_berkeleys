peer:
	gcc peer2peer.cpp -o peer2peer.out -lpthread
rundaemon:
	./peer2peer.out 2 coordinator 8098 8097
runpeer1:
	./peer2peer.out 2 notcorord 8097 8095
runpeer2:
	./peer2peer.out 2 notcorord 8097 8094
clean:
	rm *.out

	./peer2peer.out 2 coordinator 8097