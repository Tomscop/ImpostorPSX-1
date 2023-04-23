/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "pinkpretender.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Boyfriend Pretender player types
enum
{
  PinkPretender_ArcMain_Idle0,
  PinkPretender_ArcMain_Idle1,
  PinkPretender_ArcMain_Idle2,
  PinkPretender_ArcMain_Idle3,
  PinkPretender_ArcMain_Idle4,
  PinkPretender_ArcMain_Idle5,
  PinkPretender_ArcMain_Idle6,
  PinkPretender_ArcMain_Left0,
  PinkPretender_ArcMain_Left1,
  PinkPretender_ArcMain_Left2,
  PinkPretender_ArcMain_Left3,
  PinkPretender_ArcMain_Left4,
  PinkPretender_ArcMain_Left5,
  PinkPretender_ArcMain_Down0,
  PinkPretender_ArcMain_Down1,
  PinkPretender_ArcMain_Down2,
  PinkPretender_ArcMain_Up0,
  PinkPretender_ArcMain_Up1,
  PinkPretender_ArcMain_Right0,
  PinkPretender_ArcMain_Right1,
  PinkPretender_ArcMain_Right2,
  PinkPretender_ArcMain_Right3,
  PinkPretender_ArcMain_Right4,
  PinkPretender_ArcMain_LeftM0,
  PinkPretender_ArcMain_DownM0,
  PinkPretender_ArcMain_UpM0,
  PinkPretender_ArcMain_RightM0,
	
	PinkPretender_ArcMain_Max,
};

#define PinkPretender_Arc_Max PinkPretender_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[PinkPretender_Arc_Max];
	
	Gfx_Tex tex, tex_retry;
	u8 frame, tex_id;
} Char_PinkPretender;

//Boyfriend Pretender player definitions
static const CharFrame char_pinkpretender_frame[] = {
  {PinkPretender_ArcMain_Idle0, {  0,  0,145,125}, {207,125}}, //0 idle 1
  {PinkPretender_ArcMain_Idle1, {  0,  0,143,130}, {207,131}}, //1 idle 2
  {PinkPretender_ArcMain_Idle2, {  0,  0,142,129}, {208,131}}, //2 idle 3
  {PinkPretender_ArcMain_Idle3, {  0,  0,141,128}, {208,131}}, //3 idle 4
  {PinkPretender_ArcMain_Idle4, {  0,  0,142,127}, {209,130}}, //4 idle 5
  {PinkPretender_ArcMain_Idle5, {  0,  0,141,127}, {209,130}}, //5 idle 6
  {PinkPretender_ArcMain_Idle6, {  0,  0,141,126}, {209,129}}, //6 idle 7
  
  {PinkPretender_ArcMain_Left0, {  0,  0,127,128}, {199-4,132-1}}, //7 left 1
  {PinkPretender_ArcMain_Left1, {  0,  0,130,126}, {204-4,130-1}}, //8 left 2
  {PinkPretender_ArcMain_Left1, {  0,127,130,127}, {204-4,131-1}}, //9 left 3
  {PinkPretender_ArcMain_Left2, {  0,  0,131,127}, {205-4,131-1}}, //10 left 4
  {PinkPretender_ArcMain_Left3, {  0,  0,130,126}, {204-4,131-1}}, //11 left 5
  {PinkPretender_ArcMain_Left3, {  0,127,130,126}, {204-4,131-1}}, //12 left 6
  {PinkPretender_ArcMain_Left4, {  0,  0,130,125}, {204-4,130-1}}, //13 left 7
  {PinkPretender_ArcMain_Left4, {  0,126,130,125}, {204-4,130-1}}, //14 left 8
  {PinkPretender_ArcMain_Left5, {  0,  0,130,125}, {204-4,129-1}}, //15 left 9
  {PinkPretender_ArcMain_Left5, {  0,126,131,125}, {204-4,129-1}}, //16 left 10
  
  {PinkPretender_ArcMain_Down0, {  0,  0,111,117}, {191+4,119-2}}, //17 down 1
  {PinkPretender_ArcMain_Down0, {112,  0,111,117}, {191+4,119-2}}, //18 down 2
  {PinkPretender_ArcMain_Down0, {  0,118,112,114}, {189+4,119-2}}, //19 down 3
  {PinkPretender_ArcMain_Down0, {113,118,113,114}, {190+4,119-2}}, //20 down 4
  {PinkPretender_ArcMain_Down1, {  0,  0,113,114}, {190+4,119-2}}, //21 down 5
  {PinkPretender_ArcMain_Down1, {114,  0,112,115}, {189+4,120-2}}, //22 down 6
  {PinkPretender_ArcMain_Down1, {  0,116,113,115}, {189+4,120-2}}, //23 down 7
  {PinkPretender_ArcMain_Down1, {114,116,113,116}, {190+4,121-2}}, //24 down 8
  {PinkPretender_ArcMain_Down2, {  0,  0,113,116}, {190+4,121-2}}, //25 down 9
  
  {PinkPretender_ArcMain_Up0, {  0,  0,120,106}, {191+10,112-2}}, //26 up 1
  {PinkPretender_ArcMain_Up0, {121,  0,117,107}, {189+10,113-2}}, //27 up 2
  {PinkPretender_ArcMain_Up0, {  0,108,118,106}, {189+10,112-2}}, //28 up 3
  {PinkPretender_ArcMain_Up0, {119,108,118,107}, {190+10,113-2}}, //29 up 4
  {PinkPretender_ArcMain_Up1, {  0,  0,118,107}, {189+10,113-2}}, //30 up 5
  
  {PinkPretender_ArcMain_Right0, {  0,  0,148,126}, {183+60,130-1}}, //31 right 1
  {PinkPretender_ArcMain_Right0, {  0,127,148,123}, {183+60,127-1}}, //32 right 2
  {PinkPretender_ArcMain_Right1, {  0,  0,150,123}, {178+60,126-1}}, //33 right 3
  {PinkPretender_ArcMain_Right1, {  0,124,150,123}, {178+60,126-1}}, //34 right 4
  {PinkPretender_ArcMain_Right2, {  0,  0,149,123}, {178+60,126-1}}, //35 right 5
  {PinkPretender_ArcMain_Right2, {  0,124,149,124}, {177+60,128-1}}, //36 right 6
  {PinkPretender_ArcMain_Right3, {  0,  0,150,124}, {178+60,128-1}}, //37 right 7
  {PinkPretender_ArcMain_Right3, {  0,125,149,125}, {178+60,128-1}}, //38 right 8
  {PinkPretender_ArcMain_Right4, {  0,  0,149,125}, {178+60,128-1}}, //39 right 9
  {PinkPretender_ArcMain_Right4, {  0,126,149,124}, {177+60,128-1}}, //40 right 10
  
  {PinkPretender_ArcMain_LeftM0, {  0,  0,125,103}, {200-4,105-1}}, //41 leftm 1
  {PinkPretender_ArcMain_LeftM0, {126,  0,126,102}, {200-4,104-1}}, //42 leftm 2
  
  {PinkPretender_ArcMain_DownM0, {  0,  0,108,117}, {187+4,121-2}}, //43 downm 1
  {PinkPretender_ArcMain_DownM0, {109,  0,110,115}, {188+4,119-2}}, //44 downm 2
  
  {PinkPretender_ArcMain_UpM0, {  0,  0,116, 97}, {190+10,104-2}}, //45 upm 1
  {PinkPretender_ArcMain_UpM0, {117,  0,115, 98}, {189+10,104-2}}, //46 upm 2
  
  {PinkPretender_ArcMain_RightM0, {  0,  0,138,124}, {173+60,128-1}}, //47 rightm 1
  {PinkPretender_ArcMain_RightM0, {  0,125,138,123}, {172+60,127-1}}, //48 rightm 2
};

static const Animation char_pinkpretender_anim[PlayerAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, ASCR_BACK, 1}}, //CharAnim_Idle
	{1, (const u8[]){ 7, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 16, ASCR_BACK, 1}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{1, (const u8[]){ 17, 18, 19, 20, 21, 22, 23, 24, 25, 25, 25, ASCR_BACK, 1}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{1, (const u8[]){ 26, 26, 27, 27, 28, 28, 29, 29, 30, 30, 30, ASCR_BACK, 1}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{1, (const u8[]){ 31, 32, 33, 33, 34, 35, 36, 37, 38, 39, 40, 40, 40, ASCR_BACK, 1}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
    
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special1
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special2
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special3
	
	{2, (const u8[]){ 41, 42, 42, ASCR_BACK, 1}},     //PlayerAnim_LeftMiss
	{2, (const u8[]){ 43, 44, 44, ASCR_BACK, 1}},     //PlayerAnim_DownMiss
	{2, (const u8[]){ 45, 46, 46, ASCR_BACK, 1}},     //PlayerAnim_UpMiss
	{2, (const u8[]){ 47, 48, 48, ASCR_BACK, 1}},     //PlayerAnim_RightMiss
};

//Boyfriend Pretender player functions
void Char_PinkPretender_SetFrame(void *user, u8 frame)
{
	Char_PinkPretender *this = (Char_PinkPretender*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_pinkpretender_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_PinkPretender_Tick(Character *character)
{
	Char_PinkPretender *this = (Char_PinkPretender*)character;
	
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
	
	//Animate and draw character
	Animatable_Animate(&character->animatable, (void*)this, Char_PinkPretender_SetFrame);
	Character_DrawFlipped(character, &this->tex, &char_pinkpretender_frame[this->frame]);
}

void Char_PinkPretender_SetAnim(Character *character, u8 anim)
{
	Char_PinkPretender *this = (Char_PinkPretender*)character;
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_PinkPretender_Free(Character *character)
{
	Char_PinkPretender *this = (Char_PinkPretender*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_PinkPretender_New(fixed_t x, fixed_t y)
{
	//Allocate boyfriend reactor object
	Char_PinkPretender *this = Mem_Alloc(sizeof(Char_PinkPretender));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_PinkPretender_New] Failed to allocate boyfriend reactor object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_PinkPretender_Tick;
	this->character.set_anim = Char_PinkPretender_SetAnim;
	this->character.free = Char_PinkPretender_Free;
	
	Animatable_Init(&this->character.animatable, char_pinkpretender_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = CHAR_SPEC_MISSANIM;
	
	this->character.health_i = 6;
	
	//health bar color
	this->character.health_bar = 0xFFED62CA;
	
	
	this->character.focus_x = FIXED_DEC(33,1);
	this->character.focus_y = FIXED_DEC(-140,1);
	this->character.focus_zoom = FIXED_DEC(682,1024);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(1,1);

	//Load art
	this->arc_main = IO_Read("\\PLAYER\\PINKPRET.ARC;1");
		
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "idle2.tim",
  "idle3.tim",
  "idle4.tim",
  "idle5.tim",
  "idle6.tim",
  "left0.tim",
  "left1.tim",
  "left2.tim",
  "left3.tim",
  "left4.tim",
  "left5.tim",
  "down0.tim",
  "down1.tim",
  "down2.tim",
  "up0.tim",
  "up1.tim",
  "right0.tim",
  "right1.tim",
  "right2.tim",
  "right3.tim",
  "right4.tim",
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
