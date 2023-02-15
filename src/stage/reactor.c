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
	Gfx_Tex tex_back0; //back0
	Gfx_Tex tex_back1; //back1
	Gfx_Tex tex_back2; //back2
	Gfx_Tex tex_bop; //bop
	Gfx_Tex tex_fg;    //fg

} Back_Reactor;

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
		beat_bop = FIXED_UNIT - ((stage.note_scroll / 24) & FIXED_LAND);
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
	
	return (StageBack*)this;
}
