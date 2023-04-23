/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "pretender.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"
#include "../character/speaker.h"

//Pretender background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	IO_Data arc_snapperd, arc_snapperd_ptr[1];
	Gfx_Tex tex_back0; //back0
	Gfx_Tex tex_back1; //back1
	Gfx_Tex tex_vines; //vines
	Gfx_Tex tex_pot; //pot
	Gfx_Tex tex_border; //border
	
	//SnapperD state
	Gfx_Tex tex_snapperd;
	u8 snapperd_frame, snapperd_tex_id;
	Animatable snapperd_animatable;
	
	//Speaker
	Speaker speaker;
	
} Back_Pretender;

//SnapperD animation and rects
static const CharFrame snapperd_frame[] = {
  {0, {  0,  0,105, 63}, {160,157}}, //0 snaperd 1
  {0, {105,  0,104, 63}, {160,157}}, //1 snaperd 2
  {0, {  0, 63,103, 63}, {160,158}}, //2 snaperd 3
  {0, {103, 63,105, 65}, {159,160}}, //3 snaperd 4
  {0, {  0,128,105, 66}, {159,160}}, //4 snaperd 5
};

static const Animation snapperd_anim[] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, ASCR_BACK, 1}}, //Idle
};

//SnapperD functions
void Pretender_SnapperD_SetFrame(void *user, u8 frame)
{
	Back_Pretender *this = (Back_Pretender*)user;
	
	//Check if this is a new frame
	if (frame != this->snapperd_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &snapperd_frame[this->snapperd_frame = frame];
		if (cframe->tex != this->snapperd_tex_id)
			Gfx_LoadTex(&this->tex_snapperd, this->arc_snapperd_ptr[this->snapperd_tex_id = cframe->tex], 0);
	}
}

void Pretender_SnapperD_Draw(Back_Pretender *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &snapperd_frame[this->snapperd_frame];
    
    fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = { ox, oy, src.w * FIXED_DEC(1667,1000),src.h * FIXED_DEC(1667,1000)};
	Debug_StageMoveDebug(&dst, 7, stage.camera.x, stage.camera.y);
	Stage_DrawTex(&this->tex_snapperd, &src, &dst, stage.camera.bzoom);
}

void Back_Pretender_DrawFG(StageBack *back)
{
	Back_Pretender *this = (Back_Pretender*)back;

	fixed_t fx, fy;

	//Draw pretender
	fx = (stage.camera.x * 6) / 5;
	fy = stage.camera.y;
	
	RECT border_src = {  0,  0,144,108};
	RECT_FIXED border_dst = {
		FIXED_DEC(-160 - screen.SCREEN_WIDEOADD2,1),
		FIXED_DEC(-120,1),
		FIXED_DEC(320 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(240,1)
	};
	
	RECT vine0_src = {  0,  0,115,233};
	RECT_FIXED vine0_dst = {
		FIXED_DEC(23 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-8,1) - fy,
		FIXED_DEC(169 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(343,1)
	};
	
	RECT vine1_src = {116,  0,132,252};
	RECT_FIXED vine1_dst = {
		FIXED_DEC(23+549 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-8,1) - fy,
		FIXED_DEC(194 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(371,1)
	};
	
	RECT pot_src = {  0,  0,239, 91};
	RECT_FIXED pot_dst = {
		FIXED_DEC(-2 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(293,1) - fy,
		FIXED_DEC(239 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(91,1)
	};
	
	Debug_StageMoveDebug(&vine0_dst, 8, fx, fy);
	Debug_StageMoveDebug(&vine1_dst, 9, fx, fy);
	Debug_StageMoveDebug(&pot_dst, 10, fx, fy);
	Stage_BlendTex(&this->tex_border, &border_src, &border_dst, FIXED_DEC(1,1), 1);
	Stage_DrawTex(&this->tex_vines, &vine0_src, &vine0_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_vines, &vine1_src, &vine1_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_pot, &pot_src, &pot_dst, stage.camera.bzoom);
	
	//Animate and draw snapperd
	if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step & 0x7) == 0)
		Animatable_SetAnim(&this->snapperd_animatable, 0);
	Animatable_Animate(&this->snapperd_animatable, (void*)this, Pretender_SnapperD_SetFrame);
	Pretender_SnapperD_Draw(this, FIXED_DEC(36 + 224,1) - fx, FIXED_DEC(229 + 160,1) - fy);
}

void Back_Pretender_DrawMG(StageBack *back)
{
	Back_Pretender *this = (Back_Pretender*)back;

	fixed_t fx, fy;

	//Draw pretender
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT deadgf_src = {  0,113, 67, 64};
	RECT_FIXED deadgf_dst = {
		FIXED_DEC(342 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(150,1) - fy,
		FIXED_DEC(74 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(70,1)
	};
	
	Debug_StageMoveDebug(&deadgf_dst, 11, fx, fy);
	Stage_DrawTex(&this->tex_border, &deadgf_src, &deadgf_dst, stage.camera.bzoom);
	
	if ((stage.song_step % stage.gf_speed) == 0)
	{
		//Bump speakers
		Speaker_Bump(&this->speaker);
	}
	//Tick speakers
	Speaker_Tick(&this->speaker, FIXED_DEC(373,1) - fx, FIXED_DEC(183,1) - fy, 0);
}

void Back_Pretender_DrawBG(StageBack *back)
{
	Back_Pretender *this = (Back_Pretender*)back;

	fixed_t fx, fy;

	//Draw pretender
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT back0_src = {  0,  0,255,255};
	RECT_FIXED back0_dst = {
		FIXED_DEC(0 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(361 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(361,1)
	};
	
	RECT back1_src = {  0,  0,246,255};
	RECT_FIXED back1_dst = {
		FIXED_DEC(359 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(348 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(361,1)
	};
	
	Debug_StageMoveDebug(&back0_dst, 5, fx, fy);
	Debug_StageMoveDebug(&back1_dst, 6, fx, fy);
	Stage_DrawTex(&this->tex_back0, &back0_src, &back0_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back1, &back1_src, &back1_dst, stage.camera.bzoom);
}

void Back_Pretender_Free(StageBack *back)
{
	Back_Pretender *this = (Back_Pretender*)back;
	
	//Free snapperd archive
	Mem_Free(this->arc_snapperd);
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_Pretender_New(void)
{
	//Allocate background structure
	Back_Pretender *this = (Back_Pretender*)Mem_Alloc(sizeof(Back_Pretender));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = Back_Pretender_DrawFG;
	this->back.draw_md = Back_Pretender_DrawMG;
	this->back.draw_bg = Back_Pretender_DrawBG;
	this->back.free = Back_Pretender_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\BG\\PRETENDR.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_vines, Archive_Find(arc_back, "vines.tim"), 0);
	Gfx_LoadTex(&this->tex_pot, Archive_Find(arc_back, "pot.tim"), 0);
	Gfx_LoadTex(&this->tex_border, Archive_Find(arc_back, "border.tim"), 0);
	Mem_Free(arc_back);
	
	//Load snapperd textures
	this->arc_snapperd = IO_Read("\\BG\\SNAPPERD.ARC;1");
	this->arc_snapperd_ptr[0] = Archive_Find(this->arc_snapperd, "snapperd0.tim");
	
	//Initialize snapperd state
	Animatable_Init(&this->snapperd_animatable, snapperd_anim);
	Animatable_SetAnim(&this->snapperd_animatable, 0);
	this->snapperd_frame = this->snapperd_tex_id = 0xFF; //Force art load
	
	//Initialize speaker
	Speaker_Init(&this->speaker);
	
	return (StageBack*)this;
}
