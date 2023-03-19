/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "chefogus.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Chefogus character structure
enum
{
  Chefogus_ArcMain_Idle0,
  Chefogus_ArcMain_Idle1,
  Chefogus_ArcMain_Idle2,
  Chefogus_ArcMain_Idle3,
  Chefogus_ArcMain_Left0,
  Chefogus_ArcMain_Left1,
  Chefogus_ArcMain_Left2,
  Chefogus_ArcMain_Left3,
  Chefogus_ArcMain_Down0,
  Chefogus_ArcMain_Down1,
  Chefogus_ArcMain_Down2,
  Chefogus_ArcMain_Down3,
  Chefogus_ArcMain_Down4,
  Chefogus_ArcMain_Up0,
  Chefogus_ArcMain_Up1,
  Chefogus_ArcMain_Up2,
  Chefogus_ArcMain_Up3,
  Chefogus_ArcMain_Up4,
  Chefogus_ArcMain_Right0,
  Chefogus_ArcMain_Right1,
  Chefogus_ArcMain_Right2,
  Chefogus_ArcMain_Right3,
  Chefogus_ArcMain_Right4,
  Chefogus_ArcMain_Right5,
  Chefogus_ArcMain_Right6,
  Chefogus_ArcMain_Right7,
  Chefogus_ArcMain_Right8,
  Chefogus_ArcMain_DownAlt0,
  Chefogus_ArcMain_DownAlt1,
  Chefogus_ArcMain_DownAlt2,
  Chefogus_ArcMain_DownAlt3,
  Chefogus_ArcMain_DownAlt4,
  Chefogus_ArcMain_UpAlt0,
  Chefogus_ArcMain_UpAlt1,
  Chefogus_ArcMain_UpAlt2,
  Chefogus_ArcMain_UpAlt3,
	
	Chefogus_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Chefogus_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Chefogus;

//Chefogus character definitions
static const CharFrame char_chefogus_frame[] = {
  {Chefogus_ArcMain_Idle0, {  0,  0, 96,151}, {156,141}}, //0 idle 1
  {Chefogus_ArcMain_Idle0, { 96,  0, 95,151}, {156,140}}, //1 idle 2
  {Chefogus_ArcMain_Idle1, {  0,  0, 96,154}, {157,141}}, //2 idle 3
  {Chefogus_ArcMain_Idle1, { 96,  0, 97,156}, {159,143}}, //3 idle 4
  {Chefogus_ArcMain_Idle2, {  0,  0, 98,155}, {159,142}}, //4 idle 5
  {Chefogus_ArcMain_Idle2, { 98,  0, 99,155}, {160,142}}, //5 idle 6
  {Chefogus_ArcMain_Idle3, {  0,  0, 98,155}, {160,142}}, //6 idle 7

  {Chefogus_ArcMain_Left0, {  0,  0,137,137}, {172,135}}, //7 left 1
  {Chefogus_ArcMain_Left1, {  0,  0,137,140}, {170,138}}, //8 left 2
  {Chefogus_ArcMain_Left2, {  0,  0,137,141}, {170,139}}, //9 left 3
  {Chefogus_ArcMain_Left3, {  0,  0,136,141}, {170,139}}, //10 left 4

  {Chefogus_ArcMain_Down0, {  0,  0,162,147}, {149,129}}, //11 down 1
  {Chefogus_ArcMain_Down1, {  0,  0,162,146}, {149,132}}, //12 down 2
  {Chefogus_ArcMain_Down2, {  0,  0,164,146}, {150,132}}, //13 down 3
  {Chefogus_ArcMain_Down3, {  0,  0,162,146}, {150,133}}, //14 down 4
  {Chefogus_ArcMain_Down4, {  0,  0,159,146}, {150,133}}, //15 down 5

  {Chefogus_ArcMain_Up0, {  0,  0,142,155}, {165,153}}, //16 up 1
  {Chefogus_ArcMain_Up1, {  0,  0,143,153}, {166,150}}, //17 up 2
  {Chefogus_ArcMain_Up2, {  0,  0,142,153}, {165,150}}, //18 up 3
  {Chefogus_ArcMain_Up3, {  0,  0,144,153}, {167,150}}, //19 up 4
  {Chefogus_ArcMain_Up4, {  0,  0,144,153}, {167,150}}, //20 up 5

  {Chefogus_ArcMain_Right0, {  0,  0,187,143}, {182,139}}, //21 right 1
  {Chefogus_ArcMain_Right1, {  0,  0,181,145}, {178,141}}, //22 right 2
  {Chefogus_ArcMain_Right2, {  0,  0,181,145}, {179,141}}, //23 right 3
  {Chefogus_ArcMain_Right3, {  0,  0,180,145}, {179,141}}, //24 right 4
  {Chefogus_ArcMain_Right4, {  0,  0,180,145}, {179,141}}, //25 right 5
  {Chefogus_ArcMain_Right5, {  0,  0,180,144}, {179,141}}, //26 right 6
  {Chefogus_ArcMain_Right6, {  0,  0,173,144}, {171,141}}, //27 right 7
  {Chefogus_ArcMain_Right7, {  0,  0,172,144}, {170,141}}, //28 right 8
  {Chefogus_ArcMain_Right8, {  0,  0,172,144}, {171,141}}, //29 right 9

  {Chefogus_ArcMain_DownAlt0, {  0,  0,128,133}, {149,129}}, //30 downalt 1
  {Chefogus_ArcMain_DownAlt1, {  0,  0,128,130}, {151,126}}, //31 downalt 2
  {Chefogus_ArcMain_DownAlt2, {  0,  0,129,130}, {152,126}}, //32 downalt 3
  {Chefogus_ArcMain_DownAlt3, {  0,  0,129,131}, {152,126}}, //33 downalt 4
  {Chefogus_ArcMain_DownAlt4, {  0,  0,129,131}, {152,126}}, //34 downalt 5

  {Chefogus_ArcMain_UpAlt0, {  0,  0,133,142}, {168,139}}, //35 upalt 1
  {Chefogus_ArcMain_UpAlt1, {  0,  0,132,144}, {166,141}}, //36 upalt 2
  {Chefogus_ArcMain_UpAlt2, {  0,  0,133,144}, {166,141}}, //37 upalt 3
  {Chefogus_ArcMain_UpAlt3, {  0,  0,133,143}, {167,140}}, //38 upalt 4
};

static const Animation char_chefogus_anim[CharAnim_Max] = {
	{1, (const u8[]){ 0, 0, 1, 1, 2, 2, 3, 3, 4, 5, 6, 6, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 7, 8, 9, 10, 10, 10, 10, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 11, 12, 13, 14, 15, 15, 15, ASCR_BACK, 1}},         //CharAnim_Down
	{2, (const u8[]){ 30, 31, 32, 33, 34, 34, 34, ASCR_BACK, 1}},   //CharAnim_DownAlt
	{2, (const u8[]){ 16, 17, 18, 19, 20, 20, ASCR_BACK, 1}},         //CharAnim_Up
	{2, (const u8[]){ 35, 36, 37, 38, 38, 38, ASCR_BACK, 1}},   //CharAnim_UpAlt
	{1, (const u8[]){ 21, 21, 22, 23, 24, 25, 26, 27, 28, 29, 29, 29, 29, 29, 29, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Chefogus character functions
void Char_Chefogus_SetFrame(void *user, u8 frame)
{
	Char_Chefogus *this = (Char_Chefogus*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_chefogus_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Chefogus_Tick(Character *character)
{
	Char_Chefogus *this = (Char_Chefogus*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Chefogus_SetFrame);
	Character_Draw(character, &this->tex, &char_chefogus_frame[this->frame]);
}

void Char_Chefogus_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Chefogus_Free(Character *character)
{
	Char_Chefogus *this = (Char_Chefogus*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Chefogus_New(fixed_t x, fixed_t y)
{
	//Allocate chefogus object
	Char_Chefogus *this = Mem_Alloc(sizeof(Char_Chefogus));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Chefogus_New] Failed to allocate chefogus object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Chefogus_Tick;
	this->character.set_anim = Char_Chefogus_SetAnim;
	this->character.free = Char_Chefogus_Free;
	
	Animatable_Init(&this->character.animatable, char_chefogus_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 4;

	//health bar color
	this->character.health_bar = 0xFFC75E52;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-91,1);
	this->character.focus_zoom = FIXED_DEC(1130,1024);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\OPPONENT\\CHEF.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "idle2.tim",
  "idle3.tim",
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
  "up4.tim",
  "right0.tim",
  "right1.tim",
  "right2.tim",
  "right3.tim",
  "right4.tim",
  "right5.tim",
  "right6.tim",
  "right7.tim",
  "right8.tim",
  "downalt0.tim",
  "downalt1.tim",
  "downalt2.tim",
  "downalt3.tim",
  "downalt4.tim",
  "upalt0.tim",
  "upalt1.tim",
  "upalt2.tim",
  "upalt3.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
