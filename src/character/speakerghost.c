/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "speakerghost.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//SpeakerGhost character structure
enum
{
  SpeakerGhost_ArcMain_SG0,
  SpeakerGhost_ArcMain_SG1,
  SpeakerGhost_ArcMain_SG2,
	
	SpeakerGhost_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[SpeakerGhost_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_SpeakerGhost;

//SpeakerGhost character definitions
static const CharFrame char_speakerghost_frame[] = {
  {SpeakerGhost_ArcMain_SG0, {  0,  0,175, 97}, {  0,  0}}, //0 idle 1
  {SpeakerGhost_ArcMain_SG0, {  0, 98,176, 97}, {  1,  0}}, //1 idle 2
  {SpeakerGhost_ArcMain_SG1, {  0,  0,176, 98}, {  1,  0}}, //2 idle 3
  {SpeakerGhost_ArcMain_SG1, {  0, 99,175, 98}, {  0,  0}}, //3 idle 4
  {SpeakerGhost_ArcMain_SG2, {  0,  0,175, 97}, {  0,  0}}, //4 idle 5
};

static const Animation char_speakerghost_anim[CharAnim_Max] = {
	{1, (const u8[]){ASCR_CHGANI, CharAnim_Idle}}, //CharAnim_Idle
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},         //CharAnim_Left
	{1, (const u8[]){ 0, 1, 2, 3, 4, ASCR_BACK, 1}},   //CharAnim_LeftAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},         //CharAnim_Right
	{1, (const u8[]){ 0, 1, 2, 3, 4, ASCR_BACK, 1}},   //CharAnim_RightAlt
};

//SpeakerGhost character functions
void Char_SpeakerGhost_SetFrame(void *user, u8 frame)
{
	Char_SpeakerGhost *this = (Char_SpeakerGhost*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_speakerghost_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_SpeakerGhost_Tick(Character *character)
{
	Char_SpeakerGhost *this = (Char_SpeakerGhost*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
	{
		Character_CheckEndSing(character);
		
		if (stage.flag & STAGE_FLAG_JUST_STEP)
		{
			if ((Animatable_Ended(&character->animatable) || character->animatable.anim == CharAnim_LeftAlt || character->animatable.anim == CharAnim_RightAlt) &&
				(character->animatable.anim != CharAnim_Left &&
				 character->animatable.anim != CharAnim_Down &&
				 character->animatable.anim != CharAnim_Up &&
				 character->animatable.anim != CharAnim_Right) &&
				(stage.song_step & 0x3) == 0)
				character->set_anim(character, CharAnim_Idle);
		}
	}
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_SpeakerGhost_SetFrame);
	Character_Draw(character, &this->tex, &char_speakerghost_frame[this->frame]);
}

void Char_SpeakerGhost_SetAnim(Character *character, u8 anim)
{
	//Set animation
	if (anim == CharAnim_Idle)
	{
		if (character->animatable.anim == CharAnim_LeftAlt)
			anim = CharAnim_RightAlt;
		else
			anim = CharAnim_LeftAlt;
		character->sing_end = FIXED_DEC(0x7FFF,1);
	}
	else
	{
		Character_CheckStartSing(character);
	}
	Animatable_SetAnim(&character->animatable, anim);
}

void Char_SpeakerGhost_Free(Character *character)
{
	Char_SpeakerGhost *this = (Char_SpeakerGhost*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_SpeakerGhost_New(fixed_t x, fixed_t y)
{
	//Allocate speakerghost object
	Char_SpeakerGhost *this = Mem_Alloc(sizeof(Char_SpeakerGhost));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_SpeakerGhost_New] Failed to allocate speakerghost object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_SpeakerGhost_Tick;
	this->character.set_anim = Char_SpeakerGhost_SetAnim;
	this->character.free = Char_SpeakerGhost_Free;
	
	Animatable_Init(&this->character.animatable, char_speakerghost_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;

	//health bar color
	this->character.health_bar = 0xFF625773;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-115,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\SPEAKERG.ARC;1");
	
	const char **pathp = (const char *[]){
  "speakerg0.tim",
  "speakerg1.tim",
  "speakerg2.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
