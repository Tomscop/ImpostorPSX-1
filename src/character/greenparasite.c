/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "greenparasite.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//GreenParasite character structure
enum
{
  GreenParasite_ArcMain_Idle0,
  GreenParasite_ArcMain_Idle1,
  GreenParasite_ArcMain_Idle2,
  GreenParasite_ArcMain_Idle3,
  GreenParasite_ArcMain_Idle4,
  GreenParasite_ArcMain_Idle5,
  GreenParasite_ArcMain_Left0,
  GreenParasite_ArcMain_Left1,
  GreenParasite_ArcMain_Left2,
  GreenParasite_ArcMain_Down0,
  GreenParasite_ArcMain_Down1,
  GreenParasite_ArcMain_Down2,
  GreenParasite_ArcMain_Up0,
  GreenParasite_ArcMain_Up1,
  GreenParasite_ArcMain_Right0,
  GreenParasite_ArcMain_Right1,
	
	GreenParasite_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[GreenParasite_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_GreenParasite;

//GreenParasite character definitions
static const CharFrame char_greenparasite_frame[] = {
  {GreenParasite_ArcMain_Idle0, {  0,  0,100, 81}, {204,171}}, //0 idle 1
  {GreenParasite_ArcMain_Idle0, {100,  0,101, 80}, {205,170}}, //1 idle 2
  {GreenParasite_ArcMain_Idle0, {  0, 81,101, 80}, {205,170}}, //2 idle 3
  {GreenParasite_ArcMain_Idle0, {101, 81,101, 82}, {204,171}}, //3 idle 4
  {GreenParasite_ArcMain_Idle0, {  0,163,107, 80}, {208,170}}, //4 idle 5
  {GreenParasite_ArcMain_Idle0, {107,163,107, 79}, {208,169}}, //5 idle 6
  {GreenParasite_ArcMain_Idle1, {  0,  0,102, 80}, {203,170}}, //6 idle 7
  {GreenParasite_ArcMain_Idle1, {102,  0,103, 79}, {204,169}}, //7 idle 8
  {GreenParasite_ArcMain_Idle1, {  0, 80, 94, 79}, {200,169}}, //8 idle 9
  {GreenParasite_ArcMain_Idle1, { 94, 80, 94, 78}, {201,168}}, //9 idle 10
  {GreenParasite_ArcMain_Idle1, {  0,160, 90, 80}, {200,170}}, //10 idle 11
  {GreenParasite_ArcMain_Idle1, { 90,160, 91, 80}, {200,170}}, //11 idle 12
  {GreenParasite_ArcMain_Idle2, {  0,  0, 91, 80}, {201,170}}, //12 idle 13
  {GreenParasite_ArcMain_Idle2, { 91,  0, 91, 79}, {201,169}}, //13 idle 14
  {GreenParasite_ArcMain_Idle2, {  0, 80, 95, 80}, {204,170}}, //14 idle 15
  {GreenParasite_ArcMain_Idle2, { 95, 80, 95, 81}, {204,171}}, //15 idle 16
  {GreenParasite_ArcMain_Idle2, {  0,161,101, 80}, {207,170}}, //16 idle 17
  {GreenParasite_ArcMain_Idle2, {101,161,100, 79}, {207,169}}, //17 idle 18
  {GreenParasite_ArcMain_Idle3, {  0,  0,102, 82}, {204,172}}, //18 idle 19
  {GreenParasite_ArcMain_Idle3, {102,  0,102, 81}, {204,171}}, //19 idle 20
  {GreenParasite_ArcMain_Idle3, {  0, 82,100, 81}, {204,171}}, //20 idle 21
  {GreenParasite_ArcMain_Idle3, {100, 82, 99, 80}, {204,170}}, //21 idle 22
  {GreenParasite_ArcMain_Idle3, {  0,164,101, 80}, {207,170}}, //22 idle 23
  {GreenParasite_ArcMain_Idle3, {101,164,101, 81}, {208,171}}, //23 idle 24
  {GreenParasite_ArcMain_Idle4, {  0,  0, 90, 79}, {200,169}}, //24 idle 25
  {GreenParasite_ArcMain_Idle4, { 90,  0, 91, 81}, {200,171}}, //25 idle 26
  {GreenParasite_ArcMain_Idle4, {  0, 81, 90, 79}, {200,169}}, //26 idle 27
  {GreenParasite_ArcMain_Idle4, { 90, 81, 90, 80}, {200,170}}, //27 idle 28
  {GreenParasite_ArcMain_Idle4, {  0,162, 89, 80}, {200,170}}, //28 idle 29
  {GreenParasite_ArcMain_Idle4, { 89,162, 90, 80}, {200,170}}, //29 idle 30
  {GreenParasite_ArcMain_Idle5, {  0,  0, 94, 80}, {200,170}}, //30 idle 31
  {GreenParasite_ArcMain_Idle5, { 94,  0, 93, 81}, {200,171}}, //31 idle 32
  {GreenParasite_ArcMain_Idle5, {  0, 81,103, 79}, {204,170}}, //32 idle 33
  {GreenParasite_ArcMain_Idle5, {103, 81,102, 81}, {203,171}}, //33 idle 34
  {GreenParasite_ArcMain_Idle5, {  0,162,104, 80}, {208,170}}, //34 idle 35
  {GreenParasite_ArcMain_Idle5, {104,162,104, 81}, {207,171}}, //35 idle 36

  {GreenParasite_ArcMain_Left0, {  0,  0,100, 81}, {221-7,162+8}}, //36 left 1
  {GreenParasite_ArcMain_Left0, {100,  0, 99, 81}, {220-7,162+8}}, //37 left 2
  {GreenParasite_ArcMain_Left0, {  0, 81,100, 81}, {219-7,161+8}}, //38 left 3
  {GreenParasite_ArcMain_Left0, {100, 81,100, 81}, {219-7,161+8}}, //39 left 4
  {GreenParasite_ArcMain_Left0, {  0,162,100, 81}, {218-7,161+8}}, //40 left 5
  {GreenParasite_ArcMain_Left0, {100,162,100, 81}, {218-7,161+8}}, //41 left 6
  {GreenParasite_ArcMain_Left1, {  0,  0, 95, 81}, {215-7,161+8}}, //42 left 7
  {GreenParasite_ArcMain_Left1, { 95,  0, 96, 80}, {216-7,160+8}}, //43 left 8
  {GreenParasite_ArcMain_Left1, {  0, 81, 96, 80}, {216-7,160+8}}, //44 left 9
  {GreenParasite_ArcMain_Left1, { 96, 81, 91, 85}, {215-7,161+8}}, //45 left 10
  {GreenParasite_ArcMain_Left1, {  0,166, 91, 85}, {215-7,161+8}}, //46 left 11
  {GreenParasite_ArcMain_Left1, { 91,166, 90, 86}, {214-7,161+8}}, //47 left 12
  {GreenParasite_ArcMain_Left2, {  0,  0, 90, 84}, {214-7,159+8}}, //48 left 13
  {GreenParasite_ArcMain_Left2, { 90,  0, 90, 86}, {214-7,161+8}}, //49 left 14

  {GreenParasite_ArcMain_Down0, {  0,  0, 87, 71}, {184+10,140+18}}, //50 down 1
  {GreenParasite_ArcMain_Down0, { 87,  0, 86, 71}, {183+10,140+18}}, //51 down 2
  {GreenParasite_ArcMain_Down0, {  0, 71, 86, 71}, {183+10,142+18}}, //52 down 3
  {GreenParasite_ArcMain_Down0, { 86, 71, 86, 71}, {184+10,142+18}}, //53 down 4
  {GreenParasite_ArcMain_Down0, {  0,142, 87, 71}, {185+10,142+18}}, //54 down 5
  {GreenParasite_ArcMain_Down0, { 87,142, 86, 72}, {185+10,143+18}}, //55 down 6
  {GreenParasite_ArcMain_Down1, {  0,  0, 88, 71}, {186+10,143+18}}, //56 down 7
  {GreenParasite_ArcMain_Down1, { 88,  0, 87, 71}, {185+10,143+18}}, //57 down 8
  {GreenParasite_ArcMain_Down1, {  0, 71, 90, 71}, {188+10,143+18}}, //58 down 9
  {GreenParasite_ArcMain_Down1, { 90, 71, 90, 71}, {188+10,143+18}}, //59 down 10
  {GreenParasite_ArcMain_Down1, {  0,142, 87, 71}, {185+10,143+18}}, //60 down 11
  {GreenParasite_ArcMain_Down1, { 87,142, 87, 71}, {186+10,143+18}}, //61 down 12
  {GreenParasite_ArcMain_Down2, {  0,  0, 86, 71}, {185+10,143+18}}, //62 down 13
  {GreenParasite_ArcMain_Down2, { 86,  0, 86, 72}, {185+10,143+18}}, //63 down 14

  {GreenParasite_ArcMain_Up0, {  0,  0, 78, 85}, {185+9,177-3}}, //64 up 1
  {GreenParasite_ArcMain_Up0, { 78,  0, 77, 87}, {184+9,179-3}}, //65 up 2
  {GreenParasite_ArcMain_Up0, {155,  0, 77, 83}, {184+9,175-3}}, //66 up 3
  {GreenParasite_ArcMain_Up0, {  0, 87, 78, 83}, {185+9,175-3}}, //67 up 4
  {GreenParasite_ArcMain_Up0, { 78, 87, 77, 81}, {185+9,173-3}}, //68 up 5
  {GreenParasite_ArcMain_Up0, {155, 87, 77, 81}, {184+9,173-3}}, //69 up 6
  {GreenParasite_ArcMain_Up1, {  0,  0, 77, 78}, {184+9,171-3}}, //70 up 7
  {GreenParasite_ArcMain_Up1, { 77,  0, 77, 78}, {185+9,171-3}}, //71 up 8
  {GreenParasite_ArcMain_Up1, {154,  0, 77, 77}, {185+9,170-3}}, //72 up 9
  {GreenParasite_ArcMain_Up1, {  0, 78, 77, 77}, {184+9,170-3}}, //73 up 10
  {GreenParasite_ArcMain_Up1, { 77, 78, 78, 77}, {185+9,170-3}}, //74 up 11
  {GreenParasite_ArcMain_Up1, {155, 78, 77, 77}, {185+9,170-3}}, //75 up 12
  {GreenParasite_ArcMain_Up1, {  0,156, 77, 77}, {184+9,170-3}}, //76 up 13
  {GreenParasite_ArcMain_Up1, { 77,156, 78, 77}, {185+9,170-3}}, //77 up 14

  {GreenParasite_ArcMain_Right0, {  0,  0, 96, 80}, {169+19,158+12}}, //78 right 1
  {GreenParasite_ArcMain_Right0, { 96,  0, 95, 80}, {169+19,158+12}}, //79 right 2
  {GreenParasite_ArcMain_Right0, {  0, 80, 92, 79}, {169+19,158+12}}, //80 right 3
  {GreenParasite_ArcMain_Right0, { 92, 80, 93, 79}, {170+19,158+12}}, //81 right 4
  {GreenParasite_ArcMain_Right0, {  0,160, 89, 79}, {168+19,158+12}}, //82 right 5
  {GreenParasite_ArcMain_Right0, { 89,160, 89, 80}, {168+19,159+12}}, //83 right 6
  {GreenParasite_ArcMain_Right1, {  0,  0, 86, 80}, {167+19,159+12}}, //84 right 7
  {GreenParasite_ArcMain_Right1, { 86,  0, 87, 80}, {167+19,159+12}}, //85 right 8
  {GreenParasite_ArcMain_Right1, {  0, 80, 87, 79}, {167+19,159+12}}, //86 right 9
  {GreenParasite_ArcMain_Right1, { 87, 80, 86, 78}, {167+19,158+12}}, //87 right 10
  {GreenParasite_ArcMain_Right1, {173, 80, 81, 77}, {166+19,157+12}}, //88 right 11
  {GreenParasite_ArcMain_Right1, {  0,160, 81, 78}, {166+19,158+12}}, //89 right 12
  {GreenParasite_ArcMain_Right1, { 81,160, 81, 78}, {167+19,158+12}}, //90 right 13
  {GreenParasite_ArcMain_Right1, {162,160, 81, 78}, {167+19,158+12}}, //91 right 14
};

static const Animation char_greenparasite_anim[CharAnim_Max] = {
	{1, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, ASCR_CHGANI, CharAnim_Idle}}, //CharAnim_Idle
	{1, (const u8[]){ 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{1, (const u8[]){ 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{1, (const u8[]){ 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{1, (const u8[]){ 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//GreenParasite character functions
void Char_GreenParasite_SetFrame(void *user, u8 frame)
{
	Char_GreenParasite *this = (Char_GreenParasite*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_greenparasite_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_GreenParasite_Tick(Character *character)
{
	Char_GreenParasite *this = (Char_GreenParasite*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_GreenParasite_SetFrame);
	Character_Draw(character, &this->tex, &char_greenparasite_frame[this->frame]);
}

void Char_GreenParasite_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_GreenParasite_Free(Character *character)
{
	Char_GreenParasite *this = (Char_GreenParasite*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_GreenParasite_New(fixed_t x, fixed_t y)
{
	//Allocate greenparasite object
	Char_GreenParasite *this = Mem_Alloc(sizeof(Char_GreenParasite));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_GreenParasite_New] Failed to allocate greenparasite object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_GreenParasite_Tick;
	this->character.set_anim = Char_GreenParasite_SetAnim;
	this->character.free = Char_GreenParasite_Free;
	
	Animatable_Init(&this->character.animatable, char_greenparasite_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 6;

	//health bar color
	this->character.health_bar = 0xFF003315;
	
	this->character.focus_x = FIXED_DEC(-300,1);
	this->character.focus_y = FIXED_DEC(-481,1);
	this->character.focus_zoom = FIXED_DEC(277,512);
	
	this->character.size = FIXED_DEC(334,100);
	
	//Load art
	this->arc_main = IO_Read("\\OPPONENT\\GREENPAR.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "idle2.tim",
  "idle3.tim",
  "idle4.tim",
  "idle5.tim",
  "left0.tim",
  "left1.tim",
  "left2.tim",
  "down0.tim",
  "down1.tim",
  "down2.tim",
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
