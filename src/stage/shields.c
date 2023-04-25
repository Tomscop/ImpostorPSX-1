/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "shields.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

//Shields background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0; //back0
	Gfx_Tex tex_back1; //back1

} Back_Shields;

void Back_Shields_DrawFG(StageBack *back)
{
	Back_Shields *this = (Back_Shields*)back;

	fixed_t fx, fy;

	//Draw shields
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	//step 1152: emergency meeting
	//step 1157: thing appear
	//step 1168: eject
}

void Back_Shields_DrawBG(StageBack *back)
{
	Back_Shields *this = (Back_Shields*)back;

	fixed_t fx, fy;

	//Draw shields
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT back0_src = {  0,  0,255,255};
	RECT_FIXED back0_dst = {
		FIXED_DEC(0 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(273 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(273,1)
	};
	
	RECT back1_src = {  0,  0,188,255};
	RECT_FIXED back1_dst = {
		FIXED_DEC(272 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(201 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(273,1)
	};
	
	Debug_StageMoveDebug(&back0_dst, 5, fx, fy);
	Debug_StageMoveDebug(&back1_dst, 6, fx, fy);
	Stage_DrawTex(&this->tex_back0, &back0_src, &back0_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back1, &back1_src, &back1_dst, stage.camera.bzoom);
}

void Back_Shields_Free(StageBack *back)
{
	Back_Shields *this = (Back_Shields*)back;
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_Shields_New(void)
{
	//Allocate background structure
	Back_Shields *this = (Back_Shields*)Mem_Alloc(sizeof(Back_Shields));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Shields_DrawBG;
	this->back.free = Back_Shields_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\BG2\\SHIELDS.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Mem_Free(arc_back);
	
	return (StageBack*)this;
}
