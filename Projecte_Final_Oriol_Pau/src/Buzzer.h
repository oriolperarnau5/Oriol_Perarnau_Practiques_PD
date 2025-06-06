#ifndef BUZZER_H
#define BUZZER_H

class Buzzer {
private:
    int pin;
public:
    Buzzer(int pin);
    void iniciar();
    void sonar();
    void parar();
};

#endif
