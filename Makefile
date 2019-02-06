HOST_SYSTEM = $(shell uname | cut -f 1 -d_)
SYSTEM ?= $(HOST_SYSTEM)
CXX = g++
CPPFLAGS += `pkg-config --cflags protobuf grpc`
CXXFLAGS += -std=c++11
ifeq ($(SYSTEM),Darwin)
LDFLAGS += -L/usr/local/lib `pkg-config --libs protobuf grpc++`\
           -lgrpc++_reflection\
           -ldl
else
LDFLAGS += -L/usr/local/lib `pkg-config --libs protobuf grpc++`\
           -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed\
           -ldl\
           -lgflags\
           -lgtest\
	   -lglog
endif
PROTOC = protoc
GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`

PROTOS_PATH = ./protos

vpath %.proto $(PROTOS_PATH)

all: servicelayer chirp keyvaluestore

keyvaluestore: KeyValueStore.pb.o KeyValueStore.grpc.pb.o kvs_server.o keyvaluestore.o
		$(CXX) $^ $(LDFLAGS) -o $@

servicelayer: KeyValueStore.pb.o KeyValueStore.grpc.pb.o ServiceLayer.pb.o ServiceLayer.grpc.pb.o sl_server.o kvs_client.o servicelayer.o
	$(CXX) $^ $(LDFLAGS) -o $@

chirp: ServiceLayer.pb.o ServiceLayer.grpc.pb.o sl_client.o chirp.o
	$(CXX) $^ $(LDFLAGS) -o $@

%.grpc.pb.cc: %.proto
	$(PROTOC) -I $(PROTOS_PATH) --grpc_out=. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

%.pb.cc: %.proto
	$(PROTOC) -I $(PROTOS_PATH) --cpp_out=. $<

clean:
	rm -f *.o *.pb.cc *.pb.h service chirp
