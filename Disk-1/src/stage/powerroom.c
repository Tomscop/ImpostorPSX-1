/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "powerroom.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

//Power Room background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	IO_Data arc_black, arc_black_ptr[5];
	Gfx_Tex tex_back0; //back0
	Gfx_Tex tex_back1; //back1
	Gfx_Tex tex_back2; //back2
	Gfx_Tex tex_back3; //back3
	
	//Black state
	Gfx_Tex tex_black;
	u8 black_frame, black_tex_id;
	Animatable black_animatable;

} Back_PowerRoom;

//Black animation and rects
static const CharFrame black_frame[] = {
  {0, {  0,  0,141,116}, {158,159}}, //0 black 1
  {0, {  0,116,143,115}, {158,158}}, //1 black 2
  {1, {  0,  0,142,115}, {158,158}}, //2 black 3
  {1, {  0,115,144,114}, {158,157}}, //3 black 4
  {2, {  0,  0,142,114}, {158,157}}, //4 black 5
  {2, {  0,114,140,116}, {158,158}}, //5 black 6
  {3, {  0,  0,140,116}, {159,158}}, //6 black 7
  {3, {  0,116,139,116}, {158,159}}, //7 black 8
  {4, {  0,  0,139,116}, {158,159}}, //8 black 9
};

static const Animation black_anim[] = {
	{1, (const u8[]){0, 0, 1, 2, 3, 3, 4, 4, 5, 6, 7, 8, 8, ASCR_BACK, 1}}, //Idle
};

//Black functions
void PowerRoom_Black_SetFrame(void *user, u8 frame)
{
	Back_PowerRoom *this = (Back_PowerRoom*)user;
	
	//Check if this is a new frame
	if (frame != this->black_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &black_frame[this->black_frame = frame];
		if (cframe->tex != this->black_tex_id)
			Gfx_LoadTex(&this->tex_black, this->arc_black_ptr[this->black_tex_id = cframe->tex], 0);
	}
}

void PowerRoom_Black_Draw(Back_PowerRoom *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &black_frame[this->black_frame];
    
    fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = { ox, oy, (src.w * FIXED_DEC(139,100)),(src.h * FIXED_DEC(139,100))};
	Debug_StageMoveDebug(&dst, 8, stage.camera.x, stage.camera.y);
	Stage_DrawTex(&this->tex_black, &src, &dst, stage.camera.bzoom);
}

void Back_PowerRoom_DrawFG(StageBack *back)
{
	Back_PowerRoom *this = (Back_PowerRoom*)back;

	fixed_t fx, fy;

	//Draw powerroom
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	//Draw shade
	RECT shade_src = {106, 93,  1,  1};
	RECT_FIXED shade_dst = {
		FIXED_DEC(-352,1),
		FIXED_DEC(-266,1),
		FIXED_DEC(565,1),
		FIXED_DEC(404,1)
	};
	Debug_StageMoveDebug(&shade_dst, 11, shade_dst.x, shade_dst.y);
	Stage_BlendTex(&this->tex_back0, &shade_src, &shade_dst, FIXED_DEC(1,1), 1);
	
	RECT back2_src = {  0,  0,253, 55};
	RECT_FIXED back2_dst = {
		FIXED_DEC(129 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(189,1) - fy,
		FIXED_DEC(253 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(55,1)
	};
	
	Debug_StageMoveDebug(&back2_dst, 7, fx, fy);
	Stage_DrawTex(&this->tex_back2, &back2_src, &back2_dst, stage.camera.bzoom);
}

void Back_PowerRoom_DrawBG(StageBack *back)
{
	Back_PowerRoom *this = (Back_PowerRoom*)back;

	fixed_t fx, fy;
	
	//Draw powerroom	
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	//Animate and draw black
	if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step & 0x7) == 0)
		Animatable_SetAnim(&this->black_animatable, 0);
	Animatable_Animate(&this->black_animatable, (void*)this, PowerRoom_Black_SetFrame);
	PowerRoom_Black_Draw(this, FIXED_DEC(-58 + 158,1) - fx, FIXED_DEC(30 + 159,1) - fy);
	
	RECT back0_src = {  0,  0,255,223};
	RECT_FIXED back0_dst = {
		FIXED_DEC(0 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(266 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(233,1)
	};
	
	RECT back1_src = {  0,  0,255,223};
	RECT_FIXED back1_dst = {
		FIXED_DEC(265 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(266 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(233,1)
	};
	
	RECT back3_src = {  0,  0,117,132};
	RECT_FIXED back3_dst = {
		FIXED_DEC(372 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(48,1) - fy,
		FIXED_DEC(122 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(138,1)
	};
	
	Debug_StageMoveDebug(&back3_dst, 9, fx, fy);
	Stage_DrawTex(&this->tex_back3, &back3_src, &back3_dst, stage.camera.bzoom);
	Debug_StageMoveDebug(&back0_dst, 5, fx, fy);
	Debug_StageMoveDebug(&back1_dst, 6, fx, fy);
	Stage_DrawTex(&this->tex_back0, &back0_src, &back0_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back1, &back1_src, &back1_dst, stage.camera.bzoom);
}

void Back_PowerRoom_Free(StageBack *back)
{
	Back_PowerRoom *this = (Back_PowerRoom*)back;
	
	//Free black archive
	Mem_Free(this->arc_black);
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_PowerRoom_New(void)
{
	//Allocate background structure
	Back_PowerRoom *this = (Back_PowerRoom*)Mem_Alloc(sizeof(Back_PowerRoom));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = Back_PowerRoom_DrawFG;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_PowerRoom_DrawBG;
	this->back.free = Back_PowerRoom_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\BG\\POWEROOM.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_back2, Archive_Find(arc_back, "back2.tim"), 0);
	Gfx_LoadTex(&this->tex_back3, Archive_Find(arc_back, "back3.tim"), 0);
	Mem_Free(arc_back);
	
	//Load black textures
	this->arc_black = IO_Read("\\BG\\BLACKG.ARC;1");
	this->arc_black_ptr[0] = Archive_Find(this->arc_black, "black0.tim");
	this->arc_black_ptr[1] = Archive_Find(this->arc_black, "black1.tim");
	this->arc_black_ptr[2] = Archive_Find(this->arc_black, "black2.tim");
	this->arc_black_ptr[3] = Archive_Find(this->arc_black, "black3.tim");
	this->arc_black_ptr[4] = Archive_Find(this->arc_black, "black4.tim");
	
	//Initialize black state
	Animatable_Init(&this->black_animatable, black_anim);
	Animatable_SetAnim(&this->black_animatable, 0);
	this->black_frame = this->black_tex_id = 0xFF; //Force art load
	
	return (StageBack*)this;
}
