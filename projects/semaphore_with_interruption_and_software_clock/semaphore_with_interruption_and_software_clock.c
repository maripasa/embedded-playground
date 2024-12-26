#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

#define LED_PIN_RED 13
#define LED_PIN_GREEN 11 
#define BUTTON_PIN1 5
#define BUTTON_PIN2 6

enum Color {
	RED,
	YELLOW,
	GREEN,
	NUM_COLORS
};

volatile bool automatic_mode = false;

volatile enum Color current_color = RED;

volatile absolute_time_t last_press_time_button1 = 0;
volatile absolute_time_t last_press_time_button2 = 0;

void handle_color(enum Color color);
void gpio_irq_handler(uint gpio, uint32_t events);
void toggle_automatic_mode();

int main() {
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

	gpio_set_irq_enabled_with_callback(BUTTON_PIN1, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
	gpio_set_irq_enabled_with_callback(BUTTON_PIN2, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

	handle_color(current_color);

	while (true) {
		if (automatic_mode) {
			sleep_ms(1000);
			current_color = (current_color + 1) % NUM_COLORS;
			handle_color(current_color);
      continue;
		}
    sleep_ms(1);
	}
}

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

void gpio_irq_handler(uint gpio, uint32_t events) {
	absolute_time_t current_time = get_absolute_time();

	if (gpio == BUTTON_PIN1) {
		if (absolute_time_diff_us(last_press_time_button2, current_time) < 100000) {
			automatic_mode = !automatic_mode;
		}

		if ((!automatic_mode) && (absolute_time_diff_us(last_press_time_button1, current_time) > 200000)) {
			current_color = (current_color + 1) % NUM_COLORS;
			handle_color(current_color);
		}

    last_press_time_button1 = current_time;
	}

	if (gpio == BUTTON_PIN2) {
		if (absolute_time_diff_us(last_press_time_button1, current_time) < 100000) {
			automatic_mode = !automatic_mode;
		}

		if ((!automatic_mode) && (absolute_time_diff_us(last_press_time_button2, current_time) > 200000)) {
			current_color = (current_color - 1 + NUM_COLORS) % NUM_COLORS;
			handle_color(current_color);
		}

    last_press_time_button2 = current_time;

	}
}
