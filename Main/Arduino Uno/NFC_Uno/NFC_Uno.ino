#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <NfcAdapter.h>
#include <Wire.h>

PN532_SPI pn532spi(SPI, 10);
PN532 nfc(pn532spi);

String emulatedUID = "";  // Variável para armazenar o UID para emulação

void setup() {
  Serial.begin(9600); // Comunicação serial para receber UID
  
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.print("PN53x não encontrado. Verifique a conexão.");
    while (1);
  }

}

void loop() {
  if (Serial.available() > 0) {
    char command = Serial.read();
    
    if (command == 'R') { // Comando para ler o cartão
      Ler_Cartao();
    } else if (command == 'E') { // Comando para iniciar a emulação
      ReceberUID();
      Emulate_Card(emulatedUID);
    }
    
    delay(5000);
  }
}

void Ler_Cartao() {
  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
    Serial.print("UID detectado: ");
    for (uint8_t i = 0; i < uidLength; i++) {
      Serial.print(uid[i], HEX);
    }
    Serial.println();
  } else {
    Serial.println("Nenhum cartão detectado.");
  }
}

// Função para receber o UID via serial
void ReceberUID() {
  emulatedUID = ""; // Limpa o UID anterior

  // Espera o UID via serial
  while (Serial.available() == 0) {
    delay(10);  // Pequeno delay para evitar espera ativa
  }

  emulatedUID = Serial.readStringUntil('\n'); // Lê o UID como string até o caractere de nova linha
  Serial.print("UID para emulação recebido: ");
  Serial.println(emulatedUID);
}

// Função para emular o cartão NFC com o UID especificado
void Emulate_Card(String uidToEmulate) {
  uint8_t ndefRecord[] = {
    0xD1, 0x01, 0x0E, 0x54, 0x02, 0x65, 0x6E,  // Header da mensagem NDEF
    'E', 'm', 'u', 'l', 'a', 't', 'e',          // Mensagem de exemplo
    ' ', 'T', 'a', 'g'
  };
  nfc.emulateTag(ndefRecord, sizeof(ndefRecord), uidToEmulate.c_str());
  Serial.println("Emulação NFC ativa com o UID: " + uidToEmulate);
  delay(1000);  // Delay para manter a emulação ativa por algum tempo

  nfc.stopEmulation(); // Finaliza a emulação após o delay
  Serial.println("Emulação finalizada.");
}
