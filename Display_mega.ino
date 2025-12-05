// Segmentos del display: del PIN 2 al PIN 8 (PD2–PD7) + PB0 (D8)
#define BOTON_PIN   9   // Botón en D9 (PB1)

// Tabla de segmentos (cátodo común) para 0–9
// Bits: g f e d c b a   (usaremos PORTD y PORTB)
const uint8_t numeros[10] = {
  // a b c d e f g
  0b00111111, // 0
  0b00000110, // 1
  0b01011011, // 2
  0b01001111, // 3
  0b01100110, // 4
  0b01101101, // 5
  0b01111101, // 6
  0b00000111, // 7
  0b01111111, // 8
  0b01101111  // 9
};

void mostrarNumero(uint8_t n) {
  uint8_t seg = numeros[n];

  // PORTD controla pines D2–D7 → segmentos a–f
  PORTD = (PORTD & 0b00000011) | (seg << 2);

  // PORTB controla D8 → segmento g
  if (seg & 0b01000000)
    PORTB |=  (1 << PB0);  // HIGH
  else
    PORTB &= ~(1 << PB0);  // LOW
}

void setup() {
  // Configurar pines D2–D7 como salida (PD2–PD7)
  DDRD |= 0b11111100;

  // D8 como salida (PB0)
  DDRB |= (1 << PB0);

  // D9 como entrada (PB1)
  DDRB &= ~(1 << PB1);

  // Pull-down manual → no activamos pull-up
}

uint8_t numero = 0;
uint8_t ultimoEstado = 0;

void loop() {
  uint8_t lectura = (PINB & (1 << PB1)) != 0; // Leer D9 por registros

  if (lectura && !ultimoEstado) {
    delay(25); // antirrebote

    if ((PINB & (1 << PB1)) != 0) {
      numero++;
      if (numero > 9) numero = 0;
      mostrarNumero(numero);
    }
  }

  ultimoEstado = lectura;
}