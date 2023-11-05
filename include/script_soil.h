#pragma once
#include "dice.h"

typedef enum SoilState {
	IDLE,
	CLICKED,
	GROWING
} SoilState;

typedef struct SoilData {
	SoilState state;
	Dice* dice;
} SoilData;

/// <summary>
/// Creates a new soil data
/// </summary>
/// <returns></returns>
SoilData* script_soil_newdata();

/// <summary>
/// Frees a soil data for a script
/// </summary>
/// <param name="data"></param>
void script_soil_freedata(Script* script);
