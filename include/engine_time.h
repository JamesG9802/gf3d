#pragma once
#include <stdio.h>
#include <time.h>

#include "gfc_types.h"

/*
 * @brief Initializes the time engine so the time delta between each update can be recorded.
 */
void engine_time_init();

/*
 * @brief Calculates the current time delta based on the time based on last frame.
 */
void engine_time_renew();

/*
 * @brief Gets the current time delta.
 */
double engine_time_delta();