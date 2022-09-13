#include "doublePendulum.h"
#include <cmath>

const double GRAVITY = 9.81;

DoublePendulum::DoublePendulum() {
	_theta1 = 0.0;
	_theta2 = 0.0;
}

DoublePendulum::DoublePendulum(double theta1, double theta2) {
	_theta1 = theta1;
	_theta2 = theta2;
}

void DoublePendulum::step(double deltaT) {
	double twoM1M2 = (2.0 * _mass1) + _mass2;
	double deltaTheta1Sq = pow(_deltaTheta1, 2.0);
	double deltaTheta2Sq = pow(_deltaTheta2, 2.0);
	double cosThetaDiff = cos(_theta1 - _theta2);
	double sinThetaDiff = sin(_theta1 - _theta2);
	double bottom = twoM1M2 - (_mass2 * cos(2.0 * (_theta1 - _theta2)));
	double ddTheta1 = (-GRAVITY * twoM1M2 * sin(_theta1) - _mass2 * GRAVITY * sin(_theta1 - 2.0 * _theta2) - 2.0 * sinThetaDiff * _mass2 * ( deltaTheta2Sq * _length2 + deltaTheta1Sq * _length1 * cosThetaDiff )) / (_length1 * bottom);
	double ddTheta2 = (2.0 * sinThetaDiff * (deltaTheta1Sq * _length1 * (_mass1 - _mass2) + GRAVITY * (_mass1 + _mass2) * cos(_theta1) + deltaTheta2Sq * _length2 * _mass2 * cosThetaDiff)) / (_length2 * bottom);
	_deltaTheta1 += deltaT * ddTheta1;
	_deltaTheta2 += deltaT * ddTheta2;
	_theta1 += deltaT * _deltaTheta1;
	_theta2 += deltaT * _deltaTheta2;
}

PendulumColor DoublePendulum::getColor() {
	const double radius = 127.0;

	double sinTheta1 = sin(_theta1);

	return PendulumColor {
		(int) (127.0 + radius * cos(_theta2) * sinTheta1),
		(int) (127.0 + radius * sin(_theta2) * sinTheta1),
		(int) (127.0 + radius * cos(_theta1))
	};
}
