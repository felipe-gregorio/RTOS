#include "sirene.h"

Sirene::Sirene(int porta){
  _porta=porta;
  pinMode(_porta, OUTPUT);
}

void Sirene::liga(int acao){
  if (estado_sirene==5){
    if (acao==ligar_sirene){
      estado_sirene=1;
    }else if(acao==ligar_bip){
      estado_sirene=3;
    } 
  }
  if (acao==desliga_sirene && (estado_sirene==1||estado_sirene==2) ){
    estado_sirene=4;
  }
}


void Sirene::tick(void){
  switch(estado_sirene){
    case 0:
      estado_sirene=5;
      break;
    case 1:
      vTaskDelay(1000 / portTICK_PERIOD_MS ); // wait for one second
      tone(_porta,440);
      Serial.println("LED ligado");
      estado_sirene=2;
      break;
    case 2:
      vTaskDelay(1000 / portTICK_PERIOD_MS ); // wait for one second
      noTone(_porta);
      Serial.println("LED desligado");
      estado_sirene=1;
      break;
    case 3:
      tone(_porta,440);
      estado_sirene=4;
      break;
    case 4:
      vTaskDelay(100 / portTICK_PERIOD_MS ); // wait for one second
      noTone(_porta);
      estado_sirene=5;
      break;
    case 5:
      break;
  }
}
