#include "botoes.h"

Botoes::Botoes(int porta) {
  _porta = porta;
  estado_botao = 0;
  pinMode(_porta, INPUT_PULLUP);
}

int Botoes::status(void) {
  if (estado_botao == 2) {
    estado_botao = 0;
    return pressionado;
  } else {
    return naopressionado;
  }
}

void Botoes::tick(void) {
  switch (estado_botao) {
    case 0:
      // vTaskDelay(1/portTICK_PERIOD_MS); // wait for one second
      if (!digitalRead(_porta)) {
        count++;
        Serial.print(count);
        if (count > 50) {
          estado_botao = 1;
        }
      } else {
        count = 0;
      }
      break;
    case 1:
      // vTaskDelay(1 / portTICK_PERIOD_MS ); // wait for one second
      if (digitalRead(_porta)) {
        Serial.print(count);
        count++;
        if (count > 50) {
          estado_botao = 2;
        }
      } else {
        count = 0;
      }
      break;
    case 2:
      break;
  }
}
