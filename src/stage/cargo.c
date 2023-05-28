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
	Gfx_Tex tex_back2; //back3
	Gfx_Tex tex_back3; //back3
	
	//fade stuff
	fixed_t fade, fadespd;
	fixed_t fade2, fadespd2;
	fixed_t fade3, fadespd3;
	fixed_t fade4, fadespd4;

} Back_Cargo;

void Back_Cargo_DrawFG(StageBack *back)
{
	Back_Cargo *this = (Back_Cargo*)back;
	
	if (stage.song_step == 3920)
	{
		stage.reactor = FIXED_DEC(255,1);
		stage.reactorspd = FIXED_DEC(106,1);
	}
	
	RECT screen_src = {0, 0, screen.SCREEN_WIDTH, screen.SCREEN_HEIGHT};
	if (stage.song_step >= 3920)
		Gfx_DrawRect(&screen_src, 0, 0, 0);
	
	//start fade
	if (stage.song_step == 0)
	{
		this->fade = FIXED_DEC(255,1);
		this->fadespd = FIXED_DEC(205,1);
	}
	if (stage.song_step == 1424)
	{
		this->fade = FIXED_DEC(255,1);
		this->fadespd = FIXED_DEC(717,1);
	}
	if (stage.song_step == 3408)
	{
		this->fade = FIXED_DEC(255,1);
		this->fadespd = FIXED_DEC(171,1);
	}

	//end fade
	if ((stage.song_step == 15) || (stage.song_step == 1432) || (stage.song_step == 3452))
		this->fade = 0;

	if (this->fade > 0)
	{
		RECT flash = {0, 0, screen.SCREEN_WIDTH, screen.SCREEN_HEIGHT};
		u8 flash_col = this->fade >> FIXED_SHIFT;
		Gfx_BlendRect(&flash, flash_col, flash_col, flash_col, 2);
		if (stage.paused == false)
			this->fade -= FIXED_MUL(this->fadespd, timer_dt);
	}

	//start fade2
	if (stage.song_step == 3392)
	{
		this->fade2 = FIXED_DEC(1,1);
		this->fadespd2 = FIXED_DEC(246,1);
	}

	//end fade2
	if (stage.song_step == 3408)
		this->fade2 = 0;

	if (this->fade2 > 0)
	{
		RECT flash = {0, 0, screen.SCREEN_WIDTH, screen.SCREEN_HEIGHT};
		u8 flash_col = this->fade2 >> FIXED_SHIFT;
		Gfx_BlendRect(&flash, flash_col, flash_col, flash_col, 2);
		if (stage.paused == false)
			this->fade2 += FIXED_MUL(this->fadespd2, timer_dt);
	}
}
void Back_Cargo_DrawMG(StageBack *back)
{
	Back_Cargo *this = (Back_Cargo*)back;

	fixed_t fx, fy;
	
	//start fade3
	if (stage.song_step == 1424)
	{
		this->fade3 = FIXED_DEC(1,1);
		this->fadespd3 = FIXED_DEC(67,1);
	}

	//end fade3
	if (stage.song_step == 1458)
		this->fade3 = 0;

	if (this->fade3 > 0)
	{
		RECT flash = {0, 0, screen.SCREEN_WIDTH, screen.SCREEN_HEIGHT};
		u8 flash_col = this->fade3 >> FIXED_SHIFT;
		Gfx_BlendRect(&flash, flash_col, flash_col, flash_col, 2);
		if (stage.paused == false)
			this->fade3 += FIXED_MUL(this->fadespd3, timer_dt);
	}
	
	RECT screen_src = {0, 0, screen.SCREEN_WIDTH, screen.SCREEN_HEIGHT};
	if ((stage.song_step >= 1458) && (stage.song_step <= 1557))
		Gfx_DrawRect(&screen_src, 0, 0, 0);
	
	//start fade4
	if (stage.song_step == 1558)
	{
		this->fade4 = FIXED_DEC(255,1);
		this->fadespd4 = FIXED_DEC(25,1);
	}

	//end fade4
	if (stage.song_step == 1680)
		this->fade4 = 0;

	if (this->fade4 > 0)
	{
		RECT flash = {0, 0, screen.SCREEN_WIDTH, screen.SCREEN_HEIGHT};
		u8 flash_col = this->fade4 >> FIXED_SHIFT;
		Gfx_BlendRect(&flash, flash_col, flash_col, flash_col, 2);
		if (stage.paused == false)
			this->fade4 -= FIXED_MUL(this->fadespd4, timer_dt);
	}
	
	//Draw cargo
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT back2_src = {  0,  0,233,173};
	RECT_FIXED back2_dst = {
		FIXED_DEC(271 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(61,1) - fy,
		FIXED_DEC(233 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(173,1)
	};
	
	Debug_StageMoveDebug(&back2_dst, 8, fx, fy);
	if ((stage.song_step >= 1552) && (stage.song_step <= 1679))
		Stage_DrawTex(&this->tex_back2, &back2_src, &back2_dst, stage.camera.bzoom);
}

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
	
	RECT screen_src = {0, 0, screen.SCREEN_WIDTH, screen.SCREEN_HEIGHT};
	
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
	this->back.draw_fg = Back_Cargo_DrawFG;
	this->back.draw_md = Back_Cargo_DrawMG;
	this->back.draw_bg = Back_Cargo_DrawBG;
	this->back.free = Back_Cargo_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\BG\\CARGO.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_back2, Archive_Find(arc_back, "back2.tim"), 0);
	Gfx_LoadTex(&this->tex_back3, Archive_Find(arc_back, "back3.tim"), 0);
	Mem_Free(arc_back);
	
	//Initialize Fade
	this->fade = FIXED_DEC(255,1);
	this->fadespd = 0;
	this->fade2 = this->fadespd2 = 0;
	this->fade3 = this->fadespd3 = 0;
	this->fade4 = this->fadespd4 = 0;
	
	return (StageBack*)this;
}
