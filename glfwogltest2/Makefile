#Need to clean this up
CXX=g++
CXXFLAGS=-I/usr/include/SOIL/ -lGLEW -lGL -lglfw -lSOIL -Wall

DEPS=Camera.h CubeObject.h GameObject.h Light.h Mesh.h ShaderProgram.h main.h
SOURCEFILES=Camera.cpp CubeObject.cpp GameObject.cpp Light.cpp Mesh.cpp ShaderProgram.cpp main.cpp
OBJECTFILES=Camera.o CubeObject.o GameObject.o Light.o Mesh.o ShaderProgram.o main.o

glfwogltest2: $(OBJECTFILES)
	$(CXX) $(CXXFLAGS) $(OBJECTFILES) -o glfwogltest2

Camera.o: Camera.cpp
	$(CXX) $(CXXFLAGS) -c Camera.cpp -o Camera.o

CubeObject.o: CubeObject.cpp
	$(CXX) $(CXXFLAGS) -c CubeObject.cpp -o CubeObject.o

GameObject.o: GameObject.cpp
	$(CXX) $(CXXFLAGS) -c GameObject.cpp -o GameObject.o

Light.o: Light.cpp
	$(CXX) $(CXXFLAGS) -c Light.cpp -o Light.o

Mesh.o: Mesh.cpp
	$(CXX) $(CXXFLAGS) -c Mesh.cpp -o Mesh.o

ShaderProgram.o: ShaderProgram.cpp
	$(CXX) $(CXXFLAGS) -c ShaderProgram.cpp -o ShaderProgram.o

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

clean:
	rm *.o glfwogltest2
