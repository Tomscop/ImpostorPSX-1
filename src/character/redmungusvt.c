/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "redmungusvt.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Redmungus VT character structure
enum
{
  RedmungusVT_ArcMain_Idle0,
  RedmungusVT_ArcMain_Left0,
  RedmungusVT_ArcMain_Down0,
  RedmungusVT_ArcMain_Up0,
  RedmungusVT_ArcMain_Right0,
  RedmungusVT_ArcMain_IdleL0,
  RedmungusVT_ArcMain_IdleR0,
  RedmungusVT_ArcMain_LeftM0,
  RedmungusVT_ArcMain_DownM0,
  RedmungusVT_ArcMain_UpM0,
  RedmungusVT_ArcMain_RightM0,
  RedmungusVT_ArcMain_Fuck0,
  RedmungusVT_ArcMain_Fuck1,
	
	RedmungusVT_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[RedmungusVT_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_RedmungusVT;

//Redmungus VT character definitions
static const CharFrame char_redmungusvt_frame[] = {
  {RedmungusVT_ArcMain_Idle0, {  0,  0, 58, 78}, {172,169}}, //0 idle 1
  {RedmungusVT_ArcMain_Idle0, { 58,  0, 57, 80}, {171,171}}, //1 idle 2
  {RedmungusVT_ArcMain_Idle0, {115,  0, 57, 81}, {172,172}}, //2 idle 3
  {RedmungusVT_ArcMain_Idle0, {172,  0, 55, 81}, {171,172}}, //3 idle 4

  {RedmungusVT_ArcMain_Left0, {  0,  0, 64, 81}, {187,172}}, //4 left 1
  {RedmungusVT_ArcMain_Left0, { 64,  0, 64, 81}, {187,172}}, //5 left 2
  {RedmungusVT_ArcMain_Left0, {128,  0, 63, 81}, {186,172}}, //6 left 3
  {RedmungusVT_ArcMain_Left0, {191,  0, 63, 81}, {185,172}}, //7 left 4

  {RedmungusVT_ArcMain_Down0, {  0,  0, 64, 70}, {181,161}}, //8 down 1
  {RedmungusVT_ArcMain_Down0, { 64,  0, 64, 70}, {181,161}}, //9 down 2
  {RedmungusVT_ArcMain_Down0, {128,  0, 62, 73}, {180,164}}, //10 down 3
  {RedmungusVT_ArcMain_Down0, {190,  0, 62, 74}, {181,165}}, //11 down 4

  {RedmungusVT_ArcMain_Up0, {  0,  0, 61, 85}, {172,176}}, //12 up 1
  {RedmungusVT_ArcMain_Up0, { 61,  0, 61, 85}, {172,176}}, //13 up 2
  {RedmungusVT_ArcMain_Up0, {122,  0, 63, 82}, {172,174}}, //14 up 3
  {RedmungusVT_ArcMain_Up0, {185,  0, 63, 82}, {172,174}}, //15 up 4

  {RedmungusVT_ArcMain_Right0, {  0,  0, 69, 78}, {171,169}}, //16 right 1
  {RedmungusVT_ArcMain_Right0, { 69,  0, 69, 79}, {171,170}}, //17 right 2
  {RedmungusVT_ArcMain_Right0, {138,  0, 64, 80}, {171,171}}, //18 right 3
  {RedmungusVT_ArcMain_Right0, {  0, 80, 64, 81}, {171,172}}, //19 right 4
  
  {RedmungusVT_ArcMain_IdleL0, {  0,  0, 59, 80}, {172,171+2}}, //20 idlel 1
  {RedmungusVT_ArcMain_IdleL0, { 59,  0, 59, 80}, {171,171+2}}, //21 idlel 2
  {RedmungusVT_ArcMain_IdleL0, {118,  0, 59, 81}, {170,172+2}}, //22 idlel 3
  {RedmungusVT_ArcMain_IdleL0, {177,  0, 59, 81}, {169,172+2}}, //23 idlel 4

  {RedmungusVT_ArcMain_IdleR0, {  0,  0, 59, 79}, {167,170+2}}, //24 idler 1
  {RedmungusVT_ArcMain_IdleR0, { 59,  0, 60, 79}, {168,170+2}}, //25 idler 2
  {RedmungusVT_ArcMain_IdleR0, {119,  0, 59, 80}, {169,171+2}}, //26 idler 3
  {RedmungusVT_ArcMain_IdleR0, {178,  0, 59, 81}, {169,172+2}}, //27 idler 4

  {RedmungusVT_ArcMain_LeftM0, {  0,  0, 90, 81}, {202,172+2}}, //28 leftm 1
  {RedmungusVT_ArcMain_LeftM0, { 90,  0, 90, 81}, {202,172+2}}, //29 leftm 2
  {RedmungusVT_ArcMain_LeftM0, {  0, 81, 91, 81}, {201,172+2}}, //30 leftm 3
  {RedmungusVT_ArcMain_LeftM0, { 91, 81, 90, 81}, {201,172+2}}, //31 leftm 4

  {RedmungusVT_ArcMain_DownM0, {  0,  0, 69, 73}, {180,164+2}}, //32 downm 1
  {RedmungusVT_ArcMain_DownM0, { 69,  0, 69, 73}, {181,164+2}}, //33 downm 2
  {RedmungusVT_ArcMain_DownM0, {138,  0, 70, 74}, {181,164+2}}, //34 downm 3
  {RedmungusVT_ArcMain_DownM0, {  0, 74, 70, 74}, {181,164+2}}, //35 downm 4

  {RedmungusVT_ArcMain_UpM0, {  0,  0,100, 87}, {191,176+2}}, //36 upm 1
  {RedmungusVT_ArcMain_UpM0, {100,  0,100, 86}, {192,175+2}}, //37 upm 2
  {RedmungusVT_ArcMain_UpM0, {  0, 87,101, 84}, {192,174+2}}, //38 upm 3
  {RedmungusVT_ArcMain_UpM0, {101, 87,101, 84}, {192,174+2}}, //39 upm 4

  {RedmungusVT_ArcMain_RightM0, {  0,  0, 85, 80}, {179,172+2}}, //40 rightm 1
  {RedmungusVT_ArcMain_RightM0, { 85,  0, 86, 80}, {180,172+2}}, //41 rightm 2
  {RedmungusVT_ArcMain_RightM0, {  0, 80, 87, 81}, {182,172+2}}, //42 rightm 3
  {RedmungusVT_ArcMain_RightM0, { 87, 80, 87, 81}, {182,172+2}}, //43 rightm 4

  {RedmungusVT_ArcMain_Fuck0, {  0,  0, 56, 81}, {167,172+2}}, //44 fuck 1
  {RedmungusVT_ArcMain_Fuck0, { 56,  0,101, 84}, {192,174+2}}, //45 fuck 2
  {RedmungusVT_ArcMain_Fuck0, {  0, 84,103, 83}, {193,173+2}}, //46 fuck 3
  {RedmungusVT_ArcMain_Fuck0, {103, 84,103, 83}, {193,173+2}}, //47 fuck 4
  {RedmungusVT_ArcMain_Fuck0, {  0,168,103, 84}, {194,174+2}}, //48 fuck 5
  {RedmungusVT_ArcMain_Fuck0, {103,168,102, 84}, {192,173+2}}, //49 fuck 6
  {RedmungusVT_ArcMain_Fuck1, {  0,  0,113, 86}, {198,174+2}}, //50 fuck 7
  {RedmungusVT_ArcMain_Fuck1, {113,  0,115, 85}, {199,173+2}}, //51 fuck 8
  {RedmungusVT_ArcMain_Fuck1, {  0, 86,115, 85}, {199,173+2}}, //52 fuck 9
  {RedmungusVT_ArcMain_Fuck1, {115, 86, 56, 81}, {167,171+2}}, //53 fuck 10
  {RedmungusVT_ArcMain_Fuck1, {171, 86, 56, 81}, {167,171+2}}, //54 fuck 11
};

static const Animation char_redmungusvt_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 4, 5, 6, 7, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 8, 9, 10, 11, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 12, 13, 14, 15, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 16, 17, 18, 19, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
	
	{2, (const u8[]){ 44, 45, 46, 47, 48, 47, 48, 47, 48, 47, 48, 47, 48, 47, 48, 47, 48, 49, 50, 51, 52, 52, 52, 52, 53, 53, 54, 54, 53, 53, 54, 54, 53, 53, 54, 54, 53, 53, 54, 54, ASCR_BACK, 1}},         //CharAnim_Special1
};

static const Animation char_redmungusvt_anim2[CharAnim_Max] = {
	{2, (const u8[]){ASCR_CHGANI, CharAnim_Idle}}, //CharAnim_Idle
	{2, (const u8[]){ 28, 29, 30, 31, ASCR_BACK, 1}},         //CharAnim_Left
	{2, (const u8[]){ 20, 21, 22, 23, 23, ASCR_BACK, 1}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 32, 33, 34, 35, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 36, 37, 38, 39, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 40, 41, 42, 43, ASCR_BACK, 1}},         //CharAnim_Right
	{2, (const u8[]){ 24, 25, 26, 27, 27, ASCR_BACK, 1}},   //CharAnim_RightAlt
	
	{2, (const u8[]){ 44, 45, 46, 47, 48, 47, 48, 47, 48, 47, 48, 47, 48, 47, 48, 47, 48, 49, 50, 51, 52, 52, 52, 52, 53, 53, 54, 54, 53, 53, 54, 54, 53, 53, 54, 54, 53, 53, 54, 54, ASCR_BACK, 1}},         //CharAnim_Special1
};

//RedmungusVT character functions
void Char_RedmungusVT_SetFrame(void *user, u8 frame)
{
	Char_RedmungusVT *this = (Char_RedmungusVT*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_redmungusvt_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_RedmungusVT_Tick(Character *character)
{
	Char_RedmungusVT *this = (Char_RedmungusVT*)character;
	
	//Switch
	if (stage.song_step == 1168)
		Animatable_Init(&this->character.animatable, char_redmungusvt_anim2);
	
	//Perform idle dance
	if (stage.song_step >= 1168)
	{
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
	}
	else
	{
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	}
	
	//Stage specific animations
		switch (stage.stage_id)
		{
			case StageId_VotingTime: //FUCK!!!
				if (stage.song_step == 1312)
					character->set_anim(character, CharAnim_Special1);
				break;
			default:
				break;
		}
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_RedmungusVT_SetFrame);
	Character_Draw(character, &this->tex, &char_redmungusvt_frame[this->frame]);
}

void Char_RedmungusVT_SetAnim(Character *character, u8 anim)
{
	//Set animation
	if ((anim == CharAnim_Idle) && (stage.song_step >= 1168))
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

void Char_RedmungusVT_Free(Character *character)
{
	Char_RedmungusVT *this = (Char_RedmungusVT*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_RedmungusVT_New(fixed_t x, fixed_t y)
{
	//Allocate redmungusvt object
	Char_RedmungusVT *this = Mem_Alloc(sizeof(Char_RedmungusVT));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_RedmungusVT_New] Failed to allocate redmungusvt object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_RedmungusVT_Tick;
	this->character.set_anim = Char_RedmungusVT_SetAnim;
	this->character.free = Char_RedmungusVT_Free;
	
	Animatable_Init(&this->character.animatable, char_redmungusvt_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;

	//health bar color
	this->character.health_bar = 0xFFE51919;
	
	this->character.focus_x = FIXED_DEC(-41,1);
	this->character.focus_y = FIXED_DEC(-124,1);
	this->character.focus_zoom = FIXED_DEC(1086,1024);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\OPPONENT\\REDMUNGV.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "left0.tim",
  "down0.tim",
  "up0.tim",
  "right0.tim",
  "idlel0.tim",
  "idler0.tim",
  "leftm0.tim",
  "downm0.tim",
  "upm0.tim",
  "rightm0.tim",
  "fuck0.tim",
  "fuck1.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
