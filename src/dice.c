#include <SDL_image.h>

#include "gfc_list.h"

#include "dice.h"
#include "dicevalue.h"

#include "engine_utility.h"

#include "script_ui.h"

Dice* dice_new(Bool isSeed, int age, int sideCount, DiceValue* sideValues, double* sideWeights, int maxLifespan, int manacost) {
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
	if (!dice) return;
	
	if (dice->sideValues)
	{
		free(dice->sideValues);
	}
	if(dice->sideWeights)
	{
		free(dice->sideWeights);
	}
}

//	Dice to texture helper functions

SDL_Surface* dice_to_texture_six(Dice* dice, Entity* entity) {
	SDL_Surface* surface = IMG_Load("images/dice6.png");
	if (!surface || !dice->sideValues) return;

	SDL_Rect rect = gfc_sdl_rect(0, 0, 0, 0);

	/*	
	Side 0: 256, 0		Side 1: 0, 256		Side 2: 256, 256
	Side 3: 512, 256	Side 4: 256, 512	Side 5: 256, 768
	*/

	for (int i = 0; i < dice->sideCount; i++) {
		//	Render Symbol
		SDL_Rect rect;
		int x = 0, y = 0;
		SDL_Surface* side = dicevalue_getsurface(dice->sideValues[i]);
		if (!side) {
			SDL_FreeSurface(surface);
			return;
		}
		switch (i) {
		case 0: x = 256; y = 0; break;		case 1: x = 0; y = 256; break;		case 2: x = 256; y = 256; break;
		case 3: x = 512; y = 256; break;	case 4: x = 256; y = 512; break;	case 5: x = 256; y = 768; break;
		}
		rect = gfc_sdl_rect(x, y, 256, 256);
		SDL_BlitSurface(side, NULL, surface, &rect);
		SDL_free(side);
		side = NULL;
		//	Render Text
		char text[16];
		text[0] = '\0';
		sprintf_s(text, sizeof(char) * 16, "%d", dice->sideValues[i].value);
		Font* font = gf2d_font_get_by_tag(FT_H2)->font;
		
		TTF_SetFontSize(font, 64);
		side = TTF_RenderText_Blended(
			font, 
			text, 
			gfc_color_to_sdl(gfc_color(1, 1, 1, 1))
		);
		if (!side) continue;
		//	Center value
		x += (256 - side->w) / 2;
		y += (256 - side->h) / 2;
		rect = gfc_sdl_rect(x, y, 256, 256); 
		SDL_BlitSurface(side, NULL, surface, &rect);
		SDL_free(side);
	}
	return surface;
}

void dice_to_texture(Dice* dice, Entity* entity) {
	if (!dice) return;

	SDL_Surface* surface = NULL;
	
	switch (dice->sideCount) {
	default: 
		return;
	case 6:
		surface = dice_to_texture_six(dice, entity);
		break;
	}

	if (!surface) return;
	engine_utility_settexturebysurface(entity, surface);
}

void dice_to_ui(Dice* dice, Entity* entity) {
	if (!dice) return;

	SDL_Surface* surface = NULL;

	switch (dice->sideCount) {
	default:
		return;
	case 6:
		surface = dice_to_texture_six(dice, entity);
		break;
	}
	if (!surface) return;
	Sprite* sprite = gf2d_sprite_from_surface(surface, 0, 0, 0);
	if (entity->customData)
	{
		gf2d_sprite_free(((UIData*)entity->customData)->sprite);
		((UIData*)entity->customData)->sprite = sprite;
	}
}
