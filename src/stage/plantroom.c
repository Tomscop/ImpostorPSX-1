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
	IO_Data arc_grey, arc_grey_ptr[2];
	IO_Data arc_longtomato, arc_longtomato_ptr[4];
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
	
	//Animate and draw grey
	if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step & 0x7) == 0)
		Animatable_SetAnim(&this->grey_animatable, 0);
	Animatable_Animate(&this->grey_animatable, (void*)this, Plantroom_Grey_SetFrame);
	Plantroom_Grey_Draw(this, FIXED_DEC(0 - 159,1) - fx, FIXED_DEC(0 + 160,1) - fy);
	
	//Animate and draw longtomato
	if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step & 0x7 )== 0)
		Animatable_SetAnim(&this->longtomato_animatable, 0);
	Animatable_Animate(&this->longtomato_animatable, (void*)this, Plantroom_LongTomato_SetFrame);
	Plantroom_LongTomato_Draw(this, FIXED_DEC(0 - 156,1) - fx, FIXED_DEC(0 + 160,1) - fy);
	
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
	
	//Initialize grey state
	Animatable_Init(&this->grey_animatable, grey_anim);
	Animatable_SetAnim(&this->grey_animatable, 0);
	this->grey_frame = this->grey_tex_id = 0xFF; //Force art load
	
	//Initialize longtomato state
	Animatable_Init(&this->longtomato_animatable, longtomato_anim);
	Animatable_SetAnim(&this->longtomato_animatable, 0);
	this->longtomato_frame = this->longtomato_tex_id = 0xFF; //Force art load
	
	return (StageBack*)this;
}
