#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <NfcAdapter.h>

PN532_SPI pn532_spi(SPI, 10);
NfcAdapter nfc = NfcAdapter(pn532_spi);

void setup() {
  Serial.begin(9600); // Comunicação com o Mega
  nfc.begin();
}

void loop() {
  if (Serial.available() > 0) {
    char command = Serial.read();
    
    if (command == 'R') { // Comando para ler o cartão
      Ler_Cartao();
    }

    delay(5000);

  }
}

void Ler_Cartao() {
  if (nfc.tagPresent()) {
    NfcTag tag = nfc.read();
    String uid = tag.getUidString();
    Serial.println(uid);
  }
}