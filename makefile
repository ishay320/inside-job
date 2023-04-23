
BUILD_PATH=build


${BUILD_PATH}/main: src/main.cpp include/hashmap_tree.h include/broker.h ${BUILD_PATH}
	g++ $< -o $@ -I./include -g

${BUILD_PATH}:
	mkdir ${BUILD_PATH}