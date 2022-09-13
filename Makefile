CXX=g++

pendulums: pendulums.o fpng.o doublePendulum.o
	$(CXX) pendulums.o fpng.o doublePendulum.o -o pendulums

pendulums.o: pendulums.cpp
	$(CXX) pendulums.cpp -c -std=c++20

doublePendulum.o: doublePendulum.cpp
	$(CXX) doublePendulum.cpp -c -std=c++20

fpng.o: fpng.cpp
	$(CXX) fpng.cpp -c -msse4.1 -mpclmul -fno-strict-aliasing
