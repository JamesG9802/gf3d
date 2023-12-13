#pragma once
#include "dice.h"
#include "gfc_audio.h"

typedef enum SoilState {
	IDLE,
	CLICKED,
	GROWING,
	READY
} SoilState;

typedef struct SoilData {
	SoilState state;
	int daysRemaining;	//	days remaining before plant can be harvested
	Dice* dice;
	Sound* soundPlant;		//	sound effect for planting
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
