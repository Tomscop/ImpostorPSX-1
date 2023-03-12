/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "cargo.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

//Cargo background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0; //back0
	Gfx_Tex tex_back1; //back1
	Gfx_Tex tex_back3; //back3

} Back_Cargo;


void Back_Cargo_DrawBG(StageBack *back)
{
	Back_Cargo *this = (Back_Cargo*)back;

	fixed_t fx, fy;

	//Draw cargo
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT back0_src = {  0,  0,255,222};
	RECT_FIXED back0_dst = {
		FIXED_DEC(0 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(270 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(235,1)
	};
	
	RECT back1_src = {  0,  0,255,222};
	RECT_FIXED back1_dst = {
		FIXED_DEC(268 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(270 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(235,1)
	};
	
	RECT back3_src = {  0,  0,255, 81};
	RECT_FIXED back3_dst = {
		FIXED_DEC(18 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(56,1) - fy,
		FIXED_DEC(477 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(151,1)
	};
	
	Debug_StageMoveDebug(&back0_dst, 6, fx, fy);
	Debug_StageMoveDebug(&back1_dst, 7, fx, fy);
	Debug_StageMoveDebug(&back3_dst, 9, fx, fy);
	if (stage.song_step <= 3407)
	{
		Stage_DrawTex(&this->tex_back0, &back0_src, &back0_dst, stage.camera.bzoom);
		Stage_DrawTex(&this->tex_back1, &back1_src, &back1_dst, stage.camera.bzoom);
	}
	if (stage.song_step >= 3408)
		Stage_DrawTex(&this->tex_back3, &back3_src, &back3_dst, stage.camera.bzoom);
	
	//Draw black
	RECT screen_src = {0, 0, screen.SCREEN_WIDTH, screen.SCREEN_HEIGHT};
	if (stage.song_step >= 3408)
		Gfx_DrawRect(&screen_src, 0, 0, 0);
}

void Back_Cargo_Free(StageBack *back)
{
	Back_Cargo *this = (Back_Cargo*)back;
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_Cargo_New(void)
{
	//Allocate background structure
	Back_Cargo *this = (Back_Cargo*)Mem_Alloc(sizeof(Back_Cargo));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Cargo_DrawBG;
	this->back.free = Back_Cargo_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\BG\\CARGO.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_back3, Archive_Find(arc_back, "back3.tim"), 0);
	Mem_Free(arc_back);
	
	return (StageBack*)this;
}
