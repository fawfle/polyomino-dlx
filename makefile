# the c++ compiler
CXX = g++

# -Wall                        - gives warnings (enable all warnings)
# -g                           - generate debug info
# -I          -Iglad/include   - adds the glad/include directory to be searched for header files
# -l          -lglfw           - search for the glfw library when linking
# -l          -ldl             - -link the dynamic loader
CXXFLAGS = -Wall -g -Iglad/include -lglfw -ldl -pedantic

TARGETS = ./src/puzzle_solver.cc ./src/dlx.cc ./src/rooks.cc ./src/polyomino.cc

BUILD_TARGET = ./build/puzzle_solver

all: $(TARGET)
	$(CXX) $(CXXFLAGS) glad/src/gl.c $(TARGETS) -o $(BUILD_TARGET)

clean:
	$(RM) $(TARGET)
