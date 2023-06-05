#include <Arduino_FreeRTOS.h>
#include "funcSirene.h"
#include "funcBotoes.h"
#include "funcJoystick.h"
#include <queue.h>
#include <LiquidCrystal.h>
#include "relogio.h"


// define two tasks for Blink & AnalogRead
void TaskSirene(void *pvParameters);
void TaskBotoes(void *pvParameters);
void TaskInterface(void *pvParameters);

TaskHandle_t TaskSireneHandle;
TaskHandle_t TaskBotoesHandle;
TaskHandle_t TaskInterfaceHandle;
TaskHandle_t TaskSerialHandle;
QueueHandle_t buzzerQueue;
QueueHandle_t botoesQueue;
QueueHandle_t joystickQueue;
QueueHandle_t relogioQueue;
QueueHandle_t ajusterelogioQueue;


//Sirene Buzzer(13);
//Botoes Button(5);
// the setup function runs once when you press reset or power the board

const int rs = 22, en = 24, d4 = 26, d5 = 28, d6 = 30, d7 = 32;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  buzzerQueue = xQueueCreate(1,           // Queue length
                             sizeof(int)  // Queue item size
  );

  botoesQueue = xQueueCreate(1,           // Queue length
                             sizeof(int)  // Queue item size
  );

  joystickQueue = xQueueCreate(1,           // Queue length
                             sizeof(int)  // Queue item size
  );
  relogioQueue = xQueueCreate(10,           // Queue length
                             2*sizeof(int)  // Queue item size
  );
  ajusterelogioQueue = xQueueCreate(10,           // Queue length
                             2*sizeof(int)  // Queue item size
  );

  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
  lcd.begin(16,2);
  // while (!Serial) {
  //   ;  // wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and other 32u4 based boards.
  // }
  // Now set up two tasks to run independently.

  xTaskCreate(
    TaskSirene, "Sirene"  // A name just for humans
    ,
    128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,
    NULL, 1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,
    &TaskSireneHandle);

  xTaskCreate(
    TaskBotoes, "Botoes", 128  // Stack size
    ,
    NULL, 1  // Priority
    ,
    &TaskBotoesHandle);

  xTaskCreate(
    TaskInterface, "Interface", 512  // Stack size
    ,
    NULL, 1  // Priority
    ,
    &TaskInterfaceHandle);

  xTaskCreate(
    TaskJoystick, "Joystick", 128  // Stack size
    ,
    NULL, 1  // Priority
    ,
    NULL);
  xTaskCreate(
    TaskRelogio, "Relogio", 128  // Stack size
    ,
    NULL, 1  // Priority
    ,
    NULL);  

  //  xTaskCreate(
  //    TaskSerial
  //    , "Serial"
  //    , 128  // Stack size
  //    , NULL
  //    , 1  // Priority
  //    , &TaskSerialHandle);

  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}

void loop() {
  // Empty. Things are done in Tasks.
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/
void TaskSerial(void *pvParameters) {
  (void)pvParameters;

  Serial.begin(115200);

  for (;;) {
    Serial.println("======== Tasks status ========");
    Serial.print("Tick count: ");
    Serial.print(xTaskGetTickCount());
    Serial.print(", Task count: ");
    Serial.print(uxTaskGetNumberOfTasks());

    Serial.println();
    Serial.println();

    // Serial task status
    Serial.print("- TASK ");
    Serial.print(pcTaskGetName(NULL));  // Get task name without handler https://www.freertos.org/a00021.html#pcTaskGetName
    Serial.print(", High Watermark: ");
    Serial.print(uxTaskGetStackHighWaterMark(NULL));  // https://www.freertos.org/uxTaskGetStackHighWaterMark.html


    TaskHandle_t taskSerialHandle = xTaskGetCurrentTaskHandle();  // Get current task handle. https://www.freertos.org/a00021.html#xTaskGetCurrentTaskHandle

    Serial.println();

    Serial.print("- TASK ");
    Serial.print(pcTaskGetName(TaskSireneHandle));  // Get task name with handler
    Serial.print(", High Watermark: ");
    Serial.print(uxTaskGetStackHighWaterMark(TaskSireneHandle));
    Serial.println();

    Serial.print("- TASK ");
    Serial.print(pcTaskGetName(TaskBotoesHandle));
    Serial.print(", High Watermark: ");
    Serial.print(uxTaskGetStackHighWaterMark(TaskBotoesHandle));
    Serial.println();

    Serial.print("- TASK ");
    Serial.print(pcTaskGetName(TaskInterfaceHandle));
    Serial.print(", High Watermark: ");
    Serial.print(uxTaskGetStackHighWaterMark(TaskInterfaceHandle));
    Serial.println();

    Serial.println();

    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
}
void TaskInterface(void *pvParameters) {
  (void)pvParameters;
  int acao;
  int horario[2];
  int estado_loop=0;
  int loop_hora=0;
  int loop_min=0;
  int despertador[2]={0,0};
  int liga_despertador=0;
  for (;;)  // A Task shall never return or exit.
  {
    // set the cursor to column 0, line 1
    // (note: line 1 is the second row, since counting begins with 0):
    lcd.setCursor(0, 1);
   if (xQueueReceive(relogioQueue, &horario, 0) == pdPASS) {
    // print the number of seconds since reset:
    if (horario[1] < 10) {
      lcd.print("0");
    }
    lcd.print(horario[1]);
    lcd.print(":");
    if (horario[0] < 10) {
      lcd.print("0");
    }
    lcd.print(horario[0]);
   }
    switch (estado_loop) {
      case 0:
        if (xQueueReceive(botoesQueue, &acao, 0)== pdPASS) {
          if(acao==pressionado){
              acao=ligar_bip;
              xQueueSend(buzzerQueue, &acao, 0);
              estado_loop=20;
          }
        }
        if(horario[0]==despertador[0] && horario[1]==despertador[1] && liga_despertador){
          estado_loop=2;
          acao=ligar_sirene;
          xQueueSend(buzzerQueue, &acao, 0);
        }
        break;
        case 2:
          if (xQueueReceive(botoesQueue, &acao, 0)== pdPASS) {
          if(acao==pressionado){
              acao=desliga_sirene;
              xQueueSend(buzzerQueue, &acao, 0);
              estado_loop=0;
          }
        }
        break;
        case 20:
        lcd.setCursor(0, 0);
        lcd.print("Ajuste de hora");
        if (xQueueReceive(joystickQueue, &acao, 0) == pdPASS){
           if (acao == Apressionadoalto || acao==Apressionadobaixo) {
              acao = ligar_bip;
             xQueueSend(buzzerQueue, &acao, 0);
             estado_loop = 21;
            }
        }
        if (xQueueReceive(botoesQueue, &acao, 0)== pdPASS) {
          if(acao==pressionado){
              acao=ligar_bip;
              xQueueSend(buzzerQueue, &acao, 0);
              estado_loop=31;
              loop_hora=horario[1];
              loop_min=horario[0];
          }
        }
          break;
      case 21:
        lcd.setCursor(0, 0);
        lcd.print("Despertador   ");
        if (xQueueReceive(joystickQueue, &acao, 0) == pdPASS){
           if (acao == Apressionadoalto || acao==Apressionadobaixo) {
              acao = ligar_bip;
             xQueueSend(buzzerQueue, &acao, 0);
             estado_loop = 20;
            }
        }
        if (xQueueReceive(botoesQueue, &acao, 0)== pdPASS) {
          if(acao==pressionado){
              acao=ligar_bip;
              xQueueSend(buzzerQueue, &acao, 0);
              estado_loop=41;
          }
        }
        break;
       case 31:
        lcd.setCursor(0, 0);
        lcd.print("Ajuste de horario");
        lcd.setCursor(8, 1);
        if (loop_hora < 10) {
          lcd.print("0");
        }
        lcd.print(loop_hora);
        lcd.print(":");
        if (loop_min < 10) {
          lcd.print("0");
        }
        lcd.print(loop_min);
        if (xQueueReceive(joystickQueue, &acao, 0) == pdPASS){
           if (acao == Bpressionadoalto) {
            acao=ligar_bip;
            xQueueSend(buzzerQueue, &acao, 0);
              if (loop_hora < 23) {
                loop_hora++;
              }
           }else if(acao==Bpressionadobaixo){
              acao=ligar_bip;
              xQueueSend(buzzerQueue, &acao, 0);
              if (loop_hora > 0) {
                loop_hora--;
              }
           }else if (acao == Apressionadoalto || acao==Apressionadobaixo) {
              acao = ligar_bip;
              xQueueSend(buzzerQueue, &acao, 0);
              estado_loop = 32;
            }
            
        }
        if (xQueueReceive(botoesQueue, &acao, 0)== pdPASS) {
              if(acao==pressionado){
                  acao=ligar_bip;
                  xQueueSend(buzzerQueue, &acao, 0);
                  estado_loop=33;
              }
            }
        break;
      case 32:
        lcd.setCursor(0, 0);
        lcd.print("Ajuste de horario");
        lcd.setCursor(8, 1);
        if (loop_hora < 10) {
          lcd.print("0");
        }
        lcd.print(loop_hora);
        lcd.print(":");
        if (loop_min < 10) {
          lcd.print("0");
        }
        lcd.print(loop_min);
        if (xQueueReceive(joystickQueue, &acao, 0) == pdPASS){
           if (acao == Bpressionadoalto) {
            acao=ligar_bip;
            xQueueSend(buzzerQueue, &acao, 0);
              if (loop_min < 59) {
                loop_min++;
              }
           }else if(acao==Bpressionadobaixo){
              acao=ligar_bip;
              xQueueSend(buzzerQueue, &acao, 0);
              if (loop_min > 0) {
                loop_min--;
              }
           }else if (acao == Apressionadoalto || acao==Apressionadobaixo) {
              acao = ligar_bip;
              xQueueSend(buzzerQueue, &acao, 0);
              estado_loop = 31;
            }
        }
         if (xQueueReceive(botoesQueue, &acao, 0)== pdPASS) {
          if(acao==pressionado){
              acao=ligar_bip;
              xQueueSend(buzzerQueue, &acao, 0);
              estado_loop=33;
          }
        }
        break;
        case 33:
          horario[1]=loop_hora;
          horario[0]=loop_min;
          xQueueSend(ajusterelogioQueue, &horario, 0);
          lcd.clear();
          estado_loop=0;
          lcd.clear();
        break;
        ////////////////////////////////////////////////////////////////////////////////////
        case 41:
        lcd.setCursor(0, 0);
        lcd.print("Despertador");
        lcd.setCursor(8, 1);
        if (despertador[1] < 10) {
          lcd.print("0");
        }
        lcd.print(despertador[1]);
        lcd.print(":");
        if (despertador[0] < 10) {
          lcd.print("0");
        }
        lcd.print(despertador[0]);
        if (xQueueReceive(joystickQueue, &acao, 0) == pdPASS){
           if (acao == Bpressionadoalto) {
            acao=ligar_bip;
            xQueueSend(buzzerQueue, &acao, 0);
              if (despertador[1] < 23) {
                despertador[1]++;
              }
           }else if(acao==Bpressionadobaixo){
              acao=ligar_bip;
              xQueueSend(buzzerQueue, &acao, 0);
              if (despertador[1] > 0) {
                despertador[1]--;
              }
           }else if (acao == Apressionadoalto || acao==Apressionadobaixo) {
              acao = ligar_bip;
              xQueueSend(buzzerQueue, &acao, 0);
              estado_loop = 42;
            }
            
        }
        if (xQueueReceive(botoesQueue, &acao, 0)== pdPASS) {
          if(acao==pressionado){
              acao=ligar_bip;
              xQueueSend(buzzerQueue, &acao, 0);
              estado_loop=0;
              lcd.clear();
              liga_despertador=1;
          }
        }
        break;
      case 42:
        lcd.setCursor(0, 0);
        lcd.print("Despertador");
        lcd.setCursor(8, 1);
        if (despertador[1] < 10) {
          lcd.print("0");
        }
        lcd.print(despertador[1]);
        lcd.print(":");
        if (despertador[0] < 10) {
          lcd.print("0");
        }
        lcd.print(despertador[0]);
        if (xQueueReceive(joystickQueue, &acao, 0) == pdPASS){
           if (acao == Bpressionadoalto) {
            acao=ligar_bip;
            xQueueSend(buzzerQueue, &acao, 0);
            if (despertador[0] < 59) {
             despertador[0]++;
            }
           }else if(acao==Bpressionadobaixo){
              acao=ligar_bip;
              xQueueSend(buzzerQueue, &acao, 0);
              if (despertador[0] > 0) {
                despertador[0]--;
              }
           }else if (acao == Apressionadoalto || acao==Apressionadobaixo) {
              acao = ligar_bip;
              xQueueSend(buzzerQueue, &acao, 0);
              estado_loop = 41;
            }
        }
         if (xQueueReceive(botoesQueue, &acao, 0)== pdPASS) {
          if(acao==pressionado){
              acao=ligar_bip;
              xQueueSend(buzzerQueue, &acao, 0);
              estado_loop=0;
              lcd.clear();
              liga_despertador=1;

          }
        }
        break;
        ////////////////////////////////////////////////////////////////////////////////////
        
    }//end switch
    }//end for(;;)
  }//end taskinterface
