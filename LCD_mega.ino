
#include <avr/io.h>
#include <util/delay.h> // Se recomienda usar _delay_ms() si solo son valores pequeños


// Definiciones de Pines de Control del LCD (PORTB)
#define LCD_CONTROL_PORT PORTB
#define LCD_CONTROL_DDR DDRB

#define RS_PIN (1 << PB2) // RS conectado a PB2 (Digital Pin 10 en Arduino Uno)
#define EN_PIN (1 << PB3) // EN conectado a PB3 (Digital Pin 11 en Arduino Uno)

// Constantes para la conversión
#define VREF_MV 5000L   // Referencia de voltaje en milivolts (5V)
#define ADC_MAX 1024L   // 2^10 = 1024 niveles

// --------------------------------------------------------------------------------
// PROTOTIPOS DE FUNCIONES (Para que el compilador sepa que existen)
// --------------------------------------------------------------------------------
void set_lcd_data_pins(uint8_t data);
void lcd_pulse_enable(void);
void lcd_command(uint8_t cmd);
void lcd_data(uint8_t data);
void init_lcd(void);
void lcd_write_string(const char *str);
void lcd_set_cursor(uint8_t row, uint8_t col);
void delay_ms(int ms); 
uint16_t read_adc(void);
void display_number(uint16_t num, uint8_t digits);


void setup() {
    // 1. Configuración de Puertos para el LCD y el ADC
    
    // Configurar los 8 pines de datos (D2-D9) como SALIDA
    DDRD |= (1 << PD2) | (1 << PD3) | (1 << PD4) | (1 << PD5) | (1 << PD6) | (1 << PD7); // D2-D7
    DDRB |= (1 << PB0) | (1 << PB1); // D8-D9

    // Configurar los pines de control (D10 y D11) como SALIDA
    LCD_CONTROL_DDR |= (RS_PIN | EN_PIN); 

    // 2. Inicialización del LCD
    init_lcd();
    lcd_set_cursor(0, 0);
    lcd_write_string("RAW: ");
    lcd_set_cursor(1, 0);
    lcd_write_string("mV: ");

    // 3. Configuración del ADC (Pin A0/ADC0)
    // ADMUX: Vref=AVCC (01), Justificación Derecha (0), Canal ADC0 (0000)
    ADMUX = (0 << REFS1) | (1 << REFS0) | (0 << ADLAR) | (0 << MUX0); 
    // ADCSRA: Habilitar ADC (ADEN=1), Prescaler 128 (111)
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

void loop() {
    uint16_t raw_value;
    uint32_t mv_value; 

    // Rutina de Lectura del ADC
    raw_value = read_adc(); 

    mv_value = ( (uint32_t)raw_value * VREF_MV ) / ADC_MAX;

    // Actualización de la pantalla
    
    // Primera línea: RAW: xxxx
    lcd_set_cursor(0, 5); 
    display_number(raw_value, 4); 

    // Segunda línea: mV: xxxx
    lcd_set_cursor(1, 4); 
    display_number((uint16_t)mv_value, 4);
    
    // Retardo para estabilidad y legibilidad (100 ms)
    delay_ms(100); 
}



// Función para establecer los 8 pines de datos del LCD con manejo disperso
void set_lcd_data_pins(uint8_t data) {
    // Borrar bits previos en PORTD (D2-D7)
    PORTD &= ~((1 << PD2) | (1 << PD3) | (1 << PD4) | (1 << PD5) | (1 << PD6) | (1 << PD7));
    // Borrar bits previos en PORTB (D8-D9)
    PORTB &= ~((1 << PB0) | (1 << PB1));

    // Escribir los 8 bits en sus pines correspondientes (D0-D7)
    if (data & (1 << 0)) PORTD |= (1 << PD2); // D0 -> D2
    if (data & (1 << 1)) PORTD |= (1 << PD3); // D1 -> D3
    if (data & (1 << 2)) PORTD |= (1 << PD4); // D2 -> D4
    if (data & (1 << 3)) PORTD |= (1 << PD5); // D3 -> D5
    if (data & (1 << 4)) PORTD |= (1 << PD6); // D4 -> D6
    if (data & (1 << 5)) PORTD |= (1 << PD7); // D5 -> D7
    if (data & (1 << 6)) PORTB |= (1 << PB0); // D6 -> D8
    if (data & (1 << 7)) PORTB |= (1 << PB1); // D7 -> D9
}

// Pulso de habilitación
void lcd_pulse_enable(void) {
    LCD_CONTROL_PORT |= EN_PIN;  // EN = HIGH
    delay_ms(1);                 // Pequeño retardo
    LCD_CONTROL_PORT &= ~EN_PIN; // EN = LOW
    delay_ms(1);                 // Retardo para el comando/dato
}

// Envío de comando
void lcd_command(uint8_t cmd) {
    LCD_CONTROL_PORT &= ~RS_PIN; // RS = LOW (Modo Comando)
    set_lcd_data_pins(cmd);      // Poner el comando en los pines dispersos
    lcd_pulse_enable();
}

// Envío de dato/carácter
void lcd_data(uint8_t data) {
    LCD_CONTROL_PORT |= RS_PIN; // RS = HIGH (Modo Dato)
    set_lcd_data_pins(data);    // Poner el dato en los pines dispersos
    lcd_pulse_enable();
}

// Inicialización del LCD (Modo 8 bits)
void init_lcd(void) {
    delay_ms(50); // Retardo inicial después del encendido

    // Secuencia de inicialización del LCD 16x2 (Modo 8 bits)
    lcd_command(0x38); // Function Set: 8 bits, 2 líneas, 5x8 dots
    delay_ms(5);
    lcd_command(0x38); 
    delay_ms(1);
    lcd_command(0x38); 

    lcd_command(0x0C); // Display ON, Cursor OFF, Blink OFF
    lcd_command(0x06); // Entry Mode Set: Incrementa y No Shift
    lcd_command(0x01); // Limpiar pantalla
    delay_ms(2);
}

// Escritura de una cadena de caracteres
void lcd_write_string(const char *str) {
    while (*str) {
        lcd_data(*str++);
    }
}

// Configuración de la posición del cursor (Fila: 0 o 1, Columna: 0 a 15)
void lcd_set_cursor(uint8_t row, uint8_t col) {
    uint8_t address = col;
    if (row == 1) {
        address += 0x40; // La segunda línea empieza en la dirección 0x40
    }
    lcd_command(0x80 | address); // Comando 0x80 establece la dirección
}

// Función de lectura del ADC 
uint16_t read_adc(void) {
    ADCSRA |= (1 << ADSC); // Inicio de Conversión
    while (ADCSRA & (1 << ADSC)); // Esperar a que ADSC se borre (conversión terminada)
    
    // Lectura de 10 bits: ADCL primero, ADCH después
    uint16_t result = ADCL;
    result |= (ADCH << 8);

    return result; 
}

// Función para mostrar un número con relleno de ceros
void display_number(uint16_t num, uint8_t digits) {
    char buffer[5]; // Espacio para 4 dígitos + '\0'
    int i = digits - 1;

    // Rellenar con ceros
    for (i = digits - 1; i >= 0; i--) {
        buffer[i] = (num % 10) + '0';
        num /= 10;
    }
    buffer[digits] = '\0'; // Terminador de cadena

    // Enviar al LCD
    for (i = 0; i < digits; i++) {
        lcd_data(buffer[i]);
    }
}

// Función de retardo simple (No precisa, pero funcional para esta práctica)
void delay_ms(int ms) {
    for (int i = 0; i < ms; i++) {
        // Usa una función de retardo de util/delay.h si se define F_CPU
        // Aquí se usa un loop simple como alternativa para mantener la portabilidad
        for (long j = 0; j < 1600; j++) { 
            __asm__("nop"); 
        }
    }
}