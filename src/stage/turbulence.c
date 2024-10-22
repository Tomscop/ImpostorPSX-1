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

int backc = 324;
int balloonm = 34;
int midc = -78;
int frontc1 = -350;
int windm1 = 838;
int windm2 = -1285;
//Turbulence background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	IO_Data arc_claw, arc_claw_ptr[2];
	Gfx_Tex tex_back0; //back0
	Gfx_Tex tex_back1; //back1
	Gfx_Tex tex_back2; //back2
	Gfx_Tex tex_back3; //back3
	
	//Claw state
	Gfx_Tex tex_claw;
	u8 claw_frame, claw_tex_id;
	Animatable claw_animatable;
	
} Back_Turbulence;

//Claw animation and rects
static const CharFrame claw_frame[] = {
  {0, {  0,  0, 99,139}, {160,157}}, //0 claw 1
  {0, { 99,  0, 99,139}, {160,157}}, //1 claw 2
  {1, {  0,  0, 98,144}, {160,160}}, //2 claw 3
  {1, { 98,  0, 97,145}, {160,160}}, //3 claw 4
};

static const Animation claw_anim[] = {
	{2, (const u8[]){ 0, 1, 2, 3, ASCR_BACK, 1}}, //Idle
};

//Claw functions
void Turbulence_Claw_SetFrame(void *user, u8 frame)
{
	Back_Turbulence *this = (Back_Turbulence*)user;
	
	//Check if this is a new frame
	if (frame != this->claw_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &claw_frame[this->claw_frame = frame];
		if (cframe->tex != this->claw_tex_id)
			Gfx_LoadTex(&this->tex_claw, this->arc_claw_ptr[this->claw_tex_id = cframe->tex], 0);
	}
}

void Turbulence_Claw_Draw(Back_Turbulence *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &claw_frame[this->claw_frame];
    
    fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = { ox, oy, src.w << FIXED_SHIFT, src.h << FIXED_SHIFT};
	Debug_StageMoveDebug(&dst, 11, stage.camera.x, stage.camera.y);
	Stage_DrawTex(&this->tex_claw, &src, &dst, stage.camera.bzoom);
}

void Back_Turbulence_DrawFG(StageBack *back)
{
	Back_Turbulence *this = (Back_Turbulence*)back;

	fixed_t fx, fy;

	//Draw turbulence
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT frontcloud1_src = {  0, 62,128, 26};
	RECT_FIXED frontcloud1_dst = {
		FIXED_DEC(frontc1 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(354,1) - fy,
		FIXED_DEC(687 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(140,1)
	};
	
	RECT frontcloud2_src = {127, 62,128, 26};
	RECT_FIXED frontcloud2_dst = {
		FIXED_DEC(frontc1+682 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(354,1) - fy,
		FIXED_DEC(687 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(140,1)
	};
	
	RECT wind_src = {  0, 52,255, 84};
	RECT_FIXED wind1_dst = {
		FIXED_DEC(windm1 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(135,1) - fy,
		FIXED_DEC(1035 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(341,1)
	};
	RECT_FIXED wind2_dst = {
		FIXED_DEC(windm2 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(135,1) - fy,
		FIXED_DEC(1035 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(341,1)
	};
	
	Debug_StageMoveDebug(&frontcloud1_dst, 13, fx, fy);
	Debug_StageMoveDebug(&frontcloud2_dst, 14, fx, fy);
	Debug_StageMoveDebug(&wind1_dst, 15, fx, fy);
	Debug_StageMoveDebug(&wind2_dst, 16, fx, fy);
	Stage_BlendTex(&this->tex_back2, &wind_src, &wind1_dst, stage.camera.bzoom,1);
	Stage_BlendTex(&this->tex_back2, &wind_src, &wind2_dst, stage.camera.bzoom,1);
	Stage_DrawTex(&this->tex_back3, &frontcloud1_src, &frontcloud1_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back3, &frontcloud2_src, &frontcloud2_dst, stage.camera.bzoom);
	
	//Animate and draw claw
	if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step & 0x7) == 0)
		Animatable_SetAnim(&this->claw_animatable, 0);
	Animatable_Animate(&this->claw_animatable, (void*)this, Turbulence_Claw_SetFrame);
	Turbulence_Claw_Draw(this, FIXED_DEC(192+426 + 160,1) - fx, FIXED_DEC(293-53 + 160,1) - fy);
}

void Back_Turbulence_DrawMG(StageBack *back)
{
	Back_Turbulence *this = (Back_Turbulence*)back;

	fixed_t fx, fy;

	//Draw turbulence
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT arm_src = {  0, 89,255, 31};
	RECT_FIXED arm_dst = {
		FIXED_DEC(192 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(293,1) - fy,
		FIXED_DEC(513 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(63,1)
	};
	
	Debug_StageMoveDebug(&arm_dst, 10, fx, fy);
	Stage_DrawTex(&this->tex_back3, &arm_src, &arm_dst, stage.camera.bzoom);
}

void Back_Turbulence_DrawBG(StageBack *back)
{
	Back_Turbulence *this = (Back_Turbulence*)back;

	fixed_t fx, fy;

	//Draw turbulence
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT back0_src = {  0,  0,255,216};
	RECT_FIXED back0_dst = {
		FIXED_DEC(-217 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(583 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(494,1)
	};
	
	RECT back1_src = {  0,  0,255,216};
	RECT_FIXED back1_dst = {
		FIXED_DEC(580-217 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(583 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(494,1)
	};
	
	RECT backcloud_src = {  0,  0,122, 51};
	RECT_FIXED backcloud_dst = {
		FIXED_DEC(backc - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(200,1) - fy,
		FIXED_DEC(280 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(115,1)
	};
	
	RECT balloon_src = {123,  0, 30, 36};
	RECT_FIXED balloon_dst = {
		FIXED_DEC(balloonm - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(137,1) - fy,
		FIXED_DEC(71 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(83,1)
	};
	
	RECT midcloud_src = {  0,  0,255, 61};
	RECT_FIXED midcloud_dst = {
		FIXED_DEC(midc - screen.SCREEN_WIDEOADD2,1) - fx,
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
	
	//move stuff
	if (stage.paused == false)
	{
	if ((stage.song_step >= -29) && (stage.song_step <= 799)) //speed = 1
	{
		backc += 2;
		balloonm += 3;
		midc += 4;
		frontc1 += 8;
		windm1 += 7;
		windm2 += 7;
	}
	else if ((stage.song_step >= 800) && (stage.song_step <= 815)) //speed = 1.2
	{
		backc += 2;
		balloonm += 3;
		midc += 5;
		frontc1 += 9;
		windm1 += 8;
		windm2 += 8;
	}
	else if ((stage.song_step >= 816) && (stage.song_step <= 943)) //speed = 1.4
	{
		backc += 2;
		balloonm += 3;
		midc += 6;
		frontc1 += 10;
		windm1 += 9;
		windm2 += 9;
	}
	else if ((stage.song_step >= 944) && (stage.song_step <= 1039)) //speed = 1.6
	{
		backc += 3;
		balloonm += 3;
		midc += 6;
		frontc1 += 12;
		windm1 += 11;
		windm2 += 11;
	}
	else if ((stage.song_step >= 1040) && (stage.song_step <= 1055)) //speed = 1.8
	{
		backc += 3;
		balloonm += 4;
		midc += 7;
		frontc1 += 13;
		windm1 += 12;
		windm2 += 12;
	}
	else if ((stage.song_step >= 1056) && (stage.song_step <= 1071)) //speed = 1.9
	{
		backc += 3;
		balloonm += 4;
		midc += 7;
		frontc1 += 14;
		windm1 += 13;
		windm2 += 13;
	}
	else if ((stage.song_step >= 1072) && (stage.song_step <= 1135)) //speed = 2
	{
		backc += 3;
		balloonm += 4;
		midc += 8;
		frontc1 += 14;
		windm1 += 14;
		windm2 += 14;
	}
	else if ((stage.song_step >= 1136) && (stage.song_step <= 1199)) //speed = 2.2
	{
		backc += 4;
		balloonm += 5;
		midc += 9;
		frontc1 += 17;
		windm1 += 17;
		windm2 += 17;
	}
	else if (stage.song_step >= 1200) //speed = 3
	{
		backc += 4;
		balloonm += 5;
		midc += 11;
		frontc1 += 21;
		windm1 += 21;
		windm2 += 21;
	}
	
	if (backc > 1285)
		backc = -338;
	if (balloonm > 860)
		balloonm = -338;
	if (midc > 1285)
		midc = -838;
	if (frontc1 > 1285)
		frontc1 = -1088;
	if (windm1 > 1285)
		windm1 = -838;
	if (windm2 > 1285)
		windm2 = -838;
	}
}

void Back_Turbulence_Free(StageBack *back)
{
	Back_Turbulence *this = (Back_Turbulence*)back;
	
	//Free claw archive
	Mem_Free(this->arc_claw);
	
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
	this->back.draw_md = Back_Turbulence_DrawMG;
	this->back.draw_bg = Back_Turbulence_DrawBG;
	this->back.free = Back_Turbulence_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\BG2\\TURBULEN.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_back2, Archive_Find(arc_back, "back2.tim"), 0);
	Gfx_LoadTex(&this->tex_back3, Archive_Find(arc_back, "back3.tim"), 0);
	Mem_Free(arc_back);
	
	//Load claw textures
	this->arc_claw = IO_Read("\\BG2\\CLAW.ARC;1");
	this->arc_claw_ptr[0] = Archive_Find(this->arc_claw, "claw0.tim");
	this->arc_claw_ptr[1] = Archive_Find(this->arc_claw, "claw1.tim");
	
	//Initialize claw state
	Animatable_Init(&this->claw_animatable, claw_anim);
	Animatable_SetAnim(&this->claw_animatable, 0);
	this->claw_frame = this->claw_tex_id = 0xFF; //Force art load
	
	return (StageBack*)this;
}
