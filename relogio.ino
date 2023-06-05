#include "relogio.h"

void TaskRelogio(void *pvParameters) {  // This is a task.
(void)pvParameters;
int horario[2]={0, 0};
int _temp_base_tempo=0;
int acao[2];
for (;;) { 
  vTaskDelay(10/ portTICK_PERIOD_MS);
   _temp_base_tempo++;
   Serial.println(_temp_base_tempo);
   if(xQueueReceive(ajusterelogioQueue, &acao, 0)== pdPASS){
    horario[1]=acao[1];
    horario[0]=acao[0];
   }
  if(_temp_base_tempo==10){
    _temp_base_tempo=0;
    horario[0]++;
    if (horario[0]>=60){
       horario[0]=0;
      horario[1]++;
      if(horario[1]>=24){
        horario[1]=0;  
      }
    }
  }
   xQueueSend(relogioQueue, &horario, 0);
}
}
