#include <SDL_image.h>

#include "simple_logger.h"

#include "gfc_types.h"
#include "gfc_list.h"

#include "gf3d_vgraphics.h"

#include "dice.h"
#include "dicevalue.h"

#include "engine_utility.h"

#include "script_ui.h"
#include "script_player.h"
#include "script_monster.h"

Dice* dice_load(SJson* json) {
	if (!json) {
		slog("Dice json does not exist");
		return NULL;
	}
	Bool isSeed;
	int age, sideCount, maxLifespan, manaCost;
	DiceValue* sideValues;
	double* sideWeights;
	SJson* array;
	if (!sj_object_get_value_as_bool(json, "isSeed", &isSeed)) {
		slog("Something went wrong reading isSeed");
		goto fail;
	}
	if (!sj_object_get_value_as_int(json, "age", &age)) {
		slog("Something went wrong reading age");
		goto fail;
	}
	if (!sj_object_get_value_as_int(json, "sideCount", &sideCount)) {
		slog("Something went wrong reading sideCount");
		goto fail;
	}
	if (!sj_object_get_value_as_int(json, "maxLifespan", &maxLifespan)) {
		slog("Something went wrong reading maxLifespan");
		goto fail;
	}
	if (!sj_object_get_value_as_int(json, "manaCost", &manaCost)) {
		slog("Something went wrong reading manaCost");
		goto fail;
	}
	if (!(array = sj_object_get_value(json, "sideValues"))) {
		slog("Something went wrong reading sideValues");
		goto fail;
	}	
	if (sideCount != sj_array_get_count(array)) {
		slog("Side counts do not match number of side values");
		goto fail;
	}
	sideValues = malloc(sizeof(DiceValue) * sideCount);
	if (!sideValues) {
		slog("Couldn't allocate memory for side values");
		goto fail;
	}

	for (int i = 0; i < sj_array_get_count(array); i++) {
		DiceValueType type;
		int value;
		if (sj_is_object(sj_array_get_nth(array, i))) {
			if (!sj_object_get_value_as_string(sj_array_get_nth(array, i), "type")) {
				slog("Something went wrong reading the %dth side value's type", i);
				goto diceValueFail;
			}
			type = dicevalue_get_type_from_string(
				sj_object_get_value_as_string(sj_array_get_nth(array, i), 
				"type")
			);
			if (!sj_object_get_value_as_int(sj_array_get_nth(array, i), "value", &value)) {
				slog("Something went wrong reading the %dth side value's value", i);
				goto diceValueFail;
			}
			sideValues[i].type = type;
			sideValues[i].value = value;
		}
	}
	array = NULL;
	if (!(array = sj_object_get_value(json, "sideWeights"))) {
		slog("Something went wrong reading sideWeights");
		goto diceValueFail;
	}	
	if (sideCount != sj_array_get_count(array)) {
		slog("Side counts do not match number of side weights");
		goto diceValueFail;
	}
	sideWeights = malloc(sizeof(double) * sideCount);
	if (!sideWeights) {
		slog("Couldn't allocate memory for sideWeights");
		goto diceValueFail;
	}
	for (int i = 0; i < sj_array_get_count(array); i++) {
		float weight;
		if (!sj_get_float_value(sj_array_get_nth(array, i), &weight)) {
			slog("Something went wrong reading the %dth side weight", i);
			goto diceWeightFail;
		}
		sideWeights[i] = (double)weight;
	}
	return dice_new(isSeed, age, sideCount, sideValues, sideWeights, maxLifespan, manaCost);
diceWeightFail:
	free(sideWeights);
diceValueFail:
	free(sideValues);
fail:
	return NULL;
}

List* dice_list_load(char* path) {
	SJson* json = sj_load(path);
	SJson* array;
	if (!json) {
		slog("Couldn't load dice shop data.");
		return;
	}
	List* dices = gfc_list_new();
	if (!dices)
	{
		slog("Could not allocate memory for dice shop list.");
		goto jsonfail;
	}

	if (!(array = sj_object_get_value(json, "dices")) || !sj_is_array(array)) {
		slog("Dice shop data not properly formatted");
		goto listfail;
	}

	for (int i = 0; i < sj_array_get_count(array); i++) {
		Dice* dice = dice_load(sj_array_get_nth(array, i));
		if (!dice) {
			goto listfail;
		}
		gfc_list_append(dices, dice);
	}
	sj_free(json);
	return dices;
listfail:
	gfc_list_delete(dices);
jsonfail:
	sj_free(json);
	return NULL;
}
Dice* dice_new(Bool isSeed, int age, int sideCount, DiceValue* sideValues, double* sideWeights, int maxLifespan, int manaCost) {
	Dice* dice = malloc(sizeof(Dice));
	if (!dice) return NULL;

	dice->isSeed = isSeed;
	dice->age = age;
	dice->sideCount = sideCount;
	dice->sideValues = sideValues;
	dice->sideWeights = sideWeights;
	dice->maxLifespan = maxLifespan;
	dice->manaCost = manaCost;

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
	free(dice);
}
Dice* dice_seed_reward(int manacost) {
	int numSides = 0;
	float random = gfc_random();
	if (random > .5) {
		numSides = 4;
	}
	else {
		numSides = 6;
	}
	DiceValue* values = malloc(sizeof(DiceValue) * numSides);
	double* weights = malloc(sizeof(double) * numSides);

	for (int i = 0; i < numSides; i++) {
		random = gfc_random();
		if (random > .33) {
			values[i].type = Mana;
		}
		else if (random > .66) {
			values[i].type = Fire;
		}
		else {
			values[i].type = Heart;
		}
		values[i].value = (int)(.75 * manacost + 1 + gfc_random() * (7 + 1.1 * manacost));
		weights[i] = gfc_random() * .2 + 1;
	}
	int lifespan = (int)(gfc_random() * (6 + 0.5 * manacost) + 2 + .3 * manacost);
	return dice_new(true, 0, numSides, values, weights, lifespan, manacost);
}

void dice_harvest(Dice* dice) {
	if (!dice) return;
	dice->age;
	dice->isSeed = false;
	for (int i = 0; i < dice->sideCount; i++) {
		if (gfc_random() > .5)
		{
			dice->sideValues[i].value = dice->sideValues[i].value + 1;
			dice->manaCost = dice->manaCost + 1;
		}
		else if (gfc_random() > .5)
		{
			if (dice->sideValues[i].value > 1 && dice->manaCost > 0)
			{
				dice->sideValues[i].value = dice->sideValues[i].value - 1;
				dice->manaCost = dice->manaCost - 1;
			}
		}
		if (gfc_random() > .5)
		{
			if (gfc_random() > .5)
				dice->sideValues[i].type = Mana;
			else if (gfc_random() > .5)
				dice->sideValues[i].type = Fire;
			else
				dice->sideValues[i].type = Heart;
		}
	}
	for (int i = 0; i < dice->sideCount; i++) {
		if (gfc_random() > .5)
			dice->sideValues[i].value = dice->sideValues[i].value + .1;
		else if (gfc_random() > .5 && dice->sideValues[i].value > .1)
			dice->sideValues[i].value = dice->sideValues[i].value - .1;
	}
	if (gfc_random() > .5) {
		if(dice->maxLifespan > 0)
			dice->maxLifespan = dice->maxLifespan - 1;
	}
	else {
		dice->maxLifespan = dice->maxLifespan + 1;
	}
}

void dice_activate_effect(Dice* dice) {
	Entity* battlingmonster = script_monster_getbattlingmonster(); 
	if (dice->manaCost > script_player_getplayerdata()->currentMana) {
		script_player_getplayerdata()->currentMana = script_player_getplayerdata()->currentMana + 5;
		return;
	}
	script_player_getplayerdata()->currentMana = script_player_getplayerdata()->currentMana - dice->manaCost;
	
	float sumweights = 0;
	for (int i = 0; i < dice->sideCount; i++) {
		sumweights = sumweights + dice->sideWeights[i];
	}
	float random = gfc_random() * sumweights;
	sumweights = 0;
	for (int i = 0; i < dice->sideCount; i++) {
		sumweights = sumweights + dice->sideWeights[i];
		if (random < sumweights)
		{
			dicevalue_activate_effect(dice->sideValues[i]);
			return;
		}
	}
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