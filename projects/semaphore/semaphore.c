// it actually needs the debounce, the teacher was right, what a surprise

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

#define LED_PIN_RED 13
#define LED_PIN_GREEN 11 
#define NUM_COLORS 3

enum Color {
	RED,
	YELLOW,
	GREEN,
};

void handle_color(enum Color color) {
	switch (color) {
		case RED:
			gpio_put(LED_PIN_RED, 1);
			gpio_put(LED_PIN_GREEN, 0);
			break;
		case YELLOW:
			gpio_put(LED_PIN_RED, 1);
			gpio_put(LED_PIN_GREEN, 1);
			break;
		case GREEN:
			gpio_put(LED_PIN_RED, 0);
			gpio_put(LED_PIN_GREEN, 1);
			break;
	}
}

int main() {
	const uint BUTTON_PIN1 = 5;
	const uint BUTTON_PIN2 = 6;

	stdio_init_all();

	gpio_init(LED_PIN_RED);
	gpio_init(LED_PIN_GREEN);

	gpio_set_dir(LED_PIN_RED, GPIO_OUT);
	gpio_set_dir(LED_PIN_GREEN, GPIO_OUT);

	gpio_set_slew_rate(LED_PIN_RED, GPIO_SLEW_RATE_SLOW);
	gpio_set_slew_rate(LED_PIN_GREEN, GPIO_SLEW_RATE_SLOW);

	gpio_init(BUTTON_PIN1);
	gpio_init(BUTTON_PIN2);

	gpio_set_dir(BUTTON_PIN1, GPIO_IN);
	gpio_set_dir(BUTTON_PIN2, GPIO_IN);

	gpio_pull_up(BUTTON_PIN1);
	gpio_pull_up(BUTTON_PIN2);
	
	enum Color color = RED;
	handle_color(color);

	bool button1_pressed = false;
	bool button2_pressed = false;
	bool both_pressed = false;
	int automatic_mode = -1;

	while (true) {
		if ((gpio_get(BUTTON_PIN1) == 0 && gpio_get(BUTTON_PIN2) == 0) && !both_pressed) {
			automatic_mode = -automatic_mode;
			both_pressed = true;
			sleep_ms(200);  // debounce
		}

		if (gpio_get(BUTTON_PIN1) != 0 || gpio_get(BUTTON_PIN2) != 0) {
			both_pressed = false;
		}

		if (automatic_mode > 0) {
			color = (color - 1 + NUM_COLORS) % NUM_COLORS;
			handle_color(color);
			sleep_ms(1000);
			continue;
		}

		if (gpio_get(BUTTON_PIN1) == 0 && !button1_pressed) {
			color = (color + 1) % NUM_COLORS;
			handle_color(color);
			button1_pressed = true;
			sleep_ms(200);  // debounce
		} else if (gpio_get(BUTTON_PIN1) != 0) {
			button1_pressed = false;
		}

		if (gpio_get(BUTTON_PIN2) == 0 && !button2_pressed) {
			color = (color - 1 + NUM_COLORS) % NUM_COLORS;
			handle_color(color);
			button2_pressed = true;
			sleep_ms(200);  // debounce
		} else if (gpio_get(BUTTON_PIN2) != 0) {
			button2_pressed = false;
		}
	}

	return 0;
}
