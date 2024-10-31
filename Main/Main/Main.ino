#include <NfcAdapter.h>
#include <SD.h>
#include <Adafruit_Fingerprint.h>
#include <Adafruit_GFX.h>    
#include <Adafruit_TFTLCD.h> 
#include <TouchScreen.h>
#include <MCUFRIEND_kbv.h>

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
MCUFRIEND_kbv tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

String tagId = "None";
byte nuidPICC[4];

const int chipSelect = 53; //Variaveis Card SD
File myFile;
String lastUID = ""; // Armazena o último UID registrado
String encryptionKey = "Xmitt"; // Chave de criptografia

const uint32_t password = 0x0; //Variaveis biometria
Adafruit_Fingerprint fingerprintSensor = Adafruit_Fingerprint(&Serial2, password);
int location = 0;

int X, Y;
long corBot[3] = {DESLIGADO, DESLIGADO, DESLIGADO};
bool valid_nfc = false;

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
  Serial1.begin(9600);
  
  tft.reset();
  tft.begin(0x9341); // CÓDIGO DO DRIVER DO SEU DISPLAY
  tft.setRotation(2);
  tft.fillScreen(BLACK);

  verify_Biometry();

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

    if (Y > 220 && Y < 300) { // LOGICA PARA O BOTAO 3
      Serial.println ("Button 3");
      if (corBot[2] == LIGADO) corBot[2] = DESLIGADO;
      else corBot[2] = LIGADO;
      draw_Buttons(3);
    }


    if (Y > 120 && Y < 200) { // LOGICA PARA O BOTAO 2
      Serial.println ("Button 2");
      if (valid_nfc){
        Valid_NFC();
      }else if (corBot[1] == LIGADO) corBot[1] = DESLIGADO;
      else corBot[1] = LIGADO;
      draw_Buttons(2);
    }

    if (Y > 20 && Y < 100) { // LOGICA PARA O BOTAO 1
      Serial.println ("Button 1");
      Tela_NFC();
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
      tft.println("Ler NFC");
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
      tft.println("Ler NFC");

      tft.setCursor(30, 150);
      tft.println("Utilizar cartao");

      tft.setCursor(30, 250);
      tft.println("LogOff");
  }

}

void Tela_NFC(){

  tft.fillScreen(BLACK);

  tft.fillRect  (20, 120, 190, 80, YELLOW);

  tft.setTextColor(BLACK);
  tft.setTextSize (2);
  tft.setCursor(30, 150);
  tft.println("Utilizar cartao");

  valid_nfc = true;

}

void Valid_NFC(){

  Serial.println ("Validando NFC");

  tagId = " ";

  Serial1.write('R'); // Envia o comando 'R' para o UNO

  Serial.println( Serial1.available() );

  if (Serial1.available() > 0) {
    String cardData = Serial1.readStringUntil('\n');

    Serial.println(cardData);

    if ( ( !cardData.startsWith("Tag") ) && ( cardData.length() == 12 ) ) { // Valida se o dado é um UID

      Serial.println("UID do Cartão NFC: " + cardData); // Exibe apenas o UID no monitor serial

      // Grava no SD apenas se o UID for diferente do último registrado
      if (cardData != lastUID) {
        String encryptedData = encrypt(cardData, encryptionKey); // Criptografa o UID
        Write_SD(encryptedData);
        lastUID = cardData; // Atualiza o último UID registrado
      }
    }else{

      delay(1000);

      Valid_NFC();

    }
  }
}

// Função de criptografia XOR
String encrypt(String data, String key) {
  String encryptedData = data;
  for (int i = 0; i < data.length(); i++) {
    encryptedData[i] = data[i] ^ key[i % key.length()]; // XOR com a chave
  }
  return encryptedData;
}

// Função para escrever no SD
void Write_SD(String encryptedData) {

   // Inicializa o cartão SD
  if (!SD.begin(chipSelect)) {
    Serial.println("Erro ao inicializar o cartão SD");
    return;
  }
  Serial.println("Cartão SD inicializado com sucesso");
  
  // Remove o arquivo anterior antes de criar um novo
  SD.remove("card1.txt");
  
  myFile = SD.open("card1.txt", FILE_WRITE);

  // Se o arquivo foi aberto, grava nele
  if (myFile) {
    Serial.print("Writing encrypted data to card1.txt...");
    myFile.println(encryptedData);
    myFile.close(); // Fecha o arquivo
    Serial.println("done.");

    String decryptedData = encrypt(encryptedData, encryptionKey); //Testa descriptogração
    Serial.println("Dado descriptografado (UID): " + decryptedData);

  } else {
    // Se o arquivo não abriu, exibe erro
    Serial.println("error opening card1.txt");
  }
}

void verify_Biometry(){

  fingerprintSensor.begin(57600);
  if (!fingerprintSensor.verifyPassword()) {
    Serial.println(F("Não foi possível conectar ao sensor. Verifique a senha ou a conexão"));
    while (true)
      ;
  }

  fingerprintSensor.getTemplateCount();

  Serial.println( fingerprintSensor.templateCount );

  if (fingerprintSensor.templateCount == 0){

    tft.fillScreen(BLACK);
    tft.fillRect  ( 0, 20 , 190, 80, BLUE);

      tft.setTextColor(BLACK);
      tft.setTextSize (2);
      tft.setCursor(10, 50);
      tft.println("Biometria não cadastrada, aproxime o dedo do sensor");

      location = fingerprintSensor.getTemplateCount() + 1;

      while (fingerprintSensor.getImage() != FINGERPRINT_OK)
    ;

    //Converte a imagem para o primeiro padrão
    if (fingerprintSensor.image2Tz(1) != FINGERPRINT_OK) {
      //Se chegou aqui deu erro, então abortamos os próximos passos
      Serial.println("Erro image2Tz 1");
      return;
    }

    Serial.println("Tire o dedo do sensor");
    delay(2000);
    //Espera até tirar o dedo
    while (fingerprintSensor.getImage() != FINGERPRINT_NOFINGER)
      ;
    //Antes de guardar precisamos de outra imagem da mesma digital
    Serial.println("Encoste o mesmo dedo no sensor");
    //Espera até pegar uma imagem válida da digital
    while (fingerprintSensor.getImage() != FINGERPRINT_OK)
      ;
    //Converte a imagem para o segundo padrão
    if (fingerprintSensor.image2Tz(2) != FINGERPRINT_OK) {
      //Se chegou aqui deu erro, então abortamos os próximos passos
      Serial.println("Erro image2Tz 2");
      return;
    }
    //Cria um modelo da digital a partir dos dois padrões
    if (fingerprintSensor.createModel() != FINGERPRINT_OK) {
      //Se chegou aqui deu erro, então abortamos os próximos passos
      Serial.println("Erro createModel");
      return;
    }
    //Guarda o modelo da digital no sensor
    if (fingerprintSensor.storeModel(location) != FINGERPRINT_OK) {
      //Se chegou aqui deu erro, então abortamos os próximos passos
      Serial.println("Erro storeModel");
      return;
    }
    //Se chegou aqui significa que todos os passos foram bem sucedidos
    Serial.println("Sucesso!!!");

  }else{

    checkFingerprint();

  }

}

//Verifica se a digital está cadastrada
void checkFingerprint() {
  Serial.println("Encoste o dedo no sensor");
  //Espera até pegar uma imagem válida da digital
  while (fingerprintSensor.getImage() != FINGERPRINT_OK)
    ;
  //Converte a imagem para o padrão que será utilizado
  //para verificar com o banco de digitais
  if (fingerprintSensor.image2Tz() != FINGERPRINT_OK) {
    //Se chegou aqui deu erro, então abortamos os próximos passos
    Serial.println("Erro image2Tz");
    return;
  }
  //Procura por este padrão no banco de digitais
  if (fingerprintSensor.fingerFastSearch() != FINGERPRINT_OK) {
    //Se chegou aqui significa que a digital não foi encontrada
    Serial.println("Digital não encontrada");
    return;
  }
  //Se chegou aqui a digital foi encontrada
  //Mostramos a posição onde a digital estava salva e a confiança
  //Quanto mais alta a confiança melhor
  Serial.print("Digital encontrada!");
}  //checkFingerprint