// #include "sim.h"

// MySim mob;
// SoftwareSerial mySerial(10, 11); // RX, TX
// MySim::MySim(/* args */)
// {
// }

// int16_t MySim::wait(char* Values, uint16_t timeout) {

//   uint16_t Length = strlen(Values);

//   char InputBuffer[Length + 1];
//   strcpy(InputBuffer, Values);
//   char CompareBuffer[Length + 1];
//   memset(CompareBuffer,0,sizeof(CompareBuffer));

//   uint16_t Quantity = 1;
  
//   for (int16_t n = 0; n < Length; n++) {
//     if (InputBuffer[n] == ',')
//       Quantity++;
//   }

//   char* InputTokens[Quantity];
//   memset(InputTokens, 0, sizeof(InputTokens));
//   char* CompareTokens[Quantity];
//   memset(CompareTokens, 0, sizeof(CompareTokens));

//   InputTokens[0] = InputBuffer;
//   CompareTokens[0] = CompareBuffer;

//   uint16_t TokenPosition = 1;
//   for (int16_t n = 0; n < Length; n++) {
//     if (InputBuffer[n] == ',') {
//       InputBuffer[n] = 0;
//       InputTokens[TokenPosition] = &InputBuffer[n + 1];
//       CompareTokens[TokenPosition] = &CompareBuffer[n + 1];
//       TokenPosition++;
//     }
//   }

//   uint64_t timer = millis();
//   char c;

//   while (millis() - timer < timeout) {
//     while (mySerial.available()) {
//       c = mySerial.read();
//       Serial.print(c);

//       for (int16_t n = 0; n < Quantity; n++) {
//         Length = strlen(CompareTokens[n]);
//         if (c == InputTokens[n][Length])
//           CompareTokens[n][Length] = c;
//         else
//           memset(CompareTokens[n], 0, Length);

//         if (!strcmp(InputTokens[n],CompareTokens[n]))
//           return n;
//       }
//     }
//   }
//   return -1;
// }