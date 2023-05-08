/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "jorsawsee.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Jorsawsee character structure
enum
{
  Jorsawsee_ArcMain_Idle0,
  Jorsawsee_ArcMain_Idle1,
  Jorsawsee_ArcMain_Idle2,
  Jorsawsee_ArcMain_Idle3,
  Jorsawsee_ArcMain_Idle4,
  Jorsawsee_ArcMain_Idle5,
  Jorsawsee_ArcMain_Left0,
  Jorsawsee_ArcMain_Left1,
  Jorsawsee_ArcMain_Down0,
  Jorsawsee_ArcMain_Down1,
  Jorsawsee_ArcMain_Down2,
  Jorsawsee_ArcMain_Down3,
  Jorsawsee_ArcMain_Up0,
  Jorsawsee_ArcMain_Up1,
  Jorsawsee_ArcMain_Up2,
  Jorsawsee_ArcMain_Up3,
  Jorsawsee_ArcMain_Right0,
  Jorsawsee_ArcMain_Right1,
	
	Jorsawsee_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Jorsawsee_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Jorsawsee;

//Jorsawsee character definitions
static const CharFrame char_jorsawsee_frame[] = {
  {Jorsawsee_ArcMain_Idle0, {  0,  0,166,122}, {155, 86}}, //0 idle 1
  {Jorsawsee_ArcMain_Idle0, {  0,122,171,122}, {160, 86}}, //1 idle 2
  {Jorsawsee_ArcMain_Idle1, {  0,  0,172,121}, {162, 85}}, //2 idle 3
  {Jorsawsee_ArcMain_Idle1, {  0,121,173,121}, {163, 85}}, //3 idle 4
  {Jorsawsee_ArcMain_Idle2, {  0,  0,166,123}, {158, 87}}, //4 idle 5
  {Jorsawsee_ArcMain_Idle2, {  0,123,165,123}, {157, 87}}, //5 idle 6
  {Jorsawsee_ArcMain_Idle3, {  0,  0,165,125}, {157, 89}}, //6 idle 7
  {Jorsawsee_ArcMain_Idle3, {  0,125,165,125}, {158, 89}}, //7 idle 8
  {Jorsawsee_ArcMain_Idle4, {  0,  0,172,125}, {165, 90}}, //8 idle 9
  {Jorsawsee_ArcMain_Idle4, {  0,125,172,125}, {165, 90}}, //9 idle 10
  {Jorsawsee_ArcMain_Idle5, {  0,  0,174,126}, {166, 90}}, //10 idle 11
  {Jorsawsee_ArcMain_Idle5, {  0,126,174,126}, {166, 90}}, //11 idle 12

  {Jorsawsee_ArcMain_Left0, {  0,  0,123,123}, {153, 88}}, //12 left 1
  {Jorsawsee_ArcMain_Left0, {  0,123,136,123}, {159, 87}}, //13 left 2
  {Jorsawsee_ArcMain_Left1, {  0,  0,139,123}, {161, 88}}, //14 left 3
  {Jorsawsee_ArcMain_Left1, {  0,123,141,123}, {162, 88}}, //15 left 4

  {Jorsawsee_ArcMain_Down0, {  0,  0,177,118}, {182, 82}}, //16 down 1
  {Jorsawsee_ArcMain_Down0, {  0,118,177,118}, {182, 82}}, //17 down 2
  {Jorsawsee_ArcMain_Down1, {  0,  0,184,120}, {187, 84}}, //18 down 3
  {Jorsawsee_ArcMain_Down1, {  0,120,184,120}, {187, 84}}, //19 down 4
  {Jorsawsee_ArcMain_Down2, {  0,  0,180,121}, {182, 85}}, //20 down 5
  {Jorsawsee_ArcMain_Down2, {  0,121,181,121}, {182, 85}}, //21 down 6
  {Jorsawsee_ArcMain_Down3, {  0,  0,177,123}, {178, 87}}, //22 down 7
  {Jorsawsee_ArcMain_Down3, {  0,123,178,123}, {179, 87}}, //23 down 8

  {Jorsawsee_ArcMain_Up0, {  0,  0,138,133}, {159, 98}}, //24 up 1
  {Jorsawsee_ArcMain_Up1, {  0,  0,145,129}, {157, 94}}, //25 up 2
  {Jorsawsee_ArcMain_Up2, {  0,  0,145,128}, {157, 93}}, //26 up 3
  {Jorsawsee_ArcMain_Up3, {  0,  0,147,127}, {157, 93}}, //27 up 4

  {Jorsawsee_ArcMain_Right0, {  0,  0,188,124}, {166, 88}}, //28 right 1
  {Jorsawsee_ArcMain_Right0, {  0,124,170,122}, {160, 86}}, //29 right 2
  {Jorsawsee_ArcMain_Right1, {  0,  0,166,122}, {159, 86}}, //30 right 3
  {Jorsawsee_ArcMain_Right1, {  0,122,165,122}, {159, 86}}, //31 right 4
};

static const Animation char_jorsawsee_anim[CharAnim_Max] = {
	{1, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 11, 11, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 12, 13, 14, 15, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{1, (const u8[]){ 16, 17, 18, 19, 20, 21, 22, 23, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 24, 25, 26, 27, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 28, 29, 30, 31, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Jorsawsee character functions
void Char_Jorsawsee_SetFrame(void *user, u8 frame)
{
	Char_Jorsawsee *this = (Char_Jorsawsee*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_jorsawsee_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Jorsawsee_Tick(Character *character)
{
	Char_Jorsawsee *this = (Char_Jorsawsee*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Jorsawsee_SetFrame);
	if (stage.song_step <= 408)
		Character_Draw(character, &this->tex, &char_jorsawsee_frame[this->frame]);
}

void Char_Jorsawsee_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Jorsawsee_Free(Character *character)
{
	Char_Jorsawsee *this = (Char_Jorsawsee*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Jorsawsee_New(fixed_t x, fixed_t y)
{
	//Allocate jorsawsee object
	Char_Jorsawsee *this = Mem_Alloc(sizeof(Char_Jorsawsee));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Jorsawsee_New] Failed to allocate jorsawsee object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Jorsawsee_Tick;
	this->character.set_anim = Char_Jorsawsee_SetAnim;
	this->character.free = Char_Jorsawsee_Free;
	
	Animatable_Init(&this->character.animatable, char_jorsawsee_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 0;

	//health bar color
	this->character.health_bar = 0xFF277FE7;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-115,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\OPPONENT\\JORSAWSE.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "idle2.tim",
  "idle3.tim",
  "idle4.tim",
  "idle5.tim",
  "left0.tim",
  "left1.tim",
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
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
