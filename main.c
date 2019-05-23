#include <avr/io.h>
#include <stdint.h>

#define NEXT 			PB2
#define PREV 			PB3

typedef enum {NONE, RIGHT, LEFT} STATUS;

void enable_pwm () {

}

void disable_pwm () {

}

void send_code (uint8_t code) {

}

int main (void) {

	/* init Timer0 for PWM on OC0A */
	/* of the 38khz carrier for IR NEC */


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
