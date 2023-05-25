/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "gfdanger.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

#include "speaker.h"

//GFDanger character structure
enum
{
  GFDanger_ArcMain_GFDanger0,
  GFDanger_ArcMain_GFDanger1,
  GFDanger_ArcMain_GFDanger2,
  GFDanger_ArcMain_GFDanger3,
  GFDanger_ArcMain_GFDanger4,
  GFDanger_ArcMain_GFDanger5,
  GFDanger_ArcMain_GFDanger6,
  GFDanger_ArcMain_GFDanger7,
	
	GFDanger_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[GFDanger_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
	
	//Speaker
	Speaker speaker;
	
} Char_GFDanger;

//GFDanger character definitions
static const CharFrame char_gfdanger_frame[] = {
  {GFDanger_ArcMain_GFDanger0, {  0,  0,119,103}, {150-63,152-82}}, //0 gfdanger 1
  {GFDanger_ArcMain_GFDanger0, {119,  0,129,101}, {159-63,149-82}}, //1 gfdanger 2
  {GFDanger_ArcMain_GFDanger0, {  0,103,114,104}, {145-63,152-82}}, //2 gfdanger 3
  {GFDanger_ArcMain_GFDanger0, {114,103,120,103}, {151-63,152-82}}, //3 gfdanger 4
  {GFDanger_ArcMain_GFDanger1, {  0,  0,130,101}, {160-63,150-82}}, //4 gfdanger 5
  {GFDanger_ArcMain_GFDanger1, {130,  0,114,104}, {144-63,153-82}}, //5 gfdanger 6
  {GFDanger_ArcMain_GFDanger1, {  0,104,120,104}, {150-63,154-82}}, //6 gfdanger 7
  {GFDanger_ArcMain_GFDanger1, {120,104,120,108}, {146-63,157-82}}, //7 gfdanger 8
  {GFDanger_ArcMain_GFDanger2, {  0,  0,131,106}, {156-63,156-82}}, //8 gfdanger 9
  {GFDanger_ArcMain_GFDanger2, {131,  0,118,106}, {144-63,155-82}}, //9 gfdanger 10
  {GFDanger_ArcMain_GFDanger2, {  0,106,121,108}, {146-63,158-82}}, //10 gfdanger 11
  {GFDanger_ArcMain_GFDanger2, {121,106,133,106}, {157-63,156-82}}, //11 gfdanger 12
  {GFDanger_ArcMain_GFDanger3, {  0,  0,120,106}, {145-63,156-82}}, //12 gfdanger 13
  {GFDanger_ArcMain_GFDanger3, {120,  0,121,108}, {146-63,158-82}}, //13 gfdanger 14
  {GFDanger_ArcMain_GFDanger3, {  0,108,133,107}, {157-63,156-82}}, //14 gfdanger 15
  
  {GFDanger_ArcMain_GFDanger3, {133,108,118,104}, {141-63,153-82}}, //15 gfdanger 16
  {GFDanger_ArcMain_GFDanger4, {  0,  0,119,108}, {142-63,157-82}}, //16 gfdanger 17
  {GFDanger_ArcMain_GFDanger4, {119,  0,129,105}, {154-63,154-82}}, //17 gfdanger 18
  {GFDanger_ArcMain_GFDanger4, {  0,108,118,104}, {141-63,153-82}}, //18 gfdanger 19
  {GFDanger_ArcMain_GFDanger4, {118,108,118,106}, {142-63,156-82}}, //19 gfdanger 20
  {GFDanger_ArcMain_GFDanger5, {  0,  0,129,104}, {153-63,154-82}}, //20 gfdanger 21
  {GFDanger_ArcMain_GFDanger5, {129,  0,118,104}, {142-63,153-82}}, //21 gfdanger 22
  {GFDanger_ArcMain_GFDanger5, {  0,104,119,106}, {144-63,155-82}}, //22 gfdanger 23
  {GFDanger_ArcMain_GFDanger5, {119,104,131,104}, {155-63,153-82}}, //23 gfdanger 24
  {GFDanger_ArcMain_GFDanger6, {  0,  0,116,104}, {141-63,154-82}}, //24 gfdanger 25
  {GFDanger_ArcMain_GFDanger6, {116,  0,119,106}, {144-63,155-82}}, //25 gfdanger 26
  {GFDanger_ArcMain_GFDanger6, {  0,106,130,104}, {155-63,153-82}}, //26 gfdanger 27
  {GFDanger_ArcMain_GFDanger6, {130,106,116,104}, {141-63,154-82}}, //27 gfdanger 28
  {GFDanger_ArcMain_GFDanger7, {  0,  0,119,106}, {145-63,155-82}}, //28 gfdanger 29
  {GFDanger_ArcMain_GFDanger7, {119,  0,130,103}, {155-63,153-82}}, //29 gfdanger 30
};

static const Animation char_gfdanger_anim[CharAnim_Max] = {
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

//GFDanger character functions
void Char_GFDanger_SetFrame(void *user, u8 frame)
{
	Char_GFDanger *this = (Char_GFDanger*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_gfdanger_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_GFDanger_Tick(Character *character)
{
	Char_GFDanger *this = (Char_GFDanger*)character;
	
	//Initialize Pico test
	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{			
		//Perform dance
	    if (stage.chart.note_scroll >= character->sing_end && (stage.song_step % stage.gf_speed) == 0)
		{
			//Switch animation
			if (character->animatable.anim == CharAnim_LeftAlt || character->animatable.anim == CharAnim_Right)
				character->set_anim(character, CharAnim_RightAlt);
			else
				character->set_anim(character, CharAnim_LeftAlt);
			
			//Bump speakers
			Speaker_Bump(&this->speaker);
		}
	}

	//Get parallax
	fixed_t parallax;
	parallax = FIXED_UNIT;
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_GFDanger_SetFrame);
	Character_DrawParallax(character, &this->tex, &char_gfdanger_frame[this->frame], parallax);
	
	//Tick speakers
	Speaker_Tick(&this->speaker, character->x, character->y, parallax);
	
	//Bye
	if ((stage.song_step >= 656) && (stage.song_step <= 662))
	{
		stage.gf->x -= FIXED_DEC(2,1);
	}
	if ((stage.song_step >= 663) && (stage.song_step <= 670))
	{
		stage.gf->x -= FIXED_DEC(3,1);
	}
	if ((stage.song_step >= 671) && (stage.song_step <= 720))
	{
		stage.gf->x -= FIXED_DEC(4,1);
	}
}

void Char_GFDanger_SetAnim(Character *character, u8 anim)
{
	//Set animation
	if (anim == CharAnim_Left || anim == CharAnim_Down || anim == CharAnim_Up || anim == CharAnim_Right || anim == CharAnim_UpAlt)
		character->sing_end = stage.chart.note_scroll + FIXED_DEC(22,1); //Nearly 2 steps
	Animatable_SetAnim(&character->animatable, anim);
}

void Char_GFDanger_Free(Character *character)
{
	Char_GFDanger *this = (Char_GFDanger*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_GFDanger_New(fixed_t x, fixed_t y)
{
	//Allocate gfdanger object
	Char_GFDanger *this = Mem_Alloc(sizeof(Char_GFDanger));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_GFDanger_New] Failed to allocate gfdanger object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_GFDanger_Tick;
	this->character.set_anim = Char_GFDanger_SetAnim;
	this->character.free = Char_GFDanger_Free;
	
	Animatable_Init(&this->character.animatable, char_gfdanger_anim);
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
	this->arc_main = IO_Read("\\GF\\GFDANGER.ARC;1");
		
	const char **pathp = (const char *[]){
  "gf0.tim",
  "gf1.tim",
  "gf2.tim",
  "gf3.tim",
  "gf4.tim",
  "gf5.tim",
  "gf6.tim",
  "gf7.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	//Initialize speaker
	Speaker_Init(&this->speaker);
	
	return (Character*)this;
}
