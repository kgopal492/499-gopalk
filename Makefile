HOST_SYSTEM = $(shell uname | cut -f 1 -d_)
SYSTEM ?= $(HOST_SYSTEM)
CXX = clang++
CPPFLAGS += `pkg-config --cflags protobuf grpc`
CXXFLAGS += -std=c++11
ifeq ($(SYSTEM),Darwin)
LDFLAGS += -L/usr/local/lib `pkg-config --libs protobuf grpc++`\
           -lgrpc++_reflection\
           -ldl\
					 -lglog\
           -lgflags\
					 -lpthread\
					 -lgtest_main\
					 -lgtest

else
LDFLAGS += -L/usr/local/lib `pkg-config --libs protobuf grpc++`\
           -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed\
           -ldl\
					 -lglog\
           -lgflags\
					 -lpthread\
					 -lgtest_main\
					 -lgtest
endif
PROTOC = protoc
GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`

PROTOS_PATH = ./protos

vpath %.proto $(PROTOS_PATH)

all: key_value_layer service_layer chirp key_value_store_test service_layer_test

key_value_layer: KeyValueStore.pb.o KeyValueStore.grpc.pb.o kvs_server.o kvs_backend.o key_value_layer.o
	$(CXX) $^ $(LDFLAGS) -o $@

service_layer: KeyValueStore.pb.o KeyValueStore.grpc.pb.o ServiceLayer.pb.o ServiceLayer.grpc.pb.o Backend.pb.o Backend.grpc.pb.o sl_server.o sl_functionality.o kvs_client.o kvs_backend.o kvs_client_test.o service_layer.o
	$(CXX) $^ $(LDFLAGS) -o $@

chirp: ServiceLayer.pb.o ServiceLayer.grpc.pb.o sl_client.o chirp.o
	$(CXX) $^ $(LDFLAGS) -o $@

key_value_store_test: key_value_store_test.o kvs_backend.o
	$(CXX) $^ $(LDFLAGS) -o $@

service_layer_test: KeyValueStore.pb.o KeyValueStore.grpc.pb.o ServiceLayer.pb.o ServiceLayer.grpc.pb.o Backend.pb.o Backend.grpc.pb.o service_layer_test.o kvs_client.o kvs_client_test.o kvs_backend.o sl_functionality.o
	$(CXX) $^ $(LDFLAGS) -o $@

%.grpc.pb.cc: %.proto
	$(PROTOC) -I $(PROTOS_PATH) --grpc_out=. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

%.pb.cc: %.proto
	$(PROTOC) -I $(PROTOS_PATH) --cpp_out=. $<

clean:
	rm -f *.o *.pb.cc *.pb.h key_value_layer service_layer chirp key_value_store_test service_layer_test
