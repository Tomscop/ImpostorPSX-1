/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "ejected.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

int bgy = -120;

//Ejected background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0; //back0
	Gfx_Tex tex_back1; //back1
	Gfx_Tex tex_back2; //back2
	Gfx_Tex tex_lines; //lines

} Back_Ejected;

void Back_Ejected_DrawFG(StageBack *back)
{
	Back_Ejected *this = (Back_Ejected*)back;

	fixed_t fx, fy;

	//Draw ejected
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT lines_src = {  0,  0,255,251};
	RECT_FIXED lines01_dst = {
		FIXED_DEC(-160 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(bgy,1) - fy,
		FIXED_DEC(255 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(251,1)
	};
	RECT_FIXED lines02_dst = {
		FIXED_DEC(198 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(bgy,1) - fy,
		FIXED_DEC(255 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(251,1)
	};
	RECT_FIXED lines11_dst = {
		FIXED_DEC(-160 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(bgy+907,1) - fy,
		FIXED_DEC(255 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(251,1)
	};
	RECT_FIXED lines12_dst = {
		FIXED_DEC(198 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(bgy+907,1) - fy,
		FIXED_DEC(255 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(251,1)
	};
	RECT_FIXED lines21_dst = {
		FIXED_DEC(-160 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(bgy+1814,1) - fy,
		FIXED_DEC(255 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(251,1)
	};
	RECT_FIXED lines22_dst = {
		FIXED_DEC(198 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(bgy+1814,1) - fy,
		FIXED_DEC(255 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(251,1)
	};
	RECT_FIXED lines31_dst = {
		FIXED_DEC(-160 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(bgy+2683,1) - fy,
		FIXED_DEC(255 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(251,1)
	};
	RECT_FIXED lines32_dst = {
		FIXED_DEC(198 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(bgy+2683,1) - fy,
		FIXED_DEC(255 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(251,1)
	};
	RECT_FIXED lines41_dst = {
		FIXED_DEC(-160 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(bgy+3566,1) - fy,
		FIXED_DEC(255 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(251,1)
	};
	RECT_FIXED lines42_dst = {
		FIXED_DEC(198 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(bgy+3566,1) - fy,
		FIXED_DEC(255 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(251,1)
	};
	
	RECT cloud0_src = {  0,  0,200, 46};
	RECT cloud1_src = {  0, 47,114, 35};
	RECT cloud2_src = {  0, 83, 98, 18};
	RECT cloud3_src = { 99, 83, 33, 11};
	
	RECT_FIXED cloud0_dst = {
		FIXED_DEC(99 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(bgy+2433,1) - fy,
		FIXED_DEC(555 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(129,1)
	};
	RECT_FIXED cloud1_dst = {
		FIXED_DEC(416 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(bgy+1011,1) - fy,
		FIXED_DEC(317 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(98,1)
	};
	RECT_FIXED cloud2_dst = {
		FIXED_DEC(72 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(bgy+3242,1) - fy,
		FIXED_DEC(275 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(48,1)
	};
	RECT_FIXED cloud3_dst = {
		FIXED_DEC(255 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(bgy+1733,1) - fy,
		FIXED_DEC(99 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(28,1)
	};
	RECT_FIXED cloud4_dst = {
		FIXED_DEC(-100 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(bgy+175,1) - fy,
		FIXED_DEC(555 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(129,1)
	};
	RECT_FIXED cloud5_dst = {
		FIXED_DEC(-100 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(bgy+3741,1) - fy,
		FIXED_DEC(555 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(129,1)
	};
	RECT_FIXED cloud6_dst = {
		FIXED_DEC(40 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(bgy+1292,1) - fy,
		FIXED_DEC(275 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(48,1)
	};
	RECT_FIXED cloud7_dst = {
		FIXED_DEC(567 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(bgy+2983,1) - fy,
		FIXED_DEC(99 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(28,1)
	};
	RECT_FIXED cloud8_dst = {
		FIXED_DEC(459 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(bgy+180,1) - fy,
		FIXED_DEC(99 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(28,1)
	};
	RECT_FIXED cloud9_dst = {
		FIXED_DEC(459 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(bgy+3775,1) - fy,
		FIXED_DEC(99 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(28,1)
	};
	
	Stage_DrawTex(&this->tex_lines, &lines_src, &lines01_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_lines, &lines_src, &lines02_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_lines, &lines_src, &lines11_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_lines, &lines_src, &lines12_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_lines, &lines_src, &lines21_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_lines, &lines_src, &lines22_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_lines, &lines_src, &lines31_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_lines, &lines_src, &lines32_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_lines, &lines_src, &lines41_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_lines, &lines_src, &lines42_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back2, &cloud0_src, &cloud0_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back2, &cloud1_src, &cloud1_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back2, &cloud2_src, &cloud2_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back2, &cloud3_src, &cloud3_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back2, &cloud0_src, &cloud4_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back2, &cloud0_src, &cloud5_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back2, &cloud2_src, &cloud6_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back2, &cloud3_src, &cloud7_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back2, &cloud3_src, &cloud8_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back2, &cloud3_src, &cloud9_dst, stage.camera.bzoom);
}

void Back_Ejected_DrawBG(StageBack *back)
{
	Back_Ejected *this = (Back_Ejected*)back;

	fixed_t fx, fy;

	//Draw ejected
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT building0_src = {  0,  0,100,128};
	RECT_FIXED building0_dst = {
		FIXED_DEC(-160 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(bgy,1) - fy,
		FIXED_DEC(715 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(914,1)
	};
	RECT building1_src = {  0,127,100,128};
	RECT_FIXED building1_dst = {
		FIXED_DEC(-160 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(bgy+907,1) - fy,
		FIXED_DEC(715 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(914,1)
	};
	RECT building2_src = {101,  0,100,123};
	RECT_FIXED building2_dst = {
		FIXED_DEC(-160 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(bgy+1814,1) - fy,
		FIXED_DEC(715 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(876,1)
	};
	RECT building3_src = {101,122,100,124};
	RECT_FIXED building3_dst = {
		FIXED_DEC(-160 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(bgy+2683,1) - fy,
		FIXED_DEC(715 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(883,1)
	};
	RECT building4_src = {  0,  0,100,128};
	RECT_FIXED building4_dst = {
		FIXED_DEC(-160 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(bgy+3566,1) - fy,
		FIXED_DEC(715 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(914,1)
	};
	
	RECT back0_src = {  0,  0,255,175};
	RECT_FIXED back0_dst = {
		FIXED_DEC(-160 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-120,1) - fy,
		FIXED_DEC(715 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(491,1)
	};
	
	Debug_StageMoveDebug(&back0_dst, 5, fx, fy);
	Stage_DrawTex(&this->tex_back1, &building0_src, &building0_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back1, &building1_src, &building1_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back1, &building2_src, &building2_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back1, &building3_src, &building3_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back1, &building4_src, &building4_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back0, &back0_src, &back0_dst, stage.camera.bzoom);
	
	//move da funni
	if (stage.paused == false)
		bgy -= 52;
	if (bgy <= -3546)
		bgy = -120;
}

void Back_Ejected_Free(StageBack *back)
{
	Back_Ejected *this = (Back_Ejected*)back;
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_Ejected_New(void)
{
	//Allocate background structure
	Back_Ejected *this = (Back_Ejected*)Mem_Alloc(sizeof(Back_Ejected));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = Back_Ejected_DrawFG;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Ejected_DrawBG;
	this->back.free = Back_Ejected_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\BG\\EJECTED.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_back2, Archive_Find(arc_back, "back2.tim"), 0);
	Gfx_LoadTex(&this->tex_lines, Archive_Find(arc_back, "lines.tim"), 0);
	Mem_Free(arc_back);
	
	return (StageBack*)this;
}
