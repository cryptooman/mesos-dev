all:
	g++ -std=c++11 -Wall -g \
-I$(MESOS_SRC)/include \
-I$(MESOS_SRC)/build/include \
-I$(MESOS_SRC)/3rdparty/stout/include \
-I$(MESOS_SRC)/build/3rdparty/picojson-1.3.0 \
-L$(MESOS_LIBS) \
test-service-1.cpp -o test-service-1.bin \
-lmesos -lprotobuf -lpthread -pthread

