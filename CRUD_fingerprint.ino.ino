#include <Adafruit_Fingerprint.h>  //https://github.com/adafruit/Adafruit-Fingerprint-Sensor-Library
const uint32_t password = 0x0;
Adafruit_Fingerprint fingerprintSensor = Adafruit_Fingerprint(&Serial2, password);

void setup() {
  Serial.begin(9600);
  setupFingerprintSensor();
}

void setupFingerprintSensor() {
  fingerprintSensor.begin(57600);
  //Verifica se a senha está correta
  if (!fingerprintSensor.verifyPassword()) {
    Serial.println(F("Não foi possível conectar ao sensor. Verifique a senha ou a conexão"));
    while (true)
      ;
  }
}

void loop() {
  printMenu();
  String command = getCommand();

  int i = command.toInt();

  switch (i) {
    case 1:
      storeFingerprint();
      break;
    case 2:
      checkFingerprint();
      break;
    case 3:
      printStoredFingerprintsCount();
      break;
    case 4:
      deleteFingerprint();
      break;
    case 5:
      emptyDatabase();
      break;
    default:
      Serial.println(F("Opção inválida"));
      break;
  }
  delay(1000);
}

void printMenu() {
  Serial.println();
  Serial.println(F("Digite um dos números do menu abaixo"));
  Serial.println(F("1 - Cadastrar digital"));
  Serial.println(F("2 - Verificar digital"));
  Serial.println(F("3 - Mostrar quantidade de digitais cadastradas"));
  Serial.println(F("4 - Apagar digital em uma posição"));
  Serial.println(F("5 - Apagar banco de digitais"));
}

String getCommand() {
  while (!Serial.available()) delay(100);
  return Serial.readStringUntil('\n');
}


//Cadastro da digital
void storeFingerprint() {
  Serial.println(F("Qual a posição para guardar a digital? (1 a 149)"));
  String strLocation = getCommand();
  int location = strLocation.toInt();
  if (location < 1 || location > 149) {

    Serial.println(F("Posição inválida"));
    return;
  }
  Serial.println(F("Encoste o dedo no sensor"));
  while (fingerprintSensor.getImage() != FINGERPRINT_OK)
    ;

  if (fingerprintSensor.image2Tz(1) != FINGERPRINT_OK) {

    Serial.println(F("Erro image2Tz 1"));
    return;
  }

  Serial.println(F("Tire o dedo do sensor"));
  delay(2000);
  while (fingerprintSensor.getImage() != FINGERPRINT_NOFINGER)
    ;

  Serial.println(F("Encoste o mesmo dedo no sensor"));
  while (fingerprintSensor.getImage() != FINGERPRINT_OK)
    ;
  if (fingerprintSensor.image2Tz(2) != FINGERPRINT_OK) {
    
    Serial.println(F("Erro image2Tz 2"));
    return;
  }

  if (fingerprintSensor.createModel() != FINGERPRINT_OK) {
    
    Serial.println(F("Erro createModel"));
    return;
  }

  if (fingerprintSensor.storeModel(location) != FINGERPRINT_OK) {
    
    Serial.println(F("Erro storeModel"));
    return;
  }

  Serial.println(F("Sucesso!!!"));
}  //storeFingerprint

//Verifica se a digital está cadastrada
void checkFingerprint() {
  Serial.println(F("Encoste o dedo no sensor"));

  while (fingerprintSensor.getImage() != FINGERPRINT_OK)
    ;
  if (fingerprintSensor.image2Tz() != FINGERPRINT_OK) {
    
    Serial.println(F("Erro image2Tz"));
    return;
  }
  if (fingerprintSensor.fingerFastSearch() != FINGERPRINT_OK) {

    Serial.println(F("Digital não encontrada"));
    return;
  }

  Serial.print(F("Digital encontrada com confiança de "));
  Serial.print(fingerprintSensor.confidence);
  Serial.print(F(" na posição "));
  Serial.println(fingerprintSensor.fingerID);
}  //checkFingerprint

void printStoredFingerprintsCount() {

  fingerprintSensor.getTemplateCount();
  Serial.print(F("Digitais cadastradas: "));
  Serial.println(fingerprintSensor.templateCount);
}

void deleteFingerprint() {
  Serial.println(F("Qual a posição para apagar a digital? (1 a 149)"));

  String strLocation = getCommand();
  int location = strLocation.toInt();
  if (location < 1 || location > 149) {

    Serial.println(F("Posição inválida"));
    return;
  }

  if (fingerprintSensor.deleteModel(location) != FINGERPRINT_OK) {
    Serial.println(F("Erro ao apagar digital"));
  } else {
    Serial.println(F("Digital apagada com sucesso!!!"));
  }
}  //deleteFingerprint

void emptyDatabase() {
  Serial.println(F("Tem certeza? (s/N)"));

  String command = getCommand();
  command.toUpperCase();
  if (command == "S" || command == "SIM") {
    Serial.println(F("Apagando banco de digitais..."));
    if (fingerprintSensor.emptyDatabase() != FINGERPRINT_OK) {
      Serial.println(F("Erro ao apagar banco de digitais"));
    } else {
      Serial.println(F("Banco de digitais apagado com sucesso!!!"));
    }
  } else {
    Serial.println(F("Cancelado"));
  }
}  //emptyDatabase
