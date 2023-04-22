/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "gray.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Gray character structure
enum
{
  Gray_ArcMain_Idle0,
  Gray_ArcMain_Idle1,
  Gray_ArcMain_Idle2,
  Gray_ArcMain_Idle3,
  Gray_ArcMain_Idle4,
  Gray_ArcMain_Idle5,
  Gray_ArcMain_Idle6,
  Gray_ArcMain_Idle7,
  Gray_ArcMain_Idle8,
  Gray_ArcMain_Idle9,
  Gray_ArcMain_Idle10,
  Gray_ArcMain_Idle11,
  Gray_ArcMain_Idle12,
  Gray_ArcMain_Idle13,
  Gray_ArcMain_Left0,
  Gray_ArcMain_Left1,
  Gray_ArcMain_Left2,
  Gray_ArcMain_Left3,
  Gray_ArcMain_Down0,
  Gray_ArcMain_Down1,
  Gray_ArcMain_Down2,
  Gray_ArcMain_Down3,
  Gray_ArcMain_Up0,
  Gray_ArcMain_Up1,
  Gray_ArcMain_Up2,
  Gray_ArcMain_Up3,
  Gray_ArcMain_Right0,
  Gray_ArcMain_Right1,
  Gray_ArcMain_Right2,
  Gray_ArcMain_Right3,
	
	Gray_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Gray_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Gray;

//Gray character definitions
static const CharFrame char_gray_frame[] = {
  {Gray_ArcMain_Idle0, {  0,  0,169,117}, {146,148}}, //0 idle 1
  {Gray_ArcMain_Idle0, {  0,118,168,117}, {146,148}}, //1 idle 2
  {Gray_ArcMain_Idle1, {  0,  0,169,116}, {150,145}}, //2 idle 3
  {Gray_ArcMain_Idle1, {  0,117,170,116}, {150,145}}, //3 idle 4
  {Gray_ArcMain_Idle2, {  0,  0,169,115}, {150,143}}, //4 idle 5
  {Gray_ArcMain_Idle2, {  0,116,169,115}, {150,143}}, //5 idle 6
  {Gray_ArcMain_Idle3, {  0,  0,170,114}, {149,142}}, //6 idle 7
  {Gray_ArcMain_Idle3, {  0,115,169,114}, {149,142}}, //7 idle 8
  {Gray_ArcMain_Idle4, {  0,  0,171,112}, {149,142}}, //8 idle 9
  {Gray_ArcMain_Idle4, {  0,113,171,112}, {149,142}}, //9 idle 10
  {Gray_ArcMain_Idle5, {  0,  0,172,114}, {145,145}}, //10 idle 11
  {Gray_ArcMain_Idle5, {  0,115,172,114}, {145,145}}, //11 idle 12
  {Gray_ArcMain_Idle6, {  0,  0,172,115}, {143,147}}, //12 idle 13
  {Gray_ArcMain_Idle6, {  0,116,172,115}, {143,147}}, //13 idle 14
  {Gray_ArcMain_Idle7, {  0,  0,168,117}, {138,149}}, //14 idle 15
  {Gray_ArcMain_Idle7, {  0,118,168,117}, {138,149}}, //15 idle 16
  {Gray_ArcMain_Idle8, {  0,  0,166,118}, {135,148}}, //16 idle 17
  {Gray_ArcMain_Idle8, {  0,119,166,118}, {135,148}}, //17 idle 18
  {Gray_ArcMain_Idle9, {  0,  0,166,116}, {134,147}}, //18 idle 19
  {Gray_ArcMain_Idle9, {  0,117,165,116}, {134,147}}, //19 idle 20
  {Gray_ArcMain_Idle10, {  0,  0,166,117}, {134,146}}, //20 idle 21
  {Gray_ArcMain_Idle10, {  0,118,166,117}, {134,146}}, //21 idle 22
  {Gray_ArcMain_Idle11, {  0,  0,166,117}, {137,148}}, //22 idle 23
  {Gray_ArcMain_Idle11, {  0,118,167,117}, {137,148}}, //23 idle 24
  {Gray_ArcMain_Idle12, {  0,  0,167,117}, {140,150}}, //24 idle 25
  {Gray_ArcMain_Idle12, {  0,118,167,117}, {140,150}}, //25 idle 26
  {Gray_ArcMain_Idle13, {  0,  0,166,117}, {142,150}}, //26 idle 27
  {Gray_ArcMain_Idle13, {  0,118,166,117}, {142,150}}, //27 idle 28

  {Gray_ArcMain_Left0, {  0,  0,147,117}, {152,149}}, //28 left 1
  {Gray_ArcMain_Left0, {  0,118,146,117}, {152,149}}, //29 left 2
  {Gray_ArcMain_Left1, {  0,  0,158,118}, {151,149}}, //30 left 3
  {Gray_ArcMain_Left1, {  0,119,158,118}, {151,149}}, //31 left 4
  {Gray_ArcMain_Left2, {  0,  0,157,119}, {151,149}}, //32 left 5
  {Gray_ArcMain_Left2, {  0,120,157,119}, {151,149}}, //33 left 6
  {Gray_ArcMain_Left3, {  0,  0,157,119}, {151,149}}, //34 left 7

  {Gray_ArcMain_Down0, {  0,  0,133,124}, {126,127}}, //35 down 1
  {Gray_ArcMain_Down0, {  0,125,132,124}, {126,127}}, //36 down 2
  {Gray_ArcMain_Down1, {  0,  0,136,123}, {129,129}}, //37 down 3
  {Gray_ArcMain_Down1, {  0,124,136,122}, {129,129}}, //38 down 4
  {Gray_ArcMain_Down2, {  0,  0,136,123}, {129,129}}, //39 down 5
  {Gray_ArcMain_Down2, {  0,124,136,123}, {129,129}}, //40 down 6
  {Gray_ArcMain_Down3, {  0,  0,137,122}, {129,129}}, //41 down 7

  {Gray_ArcMain_Up0, {  0,  0,161,105}, {140,142}}, //42 up 1
  {Gray_ArcMain_Up0, {  0,106,161,105}, {140,142}}, //43 up 2
  {Gray_ArcMain_Up1, {  0,  0,154,108}, {139,145}}, //44 up 3
  {Gray_ArcMain_Up1, {  0,109,154,108}, {139,145}}, //45 up 4
  {Gray_ArcMain_Up2, {  0,  0,154,108}, {139,145}}, //46 up 5
  {Gray_ArcMain_Up2, {  0,109,154,108}, {139,145}}, //47 up 6
  {Gray_ArcMain_Up3, {  0,  0,154,107}, {139,144}}, //48 up 7

  {Gray_ArcMain_Right0, {  0,  0,194,118}, {116,154}}, //49 right 1
  {Gray_ArcMain_Right0, {  0,119,194,118}, {116,154}}, //50 right 2
  {Gray_ArcMain_Right1, {  0,  0,192,121}, {118,155}}, //51 right 3
  {Gray_ArcMain_Right1, {  0,122,192,121}, {118,155}}, //52 right 4
  {Gray_ArcMain_Right2, {  0,  0,192,122}, {118,155}}, //53 right 5
  {Gray_ArcMain_Right2, {  0,123,192,122}, {118,155}}, //54 right 6
  {Gray_ArcMain_Right3, {  0,  0,192,122}, {118,155}}, //55 right 7
};

static const Animation char_gray_anim[CharAnim_Max] = {
	{1, (const u8[]){ASCR_CHGANI, CharAnim_Idle}}, //CharAnim_Idle
	{1, (const u8[]){ 28, 29, 30, 31, 32, 33, 34, 32, 33, 34, 32, 33, 34, 32, ASCR_BACK, 1}},         //CharAnim_Left
	{1, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, ASCR_BACK, 1}},   //CharAnim_LeftAlt
	{1, (const u8[]){ 35, 36, 37, 38, 39, 40, 41, 39, 40, 41, 39, 40, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{1, (const u8[]){ 42, 43, 44, 45, 46, 47, 48, 46, 47, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{1, (const u8[]){ 49, 50, 51, 52, 53, 54, 55, 53, 54, 55, ASCR_BACK, 1}},         //CharAnim_Right
	{1, (const u8[]){ 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 27, 27, ASCR_BACK, 1}},   //CharAnim_RightAlt
};

//Gray character functions
void Char_Gray_SetFrame(void *user, u8 frame)
{
	Char_Gray *this = (Char_Gray*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_gray_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Gray_Tick(Character *character)
{
	Char_Gray *this = (Char_Gray*)character;
	
	//Camera stuff
	if (stage.stage_id == StageId_Pretender)
	{
		if (stage.song_step == 4)
			this->character.focus_zoom = FIXED_DEC(679,1024);
	}
	
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
	Animatable_Animate(&character->animatable, (void*)this, Char_Gray_SetFrame);
	Character_Draw(character, &this->tex, &char_gray_frame[this->frame]);
}

void Char_Gray_SetAnim(Character *character, u8 anim)
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

void Char_Gray_Free(Character *character)
{
	Char_Gray *this = (Char_Gray*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Gray_New(fixed_t x, fixed_t y)
{
	//Allocate gray object
	Char_Gray *this = Mem_Alloc(sizeof(Char_Gray));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Gray_New] Failed to allocate gray object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Gray_Tick;
	this->character.set_anim = Char_Gray_SetAnim;
	this->character.free = Char_Gray_Free;
	
	Animatable_Init(&this->character.animatable, char_gray_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 2;

	//health bar color
	this->character.health_bar = 0xFF625773;
	
	if ((stage.stage_id >= StageId_Delusion) && (stage.stage_id <= StageId_Neurotic))
	{
	this->character.focus_x = FIXED_DEC(-41,1);
	this->character.focus_y = FIXED_DEC(-124,1);
	this->character.focus_zoom = FIXED_DEC(1086,1024);
	}
	else if (stage.stage_id == StageId_Pretender)
	{
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-115,1);
	this->character.focus_zoom = FIXED_DEC(950,1024);
	}
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\OPPONENT\\GRAY.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "idle2.tim",
  "idle3.tim",
  "idle4.tim",
  "idle5.tim",
  "idle6.tim",
  "idle7.tim",
  "idle8.tim",
  "idle9.tim",
  "idle10.tim",
  "idle11.tim",
  "idle12.tim",
  "idle13.tim",
  "left0.tim",
  "left1.tim",
  "left2.tim",
  "left3.tim",
  "down0.tim",
  "down1.tim",
  "down2.tim",
  "down3.tim",
  "up0.tim",
  "up1.tim",
  "up2.tim",
  "up3.tim",
  "right0.tim",
  "right1.tim",
  "right2.tim",
  "right3.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
