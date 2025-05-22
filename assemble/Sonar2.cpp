#include <wiringPi.h>
#include "Sonar2.h"

using namespace std;

Sonar2::Sonar2(int successPin, int failPin) 
    : pinSuccess(successPin), pinFail(failPin) 
{
    wiringPiSetupGpio(); // استفاده از شماره‌گذاری BCM
    pinMode(pinSuccess, INPUT);
    pinMode(pinFail, INPUT);
}

bool Sonar2::getSuccess() const {
    return digitalRead(pinSuccess) == HIGH;
}

bool Sonar2::getFail() const {
    return digitalRead(pinFail) == HIGH;
}
