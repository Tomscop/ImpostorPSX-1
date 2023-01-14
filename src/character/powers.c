/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "powers.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Powers character structure
enum
{
  Powers_ArcMain_Idle0,
  Powers_ArcMain_Idle1,
  Powers_ArcMain_Idle2,
  Powers_ArcMain_Idle3,
  Powers_ArcMain_Idle4,
  Powers_ArcMain_Idle5,
  Powers_ArcMain_Idle6,
  Powers_ArcMain_Left0,
  Powers_ArcMain_Down0,
  Powers_ArcMain_Up0,
  Powers_ArcMain_Up1,
  Powers_ArcMain_Up2,
  Powers_ArcMain_Right0,
  Powers_ArcMain_Right1,
  Powers_ArcMain_Right2,
  Powers_ArcMain_Right3,
  Powers_ArcMain_LeftAlt0,
  Powers_ArcMain_DownAlt0,
  Powers_ArcMain_DownAlt1,
  Powers_ArcMain_UpAlt0,
  Powers_ArcMain_UpAlt1,
  Powers_ArcMain_UpAlt2,
  Powers_ArcMain_UpAlt3,
  Powers_ArcMain_RightAlt0,
	
	Powers_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Powers_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Powers;

//Powers character definitions
static const CharFrame char_powers_frame[] = {
  {Powers_ArcMain_Idle0, {  0,  0,161,142}, {182,138}}, //0 idle 1
  {Powers_ArcMain_Idle1, {  0,  0,164,141}, {184,136}}, //1 idle 2
  {Powers_ArcMain_Idle2, {  0,  0,161,142}, {183,136}}, //2 idle 3
  {Powers_ArcMain_Idle3, {  0,  0,160,142}, {181,138}}, //3 idle 4
  {Powers_ArcMain_Idle4, {  0,  0,159,140}, {181,140}}, //4 idle 5
  {Powers_ArcMain_Idle5, {  0,  0,159,140}, {180,140}}, //5 idle 6
  {Powers_ArcMain_Idle6, {  0,  0,159,140}, {180,140}}, //6 idle 7

  {Powers_ArcMain_Left0, {  0,  0,146,121}, {189,130}}, //7 left 1
  {Powers_ArcMain_Left0, {  0,122,144,122}, {187,132}}, //8 left 2

  {Powers_ArcMain_Down0, {  0,  0,140,125}, {172,128}}, //9 down 1
  {Powers_ArcMain_Down0, {  0,126,138,129}, {170,133}}, //10 down 2

  {Powers_ArcMain_Up0, {  0,  0,149,155}, {175,164}}, //11 up 1
  {Powers_ArcMain_Up1, {  0,  0,151,154}, {175,163}}, //12 up 2
  {Powers_ArcMain_Up2, {  0,  0,151,154}, {175,163}}, //13 up 3

  {Powers_ArcMain_Right0, {  0,  0,152,130}, {162,140}}, //14 right 1
  {Powers_ArcMain_Right1, {  0,  0,152,130}, {162,140}}, //15 right 2
  {Powers_ArcMain_Right2, {  0,  0,155,130}, {167,140}}, //16 right 3
  {Powers_ArcMain_Right3, {  0,  0,154,130}, {167,140}}, //17 right 4

  {Powers_ArcMain_LeftAlt0, {  0,  0, 87,156}, {147,129}}, //18 lefta 1
  {Powers_ArcMain_LeftAlt0, { 88,  0, 93,156}, {148,131}}, //19 lefta 2

  {Powers_ArcMain_DownAlt0, {  0,  0,137,138}, {172,128}}, //20 downa 1
  {Powers_ArcMain_DownAlt1, {  0,  0,135,143}, {170,133}}, //21 downa 2

  {Powers_ArcMain_UpAlt0, {  0,  0,145,155}, {162,164}}, //22 upa 1
  {Powers_ArcMain_UpAlt1, {  0,  0,142,154}, {158,163}}, //23 upa 2
  {Powers_ArcMain_UpAlt2, {  0,  0,140,154}, {158,163}}, //24 upa 3
  {Powers_ArcMain_UpAlt3, {  0,  0,139,154}, {158,163}}, //25 upa 4

  {Powers_ArcMain_RightAlt0, {  0,  0,127,147}, {137,140}}, //26 righta 1
  {Powers_ArcMain_RightAlt0, {  0,  0,129,149}, {137,140}}, //27 righta 2
};

static const Animation char_powers_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 7, 8, 8, 8, ASCR_BACK, 1}},         //CharAnim_Left
	{2, (const u8[]){ 18, 19, 19, 19, ASCR_BACK, 1}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 9, 10, 10, 10, 10, ASCR_BACK, 1}},         //CharAnim_Down
	{2, (const u8[]){ 20, 21, 21, 21, 21, ASCR_BACK, 1}},   //CharAnim_DownAlt
	{2, (const u8[]){ 11, 12, 13, 13, 13, ASCR_BACK, 1}},         //CharAnim_Up
	{2, (const u8[]){ 22, 23, 24, 25, 25, ASCR_BACK, 1}},   //CharAnim_UpAlt
	{1, (const u8[]){ 14, 15, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, ASCR_BACK, 1}},         //CharAnim_Right
	{2, (const u8[]){ 26, 27, 27, 27, 27, 27, 27, ASCR_BACK, 1}},   //CharAnim_RightAlt
};

//Powers character functions
void Char_Powers_SetFrame(void *user, u8 frame)
{
	Char_Powers *this = (Char_Powers*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_powers_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Powers_Tick(Character *character)
{
	Char_Powers *this = (Char_Powers*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Powers_SetFrame);
	Character_Draw(character, &this->tex, &char_powers_frame[this->frame]);
}

void Char_Powers_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Powers_Free(Character *character)
{
	Char_Powers *this = (Char_Powers*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Powers_New(fixed_t x, fixed_t y)
{
	//Allocate powers object
	Char_Powers *this = Mem_Alloc(sizeof(Char_Powers));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Powers_New] Failed to allocate powers object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Powers_Tick;
	this->character.set_anim = Char_Powers_SetAnim;
	this->character.free = Char_Powers_Free;
	
	Animatable_Init(&this->character.animatable, char_powers_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	this->character.idle2 = 0;
	
	this->character.health_i = 1;

	//health bar color
	this->character.health_bar = 0xFF89D7FF;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-115,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\POWERS.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "idle2.tim",
  "idle3.tim",
  "idle4.tim",
  "idle5.tim",
  "idle6.tim",
  "left0.tim",
  "down0.tim",
  "up0.tim",
  "up1.tim",
  "up2.tim",
  "right0.tim",
  "right1.tim",
  "right2.tim",
  "right3.tim",
  "lefta0.tim",
  "downa0.tim",
  "downa1.tim",
  "upa0.tim",
  "upa1.tim",
  "upa2.tim",
  "upa3.tim",
  "righta0.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
