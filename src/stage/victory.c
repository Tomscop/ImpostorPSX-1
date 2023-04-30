/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "victory.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

//Victory background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0; //back0
	Gfx_Tex tex_back1; //back1
	Gfx_Tex tex_back2; //back2

} Back_Victory;

void Back_Victory_DrawFG(StageBack *back)
{
	Back_Victory *this = (Back_Victory*)back;

	fixed_t fx, fy;

	//Draw victorybg
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	//Draw black
	RECT screen_src = {0, 0, screen.SCREEN_WIDTH, screen.SCREEN_HEIGHT};
	if (((stage.song_step >= 120) && (stage.song_step <= 127)) || ((stage.song_step >= 448) && (stage.song_step <= 463)) || ((stage.song_step >= 716) && (stage.song_step <= 719)) || ((stage.song_step >= 976) && (stage.song_step <= 991)) || ((stage.song_step >= 1052) && (stage.song_step <= 1055)) || ((stage.song_step >= 1116) && (stage.song_step <= 1119)))
		Gfx_DrawRect(&screen_src, 0, 0, 0);
	
	fx = (stage.camera.x / 2) * 3;
	
	RECT fogfront_src = {  0,175,255, 35};
	RECT_FIXED fogfront_dst = {
		FIXED_DEC(0 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(949 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(131,1)
	};
	
	Debug_StageMoveDebug(&fogfront_dst, 13, fx, fy);
	//Stage_BlendTex(&this->tex_back1, &fogfront_src, &fogfront_dst, stage.camera.bzoom, 1);
	
	fx = stage.camera.x;
	
	RECT lights_src = {  0,  0,114,209};
	RECT_FIXED light1_dst = {
		FIXED_DEC(0 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(143 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(261,1)
	};
	RECT_FIXED light2_dst = {
		FIXED_DEC(0+252 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(143 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(261,1)
	};
	
	Debug_StageMoveDebug(&light1_dst, 10, fx, fy);
	Debug_StageMoveDebug(&light2_dst, 10, fx, fy);
	Stage_BlendTex(&this->tex_back2, &lights_src, &light1_dst, stage.camera.bzoom, 1);
	Stage_BlendTex(&this->tex_back2, &lights_src, &light2_dst, stage.camera.bzoom, 1);
}

void Back_Victory_DrawBG(StageBack *back)
{
	Back_Victory *this = (Back_Victory*)back;

	fixed_t fx, fy;

	//Draw victorybg
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT fogmid_src = {  0,144,255, 30};
	RECT_FIXED fogmid_dst = {
		FIXED_DEC(-35 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(99,1) - fy,
		FIXED_DEC(581 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(68,1)
	};
	
	Debug_StageMoveDebug(&fogmid_dst, 12, fx, fy);
	Stage_BlendTex(&this->tex_back1, &fogmid_src, &fogmid_dst, stage.camera.bzoom, 1);
	
	//Boppers
	fixed_t beat_bop, other_bop;
	if ((stage.song_step & 0x3) == 0)
		beat_bop = FIXED_UNIT - ((stage.note_scroll / 24) & FIXED_LAND/4);
	else
		beat_bop = 0;
	if ((stage.song_step & 0x7) == 0)
		other_bop = FIXED_UNIT - ((stage.note_scroll / 24) & FIXED_LAND/4);
	else
		other_bop = 0;
	
	//Draw warchief
	static const struct Back_Victory_WarchiefBop1
	{
		RECT src;
		RECT_FIXED dst;
	} wbop_piece1[] = {
		{{136,142, 70,101}, {FIXED_DEC(190+40,1), FIXED_DEC(54,1), FIXED_DEC(70,1), FIXED_DEC(101,1)}},
	};
	const struct Back_Victory_WarchiefBop1 *wbop_p1 = wbop_piece1;
	for (size_t i = 0; i < COUNT_OF(wbop_piece1); i++, wbop_p1++)
	{
		RECT_FIXED wbop_dst = {
			wbop_p1->dst.x - fx - (other_bop << 1),
			wbop_p1->dst.y - fy + (other_bop << 3),
			wbop_p1->dst.w + (other_bop << 2),
			wbop_p1->dst.h - (other_bop << 3),
		};
		Debug_StageMoveDebug(&wbop_dst, 7, fx, fy);
		if (stage.camswitch == 2)
			Stage_DrawTex(&this->tex_back0, &wbop_p1->src, &wbop_dst, stage.camera.bzoom);
	}
	static const struct Back_Victory_WarchiefBop2
	{
		RECT src;
		RECT_FIXED dst;
	} wbop_piece2[] = {
		{{136,142, 70,101}, {FIXED_DEC(190,1), FIXED_DEC(54,1), FIXED_DEC(70,1), FIXED_DEC(101,1)}},
	};
	const struct Back_Victory_WarchiefBop2 *wbop_p2 = wbop_piece2;
	for (size_t i = 0; i < COUNT_OF(wbop_piece2); i++, wbop_p2++)
	{
		RECT_FIXED wbop_dst = {
			wbop_p2->dst.x - fx - (other_bop << 1),
			wbop_p2->dst.y - fy + (other_bop << 3),
			wbop_p2->dst.w + (other_bop << 2),
			wbop_p2->dst.h - (other_bop << 3),
		};
		Debug_StageMoveDebug(&wbop_dst, 7, fx, fy);
		if ((stage.camswitch == 3) || (stage.camswitch == 5))
			Stage_DrawTex(&this->tex_back0, &wbop_p2->src, &wbop_dst, stage.camera.bzoom);
	}
	
	//Draw jelqer
	static const struct Back_Victory_JelqerBop1
	{
		RECT src;
		RECT_FIXED dst;
	} jlbop_piece1[] = {
		{{140, 48, 69, 93}, {FIXED_DEC(183+45,1), FIXED_DEC(63,1), FIXED_DEC(69,1), FIXED_DEC(93,1)}},
	};
	const struct Back_Victory_JelqerBop1 *jlbop_p1 = jlbop_piece1;
	for (size_t i = 0; i < COUNT_OF(jlbop_piece1); i++, jlbop_p1++)
	{
		RECT_FIXED jlbop_dst = {
			jlbop_p1->dst.x - fx - (beat_bop << 1),
			jlbop_p1->dst.y - fy + (beat_bop << 3),
			jlbop_p1->dst.w + (beat_bop << 2),
			jlbop_p1->dst.h - (beat_bop << 3),
		};
		Debug_StageMoveDebug(&jlbop_dst, 8, fx, fy);
		if (stage.camswitch == 1)
			Stage_DrawTex(&this->tex_back0, &jlbop_p1->src, &jlbop_dst, stage.camera.bzoom);
	}
	static const struct Back_Victory_JelqerBop2
	{
		RECT src;
		RECT_FIXED dst;
	} jlbop_piece2[] = {
		{{140, 48, 69, 93}, {FIXED_DEC(183+83,1), FIXED_DEC(63,1), FIXED_DEC(69,1), FIXED_DEC(93,1)}},
	};
	const struct Back_Victory_JelqerBop2 *jlbop_p2 = jlbop_piece2;
	for (size_t i = 0; i < COUNT_OF(jlbop_piece2); i++, jlbop_p2++)
	{
		RECT_FIXED jlbop_dst = {
			jlbop_p2->dst.x - fx - (beat_bop << 1),
			jlbop_p2->dst.y - fy + (beat_bop << 3),
			jlbop_p2->dst.w + (beat_bop << 2),
			jlbop_p2->dst.h - (beat_bop << 3),
		};
		Debug_StageMoveDebug(&jlbop_dst, 8, fx, fy);
		if (stage.camswitch == 3)
			Stage_DrawTex(&this->tex_back0, &jlbop_p2->src, &jlbop_dst, stage.camera.bzoom);
	}
	static const struct Back_Victory_JelqerBop3
	{
		RECT src;
		RECT_FIXED dst;
	} jlbop_piece3[] = {
		{{140, 48, 69, 93}, {FIXED_DEC(183,1), FIXED_DEC(63,1), FIXED_DEC(69,1), FIXED_DEC(93,1)}},
	};
	const struct Back_Victory_JelqerBop3 *jlbop_p3 = jlbop_piece3;
	for (size_t i = 0; i < COUNT_OF(jlbop_piece3); i++, jlbop_p3++)
	{
		RECT_FIXED jlbop_dst = {
			jlbop_p3->dst.x - fx - (beat_bop << 1),
			jlbop_p3->dst.y - fy + (beat_bop << 3),
			jlbop_p3->dst.w + (beat_bop << 2),
			jlbop_p3->dst.h - (beat_bop << 3),
		};
		Debug_StageMoveDebug(&jlbop_dst, 8, fx, fy);
		if (stage.camswitch == 4)
			Stage_DrawTex(&this->tex_back0, &jlbop_p3->src, &jlbop_dst, stage.camera.bzoom);
	}
	
	//Draw jorsawsee
	static const struct Back_Victory_JorsawseeBop
	{
		RECT src;
		RECT_FIXED dst;
	} jbop_piece[] = {
		{{  0,  0, 72,107}, {FIXED_DEC(269,1), FIXED_DEC(51,1), FIXED_DEC(72,1), FIXED_DEC(107,1)}},
	};
	const struct Back_Victory_JorsawseeBop *jbop_p = jbop_piece;
	for (size_t i = 0; i < COUNT_OF(jbop_piece); i++, jbop_p++)
	{
		RECT_FIXED jbop_dst = {
			jbop_p->dst.x - fx - (other_bop << 1),
			jbop_p->dst.y - fy + (other_bop << 3),
			jbop_p->dst.w + (other_bop << 2),
			jbop_p->dst.h - (other_bop << 3),
		};
		Debug_StageMoveDebug(&jbop_dst, 9, fx, fy);
		if ((stage.camswitch == 4) || (stage.camswitch == 5))
			Stage_DrawTex(&this->tex_back1, &jbop_p->src, &jbop_dst, stage.camera.bzoom);
	}
	
	RECT fogback_src = {  0,108,255, 35};
	RECT_FIXED fogback_dst = {
		FIXED_DEC(98 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(110,1) - fy,
		FIXED_DEC(285 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(39,1)
	};
	
	Debug_StageMoveDebug(&fogback_dst, 11, fx, fy);
	Stage_BlendTex(&this->tex_back1, &fogback_src, &fogback_dst, stage.camera.bzoom, 1);
	
	//Draw bop
	static const struct Back_Victory_LowerBop
	{
		RECT src;
		RECT_FIXED dst;
	} lbop_piece[] = {
		{{  0, 48,139,107}, {FIXED_DEC(0,1), FIXED_DEC(0,1), FIXED_DEC(139,1), FIXED_DEC(107,1)}},
		{{  0,156,135, 95}, {FIXED_DEC(0+396,1), FIXED_DEC(0+11,1), FIXED_DEC(135,1), FIXED_DEC(95,1)}},
	};
	const struct Back_Victory_LowerBop *lbop_p = lbop_piece;
	for (size_t i = 0; i < COUNT_OF(lbop_piece); i++, lbop_p++)
	{
		RECT_FIXED lbop_dst = {
			lbop_p->dst.x - fx - (beat_bop << 1),
			lbop_p->dst.y - fy + (beat_bop << 3),
			lbop_p->dst.w + (beat_bop << 2),
			lbop_p->dst.h - (beat_bop << 3),
		};
		Debug_StageMoveDebug(&lbop_dst, 6, fx, fy);
		Stage_DrawTex(&this->tex_back0, &lbop_p->src, &lbop_dst, stage.camera.bzoom);
	}
	
	RECT victory_src = {  0,  0,248, 47};
	RECT_FIXED victory_dst = {
		FIXED_DEC(136 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-23,1) - fy,
		FIXED_DEC(248 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(47,1)
	};
	
	Debug_StageMoveDebug(&victory_dst, 5, fx, fy);
	Stage_DrawTex(&this->tex_back0, &victory_src, &victory_dst, stage.camera.bzoom);
	
	//Draw black
	RECT screen_src = {0, 0, screen.SCREEN_WIDTH, screen.SCREEN_HEIGHT};
	Gfx_DrawRect(&screen_src, 0, 0, 0);
	
	//Switch thing
	if (stage.song_step == -29)
		stage.camswitch = 0;
	if (stage.song_step == 121)
		stage.camswitch = 1;
	if (stage.song_step == 450)
		stage.camswitch = 2;
	if (stage.song_step == 717)
		stage.camswitch = 3;
	if (stage.song_step == 977)
		stage.camswitch = 4;
	if (stage.song_step == 1053)
		stage.camswitch = 5;
	if (stage.song_step == 1117)
		stage.camswitch = 3;
	
	/*
	0 == warchief, no one in bg
	1 == warchief, jelqer in middle of bg
	2 == jelqer, warchief in middle of bg
	3 == jorsawsee, warchief and jelqer(right) in bg
	4 == warchief, jelqer(left) and jorsawsee in bg
	5 == jelqer, wachief and jorsawsee in bg
	*/
}

void Back_Victory_Free(StageBack *back)
{
	Back_Victory *this = (Back_Victory*)back;
	

	//Free structure
	Mem_Free(this);
}

StageBack *Back_Victory_New(void)
{
	//Allocate background structure
	Back_Victory *this = (Back_Victory*)Mem_Alloc(sizeof(Back_Victory));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = Back_Victory_DrawFG;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Victory_DrawBG;
	this->back.free = Back_Victory_Free;

	//Load background textures
	IO_Data arc_back = IO_Read("\\BG2\\VICTORY.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_back2, Archive_Find(arc_back, "back2.tim"), 0);
	Mem_Free(arc_back);

	Gfx_SetClear(0, 0, 0);
	
	return (StageBack*)this;
}
