/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "polusmaroon.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

int snowx1, snowy1, snowx2, snowy2, snowx3, snowy3, snowx4, snowy4, snowx5, snowy5, snowx6, snowy6, snowx7, snowy7, snowx8, snowy8, snowx9, snowy9, snowx10, snowy10;

//PolusMaroon Maroon background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0; //back0
	Gfx_Tex tex_back1; //back1
	
} Back_PolusMaroon;

void Back_PolusMaroon_DrawFG(StageBack *back)
{
	Back_PolusMaroon *this = (Back_PolusMaroon*)back;
	
	if (stage.paused == false)
	{
		snowx1 = RandomRange(0,320);
		snowy1 = RandomRange(0,240);
		snowx2 = RandomRange(0,320);
		snowy2 = RandomRange(0,240);
		snowx3 = RandomRange(0,320);
		snowy3 = RandomRange(0,240);
		snowx4 = RandomRange(0,320);
		snowy4 = RandomRange(0,240);
		snowx5 = RandomRange(0,320);
		snowy5 = RandomRange(0,240);
		snowx6 = RandomRange(0,320);
		snowy6 = RandomRange(0,240);
		snowx7 = RandomRange(0,320);
		snowy7 = RandomRange(0,240);
		snowx8 = RandomRange(0,320);
		snowy8 = RandomRange(0,240);
		snowx9 = RandomRange(0,320);
		snowy9 = RandomRange(0,240);
		snowx10 = RandomRange(0,320);
		snowy10 = RandomRange(0,240);
	}
	RECT snow1_src = {snowx1, snowy1, 4, 4};
	Gfx_DrawRect(&snow1_src, 255, 255, 255);
	RECT snow2_src = {snowx2, snowy2, 4, 4};
	Gfx_DrawRect(&snow2_src, 255, 255, 255);
	RECT snow3_src = {snowx3, snowy3, 4, 4};
	Gfx_DrawRect(&snow3_src, 255, 255, 255);
	RECT snow4_src = {snowx4, snowy4, 4, 4};
	Gfx_DrawRect(&snow4_src, 255, 255, 255);
	RECT snow5_src = {snowx5, snowy5, 4, 4};
	Gfx_DrawRect(&snow5_src, 255, 255, 255);
	RECT snow6_src = {snowx6, snowy6, 4, 4};
	Gfx_DrawRect(&snow6_src, 255, 255, 255);
	RECT snow7_src = {snowx7, snowy7, 4, 4};
	Gfx_DrawRect(&snow7_src, 255, 255, 255);
	RECT snow8_src = {snowx8, snowy8, 4, 4};
	Gfx_DrawRect(&snow8_src, 255, 255, 255);
	RECT snow9_src = {snowx9, snowy9, 4, 4};
	Gfx_DrawRect(&snow9_src, 255, 255, 255);
	RECT snow10_src = {snowx10, snowy10, 4, 4};
	Gfx_DrawRect(&snow10_src, 255, 255, 255);
}

void Back_PolusMaroon_DrawBG(StageBack *back)
{
	Back_PolusMaroon *this = (Back_PolusMaroon*)back;

	fixed_t fx, fy;

	//Draw polus maroon
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT back0_src = {  0,  0,255,255};
	RECT_FIXED back0_dst = {
		FIXED_DEC(0 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(260 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(260,1)
	};
	
	RECT back1_src = {  0,  0,231,255};
	RECT_FIXED back1_dst = {
		FIXED_DEC(259 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(236 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(260,1)
	};
	
	Debug_StageMoveDebug(&back0_dst, 5, fx, fy);
	Debug_StageMoveDebug(&back1_dst, 6, fx, fy);
	Stage_DrawTex(&this->tex_back0, &back0_src, &back0_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back1, &back1_src, &back1_dst, stage.camera.bzoom);
}

void Back_PolusMaroon_Free(StageBack *back)
{
	Back_PolusMaroon *this = (Back_PolusMaroon*)back;
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_PolusMaroon_New(void)
{
	//Allocate background structure
	Back_PolusMaroon *this = (Back_PolusMaroon*)Mem_Alloc(sizeof(Back_PolusMaroon));
	if (this == NULL)
		return NULL;
		
	//Set background functions
	this->back.draw_fg = Back_PolusMaroon_DrawFG;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_PolusMaroon_DrawBG;
	this->back.free = Back_PolusMaroon_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\BG\\POLUSMRN.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Mem_Free(arc_back);
	
	return (StageBack*)this;
}
