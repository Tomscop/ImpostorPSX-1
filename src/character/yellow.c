/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "yellow.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Yellow character structure
enum
{
  Yellow_ArcMain_Idle0,
  Yellow_ArcMain_Idle1,
  Yellow_ArcMain_Idle2,
  Yellow_ArcMain_Left0,
  Yellow_ArcMain_Left1,
  Yellow_ArcMain_Down0,
  Yellow_ArcMain_Down1,
  Yellow_ArcMain_Down2,
  Yellow_ArcMain_Down3,
  Yellow_ArcMain_Up0,
  Yellow_ArcMain_Up1,
  Yellow_ArcMain_Right0,
  Yellow_ArcMain_Right1,
  Yellow_ArcMain_Death0,
  Yellow_ArcMain_Death1,
  Yellow_ArcMain_Death2,
  Yellow_ArcMain_Death3,
  Yellow_ArcMain_Death4,
  Yellow_ArcMain_Death5,
  Yellow_ArcMain_Death6,
  Yellow_ArcMain_Death7,
  Yellow_ArcMain_Death8,
	
	Yellow_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Yellow_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Yellow;

//Yellow character definitions
static const CharFrame char_yellow_frame[] = {
  {Yellow_ArcMain_Idle0, {  0,  0,100,109}, {150,119}}, //0 idle 1
  {Yellow_ArcMain_Idle0, {100,  0, 98,109}, {149,118}}, //1 idle 2
  {Yellow_ArcMain_Idle0, {  0,109, 99,113}, {149,123}}, //2 idle 3
  {Yellow_ArcMain_Idle0, { 99,109,100,113}, {149,123}}, //3 idle 4
  {Yellow_ArcMain_Idle1, {  0,  0,101,113}, {149,122}}, //4 idle 5
  {Yellow_ArcMain_Idle1, {101,  0,101,112}, {149,122}}, //5 idle 6
  {Yellow_ArcMain_Idle1, {  0,113,100,112}, {149,122}}, //6 idle 7
  {Yellow_ArcMain_Idle1, {100,113,108,109}, {150,119}}, //7 idle 8
  {Yellow_ArcMain_Idle2, {  0,  0,103,108}, {149,118}}, //8 idle 9
  {Yellow_ArcMain_Idle2, {103,  0, 99,113}, {149,123}}, //9 idle 10
  {Yellow_ArcMain_Idle2, {  0,113,100,113}, {149,123}}, //10 idle 11
  {Yellow_ArcMain_Idle2, {100,113,100,113}, {149,122}}, //11 idle 12

  {Yellow_ArcMain_Left0, {  0,  0,107, 90}, {158, 99}}, //12 left 1
  {Yellow_ArcMain_Left0, {107,  0,116, 97}, {165,105}}, //13 left 2
  {Yellow_ArcMain_Left0, {  0, 97,111, 89}, {161, 98}}, //14 left 3
  {Yellow_ArcMain_Left0, {111, 97,111, 87}, {162, 96}}, //15 left 4
  {Yellow_ArcMain_Left1, {  0,  0,110, 87}, {162, 96}}, //16 left 5

  {Yellow_ArcMain_Down0, {  0,  0,104, 95}, {141,104}}, //17 down 1
  {Yellow_ArcMain_Down0, {104,  0,108, 88}, {137, 90}}, //18 down 2
  {Yellow_ArcMain_Down0, {  0, 95,109, 88}, {139, 95}}, //19 down 3
  {Yellow_ArcMain_Down0, {109, 95,108, 88}, {138, 96}}, //20 down 4
  {Yellow_ArcMain_Down1, {  0,  0,108, 88}, {138, 96}}, //21 down 5
  {Yellow_ArcMain_Down1, {108,  0,108, 87}, {138, 94}}, //22 down 6
  {Yellow_ArcMain_Down1, {  0, 88,107, 80}, {138, 88}}, //23 down 7
  {Yellow_ArcMain_Down1, {107, 88,105, 78}, {138, 86}}, //24 down 8
  {Yellow_ArcMain_Down2, {  0,  0,104, 78}, {138, 86}}, //25 down 9
  {Yellow_ArcMain_Down2, {104,  0,105, 78}, {138, 86}}, //26 down 10
  {Yellow_ArcMain_Down2, {  0, 78,105, 78}, {138, 86}}, //27 down 11
  {Yellow_ArcMain_Down2, {105, 78,105, 79}, {138, 87}}, //28 down 12
  {Yellow_ArcMain_Down2, {  0,157,104, 83}, {138, 91}}, //29 down 13
  {Yellow_ArcMain_Down2, {104,157,105, 84}, {138, 92}}, //30 down 14
  {Yellow_ArcMain_Down3, {  0,  0,105, 84}, {138, 92}}, //31 down 15
  {Yellow_ArcMain_Down3, {105,  0,104, 84}, {138, 92}}, //32 down 16
  {Yellow_ArcMain_Down3, {  0, 84,104, 84}, {138, 92}}, //33 down 17
  {Yellow_ArcMain_Down3, {104, 84,106, 84}, {138, 92}}, //34 down 18
  {Yellow_ArcMain_Down3, {  0,168,105, 84}, {138, 92}}, //35 down 19

  {Yellow_ArcMain_Up0, {  0,  0,106,107}, {157,117}}, //36 up 1
  {Yellow_ArcMain_Up0, {106,  0,111,107}, {165,117}}, //37 up 2
  {Yellow_ArcMain_Up0, {  0,107,109,108}, {162,118}}, //38 up 3
  {Yellow_ArcMain_Up0, {109,107,108,108}, {161,118}}, //39 up 4
  {Yellow_ArcMain_Up1, {  0,  0,109,108}, {161,118}}, //40 up 5

  {Yellow_ArcMain_Right0, {  0,  0,110,105}, {160,115}}, //41 right 1
  {Yellow_ArcMain_Right0, {110,  0,141,101}, {155,111}}, //42 right 2
  {Yellow_ArcMain_Right0, {  0,105,140,104}, {158,114}}, //43 right 3
  {Yellow_ArcMain_Right1, {  0,  0,139,104}, {159,114}}, //44 right 4
  {Yellow_ArcMain_Right1, {  0,104,140,104}, {159,114}}, //45 right 5

  {Yellow_ArcMain_Death0, {  0,  0,106,109}, {150,118}}, //46 death 1
  {Yellow_ArcMain_Death0, {106,  0,127,104}, {150,113}}, //47 death 2
  {Yellow_ArcMain_Death0, {  0,109,129,104}, {149,113}}, //48 death 3
  {Yellow_ArcMain_Death1, {  0,  0,130, 89}, {150, 98}}, //49 death 4
  {Yellow_ArcMain_Death1, {  0, 89,129, 89}, {150, 98}}, //50 death 5
  {Yellow_ArcMain_Death2, {  0,  0,134, 93}, {150,102}}, //51 death 6
  {Yellow_ArcMain_Death2, {  0, 93,138, 93}, {150,102}}, //52 death 7
  {Yellow_ArcMain_Death3, {  0,  0,140, 93}, {150,102}}, //53 death 8
  {Yellow_ArcMain_Death3, {  0, 93,122, 93}, {150,102}}, //54 death 9
  {Yellow_ArcMain_Death3, {122, 93,132,105}, {150,105}}, //55 death 10
  {Yellow_ArcMain_Death4, {  0,  0,132,105}, {150,105}}, //56 death 11
  {Yellow_ArcMain_Death4, {  0,105,140,107}, {150,107}}, //57 death 12
  {Yellow_ArcMain_Death5, {  0,  0,137,107}, {150,107}}, //58 death 13
  {Yellow_ArcMain_Death5, {  0,107,138,106}, {150,106}}, //59 death 14
  {Yellow_ArcMain_Death6, {  0,  0,137,103}, {150,103}}, //60 death 15
  {Yellow_ArcMain_Death6, {  0,103,122,100}, {150,100}}, //61 death 16
  {Yellow_ArcMain_Death6, {122,103,103, 91}, {150, 91}}, //62 death 17
  {Yellow_ArcMain_Death7, {  0,  0, 98, 91}, {150, 91}}, //63 death 18
  {Yellow_ArcMain_Death7, { 98,  0, 98, 76}, {150, 76}}, //64 death 19
  {Yellow_ArcMain_Death7, {  0, 91, 98, 54}, {150, 54}}, //65 death 20
  {Yellow_ArcMain_Death7, { 98, 91,102, 46}, {150, 46}}, //66 death 21
  {Yellow_ArcMain_Death8, {  0,  0,103, 46}, {150, 46}}, //67 death 22
  {Yellow_ArcMain_Death8, {103,  0, 97, 46}, {150, 46}}, //68 death 23
  {Yellow_ArcMain_Death8, {  0, 46, 97, 46}, {149, 46}}, //69 death 24
  {Yellow_ArcMain_Death8, { 97, 46, 97, 46}, {150, 46}}, //70 death 25
  {Yellow_ArcMain_Death8, {  0, 92, 98, 46}, {150, 46}}, //71 death 26
};

static const Animation char_yellow_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 5, 6, ASCR_CHGANI, CharAnim_Idle}}, //CharAnim_Idle
	{1, (const u8[]){ 12, 13, 13, 14, 14, 15, 15, 16, 16, ASCR_BACK, 1}},         //CharAnim_Left
	{1, (const u8[]){ 46, 47, 48, 49, 50, 51, 51, 52, 52, 53, 53, 54, 54, 55, 56, 57, 58, 59, 59, 60, 60, 61, 61, 62, 63, 64, 64, 65, 66, 67, 68, 68, 69, 69, 70, 70, 71, 71, ASCR_CHGANI, CharAnim_DownAlt}},   //CharAnim_LeftAlt
	{1, (const u8[]){ 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 25, 26, 27, 28, 28, 29, 29, 30, 30, 31, 31, 32, 32, 33, 33, 34, 34, 35, ASCR_CHGANI, CharAnim_Idle}},         //CharAnim_Down
	{2, (const u8[]){ 71, 71, 71, 71, 71, 71, ASCR_CHGANI, CharAnim_DownAlt}},   //CharAnim_DownAlt
	{1, (const u8[]){ 36, 37, 37, 38, 38, 39, 39, 40, 40, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{1, (const u8[]){ 41, 42, 42, 43, 43, 44, 44, 45, 45, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Yellow character functions
void Char_Yellow_SetFrame(void *user, u8 frame)
{
	Char_Yellow *this = (Char_Yellow*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_yellow_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Yellow_Tick(Character *character)
{
	Char_Yellow *this = (Char_Yellow*)character;
	
	if((character->animatable.anim  != CharAnim_LeftAlt) && (character->animatable.anim  != CharAnim_DownAlt) && (character->animatable.anim  != CharAnim_Down))
	{
	   //Perform idle dance
	   if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
			Character_PerformIdle(character);
	}
	
	//Stage specific animations
		switch (stage.stage_id)
		{
			case StageId_Dlow: //Major L
				if (stage.song_step == 1424)
					character->set_anim(character, CharAnim_LeftAlt);
				break;
			default:
				break;
		}
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Yellow_SetFrame);
	Character_Draw(character, &this->tex, &char_yellow_frame[this->frame]);
}

void Char_Yellow_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Yellow_Free(Character *character)
{
	Char_Yellow *this = (Char_Yellow*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Yellow_New(fixed_t x, fixed_t y)
{
	//Allocate yellow object
	Char_Yellow *this = Mem_Alloc(sizeof(Char_Yellow));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Yellow_New] Failed to allocate yellow object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Yellow_Tick;
	this->character.set_anim = Char_Yellow_SetAnim;
	this->character.free = Char_Yellow_Free;
	
	Animatable_Init(&this->character.animatable, char_yellow_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	this->character.idle2 = 0;
	
	this->character.health_i = 1;

	//health bar color
	this->character.health_bar = 0xFFF0B946;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-115,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\YELLOW.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "idle2.tim",
  "left0.tim",
  "left1.tim",
  "down0.tim",
  "down1.tim",
  "down2.tim",
  "down3.tim",
  "up0.tim",
  "up1.tim",
  "right0.tim",
  "right1.tim",
  "death0.tim",
  "death1.tim",
  "death2.tim",
  "death3.tim",
  "death4.tim",
  "death5.tim",
  "death6.tim",
  "death7.tim",
  "death8.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
