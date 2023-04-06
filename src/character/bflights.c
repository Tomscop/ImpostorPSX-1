/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "bflights.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Boyfriend Lights player types
enum
{
	BFLights_ArcMain_BFLights0,
	BFLights_ArcMain_BFLights1,
	BFLights_ArcMain_BFLights2,
	BFLights_ArcMain_BFLights3,
	BFLights_ArcMain_BFLights4,
	BFLights_ArcMain_BFLights5,
	BFLights_ArcMain_BFLights6,
  BFLights_ArcMain_Idle0,
  BFLights_ArcMain_Idle1,
  BFLights_ArcMain_Left0,
  BFLights_ArcMain_Down0,
  BFLights_ArcMain_Up0,
  BFLights_ArcMain_Right0,
  BFLights_ArcMain_LeftM0,
  BFLights_ArcMain_DownM0,
  BFLights_ArcMain_UpM0,
  BFLights_ArcMain_RightM0,
	
	BFLights_ArcMain_Max,
};

#define BFLights_Arc_Max BFLights_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[BFLights_Arc_Max];
	
	Gfx_Tex tex, tex_retry;
	u8 frame, tex_id;
} Char_BFLights;

//Boyfriend Lights player definitions
static const CharFrame char_bflights_frame[] = {
	{BFLights_ArcMain_BFLights0, {  0,   0, 102,  99}, { 53,  92}}, //0 idle 1
	{BFLights_ArcMain_BFLights0, {103,   0, 102,  99}, { 53,  92}}, //1 idle 2
	{BFLights_ArcMain_BFLights0, {  0, 100, 102, 101}, { 53,  94}}, //2 idle 3
	{BFLights_ArcMain_BFLights0, {103, 100, 103, 104}, { 53,  97}}, //3 idle 4
	{BFLights_ArcMain_BFLights1, {  0,   0, 103, 104}, { 53,  97}}, //4 idle 5
	
	{BFLights_ArcMain_BFLights1, {104,   0,  96, 102}, { 56,  95}}, //5 left 1
	{BFLights_ArcMain_BFLights1, {  0, 105,  94, 102}, { 54,  95}}, //6 left 2
	
	{BFLights_ArcMain_BFLights1, { 95, 103,  94,  89}, { 52,  82}}, //7 down 1
	{BFLights_ArcMain_BFLights2, {  0,   0,  94,  90}, { 52,  83}}, //8 down 2
	
	{BFLights_ArcMain_BFLights2, { 95,   0,  93, 112}, { 41, 104}}, //9 up 1
	{BFLights_ArcMain_BFLights2, {  0,  91,  94, 111}, { 42, 103}}, //10 up 2
	
	{BFLights_ArcMain_BFLights2, { 95, 113, 102, 102}, { 41,  95}}, //11 right 1
	{BFLights_ArcMain_BFLights3, {  0,   0, 102, 102}, { 41,  95}}, //12 right 2
	
	{BFLights_ArcMain_BFLights3, {103,   0,  99, 105}, { 54,  98}}, //13 peace 1
	{BFLights_ArcMain_BFLights3, {  0, 103, 104, 103}, { 54,  96}}, //14 peace 2
	{BFLights_ArcMain_BFLights3, {105, 106, 104, 104}, { 54,  97}}, //15 peace 3
	
	{BFLights_ArcMain_BFLights4, {  0,  0,102,103}, { 53,  96}}, //16 shock
	
	{BFLights_ArcMain_BFLights5, {  0,   0,  93, 108}, { 52, 101}}, //17 left miss 1
	{BFLights_ArcMain_BFLights5, { 94,   0,  93, 108}, { 52, 101}}, //18 left miss 2
	
	{BFLights_ArcMain_BFLights5, {  0, 109,  95,  98}, { 50,  90}}, //19 down miss 1
	{BFLights_ArcMain_BFLights5, { 96, 109,  95,  97}, { 50,  89}}, //20 down miss 2
	
	{BFLights_ArcMain_BFLights6, {  0,   0,  90, 107}, { 44,  99}}, //21 up miss 1
	{BFLights_ArcMain_BFLights6, { 91,   0,  89, 108}, { 44, 100}}, //22 up miss 2
	
	{BFLights_ArcMain_BFLights6, {  0, 108,  99, 108}, { 42, 101}}, //23 right miss 1
	{BFLights_ArcMain_BFLights6, {100, 109, 101, 108}, { 43, 101}}, //24 right miss 2
};

static const CharFrame char_bflights_frame2[] = {
  {BFLights_ArcMain_Idle0, {  0,  0,102, 99}, {159-105,155-63}}, //0 idle 1
  {BFLights_ArcMain_Idle0, {102,  0,103,100}, {159-105,155-63}}, //1 idle 2
  {BFLights_ArcMain_Idle0, {  0,100,103,100}, {158-105,155-63}}, //2 idle 3
  {BFLights_ArcMain_Idle0, {103,100,104,104}, {159-105,160-63}}, //3 idle 4
  {BFLights_ArcMain_Idle1, {  0,  0,103,104}, {158-105,160-63}}, //4 idle 5

  {BFLights_ArcMain_Left0, {  0,  0, 97,102}, {161-105,159-63}}, //5 left 1
  {BFLights_ArcMain_Left0, { 97,  0, 95,102}, {158-105,159-63}}, //6 left 2

  {BFLights_ArcMain_Down0, {  0,  0, 94, 90}, {154-105,147-63}}, //7 down 1
  {BFLights_ArcMain_Down0, { 94,  0, 94, 91}, {154-105,148-63}}, //8 down 2

  {BFLights_ArcMain_Up0, {  0,  0, 94,113}, {147-105,166-63}}, //9 up 1
  {BFLights_ArcMain_Up0, { 94,  0, 95,111}, {148-105,164-63}}, //10 up 2

  {BFLights_ArcMain_Right0, {  0,  0,102,102}, {146-105,157-63}}, //11 right 1
  {BFLights_ArcMain_Right0, {102,  0,102,103}, {146-105,159-63}}, //12 right 2
  
	{BFLights_ArcMain_BFLights3, {103,   0,  99, 105}, { 54,  98}}, //13 peace 1
	{BFLights_ArcMain_BFLights3, {  0, 103, 104, 103}, { 54,  96}}, //14 peace 2
	{BFLights_ArcMain_BFLights3, {105, 106, 104, 104}, { 54,  97}}, //15 peace 3
	
	{BFLights_ArcMain_BFLights4, {  0,  0,102,103}, { 53,  96}}, //16 shock

  {BFLights_ArcMain_LeftM0, {  0,  0, 93,108}, {157-105,163-63}}, //17 leftm 1
  {BFLights_ArcMain_LeftM0, { 93,  0, 94,108}, {158-105,163-63}}, //18 leftm 2

  {BFLights_ArcMain_DownM0, {  0,  0, 95, 97}, {157-105,154-63}}, //19 downm 1
  {BFLights_ArcMain_DownM0, { 95,  0, 95, 96}, {156-105,153-63}}, //20 downm 2

  {BFLights_ArcMain_UpM0, {  0,  0, 91,107}, {151-105,162-63}}, //21 upm 1
  {BFLights_ArcMain_UpM0, { 91,  0, 90,107}, {150-105,162-63}}, //22 upm 2

  {BFLights_ArcMain_RightM0, {  0,  0,100,110}, {149-105,165-63}}, //23 rightm 1
  {BFLights_ArcMain_RightM0, {100,  0,101,108}, {149-105,164-63}}, //24 rightm 2
};

static const Animation char_bflights_anim[PlayerAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 5,  6, ASCR_BACK, 1}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{2, (const u8[]){ 7,  8, ASCR_BACK, 1}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{2, (const u8[]){ 9, 10, ASCR_BACK, 1}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{2, (const u8[]){ 11, 12, ASCR_BACK, 1}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
    
    {2, (const u8[]){ 16, ASCR_CHGANI, CharAnim_Special1}},   //CharAnim_Special1
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special2
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special3
	
	{1, (const u8[]){ 17, 17, 18, ASCR_BACK, 1}},     //PlayerAnim_LeftMiss
	{1, (const u8[]){ 19, 19, 20, ASCR_BACK, 1}},     //PlayerAnim_DownMiss
	{1, (const u8[]){ 21, 21, 22, ASCR_BACK, 1}},     //PlayerAnim_UpMiss
	{1, (const u8[]){ 23, 23, 24, ASCR_BACK, 1}},     //PlayerAnim_RightMiss
	
	{2, (const u8[]){ 13, 14, 15, ASCR_BACK, 1}},         //PlayerAnim_Peace
};

//Boyfriend Lights player functions
void Char_BFLights_SetFrame(void *user, u8 frame)
{
	Char_BFLights *this = (Char_BFLights*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		if (stage.lights != 1)
		{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_bflights_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
		}
		else
		{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_bflights_frame2[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
		}
	}
}

void Char_BFLights_Tick(Character *character)
{
	Char_BFLights *this = (Char_BFLights*)character;
	
	if(character->animatable.anim  != CharAnim_Special1)
	{
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
	}
	}
	
	//Animate and draw character
	Animatable_Animate(&character->animatable, (void*)this, Char_BFLights_SetFrame);
	if (stage.song_step <= 1599)
	{
		if (stage.lights != 1)
			Character_Draw(character, &this->tex, &char_bflights_frame[this->frame]);
		else
			Character_Draw(character, &this->tex, &char_bflights_frame2[this->frame]);
	}
}

void Char_BFLights_SetAnim(Character *character, u8 anim)
{
	Char_BFLights *this = (Char_BFLights*)character;
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_BFLights_Free(Character *character)
{
	Char_BFLights *this = (Char_BFLights*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_BFLights_New(fixed_t x, fixed_t y)
{
	//Allocate boyfriend lights object
	Char_BFLights *this = Mem_Alloc(sizeof(Char_BFLights));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BFLights_New] Failed to allocate boyfriend lights object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BFLights_Tick;
	this->character.set_anim = Char_BFLights_SetAnim;
	this->character.free = Char_BFLights_Free;
	
	Animatable_Init(&this->character.animatable, char_bflights_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = CHAR_SPEC_MISSANIM;
	
	this->character.health_i = 20;
	
	//health bar color
	this->character.health_bar = 0xFF29B5D6;
	
	this->character.focus_x = FIXED_DEC(-64,1);
	this->character.focus_y = FIXED_DEC(-77,1);
	this->character.focus_zoom = FIXED_DEC(1228,1024);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(1,1);

	//Load art
	this->arc_main = IO_Read("\\PLAYER\\BFLIGHTS.ARC;1");
		
	const char **pathp = (const char *[]){
		"bf0.tim",   //BFLights_ArcMain_BFLights0
		"bf1.tim",   //BFLights_ArcMain_BFLights1
		"bf2.tim",   //BFLights_ArcMain_BFLights2
		"bf3.tim",   //BFLights_ArcMain_BFLights3
		"bf4.tim",   //BFLights_ArcMain_BFLights4
		"bf5.tim",   //BFLights_ArcMain_BFLights5
		"bf6.tim",   //BFLights_ArcMain_BFLights6
  "idle0.tim",
  "idle1.tim",
  "left0.tim",
  "down0.tim",
  "up0.tim",
  "right0.tim",
  "leftm0.tim",
  "downm0.tim",
  "upm0.tim",
  "rightm0.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
