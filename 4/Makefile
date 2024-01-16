s ?= client.cpp
e ?= 1
CXX ?= g++
CXXFLAGS := -pthread -Wall -g  -o$(e) -std=c++20 #-fsanitize=thread  
SFMLFLAGS := -lsfml-graphics -lsfml-window -lsfml-system
main: 	$(s)
		$(CXX) $(s) $(CXXFLAGS) $(SFMLFLAGS)