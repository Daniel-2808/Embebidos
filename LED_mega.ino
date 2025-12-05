void setup() {
  // PB1 como salida (LED)
  DDRB |= (1 << PB1);

  // PD2 como entrada (botón)
  DDRD &= ~(1 << PD2);

  // No activar pull-up porque el circuito usa pull-down externa
  PORTD &= ~(1 << PD2);
}

void loop() {
  static uint8_t estadoLED = 0;
  static uint8_t estadoAnterior = 0;

  // Leer pin PD2
  uint8_t lectura = (PIND & (1 << PD2));

  // Detectar flanco ascendente (0 → 1)
  if (lectura && !estadoAnterior) {
    estadoLED = !estadoLED;

    if (estadoLED)
      PORTB |= (1 << PB1);   // Encender LED
    else
      PORTB &= ~(1 << PB1);  // Apagar LED

    delay(50);  // Antirrebote
  }

  estadoAnterior = lectura;
}
