pendulum: pendulum.o fpng.o
	g++ pendulum.o fpng.o -o pendulum

pendulum.o: pendulum.cpp
	g++ pendulum.cpp -c

fpng.o: fpng.cpp
	g++ fpng.cpp -c -msse4.1 -mpclmul -fno-strict-aliasing
