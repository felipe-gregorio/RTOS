#include "funcBotoes.h"

void TaskBotoes(void *pvParameters) {  // This is a task.
  (void)pvParameters;
  int estado_botao=0;
  int count=0;
  int _porta=5;
  int acao;
  pinMode(_porta, INPUT_PULLUP);

  for (;;)  // A Task shall never return or exit.
  {
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
        acao=pressionado;
        xQueueSend(botoesQueue, &acao, 0);
        estado_botao=0;
        break;
    }
  }
}
