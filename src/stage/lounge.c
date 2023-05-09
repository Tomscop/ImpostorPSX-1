/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "lounge.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

//Lounge background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0; //back0
	Gfx_Tex tex_back1; //back1
	
	//fade stuff
	fixed_t fade, fadespd;
} Back_Lounge;

void Back_Lounge_DrawFG(StageBack *back)
{
	Back_Lounge *this = (Back_Lounge*)back;

	fixed_t fx, fy;

	//Draw lounge
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	if (stage.song_step == 408)
	{
		stage.reactor = FIXED_DEC(255,1);
		stage.reactorspd = FIXED_DEC(768,1);
	}
	
	//start fade
	if (stage.song_step == 392)
	{
		this->fade = FIXED_DEC(1,1);
		this->fadespd = FIXED_DEC(573,1);
	}
	if (stage.song_step == 481)
	{
		this->fade = FIXED_DEC(255,1);
		this->fadespd = FIXED_DEC(573,1);
	}

	//end fade
	if (stage.song_step == 397)
		this->fade = FIXED_DEC(255,1);
	if (stage.song_step == 486)
		this->fade = 0;

	if (this->fade > 0)
	{
		RECT flash = {0, 0, screen.SCREEN_WIDTH, screen.SCREEN_HEIGHT};
		u8 flash_col = this->fade >> FIXED_SHIFT;
		Gfx_BlendRect(&flash, flash_col, flash_col, flash_col, 2);
		if (stage.song_step <= 396)
			this->fade += FIXED_MUL(this->fadespd, timer_dt);
		else if ((stage.song_step >= 397) && (stage.song_step <= 480))
			this->fade = FIXED_DEC(255,1);
		else if (stage.song_step >= 481)
			this->fade -= FIXED_MUL(this->fadespd, timer_dt);
	}
	
	RECT black_src = {214, 55,  1,  1};
	RECT_FIXED black_dst = {
		0,
		0,
		screen.SCREEN_WIDTH,
		screen.SCREEN_HEIGHT
	};
	
	if (stage.song_step >= 409)
		Gfx_BlendTex(&this->tex_back1, &black_src, &black_dst, 1);
}

void Back_Lounge_DrawBG(StageBack *back)
{
	Back_Lounge *this = (Back_Lounge*)back;

	fixed_t fx, fy;

	//Draw lounge
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT back0_src = {  0,  0,255,255};
	RECT_FIXED back0_dst = {
		FIXED_DEC(0 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(255 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(255,1)
	};
	
	RECT back1_src = {  0,  0,255,255};
	RECT_FIXED back1_dst = {
		FIXED_DEC(254 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(255 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(255,1)
	};
	
	Debug_StageMoveDebug(&back0_dst, 5, fx, fy);
	Debug_StageMoveDebug(&back1_dst, 6, fx, fy);
	Stage_DrawTex(&this->tex_back0, &back0_src, &back0_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back1, &back1_src, &back1_dst, stage.camera.bzoom);
}

void Back_Lounge_Free(StageBack *back)
{
	Back_Lounge *this = (Back_Lounge*)back;
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_Lounge_New(void)
{
	//Allocate background structure
	Back_Lounge *this = (Back_Lounge*)Mem_Alloc(sizeof(Back_Lounge));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = Back_Lounge_DrawFG;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Lounge_DrawBG;
	this->back.free = Back_Lounge_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\BG2\\LOUNGE.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Mem_Free(arc_back);
	
	//Initialize fade
	this->fade = this->fadespd = 0;
	
	return (StageBack*)this;
}
