#include "gfc_list.h"

#include "dice.h"
#include "dicevalue.h"


Dice* dice_new(Bool isSeed, int age, int sideCount, List* sideValues, List* sideWeights, int maxLifespan, int manacost) {
	Dice* dice = malloc(sizeof(Dice));
	if (!dice) return NULL;

	dice->isSeed = isSeed;
	dice->age = age;
	dice->sideCount = sideCount;
	dice->sideValues = sideValues;
	dice->sideWeights = sideWeights;
	dice->maxLifespan = maxLifespan;
	dice->manaCost = manacost;

	return dice;
}
void dice_free(Dice* dice) {
	
}
