#Need to clean this up / use CMake
CXX=g++
CXXFLAGS=-I/usr/include/SOIL/ -lGLEW -lGL -lglfw -lSOIL -Wall -g

DEPS=Camera.h CubeObject.h GameObject.h Light.h Mesh.h ShaderProgram.h TextureManager.h PointLight.h DirLight.h main.h
SOURCEFILES=Camera.cpp CubeObject.cpp GameObject.cpp Light.cpp Mesh.cpp ShaderProgram.cpp TextureManager.cpp PointLight.cpp DirLight.cpp main.cpp
OBJECTFILES=Camera.o CubeObject.o GameObject.o Light.o Mesh.o ShaderProgram.o TextureManager.o PointLight.o DirLight.o main.o

glfwogltest2: $(OBJECTFILES)
	$(CXX) $(CXXFLAGS) $(OBJECTFILES) -o glfwogltest2

Camera.o: Camera.cpp Camera.h
	$(CXX) $(CXXFLAGS) -c Camera.cpp -o Camera.o

CubeObject.o: CubeObject.cpp CubeObject.h
	$(CXX) $(CXXFLAGS) -c CubeObject.cpp -o CubeObject.o

GameObject.o: GameObject.cpp GameObject.h
	$(CXX) $(CXXFLAGS) -c GameObject.cpp -o GameObject.o

Light.o: Light.cpp Light.h
	$(CXX) $(CXXFLAGS) -c Light.cpp -o Light.o

Mesh.o: Mesh.cpp Mesh.h
	$(CXX) $(CXXFLAGS) -c Mesh.cpp -o Mesh.o

ShaderProgram.o: ShaderProgram.cpp ShaderProgram.h
	$(CXX) $(CXXFLAGS) -c ShaderProgram.cpp -o ShaderProgram.o

TextureManager.o: TextureManager.cpp TextureManager.h
	$(CXX) $(CXXFLAGS) -c TextureManager.cpp -o TextureManager.o
	
DirLight.o: DirLight.cpp DirLight.h Light.h
	$(CXX) $(CXXFLAGS) -c DirLight.cpp -o DirLight.o

PointLight.o: PointLight.cpp PointLight.h Light.h
	$(CXX) $(CXXFLAGS) -c PointLight.cpp -o PointLight.o
	
main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

clean:
	rm *.o glfwogltest2
