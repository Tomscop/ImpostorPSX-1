/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "turbulence.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

//Turbulence background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0; //back0
	Gfx_Tex tex_back1; //back1
	Gfx_Tex tex_back2; //back2
	Gfx_Tex tex_back3; //back3

} Back_Turbulence;

void Back_Turbulence_DrawFG(StageBack *back)
{
	Back_Turbulence *this = (Back_Turbulence*)back;

	fixed_t fx, fy;

	//Draw turbulence
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT frontcloud1_src = {  0, 62,128, 26};
	RECT_FIXED frontcloud1_dst = {
		FIXED_DEC(0 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(354,1) - fy,
		FIXED_DEC(687 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(140,1)
	};
	
	RECT frontcloud2_src = {127, 62,128, 26};
	RECT_FIXED frontcloud2_dst = {
		FIXED_DEC(0+682 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(354,1) - fy,
		FIXED_DEC(687 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(140,1)
	};
	
	Debug_StageMoveDebug(&frontcloud1_dst, 10, fx, fy);
	Debug_StageMoveDebug(&frontcloud2_dst, 11, fx, fy);
	Stage_DrawTex(&this->tex_back3, &frontcloud1_src, &frontcloud1_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back3, &frontcloud2_src, &frontcloud2_dst, stage.camera.bzoom);
}

void Back_Turbulence_DrawBG(StageBack *back)
{
	Back_Turbulence *this = (Back_Turbulence*)back;

	fixed_t fx, fy;

	//Draw turbulence
	fx = stage.camera.x / 2;
	fy = stage.camera.y / 2;
	
	RECT back0_src = {  0,  0,255,216};
	RECT_FIXED back0_dst = {
		FIXED_DEC(0 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(583 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(494,1)
	};
	
	RECT back1_src = {  0,  0,255,216};
	RECT_FIXED back1_dst = {
		FIXED_DEC(580 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(583 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(494,1)
	};
	
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT backcloud_src = {  0,  0,122, 51};
	RECT_FIXED backcloud_dst = {
		FIXED_DEC(0 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(200,1) - fy,
		FIXED_DEC(280 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(115,1)
	};
	
	RECT balloon_src = {123,  0, 30, 36};
	RECT_FIXED balloon_dst = {
		FIXED_DEC(0 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(137,1) - fy,
		FIXED_DEC(71 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(83,1)
	};
	
	RECT midcloud_src = {  0,  0,255, 61};
	RECT_FIXED midcloud_dst = {
		FIXED_DEC(0 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(164,1) - fy,
		FIXED_DEC(840 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(202,1)
	};
	
	Debug_StageMoveDebug(&back0_dst, 5, fx, fy);
	Debug_StageMoveDebug(&back1_dst, 6, fx, fy);
	Debug_StageMoveDebug(&backcloud_dst, 7, fx, fy);
	Debug_StageMoveDebug(&balloon_dst, 8, fx, fy);
	Debug_StageMoveDebug(&midcloud_dst, 9, fx, fy);
	Stage_DrawTex(&this->tex_back3, &midcloud_src, &midcloud_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back2, &balloon_src, &balloon_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back2, &backcloud_src, &backcloud_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back0, &back0_src, &back0_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back1, &back1_src, &back1_dst, stage.camera.bzoom);
}

void Back_Turbulence_Free(StageBack *back)
{
	Back_Turbulence *this = (Back_Turbulence*)back;
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_Turbulence_New(void)
{
	//Allocate background structure
	Back_Turbulence *this = (Back_Turbulence*)Mem_Alloc(sizeof(Back_Turbulence));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = Back_Turbulence_DrawFG;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Turbulence_DrawBG;
	this->back.free = Back_Turbulence_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\BG2\\TURBULEN.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_back2, Archive_Find(arc_back, "back2.tim"), 0);
	Gfx_LoadTex(&this->tex_back3, Archive_Find(arc_back, "back3.tim"), 0);
	Mem_Free(arc_back);
	
	return (StageBack*)this;
}
