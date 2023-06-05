#include "Arduino.h"
#include <Arduino_FreeRTOS.h>

#define pressionado 11
#define naopressionado 22


class Botoes{
  int _porta;
  int count;
  int estado_botao;
  public:
    Botoes(int);
    void tick(void);
    int status(void);
};
