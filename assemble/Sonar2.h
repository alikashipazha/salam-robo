#ifndef SONAR2_H
#define SONAR2_H

using namespace std;

class Sonar2 {
private:
    int pinSuccess;
    int pinFail;

public:
    Sonar2(int successPin, int failPin);
    bool getSuccess() const;
    bool getFail() const;
};

#endif // SONAR2_H
