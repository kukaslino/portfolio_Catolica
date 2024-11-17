#ifdef ARDUINO

//#ifndef UNIT_TEST
#include <NfcAdapter.h>
#include <SD.h>
#include <Adafruit_Fingerprint.h>
#include <Adafruit_GFX.h>    
#include <Adafruit_TFTLCD.h> 
#include <TouchScreen.h>
#include <MCUFRIEND_kbv.h>
#include <avr/wdt.h>

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
#define YP A2
#define XM A1
#define YM 6
#define XP 7

#define TS_LEFT 886
#define TS_RT   177
#define TS_TOP  935
#define TS_BOT  203
/*______FIM DA CALIBRAÇÃO______*/

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300); //300 É A SENSITIVIDADE
MCUFRIEND_kbv tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

String tagId = "None";
byte nuidPICC[4];

const int chipSelect = 53; //Variaveis Card SD
File myFile;
String lastUID = ""; // Armazena o último UID registrado
String cardData = "";
String encryptionKey = "Xmitt"; // Chave de criptografia

const uint32_t password = 0x0; //Variaveis biometria
Adafruit_Fingerprint fingerprintSensor = Adafruit_Fingerprint(&Serial2, password);
int location = 0;

//Variaveis de controle
int X, Y;
long corBot[3]     = {DESLIGADO, DESLIGADO, DESLIGADO};
bool valid_nfc     = false;
bool emulate_card  = false;
bool format_system = false;
bool valid_return  = false;

void resetArduino() {

  #ifdef ARDUINO

    wdt_enable(WDTO_15MS); // Configura o Watchdog Timer para reiniciar em 15ms
    while (true) {}        // Entra em loop infinito até o watchdog acionar o reset

  #endif

}

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

String read_SD(int fileNumber) {

  String fileName = "card" + String(fileNumber) + ".txt";

  Iniciate_SD();

  File myFile = SD.open(fileName);
  if (myFile) {
    Serial.print("Lendo o conteúdo de ");
    Serial.println(fileName);

    String fileContent = "";
    while (myFile.available()) {
      fileContent += (char)myFile.read();
    }
    myFile.close(); // Fecha o arquivo após a leitura

    String decryptedContent = encrypt(fileContent, encryptionKey);
    return decryptedContent;
  } else {
    Serial.print("Erro ao abrir ");
    Serial.println(fileName);
    return "";
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

void setup() {

  Serial.begin(9600);
  Serial1.begin(9600);
  
  tft.reset();
  tft.begin(0x9341);
  tft.setRotation(2);
  tft.fillScreen(BLACK);

  //verify_Biometry();

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
      if (valid_nfc){
        Valid_NFC( 3 );
      }else if ( emulate_card ){
        Emulate_Card( 3 );
      }else{
        FormatSystem();
      }
    }


    if (Y > 120 && Y < 200) { // LOGICA PARA O BOTAO 2
      Serial.println ("Button 2");
      if (valid_nfc){
        Valid_NFC( 2 );
      }else if ( !emulate_card ){
        Tela_Emulate();
      }else if ( emulate_card){
        Emulate_Card( 2 );
      }
    }

    if (Y > 20 && Y < 100) { // LOGICA PARA O BOTAO 1
      Serial.println ("Button 1");
      if (valid_nfc){
        Valid_NFC( 1 );
      }else if ( emulate_card ){
        Emulate_Card( 1 );
      }else{

        Tela_NFC();

      }
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
      tft.println("Emular Cartão");
      break;

    case 3:
      tft.fillRect  (20, 220, 190, 80, corBot[2]);

      tft.setTextColor(BLACK);
      tft.setTextSize (2);
      tft.setCursor(30, 250);
      tft.println("Formatar disppsitivo");
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
      tft.println("Emular Cartão");

      tft.setCursor(30, 250);
      tft.println("Formatar disppsitivo");
  }

}

void Tela_NFC(){

  tft.fillScreen(BLACK);

  tft.fillRect  (20, 20 , 190, 80, YELLOW);
  tft.fillRect  (20, 120, 190, 80, YELLOW);
  tft.fillRect  (20, 220, 190, 80, YELLOW);

  tft.setTextColor(BLACK);
  tft.setTextSize (2);

  tft.setCursor(30, 50);
  tft.println("Cartao 01");

  tft.setCursor(30, 150);
  tft.println("Cartao 02");

  tft.setCursor(30, 250);
  tft.println("Cartao 03");

  valid_nfc = true;

}

void Tela_Emulate(){

  tft.fillScreen(BLACK);

  tft.fillRect  (20, 20 , 190, 80, RED);
  tft.fillRect  (20, 120, 190, 80, RED);
  tft.fillRect  (20, 220, 190, 80, RED);

  tft.setTextColor(BLACK);
  tft.setTextSize (2);

  tft.setCursor(30, 50);
  tft.println("Cartao 01");

  tft.setCursor(30, 150);
  tft.println("Cartao 02");

  tft.setCursor(30, 250);
  tft.println("Cartao 03");

  emulate_card = true;

}

void FormatSystem(){

  Serial.println( "Inicando formatação de sistema" );

  // Remove todas as digitais
  valid_return = deleteAllFingerprints();

  // Remove todos os arquivos do cartão SD
  deleteAllSDFiles();

  if ( valid_return ){

    Serial.println( "Formatação realizada com sucesso" );

    //Tela de sucesso

    resetArduino();

  }else{

    Serial.println( "Erro ao formatar sistema" );

    //Tela de erro

    resetArduino();

  }

}

bool deleteAllFingerprints() {

  Serial.println("Deletando todas as digitais...");

  if (fingerprintSensor.emptyDatabase() == FINGERPRINT_OK) {
    Serial.println("Todas as digitais foram deletadas com sucesso!");
    return true;
  } else {
    Serial.println("Falha ao deletar as digitais. Verifique o sensor.");
    return false;
  }

}

bool deleteAllSDFiles() {

  Iniciate_SD();

  Serial.println("Deletando todos os arquivos do cartão SD...");
  File root = SD.open("/");
  File file = root.openNextFile();

  while (file) {
    if (!file.isDirectory()) {
      Serial.print("Deletando arquivo: ");
      Serial.println(file.name());
      if (SD.remove(file.name())) {
        Serial.println("Arquivo deletado com sucesso!");
      } else {
        Serial.println("Falha ao deletar o arquivo.");
      }
    }
    file.close();
    file = root.openNextFile();
  }
  root.close();
  Serial.println("Todos os arquivos foram deletados do cartão SD.");
}

void Emulate_Card( int card_select ){

  Serial.println ("Emulando NFC");

  cardData = read_SD( card_select );

  Serial.println( cardData );

  Serial1.write( 'E' );

  Serial1.println( cardData );

  if (Serial1.available() > 0) {

    cardData = Serial1.readStringUntil('END');

    Serial.println(cardData);

  }

}

void Valid_NFC( int fileNumber ){

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
        Write_SD(encryptedData, fileNumber);
        lastUID = cardData; // Atualiza o último UID registrado
      }
    }else{

      delay(1000);

      Valid_NFC( fileNumber );

    }
  }
}

// Função para escrever no SD
void Write_SD(String encryptedData, int fileNumber) {
  
  // Cria o nome do arquivo com base no número
  String fileName = "card" + String(fileNumber) + ".txt";
  
  // Remove o arquivo anterior antes de criar um novo
  SD.remove(fileName.c_str());

  myFile = SD.open(fileName.c_str(), FILE_WRITE);

  // Se o arquivo foi aberto, grava nele
  if (myFile) {
    Serial.print("Writing encrypted data to ");
    Serial.print(fileName);
    Serial.println("...");
    myFile.println(encryptedData);
    myFile.close(); // Fecha o arquivo
    Serial.println("done.");

    String decryptedData = encrypt(encryptedData, encryptionKey); // Testa descriptografação
    Serial.println("Dado descriptografado (UID): " + decryptedData);
  } else {
    Serial.print("Erro ao abrir ");
    Serial.println(fileName);
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

    if ( !checkFingerprint() ){

      Serial.println( 'Digital não encontrada, tente novamente' );
      checkFingerprint();

    }

  }

}

//Verifica se a digital está cadastrada
bool checkFingerprint() {
  Serial.println("Encoste o dedo no sensor");
  //Espera até pegar uma imagem válida da digital
  while (fingerprintSensor.getImage() != FINGERPRINT_OK)
    ;
  //Converte a imagem para o padrão que será utilizado
  //para verificar com o banco de digitais
  if (fingerprintSensor.image2Tz() != FINGERPRINT_OK) {
    //Se chegou aqui deu erro, então abortamos os próximos passos
    Serial.println("Erro image2Tz");
    return false;
  }
  //Procura por este padrão no banco de digitais
  if (fingerprintSensor.fingerFastSearch() != FINGERPRINT_OK) {
    //Se chegou aqui significa que a digital não foi encontrada
    Serial.println("Digital não encontrada");
    return false;
  }
  //Se chegou aqui a digital foi encontrada
  //Mostramos a posição onde a digital estava salva e a confiança
  //Quanto mais alta a confiança melhor
  Serial.print("Digital encontrada!");
}  //checkFingerprint

void Iniciate_SD(){
  // Inicializa o cartão SD
  if (!SD.begin(chipSelect)) {
    Serial.println("Erro ao inicializar o cartão SD");
  }
}

//#endif

#else

#include <iostream>
int main() {
    std::cout << "Simulando ambiente Arduino\n";
    return 0;
}

#endif
