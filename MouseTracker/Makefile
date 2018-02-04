########### USERS NEED TO CONFIGURE THIS VARIABLE THEMSELVES #####################
OPENCV_ROOT_DIR=/home/alan/opencv_install
#####################################################################################

CPP=g++
STD=c++11
DEBUG_DIR=./Debug
RELEASE_DIR=./Release
SRC_DIR=./src
LOCAL_INCLUDE_DIR=./header
OPENCV_INCLUDE_DIR=$(OPENCV_ROOT_DIR)/include
OPENCV_LIB_DIR=$(OPENCV_ROOT_DIR)/lib/x86_64-linux-gnu
OUTPUT_BIN_NAME=MouseTracker
OUTPUT_DIR=$(DEBUG_DIR)
OUTPUT_BIN=$(OUTPUT_DIR)/$(OUTPUT_BIN_NAME)

OTHER_OPTS=-O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP


all: clean build
	
build:	compile link

compile: utils object object_finder car udp_client main

clean:
	rm -rf  $(DEBUG_DIR)/* $(RELEASE_DIR)/*
	
utils:
	mkdir -p $(OUTPUT_DIR)/src
	$(CPP) -std=$(STD) -I$(LOCAL_INCLUDE_DIR) -I$(OPENCV_INCLUDE_DIR) $(OTHER_OPTS) -MF"$(OUTPUT_DIR)/src/utils.d" -MT"$(OUTPUT_DIR)/src/utils.d" -o "$(OUTPUT_DIR)/src/utils.o" "$(SRC_DIR)/utils.cpp"

object:
	mkdir -p $(OUTPUT_DIR)/src
	$(CPP) -std=$(STD) -I$(LOCAL_INCLUDE_DIR) -I$(OPENCV_INCLUDE_DIR) $(OTHER_OPTS) -MF"$(OUTPUT_DIR)/src/Object.d" -MT"$(OUTPUT_DIR)/src/Object.d" -o "$(OUTPUT_DIR)/src/Object.o" "$(SRC_DIR)/Object.cpp"
	
object_finder:
	mkdir -p $(OUTPUT_DIR)/src
	$(CPP) -std=$(STD) -I$(LOCAL_INCLUDE_DIR) -I$(OPENCV_INCLUDE_DIR) $(OTHER_OPTS) -MF"$(OUTPUT_DIR)/src/ObjectFinder.d" -MT"$(OUTPUT_DIR)/src/ObjectFinder.d" -o "$(OUTPUT_DIR)/src/ObjectFinder.o" "$(SRC_DIR)/ObjectFinder.cpp"

 
car:
	mkdir -p $(OUTPUT_DIR)/src
	$(CPP) -std=$(STD) -I$(LOCAL_INCLUDE_DIR) -I$(OPENCV_INCLUDE_DIR) $(OTHER_OPTS) -MF"$(OUTPUT_DIR)/src/car.d" -MT"$(OUTPUT_DIR)/src/car.d" -o "$(OUTPUT_DIR)/src/car.o" "$(SRC_DIR)/car.cpp"

udp_client:
	mkdir -p $(OUTPUT_DIR)/src
	$(CPP) -std=$(STD) -I$(LOCAL_INCLUDE_DIR) -I$(OPENCV_INCLUDE_DIR) $(OTHER_OPTS) -MF"$(OUTPUT_DIR)/src/udp_client.d" -MT"$(OUTPUT_DIR)/src/udp_client.d" -o "$(OUTPUT_DIR)/src/udp_client.o" "$(SRC_DIR)/udp_client.cpp"

main:
	mkdir -p $(OUTPUT_DIR)/src
	$(CPP) -std=$(STD) -I$(LOCAL_INCLUDE_DIR) -I$(OPENCV_INCLUDE_DIR) $(OTHER_OPTS) -MF"$(OUTPUT_DIR)/src/main.d" -MT"$(OUTPUT_DIR)/src/main.d" -o "$(OUTPUT_DIR)/src/main.o" "$(SRC_DIR)/main.cpp"

link:
	$(CPP) -L$(OPENCV_LIB_DIR) -o $(OUTPUT_BIN)  $(OUTPUT_DIR)/src/Object.o $(OUTPUT_DIR)/src/ObjectFinder.o $(OUTPUT_DIR)/src/car.o $(OUTPUT_DIR)/src/udp_client.o $(OUTPUT_DIR)/src/utils.o $(OUTPUT_DIR)/src/main.o   -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lopencv_video -lopencv_highgui -lopencv_videoio -lopencv_tracking

run:
	LD_LIBRARY_PATH=$(OPENCV_LIB_DIR) $(OUTPUT_BIN)
