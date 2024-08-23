// INCLUSÃO DE BIBLIOTECAS
#include <Adafruit_GFX.h>    
#include <Adafruit_TFTLCD.h> 
#include <TouchScreen.h>

// DEFINIÇÕES DE PINOS
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4


// DEFINIÇÕES
#define WHITE          0xFFFF
#define YELLOW         0xFFE0
#define RED            0xF800
#define BLUE           0x001F
#define BLACK          0x0000

#define LIGADO         0x07E0
#define DESLIGADO      0xF800

#define MINPRESSURE 10
#define MAXPRESSURE 1000


/*____CALIBRAR TFT LCD_____*/
#define YP A1
#define XM A2
#define YM 7
#define XP 6

#define TS_LEFT 162
#define TS_RT   897
#define TS_TOP  929
#define TS_BOT  205
/*______FIM DA CALIBRAÇÃO______*/

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300); //300 É A SENSITIVIDADE
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

int X, Y;
long corBot[3] = {DESLIGADO, DESLIGADO, DESLIGADO};

TSPoint waitTouch() {
  TSPoint p;
  do {
    p = ts.getPoint();
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
  } while ((p.z < MINPRESSURE ) || (p.z > MAXPRESSURE));
  
  p.x = map(p.x, TS_LEFT, TS_RT, 0, 240);
  p.y = map(p.y, TS_BOT, TS_TOP, 0, 320);
  
  return p;
}

void setup() {
  Serial.begin(9600);
  
  tft.reset();
  tft.begin(0x9341); // CÓDIGO DO DRIVER DO SEU DISPLAY
  tft.setRotation(2);
  tft.fillScreen(BLACK);

  draw_Buttons(0); // DESENHA TODOS OS BOTOES
}

void loop() {
  TSPoint p = waitTouch();
  X = p.x; Y = p.y;

  DetectButtons(); // DETECTA OS BOTOES

  delay(300);
}

// IMPLEMENTO DE FUNÇÕES

void DetectButtons() {

  if (X < 200 && X > 40) {

    if (Y > 220 && Y < 300) { // LOGICA PARA O BOTAO 1
      Serial.println ("Button 1");
      if (corBot[0] == LIGADO) corBot[0] = DESLIGADO;
      else corBot[0] = LIGADO;
      draw_Buttons(1);
    }


    if (Y > 120 && Y < 200) { // LOGICA PARA O BOTAO 2
      Serial.println ("Button 2");
      if (corBot[1] == LIGADO) corBot[1] = DESLIGADO;
      else corBot[1] = LIGADO;
      draw_Buttons(2);
    }

    if (Y > 20 && Y < 100) { // LOGICA PARA O BOTAO 3
      Serial.println ("Button 3");
      if (corBot[2] == LIGADO) corBot[2] = DESLIGADO;
      else corBot[2] = LIGADO;
      draw_Buttons(3);
    }

  }

}

void draw_Buttons(int type) {
  switch (type) {
    case 1:
      tft.fillRect  (20, 20 , 190, 80, corBot[0]);

      tft.setTextColor(BLACK);
      tft.setTextSize (2);
      tft.setCursor(30, 50);
      tft.println("Incluir cartao");
      break;

    case 2:
      tft.fillRect  (20, 120, 190, 80, corBot[1]);

      tft.setTextColor(BLACK);
      tft.setTextSize (2);
      tft.setCursor(30, 150);
      tft.println("Utilizar cartao");
      break;

    case 3:
      tft.fillRect  (20, 220, 190, 80, corBot[2]);

      tft.setTextColor(BLACK);
      tft.setTextSize (2);
      tft.setCursor(30, 250);
      tft.println("Logoof");
      break;

    default:
      tft.fillRect  (20, 20 , 190, 80, corBot[0]);
      tft.fillRect  (20, 120, 190, 80, corBot[1]);
      tft.fillRect  (20, 220, 190, 80, corBot[2]);

      tft.setTextColor(BLACK);
      tft.setTextSize (2);

      tft.setCursor(30, 50);
      tft.println("Incluir cartao");

      tft.setCursor(30, 150);
      tft.println("Utilizar cartao");

      tft.setCursor(30, 250);
      tft.println("LogOff");
  }

}