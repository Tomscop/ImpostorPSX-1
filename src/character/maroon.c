/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "maroon.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Maroon character structure
enum
{
  Maroon_ArcMain_Idle0,
  Maroon_ArcMain_Idle1,
  Maroon_ArcMain_Idle2,
  Maroon_ArcMain_Idle3,
  Maroon_ArcMain_Idle4,
  Maroon_ArcMain_Idle5,
  Maroon_ArcMain_Idle6,
  Maroon_ArcMain_Idle7,
  Maroon_ArcMain_Left0,
  Maroon_ArcMain_Left1,
  Maroon_ArcMain_Left2,
  Maroon_ArcMain_Left3,
  Maroon_ArcMain_Down0,
  Maroon_ArcMain_Down1,
  Maroon_ArcMain_Down2,
  Maroon_ArcMain_Down3,
  Maroon_ArcMain_Down4,
  Maroon_ArcMain_Up0,
  Maroon_ArcMain_Up1,
  Maroon_ArcMain_Up2,
  Maroon_ArcMain_Up3,
  Maroon_ArcMain_Right0,
  Maroon_ArcMain_Right1,
  Maroon_ArcMain_Right2,
  Maroon_ArcMain_Right3,
  Maroon_ArcMain_Emote0,
  Maroon_ArcMain_Emote1,
  Maroon_ArcMain_Emote2,
  Maroon_ArcMain_Emote3,
  Maroon_ArcMain_Emote4,
  Maroon_ArcMain_Emote5,
  Maroon_ArcMain_Emote6,
  Maroon_ArcMain_Emote7,
  Maroon_ArcMain_Emote8,
  Maroon_ArcMain_Emote9,
  Maroon_ArcMain_Emote10,
  Maroon_ArcMain_Emote11,
  Maroon_ArcMain_Emote12,
  Maroon_ArcMain_Emote13,
  Maroon_ArcMain_Emote14,
	
	Maroon_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Maroon_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Maroon;

//Maroon character definitions
static const CharFrame char_maroon_frame[] = {
  {Maroon_ArcMain_Idle0, {  0,  0,140,106}, {199, 97}}, //0 idle 1
  {Maroon_ArcMain_Idle0, {  0,106,143,119}, {202,110}}, //1 idle 2
  {Maroon_ArcMain_Idle1, {  0,  0,154,129}, {213,118}}, //2 idle 3
  {Maroon_ArcMain_Idle2, {  0,  0,150,125}, {209,114}}, //3 idle 4
  {Maroon_ArcMain_Idle2, {  0,125,149,110}, {209, 97}}, //4 idle 5
  {Maroon_ArcMain_Idle3, {  0,  0,156,110}, {214, 97}}, //5 idle 6
  {Maroon_ArcMain_Idle3, {  0,110,147,108}, {204, 98}}, //6 idle 7
  {Maroon_ArcMain_Idle4, {  0,  0,143,108}, {202, 98}}, //7 idle 8
  {Maroon_ArcMain_Idle4, {  0,108,147,116}, {205,110}}, //8 idle 9
  {Maroon_ArcMain_Idle5, {  0,  0,157,123}, {214,118}}, //9 idle 10
  {Maroon_ArcMain_Idle5, {  0,123,154,120}, {212,116}}, //10 idle 11
  {Maroon_ArcMain_Idle6, {  0,  0,154,103}, {211, 99}}, //11 idle 12
  {Maroon_ArcMain_Idle6, {  0,103,159,103}, {216, 99}}, //12 idle 13
  {Maroon_ArcMain_Idle7, {  0,  0,146,103}, {204, 99}}, //13 idle 14

  {Maroon_ArcMain_Left0, {  0,  0,138,124}, {210,103}}, //14 left 1
  {Maroon_ArcMain_Left0, {  0,124,143,124}, {215,103}}, //15 left 2
  {Maroon_ArcMain_Left1, {  0,  0,132,125}, {203,102}}, //16 left 3
  {Maroon_ArcMain_Left1, {  0,125,131,125}, {200,102}}, //17 left 4
  {Maroon_ArcMain_Left2, {  0,  0,133,124}, {204,102}}, //18 left 5
  {Maroon_ArcMain_Left2, {  0,124,143,129}, {213,107}}, //19 left 6
  {Maroon_ArcMain_Left3, {  0,  0,141,125}, {211,103}}, //20 left 7

  {Maroon_ArcMain_Down0, {  0,  0,142, 99}, {201,111}}, //21 down 1
  {Maroon_ArcMain_Down0, {  0, 99,127, 99}, {186,111}}, //22 down 2
  {Maroon_ArcMain_Down1, {  0,  0,137, 96}, {197,108}}, //23 down 3
  {Maroon_ArcMain_Down1, {  0, 96,135,101}, {195,113}}, //24 down 4
  {Maroon_ArcMain_Down2, {  0,  0,139, 96}, {200,108}}, //25 down 5
  {Maroon_ArcMain_Down2, {  0, 96,144, 96}, {204,108}}, //26 down 6
  {Maroon_ArcMain_Down3, {  0,  0,149, 96}, {210,108}}, //27 down 7
  {Maroon_ArcMain_Down3, {  0, 96,149, 96}, {210,108}}, //28 down 8
  {Maroon_ArcMain_Down4, {  0,  0,134, 96}, {195,108}}, //29 down 9

  {Maroon_ArcMain_Up0, {  0,  0,201, 99}, {237,112}}, //30 up 1
  {Maroon_ArcMain_Up0, {  0, 99,202, 99}, {237,112}}, //31 up 2
  {Maroon_ArcMain_Up1, {  0,  0,202,106}, {239,119}}, //32 up 3
  {Maroon_ArcMain_Up1, {  0,106,203,101}, {239,114}}, //33 up 4
  {Maroon_ArcMain_Up2, {  0,  0,202, 91}, {238,104}}, //34 up 5
  {Maroon_ArcMain_Up2, {  0, 91,202, 91}, {238,104}}, //35 up 6
  {Maroon_ArcMain_Up3, {  0,  0,202, 91}, {238,104}}, //36 up 7

  {Maroon_ArcMain_Right0, {  0,  0,149,100}, {179,111}}, //37 right 1
  {Maroon_ArcMain_Right0, {  0,100,149,100}, {179,111}}, //38 right 2
  {Maroon_ArcMain_Right1, {  0,  0,152,101}, {186,112}}, //39 right 3
  {Maroon_ArcMain_Right1, {  0,101,152,101}, {186,112}}, //40 right 4
  {Maroon_ArcMain_Right2, {  0,  0,153,101}, {187,112}}, //41 right 5
  {Maroon_ArcMain_Right2, {  0,101,157,101}, {191,112}}, //42 right 6
  {Maroon_ArcMain_Right3, {  0,  0,151,101}, {184,112}}, //43 right 7

  {Maroon_ArcMain_Emote0, {  0,  0,156,116}, {208,107}}, //44 emote 1
  {Maroon_ArcMain_Emote0, {  0,116,156,116}, {208,107}}, //45 emote 2
  {Maroon_ArcMain_Emote1, {  0,  0,151,111}, {206,105}}, //46 emote 3
  {Maroon_ArcMain_Emote1, {  0,111,157,117}, {212,111}}, //47 emote 4
  {Maroon_ArcMain_Emote2, {  0,  0,155,114}, {210,107}}, //48 emote 5
  {Maroon_ArcMain_Emote2, {  0,114,154,112}, {209,105}}, //49 emote 6
  {Maroon_ArcMain_Emote3, {  0,  0,159,112}, {214,105}}, //50 emote 7
  {Maroon_ArcMain_Emote3, {  0,112,153,112}, {206,105}}, //51 emote 8
  {Maroon_ArcMain_Emote4, {  0,  0,151,112}, {206,105}}, //52 emote 9
  {Maroon_ArcMain_Emote4, {  0,112,151,112}, {206,105}}, //53 emote 10
  {Maroon_ArcMain_Emote5, {  0,  0,158,118}, {213,111}}, //54 emote 11
  {Maroon_ArcMain_Emote5, {  0,118,155,114}, {210,107}}, //55 emote 12
  {Maroon_ArcMain_Emote6, {  0,  0,154,112}, {209,105}}, //56 emote 13
  {Maroon_ArcMain_Emote6, {  0,112,159,112}, {214,105}}, //57 emote 14
  {Maroon_ArcMain_Emote7, {  0,  0,152,111}, {206,104}}, //58 emote 15
  {Maroon_ArcMain_Emote7, {  0,111,152,111}, {206,104}}, //59 emote 16
  {Maroon_ArcMain_Emote8, {  0,  0,158,118}, {212,111}}, //60 emote 17
  {Maroon_ArcMain_Emote8, {  0,118,156,114}, {210,107}}, //61 emote 18
  {Maroon_ArcMain_Emote9, {  0,  0,155,111}, {209,104}}, //62 emote 19
  {Maroon_ArcMain_Emote9, {  0,111,160,111}, {214,104}}, //63 emote 20
  {Maroon_ArcMain_Emote10, {  0,  0,152,111}, {206,104}}, //64 emote 21
  {Maroon_ArcMain_Emote10, {  0,111,153,111}, {206,104}}, //65 emote 22
  {Maroon_ArcMain_Emote11, {  0,  0,151,111}, {206,104}}, //66 emote 23
  {Maroon_ArcMain_Emote11, {  0,111,160,118}, {213,111}}, //67 emote 24
  {Maroon_ArcMain_Emote12, {  0,  0,155,114}, {210,107}}, //68 emote 25
  {Maroon_ArcMain_Emote12, {  0,114,155,112}, {209,105}}, //69 emote 26
  {Maroon_ArcMain_Emote13, {  0,  0,160,112}, {214,105}}, //70 emote 27
  {Maroon_ArcMain_Emote13, {  0,112,152,112}, {206,105}}, //71 emote 28
  {Maroon_ArcMain_Emote14, {  0,  0,152,112}, {206,105}}, //72 emote 29
};

static const Animation char_maroon_anim[CharAnim_Max] = {
	{1, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, ASCR_CHGANI, CharAnim_Special2}}, //CharAnim_Idle
	{1, (const u8[]){ 14, 15, 16, 17, 18, 19, 20, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{1, (const u8[]){ 21, 22, 23, 24, 25, 26, 27, 28, 29, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{1, (const u8[]){ 30, 31, 32, 33, 34, 35, 36, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{1, (const u8[]){ 37, 38, 39, 40, 41, 42, 43, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
	
	{1, (const u8[]){ 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 52, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, ASCR_CHGANI, CharAnim_Idle}},         //CharAnim_Special1
	{1, (const u8[]){ 10, 11, 12, 13, 10, 11, 12, 13, 10, 11, 12, 13, ASCR_CHGANI, CharAnim_Special2}},         //CharAnim_Special2
};

//Maroon character functions
void Char_Maroon_SetFrame(void *user, u8 frame)
{
	Char_Maroon *this = (Char_Maroon*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_maroon_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Maroon_Tick(Character *character)
{
	Char_Maroon *this = (Char_Maroon*)character;
	
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
		if ((Animatable_Ended(&character->animatable) &&
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
			(stage.song_step & 0x7) == 0) || 
			(character->animatable.anim == CharAnim_Special2 && 
			(stage.song_step & 0x7) == 0))
			character->set_anim(character, CharAnim_Idle);
	}
	}
	
	//Stage specific animations
		switch (stage.stage_id)
		{
			case StageId_Magmatic: //Fortnite Emote
				if (stage.song_step == 1184)
					character->set_anim(character, CharAnim_Special1);
				break;
			default:
				break;
		}
		
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Maroon_SetFrame);
	Character_Draw(character, &this->tex, &char_maroon_frame[this->frame]);
}

void Char_Maroon_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Maroon_Free(Character *character)
{
	Char_Maroon *this = (Char_Maroon*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Maroon_New(fixed_t x, fixed_t y)
{
	//Allocate maroon object
	Char_Maroon *this = Mem_Alloc(sizeof(Char_Maroon));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Maroon_New] Failed to allocate maroon object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Maroon_Tick;
	this->character.set_anim = Char_Maroon_SetAnim;
	this->character.free = Char_Maroon_Free;
	
	Animatable_Init(&this->character.animatable, char_maroon_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 0;

	//health bar color
	this->character.health_bar = 0xFF52232F;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-115,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\OPPONENT\\MAROON.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "idle2.tim",
  "idle3.tim",
  "idle4.tim",
  "idle5.tim",
  "idle6.tim",
  "idle7.tim",
  "left0.tim",
  "left1.tim",
  "left2.tim",
  "left3.tim",
  "down0.tim",
  "down1.tim",
  "down2.tim",
  "down3.tim",
  "down4.tim",
  "up0.tim",
  "up1.tim",
  "up2.tim",
  "up3.tim",
  "right0.tim",
  "right1.tim",
  "right2.tim",
  "right3.tim",
  "emote0.tim",
  "emote1.tim",
  "emote2.tim",
  "emote3.tim",
  "emote4.tim",
  "emote5.tim",
  "emote6.tim",
  "emote7.tim",
  "emote8.tim",
  "emote9.tim",
  "emote10.tim",
  "emote11.tim",
  "emote12.tim",
  "emote13.tim",
  "emote14.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
