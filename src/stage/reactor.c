/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "reactor.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

//Reactor background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	IO_Data arc_ball, arc_ball_ptr[7];
	Gfx_Tex tex_back0; //back0
	Gfx_Tex tex_back1; //back1
	Gfx_Tex tex_back2; //back2
	Gfx_Tex tex_bop; //bop
	Gfx_Tex tex_fg;    //fg
	
	//Ball state
	Gfx_Tex tex_ball;
	u8 ball_frame, ball_tex_id;
	Animatable ball_animatable;

} Back_Reactor;

//Ball animation and rects
static const CharFrame ball_frame[] = {
  {0, {  0,  0,122,104}, {  0,  0}}, //0 ball 1
  {0, {123,  0,122,103}, {  0,  0}}, //1 ball 2
  {0, {  0,104,122,104}, {  0,  0}}, //2 ball 3
  {0, {123,104,122,104}, {  0,  0}}, //3 ball 4
  {1, {  0,  0,122,104}, {  0,  0}}, //4 ball 5
  {1, {123,  0,122,104}, {  0,  0}}, //5 ball 6
  {1, {  0,104,122,103}, {  0,  0}}, //6 ball 7
  {1, {123,104,122,104}, {  0,  0}}, //7 ball 8
  {2, {  0,  0,122,104}, {  0,  0}}, //8 ball 9
  {2, {123,  0,122,104}, {  0,  0}}, //9 ball 10
  {2, {  0,104,122,104}, {  0,  0}}, //10 ball 11
  {2, {123,104,122,103}, {  0,  0}}, //11 ball 12
  {3, {  0,  0,122,104}, {  0,  0}}, //12 ball 13
  {3, {123,  0,122,103}, {  0,  0}}, //13 ball 14
  {3, {  0,104,122,105}, {  0,  0}}, //14 ball 15
  {3, {123,104,122,104}, {  0,  0}}, //15 ball 16
  {4, {  0,  0,122,104}, {  0,  0}}, //16 ball 17
  {4, {123,  0,122,104}, {  0,  0}}, //17 ball 18
  {4, {  0,104,122,103}, {  0,  0}}, //18 ball 19
  {4, {123,104,122,104}, {  0,  0}}, //19 ball 20
  {5, {  0,  0,122,104}, {  0,  0}}, //20 ball 21
  {5, {123,  0,122,104}, {  0,  0}}, //21 ball 22
  {5, {  0,104,122,104}, {  0,  0}}, //22 ball 23
  {5, {123,104,122,104}, {  0,  0}}, //23 ball 24
  {6, {  0,  0,122,104}, {  0,  0}}, //24 ball 25
};

static const Animation ball_anim[] = {
	{1, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 8, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 13, 23, 20, 21, 22, 24, ASCR_CHGANI, 0}}, //Idle
};

//Ball functions
void Reactor_Ball_SetFrame(void *user, u8 frame)
{
	Back_Reactor *this = (Back_Reactor*)user;
	
	//Check if this is a new frame
	if (frame != this->ball_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &ball_frame[this->ball_frame = frame];
		if (cframe->tex != this->ball_tex_id)
			Gfx_LoadTex(&this->tex_ball, this->arc_ball_ptr[this->ball_tex_id = cframe->tex], 0);
	}
}

void Reactor_Ball_Draw(Back_Reactor *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &ball_frame[this->ball_frame];
    
    fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = { ox, oy, src.w * FIXED_DEC(227,100), src.h * FIXED_DEC(227,100)};
	Debug_StageMoveDebug(&dst, 10, stage.camera.x, stage.camera.y);
	Stage_DrawTex(&this->tex_ball, &src, &dst, stage.camera.bzoom);
}

void Back_Reactor_DrawFG(StageBack *back)
{
	Back_Reactor *this = (Back_Reactor*)back;

	fixed_t fx, fy;

	//Draw fg
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT fg_src = {  0,  0,255, 49};
	RECT_FIXED fg_dst = {
		FIXED_DEC(0 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(177,1) - fy,
		FIXED_DEC(523 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(100,1)
	};
	
	Debug_StageMoveDebug(&fg_dst, 12, fx, fy);
	Stage_BlendTex(&this->tex_fg, &fg_src, &fg_dst, stage.camera.bzoom, 1);
}

void Back_Reactor_DrawBG(StageBack *back)
{
	Back_Reactor *this = (Back_Reactor*)back;

	fixed_t fx, fy;
	
	fixed_t beat_bop;
	if ((stage.song_step & 0xF) == 0)
		beat_bop = FIXED_UNIT - ((stage.note_scroll / 24) & FIXED_LAND/4);
	else
		beat_bop = 0;
	
	//Draw wall thing
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT wall_src = {  0,  0,154,197};
	RECT_FIXED wall0_dst = {
		FIXED_DEC(88 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(18,1) - fy,
		FIXED_DEC(-154 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(197,1)
	};
	RECT_FIXED wall1_dst = {
		FIXED_DEC(421 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(18,1) - fy,
		FIXED_DEC(154 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(197,1)
	};
	
	Debug_StageMoveDebug(&wall0_dst, 7, fx, fy);
	Debug_StageMoveDebug(&wall1_dst, 8, fx, fy);
	Stage_DrawTex(&this->tex_back2, &wall_src, &wall0_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back2, &wall_src, &wall1_dst, stage.camera.bzoom);
	
	//Draw bop
	static const struct Back_Reactor_LowerBop
	{
		RECT src;
		RECT_FIXED dst;
	} lbop_piece[] = {
		{{0, 0, 58, 89}, {FIXED_DEC(53,1), FIXED_DEC(106,1), FIXED_DEC(58,1), FIXED_DEC(89,1)}},
		{{59, 0, 58, 93}, {FIXED_DEC(385,1), FIXED_DEC(100,1), FIXED_DEC(58,1), FIXED_DEC(93,1)}},
	};
	
	const struct Back_Reactor_LowerBop *lbop_p = lbop_piece;
	for (size_t i = 0; i < COUNT_OF(lbop_piece); i++, lbop_p++)
	{
		RECT_FIXED lbop_dst = {
			lbop_p->dst.x - fx - (beat_bop << 1),
			lbop_p->dst.y - fy + (beat_bop << 3),
			lbop_p->dst.w + (beat_bop << 2),
			lbop_p->dst.h - (beat_bop << 3),
		};
		Debug_StageMoveDebug(&lbop_dst, 9, fx, fy);
		Stage_DrawTex(&this->tex_bop, &lbop_p->src, &lbop_dst, stage.camera.bzoom);
	}
	
	//Animate and draw ball
	if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step == -29))
		Animatable_SetAnim(&this->ball_animatable, 0);
	
	Animatable_Animate(&this->ball_animatable, (void*)this, Reactor_Ball_SetFrame);
	Reactor_Ball_Draw(this, FIXED_DEC(131,1) - fx, FIXED_DEC(-106,1) - fy);
	
	//Draw reactor
	RECT back0_src = {  0,  0,255,255};
	RECT_FIXED back0_dst = {
		FIXED_DEC(0 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(277 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(277,1)
	};
	
	RECT back1_src = {  0,  0,228,255};
	RECT_FIXED back1_dst = {
		FIXED_DEC(277 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(246 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(277,1)
	};
	
	Debug_StageMoveDebug(&back0_dst, 5, fx, fy);
	Debug_StageMoveDebug(&back1_dst, 6, fx, fy);
	Stage_DrawTex(&this->tex_back0, &back0_src, &back0_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back1, &back1_src, &back1_dst, stage.camera.bzoom);
	
	//Draw back thing
	RECT screen_src = {0, 0, screen.SCREEN_WIDTH, screen.SCREEN_HEIGHT};
	Gfx_DrawRect(&screen_src, 19, 16, 20);
}

void Back_Reactor_Free(StageBack *back)
{
	Back_Reactor *this = (Back_Reactor*)back;
	
	//Free ball archive
	Mem_Free(this->arc_ball);
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_Reactor_New(void)
{
	//Allocate background structure
	Back_Reactor *this = (Back_Reactor*)Mem_Alloc(sizeof(Back_Reactor));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = Back_Reactor_DrawFG;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Reactor_DrawBG;
	this->back.free = Back_Reactor_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\BG\\REACTOR.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_back2, Archive_Find(arc_back, "back2.tim"), 0);
	Gfx_LoadTex(&this->tex_bop, Archive_Find(arc_back, "bop.tim"), 0);
	Gfx_LoadTex(&this->tex_fg, Archive_Find(arc_back, "fg.tim"), 0);
	Mem_Free(arc_back);
	
	//Load ball textures
	this->arc_ball = IO_Read("\\BG\\BALL.ARC;1");
	this->arc_ball_ptr[0] = Archive_Find(this->arc_ball, "ball0.tim");
	this->arc_ball_ptr[1] = Archive_Find(this->arc_ball, "ball1.tim");
	this->arc_ball_ptr[2] = Archive_Find(this->arc_ball, "ball2.tim");
	this->arc_ball_ptr[3] = Archive_Find(this->arc_ball, "ball3.tim");
	this->arc_ball_ptr[4] = Archive_Find(this->arc_ball, "ball4.tim");
	this->arc_ball_ptr[5] = Archive_Find(this->arc_ball, "ball5.tim");
	this->arc_ball_ptr[6] = Archive_Find(this->arc_ball, "ball6.tim");
	
	//Initialize ball state
	Animatable_Init(&this->ball_animatable, ball_anim);
	Animatable_SetAnim(&this->ball_animatable, 0);
	this->ball_frame = this->ball_tex_id = 0xFF; //Force art load
	
	return (StageBack*)this;
}
