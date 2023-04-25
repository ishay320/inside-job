
BUILD_PATH=build
INCLUDE=include/hashmap_tree.h include/broker.h
CFLAGS=-Wall -Wextra

${BUILD_PATH}/main: src/main.cpp ${BUILD_PATH}/broker.o ${INCLUDE} ${BUILD_PATH}
	g++ ${CFLAGS} $< -o $@ -I./include ${BUILD_PATH}/broker.o -g

${BUILD_PATH}/broker.o: src/broker.cpp ${INCLUDE} ${BUILD_PATH}
	g++ ${CFLAGS} $< -o $@ -I./include -c -g

${BUILD_PATH}:
	mkdir ${BUILD_PATH}