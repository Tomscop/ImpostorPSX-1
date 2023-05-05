/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "gfejected.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//GFEjected character structure
enum
{
  GFEjected_ArcMain_GF0,
  GFEjected_ArcMain_GF1,
  GFEjected_ArcMain_GF2,
  GFEjected_ArcMain_GF3,
  GFEjected_ArcMain_GF4,
  GFEjected_ArcMain_GF5,
  GFEjected_ArcMain_GF6,
  GFEjected_ArcMain_GF7,
  GFEjected_ArcMain_GF8,
  GFEjected_ArcMain_GF9,
	
	GFEjected_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[GFEjected_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
	
} Char_GFEjected;

//GFEjected character definitions
static const CharFrame char_gfejected_frame[] = {
  {GFEjected_ArcMain_GF0, {  0,  0, 78,175}, { 87,154}}, //0 gf 1
  {GFEjected_ArcMain_GF0, { 78,  0, 74,172}, { 83,151}}, //1 gf 2
  {GFEjected_ArcMain_GF0, {152,  0, 80,171}, { 89,151}}, //2 gf 3
  {GFEjected_ArcMain_GF1, {  0,  0, 78,175}, { 87,154}}, //3 gf 4
  {GFEjected_ArcMain_GF1, { 78,  0, 74,171}, { 83,152}}, //4 gf 5
  {GFEjected_ArcMain_GF1, {152,  0, 80,170}, { 88,151}}, //5 gf 6
  {GFEjected_ArcMain_GF2, {  0,  0, 78,175}, { 86,155}}, //6 gf 7
  {GFEjected_ArcMain_GF2, { 78,  0, 75,177}, { 75,158}}, //7 gf 8
  {GFEjected_ArcMain_GF2, {153,  0, 79,171}, { 81,152}}, //8 gf 9
  {GFEjected_ArcMain_GF3, {  0,  0, 77,174}, { 76,154}}, //9 gf 10
  {GFEjected_ArcMain_GF3, { 77,  0, 76,177}, { 75,158}}, //10 gf 11
  {GFEjected_ArcMain_GF3, {153,  0, 81,169}, { 81,150}}, //11 gf 12
  {GFEjected_ArcMain_GF4, {  0,  0, 79,172}, { 77,153}}, //12 gf 13
  {GFEjected_ArcMain_GF4, { 79,  0, 76,177}, { 75,158}}, //13 gf 14
  {GFEjected_ArcMain_GF4, {155,  0, 81,169}, { 81,150}}, //14 gf 15
  
  {GFEjected_ArcMain_GF5, {  0,  0, 82,165}, { 72,146}}, //15 gf 16
  {GFEjected_ArcMain_GF5, { 82,  0, 80,172}, { 70,153}}, //16 gf 17
  {GFEjected_ArcMain_GF5, {162,  0, 79,171}, { 70,154}}, //17 gf 18
  {GFEjected_ArcMain_GF6, {  0,  0, 83,163}, { 75,144}}, //18 gf 19
  {GFEjected_ArcMain_GF6, { 83,  0, 81,167}, { 72,147}}, //19 gf 20
  {GFEjected_ArcMain_GF6, {164,  0, 78,174}, { 71,154}}, //20 gf 21
  {GFEjected_ArcMain_GF7, {  0,  0, 81,167}, { 77,147}}, //21 gf 22
  {GFEjected_ArcMain_GF7, { 81,  0, 77,176}, { 79,156}}, //22 gf 23
  {GFEjected_ArcMain_GF7, {158,  0, 72,175}, { 75,156}}, //23 gf 24
  {GFEjected_ArcMain_GF8, {  0,  0, 79,173}, { 82,153}}, //24 gf 25
  {GFEjected_ArcMain_GF8, { 79,  0, 77,176}, { 80,155}}, //25 gf 26
  {GFEjected_ArcMain_GF8, {156,  0, 74,176}, { 76,155}}, //26 gf 27
  {GFEjected_ArcMain_GF9, {  0,  0, 80,173}, { 83,152}}, //27 gf 28
  {GFEjected_ArcMain_GF9, { 80,  0, 73,175}, { 76,155}}, //28 gf 29
  {GFEjected_ArcMain_GF9, {153,  0, 79,172}, { 83,152}}, //29 gf 30
};

static const Animation char_gfejected_anim[CharAnim_Max] = {
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                        //CharAnim_Idle
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                                 //CharAnim_Left
	{1, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, ASCR_BACK, 1}}, //CharAnim_LeftAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                                 //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                      //CharAnim_DownAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                                 //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                                 //CharAnim_UpAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                                 //CharAnim_Right
	{1, (const u8[]){ 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, ASCR_BACK, 1}}, //CharAnim_RightAlt
};

//GFEjected character functions
void Char_GFEjected_SetFrame(void *user, u8 frame)
{
	Char_GFEjected *this = (Char_GFEjected*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_gfejected_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_GFEjected_Tick(Character *character)
{
	Char_GFEjected *this = (Char_GFEjected*)character;
	
	//Initialize Pico test
	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{			
		//Perform dance
	    if ((stage.chart.note_scroll >= character->sing_end && (stage.song_step % stage.gf_speed) == 0) && (character->animatable.anim  != CharAnim_Special1))
		{
			//Switch animation
			if (character->animatable.anim == CharAnim_LeftAlt || character->animatable.anim == CharAnim_Right)
				character->set_anim(character, CharAnim_RightAlt);
			else
				character->set_anim(character, CharAnim_LeftAlt);
		}
	}

	//Get parallax
	fixed_t parallax;
	parallax = FIXED_UNIT;
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_GFEjected_SetFrame);
	Character_DrawParallax(character, &this->tex, &char_gfejected_frame[this->frame], parallax);
}

void Char_GFEjected_SetAnim(Character *character, u8 anim)
{
	//Set animation
	if (anim == CharAnim_Left || anim == CharAnim_Down || anim == CharAnim_Up || anim == CharAnim_Right || anim == CharAnim_UpAlt)
		character->sing_end = stage.chart.note_scroll + FIXED_DEC(22,1); //Nearly 2 steps
	Animatable_SetAnim(&character->animatable, anim);
}

void Char_GFEjected_Free(Character *character)
{
	Char_GFEjected *this = (Char_GFEjected*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_GFEjected_New(fixed_t x, fixed_t y)
{
	//Allocate gfejected object
	Char_GFEjected *this = Mem_Alloc(sizeof(Char_GFEjected));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_GFEjected_New] Failed to allocate gfejected object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_GFEjected_Tick;
	this->character.set_anim = Char_GFEjected_SetAnim;
	this->character.free = Char_GFEjected_Free;
	
	Animatable_Init(&this->character.animatable, char_gfejected_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 2;

	//health bar color
	this->character.health_bar = 0xFFA5004A;
	
	this->character.focus_x = FIXED_DEC(2,1);
	this->character.focus_y = FIXED_DEC(-40,1);
	this->character.focus_zoom = FIXED_DEC(2,1);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\GF\\GFEJECT.ARC;1");
		
	const char **pathp = (const char *[]){
  "gf0.tim",
  "gf1.tim",
  "gf2.tim",
  "gf3.tim",
  "gf4.tim",
  "gf5.tim",
  "gf6.tim",
  "gf7.tim",
  "gf8.tim",
  "gf9.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
