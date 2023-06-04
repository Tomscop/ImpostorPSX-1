/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "finale.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

int icony;

//Finale background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	IO_Data arc_iconanim, arc_iconanim_ptr[2];
	IO_Data arc_fb, arc_fb_ptr[2];
	Gfx_Tex tex_back0; //back0
	Gfx_Tex tex_back1; //back1
	Gfx_Tex tex_back2; //back2
	Gfx_Tex tex_back3; //back3
	Gfx_Tex tex_back4; //back4
	Gfx_Tex tex_back5; //back5
	Gfx_Tex tex_back6; //back6
	
	//IconAnim state
	Gfx_Tex tex_iconanim;
	u8 iconanim_frame, iconanim_tex_id;
	Animatable iconanim_animatable;
	
	//fb state
	Gfx_Tex tex_fb;
	u8 fb_frame, fb_tex_id;
	Animatable fb_animatable;
	
	//fade stuff
	fixed_t fade, fadespd;
	
} Back_Finale;

//IconAnim animation and rects
static const CharFrame iconanim_frame[] = {
  {0, {  0,  0, 57, 89}, {134,154}}, //0 iconanim 1
  {0, { 57,  0, 57, 89}, {134,154}}, //1 iconanim 2
  {0, {114,  0, 56, 91}, {133,156}}, //2 iconanim 3
  {0, {170,  0, 56, 91}, {133,156}}, //3 iconanim 4
  {0, {  0, 91, 58, 83}, {135,147}}, //4 iconanim 5
  {0, { 58, 91, 58, 83}, {134,147}}, //5 iconanim 6
  {0, {116, 91, 57, 70}, {135,135}}, //6 iconanim 7
  {0, {173, 91, 57, 70}, {135,135}}, //7 iconanim 8
  {1, {  0,  0, 57, 71}, {135,136}}, //8 iconanim 9
  {1, { 57,  0, 57, 71}, {135,136}}, //9 iconanim 10
};

static const Animation iconanim_anim[] = {
	{1, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, ASCR_CHGANI, 0}}, //Idle
};

//IconAnim functions
void Finale_IconAnim_SetFrame(void *user, u8 frame)
{
	Back_Finale *this = (Back_Finale*)user;
	
	//Check if this is a new frame
	if (frame != this->iconanim_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &iconanim_frame[this->iconanim_frame = frame];
		if (cframe->tex != this->iconanim_tex_id)
			Gfx_LoadTex(&this->tex_iconanim, this->arc_iconanim_ptr[this->iconanim_tex_id = cframe->tex], 0);
	}
}

void Finale_IconAnim_Draw(Back_Finale *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &iconanim_frame[this->iconanim_frame];
    
    fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = { ox, oy, src.w << FIXED_SHIFT, src.h << FIXED_SHIFT};
	Debug_StageMoveDebug(&dst, 14, stage.camera.x, stage.camera.y);
	Stage_DrawTex(&this->tex_iconanim, &src, &dst, stage.bump);
}

void Back_Finale_DrawMG(StageBack *back) //icon
{
	Back_Finale *this = (Back_Finale*)back;

	if (stage.song_step == -29)
		Animatable_SetAnim(&this->iconanim_animatable, 0);
	Animatable_Animate(&this->iconanim_animatable, (void*)this, Finale_IconAnim_SetFrame);
	if ((stage.song_step >= 272) && (stage.song_step <= 1983))
		Finale_IconAnim_Draw(this, FIXED_DEC(-178 + 190,1), FIXED_DEC(icony + 235,1));
	if (stage.prefs.downscroll)
		icony = -231;
	else
		icony = -61;
}

//FB animation and rects
static const CharFrame fb_frame[] = {
  {0, {  0,  0,226,127}, {0,0}}, //0 polus
  {0, {  0,128,226,127}, {0,0}}, //1 mira
  {1, {  0,  0,226,127}, {0,0}}, //2 airship
  {0, {226,  0,  1,  1}, {0,0}}, //3 hide
};

static const Animation fb_anim[] = {
	{1, (const u8[]){ 0, ASCR_BACK, 1}}, //Polus
	{1, (const u8[]){ 1, ASCR_BACK, 1}}, //Mira
	{1, (const u8[]){ 2, ASCR_BACK, 1}}, //Airship
	{1, (const u8[]){ 3, ASCR_BACK, 1}}, //hide
};

//FB functions
void Finale_FB_SetFrame(void *user, u8 frame)
{
	Back_Finale *this = (Back_Finale*)user;
	
	//Check if this is a new frame
	if (frame != this->fb_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &fb_frame[this->fb_frame = frame];
		if (cframe->tex != this->fb_tex_id)
			Gfx_LoadTex(&this->tex_fb, this->arc_fb_ptr[this->fb_tex_id = cframe->tex], 0);
	}
}

void Finale_FB_Draw(Back_Finale *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &fb_frame[this->fb_frame];
    
    fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = { ox, oy, src.w * FIXED_DEC(18899,10000), FIXED_DEC(240,1)};
	Debug_StageMoveDebug(&dst, 13, stage.camera.x, stage.camera.y);
	Stage_BlendTex(&this->tex_fb, &src, &dst, stage.bump, 1);
}

void Back_Finale_DrawFG(StageBack *back)
{
	Back_Finale *this = (Back_Finale*)back;

	fixed_t fx, fy;

	//Draw finale
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	//start fade
	if (stage.song_step == -29)
	{
		this->fade = FIXED_DEC(255,1);
		this->fadespd = FIXED_DEC(0,1);
	}
	if (stage.song_step == 0)
	{
		this->fade = FIXED_DEC(255,1);
		this->fadespd = FIXED_DEC(60,1);
	}
	if (stage.song_step == 128)
	{
		stage.reactor = FIXED_DEC(255,1);
		stage.reactorspd = FIXED_DEC(1024,1);
	}
	if (stage.song_step == 272)
	{
		stage.flash = FIXED_DEC(255,1);
		stage.flashspd = FIXED_DEC(768,1);
	}
	if (stage.song_step == 1986)
	{
		this->fade = FIXED_DEC(1,1);
		this->fadespd = FIXED_DEC(60,1);
	}

	//end fade
	if (stage.song_step == 64)
		this->fade = 0;

	if (this->fade > 0)
	{
		RECT flash = {0, 0, screen.SCREEN_WIDTH, screen.SCREEN_HEIGHT};
		u8 flash_col = this->fade >> FIXED_SHIFT;
		Gfx_BlendRect(&flash, flash_col, flash_col, flash_col, 2);
		if (stage.paused == false)
		{
			if (stage.song_step <= 700)
				this->fade -= FIXED_MUL(this->fadespd, timer_dt);
			else if (this->fade <= FIXED_DEC(254,1))
				this->fade += FIXED_MUL(this->fadespd, timer_dt);
		}
	}
	
	//Draw red ending thing
	RECT screen_src = {0, 0, screen.SCREEN_WIDTH, screen.SCREEN_HEIGHT};
	if (stage.song_step >= 1984)
		Gfx_DrawRect(&screen_src, 255, 18, 102);
	
	if (stage.song_step == -29)
		Animatable_SetAnim(&this->fb_animatable, 3);
	if (stage.song_step == 64)
		Animatable_SetAnim(&this->fb_animatable, 0);
	if (stage.song_step == 80)
		Animatable_SetAnim(&this->fb_animatable, 1);
	if (stage.song_step == 96)
		Animatable_SetAnim(&this->fb_animatable, 2);
	if (stage.song_step == 128)
		Animatable_SetAnim(&this->fb_animatable, 3);
	Animatable_Animate(&this->fb_animatable, (void*)this, Finale_FB_SetFrame);
	if ((stage.song_step >= 63) && (stage.song_step <= 129))
		Finale_FB_Draw(this, FIXED_DEC(-214,1), FIXED_DEC(-120,1));
	
	RECT back2_src = {  0,  0,255,163};
	RECT_FIXED back2_dst = {
		FIXED_DEC(0 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(788 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(504,1)
	};
	
	RECT back5_src = {  0,  0,246,255};
	RECT_FIXED back5_dst = {
		FIXED_DEC(-24 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(113,1) - fy,
		FIXED_DEC(390 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(404,1)
	};
	
	RECT back6_src = {  0,  0,161,255};
	RECT_FIXED back6_dst = {
		FIXED_DEC(-24+485 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(113,1) - fy,
		FIXED_DEC(255 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(404,1)
	};
	
	RECT lamp_src = {  0,  0, 78,255};
	RECT_FIXED lamp_dst = {
		FIXED_DEC(446 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-15,1) - fy,
		FIXED_DEC(85 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(275,1)
	};
	
	RECT splat_src = { 79,  0, 98,250};
	RECT_FIXED splat_dst = {
		FIXED_DEC(379 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(434,1) - fy,
		FIXED_DEC(98 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(250,1)
	};
	
	Debug_StageMoveDebug(&back2_dst, 12, fx, fy);
	Debug_StageMoveDebug(&lamp_dst, 8, fx, fy);
	Debug_StageMoveDebug(&splat_dst, 9, fx, fy);
	Debug_StageMoveDebug(&back5_dst, 10, fx, fy);
	Debug_StageMoveDebug(&back6_dst, 11, fx, fy);
	if (stage.song_step >= 272)
	{
		Stage_BlendTex(&this->tex_back2, &back2_src, &back2_dst, stage.camera.bzoom, 1);
		Stage_DrawTex(&this->tex_back5, &back5_src, &back5_dst, stage.camera.bzoom);
		Stage_DrawTex(&this->tex_back6, &back6_src, &back6_dst, stage.camera.bzoom);
		Stage_DrawTex(&this->tex_back4, &lamp_src, &lamp_dst, stage.camera.bzoom);
		Stage_DrawTexRotate(&this->tex_back4, &splat_src, &splat_dst, stage.camera.bzoom, -64);
	}
}

void Back_Finale_DrawBG(StageBack *back)
{
	Back_Finale *this = (Back_Finale*)back;

	fixed_t fx, fy;

	//Draw finale
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT back0_src = {  0,  0,255,255};
	RECT_FIXED back0_dst = {
		FIXED_DEC(0 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(504 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(504,1)
	};
	
	RECT back1_src = {  0,  0,145,255};
	RECT_FIXED back1_dst = {
		FIXED_DEC(502 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(286 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(504,1)
	};
	RECT back1b_src = {  0,128,145,127};
	RECT_FIXED back1b_dst = {
		FIXED_DEC(502 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(253,1) - fy,
		FIXED_DEC(286 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(251,1)
	};
	
	RECT back3_src = {  0,  0,255, 81};
	RECT_FIXED back3_dst = {
		FIXED_DEC(58 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(225,1) - fy,
		FIXED_DEC(636 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(201,1)
	};
	RECT back3b_src = {128,  0,127, 81};
	RECT_FIXED back3b_dst = {
		FIXED_DEC(377 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(225,1) - fy,
		FIXED_DEC(317 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(201,1)
	};
	
	Debug_StageMoveDebug(&back0_dst, 5, fx, fy);
	Debug_StageMoveDebug(&back1_dst, 6, fx, fy);
	Debug_StageMoveDebug(&back3_dst, 7, fx, fy);
	if (stage.song_step >= 272)
	{
		Stage_DrawTex(&this->tex_back0, &back0_src, &back0_dst, stage.camera.bzoom);
		Stage_DrawTex(&this->tex_back1, &back1_src, &back1_dst, stage.camera.bzoom);
		Stage_DrawTex(&this->tex_back1, &back1b_src, &back1b_dst, stage.camera.bzoom);
	}
	else
	{
		Stage_DrawTex(&this->tex_back3, &back3_src, &back3_dst, stage.camera.bzoom);
		Stage_DrawTex(&this->tex_back3, &back3b_src, &back3b_dst, stage.camera.bzoom);
	}
	
	//Draw black
	RECT screen_src = {0, 0, screen.SCREEN_WIDTH, screen.SCREEN_HEIGHT};
	Gfx_DrawRect(&screen_src, 0, 0, 0);
}

void Back_Finale_Free(StageBack *back)
{
	Back_Finale *this = (Back_Finale*)back;
	
	//Free iconanim archive
	Mem_Free(this->arc_iconanim);
	
	//Free fb archive
	Mem_Free(this->arc_fb);
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_Finale_New(void)
{
	//Allocate background structure
	Back_Finale *this = (Back_Finale*)Mem_Alloc(sizeof(Back_Finale));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = Back_Finale_DrawFG;
	this->back.draw_md = Back_Finale_DrawMG;
	this->back.draw_bg = Back_Finale_DrawBG;
	this->back.free = Back_Finale_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\BG\\FINALE.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_back2, Archive_Find(arc_back, "back2.tim"), 0);
	Gfx_LoadTex(&this->tex_back3, Archive_Find(arc_back, "back3.tim"), 0);
	Gfx_LoadTex(&this->tex_back4, Archive_Find(arc_back, "back4.tim"), 0);
	Gfx_LoadTex(&this->tex_back5, Archive_Find(arc_back, "back5.tim"), 0);
	Gfx_LoadTex(&this->tex_back6, Archive_Find(arc_back, "back6.tim"), 0);
	Mem_Free(arc_back);
	
	//Load iconanim textures
	this->arc_iconanim = IO_Read("\\STAGE\\ICONANIM.ARC;1");
	this->arc_iconanim_ptr[0] = Archive_Find(this->arc_iconanim, "iconanim0.tim");
	this->arc_iconanim_ptr[1] = Archive_Find(this->arc_iconanim, "iconanim1.tim");
	
	//Initialize iconanim state
	Animatable_Init(&this->iconanim_animatable, iconanim_anim);
	Animatable_SetAnim(&this->iconanim_animatable, 0);
	this->iconanim_frame = this->iconanim_tex_id = 0xFF; //Force art load
	
	//Load fb textures
	this->arc_fb = IO_Read("\\BG\\FB.ARC;1");
	this->arc_fb_ptr[0] = Archive_Find(this->arc_fb, "fb0.tim");
	this->arc_fb_ptr[1] = Archive_Find(this->arc_fb, "fb1.tim");
	
	//Initialize fb state
	Animatable_Init(&this->fb_animatable, fb_anim);
	Animatable_SetAnim(&this->fb_animatable, 0);
	this->fb_frame = this->fb_tex_id = 0xFF; //Force art load
	
	//Initialize Fade
	this->fade = FIXED_DEC(255,1);
	this->fadespd = 0;
	
	return (StageBack*)this;
}