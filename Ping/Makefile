ping: main.cpp ping.o icmp_hdr.o ip_hdr.o ping.h
	g++ main.cpp ping.o icmp_hdr.o ip_hdr.o -o ping

ping.o: ping.cpp ip_hdr.h icmp_hdr.h
	g++ -c ping.cpp ip_hdr.h icmp_hdr.h

icmp_hdr.o: icmp_hdr.cpp icmp_hdr.h
	g++ -c icmp_hdr.cpp

ip_hdr.o: ip_hdr.h ip_hdr.cpp
	g++ -c ip_hdr.cpp

clean:
	rm -rf *.gch *.o
