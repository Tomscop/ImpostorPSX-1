/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "runaway.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

int platformx = 0;
int frontcloud = -3;

//Run Away background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0; //back0
	Gfx_Tex tex_back1; //back1
	Gfx_Tex tex_back2; //back2
	Gfx_Tex tex_back3; //back3
	Gfx_Tex tex_back4; //back4

} Back_RunAway;

void Back_RunAway_DrawBG(StageBack *back)
{
	Back_RunAway *this = (Back_RunAway*)back;

	fixed_t fx, fy;

	//Draw runaway
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT back0_src = {  0,  0,253,255};
	RECT_FIXED back0_dst = {
		FIXED_DEC(0,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(553,1),
		FIXED_DEC(558,1)
	};
	
	RECT back1_src = {  0,  0,219,255};
	RECT_FIXED back1_dst = {
		FIXED_DEC(551,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(479,1),
		FIXED_DEC(558,1)
	};
	
	RECT back3_src = {  0,  0,255,237};
	RECT_FIXED back3_dst = {
		FIXED_DEC(630,1) - fx,
		FIXED_DEC(7,1) - fy,
		FIXED_DEC(480,1),
		FIXED_DEC(446,1)
	};
	
	//platforms
	RECT platform_src = {  0,  0,255,193};
	RECT_FIXED platform1_dst = {FIXED_DEC(platformx,1) - fx, FIXED_DEC(339,1) - fy, FIXED_DEC(290,1), FIXED_DEC(219,1)};
	RECT_FIXED platform2_dst = {FIXED_DEC(platformx+288,1) - fx, FIXED_DEC(339,1) - fy, FIXED_DEC(290,1), FIXED_DEC(219,1)};
	RECT_FIXED platform3_dst = {FIXED_DEC(platformx+576,1) - fx, FIXED_DEC(339,1) - fy, FIXED_DEC(290,1), FIXED_DEC(219,1)};
	RECT_FIXED platform4_dst = {FIXED_DEC(platformx+864,1) - fx, FIXED_DEC(339,1) - fy, FIXED_DEC(290,1), FIXED_DEC(219,1)};
	RECT_FIXED platform5_dst = {FIXED_DEC(platformx+1152,1) - fx, FIXED_DEC(339,1) - fy, FIXED_DEC(290,1), FIXED_DEC(219,1)};
	RECT_FIXED platform6_dst = {FIXED_DEC(platformx+1440,1) - fx, FIXED_DEC(339,1) - fy, FIXED_DEC(290,1), FIXED_DEC(219,1)};
	RECT_FIXED platform7_dst = {FIXED_DEC(platformx+1728,1) - fx, FIXED_DEC(339,1) - fy, FIXED_DEC(290,1), FIXED_DEC(219,1)};
	RECT_FIXED platform8_dst = {FIXED_DEC(platformx+2016,1) - fx, FIXED_DEC(339,1) - fy, FIXED_DEC(290,1), FIXED_DEC(219,1)};
	RECT_FIXED platform9_dst = {FIXED_DEC(platformx+2304,1) - fx, FIXED_DEC(339,1) - fy, FIXED_DEC(290,1), FIXED_DEC(219,1)};
	RECT_FIXED platform10_dst = {FIXED_DEC(platformx+2592,1) - fx, FIXED_DEC(339,1) - fy, FIXED_DEC(290,1), FIXED_DEC(219,1)};
	RECT_FIXED platform11_dst = {FIXED_DEC(platformx+2280,1) - fx, FIXED_DEC(339,1) - fy, FIXED_DEC(290,1), FIXED_DEC(219,1)};
	Debug_StageMoveDebug(&platform1_dst, 8, fx, fy);
	Stage_DrawTex(&this->tex_back2, &platform_src, &platform1_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back2, &platform_src, &platform2_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back2, &platform_src, &platform3_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back2, &platform_src, &platform4_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back2, &platform_src, &platform5_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back2, &platform_src, &platform6_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back2, &platform_src, &platform7_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back2, &platform_src, &platform8_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back2, &platform_src, &platform9_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back2, &platform_src, &platform10_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back2, &platform_src, &platform11_dst, stage.camera.bzoom);
	
	RECT frontcloud1_src = {  0,  0,128, 42};
	RECT_FIXED frontcloud1_dst = {FIXED_DEC(frontcloud+3,1) - fx, FIXED_DEC(331,1) - fy, FIXED_DEC(502,1), FIXED_DEC(163,1)};
	RECT frontcloud2_src = {127,  0,128, 42};
	RECT_FIXED frontcloud2_dst = {FIXED_DEC(frontcloud+501,1) - fx, FIXED_DEC(331,1) - fy, FIXED_DEC(502,1), FIXED_DEC(163,1)};
	RECT frontcloud3_src = {  0, 43,128, 42};
	RECT_FIXED frontcloud3_dst = {FIXED_DEC(frontcloud+999,1) - fx, FIXED_DEC(331,1) - fy, FIXED_DEC(502,1), FIXED_DEC(163,1)};
	RECT frontcloud4_src = {127, 43,128, 42};
	RECT_FIXED frontcloud4_dst = {FIXED_DEC(frontcloud+1497,1) - fx, FIXED_DEC(331,1) - fy, FIXED_DEC(502,1), FIXED_DEC(163,1)};
	RECT frontcloud5_src = {  0,  0,128, 42};
	RECT_FIXED frontcloud5_dst = {FIXED_DEC(frontcloud+1995,1) - fx, FIXED_DEC(331,1) - fy, FIXED_DEC(502,1), FIXED_DEC(163,1)};
	RECT frontcloud6_src = {127,  0,128, 42};
	RECT_FIXED frontcloud6_dst = {FIXED_DEC(frontcloud+2493,1) - fx, FIXED_DEC(331,1) - fy, FIXED_DEC(502,1), FIXED_DEC(163,1)};
	RECT frontcloud7_src = {  0, 43,128, 42};
	RECT_FIXED frontcloud7_dst = {FIXED_DEC(frontcloud+2991,1) - fx, FIXED_DEC(331,1) - fy, FIXED_DEC(502,1), FIXED_DEC(163,1)};
	Stage_DrawTex(&this->tex_back4, &frontcloud1_src, &frontcloud1_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back4, &frontcloud2_src, &frontcloud2_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back4, &frontcloud3_src, &frontcloud3_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back4, &frontcloud4_src, &frontcloud4_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back4, &frontcloud5_src, &frontcloud5_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back4, &frontcloud6_src, &frontcloud6_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back4, &frontcloud7_src, &frontcloud7_dst, stage.camera.bzoom);
	
	Debug_StageMoveDebug(&back0_dst, 5, fx, fy);
	Debug_StageMoveDebug(&back1_dst, 6, fx, fy);
	Debug_StageMoveDebug(&back3_dst, 7, fx, fy);
	Stage_DrawTex(&this->tex_back3, &back3_src, &back3_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back0, &back0_src, &back0_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back1, &back1_src, &back1_dst, stage.camera.bzoom);
	
	//move
	if (stage.paused == false)
	{
		platformx -= 9;
		frontcloud -= 3;
	}
	if (platformx <= -1440)
		platformx = 0;
	if (frontcloud <= -1995)
		frontcloud = 0;
}

void Back_RunAway_Free(StageBack *back)
{
	Back_RunAway *this = (Back_RunAway*)back;
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_RunAway_New(void)
{
	//Allocate background structure
	Back_RunAway *this = (Back_RunAway*)Mem_Alloc(sizeof(Back_RunAway));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_RunAway_DrawBG;
	this->back.free = Back_RunAway_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\BG\\RUNAWAY.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_back2, Archive_Find(arc_back, "back2.tim"), 0);
	Gfx_LoadTex(&this->tex_back3, Archive_Find(arc_back, "back3.tim"), 0);
	Gfx_LoadTex(&this->tex_back4, Archive_Find(arc_back, "back4.tim"), 0);
	Mem_Free(arc_back);
	
	return (StageBack*)this;
}
