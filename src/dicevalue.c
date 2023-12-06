#include <SDL_image.h>

#include "simple_logger.h"

#include "dicevalue.h"

#include "script_player.h"
#include "script_monster.h"

DiceValue dicevalue_new(DiceValueType type, int value) {
	DiceValue dvalue;
	dvalue.type = type;
	dvalue.value = value;

	return dvalue;
}

DiceValueType dicevalue_get_type_from_string(char* type) {
	if (!type) return Mana;

	if (strcmp(type, "mana") == 0) {
		return Mana;
	}
	else if (strcmp(type, "fire") == 0) {
		return Fire;
	}
	else if (strcmp(type, "heart") == 0) {
		return Heart;
	}

	//	Unknown types default to mana
	return Mana;
}
void dicevalue_activate_effect(DiceValue dvalue) {
	PlayerData* playerData = script_player_getplayerdata();
	MonsterData* monsterData = script_monster_getbattlingmonsterdata();
	if (!playerData || !monsterData) return;
	if (dvalue.type == Mana) {
		slog("Mana");
		playerData->currentMana += dvalue.value;
	}
	else if (dvalue.type == Fire) {
		slog("Fire");
		monsterData->currentHealth -= dvalue.value;
	}
	else if (dvalue.type == Heart) {
		slog("Heart");
		playerData->currentHealth += dvalue.value;
	}
	slog("%d", dvalue.value);
	slog_sync();
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
	case Heart:
		return IMG_Load("images/dice_heart.png");
	}
}