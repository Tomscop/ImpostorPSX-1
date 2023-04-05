/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "mira.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

int powersx = -115;
int powersmove = 0;
//Mira background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0; //back0
	Gfx_Tex tex_back1; //back1
	Gfx_Tex tex_back2; //back2
	Gfx_Tex tex_back3; //back3
	
	IO_Data arc_powers, arc_powers_ptr[10];
	
	//Powers state
	Gfx_Tex tex_powers;
	u8 powers_frame, powers_tex_id;
	Animatable powers_animatable;
	
	//fade stuff
	fixed_t fade, fadespd;
	
} Back_Mira;

//Powers animation and rects
static const CharFrame powers_frame[] = {
  {0, {  0,  0, 73, 85}, {159,124}}, //0 powers 1
  {0, { 73,  0, 73, 85}, {157,125}}, //1 powers 2
  {0, {146,  0, 76, 85}, {149,135}}, //2 powers 3
  {0, {  0, 85, 79, 85}, {141,141}}, //3 powers 4
  {0, { 79, 85, 91, 85}, {141,153}}, //4 powers 5
  {0, {  0,170, 91, 85}, {141,154}}, //5 powers 6
  {0, { 91,170, 91, 85}, {141,154}}, //6 powers 7
  {1, {  0,  0, 91, 85}, {140,154}}, //7 powers 8
  {1, { 91,  0, 91, 85}, {142,153}}, //8 powers 9
  {1, {  0, 85, 95, 85}, {148,147}}, //9 powers 10
  {1, { 95, 85, 95, 85}, {147,148}}, //10 powers 11
  {1, {  0,170, 95, 85}, {147,148}}, //11 powers 12
  {1, { 95,170, 91, 85}, {140,152}}, //12 powers 13
  {2, {  0,  0, 80, 85}, {124,154}}, //13 powers 14
  {2, { 80,  0, 79, 85}, {125,154}}, //14 powers 15
  {2, {159,  0, 80, 85}, {125,154}}, //15 powers 16
  {2, {  0, 85, 89, 85}, {129,157}}, //16 powers 17
  {2, { 89, 85, 88, 85}, {129,157}}, //17 powers 18
  {2, {  0,170, 88, 85}, {129,157}}, //18 powers 19
  {2, { 88,170, 90, 85}, {132,153}}, //19 powers 20
  {3, {  0,  0, 91, 85}, {133,153}}, //20 powers 21
  {3, { 91,  0, 78, 85}, {126,157}}, //21 powers 22
  {3, {169,  0, 79, 85}, {127,156}}, //22 powers 23
  {3, {  0, 85, 87, 85}, {129,160}}, //23 powers 24
  {3, { 87, 85, 75, 85}, {125,157}}, //24 powers 25
  {3, {162, 85, 86, 85}, {128,150}}, //25 powers 26
  {3, {  0,170, 74, 85}, {124,150}}, //26 powers 27
  {3, { 74,170, 75, 85}, {124,150}}, //27 powers 28
  {3, {149,170, 78, 85}, {126,149}}, //28 powers 29
  {4, {  0,  0, 78, 85}, {126,149}}, //29 powers 30
  {4, { 78,  0, 77, 85}, {126,150}}, //30 powers 31
  {4, {155,  0, 93, 85}, {129,155}}, //31 powers 32
  {4, {  0, 85, 91, 85}, {128,155}}, //32 powers 33
  {4, { 91, 85, 94, 85}, {131,153}}, //33 powers 34
  {4, {  0,170, 93, 85}, {131,153}}, //34 powers 35
  {4, { 93,170, 95, 85}, {131,151}}, //35 powers 36
  {5, {  0,  0, 73, 85}, {122,150}}, //36 powers 37
  {5, { 73,  0, 74, 85}, {122,150}}, //37 powers 38
  {5, {147,  0, 74, 85}, {121,152}}, //38 powers 39
  {5, {  0, 85, 90, 85}, {124,155}}, //39 powers 40
  {5, { 90, 85, 91, 85}, {125,155}}, //40 powers 41
  {5, {  0,170, 93, 85}, {126,149}}, //41 powers 42
  {5, { 93,170, 92, 85}, {125,150}}, //42 powers 43
  {6, {  0,  0, 94, 85}, {126,147}}, //43 powers 44
  {6, { 94,  0, 71, 85}, {115,151}}, //44 powers 45
  {6, {165,  0, 70, 85}, {115,151}}, //45 powers 46
  {6, {  0, 85, 72, 85}, {114,152}}, //46 powers 47
  {6, { 72, 85, 77, 85}, {115,155}}, //47 powers 48
  {6, {149, 85, 81, 85}, {114,156}}, //48 powers 49
  {6, {  0,170, 82, 85}, {113,156}}, //49 powers 50
  {6, { 82,170, 86, 85}, {118,155}}, //50 powers 51
  {7, {  0,  0, 94, 85}, {131,149}}, //51 powers 52
  {7, { 94,  0, 94, 85}, {130,150}}, //52 powers 53
  {7, {  0, 85, 97, 85}, {129,152}}, //53 powers 54
  {7, { 97, 85, 97, 85}, {129,152}}, //54 powers 55
  {7, {  0,170, 98, 85}, {130,150}}, //55 powers 56
  {7, { 98,170, 98, 85}, {130,150}}, //56 powers 57
  {8, {  0,  0, 95, 85}, {126,152}}, //57 powers 58
  {8, { 95,  0, 97, 85}, {128,150}}, //58 powers 59
  {8, {  0, 85, 94, 85}, {123,151}}, //59 powers 60
  {8, { 94, 85, 94, 85}, {123,152}}, //60 powers 61
  {8, {  0,170, 94, 85}, {123,152}}, //61 powers 62
  {8, { 94,170, 84, 85}, {126,150}}, //62 powers 63
  {9, {  0,  0, 83, 85}, {125,151}}, //63 powers 64
  {9, {  0,  0,  1,  1}, {125,151}}, //64 hide
};

static const Animation powers_anim[] = {
	{1, (const u8[]){ 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 15, 15, 15, 15, 16, 16, 17, 17, 18, 18, 18, 18, 18, 19, 19, 20, 20, 20, 20, 20, 20, 20, 20, 21, 21, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 24, 24, 24, 25, 25, 25, 25, 26, 26, 27, 27, 27, 27, 27, 27, 28, 28, 29, 29, 29, 29, 29, 30, 30, 31, 31, 32, 32, 32, 32, 32, 32, 32, 33, 33, 34, 34, 34, 34, 34, 34, 35, 35, 36, 36, 
	37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 38, 38, 38, 38, 38, 38, 39, 39, 40, 40, 40, 40, 40, 40, 40, 40, 40, 41, 41, 42, 42, 42, 42, 42, 42, 43, 43, 44, 44, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 46, 46, 47, 47, 48, 48, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 50, 50, 51, 51, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 53, 53, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54,
	55, 55, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 58, 58, 58, 58, 58, 58, 58, 58, 59, 59, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 61, 61, 62, 62, 
	63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, ASCR_CHGANI, 1}}, //Idle															
	{2, (const u8[]){ 64, ASCR_BACK, 1}}, //Hide
};

//Powers functions
void Mira_Powers_SetFrame(void *user, u8 frame)
{
	Back_Mira *this = (Back_Mira*)user;
	
	//Check if this is a new frame
	if (frame != this->powers_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &powers_frame[this->powers_frame = frame];
		if (cframe->tex != this->powers_tex_id)
			Gfx_LoadTex(&this->tex_powers, this->arc_powers_ptr[this->powers_tex_id = cframe->tex], 0);
	}
}

void Mira_Powers_Draw(Back_Mira *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &powers_frame[this->powers_frame];
    
    fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = { ox, oy, src.w * FIXED_DEC(1667,1000), src.h * FIXED_DEC(1667,1000)};
	Debug_StageMoveDebug(&dst, 11, stage.camera.x, stage.camera.y);
	Stage_DrawTex(&this->tex_powers, &src, &dst, stage.camera.bzoom);
}

void Back_Mira_DrawFG(StageBack *back)
{
	Back_Mira *this = (Back_Mira*)back;
	
	fixed_t fx, fy;

	//Draw mira
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	//start fade
	if (stage.song_step == 0)
		this->fade = FIXED_DEC(0,1);
	if (stage.lights == 2)
	{
		this->fade = FIXED_DEC(255,1);
		this->fadespd = FIXED_DEC(768,1);
		stage.lights = 0;
	}
	
	//end fade
	if (this->fade == 0)
		this->fade = 0;
	
	if (this->fade > 0)
	{
		RECT flash = {0, 0, screen.SCREEN_WIDTH, screen.SCREEN_HEIGHT};
		u8 flash_col = this->fade >> FIXED_SHIFT;
		Gfx_BlendRect(&flash, flash_col, flash_col, flash_col, 2);
		this->fade -= FIXED_MUL(this->fadespd, timer_dt);
	}
	
	if (stage.stage_id == StageId_SussusToogus)
	{
		//Animate and draw powers
		if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step == -29))
			Animatable_SetAnim(&this->powers_animatable, 1);
		if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step == 896))
			Animatable_SetAnim(&this->powers_animatable, 0);
	
		Animatable_Animate(&this->powers_animatable, (void*)this, Mira_Powers_SetFrame);
		Mira_Powers_Draw(this, FIXED_DEC(powersx+159,1) - fx, FIXED_DEC(70+150,1) - fy);
		
		if (stage.paused == false)
		{
			if (powersx >= 502)
				powersx = 500;
			else if (stage.song_step >= 896)
				powersmove += 1;
			else if (stage.song_step <= 895)
				powersx = -115;
		
			if (powersmove == 2)
			{
				powersx += 1;
				powersmove = 0;
			}
		}
	}
}

void Back_Mira_DrawBG(StageBack *back)
{
	Back_Mira *this = (Back_Mira*)back;

	fixed_t fx, fy;

	//Draw mira
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT back0_src = {  0,  0,255,201};
	RECT_FIXED back0_dst = {
		FIXED_DEC(0 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(255 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(201,1)
	};
	
	RECT back1_src = {  0,  0,208,201};
	RECT_FIXED back1_dst = {
		FIXED_DEC(254 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(208 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(201,1)
	};
	
	RECT bench0_src = {  0,  0,161, 77};
	RECT_FIXED bench0_dst = {
		FIXED_DEC(0 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(76,1) - fy,
		FIXED_DEC(161 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(77,1)
	};
	
	RECT bench1_src = {  0, 78,188, 78};
	RECT_FIXED bench1_dst = {
		FIXED_DEC(204 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(74,1) - fy,
		FIXED_DEC(188 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(78,1)
	};
	
	RECT speaker_src = {  0,  0,183, 88};
	RECT_FIXED speaker_dst = {FIXED_DEC(158 - screen.SCREEN_WIDEOADD2,1) - fx, FIXED_DEC(86,1) - fy, FIXED_DEC(183 + screen.SCREEN_WIDEOADD,1), FIXED_DEC(88,1)};
	
	RECT bf0_src = {  0, 89, 44, 55};
	RECT_FIXED bf0_dst = {FIXED_DEC(77 - screen.SCREEN_WIDEOADD2,1) - fx, FIXED_DEC(33,1) - fy, FIXED_DEC(44 + screen.SCREEN_WIDEOADD,1), FIXED_DEC(55,1)};
	
	RECT bf1_src = {  45, 89, 44, 55};
	RECT_FIXED bf1_dst = {FIXED_DEC(76 - screen.SCREEN_WIDEOADD2,1) - fx, FIXED_DEC(33,1) - fy, FIXED_DEC(44 + screen.SCREEN_WIDEOADD,1), FIXED_DEC(55,1)};
	
	//Draw black
	RECT screen_src = {0, 0, screen.SCREEN_WIDTH, screen.SCREEN_HEIGHT};
	if ((stage.lights == 1) && (stage.song_step <= 1599))
		Gfx_DrawRect(&screen_src, 0, 0, 0);
	
	Debug_StageMoveDebug(&back0_dst, 5, fx, fy);
	Debug_StageMoveDebug(&back1_dst, 6, fx, fy);
	Debug_StageMoveDebug(&bench0_dst, 7, fx, fy);
	Debug_StageMoveDebug(&bench1_dst, 8, fx, fy);
	Debug_StageMoveDebug(&speaker_dst, 9, fx, fy);
	Debug_StageMoveDebug(&bf0_dst, 10, fx, fy);
	Debug_StageMoveDebug(&bf1_dst, 10, fx, fy);
	if ((stage.stage_id == StageId_LightsDown) && (stage.song_step >= 1600))
		Stage_DrawTex(&this->tex_back3, &speaker_src, &speaker_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back2, &bench0_src, &bench0_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back2, &bench1_src, &bench1_dst, stage.camera.bzoom);
	if ((stage.stage_id == StageId_LightsDown) && (stage.song_step == 1600))
		Stage_DrawTex(&this->tex_back3, &bf0_src, &bf0_dst, stage.camera.bzoom);
	if ((stage.stage_id == StageId_LightsDown) && (stage.song_step == 1601))
		Stage_DrawTex(&this->tex_back3, &bf1_src, &bf1_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back0, &back0_src, &back0_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back1, &back1_src, &back1_dst, stage.camera.bzoom);
}

void Back_Mira_Free(StageBack *back)
{
	Back_Mira *this = (Back_Mira*)back;
	
	//Free structure
	Mem_Free(this);
	
	//Free powers archive
	Mem_Free(this->arc_powers);
}

StageBack *Back_Mira_New(void)
{
	//Allocate background structure
	Back_Mira *this = (Back_Mira*)Mem_Alloc(sizeof(Back_Mira));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = Back_Mira_DrawFG;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Mira_DrawBG;
	this->back.free = Back_Mira_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\BG\\MIRA.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_back2, Archive_Find(arc_back, "back2.tim"), 0);
	Gfx_LoadTex(&this->tex_back3, Archive_Find(arc_back, "back3.tim"), 0);
	Mem_Free(arc_back);
	
	if (stage.stage_id == StageId_SussusToogus)
	{
	//Load powers textures
	this->arc_powers = IO_Read("\\BG\\POWERSS.ARC;1");
	this->arc_powers_ptr[0] = Archive_Find(this->arc_powers, "powers0.tim");
	this->arc_powers_ptr[1] = Archive_Find(this->arc_powers, "powers1.tim");
	this->arc_powers_ptr[2] = Archive_Find(this->arc_powers, "powers2.tim");
	this->arc_powers_ptr[3] = Archive_Find(this->arc_powers, "powers3.tim");
	this->arc_powers_ptr[4] = Archive_Find(this->arc_powers, "powers4.tim");
	this->arc_powers_ptr[5] = Archive_Find(this->arc_powers, "powers5.tim");
	this->arc_powers_ptr[6] = Archive_Find(this->arc_powers, "powers6.tim");
	this->arc_powers_ptr[7] = Archive_Find(this->arc_powers, "powers7.tim");
	this->arc_powers_ptr[8] = Archive_Find(this->arc_powers, "powers8.tim");
	this->arc_powers_ptr[9] = Archive_Find(this->arc_powers, "powers9.tim");
	}
	
	//Initialize powers state
	Animatable_Init(&this->powers_animatable, powers_anim);
	Animatable_SetAnim(&this->powers_animatable, 0);
	this->powers_frame = this->powers_tex_id = 0xFF; //Force art load
	
	//Initialize Fade
	this->fade = FIXED_DEC(0,1);
	
	return (StageBack*)this;
}
