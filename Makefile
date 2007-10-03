CFLAGS := -O2 -ggdb

cjwpres: cjwpres.o statusbot.o udpserver.o
	g++ $(CFLAGS) -lgloox -lpthread -ocjwpres cjwpres.o statusbot.o udpserver.o

cjwpres.o: cjwpres.cpp statusbot.h
	g++ $(CFLAGS) -c -ocjwpres.o cjwpres.cpp

statusbot.o: statusbot.cpp statusbot.h data.h defaultrosterlistener.h config.h
	g++ $(CFLAGS) -c -ostatusbot.o statusbot.cpp

udpserver.o: udpserver.cpp udpserver.h statusbot.h data.h
	g++ $(CFLAGS) -c -oudpserver.o udpserver.cpp

clean:
	rm -f cjwpres cjwpres.o statusbot.o unixserver.o

