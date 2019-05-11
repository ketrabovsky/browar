CC=g++
CCFLAGS=-c -std=c++11
LDFLAGS=-lwiringPi -pthread

OUTPUT=program

objs=main.o \
	 relay.o \
	 relayManager.o \
	 connection.o \
	 ConfigParser.o \
	 StringUtils.o \
	 server.o \
	 Json.o \
	 fileReader.o \

$(OUTPUT): $(objs)
	$(CC) -o $@ $^ $(LDFLAGS)

%.cpp:%.o
	$(CC) $(CCFLAGS) $^
	
