s ?= client.cpp
e ?= 1
CXXFLAGS := -std=c++20 -pthread -Wall -g  -o$(e) #-fsanitize=thread
main: 	$(s) ../include/picohttpparser/picotest/picotest.c ../include/picohttpparser/picohttpparser.c
		g++ $(s) $(CXXFLAGS) 

#  g++ test.cpp ../include/picohttpparser/picotest/picotest.c ../include/picohttpparser/picohttpparser.c  -std=c++20 -pthread -Wall -g -o test