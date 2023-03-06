# makefile demo  #

TARGET:= main

FILES:= main.o timerManage.o

LIBS:= #-L /home/spring/test/boost_1_68_0_install/lib -lboost_system  -lboost_chrono

CFLAGS:= -O0 -Wall -Wno-long-long

DEBUG:= -g

INCLUDE:=#-I /home/spring/test/boost_1_68_0_install/include

CC:= gcc

all:$(TARGET)

$(TARGET) : $(FILES)
	$(CC) -o $@ $(FILES) $(LIBS)


%.o : %.cpp
	$(CC) $(CFLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@


clean:
	-rm $(TARGET)
	-rm $(FILES)

