/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "kitchen.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

//Kitchen background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	IO_Data arc_grey, arc_grey_ptr[1];
	IO_Data arc_saster, arc_saster_ptr[2];
	Gfx_Tex tex_back0; //back0
	Gfx_Tex tex_back1; //back1
	Gfx_Tex tex_back2; //back2
	
	//Grey state
	Gfx_Tex tex_grey;
	u8 grey_frame, grey_tex_id;
	Animatable grey_animatable;
	
	//Saster state
	Gfx_Tex tex_saster;
	u8 saster_frame, saster_tex_id;
	Animatable saster_animatable;

} Back_Kitchen;

//Grey animation and rects
static const CharFrame grey_frame[] = {
  {0, {  0,  0, 59, 88}, {158,158}}, //0 grey 1
  {0, { 59,  0, 60, 88}, {158,158}}, //1 grey 2
  {0, {119,  0, 59, 89}, {159,159}}, //2 grey 3
  {0, {178,  0, 59, 90}, {160,160}}, //3 grey 4
  {0, {  0, 90, 59, 90}, {159,160}}, //4 grey 5
  {0, { 59, 90, 60, 90}, {160,160}}, //5 grey 6
};

static const Animation grey_anim[] = {
	{2, (const u8[]){0, 1, 2, 3, 4, 5, ASCR_BACK, 1}}, //Idle
};

//Grey functions
void Kitchen_Grey_SetFrame(void *user, u8 frame)
{
	Back_Kitchen *this = (Back_Kitchen*)user;
	
	//Check if this is a new frame
	if (frame != this->grey_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &grey_frame[this->grey_frame = frame];
		if (cframe->tex != this->grey_tex_id)
			Gfx_LoadTex(&this->tex_grey, this->arc_grey_ptr[this->grey_tex_id = cframe->tex], 0);
	}
}

void Kitchen_Grey_Draw(Back_Kitchen *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &grey_frame[this->grey_frame];
    
    fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = { ox, oy, src.w << FIXED_SHIFT ,src.h<< FIXED_SHIFT };
	Debug_StageMoveDebug(&dst, 8, stage.camera.x, stage.camera.y);
	Stage_DrawTex(&this->tex_grey, &src, &dst, stage.camera.bzoom);
}

//Saster animation and rects
static const CharFrame saster_frame[] = {
  {0, {  0,  0, 90, 96}, {160,158}}, //0 saster 1
  {0, { 90,  0, 90, 98}, {160,160}}, //1 saster 2
  {0, {  0, 98, 90, 98}, {159,160}}, //2 saster 3
  {0, { 90, 98, 89, 97}, {158,159}}, //3 saster 4
  {1, {  0,  0, 89, 97}, {157,159}}, //4 saster 5
};

static const Animation saster_anim[] = {
	{2, (const u8[]){0, 1, 2, 3, 4, ASCR_BACK, 1}}, //Idle
};

//Saster functions
void Kitchen_Saster_SetFrame(void *user, u8 frame)
{
	Back_Kitchen *this = (Back_Kitchen*)user;
	
	//Check if this is a new frame
	if (frame != this->saster_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &saster_frame[this->saster_frame = frame];
		if (cframe->tex != this->saster_tex_id)
			Gfx_LoadTex(&this->tex_saster, this->arc_saster_ptr[this->saster_tex_id = cframe->tex], 0);
	}
}

void Kitchen_Saster_Draw(Back_Kitchen *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &saster_frame[this->saster_frame];
    
    fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = { ox, oy, src.w << FIXED_SHIFT ,src.h<< FIXED_SHIFT };
	Debug_StageMoveDebug(&dst, 9, stage.camera.x, stage.camera.y);
	Stage_DrawTex(&this->tex_saster, &src, &dst, stage.camera.bzoom);
}

void Back_Kitchen_DrawMG(StageBack *back)
{
	Back_Kitchen *this = (Back_Kitchen*)back;

	fixed_t fx, fy;

	//Draw kitchen
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT back2_src = {  0,  0,244,101};
	RECT_FIXED back2_dst = {
		FIXED_DEC(105 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(128,1) - fy,
		FIXED_DEC(246 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(103,1)
	};
	
	Debug_StageMoveDebug(&back2_dst, 7, fx, fy);
	Stage_DrawTex(&this->tex_back2, &back2_src, &back2_dst, stage.camera.bzoom);
}

void Back_Kitchen_DrawBG(StageBack *back)
{
	Back_Kitchen *this = (Back_Kitchen*)back;

	fixed_t fx, fy;
	
	//Draw kitchen	
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	//Animate and draw grey
	if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step & 0x7) == 0)
		Animatable_SetAnim(&this->grey_animatable, 0);
	Animatable_Animate(&this->grey_animatable, (void*)this, Kitchen_Grey_SetFrame);
	Kitchen_Grey_Draw(this, FIXED_DEC(162 + 160,1) - fx, FIXED_DEC(99 + 160,1) - fy);
	
	//Animate and draw saster
	if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step & 0x7 )== 0)
		Animatable_SetAnim(&this->saster_animatable, 0);
	Animatable_Animate(&this->saster_animatable, (void*)this, Kitchen_Saster_SetFrame);
	Kitchen_Saster_Draw(this, FIXED_DEC(235 + 157,1) - fx, FIXED_DEC(92 + 159,1) - fy);
	
	RECT back0_src = {  0,  0,255,255};
	RECT_FIXED back0_dst = {
		FIXED_DEC(0 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(258 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(258,1)
	};
	
	RECT back1_src = {  0,  0,201,255};
	RECT_FIXED back1_dst = {
		FIXED_DEC(257 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(203 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(258,1)
	};
	
	Debug_StageMoveDebug(&back0_dst, 5, fx, fy);
	Debug_StageMoveDebug(&back1_dst, 6, fx, fy);
	Stage_DrawTex(&this->tex_back0, &back0_src, &back0_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back1, &back1_src, &back1_dst, stage.camera.bzoom);
}

void Back_Kitchen_Free(StageBack *back)
{
	Back_Kitchen *this = (Back_Kitchen*)back;
	
	//Free grey archive
	Mem_Free(this->arc_grey);
	
	//Free saster archive
	Mem_Free(this->arc_saster);
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_Kitchen_New(void)
{
	//Allocate background structure
	Back_Kitchen *this = (Back_Kitchen*)Mem_Alloc(sizeof(Back_Kitchen));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = NULL;
	this->back.draw_md = Back_Kitchen_DrawMG;
	this->back.draw_bg = Back_Kitchen_DrawBG;
	this->back.free = Back_Kitchen_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\BG\\KITCHEN.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_back2, Archive_Find(arc_back, "back2.tim"), 0);
	Mem_Free(arc_back);
	
	//Load grey textures
	this->arc_grey = IO_Read("\\BG\\GREYC.ARC;1");
	this->arc_grey_ptr[0] = Archive_Find(this->arc_grey, "grey0.tim");
	
	//Load saster textures
	this->arc_saster = IO_Read("\\BG\\SASTER.ARC;1");
	this->arc_saster_ptr[0] = Archive_Find(this->arc_saster, "saster0.tim");
	this->arc_saster_ptr[1] = Archive_Find(this->arc_saster, "saster1.tim");
	
	//Initialize grey state
	Animatable_Init(&this->grey_animatable, grey_anim);
	Animatable_SetAnim(&this->grey_animatable, 0);
	this->grey_frame = this->grey_tex_id = 0xFF; //Force art load
	
	//Initialize saster state
	Animatable_Init(&this->saster_animatable, saster_anim);
	Animatable_SetAnim(&this->saster_animatable, 0);
	this->saster_frame = this->saster_tex_id = 0xFF; //Force art load
	
	return (StageBack*)this;
}
