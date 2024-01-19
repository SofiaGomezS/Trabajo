#include "Grove_LCD_RGB_Backlight.h"
#include "mbed.h"
#define WAIT_TIME_MS 10

//Definicion de parametros
DigitalOut led1(LED1); //variable led1 en pin de la placa
DigitalIn boton(D2);
AnalogIn pot(A4);
Grove_LCD_RGB_Backlight rgbLCD(PB_9, PB_8);

Timer temporizador; 

//Definicion de variables
float peso;
float peso_0 = 1;
float peso_100 = 2;
float SV0g;
float SV100g;
float peso_filtrado;

int cont1;
int cont2;
char m1[16];

//Enumeración de estados
enum estados {
  Midiendo,
  Calibracion0g,
  Esperando,
  Calibracion100g,
} estado;

void estadoMidiendo() {

  if (boton == 1) {
      //Pasa a estado calibración al pulsar el botón. Variables 0g inicializadas a 0
    estado = Calibracion0g;
    SV0g=0;
    cont1=0; 
  }
  else{
      //Si no se pulsa el botón se mide peso. 
    rgbLCD.locate(0, 0);
    rgbLCD.print("Midiendo Peso");
    led1 = 1;
    float Vpot=pot.read();
    peso = 100 * ( Vpot - peso_0) / (peso_100 - peso_0); //Recta para obtener el peso en función de la salida de la galga
    peso_filtrado=0.9*peso_filtrado+0.1*peso; //Se hace un ajuste para tener menor desviación en la medida
    printf("Pot %f peso %f\n",Vpot,peso);
    rgbLCD.locate(0, 1);
    sprintf(m1,"%f",peso_filtrado);
    rgbLCD.print(m1);

  }
}

void estadoCalibracion0g() {
  if (boton == 0) {
      //Al soltar el boton se cambia de estado. El calibrado a 0g se obtiene con una media
    estado = Esperando;
    peso_0 = SV0g/cont1;
    printf("Peso 0: %f\n",peso_0);
    rgbLCD.clear();
    rgbLCD.locate(0, 0);
    rgbLCD.print("Pon 100 g");

  }
  else{
      //Mientras se presione el botón, se obtiene una cadena de lecturas a 0g
    rgbLCD.clear();
    rgbLCD.locate(0, 0);
    rgbLCD.print("Calibrando");
    rgbLCD.locate(0, 1);
    rgbLCD.print("0 g");
    SV0g+=pot.read();
    cont1=cont1+1;
  }
}

void estadoEsperando() {
    //Se reinician las variables de la calibración de 100g
  if (boton == 1) {
    estado = Calibracion100g;
    SV100g=0;
    cont2=0;

  }
}


void estadoCalibracion100g() {
  if (boton == 1) {
      //Se obtiene una cadena de las medidas obtenidas con 100 g mientras se mantiene el boton 
    rgbLCD.locate(0, 0);
    rgbLCD.print("Calibrando");
    rgbLCD.locate(0, 1);
    rgbLCD.print("100 g");
    float temp=pot.read();
    SV100g+=temp;
    printf("midiedo %f\n",temp);
    cont2=cont2+1;

    
  }
  else{
      //Se calibra a 100g haciendo una media de las lecturas realizadas. Al soltar el boton se cambia de estado.
    estado= Midiendo;
    peso_100 = SV100g/cont2;
    printf("Peso 100: %f\n",peso_100);

  }
}



int main() {
  led1 = 0;
  rgbLCD.setRGB(0xff, 0xff, 0xff);
  estado = Midiendo; //Partimos del estado midiendo.

  while (true) {
    switch (estado) {
    case Esperando:
      estadoEsperando();
      break;
    case Calibracion0g:
      estadoCalibracion0g();
      break;
    case Calibracion100g:
      estadoCalibracion100g();
      break;
    case Midiendo:
        estadoMidiendo();
        break;
    }

    thread_sleep_for(WAIT_TIME_MS);
  }
}