CXX=g++

SRCS1=main.cpp bwted.cpp
TARGET1=bwted

all:
	${CXX} -std=c++11 -Wall ${SRCS1} -o ${TARGET1}

run:
	./${TARGET1}

clean:
	rm -f ${TARGET1} *.o *.a
