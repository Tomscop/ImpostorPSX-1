/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "whitedk.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//White DK character structure
enum
{
  WhiteDK_ArcMain_Idle0,
  WhiteDK_ArcMain_Idle1,
  WhiteDK_ArcMain_Left0,
  WhiteDK_ArcMain_Left1,
  WhiteDK_ArcMain_Down0,
  WhiteDK_ArcMain_Down1,
  WhiteDK_ArcMain_Up0,
  WhiteDK_ArcMain_Up1,
  WhiteDK_ArcMain_Right0,
  WhiteDK_ArcMain_Right1,
	
	WhiteDK_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[WhiteDK_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_WhiteDK;

//White DK character definitions
static const CharFrame char_whitedk_frame[] = {
  {WhiteDK_ArcMain_Idle0, {  0,  0, 86,106}, {245,140}}, //0 idle 1
  {WhiteDK_ArcMain_Idle0, { 86,  0, 86,101}, {244,134}}, //1 idle 2
  {WhiteDK_ArcMain_Idle0, {  0,106, 87, 99}, {245,134}}, //2 idle 3
  {WhiteDK_ArcMain_Idle0, { 87,106, 85,101}, {245,137}}, //3 idle 4
  {WhiteDK_ArcMain_Idle1, {  0,  0, 84,102}, {245,138}}, //4 idle 5

  {WhiteDK_ArcMain_Left0, {  0,  0,141, 98}, {256,134}}, //5 left 1
  {WhiteDK_ArcMain_Left0, {  0, 98,133,101}, {253,137}}, //6 left 2
  {WhiteDK_ArcMain_Left1, {  0,  0,132,103}, {252,139}}, //7 left 3
  {WhiteDK_ArcMain_Left1, {  0,103,131,102}, {252,138}}, //8 left 4

  {WhiteDK_ArcMain_Down0, {  0,  0,147, 98}, {268,127}}, //9 down 1
  {WhiteDK_ArcMain_Down0, {  0, 98,142, 97}, {265,125}}, //10 down 2
  {WhiteDK_ArcMain_Down1, {  0,  0,141, 97}, {263,125}}, //11 down 3
  {WhiteDK_ArcMain_Down1, {  0, 97,141, 98}, {263,126}}, //12 down 4

  {WhiteDK_ArcMain_Up0, {  0,  0,137,109}, {259,147}}, //13 up 1
  {WhiteDK_ArcMain_Up0, {  0,109,143,112}, {264,150}}, //14 up 2
  {WhiteDK_ArcMain_Up1, {  0,  0,144,113}, {264,151}}, //15 up 3
  {WhiteDK_ArcMain_Up1, {  0,113,144,112}, {263,150}}, //16 up 4

  {WhiteDK_ArcMain_Right0, {  0,  0,139,107}, {246,144}}, //17 right 1
  {WhiteDK_ArcMain_Right0, {  0,107,138,102}, {251,139}}, //18 right 2
  {WhiteDK_ArcMain_Right1, {  0,  0,138,104}, {253,141}}, //19 right 3
  {WhiteDK_ArcMain_Right1, {  0,104,138,105}, {253,142}}, //20 right 4
};

static const Animation char_whitedk_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, ASCR_BACK, 1}}, //CharAnim_Idle
	{1, (const u8[]){ 5, 5, 6, 6, 7, 7, 8, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{1, (const u8[]){ 9, 9, 10, 10, 11, 11, 12, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{1, (const u8[]){ 13, 13, 14, 14, 15, 15, 16, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{1, (const u8[]){ 17, 17, 18, 18, 19, 19, 20, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//White DK character functions
void Char_WhiteDK_SetFrame(void *user, u8 frame)
{
	Char_WhiteDK *this = (Char_WhiteDK*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_whitedk_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_WhiteDK_Tick(Character *character)
{
	Char_WhiteDK *this = (Char_WhiteDK*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_WhiteDK_SetFrame);
	Character_Draw(character, &this->tex, &char_whitedk_frame[this->frame]);
}

void Char_WhiteDK_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_WhiteDK_Free(Character *character)
{
	Char_WhiteDK *this = (Char_WhiteDK*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_WhiteDK_New(fixed_t x, fixed_t y)
{
	//Allocate whitedk object
	Char_WhiteDK *this = Mem_Alloc(sizeof(Char_WhiteDK));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_WhiteDK_New] Failed to allocate whitedk object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_WhiteDK_Tick;
	this->character.set_anim = Char_WhiteDK_SetAnim;
	this->character.free = Char_WhiteDK_Free;
	
	Animatable_Init(&this->character.animatable, char_whitedk_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 2;

	//health bar color
	this->character.health_bar = 0xFFD1D2F8;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-115,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\OPPONENT\\WHITEDK.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
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
