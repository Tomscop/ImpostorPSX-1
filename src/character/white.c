/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "white.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//White character structure
enum
{
  White_ArcMain_Idle0,
  White_ArcMain_Idle1,
  White_ArcMain_Idle2,
  White_ArcMain_Left0,
  White_ArcMain_Left1,
  White_ArcMain_Down0,
  White_ArcMain_Up0,
  White_ArcMain_Up1,
  White_ArcMain_Right0,
  White_ArcMain_Right1,
	
	White_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[White_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_White;

//White character definitions
static const CharFrame char_white_frame[] = {
  {White_ArcMain_Idle0, {  0,  0,167,124}, {136,100}}, //0 idle 1
  {White_ArcMain_Idle0, {  0,124,165,123}, {134, 99}}, //1 idle 2
  {White_ArcMain_Idle1, {  0,  0,165,122}, {135, 99}}, //2 idle 3
  {White_ArcMain_Idle1, {  0,122,168,118}, {139,100}}, //3 idle 4
  {White_ArcMain_Idle2, {  0,  0,168,117}, {139,101}}, //4 idle 5
  {White_ArcMain_Idle2, {  0,117,169,117}, {140,101}}, //5 idle 6

  {White_ArcMain_Left0, {  0,  0,175,112}, {165,106}}, //6 left 1
  {White_ArcMain_Left0, {  0,114,174,114}, {163,108}}, //7 left 2
  {White_ArcMain_Left1, {  0,  0,173,114}, {162,108}}, //8 left 3

  {White_ArcMain_Down0, {  0,  0,110,105}, { 85, 85}}, //9 down 1
  {White_ArcMain_Down0, {110,  0,106,103}, { 85, 86}}, //10 down 2
  {White_ArcMain_Down0, {  0,105,106,103}, { 85, 86}}, //11 down 3

  {White_ArcMain_Up0, {  0,  0,172,121}, {141,115}}, //12 up 1
  {White_ArcMain_Up0, {  0,122,167,122}, {140,116}}, //13 up 2
  {White_ArcMain_Up1, {  0,  0,168,121}, {141,115}}, //14 up 3

  {White_ArcMain_Right0, {  0,  0,152,108}, { 76,102}}, //15 right 1
  {White_ArcMain_Right0, {  0,108,154,108}, { 80,102}}, //16 right 2
  {White_ArcMain_Right1, {  0,  0,154,108}, { 79,102}}, //17 right 3
};

static const Animation char_white_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, 5, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 6, 7, 8, 8, 8, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 9, 10, 11, 11, 11, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 12, 13, 14, 14, 14, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 15, 16, 17, 17, 17, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//White character functions
void Char_White_SetFrame(void *user, u8 frame)
{
	Char_White *this = (Char_White*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_white_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_White_Tick(Character *character)
{
	Char_White *this = (Char_White*)character;
	
	//Camera stuff
	if (stage.stage_id == StageId_Oversight)
	{
		if (stage.song_step == 0)
			this->character.focus_zoom = FIXED_DEC(814,1024);
		if (stage.song_step == 384)
			this->character.focus_zoom = FIXED_DEC(855+24,1024);
		if (stage.song_step == 387)
			this->character.focus_zoom = FIXED_DEC(875+24,1024);
		if (stage.song_step == 390)
			this->character.focus_zoom = FIXED_DEC(895+24,1024);
		if (stage.song_step == 392)
			this->character.focus_zoom = FIXED_DEC(936+24,1024);
		if (stage.song_step == 400)
			this->character.focus_zoom = FIXED_DEC(814,1024);
		if (stage.song_step == 416)
			this->character.focus_zoom = FIXED_DEC(855+24,1024);
		if (stage.song_step == 419)
			this->character.focus_zoom = FIXED_DEC(875+24,1024);
		if (stage.song_step == 422)
			this->character.focus_zoom = FIXED_DEC(895+24,1024);
		if (stage.song_step == 424)
			this->character.focus_zoom = FIXED_DEC(936+24,1024);
		if (stage.song_step == 432)
			this->character.focus_zoom = FIXED_DEC(814,1024);
		if (stage.song_step == 448)
			this->character.focus_zoom = FIXED_DEC(855+24,1024);
		if (stage.song_step == 451)
			this->character.focus_zoom = FIXED_DEC(875+24,1024);
		if (stage.song_step == 454)
			this->character.focus_zoom = FIXED_DEC(895+24,1024);
		if (stage.song_step == 456)
			this->character.focus_zoom = FIXED_DEC(936+24,1024);
		if (stage.song_step == 464)
			this->character.focus_zoom = FIXED_DEC(814,1024);
		if (stage.song_step == 480)
			this->character.focus_zoom = FIXED_DEC(855+24,1024);
		if (stage.song_step == 483)
			this->character.focus_zoom = FIXED_DEC(875+24,1024);
		if (stage.song_step == 486)
			this->character.focus_zoom = FIXED_DEC(895+24,1024);
		if (stage.song_step == 488)
			this->character.focus_zoom = FIXED_DEC(936+24,1024);
		if (stage.song_step == 496)
			this->character.focus_zoom = FIXED_DEC(814,1024);
		if (stage.song_step == 511)
			this->character.focus_zoom = FIXED_DEC(912+24,1024);
		if (stage.song_step == 640)
			this->character.focus_zoom = FIXED_DEC(814,1024);
		if (stage.song_step == 896)
			this->character.focus_zoom = FIXED_DEC(855+24,1024);
		if (stage.song_step == 899)
			this->character.focus_zoom = FIXED_DEC(875+24,1024);
		if (stage.song_step == 902)
			this->character.focus_zoom = FIXED_DEC(895+24,1024);
		if (stage.song_step == 904)
			this->character.focus_zoom = FIXED_DEC(936+24,1024);
		if (stage.song_step == 912)
			this->character.focus_zoom = FIXED_DEC(814,1024);
		if (stage.song_step == 928)
			this->character.focus_zoom = FIXED_DEC(855+24,1024);
		if (stage.song_step == 931)
			this->character.focus_zoom = FIXED_DEC(875+24,1024);
		if (stage.song_step == 934)
			this->character.focus_zoom = FIXED_DEC(895+24,1024);
		if (stage.song_step == 936)
			this->character.focus_zoom = FIXED_DEC(936+24,1024);
		if (stage.song_step == 944)
			this->character.focus_zoom = FIXED_DEC(814,1024);
		if (stage.song_step == 960)
			this->character.focus_zoom = FIXED_DEC(855+24,1024);
		if (stage.song_step == 963)
			this->character.focus_zoom = FIXED_DEC(875+24,1024);
		if (stage.song_step == 966)
			this->character.focus_zoom = FIXED_DEC(895+24,1024);
		if (stage.song_step == 968)
			this->character.focus_zoom = FIXED_DEC(936+24,1024);
		if (stage.song_step == 976)
			this->character.focus_zoom = FIXED_DEC(814,1024);
		if (stage.song_step == 992)
			this->character.focus_zoom = FIXED_DEC(855+24,1024);
		if (stage.song_step == 995)
			this->character.focus_zoom = FIXED_DEC(875+24,1024);
		if (stage.song_step == 998)
			this->character.focus_zoom = FIXED_DEC(895+24,1024);
		if (stage.song_step == 1000)
			this->character.focus_zoom = FIXED_DEC(936+24,1024);
		if (stage.song_step == 1008)
			this->character.focus_zoom = FIXED_DEC(814,1024);
	}
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_White_SetFrame);
	Character_Draw(character, &this->tex, &char_white_frame[this->frame]);
}

void Char_White_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_White_Free(Character *character)
{
	Char_White *this = (Char_White*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_White_New(fixed_t x, fixed_t y)
{
	//Allocate white object
	Char_White *this = Mem_Alloc(sizeof(Char_White));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_White_New] Failed to allocate white object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_White_Tick;
	this->character.set_anim = Char_White_SetAnim;
	this->character.free = Char_White_Free;
	
	Animatable_Init(&this->character.animatable, char_white_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 2;

	//health bar color
	this->character.health_bar = 0xFFD1D2F8;
	
	this->character.focus_x = FIXED_DEC(14,1);
	this->character.focus_y = FIXED_DEC(-79,1);
	this->character.focus_zoom = FIXED_DEC(59,64);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\OPPONENT\\WHITE.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "idle2.tim",
  "left0.tim",
  "left1.tim",
  "down0.tim",
  "up0.tim",
  "up1.tim",
  "right0.tim",
  "right1.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
