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

//PolusMaroon Maroon background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0; //back0
	Gfx_Tex tex_back1; //back1

	int snowx[10];
	int snowy[10];
	
} Back_PolusMaroon;

void Back_PolusMaroon_DrawFG(StageBack *back)
{
	Back_PolusMaroon *this = (Back_PolusMaroon*)back;
	
	if (stage.paused == false)
	{
		for (u8 i = 0; i < 10; i++)
		{
		//Set snow a random value
		this->snowx[i] = RandomRange(0,320);
		this->snowy[i] = RandomRange(0,240);
		}
	}
	//Draw snows
	for (u8 i = 0; i < 10; i++)
	{
		RECT snow_src = {this->snowx[i], this->snowy[i], 4, 4};
		Gfx_DrawRect(&snow_src, 255, 255, 255);
	}
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

	for (u8 i = 0; i < 10; i++)
	{
		this->snowx[i] = RandomRange(0,320);
		this->snowy[i] = RandomRange(0,240);
	}
	
	return (StageBack*)this;
}
