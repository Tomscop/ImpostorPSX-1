/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "green.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Green character structure
enum
{
  Green_ArcMain_IdleL0,
  Green_ArcMain_IdleL1,
  Green_ArcMain_IdleL2,
  Green_ArcMain_IdleR0,
  Green_ArcMain_IdleR1,
  Green_ArcMain_IdleR2,
  Green_ArcMain_Left0,
  Green_ArcMain_Left1,
  Green_ArcMain_Down0,
  Green_ArcMain_Down1,
  Green_ArcMain_Down2,
  Green_ArcMain_Up0,
  Green_ArcMain_Up1,
  Green_ArcMain_Up2,
  Green_ArcMain_Up3,
  Green_ArcMain_Up4,
  Green_ArcMain_Up5,
  Green_ArcMain_Up6,
  Green_ArcMain_Up7,
  Green_ArcMain_Up8,
  Green_ArcMain_Right0,
  Green_ArcMain_Right1,
  Green_ArcMain_Reaction0,
  Green_ArcMain_Reaction1,
  Green_ArcMain_Reaction2,
  Green_ArcMain_Reaction3,
  Green_ArcMain_Reaction4,
  Green_ArcMain_Reaction5,
  Green_ArcMain_Reaction6,
  Green_ArcMain_Reaction7,
  Green_ArcMain_Reaction8,
  Green_ArcMain_Reaction9,
  Green_ArcMain_Reaction10,
  Green_ArcMain_Reaction11,
  Green_ArcMain_Reaction12,
  Green_ArcMain_Reaction13,
  Green_ArcMain_Reaction14,
  Green_ArcMain_Reaction15,
  Green_ArcMain_Reaction16,
	
	Green_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Green_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Green;

//Green character definitions
static const CharFrame char_green_frame[] = {
  {Green_ArcMain_IdleL0, {  0,  0,143,102}, {174, 90}}, //0 idlel 1
  {Green_ArcMain_IdleL0, {  0,102,143,101}, {173, 91}}, //1 idlel 2
  {Green_ArcMain_IdleL1, {  0,  0,143, 99}, {173, 91}}, //2 idlel 3
  {Green_ArcMain_IdleL1, {  0, 99,143, 98}, {173, 91}}, //3 idlel 4
  {Green_ArcMain_IdleL2, {  0,  0,143, 98}, {173, 92}}, //4 idlel 5

  {Green_ArcMain_IdleR0, {  0,  0,144, 97}, {172, 91}}, //5 idler 1
  {Green_ArcMain_IdleR0, {  0, 97,145, 97}, {172, 90}}, //6 idler 2
  {Green_ArcMain_IdleR1, {  0,  0,143, 98}, {172, 91}}, //7 idler 3
  {Green_ArcMain_IdleR1, {  0, 98,142, 98}, {172, 91}}, //8 idler 4
  {Green_ArcMain_IdleR2, {  0,  0,142, 98}, {172, 92}}, //9 idler 5

  {Green_ArcMain_Left0, {  0,  0,180, 94}, {218, 88}}, //10 left 1
  {Green_ArcMain_Left0, {  0, 94,183, 96}, {221, 88}}, //11 left 2
  {Green_ArcMain_Left1, {  0,  0,184, 96}, {222, 88}}, //12 left 3
  {Green_ArcMain_Left1, {  0, 96,183, 95}, {221, 88}}, //13 left 4

  {Green_ArcMain_Down0, {  0,  0,158,102}, {186, 79}}, //14 down 1
  {Green_ArcMain_Down0, {  0,102,157,100}, {185, 80}}, //15 down 2
  {Green_ArcMain_Down1, {  0,  0,157,105}, {185, 85}}, //16 down 3
  {Green_ArcMain_Down1, {  0,105,157,105}, {185, 85}}, //17 down 4
  {Green_ArcMain_Down2, {  0,  0,159,106}, {185, 85}}, //18 down 5
  {Green_ArcMain_Down2, {  0,106,158,105}, {185, 85}}, //19 down 6

  {Green_ArcMain_Up0, {  0,  0,150,109}, {180, 99}}, //20 up 1
  {Green_ArcMain_Up0, {  0,109,151,101}, {178, 96}}, //21 up 2
  {Green_ArcMain_Up1, {  0,  0,147,101}, {170, 96}}, //22 up 3
  {Green_ArcMain_Up1, {  0,101,137,104}, {161, 98}}, //23 up 4
  {Green_ArcMain_Up2, {  0,  0,154,111}, {178,105}}, //24 up 5
  {Green_ArcMain_Up2, {  0,111,158,115}, {182,110}}, //25 up 6
  {Green_ArcMain_Up3, {  0,  0,156,125}, {180,120}}, //26 up 7
  {Green_ArcMain_Up3, {  0,125,162,119}, {186,113}}, //27 up 8
  {Green_ArcMain_Up4, {  0,  0,170,103}, {194, 97}}, //28 up 9
  {Green_ArcMain_Up4, {  0,103,169,102}, {193, 96}}, //29 up 10
  {Green_ArcMain_Up5, {  0,  0,176,103}, {200, 97}}, //30 up 11
  {Green_ArcMain_Up5, {  0,103,176,102}, {200, 97}}, //31 up 12
  {Green_ArcMain_Up6, {  0,  0,173,103}, {197, 98}}, //32 up 13
  {Green_ArcMain_Up6, {  0,103,172,104}, {196, 98}}, //33 up 14
  {Green_ArcMain_Up7, {  0,  0,169,104}, {193, 98}}, //34 up 15
  {Green_ArcMain_Up7, {  0,104,169,104}, {193, 98}}, //35 up 16
  {Green_ArcMain_Up8, {  0,  0,167,104}, {192, 98}}, //36 up 17

  {Green_ArcMain_Right0, {  0,  0,163, 98}, {173, 87}}, //37 right 1
  {Green_ArcMain_Right0, {  0, 98,134, 93}, {135, 87}}, //38 right 2
  {Green_ArcMain_Right1, {  0,  0,134, 93}, {136, 86}}, //39 right 3
  {Green_ArcMain_Right1, {  0, 93,134, 94}, {136, 87}}, //40 right 4

  {Green_ArcMain_Reaction0, {  0,  0,157,122}, {181,117}}, //41 react 1
  {Green_ArcMain_Reaction0, {  0,122,181,103}, {206, 98}}, //42 react 2
  {Green_ArcMain_Reaction1, {  0,  0,182,102}, {206, 97}}, //43 react 3
  {Green_ArcMain_Reaction1, {  0,102,186,112}, {209, 98}}, //44 react 4
  {Green_ArcMain_Reaction2, {  0,  0,201,107}, {223, 98}}, //45 react 5
  {Green_ArcMain_Reaction2, {  0,107,216,102}, {236, 97}}, //46 react 6
  {Green_ArcMain_Reaction3, {  0,  0,222,103}, {242, 98}}, //47 react 7
  {Green_ArcMain_Reaction3, {  0,103,223,115}, {243, 97}}, //48 react 8
  {Green_ArcMain_Reaction4, {  0,  0,229,111}, {248, 98}}, //49 react 9
  {Green_ArcMain_Reaction4, {  0,111,228,109}, {247, 98}}, //50 react 10
  {Green_ArcMain_Reaction5, {  0,  0,228,110}, {247, 97}}, //51 react 11
  {Green_ArcMain_Reaction5, {  0,110,226,106}, {247, 94}}, //52 react 12
  {Green_ArcMain_Reaction6, {  0,  0,225,106}, {247, 94}}, //53 react 13
  {Green_ArcMain_Reaction6, {  0,106,225,106}, {247, 94}}, //54 react 14
  {Green_ArcMain_Reaction7, {  0,  0,226,107}, {247, 95}}, //55 react 15
  {Green_ArcMain_Reaction7, {  0,107,225,107}, {247, 95}}, //56 react 16
  {Green_ArcMain_Reaction8, {  0,  0,225,113}, {246, 95}}, //57 react 17
  {Green_ArcMain_Reaction8, {  0,113,226,113}, {247, 95}}, //58 react 18
  {Green_ArcMain_Reaction9, {  0,  0,226,113}, {247, 95}}, //59 react 19
  {Green_ArcMain_Reaction9, {  0,113,226,113}, {247, 95}}, //60 react 20
  {Green_ArcMain_Reaction10, {  0,  0,226,113}, {247, 95}}, //61 react 21
  {Green_ArcMain_Reaction10, {  0,113,226,113}, {247, 95}}, //62 react 22
  {Green_ArcMain_Reaction11, {  0,  0,227,113}, {247, 95}}, //63 react 23
  {Green_ArcMain_Reaction11, {  0,113,227,114}, {247, 95}}, //64 react 24
  {Green_ArcMain_Reaction12, {  0,  0,226,114}, {246, 95}}, //65 react 25
  {Green_ArcMain_Reaction12, {  0,114,227,114}, {247, 95}}, //66 react 26
  {Green_ArcMain_Reaction13, {  0,  0,226,114}, {247, 95}}, //67 react 27
  {Green_ArcMain_Reaction13, {  0,114,224,115}, {247, 95}}, //68 react 28
  {Green_ArcMain_Reaction14, {  0,  0,223,115}, {247, 95}}, //69 react 29
  {Green_ArcMain_Reaction14, {  0,115,224,115}, {247, 95}}, //70 react 30
  {Green_ArcMain_Reaction15, {  0,  0,223,115}, {247, 94}}, //71 react 31
  {Green_ArcMain_Reaction15, {  0,115,216,114}, {247, 95}}, //72 react 32
  {Green_ArcMain_Reaction16, {  0,  0,197,111}, {247, 95}}, //73 react 33
  {Green_ArcMain_Reaction16, {  0,111,197,116}, {247,100}}, //74 react 34
};

static const Animation char_green_anim[CharAnim_Max] = {
	{2, (const u8[]){ASCR_CHGANI, CharAnim_Idle}}, //CharAnim_Idle
	{2, (const u8[]){ 10, 11, 12, 13, 13, ASCR_BACK, 1}},         //CharAnim_Left
	{2, (const u8[]){ 0, 1, 2, 3, 4, 4, CharAnim_Idle}},   //CharAnim_LeftAlt
	{1, (const u8[]){ 14, 15, 16, 17, 18, 18, 19, 19, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{1, (const u8[]){ 20, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 37, 38, 39, 40, ASCR_BACK, 1}},         //CharAnim_Right
	{2, (const u8[]){ 5, 6, 7, 8, 9, 9, CharAnim_Idle}},   //CharAnim_RightAlt
	
	{1, (const u8[]){ 41, 42, 43, 44, 45, 45, 46, 46, 47, 47, 48, 48, 49, 49, 50, 50, 51, 51, 52, 53, 54, 55, 56, 56, 57, 57, 58, 59, 60, 60, 60, 61, 61, 61, 62, 62, 63, 63, 64, 64, 64, 65, 65, 65, 66, 66, 66, 66, 66, 66, 67, 67, 68, 68, 69, 69, 70, 70, 70, 70, 70, 71, 71, 72, 72, 73, 73, 74, 74, ASCR_CHGANI, CharAnim_Special2}},   //CharAnim_Special1
    {2, (const u8[]){ 74, 74, 74, 74, 74, 74, 74, ASCR_CHGANI, CharAnim_Special2}},   //CharAnim_Special2
};

//Green character functions
void Char_Green_SetFrame(void *user, u8 frame)
{
	Char_Green *this = (Char_Green*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_green_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Green_Tick(Character *character)
{
	Char_Green *this = (Char_Green*)character;
	
	if((character->animatable.anim  != CharAnim_Special1) && (character->animatable.anim  != CharAnim_Special2))
	{
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
	}
	
	//Stage specific animations
		switch (stage.stage_id)
		{
			case StageId_LightsDown: //WHERE THE FUCK DID YOU GO WHAT
				if (stage.song_step == 1600)
					character->set_anim(character, CharAnim_Special1);
				break;
			default:
				break;
		}
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Green_SetFrame);
	Character_Draw(character, &this->tex, &char_green_frame[this->frame]);
}

void Char_Green_SetAnim(Character *character, u8 anim)
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

void Char_Green_Free(Character *character)
{
	Char_Green *this = (Char_Green*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Green_New(fixed_t x, fixed_t y)
{
	//Allocate green object
	Char_Green *this = Mem_Alloc(sizeof(Char_Green));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Green_New] Failed to allocate green object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Green_Tick;
	this->character.set_anim = Char_Green_SetAnim;
	this->character.free = Char_Green_Free;
	
	Animatable_Init(&this->character.animatable, char_green_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;

	//health bar color
	this->character.health_bar = 0xFF1F6B2B;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-115,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\GREEN.ARC;1");
	
	const char **pathp = (const char *[]){
  "idlel0.tim",
  "idlel1.tim",
  "idlel2.tim",
  "idler0.tim",
  "idler1.tim",
  "idler2.tim",
  "left0.tim",
  "left1.tim",
  "down0.tim",
  "down1.tim",
  "down2.tim",
  "up0.tim",
  "up1.tim",
  "up2.tim",
  "up3.tim",
  "up4.tim",
  "up5.tim",
  "up6.tim",
  "up7.tim",
  "up8.tim",
  "right0.tim",
  "right1.tim",
  "react0.tim",
  "react1.tim",
  "react2.tim",
  "react3.tim",
  "react4.tim",
  "react5.tim",
  "react6.tim",
  "react7.tim",
  "react8.tim",
  "react9.tim",
  "react10.tim",
  "react11.tim",
  "react12.tim",
  "react13.tim",
  "react14.tim",
  "react15.tim",
  "react16.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
