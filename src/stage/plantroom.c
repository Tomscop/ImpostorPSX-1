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
int hx1, hx2, hx3, hx4, hx5, hx6, hx7, hx8 = -1;
int hy1, hy2, hy3, hy4, hy5, hy6, hy7, hy8 = 365;

//Plantroom background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	IO_Data arc_grey, arc_grey_ptr[2];
	IO_Data arc_longtomato, arc_longtomato_ptr[4];
	IO_Data arc_snapper, arc_snapper_ptr[2];
	IO_Data arc_cyborg, arc_cyborg_ptr[4];
	Gfx_Tex tex_back0; //back0
	Gfx_Tex tex_back1; //back1
	Gfx_Tex tex_vine0; //vine0
	Gfx_Tex tex_vine1; //vine1
	Gfx_Tex tex_pot; //pot
	
	//Grey state
	Gfx_Tex tex_grey;
	u8 grey_frame, grey_tex_id;
	Animatable grey_animatable;
	
	//LongTomato state
	Gfx_Tex tex_longtomato;
	u8 longtomato_frame, longtomato_tex_id;
	Animatable longtomato_animatable;
	
	//Snapper state
	Gfx_Tex tex_snapper;
	u8 snapper_frame, snapper_tex_id;
	Animatable snapper_animatable;
	
	//Cyborg state
	Gfx_Tex tex_cyborg;
	u8 cyborg_frame, cyborg_tex_id;
	Animatable cyborg_animatable;

} Back_Plantroom;

//Grey animation and rects
static const CharFrame grey_frame[] = {
  {0, {  0,  0, 75,123}, {160,157}}, //0 grey 1
  {0, { 75,  0, 74,124}, {159,158}}, //1 grey 2
  {0, {149,  0, 72,124}, {157,158}}, //2 grey 3
  {0, {  0,124, 73,125}, {158,159}}, //3 grey 4
  {0, { 73,124, 73,126}, {159,160}}, //4 grey 5
  {0, {146,124, 73,126}, {159,160}}, //5 grey 6
  {1, {  0,  0, 73,126}, {159,160}}, //6 grey 7
};

static const Animation grey_anim[] = {
	{2, (const u8[]){0, 1, 2, 3, 4, 5, 6, ASCR_BACK, 1}}, //Idle
};

//Grey functions
void Plantroom_Grey_SetFrame(void *user, u8 frame)
{
	Back_Plantroom *this = (Back_Plantroom*)user;
	
	//Check if this is a new frame
	if (frame != this->grey_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &grey_frame[this->grey_frame = frame];
		if (cframe->tex != this->grey_tex_id)
			Gfx_LoadTex(&this->tex_grey, this->arc_grey_ptr[this->grey_tex_id = cframe->tex], 0);
	}
}

void Plantroom_Grey_Draw(Back_Plantroom *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &grey_frame[this->grey_frame];
    
    fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = { ox, oy, src.w << FIXED_SHIFT ,src.h<< FIXED_SHIFT };
	Debug_StageMoveDebug(&dst, 7, stage.camera.x, stage.camera.y);
	Stage_DrawTex(&this->tex_grey, &src, &dst, stage.camera.bzoom);
}

//LongTomato animation and rects
static const CharFrame longtomato_frame[] = {
  {0, {  0,  0,121,134}, {159,158}}, //0 longtoma 1
  {0, {121,  0,121,133}, {159,157}}, //1 longtoma 2
  {1, {  0,  0,120,134}, {159,158}}, //2 longtoma 3
  {1, {120,  0,119,135}, {157,159}}, //3 longtoma 4
  {2, {  0,  0,118,136}, {158,160}}, //4 longtoma 5
  {2, {118,  0,117,136}, {156,160}}, //5 longtoma 6
  {3, {  0,  0,118,136}, {156,160}}, //6 longtoma 7
};

static const Animation longtomato_anim[] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, ASCR_BACK, 1}}, //Idle
};

//LongTomato functions
void Plantroom_LongTomato_SetFrame(void *user, u8 frame)
{
	Back_Plantroom *this = (Back_Plantroom*)user;
	
	//Check if this is a new frame
	if (frame != this->longtomato_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &longtomato_frame[this->longtomato_frame = frame];
		if (cframe->tex != this->longtomato_tex_id)
			Gfx_LoadTex(&this->tex_longtomato, this->arc_longtomato_ptr[this->longtomato_tex_id = cframe->tex], 0);
	}
}

void Plantroom_LongTomato_Draw(Back_Plantroom *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &longtomato_frame[this->longtomato_frame];
    
    fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = { ox, oy, src.w << FIXED_SHIFT ,src.h<< FIXED_SHIFT };
	Debug_StageMoveDebug(&dst, 8, stage.camera.x, stage.camera.y);
	Stage_DrawTex(&this->tex_longtomato, &src, &dst, stage.camera.bzoom);
}

//Snapper animation and rects
static const CharFrame snapper_frame[] = {
  {0, {  0,  0,139,116}, {160,155}}, //0 snapper 1
  {0, {  0,116,138,116}, {160,155}}, //1 snapper 2
  {1, {  0,  0,133,115}, {160,156}}, //2 snapper 3
  {1, {133,  0,117,118}, {160,159}}, //3 snapper 4
  {1, {  0,118,115,119}, {159,160}}, //4 snapper 5
};

static const Animation snapper_anim[] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, ASCR_BACK, 1}}, //Idle
};

//Snapper functions
void Plantroom_Snapper_SetFrame(void *user, u8 frame)
{
	Back_Plantroom *this = (Back_Plantroom*)user;
	
	//Check if this is a new frame
	if (frame != this->snapper_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &snapper_frame[this->snapper_frame = frame];
		if (cframe->tex != this->snapper_tex_id)
			Gfx_LoadTex(&this->tex_snapper, this->arc_snapper_ptr[this->snapper_tex_id = cframe->tex], 0);
	}
}

void Plantroom_Snapper_Draw(Back_Plantroom *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &snapper_frame[this->snapper_frame];
    
    fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = { ox, oy, src.w * FIXED_DEC(1667,1000),src.h * FIXED_DEC(1667,1000)};
	Debug_StageMoveDebug(&dst, 9, stage.camera.x, stage.camera.y);
	Stage_DrawTex(&this->tex_snapper, &src, &dst, stage.camera.bzoom);
}

//Cyborg animation and rects
static const CharFrame cyborg_frame[] = {
  {0, {  0,  0, 98,147}, {149,157}}, //0 cyborg 1
  {0, { 98,  0, 99,148}, {151,156}}, //1 cyborg 2
  {1, {  0,  0, 98,147}, {150,157}}, //2 cyborg 3
  {1, { 98,  0, 97,147}, {150,159}}, //3 cyborg 4
  {2, {  0,  0, 98,148}, {150,160}}, //4 cyborg 5
  {2, { 98,  0, 97,148}, {149,160}}, //5 cyborg 6
  {3, {  0,  0, 98,148}, {150,160}}, //6 cyborg 7
};

static const Animation cyborg_anim[] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, ASCR_BACK, 1}}, //Idle
};

//Cyborg functions
void Plantroom_Cyborg_SetFrame(void *user, u8 frame)
{
	Back_Plantroom *this = (Back_Plantroom*)user;
	
	//Check if this is a new frame
	if (frame != this->cyborg_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &cyborg_frame[this->cyborg_frame = frame];
		if (cframe->tex != this->cyborg_tex_id)
			Gfx_LoadTex(&this->tex_cyborg, this->arc_cyborg_ptr[this->cyborg_tex_id = cframe->tex], 0);
	}
}

void Plantroom_Cyborg_Draw(Back_Plantroom *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &cyborg_frame[this->cyborg_frame];
    
    fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = { ox, oy, src.w * FIXED_DEC(1667,1000), src.h * FIXED_DEC(1667,1000)};
	Debug_StageMoveDebug(&dst, 10, stage.camera.x, stage.camera.y);
	Stage_DrawTex(&this->tex_cyborg, &src, &dst, stage.camera.bzoom);
}

void Back_Plantroom_DrawFG(StageBack *back)
{
	Back_Plantroom *this = (Back_Plantroom*)back;
	
	fixed_t fx, fy;

	//Draw plantroom
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	//Heart things
	RECT heart11_src = {  0, 93, 43, 35};
	RECT heart12_src = { 44, 93, 36, 35};
	RECT heart21_src = {  0,129, 43, 35};
	RECT heart22_src = { 44,129, 36, 34};
	RECT_FIXED heart11_dst = {FIXED_DEC(-159,1),FIXED_DEC(40,1),FIXED_DEC(65 + screen.SCREEN_WIDEOADD,1),FIXED_DEC(53,1)};
	RECT_FIXED heart12_dst = {FIXED_DEC(105,1),FIXED_DEC(40-1,1),FIXED_DEC(54 + screen.SCREEN_WIDEOADD,1),FIXED_DEC(53,1)};
	RECT_FIXED heart21_dst = {FIXED_DEC(-159,1),FIXED_DEC(40,1),FIXED_DEC(65 + screen.SCREEN_WIDEOADD,1),FIXED_DEC(53,1)};
	RECT_FIXED heart22_dst = {FIXED_DEC(105,1),FIXED_DEC(40,1),FIXED_DEC(54 + screen.SCREEN_WIDEOADD,1),FIXED_DEC(51,1)};
	
	if (stage.pinkstuff)
	{
	if (((stage.song_step & 0x3 )== 0) || ((stage.song_step & 0x3 )== 2))
	{
		Stage_DrawTex(&this->tex_pot, &heart11_src, &heart11_dst, FIXED_DEC(1,1));
		Stage_DrawTex(&this->tex_pot, &heart12_src, &heart12_dst, FIXED_DEC(1,1));
	}
	else if (((stage.song_step & 0x3 )== 1) || ((stage.song_step & 0x3 )== 3))
	{
		Stage_DrawTex(&this->tex_pot, &heart21_src, &heart21_dst, FIXED_DEC(1,1));
		Stage_DrawTex(&this->tex_pot, &heart22_src, &heart22_dst, FIXED_DEC(1,1));
	}
	}
	Debug_StageMoveDebug(&heart11_dst, 14, heart11_dst.x, heart11_dst.y);
	Debug_StageMoveDebug(&heart12_dst, 15, heart12_dst.x, heart12_dst.y);
	Debug_StageMoveDebug(&heart21_dst, 14, heart21_dst.x, heart21_dst.y);
	Debug_StageMoveDebug(&heart22_dst, 15, heart22_dst.x, heart22_dst.y);
	
	//Moving hearts
	RECT dot_src = { 81, 93, 11, 11};
	RECT_FIXED dot1_dst = {FIXED_DEC(hx1,1) - fx,FIXED_DEC(hy1,1) - fy,FIXED_DEC(17 + screen.SCREEN_WIDEOADD,1),FIXED_DEC(17,1)};
	RECT_FIXED dot2_dst = {FIXED_DEC(hx2,1) - fx,FIXED_DEC(hy2,1) - fy,FIXED_DEC(17 + screen.SCREEN_WIDEOADD,1),FIXED_DEC(17,1)};
	RECT_FIXED dot3_dst = {FIXED_DEC(hx3,1) - fx,FIXED_DEC(hy3,1) - fy,FIXED_DEC(17 + screen.SCREEN_WIDEOADD,1),FIXED_DEC(17,1)};
	RECT_FIXED dot4_dst = {FIXED_DEC(hx4,1) - fx,FIXED_DEC(hy4,1) - fy,FIXED_DEC(17 + screen.SCREEN_WIDEOADD,1),FIXED_DEC(17,1)};
	RECT_FIXED dot5_dst = {FIXED_DEC(hx5,1) - fx,FIXED_DEC(hy5,1) - fy,FIXED_DEC(17 + screen.SCREEN_WIDEOADD,1),FIXED_DEC(17,1)};
	RECT_FIXED dot6_dst = {FIXED_DEC(hx6,1) - fx,FIXED_DEC(hy6,1) - fy,FIXED_DEC(17 + screen.SCREEN_WIDEOADD,1),FIXED_DEC(17,1)};
	RECT_FIXED dot7_dst = {FIXED_DEC(hx7,1) - fx,FIXED_DEC(hy7,1) - fy,FIXED_DEC(17 + screen.SCREEN_WIDEOADD,1),FIXED_DEC(17,1)};
	RECT_FIXED dot8_dst = {FIXED_DEC(hx8,1) - fx,FIXED_DEC(hy8,1) - fy,FIXED_DEC(17 + screen.SCREEN_WIDEOADD,1),FIXED_DEC(17,1)};
	
	if (stage.pinkstuff)
	{
		Stage_DrawTex(&this->tex_pot, &dot_src, &dot1_dst, stage.camera.bzoom);
		Stage_DrawTex(&this->tex_pot, &dot_src, &dot2_dst, stage.camera.bzoom);
		Stage_DrawTex(&this->tex_pot, &dot_src, &dot3_dst, stage.camera.bzoom);
		Stage_DrawTex(&this->tex_pot, &dot_src, &dot4_dst, stage.camera.bzoom);
		Stage_DrawTex(&this->tex_pot, &dot_src, &dot5_dst, stage.camera.bzoom);
		Stage_DrawTex(&this->tex_pot, &dot_src, &dot6_dst, stage.camera.bzoom);
		Stage_DrawTex(&this->tex_pot, &dot_src, &dot7_dst, stage.camera.bzoom);
		Stage_DrawTex(&this->tex_pot, &dot_src, &dot8_dst, stage.camera.bzoom);
	}
	
	if (stage.paused == false)
	{
	if (hy1 <= -20)
	{
		hx1 = RandomRange(83,612);
		hy1 = 365;
	}
	if (hy2 <= -20)
	{
		hx2 = RandomRange(83,612);
		hy2 = 365;
	}
	if (hy3 <= -20)
	{
		hx3 = RandomRange(83,612);
		hy3 = 365;
	}
	if (hy4 <= -20)
	{
		hx4 = RandomRange(83,612);
		hy4 = 365;
	}
	if (hy5 <= -20)
	{
		hx5 = RandomRange(83,612);
		hy5 = 365;
	}
	if (hy6 <= -20)
	{
		hx6 = RandomRange(83,612);
		hy6 = 365;
	}
	if (hy7 <= -20)
	{
		hx7 = RandomRange(83,612);
		hy7 = 365;
	}
	if (hy8 <= -20)
	{
		hx8 = RandomRange(83,612);
		hy8 = 365;
	}
	if (stage.pinkstuff)
	{
		hy1 -= RandomRange(1,3);
		hy2 -= RandomRange(1,3);
		hy3 -= RandomRange(1,3);
		hy4 -= RandomRange(1,3);
		hy5 -= RandomRange(1,3);
		hy6 -= RandomRange(1,3);
		hy7 -= RandomRange(1,3);
		hy8 -= RandomRange(1,3);
	}
	}
	
	//Pink filter thing
	RECT screen_src = {0, 0, screen.SCREEN_WIDTH, screen.SCREEN_HEIGHT};
	if (stage.pinkstuff)
	{
		if ((stage.song_step & 0x7 )== 4)
			Gfx_BlendRect(&screen_src, 164, 58, 119, 1);
		else if ((stage.song_step & 0x7 )== 5)
			Gfx_BlendRect(&screen_src, 70, 25, 51, 1);
		else
			Gfx_BlendRect(&screen_src, 35, 12, 26, 1);
	}
	if (stage.pink == 2)
	{
		Gfx_BlendRect(&screen_src, 164, 58, 119, 1);
		if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step & 0x3)== 0)
			stage.pink = 0;
	}
	
	fx = (stage.camera.x * 7) / 5;
	RECT vine0_src = {  0,  0,171,175};
	RECT_FIXED vine0_dst = {
		FIXED_DEC(174 + vine - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(171 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(175,1)
	};
	
	RECT vine1_src = {  0,  0,189,203};
	RECT_FIXED vine1_dst = {
		FIXED_DEC(632 + vine - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(189 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(203,1)
	};
	
	fx = (stage.camera.x * 6) / 5;
	RECT pot_src = {  0,  0,239,91};
	RECT_FIXED pot_dst = {
		FIXED_DEC(-14 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(296,1) - fy,
		FIXED_DEC(239 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(91,1)
	};
	
	Debug_StageMoveDebug(&vine0_dst, 11, fx, fy);
	Debug_StageMoveDebug(&vine1_dst, 12, fx, fy);
	Debug_StageMoveDebug(&pot_dst, 13, fx, fy);
	Stage_DrawTex(&this->tex_vine0, &vine0_src, &vine0_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_vine1, &vine1_src, &vine1_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_pot, &pot_src, &pot_dst, stage.camera.bzoom);
	
	//Animate and draw snapper
	if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step & 0x3) == 0)
		Animatable_SetAnim(&this->snapper_animatable, 0);
	Animatable_Animate(&this->snapper_animatable, (void*)this, Plantroom_Snapper_SetFrame);
	Plantroom_Snapper_Draw(this, FIXED_DEC(65 + 159,1) - fx, FIXED_DEC(134 + 160,1) - fy);
	
	//Animate and draw cyborg
	if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step & 0x7 )== 0)
		Animatable_SetAnim(&this->cyborg_animatable, 0);
	Animatable_Animate(&this->cyborg_animatable, (void*)this, Plantroom_Cyborg_SetFrame);
	Plantroom_Cyborg_Draw(this, FIXED_DEC(654 + 150,1) - fx, FIXED_DEC(166 + 160,1) - fy);
	
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
	
	//Animate and draw grey
	if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step & 0x7) == 0)
		Animatable_SetAnim(&this->grey_animatable, 0);
	Animatable_Animate(&this->grey_animatable, (void*)this, Plantroom_Grey_SetFrame);
	Plantroom_Grey_Draw(this, FIXED_DEC(233 + 159,1) - fx, FIXED_DEC(113 + 160,1) - fy);
	
	//Animate and draw longtomato
	if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step & 0x7 )== 0)
		Animatable_SetAnim(&this->longtomato_animatable, 0);
	Animatable_Animate(&this->longtomato_animatable, (void*)this, Plantroom_LongTomato_SetFrame);
	Plantroom_LongTomato_Draw(this, FIXED_DEC(484 + 156,1) - fx, FIXED_DEC(121 + 160,1) - fy);
	
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
	
	//Free grey archive
	Mem_Free(this->arc_grey);
	
	//Free longtomato archive
	Mem_Free(this->arc_longtomato);
	
	//Free snapper archive
	Mem_Free(this->arc_snapper);
	
	//Free cyborg archive
	Mem_Free(this->arc_cyborg);
	
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
	
	//Load grey textures
	this->arc_grey = IO_Read("\\BG\\GREYP.ARC;1");
	this->arc_grey_ptr[0] = Archive_Find(this->arc_grey, "grey0.tim");
	this->arc_grey_ptr[1] = Archive_Find(this->arc_grey, "grey1.tim");
	
	//Load longtomato textures
	this->arc_longtomato = IO_Read("\\BG\\LONGTOMA.ARC;1");
	this->arc_longtomato_ptr[0] = Archive_Find(this->arc_longtomato, "longtoma0.tim");
	this->arc_longtomato_ptr[1] = Archive_Find(this->arc_longtomato, "longtoma1.tim");
	this->arc_longtomato_ptr[2] = Archive_Find(this->arc_longtomato, "longtoma2.tim");
	this->arc_longtomato_ptr[3] = Archive_Find(this->arc_longtomato, "longtoma3.tim");
	
	//Load snapper textures
	this->arc_snapper = IO_Read("\\BG\\SNAPPER.ARC;1");
	this->arc_snapper_ptr[0] = Archive_Find(this->arc_snapper, "snapper0.tim");
	this->arc_snapper_ptr[1] = Archive_Find(this->arc_snapper, "snapper1.tim");
	
	//Load cyborg textures
	this->arc_cyborg = IO_Read("\\BG\\CYBORG.ARC;1");
	this->arc_cyborg_ptr[0] = Archive_Find(this->arc_cyborg, "cyborg0.tim");
	this->arc_cyborg_ptr[1] = Archive_Find(this->arc_cyborg, "cyborg1.tim");
	this->arc_cyborg_ptr[2] = Archive_Find(this->arc_cyborg, "cyborg2.tim");
	this->arc_cyborg_ptr[3] = Archive_Find(this->arc_cyborg, "cyborg3.tim");
	
	//Initialize grey state
	Animatable_Init(&this->grey_animatable, grey_anim);
	Animatable_SetAnim(&this->grey_animatable, 0);
	this->grey_frame = this->grey_tex_id = 0xFF; //Force art load
	
	//Initialize longtomato state
	Animatable_Init(&this->longtomato_animatable, longtomato_anim);
	Animatable_SetAnim(&this->longtomato_animatable, 0);
	this->longtomato_frame = this->longtomato_tex_id = 0xFF; //Force art load
	
	//Initialize snapper state
	Animatable_Init(&this->snapper_animatable, snapper_anim);
	Animatable_SetAnim(&this->snapper_animatable, 0);
	this->snapper_frame = this->snapper_tex_id = 0xFF; //Force art load
	
	//Initialize cyborg state
	Animatable_Init(&this->cyborg_animatable, cyborg_anim);
	Animatable_SetAnim(&this->cyborg_animatable, 0);
	this->cyborg_frame = this->cyborg_tex_id = 0xFF; //Force art load
	
	hx1 = RandomRange(83,612);
	hx2 = RandomRange(83,612);
	hx3 = RandomRange(83,612);
	hx4 = RandomRange(83,612);
	hx5 = RandomRange(83,612);
	hx6 = RandomRange(83,612);
	hx7 = RandomRange(83,612);
	hx8 = RandomRange(83,612);
	hy1 = RandomRange(365,400);
	hy2 = RandomRange(365,400);
	hy3 = RandomRange(365,400);
	hy4 = RandomRange(365,400);
	hy5 = RandomRange(365,400);
	hy6 = RandomRange(365,400);
	hy7 = RandomRange(365,400);
	hy8 = RandomRange(365,400);
	
	return (StageBack*)this;
}
