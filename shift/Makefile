# Compiler and compile options
CXX = g++
CXXFLAGS = -std=c++11 -Wall

# Targets and object files
TARGETS = generateNewUser
OBJS = generateNewUser.o

# Build rules
all: $(TARGETS)
generateNewUser: generateNewUser.o
	$(CXX) generateNewUser.o -o generateNewUser
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGETS)