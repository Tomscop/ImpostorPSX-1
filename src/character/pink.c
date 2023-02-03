/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "pink.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Pink character structure
enum
{
  Pink_ArcMain_Idle0,
  Pink_ArcMain_Idle1,
  Pink_ArcMain_Idle2,
  Pink_ArcMain_Idle3,
  Pink_ArcMain_Left0,
  Pink_ArcMain_Down0,
  Pink_ArcMain_Down1,
  Pink_ArcMain_Down2,
  Pink_ArcMain_Up0,
  Pink_ArcMain_Up1,
  Pink_ArcMain_Right0,
  Pink_ArcMain_Right1,
  Pink_ArcMain_Right2,
	
	Pink_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Pink_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Pink;

//Pink character definitions
static const CharFrame char_pink_frame[] = {
  {Pink_ArcMain_Idle0, {  0,  0,117,121}, {158,153}}, //0 idle 1
  {Pink_ArcMain_Idle0, {117,  0,121,125}, {161,157}}, //1 idle 2
  {Pink_ArcMain_Idle0, {  0,125,118,127}, {159,159}}, //2 idle 3
  {Pink_ArcMain_Idle0, {118,125,117,128}, {159,160}}, //3 idle 4
  {Pink_ArcMain_Idle1, {  0,  0,118,128}, {159,160}}, //4 idle 5
  {Pink_ArcMain_Idle1, {118,  0,117,128}, {159,160}}, //5 idle 6
  {Pink_ArcMain_Idle2, {  0,  0,117,128}, {159,160}}, //6 idle 7
  {Pink_ArcMain_Idle2, {117,  0,117,127}, {159,159}}, //7 idle 8
  {Pink_ArcMain_Idle3, {  0,  0,117,128}, {159,160}}, //8 idle 9

  {Pink_ArcMain_Left0, {  0,  0,120,126}, {171,160}}, //9 left 1
  {Pink_ArcMain_Left0, {120,  0,118,124}, {156,157}}, //10 left 2
  {Pink_ArcMain_Left0, {  0,126,120,124}, {156,157}}, //11 left 3
  {Pink_ArcMain_Left0, {120,126,121,125}, {157,157}}, //12 left 4

  {Pink_ArcMain_Down0, {  0,  0,166,114}, {191,147}}, //13 down 1
  {Pink_ArcMain_Down0, {  0,114,167,125}, {190,158}}, //14 down 2
  {Pink_ArcMain_Down1, {  0,  0,168,124}, {190,157}}, //15 down 3
  {Pink_ArcMain_Down1, {  0,124,168,122}, {190,155}}, //16 down 4
  {Pink_ArcMain_Down2, {  0,  0,168,122}, {190,155}}, //17 down 5

  {Pink_ArcMain_Up0, {  0,  0,103,119}, {155,152}}, //18 up 1
  {Pink_ArcMain_Up0, {103,  0,110,128}, {158,162}}, //19 up 2
  {Pink_ArcMain_Up1, {  0,  0,111,127}, {160,160}}, //20 up 3
  {Pink_ArcMain_Up1, {111,  0,110,126}, {159,159}}, //21 up 4
  {Pink_ArcMain_Up1, {  0,127,110,125}, {159,159}}, //22 up 5

  {Pink_ArcMain_Right0, {  0,  0,136,130}, {171,157}}, //23 right 1
  {Pink_ArcMain_Right1, {  0,  0,130,125}, {167,158}}, //24 right 2
  {Pink_ArcMain_Right1, {  0,125,129,123}, {167,156}}, //25 right 3
  {Pink_ArcMain_Right2, {  0,  0,128,123}, {167,155}}, //26 right 4
};

static const Animation char_pink_anim[CharAnim_Max] = {
	{1, (const u8[]){ 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 6, 7, 8, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 9, 10, 11, 12, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{1, (const u8[]){ 13, 13, 14, 14, 15, 15, 16, 16, 17, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{1, (const u8[]){ 18, 18, 19, 19, 20, 20, 21, 21, 22, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 23, 24, 25, 26, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Pink character functions
void Char_Pink_SetFrame(void *user, u8 frame)
{
	Char_Pink *this = (Char_Pink*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_pink_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Pink_Tick(Character *character)
{
	Char_Pink *this = (Char_Pink*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Pink_SetFrame);
	Character_Draw(character, &this->tex, &char_pink_frame[this->frame]);
}

void Char_Pink_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Pink_Free(Character *character)
{
	Char_Pink *this = (Char_Pink*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Pink_New(fixed_t x, fixed_t y)
{
	//Allocate pink object
	Char_Pink *this = Mem_Alloc(sizeof(Char_Pink));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Pink_New] Failed to allocate pink object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Pink_Tick;
	this->character.set_anim = Char_Pink_SetAnim;
	this->character.free = Char_Pink_Free;
	
	Animatable_Init(&this->character.animatable, char_pink_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;

	//health bar color
	this->character.health_bar = 0xFFEE64CC;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-115,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\PINK.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "idle2.tim",
  "idle3.tim",
  "left0.tim",
  "down0.tim",
  "down1.tim",
  "down2.tim",
  "up0.tim",
  "up1.tim",
  "right0.tim",
  "right1.tim",
  "right2.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
