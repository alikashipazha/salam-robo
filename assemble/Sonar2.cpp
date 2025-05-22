#include <wiringPi.h>
#include <iostream>
#include "Sonar2.h"

using namespace std;

Sonar2::Sonar2(int successPin, int failPin) 
    : pinSuccess(successPin), pinFail(failPin) 
{
    wiringPiSetupGpio(); // استفاده از شماره‌گذاری BCM
    pinMode(pinSuccess, INPUT);
    pinMode(pinFail, INPUT);
    cout << "Sonar2 is constructed" << endl;
}

bool Sonar2::getSuccess() const {
	//cout << "Sonar2: getSuccess = " << (digitalRead(pinSuccess) == HIGH) << endl;
    return digitalRead(pinSuccess) == HIGH;
}

bool Sonar2::getFail() const {
	//cout << "Sonar2: getFail = " << (digitalRead(pinFail) == HIGH) << endl;
    return digitalRead(pinFail) == HIGH;
}
