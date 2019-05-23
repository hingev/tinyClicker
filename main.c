#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#define NEXT 			PB2
#define PREV 			PB3

typedef enum {NONE, RIGHT, LEFT} STATUS;

void enable_pwm () {
	TCCR0B |= (1 << CS00);												/* prescaler 1:1 */
}

void disable_pwm () {
	TCCR0B = 0;
}

void send_bit (uint8_t v) {
	enable_pwm ();
	_delay_ms (0.56);
	disable_pwm ();
	if (v)
		_delay_ms (1.69);
	else
		_delay_ms (5.6);
}

void send_byte (uint8_t b) {
	register uint8_t i = 0 ;
	for (i =0 ; i < 8; ++i) {
		send_bit (b & (1 << (7 - i)));
	}
}

void send_code (uint8_t code) {

	uint8_t addr = 0;

	enable_pwm ();
	_delay_ms (9);
	disable_pwm ();
	_delay_ms (4.5);

	send_byte (0x00);
	send_byte (0xff);
	send_byte (code);
	send_byte (code ^ 0xff);

}

int main (void) {

	/* init Timer0 for PWM on OC0A */
	/* of the 38khz carrier for IR NEC */
	TCCR0A |= (1 << COM0A0) | (1 << WGM01); 			/* CTC, toggle on match */
	OCR0A = 26; // (1 MHZ / 26 ) = 38.4 Khz ~ 38 khz carrier


	/* set up pullups for next/prev buttons  */
	PORTB |= (1 << NEXT) | (1 << PREV);


	while (1) {
		STATUS doing = NONE;
		uint8_t code = 0;

		if (PINB & (1 << NEXT)) {
			doing = RIGHT;
		}
		else if (PINB & (1 << PREV)) {
			doing = LEFT;
		}

		if (doing != NONE) {
			enable_pwm ();
			send_code ( code );
			disable_pwm ();
		}

	}


	return 0;
}
