/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "airship.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

//Airship background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0; //back0
	Gfx_Tex tex_back1; //back1
	Gfx_Tex tex_back2; //back2

} Back_Airship;


void Back_Airship_DrawBG(StageBack *back)
{
	Back_Airship *this = (Back_Airship*)back;

	fixed_t fx, fy;

	//Draw airship
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT back0_src = {  0,  0,227,255};
	RECT_FIXED back0_dst = {
		FIXED_DEC(0 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-163,1) - fy,
		FIXED_DEC(255 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(287,1)
	};
	
	RECT back1_src = {  0,  0,227,255};
	RECT_FIXED back1_dst = {
		FIXED_DEC(254 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-163,1) - fy,
		FIXED_DEC(255 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(287,1)
	};
	
	RECT floor1_src = {  0,  0,227, 49};
	RECT_FIXED floor1_dst = {
		FIXED_DEC(0 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(123,1) - fy,
		FIXED_DEC(255 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(55,1)
	};
	
	RECT floor2_src = {  0, 50,227, 49};
	RECT_FIXED floor2_dst = {
		FIXED_DEC(254 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(123,1) - fy,
		FIXED_DEC(255 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(55,1)
	};
	
	Debug_StageMoveDebug(&back0_dst, 6, fx, fy);
	Debug_StageMoveDebug(&back1_dst, 7, fx, fy);
	Debug_StageMoveDebug(&floor1_dst, 8, fx, fy);
	Debug_StageMoveDebug(&floor2_dst, 9, fx, fy);
	Stage_DrawTex(&this->tex_back0, &back0_src, &back0_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back1, &back1_src, &back1_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back2, &floor1_src, &floor1_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back2, &floor2_src, &floor2_dst, stage.camera.bzoom);
}

void Back_Airship_Free(StageBack *back)
{
	Back_Airship *this = (Back_Airship*)back;
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_Airship_New(void)
{
	//Allocate background structure
	Back_Airship *this = (Back_Airship*)Mem_Alloc(sizeof(Back_Airship));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Airship_DrawBG;
	this->back.free = Back_Airship_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\BG\\AIRSHIP.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_back2, Archive_Find(arc_back, "back2.tim"), 0);
	Mem_Free(arc_back);
	
	return (StageBack*)this;
}
