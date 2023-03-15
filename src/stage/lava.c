/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "lava.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

int x1, x2, x3, x4, x5, x6, x7, x8 = -1;
int y1, y2, y3, y4, y5, y6, y7, y8 = 309;
int rs1, rs2, rs3, rs4, rs5, rs6, rs7, rs8 = 200;

//Lava background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	IO_Data arc_bubbles, arc_bubbles_ptr[2];
	Gfx_Tex tex_back0; //back0
	Gfx_Tex tex_back1; //back1

	//Bubbles state
	Gfx_Tex tex_bubbles;
	u8 bubbles_frame, bubbles_tex_id;
	Animatable bubbles_animatable;
	
} Back_Lava;

//Bubbles animation and rects
static const CharFrame bubbles_frame[] = {
  {0, {  0,  0,  3,  3}, {145,158}}, //0 bubbles 1
  {0, {  3,  0, 59, 17}, {146,159}}, //1 bubbles 2
  {0, { 62,  0,159, 18}, {146,160}}, //2 bubbles 3
  {0, {  0, 18,159, 18}, {146,160}}, //3 bubbles 4
  {0, {  0, 36,172, 18}, {158,160}}, //4 bubbles 5
  {0, {  0, 54,172, 19}, {158,160}}, //5 bubbles 6
  {0, {  0, 73,172, 18}, {158,160}}, //6 bubbles 7
  {0, {  0, 92,173, 18}, {159,159}}, //7 bubbles 8
  {0, {  0,111,173, 18}, {159,159}}, //8 bubbles 9
  {0, {  0,130,174, 18}, {159,159}}, //9 bubbles 10
  {0, {  0,149,174, 18}, {159,159}}, //10 bubbles 11
  {0, {  0,168,175, 17}, {160,158}}, //11 bubbles 12
  {0, {  0,187,173, 17}, {160,158}}, //12 bubbles 13
  {0, {  0,206,174, 17}, {160,157}}, //13 bubbles 14
  {0, {  0,225,173, 18}, {160,157}}, //14 bubbles 15
  {1, {  0,  0,173, 18}, {160,157}}, //15 bubbles 16
  {1, {  0, 18,173, 18}, {160,158}}, //16 bubbles 17
  {1, {  0, 36,173, 18}, {160,157}}, //17 bubbles 18
  {1, {  0, 54,155, 18}, {141,157}}, //18 bubbles 19
  {1, {  0, 72,155, 18}, {141,157}}, //19 bubbles 20
  {1, {  0, 90,154, 17}, {141,156}}, //20 bubbles 21
  {1, {154, 90, 21, 13}, { 53,152}}, //21 bubbles 22
  {1, {175, 90, 21, 13}, { 52,152}}, //22 bubbles 23
  {1, {196, 90, 21, 13}, { 52,152}}, //23 bubbles 24
  {1, {217, 90, 21, 12}, { 52,152}}, //24 bubbles 25
  {1, {238, 90,  6,  3}, { 38,151}}, //25 bubbles 26
  {1, {244, 90,  6,  4}, { 38,151}}, //26 bubbles 27
};

static const Animation bubbles_anim[] = {
	{1, (const u8[]){ 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 21, 22, 23, 24, 24, 25, 26, ASCR_CHGANI, 0}}, //Idle
};

//Bubbles functions
void Lava_Bubbles_SetFrame(void *user, u8 frame)
{
	Back_Lava *this = (Back_Lava*)user;
	
	//Check if this is a new frame
	if (frame != this->bubbles_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &bubbles_frame[this->bubbles_frame = frame];
		if (cframe->tex != this->bubbles_tex_id)
			Gfx_LoadTex(&this->tex_bubbles, this->arc_bubbles_ptr[this->bubbles_tex_id = cframe->tex], 0);
	}
}

void Lava_Bubbles_Draw(Back_Lava *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &bubbles_frame[this->bubbles_frame];
    
    fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = { ox, oy, src.w * FIXED_DEC(25,10), src.h * FIXED_DEC(25,10)};
	Debug_StageMoveDebug(&dst, 9, stage.camera.x, stage.camera.y);
	Stage_DrawTex(&this->tex_bubbles, &src, &dst, stage.camera.bzoom);
}

void Back_Lava_DrawFG(StageBack *back)
{
	Back_Lava *this = (Back_Lava*)back;

	fixed_t fx, fy;
	
	//Animate and draw embers
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT dot_src = {188,  0,  5,  5};
	RECT_FIXED dot1_dst = {FIXED_DEC(x1,1) - fx,FIXED_DEC(y1,1) - fy,FIXED_DEC(5 + screen.SCREEN_WIDEOADD,1),FIXED_DEC(5,1)};
	RECT_FIXED dot2_dst = {FIXED_DEC(x2,1) - fx,FIXED_DEC(y2,1) - fy,FIXED_DEC(5 + screen.SCREEN_WIDEOADD,1),FIXED_DEC(5,1)};
	RECT_FIXED dot3_dst = {FIXED_DEC(x3,1) - fx,FIXED_DEC(y3,1) - fy,FIXED_DEC(5 + screen.SCREEN_WIDEOADD,1),FIXED_DEC(5,1)};
	RECT_FIXED dot4_dst = {FIXED_DEC(x4,1) - fx,FIXED_DEC(y4,1) - fy,FIXED_DEC(5 + screen.SCREEN_WIDEOADD,1),FIXED_DEC(5,1)};
	RECT_FIXED dot5_dst = {FIXED_DEC(x5,1) - fx,FIXED_DEC(y5,1) - fy,FIXED_DEC(5 + screen.SCREEN_WIDEOADD,1),FIXED_DEC(5,1)};
	
	Stage_DrawTex(&this->tex_back1, &dot_src, &dot1_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back1, &dot_src, &dot2_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back1, &dot_src, &dot3_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back1, &dot_src, &dot4_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back1, &dot_src, &dot5_dst, stage.camera.bzoom);
	
	if (stage.paused == false)
	{
	if (stage.song_step >= rs1)
		y1 -= RandomRange(1,3);
	if (stage.song_step >= rs2)
		y2 -= RandomRange(1,3);
	if (stage.song_step >= rs3)
		y3 -= RandomRange(1,3);
	if (stage.song_step >= rs4)
		y4 -= RandomRange(1,3);
	if (stage.song_step >= rs5)
		y5 -= RandomRange(1,3);
	if ((y1 <= -20) || ((stage.song_step == rs1) && stage.flag & STAGE_FLAG_JUST_STEP))
	{
		x1 = RandomRange(6,462);
		y1 = RandomRange(295,308);
	}
	if ((y2 <= -20) || ((stage.song_step == rs2) && stage.flag & STAGE_FLAG_JUST_STEP))
	{
		x2 = RandomRange(6,462);
		y2 = RandomRange(295,308);
	}
	if ((y3 <= -20) || ((stage.song_step == rs3) && stage.flag & STAGE_FLAG_JUST_STEP))
	{
		x3 = RandomRange(6,462);
		y3 = RandomRange(295,308);
	}
	if ((y4 <= -20) || ((stage.song_step == rs4) && stage.flag & STAGE_FLAG_JUST_STEP))
	{
		x4 = RandomRange(6,462);
		y4 = RandomRange(295,308);
	}
	if ((y5 <= -20) || ((stage.song_step == rs5) && stage.flag & STAGE_FLAG_JUST_STEP))
	{
		x5 = RandomRange(6,462);
		y5 = RandomRange(295,308);
	}
	if ((y6 <= -20) || ((stage.song_step == rs6) && stage.flag & STAGE_FLAG_JUST_STEP))
	{
		x6 = RandomRange(6,462);
		y6 = RandomRange(295,308);
	}
	if ((y7 <= -20) || ((stage.song_step == rs7) && stage.flag & STAGE_FLAG_JUST_STEP))
	{
		x7 = RandomRange(6,462);
		y7 = RandomRange(295,308);
	}
	if ((y8 <= -20) || ((stage.song_step == rs8) && stage.flag & STAGE_FLAG_JUST_STEP))
	{
		x8 = RandomRange(6,462);
		y8 = RandomRange(295,308);
	}
	
	if ((stage.song_step == -25) && stage.flag & STAGE_FLAG_JUST_STEP)
	{
		rs1 = RandomRange(-24,15);
		rs2 = RandomRange(-24,15);
		rs3 = RandomRange(-24,15);
		rs4 = RandomRange(-24,15);
		rs5 = RandomRange(-24,15);
		rs6 = RandomRange(-24,15);
		rs7 = RandomRange(-24,15);
		rs8 = RandomRange(-24,15);
	}
	}
}
	
void Back_Lava_DrawBG(StageBack *back)
{
	Back_Lava *this = (Back_Lava*)back;

	fixed_t fx, fy;
	
	//Animate and draw bubbles
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step == -29))
		Animatable_SetAnim(&this->bubbles_animatable, 0);
	
	Animatable_Animate(&this->bubbles_animatable, (void*)this, Lava_Bubbles_SetFrame);
	Lava_Bubbles_Draw(this, FIXED_DEC(33 + 160,1) - fx, FIXED_DEC(286 + 156,1) - fy);
	
	//Draw lava
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT back0_src = {  0,  0,255,255};
	RECT_FIXED back0_dst = {
		FIXED_DEC(0 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(308 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(308,1)
	};
	
	RECT back1_src = {  0,  0,187,255};
	RECT_FIXED back1_dst = {
		FIXED_DEC(307 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(226 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(308,1)
	};
	
	Debug_StageMoveDebug(&back0_dst, 5, fx, fy);
	Debug_StageMoveDebug(&back1_dst, 6, fx, fy);
	Stage_DrawTex(&this->tex_back0, &back0_src, &back0_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back1, &back1_src, &back1_dst, stage.camera.bzoom);
}

void Back_Lava_Free(StageBack *back)
{
	Back_Lava *this = (Back_Lava*)back;
	
	//Free bubbles archive
	Mem_Free(this->arc_bubbles);
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_Lava_New(void)
{
	//Allocate background structure
	Back_Lava *this = (Back_Lava*)Mem_Alloc(sizeof(Back_Lava));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = Back_Lava_DrawFG;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Lava_DrawBG;
	this->back.free = Back_Lava_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\BG\\LAVA.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Mem_Free(arc_back);
	
	//Load bubbles textures
	this->arc_bubbles = IO_Read("\\BG\\BUBBLES.ARC;1");
	this->arc_bubbles_ptr[0] = Archive_Find(this->arc_bubbles, "bubbles0.tim");
	this->arc_bubbles_ptr[1] = Archive_Find(this->arc_bubbles, "bubbles1.tim");
	
	//Initialize bubbles state
	Animatable_Init(&this->bubbles_animatable, bubbles_anim);
	Animatable_SetAnim(&this->bubbles_animatable, 0);
	this->bubbles_frame = this->bubbles_tex_id = 0xFF; //Force art load
	
	x1 = -1;
	x2 = -1;
	x3 = -1;
	x4 = -1;
	x5 = -1;
	x6 = -1;
	x7 = -1;
	x8 = -1;
	y1 = 309;
	y2 = 309;
	y3 = 309;
	y4 = 309;
	y5 = 309;
	y6 = 309;
	y7 = 309;
	y8 = 309;
	rs1 = 200;
	rs2 = 200;
	rs3 = 200;
	rs4 = 200;
	rs5 = 200;
	rs6 = 200;
	rs7 = 200;
	rs8 = 200;
	
	return (StageBack*)this;
}
