#include "Arduino.h"
#include <Arduino_FreeRTOS.h>

//#include "timer.h"

#define ligar_sirene 22
#define ligar_bip 33
#define desliga_sirene 44

class Sirene{
  int estado_sirene;
  byte tempo;
  int _porta;
//  timer timer_sirene;
  public:
    Sirene(int);
    void liga(int);
    void tick(void);
};
