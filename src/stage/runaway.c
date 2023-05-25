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
int frontcloud = -2;
int cloud, cloud2, cloud3 = 0;
int midcloud, midcloud2 = 0;
int backcloud, backcloud2 = 0;
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
	Gfx_Tex tex_back5; //back5

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
	
	RECT speakerthing_src = {  0, 84,174, 22};
	RECT_FIXED speakerthing_dst = {
		stage.gf->x-FIXED_DEC(88,1) - fx,
		stage.gf->y+FIXED_DEC(86,1) - fy,
		FIXED_DEC(174,1),
		FIXED_DEC(22,1)
	};
	Debug_StageMoveDebug(&speakerthing_dst, 10, fx, fy);
	Stage_DrawTex(&this->tex_back4, &speakerthing_src, &speakerthing_dst, stage.camera.bzoom);
	
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
	RECT_FIXED frontcloud1_dst = {FIXED_DEC(frontcloud+2,1) - fx, FIXED_DEC(331,1) - fy, FIXED_DEC(502,1), FIXED_DEC(163,1)};
	RECT frontcloud2_src = {127,  0,128, 42};
	RECT_FIXED frontcloud2_dst = {FIXED_DEC(frontcloud+500,1) - fx, FIXED_DEC(331,1) - fy, FIXED_DEC(502,1), FIXED_DEC(163,1)};
	RECT frontcloud3_src = {  0, 43,128, 42};
	RECT_FIXED frontcloud3_dst = {FIXED_DEC(frontcloud+998,1) - fx, FIXED_DEC(331,1) - fy, FIXED_DEC(502,1), FIXED_DEC(163,1)};
	RECT frontcloud4_src = {127, 43,128, 42};
	RECT_FIXED frontcloud4_dst = {FIXED_DEC(frontcloud+1496,1) - fx, FIXED_DEC(331,1) - fy, FIXED_DEC(502,1), FIXED_DEC(163,1)};
	RECT frontcloud5_src = {  0,  0,128, 42};
	RECT_FIXED frontcloud5_dst = {FIXED_DEC(frontcloud+1998,1) - fx, FIXED_DEC(331,1) - fy, FIXED_DEC(502,1), FIXED_DEC(163,1)};
	RECT frontcloud6_src = {127,  0,128, 42};
	RECT_FIXED frontcloud6_dst = {FIXED_DEC(frontcloud+2496,1) - fx, FIXED_DEC(331,1) - fy, FIXED_DEC(502,1), FIXED_DEC(163,1)};
	RECT frontcloud7_src = {  0, 43,128, 42};
	RECT_FIXED frontcloud7_dst = {FIXED_DEC(frontcloud+2994,1) - fx, FIXED_DEC(331,1) - fy, FIXED_DEC(502,1), FIXED_DEC(163,1)};
	Stage_DrawTex(&this->tex_back4, &frontcloud1_src, &frontcloud1_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back4, &frontcloud2_src, &frontcloud2_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back4, &frontcloud3_src, &frontcloud3_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back4, &frontcloud4_src, &frontcloud4_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back4, &frontcloud5_src, &frontcloud5_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back4, &frontcloud6_src, &frontcloud6_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back4, &frontcloud7_src, &frontcloud7_dst, stage.camera.bzoom);
	
	RECT cloud_src = {  0, 99,255, 89};
	RECT_FIXED cloud_dst = {FIXED_DEC(cloud,1) - fx, FIXED_DEC(38,1) - fy, FIXED_DEC(587,1), FIXED_DEC(205,1)};
	
	Debug_StageMoveDebug(&cloud_dst, 9, fx, fy);
	Stage_DrawTex(&this->tex_back5, &cloud_src, &cloud_dst, stage.camera.bzoom);
	Debug_StageMoveDebug(&back3_dst, 7, fx, fy);
	Stage_DrawTex(&this->tex_back3, &back3_src, &back3_dst, stage.camera.bzoom);
	
	RECT midcloud1_src = {  0, 47,128, 46};
	RECT_FIXED midcloud1_dst = {FIXED_DEC(midcloud,1) - fx, FIXED_DEC(244,1) - fy, FIXED_DEC(569,1), FIXED_DEC(226,1)};
	RECT midcloud2_src = {127, 47,128, 46};
	RECT_FIXED midcloud2_dst = {FIXED_DEC(midcloud+565,1) - fx, FIXED_DEC(244,1) - fy, FIXED_DEC(569,1), FIXED_DEC(226,1)};
	RECT midcloud3_src = {  0, 47,128, 46};
	RECT_FIXED midcloud3_dst = {FIXED_DEC(midcloud+1134,1) - fx, FIXED_DEC(244,1) - fy, FIXED_DEC(569,1), FIXED_DEC(226,1)};
	RECT midcloud4_src = {127, 47,128, 46};
	RECT_FIXED midcloud4_dst = {FIXED_DEC(midcloud+1699,1) - fx, FIXED_DEC(244,1) - fy, FIXED_DEC(569,1), FIXED_DEC(226,1)};
	Stage_DrawTex(&this->tex_back5, &midcloud1_src, &midcloud1_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back5, &midcloud2_src, &midcloud2_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back5, &midcloud3_src, &midcloud3_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back5, &midcloud4_src, &midcloud4_dst, stage.camera.bzoom);
	
	RECT backcloud1_src = {  0,  0,128, 46};
	RECT_FIXED backcloud1_dst = {FIXED_DEC(backcloud,1) - fx, FIXED_DEC(189,1) - fy, FIXED_DEC(569,1), FIXED_DEC(206,1)};
	RECT backcloud2_src = {127,  0,128, 46};
	RECT_FIXED backcloud2_dst = {FIXED_DEC(backcloud+565,1) - fx, FIXED_DEC(189,1) - fy, FIXED_DEC(569,1), FIXED_DEC(206,1)};
	RECT backcloud3_src = {  0,  0,128, 46};
	RECT_FIXED backcloud3_dst = {FIXED_DEC(backcloud+1134,1) - fx, FIXED_DEC(189,1) - fy, FIXED_DEC(569,1), FIXED_DEC(206,1)};
	RECT backcloud4_src = {127,  0,128, 46};
	RECT_FIXED backcloud4_dst = {FIXED_DEC(backcloud+1699,1) - fx, FIXED_DEC(189,1) - fy, FIXED_DEC(569,1), FIXED_DEC(206,1)};
	Stage_DrawTex(&this->tex_back5, &backcloud1_src, &backcloud1_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back5, &backcloud2_src, &backcloud2_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back5, &backcloud3_src, &backcloud3_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back5, &backcloud4_src, &backcloud4_dst, stage.camera.bzoom);
	
	Debug_StageMoveDebug(&back0_dst, 5, fx, fy);
	Debug_StageMoveDebug(&back1_dst, 6, fx, fy);
	Stage_DrawTex(&this->tex_back0, &back0_src, &back0_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back1, &back1_src, &back1_dst, stage.camera.bzoom);
	
	//move
	if (stage.paused == false)
	{
		platformx -= 9;
		frontcloud -= 3;
		cloud2 += 1;
		if (cloud2 == cloud3)
			cloud -= 1;
		midcloud2 += 1;
		backcloud2 += 1;
		if (midcloud2 == 2)
			midcloud -= 1;
		if (backcloud2 == 3)
			backcloud -= 1;
	}
	if (platformx <= -1440)
		platformx = 0;
	if (frontcloud <= -1998)
		frontcloud = 0;
	if (cloud <= -984)
	{
		cloud = 1030;
		cloud3 = RandomRange(1,2);
	}
	if ((cloud3 != 1) && (cloud3 != 2))
		cloud3 = 1;
	if (cloud2 == cloud3)
		cloud2 = 0;
	if (midcloud <= -1134)
		midcloud = 0;
	if (midcloud2 == 2)
		midcloud2 = 0;
	if (backcloud <= -1134)
		backcloud = 0;
	if (backcloud2 == 3)
		backcloud2 = 0;
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
	Gfx_LoadTex(&this->tex_back5, Archive_Find(arc_back, "back5.tim"), 0);
	Mem_Free(arc_back);
	
	frontcloud = RandomRange(-1100,0);
	cloud = RandomRange(-700,1130);
	cloud3 = RandomRange(1,2);
	midcloud = RandomRange(-1100,0);
	backcloud = RandomRange(-1100,0);
	
	return (StageBack*)this;
}
