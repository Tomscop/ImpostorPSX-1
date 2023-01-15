/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "picorc.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Pico RC player types
enum
{
  PicoRC_ArcMain_Idle0,
  PicoRC_ArcMain_Idle1,
  PicoRC_ArcMain_Left0,
  PicoRC_ArcMain_Down0,
  PicoRC_ArcMain_Up0,
  PicoRC_ArcMain_Right0,
  PicoRC_ArcMain_LeftM0,
  PicoRC_ArcMain_DownM0,
  PicoRC_ArcMain_UpM0,
  PicoRC_ArcMain_UpM1,
  PicoRC_ArcMain_RightM0,
  PicoRC_ArcMain_RightM1,
	
	PicoRC_ArcMain_Max,
};

#define PicoRC_Arc_Max PicoRC_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[PicoRC_Arc_Max];
	
	Gfx_Tex tex, tex_retry;
	u8 frame, tex_id;
} Char_PicoRC;

//Pico RC player definitions
static const CharFrame char_picorc_frame[] = {
  {PicoRC_ArcMain_Idle0, {  0,  0,108,116}, {146,146}}, //0 idle 1
  {PicoRC_ArcMain_Idle0, {109,  0,110,117}, {149,147}}, //1 idle 2
  {PicoRC_ArcMain_Idle0, {  0,118,111,118}, {150,148}}, //2 idle 3
  {PicoRC_ArcMain_Idle0, {112,118,110,118}, {151,148}}, //3 idle 4
  {PicoRC_ArcMain_Idle1, {  0,  0,111,120}, {151,150}}, //4 idle 5
  {PicoRC_ArcMain_Idle1, {112,  0,110,120}, {151,150}}, //5 idle 6
  {PicoRC_ArcMain_Idle1, {  0,121,110,120}, {151,150}}, //6 idle 7

  {PicoRC_ArcMain_Left0, {  0,  0,112,118}, {171,147}}, //7 left 1
  {PicoRC_ArcMain_Left0, {113,  0,113,118}, {172,147}}, //8 left 2

  {PicoRC_ArcMain_Down0, {  0,  0,124, 95}, {169,127}}, //9 down 1
  {PicoRC_ArcMain_Down0, {125,  0,125, 98}, {172,130}}, //10 down 2

  {PicoRC_ArcMain_Up0, {  0,  0,114,125}, {155,157}}, //11 up 1
  {PicoRC_ArcMain_Up0, {115,  0,113,124}, {155,156}}, //12 up 2

  {PicoRC_ArcMain_Right0, {  0,  0,115,119}, {138,149}}, //13 right 1
  {PicoRC_ArcMain_Right0, {116,  0,110,118}, {138,148}}, //14 right 2

  {PicoRC_ArcMain_LeftM0, {  0,  0,114,125}, {171,155}}, //15 leftm 1
  {PicoRC_ArcMain_LeftM0, {115,  0,115,125}, {171,155}}, //16 leftm 2
  {PicoRC_ArcMain_LeftM0, {  0,126,114,125}, {171,155}}, //17 leftm 3

  {PicoRC_ArcMain_DownM0, {  0,  0,123,107}, {170,139}}, //18 downm 1
  {PicoRC_ArcMain_DownM0, {124,  0,124,106}, {171,138}}, //19 downm 2
  {PicoRC_ArcMain_DownM0, {  0,108,124,107}, {171,139}}, //20 downm 3

  {PicoRC_ArcMain_UpM0, {  0,  0,115,132}, {157,164}}, //21 upm 1
  {PicoRC_ArcMain_UpM0, {116,  0,114,132}, {156,164}}, //22 upm 2
  {PicoRC_ArcMain_UpM1, {  0,  0,114,132}, {156,164}}, //23 upm 3

  {PicoRC_ArcMain_RightM0, {  0,  0,110,131}, {140,161}}, //24 rightm 1
  {PicoRC_ArcMain_RightM0, {111,  0,111,130}, {140,160}}, //25 rightm 2
  {PicoRC_ArcMain_RightM1, {  0,  0,111,130}, {140,160}}, //26 rightm 3
};

static const Animation char_picorc_anim[PlayerAnim_Max] = {
	{1, (const u8[]){ 0, 0, 1, 1, 2, 3, 4, 5, 6, 6, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 7, 8, 8, 8, 8, 8, 8, 8, ASCR_BACK, 1}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{2, (const u8[]){ 9, 10, 10, 10, 10, ASCR_BACK, 1}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{2, (const u8[]){ 11, 12, 12, 12, 12, 12, 12, ASCR_BACK, 1}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{2, (const u8[]){ 13, 14, 14, 14, 14, 14, 14, 14, ASCR_BACK, 1}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
	
	{1, (const u8[]){ 15, 16, 17, ASCR_BACK, 1}},     //PlayerAnim_LeftMiss
	{1, (const u8[]){ 18, 19, 20, ASCR_BACK, 1}},     //PlayerAnim_DownMiss
	{1, (const u8[]){ 21, 22, 23, ASCR_BACK, 1}},     //PlayerAnim_UpMiss
	{1, (const u8[]){ 24, 25, 26, ASCR_BACK, 1}},     //PlayerAnim_RightMiss
};

//Pico RC player functions
void Char_PicoRC_SetFrame(void *user, u8 frame)
{
	Char_PicoRC *this = (Char_PicoRC*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_picorc_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_PicoRC_Tick(Character *character)
{
	Char_PicoRC *this = (Char_PicoRC*)character;
	
	//Handle animation updates
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0 ||
	    (character->animatable.anim != CharAnim_Left &&
	     character->animatable.anim != CharAnim_LeftAlt &&
	     character->animatable.anim != CharAnim_Down &&
	     character->animatable.anim != CharAnim_DownAlt &&
	     character->animatable.anim != CharAnim_Up &&
	     character->animatable.anim != CharAnim_UpAlt &&
	     character->animatable.anim != CharAnim_Right &&
	     character->animatable.anim != CharAnim_RightAlt))
		Character_CheckEndSing(character);
	
	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{
		//Perform idle dance
		if (Animatable_Ended(&character->animatable) &&
			(character->animatable.anim != CharAnim_Left &&
		     character->animatable.anim != CharAnim_LeftAlt &&
		     character->animatable.anim != PlayerAnim_LeftMiss &&
		     character->animatable.anim != CharAnim_Down &&
		     character->animatable.anim != CharAnim_DownAlt &&
		     character->animatable.anim != PlayerAnim_DownMiss &&
		     character->animatable.anim != CharAnim_Up &&
		     character->animatable.anim != CharAnim_UpAlt &&
		     character->animatable.anim != PlayerAnim_UpMiss &&
		     character->animatable.anim != CharAnim_Right &&
		     character->animatable.anim != CharAnim_RightAlt &&
		     character->animatable.anim != PlayerAnim_RightMiss) &&
			(stage.song_step & 0x7) == 0)
			character->set_anim(character, CharAnim_Idle);
			
		if (character->idle2 == 1)
		{
			if (Animatable_Ended(&character->animatable) &&
			(character->animatable.anim != CharAnim_Left &&
		     character->animatable.anim != PlayerAnim_LeftMiss &&
		     character->animatable.anim != CharAnim_Down &&
		     character->animatable.anim != CharAnim_DownAlt &&
		     character->animatable.anim != PlayerAnim_DownMiss &&
		     character->animatable.anim != CharAnim_Up &&
		     character->animatable.anim != CharAnim_UpAlt &&
		     character->animatable.anim != PlayerAnim_UpMiss &&
		     character->animatable.anim != CharAnim_Right &&
		     character->animatable.anim != CharAnim_RightAlt &&
		     character->animatable.anim != PlayerAnim_RightMiss) &&
			(stage.song_step & 0x7) == 3)
			character->set_anim(character, CharAnim_LeftAlt);
		}
	}
	
	//Animate and draw character
	Animatable_Animate(&character->animatable, (void*)this, Char_PicoRC_SetFrame);
	Character_Draw(character, &this->tex, &char_picorc_frame[this->frame]);
}

void Char_PicoRC_SetAnim(Character *character, u8 anim)
{
	Char_PicoRC *this = (Char_PicoRC*)character;
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_PicoRC_Free(Character *character)
{
	Char_PicoRC *this = (Char_PicoRC*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_PicoRC_New(fixed_t x, fixed_t y)
{
	//Allocate picorc object
	Char_PicoRC *this = Mem_Alloc(sizeof(Char_PicoRC));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_PicoRC_New] Failed to allocate picorc object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_PicoRC_Tick;
	this->character.set_anim = Char_PicoRC_SetAnim;
	this->character.free = Char_PicoRC_Free;
	
	Animatable_Init(&this->character.animatable, char_picorc_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = CHAR_SPEC_MISSANIM;
	this->character.idle2 = 0;

	//health bar color
	this->character.health_bar = 0xFFB7D855;
	
	this->character.focus_x = FIXED_DEC(-168,1);
	this->character.focus_y = FIXED_DEC(-126,1);
	this->character.focus_zoom = FIXED_DEC(271,256);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\PICORC.ARC;1");
		
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "left0.tim",
  "down0.tim",
  "up0.tim",
  "right0.tim",
  "leftm0.tim",
  "downm0.tim",
  "upm0.tim",
  "upm1.tim",
  "rightm0.tim",
  "rightm1.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
