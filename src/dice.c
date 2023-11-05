#include <SDL_image.h>

#include "simple_logger.h"

#include "gfc_list.h"

#include "gf3d_vgraphics.h"

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

void dice_harvest(Dice* dice) {
	if (!dice) return;
	dice->isSeed = false;
	
}

//	Dice to texture helper functions
SDL_Surface* dice_to_texture_four(Dice* dice, Entity* entity) {
	SDL_Surface* surface = IMG_Load("images/dice4.png");
	if (!surface || !dice->sideValues) return NULL;
	SDL_Rect rect = gfc_sdl_rect(0, 0, 0, 0);

	for (int i = 0; i < dice->sideCount; i++) {
		//	Render Symbol
		SDL_Rect rect;
		int x = 0, y = 0;
		SDL_Surface* side = dicevalue_getsurface(dice->sideValues[i]);
		if (!side) {
			SDL_FreeSurface(surface);
			return NULL;
		}
		switch (i) {
		case 0: x = 128; y = 256; break;		case 1: x = 387; y = 0; break;		case 2: x = 518; y = 512; break;
		case 3: x = 777; y = 256; break;
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
SDL_Surface* dice_to_texture_six(Dice* dice, Entity* entity) {
	SDL_Surface* surface = IMG_Load("images/dice6.png");
	if (!surface || !dice->sideValues) return NULL;

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
			return NULL;
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
	case 4:
		surface = dice_to_texture_four(dice, entity);
		break;
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
	case 4:
		surface = dice_to_texture_four(dice, entity);
		break;
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


void dice_to_ui_simplified(List* dices, int diceIndex, int selectedDiceIndex, Entity* entity) {
	if (!dices) return;
	int tileWidth = 640;
	int tileHeight = 256;

	//	creating a bigger surface to store both icon and text four times, two on each row.
	SDL_Surface* combined = gf3d_vgraphics_create_surface(tileWidth * 2 + 40, tileHeight * 2 + 40);
	int count = 0;
	for (int i = diceIndex; i < gfc_list_get_count(dices) && count < 4; i++)
	{
		Dice* dice = gfc_list_get_nth(dices, i);
		SDL_Surface* information = NULL;
		SDL_Surface* surface = NULL;
		switch (dice->sideCount) {
		default:
			continue;
		case 4:
			surface = IMG_Load("images/dice4_icon.png");
			break;
		case 6:
			surface = IMG_Load("images/dice6_icon.png");
			break;
		}
		if (!surface) continue;;
		char text[512];
		text[0] = '\0';
		if (dice->isSeed)
			sprintf(text, "%d Sided Dice\nLifespan: %d\nMana Cost: %d", dice->sideCount, dice->maxLifespan, dice->manaCost);
		else
			sprintf(text, "%d Sided Dice\nAge: %d/%d\nMana Cost: %d", dice->sideCount, dice->age, dice->maxLifespan, dice->manaCost);
		TTF_SetFontSize(gf2d_font_get_by_tag(FT_H2)->font, 64);
		information = TTF_RenderText_Blended_Wrapped(
			gf2d_font_get_by_tag(FT_H2)->font,
			text,
			gfc_color_to_sdl(gfc_color(0, 0, 0, 1)),
			384
		);

		if (!information) {
			SDL_FreeSurface(surface);
			continue;
		}
		int xOffset, yOffset;
		switch (count) {
		default:
		case 0: //	first one top left corner
			xOffset = 0;
			yOffset = 0;
			break;
		case 1: //	second one top right corner
			xOffset = tileWidth + 40;
			yOffset = 0;
			break;
		case 2: //	third one bottom left corner
			xOffset = 0;
			yOffset = tileHeight + 40;
			break;
		case 3: //	fourth one bottom right corner
			xOffset = tileWidth + 40;
			yOffset = tileHeight + 40;
			break;

		}
		if (selectedDiceIndex == i)
			SDL_SetSurfaceColorMod(surface, 255, 255, 128);
		SDL_Rect iconDestination = gfc_sdl_rect(xOffset, yOffset, 256, 256);
		SDL_Rect informationDestination = gfc_sdl_rect(xOffset + 256, yOffset, 256, 256);
		SDL_BlitSurface(surface, NULL, combined, &iconDestination);
		SDL_BlitSurface(information, NULL, combined, &informationDestination);
		SDL_FreeSurface(surface);
		SDL_FreeSurface(information);
		count++;
	}
	Sprite* sprite = gf2d_sprite_from_surface(combined, 0, 0, 0);
	if (entity->customData)
	{
		gf2d_sprite_free(((UIData*)entity->customData)->sprite);
		((UIData*)entity->customData)->sprite = sprite;
	}
}