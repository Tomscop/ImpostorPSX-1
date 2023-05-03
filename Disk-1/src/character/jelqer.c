/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "jelqer.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Jelqer character structure
enum
{
  Jelqer_ArcMain_IdleL0,
  Jelqer_ArcMain_IdleL1,
  Jelqer_ArcMain_IdleL2,
  Jelqer_ArcMain_IdleL3,
  Jelqer_ArcMain_IdleR0,
  Jelqer_ArcMain_IdleR1,
  Jelqer_ArcMain_IdleR2,
  Jelqer_ArcMain_Left0,
  Jelqer_ArcMain_Left1,
  Jelqer_ArcMain_Down0,
  Jelqer_ArcMain_Down1,
  Jelqer_ArcMain_Up0,
  Jelqer_ArcMain_Up1,
  Jelqer_ArcMain_Right0,
  Jelqer_ArcMain_Right1,
	
	Jelqer_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Jelqer_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Jelqer;

//Jelqer character definitions
static const CharFrame char_jelqer_frame[] = {
  {Jelqer_ArcMain_IdleL0, {  0,  0,138,112}, {160,117}}, //0 idlel 1
  {Jelqer_ArcMain_IdleL0, {  0,112,138,113}, {159,117}}, //1 idlel 2
  {Jelqer_ArcMain_IdleL1, {  0,  0,139,113}, {159,117}}, //2 idlel 3
  {Jelqer_ArcMain_IdleL1, {  0,113,139,113}, {158,117}}, //3 idlel 4
  {Jelqer_ArcMain_IdleL2, {  0,  0,138,113}, {157,118}}, //4 idlel 5
  {Jelqer_ArcMain_IdleL2, {  0,113,136,114}, {154,118}}, //5 idlel 6
  {Jelqer_ArcMain_IdleL3, {  0,  0,136,114}, {154,118}}, //6 idlel 7

  {Jelqer_ArcMain_IdleR0, {  0,  0,128,112}, {140+1,116}}, //7 idler 1
  {Jelqer_ArcMain_IdleR0, {128,  0,126,112}, {140+1,116}}, //8 idler 2
  {Jelqer_ArcMain_IdleR0, {  0,112,126,112}, {141+1,116}}, //9 idler 3
  {Jelqer_ArcMain_IdleR0, {126,112,129,113}, {145+1,117}}, //10 idler 4
  {Jelqer_ArcMain_IdleR1, {  0,  0,136,113}, {152+1,117}}, //11 idler 5
  {Jelqer_ArcMain_IdleR1, {  0,113,136,113}, {153+1,117}}, //12 idler 6
  {Jelqer_ArcMain_IdleR2, {  0,  0,136,114}, {153+1,118}}, //13 idler 7

  {Jelqer_ArcMain_Left0, {  0,  0,162,115}, {168,119}}, //14 left 1
  {Jelqer_ArcMain_Left0, {  0,115,158,115}, {164,119}}, //15 left 2
  {Jelqer_ArcMain_Left1, {  0,  0,158,115}, {164,119}}, //16 left 3

  {Jelqer_ArcMain_Down0, {  0,  0,132, 96}, {136+2, 98+1}}, //17 down 1
  {Jelqer_ArcMain_Down0, {  0, 96,133, 98}, {137+2,100+1}}, //18 down 2
  {Jelqer_ArcMain_Down1, {  0,  0,132, 99}, {137+2,102+1}}, //19 down 3

  {Jelqer_ArcMain_Up0, {  0,  0,132,123}, {127+2,129-2}}, //20 up 1
  {Jelqer_ArcMain_Up0, {  0,123,132,122}, {127+2,128-2}}, //21 up 2
  {Jelqer_ArcMain_Up1, {  0,  0,132,121}, {127+2,127-2}}, //22 up 3

  {Jelqer_ArcMain_Right0, {  0,  0,143,102}, {134+1,107}}, //23 right 1
  {Jelqer_ArcMain_Right0, {  0,102,151,105}, {141+1,109}}, //24 right 2
  {Jelqer_ArcMain_Right1, {  0,  0,152,105}, {142+1,109}}, //25 right 3
};

static const Animation char_jelqer_anim[CharAnim_Max] = {
	{2, (const u8[]){ASCR_CHGANI, CharAnim_Idle}}, //CharAnim_Idle
	{2, (const u8[]){ 14, 15, 16, ASCR_BACK, 1}},         //CharAnim_Left
	{2, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, ASCR_BACK, 1}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 17, 18, 19, 19, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 20, 21, 22, 22, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 23, 24, 25, ASCR_BACK, 1}},         //CharAnim_Right
	{2, (const u8[]){ 7, 8, 9, 10, 11, 12, 13, ASCR_BACK, 1}},   //CharAnim_RightAlt
};

//Jelqer character functions
void Char_Jelqer_SetFrame(void *user, u8 frame)
{
	Char_Jelqer *this = (Char_Jelqer*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_jelqer_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Jelqer_Tick(Character *character)
{
	Char_Jelqer *this = (Char_Jelqer*)character;
	
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
	Animatable_Animate(&character->animatable, (void*)this, Char_Jelqer_SetFrame);
	if (stage.stage_id == StageId_VotingTime)
		Character_Draw(character, &this->tex, &char_jelqer_frame[this->frame]);
	else if ((stage.camswitch == 2) || (stage.camswitch == 5))
		Character_DrawCol(character, &this->tex, &char_jelqer_frame[this->frame], 175, 175, 175);
}

void Char_Jelqer_SetAnim(Character *character, u8 anim)
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

void Char_Jelqer_Free(Character *character)
{
	Char_Jelqer *this = (Char_Jelqer*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Jelqer_New(fixed_t x, fixed_t y)
{
	//Allocate jelqer object
	Char_Jelqer *this = Mem_Alloc(sizeof(Char_Jelqer));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Jelqer_New] Failed to allocate jelqer object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Jelqer_Tick;
	this->character.set_anim = Char_Jelqer_SetAnim;
	this->character.free = Char_Jelqer_Free;
	
	Animatable_Init(&this->character.animatable, char_jelqer_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 3;

	//health bar color
	this->character.health_bar = 0xFFFFC8E2;
	
	this->character.focus_x = FIXED_DEC(-41,1);
	this->character.focus_y = FIXED_DEC(-124,1);
	this->character.focus_zoom = FIXED_DEC(1086,1024);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\OPPONENT\\JELQER.ARC;1");
	
	const char **pathp = (const char *[]){
  "idlel0.tim",
  "idlel1.tim",
  "idlel2.tim",
  "idlel3.tim",
  "idler0.tim",
  "idler1.tim",
  "idler2.tim",
  "left0.tim",
  "left1.tim",
  "down0.tim",
  "down1.tim",
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
