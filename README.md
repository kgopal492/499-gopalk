# Chirp Phase 1
## Environment Setup
1. Create & start Vagrant box
```
vagrant init generic/ubuntu1804
vagrant up
vagrant ssh
```
2. Install dependencies/packages
```
sudo apt-get -y update
sudo apt-get -y install build-essential autoconf libtool pkg-config
sudo apt-get -y install clang libc++-dev
sudo apt-get -y install make cmake
```
3. Install gflags
```
sudo apt-get -y install libgflags-dev
```

4. Install glog
```
git clone https://github.com/google/glog.git
cd glog
mkdir build && cd build
cmake -DGFLAGS_NAMESPACE=google -DCMAKE_CXX_FLAGS=-fPIC ..
make && sudo make install
cd ~
```

5. Install gtest
```
sudo apt-get -y install libgtest-dev
cd /usr/src/gtest
sudo cmake CMakeLists.txt
sudo make
sudo cp *.a /usr/lib
cd ~
```

6. Install GRPC
```
git clone -b $(curl -L https://grpc.io/release) https://github.com/grpc/grpc
cd grpc
git submodule update --init
make && sudo make install
cd third_party/protobuf
make && sudo make install
cd ~
```

7. Clone Chirp project from GitHub
```
git clone https://github.com/kgopal492/499-gopalk.git
cd 499-gopalk
git checkout phase1
```

## Compile Program
Compile with `make` command in root directory: 499-gopalk/

## Run Chirp
1. Run executable keyvaluestore in one terminal shell: `./keyvaluestore`
2. Run executable servicelayer in *another* terminal shell: `./servicelayer`
3. Run any chirp commands in other terminal(s), e.g.:

```
./chirp --register "barath"
./chirp --register "krishna"
./chirp --follow "barath" --user "krishna"
./chirp --chirp "Hello, class!" --user "barath"
./chirp --read "0"
./chirp --monitor=true --user "krishna"
```
4. If monitor flag is run in one shell, spawn another shell to continue to execute chirp commands.
