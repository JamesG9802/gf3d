#include <SDL_image.h>
#include "dicevalue.h"

DiceValue dicevalue_new(DiceValueType type, int value) {
	DiceValue dvalue;
	dvalue.type = type;
	dvalue.value = value;

	return dvalue;
}

SDL_Surface* dicevalue_getsurface(DiceValue dvalue) {
	switch (dvalue.type)
	{
	default:
		return IMG_Load("images/default.png");
	case Fire:
		return IMG_Load("images/dice_fire.png");
	case Mana:
		return IMG_Load("images/dice_mana.png");
	}
}