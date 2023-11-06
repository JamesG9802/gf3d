#include <stdio.h>
#include <SDL_image.h>

#include "simple_logger.h"

#include "gfc_types.h"
#include "gfc_list.h"
#include "gfc_input.h"

#include "entity.h"
#include "script.h"
#include "dice.h"
#include "dicevalue.h"

#include "engine_time.h"

#include "script_defs.h"
#include "script_manager.h"
#include "script_dice.h"

/**
 * @brief Called when a script is created.
 */
static void Start(Entity* self, Script* script) {
    {
        Dice* dice;
        DiceValue* diceValues = malloc(sizeof(DiceValue) * 4);
        diceValues[0] = dicevalue_new(Mana, 1);
        diceValues[1] = dicevalue_new(Mana, 2);
        diceValues[2] = dicevalue_new(Mana, 3);
        diceValues[3] = dicevalue_new(Mana, 4);
        double* sideWeights = malloc(sizeof(double) * 4);
        sideWeights[0] = 1;
        sideWeights[1] = 1;
        sideWeights[2] = 1;
        sideWeights[3] = 1;
        dice = dice_new(true, 0, 4, diceValues, sideWeights, 10, 0);
        dice_to_texture(dice, self);
        dice_free(dice);
    }
}
/**
 * @brief Called when a script is created.
 */
static void Think(Entity* self, Script* script) {
    self->rotation.x += 2 * GFC_2PI * engine_time_delta();
    self->rotation.z += 2 * GFC_2PI * engine_time_delta();
}
/**
 * @brief Called when a script is created.
 */
static void Update(Entity* self, Script* script) {
}
/**
 * @brief Called when a script is created.
 */
static void Destroy(Entity* self, Script* script) {
}
static void Arguments(Entity* self, Script* script, SJson* json) {
}
Script* script_new_dice() {
	return script_new("dice", &Start, &Think, &Update, &Destroy, &Arguments);
}


/*eol@eof*/
