#pragma once
#include "gfc_types.h";

typedef enum AnchorMode {
	TOPLEFT,
	TOPMIDDLE,
	CENTER,
	MIDDLERIGHT
} AnchorMode;

typedef struct UIData {
	Sprite* sprite;
	Color color;

	/// <summary>
	/// The anchor mode of the ui element.
	/// </summary>
	AnchorMode mode;
	
	/// <summary>
	/// Controls whether the ui element is interactable
	/// </summary>
	Bool isInteractable;

	/// <summary>
	/// Event to be fired when an interactable ui element is clicked.
	/// </summary>
	TextLine associatedEvent;
	
} UIData;


/// <summary>
/// Returns true if the mouse over the UI.
/// </summary>
/// <returns></returns>
Bool script_ui_ismouseover(Entity* self, Script* script);

/// <summary>
/// Sets the color of the sprite.
/// </summary>
/// <param name="self"></param>
/// <param name="script"></param>
/// <param name="c"></param>
void script_ui_setcolor(Entity* self, Script* script, Color color);

/// <summary>
/// Sets the position of the sprite in ndc mode.
/// </summary>
/// <param name="self"></param>
/// <param name="script"></param>
void script_ui_setpositionndc(Entity* self, Script* script, Vector2D position);

/// <summary>
/// Sets the anchor mode of the sprite
/// </summary>
/// <param name="self"></param>
/// <param name="script"></param>
/// <param name="mode"></param>
void script_ui_setanchormode(Entity* self, Script* script, AnchorMode mode);
