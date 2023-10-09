#include <time.h>
#include "engine_time.h"

static clock_t current;
static double timeDelta = 0.0;
/*
 * @brief Initializes the time engine so the time delta between each update can be recorded.
 */
void engine_time_init() {
	current = clock();
}

/*
 * @brief Calculates the current time delta based on the time based on last frame.
 */
void engine_time_renew() {
	timeDelta = (double)(clock() - current) / (double)CLOCKS_PER_SEC;
	current = clock();
}

/*
 * @brief Gets the current time delta.
 */
double engine_time_delta() {
	return timeDelta;
}