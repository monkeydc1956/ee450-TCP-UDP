# Compiler and compile options
CXX = g++
CXXFLAGS = -std=c++11 -Wall

# Targets and object files
TARGETS = serverM serverS serverD serverU client
OBJS = main.o serverM.o serverS.o serverD.o serverU.o client.o

# Build rules
all: $(TARGETS)
serverM: serverM.o
	$(CXX) serverM.o -o serverM
serverS: serverS.o
	$(CXX) serverS.o -o serverS
serverD: serverD.o
	$(CXX) serverD.o -o serverD
serverU: serverU.o
	$(CXX) serverU.o -o serverU
client: client.o
	$(CXX) client.o -o client

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGETS)