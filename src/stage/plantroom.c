/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "plantroom.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

int vine = 0;

//Plantroom background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0; //back0
	Gfx_Tex tex_back1; //back1
	Gfx_Tex tex_vine0; //vine0
	Gfx_Tex tex_vine1; //vine1
	Gfx_Tex tex_pot; //pot

} Back_Plantroom;

void Back_Plantroom_DrawFG(StageBack *back)
{
	Back_Plantroom *this = (Back_Plantroom*)back;

	fixed_t fx, fy;

	//Draw plantroom
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	fx = (stage.camera.x * 7) / 5;
	RECT vine0_src = {  0,  0,171,175};
	RECT_FIXED vine0_dst = {
		FIXED_DEC(0 + vine - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(171 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(175,1)
	};
	
	RECT vine1_src = {  0,  0,189,203};
	RECT_FIXED vine1_dst = {
		FIXED_DEC(458 + vine - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(189 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(203,1)
	};
	
	fx = (stage.camera.x * 6) / 5;
	RECT pot_src = {  0,  0,239,91};
	RECT_FIXED pot_dst = {
		FIXED_DEC(0 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(239 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(91,1)
	};
	
	Debug_StageMoveDebug(&vine0_dst, 11, fx, fy);
	Debug_StageMoveDebug(&vine1_dst, 12, fx, fy);
	Debug_StageMoveDebug(&pot_dst, 13, fx, fy);
	Stage_DrawTex(&this->tex_vine0, &vine0_src, &vine0_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_vine1, &vine1_src, &vine1_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_pot, &pot_src, &pot_dst, stage.camera.bzoom);
	
	//Vine move
	if ((stage.song_step & 0x7 )== 2)
		vine = -1;
	else if ((stage.song_step & 0x7 )== 3)
		vine = -1;
	else if ((stage.song_step & 0x7 )== 6)
		vine = 1;
	else if ((stage.song_step & 0x7 )== 7)
		vine = 1;
	else
		vine = 0;
}

void Back_Plantroom_DrawBG(StageBack *back)
{
	Back_Plantroom *this = (Back_Plantroom*)back;

	fixed_t fx, fy;

	//Draw plantroom
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT back0_src = {  0,  0,254,255};
	RECT_FIXED back0_dst = {
		FIXED_DEC(0 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(363 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(364,1)
	};
	
	RECT back1_src = {  0,  0,237,255};
	RECT_FIXED back1_dst = {
		FIXED_DEC(361 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(339 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(364,1)
	};
	
	Debug_StageMoveDebug(&back0_dst, 5, fx, fy);
	Debug_StageMoveDebug(&back1_dst, 6, fx, fy);
	Stage_DrawTex(&this->tex_back0, &back0_src, &back0_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back1, &back1_src, &back1_dst, stage.camera.bzoom);
}

void Back_Plantroom_Free(StageBack *back)
{
	Back_Plantroom *this = (Back_Plantroom*)back;
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_Plantroom_New(void)
{
	//Allocate background structure
	Back_Plantroom *this = (Back_Plantroom*)Mem_Alloc(sizeof(Back_Plantroom));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = Back_Plantroom_DrawFG;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Plantroom_DrawBG;
	this->back.free = Back_Plantroom_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\BG\\PLANTROM.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_vine0, Archive_Find(arc_back, "vine0.tim"), 0);
	Gfx_LoadTex(&this->tex_vine1, Archive_Find(arc_back, "vine1.tim"), 0);
	Gfx_LoadTex(&this->tex_pot, Archive_Find(arc_back, "pot.tim"), 0);
	Mem_Free(arc_back);
	
	return (StageBack*)this;
}
