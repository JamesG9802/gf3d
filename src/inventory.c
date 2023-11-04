#include "simple_logger.h"

#include "gfc_list.h"

#include "inventory.h"


Inventory* inventory_new() {
	Inventory* inventory = calloc(1, sizeof(Inventory));
	if (!inventory) return NULL;
	inventory->diceSeeds = gfc_list_new();
	inventory->diceInventory = gfc_list_new();
	inventory->diceLoadout = gfc_list_new();
	return inventory;
}

void inventory_free(Inventory* inventory) {
	if (!inventory) return;
	if (inventory->diceSeeds) {
		for (int i = 0; i < gfc_list_get_count(inventory->diceSeeds); i++)
		{
			dice_free(gfc_list_get_nth(inventory->diceSeeds, i));
		}
		gfc_list_delete(inventory->diceSeeds);
	}
	if (inventory->diceInventory) {
		for (int i = 0; i < gfc_list_get_count(inventory->diceInventory); i++)
		{
			dice_free(gfc_list_get_nth(inventory->diceInventory, i));
		}
		gfc_list_delete(inventory->diceInventory);
	}
	if (inventory->diceLoadout) {
		for (int i = 0; i < gfc_list_get_count(inventory->diceLoadout); i++)
		{
			dice_free(gfc_list_get_nth(inventory->diceLoadout, i));
		}
		gfc_list_delete(inventory->diceLoadout);
	}
	free(inventory);
}