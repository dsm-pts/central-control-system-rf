#include <SPI.h>
#include "RF24.h"

#define ADDRESS_RPI 0
#define ADDRESS_BUS 1

// pin 10 : CE
// pin 9  : CSN
RF24 radio(10, 9);

uint8_t address[][6] = {"Node1", "Node2"};
char payload[6] = {0, 0, 0, 0, 0, 0};

void setup() {
  Serial.begin(115200);
  while(!radio.begin());
  
  radio.setPALevel(RF24_PA_LOW);
  radio.setPayloadSize(sizeof(payload));

  radio.openWritingPipe(address[ADDRESS_RPI]);    // TX, pipe 0
  radio.openReadingPipe(1, address[ADDRESS_BUS]); // RX, pipe 1
  
  radio.startListening();
} // setup

void loop() {
  if (Serial.available()) {
    radio.stopListening();

    while(Serial.read() != 0x02 && Serial.available());
    for (int i = 0; i < 4; i++) {
       payload[i] = Serial.read();
    }
    while(Serial.read() != 0x03 && Serial.available());
    
    bool report = radio.write(&payload, sizeof(payload));
    if (!report) {
      Serial.println(F("Transmission failed or timed out"));
    }
  } else {
    radio.startListening();
    
    uint8_t pipe;
    if (radio.available(&pipe)) {
      uint8_t bytes = radio.getPayloadSize();
      radio.read(&payload, bytes);
      Serial.print(payload);
    }
  }
} // loop
