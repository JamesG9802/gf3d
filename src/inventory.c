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
	if (inventory->diceSeeds) gfc_list_delete(inventory->diceSeeds);
	if (inventory->diceInventory) gfc_list_delete(inventory->diceInventory);
	if (inventory->diceLoadout) gfc_list_delete(inventory->diceLoadout);
	free(inventory);
}