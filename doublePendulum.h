#ifndef DOUBLE_PENDULUM
#define DOUBLE_PENDULUM

struct PendulumColor {
	public:
		int r, g, b;
};

class DoublePendulum {
	public:
		DoublePendulum();
		DoublePendulum(double theta1, double theta2);
		void step(double deltaT);
		PendulumColor getColor();
	private:
		double _theta1 = 0.0, _deltaTheta1 = 0.0, _length1 = 1.0, _mass1 = 1.0;
		double _theta2 = 0.0, _deltaTheta2 = 0.0, _length2 = 1.0, _mass2 = 1.0;
};

#endif
