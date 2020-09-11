
CC = g++
CFLAGS = -std=c++11 -g -O0
FINAL_LIBS = -lzmq
FINAL_FLAGS = $(CFLAGS) $(FINAL_LIBS)


src = $(wildcard *.c*)
obj = $(patsubst %.cpp, %.o, $(src))


#target = server
all: hwserver hwclient wuclient wuserver taskwork tasksink taskvent rrclient rrbroker rrworker

hwserver: hwserver.o
	$(CC) -o $@ $^ $(FINAL_FLAGS)

hwclient: hwclient.o
	$(CC) -o $@ $^ $(FINAL_FLAGS)

wuclient: wuclient.o
	$(CC) -o $@ $^ $(FINAL_FLAGS)

wuserver: wuserver.o
	$(CC) -o $@ $^ $(FINAL_FLAGS)

taskwork: taskwork.o
	$(CC) -o $@ $^ $(FINAL_FLAGS)

tasksink: tasksink.o
	$(CC) -o $@ $^ $(FINAL_FLAGS)

taskvent: taskvent.o
	$(CC) -o $@ $^ $(FINAL_FLAGS)

rrclient: rrclient.o
	$(CC) -o $@ $^ $(FINAL_FLAGS)

rrbroker: rrbroker.o
	$(CC) -o $@ $^ $(FINAL_FLAGS)

rrworker: rrworker.o
	$(CC) -o $@ $^ $(FINAL_FLAGS)

hwclient.o: hwclient.cpp
	$(CC) -o $@ -c $^ $(FINAL_FLAGS)

hwserver.o: hwserver.cpp
	$(CC) -o $@ -c $^ $(FINAL_FLAGS)

wuclient.o: wuclient.cpp
	$(CC) -o $@ -c $^ $(FINAL_FLAGS)

wuserver.o: wuserver.cpp
	$(CC) -o $@ -c $^ $(FINAL_FLAGS)

taskwork.o: taskwork.cpp
	$(CC) -o $@ -c $^ $(FINAL_FLAGS)

tasksink.o: tasksink.cpp
	$(CC) -o $@ -c $^ $(FINAL_FLAGS)
	
taskvent.o: taskvent.cpp
	$(CC) -o $@ -c $^ $(FINAL_FLAGS)

rrclient.o: rrclient.cpp
	$(CC) -o $@ -c $^ $(FINAL_FLAGS)

rrbroker.o: rrbroker.cpp
	$(CC) -o $@ -c $^ $(FINAL_FLAGS)

rrworker.o: rrworker.cpp
	$(CC) -o $@ -c $^ $(FINAL_FLAGS)

clean:
	-rm -rf *.o $(shell find . -type f -perm /1)
