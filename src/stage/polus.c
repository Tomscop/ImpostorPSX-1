/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "polus.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

//Polus background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0; //back0
	Gfx_Tex tex_back1; //back1
	Gfx_Tex tex_back2; //back2

} Back_Polus;


void Back_Polus_DrawBG(StageBack *back)
{
	Back_Polus *this = (Back_Polus*)back;

	fixed_t fx, fy;

	//Draw polus
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT back0_src = {  0,  0,255,223};
	RECT_FIXED back0_dst = {
		FIXED_DEC(0 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(272 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(238,1)
	};
	
	RECT back1_src = {  0,  0,255,223};
	RECT_FIXED back1_dst = {
		FIXED_DEC(271 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(272 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(238,1)
	};
	
	Debug_StageMoveDebug(&back0_dst, 5, fx, fy);
	Stage_DrawTex(&this->tex_back0, &back0_src, &back0_dst, stage.camera.bzoom);
	Debug_StageMoveDebug(&back1_dst, 6, fx, fy);
	Stage_DrawTex(&this->tex_back1, &back1_src, &back1_dst, stage.camera.bzoom);
	
	
	//Draw sky
	fx = stage.camera.x / 2;
	fy = stage.camera.y / 2;
	
	RECT sky0_src = {  0,  0,180,127};
	RECT_FIXED sky0_dst = {
		FIXED_DEC(-58 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-70,1) - fy,
		FIXED_DEC(238 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(168,1)
	};
	
	RECT sky1_src = {  0,128,181,127};
	RECT_FIXED sky1_dst = {
		FIXED_DEC(180 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-70,1) - fy,
		FIXED_DEC(239 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(168,1)
	};
	
	Debug_StageMoveDebug(&sky0_dst, 7, fx,fy);
	Stage_DrawTex(&this->tex_back2, &sky0_src, &sky0_dst, stage.camera.bzoom);
	Debug_StageMoveDebug(&sky1_dst, 8, fx, fy);
	Stage_DrawTex(&this->tex_back2, &sky1_src, &sky1_dst, stage.camera.bzoom);
}

void Back_Polus_Free(StageBack *back)
{
	Back_Polus *this = (Back_Polus*)back;
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_Polus_New(void)
{
	//Allocate background structure
	Back_Polus *this = (Back_Polus*)Mem_Alloc(sizeof(Back_Polus));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Polus_DrawBG;
	this->back.free = Back_Polus_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\BG\\POLUS.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_back2, Archive_Find(arc_back, "back2.tim"), 0);
	Mem_Free(arc_back);
	
	return (StageBack*)this;
}
