#pragma once

#include "gfc_types.h"
#include "gf2d_sprite.h"

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
	
	/// <summary>
	/// The index of the frames that should be rendered
	/// </summary>
	Uint32 currentFrame;
} UIData;

/// <summary>
/// Creates a UIData struct.
/// </summary>
/// <returns></returns>
UIData script_ui_newuidata();

/// <summary>
/// Returns true if the mouse over the UI.
/// </summary>
/// <returns></returns>
Bool script_ui_ismouseover(Entity* self);

/// <summary>
/// Sets the color of the sprite.
/// </summary>
/// <param name="self"></param>
/// <param name="script"></param>
/// <param name="c"></param>
void script_ui_setcolor(Entity* self, Color color);

/// <summary>
/// Sets the position of the sprite in ndc mode.
/// </summary>
/// <param name="self"></param>
/// <param name="script"></param>
void script_ui_setpositionndc(Entity* self, Vector2D position);

/// <summary>
/// Sets the anchor mode of the sprite
/// </summary>
/// <param name="self"></param>
/// <param name="script"></param>
/// <param name="mode"></param>
void script_ui_setanchormode(Entity* self, AnchorMode mode);

/// <summary>
/// Sets the visibility of the sprite
/// </summary>
/// <param name="self"></param>
/// <param name="script"></param>
/// <param name="hidden"></param>
void script_ui_sethidden(Entity* self, Bool hidden);

/// <summary>
/// Sets the frame number of the entity.
/// </summary>
/// <param name="self"></param>
/// <param name="frameNum"></param>
void script_ui_setframenum(Entity* self, Uint32 frameNum);
