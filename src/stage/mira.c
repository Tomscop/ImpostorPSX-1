/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "mira.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

//Mira background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0; //back0
	Gfx_Tex tex_back1; //back1
	Gfx_Tex tex_back2; //back2
	
	//fade stuff
	fixed_t fade, fadespd;
	
} Back_Mira;

void Back_Mira_DrawFG(StageBack *back)
{
	Back_Mira *this = (Back_Mira*)back;
	
	//start fade
	if (stage.lights == 2)
	{
		this->fade = FIXED_DEC(255,1);
		this->fadespd = FIXED_DEC(768,1);
		stage.lights = 0;
	}
	
	//end fade
	if (this->fade == 0)
		this->fade = 0;
	
	if (this->fade > 0)
	{
		RECT flash = {0, 0, screen.SCREEN_WIDTH, screen.SCREEN_HEIGHT};
		u8 flash_col = this->fade >> FIXED_SHIFT;
		Gfx_BlendRect(&flash, flash_col, flash_col, flash_col, 2);
		this->fade -= FIXED_MUL(this->fadespd, timer_dt);
	}
}

void Back_Mira_DrawBG(StageBack *back)
{
	Back_Mira *this = (Back_Mira*)back;

	fixed_t fx, fy;

	//Draw mira
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT back0_src = {  0,  0,255,201};
	RECT_FIXED back0_dst = {
		FIXED_DEC(0 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(255 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(201,1)
	};
	
	RECT back1_src = {  0,  0,208,201};
	RECT_FIXED back1_dst = {
		FIXED_DEC(254 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(208 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(201,1)
	};
	
	RECT bench0_src = {  0,  0,161, 77};
	RECT_FIXED bench0_dst = {
		FIXED_DEC(0 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(76,1) - fy,
		FIXED_DEC(161 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(77,1)
	};
	
	RECT bench1_src = {  0, 78,188, 78};
	RECT_FIXED bench1_dst = {
		FIXED_DEC(204 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(74,1) - fy,
		FIXED_DEC(188 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(78,1)
	};
	
	//Draw black
	RECT screen_src = {0, 0, screen.SCREEN_WIDTH, screen.SCREEN_HEIGHT};
	if ((stage.lights == 1) && (stage.song_step <= 1599))
		Gfx_DrawRect(&screen_src, 0, 0, 0);
	
	Debug_StageMoveDebug(&back0_dst, 5, fx, fy);
	Debug_StageMoveDebug(&back1_dst, 6, fx, fy);
	Debug_StageMoveDebug(&bench0_dst, 7, fx, fy);
	Debug_StageMoveDebug(&bench1_dst, 8, fx, fy);
	Stage_DrawTex(&this->tex_back2, &bench0_src, &bench0_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back2, &bench1_src, &bench1_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back0, &back0_src, &back0_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back1, &back1_src, &back1_dst, stage.camera.bzoom);
}

void Back_Mira_Free(StageBack *back)
{
	Back_Mira *this = (Back_Mira*)back;
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_Mira_New(void)
{
	//Allocate background structure
	Back_Mira *this = (Back_Mira*)Mem_Alloc(sizeof(Back_Mira));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = Back_Mira_DrawFG;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Mira_DrawBG;
	this->back.free = Back_Mira_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\BG\\MIRA.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_back2, Archive_Find(arc_back, "back2.tim"), 0);
	Mem_Free(arc_back);
	
	return (StageBack*)this;
}
