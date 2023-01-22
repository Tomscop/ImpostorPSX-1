/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "black.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Black character structure
enum
{
  Black_ArcMain_Idle0,
  Black_ArcMain_Idle1,
  Black_ArcMain_Idle2,
  Black_ArcMain_Idle3,
  Black_ArcMain_Idle4,
  Black_ArcMain_Idle5,
  Black_ArcMain_Idle6,
  Black_ArcMain_Left0,
  Black_ArcMain_Left1,
  Black_ArcMain_Left2,
  Black_ArcMain_Down0,
  Black_ArcMain_Down1,
  Black_ArcMain_Down2,
  Black_ArcMain_Down3,
  Black_ArcMain_Down4,
  Black_ArcMain_Down5,
  Black_ArcMain_Up0,
  Black_ArcMain_Up1,
  Black_ArcMain_Up2,
  Black_ArcMain_Up3,
  Black_ArcMain_Up4,
  Black_ArcMain_Right0,
  Black_ArcMain_Right1,
  Black_ArcMain_Right2,
	
	Black_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Black_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Black;

//Black character definitions
static const CharFrame char_black_frame[] = {
  {Black_ArcMain_Idle0, {  0,  0,120,120}, {225,154}}, //0 idle 1
  {Black_ArcMain_Idle0, {120,  0,120,118}, {225,153}}, //1 idle 2
  {Black_ArcMain_Idle0, {  0,120,121,118}, {225,152}}, //2 idle 3
  {Black_ArcMain_Idle0, {121,120,121,116}, {225,151}}, //3 idle 4
  {Black_ArcMain_Idle1, {  0,  0,121,117}, {223,152}}, //4 idle 5
  {Black_ArcMain_Idle1, {121,  0,121,117}, {222,151}}, //5 idle 6
  {Black_ArcMain_Idle1, {  0,117,121,117}, {223,152}}, //6 idle 7
  {Black_ArcMain_Idle1, {121,117,121,117}, {222,152}}, //7 idle 8
  {Black_ArcMain_Idle2, {  0,  0,120,117}, {221,152}}, //8 idle 9
  {Black_ArcMain_Idle2, {120,  0,120,117}, {220,152}}, //9 idle 10
  {Black_ArcMain_Idle2, {  0,118,120,118}, {220,152}}, //10 idle 11
  {Black_ArcMain_Idle2, {120,118,120,119}, {220,153}}, //11 idle 12
  {Black_ArcMain_Idle3, {  0,  0,120,121}, {220,155}}, //12 idle 13
  {Black_ArcMain_Idle3, {120,  0,120,122}, {220,156}}, //13 idle 14
  {Black_ArcMain_Idle3, {  0,122,119,121}, {220,156}}, //14 idle 15
  {Black_ArcMain_Idle3, {119,122,119,123}, {221,157}}, //15 idle 16
  {Black_ArcMain_Idle4, {  0,  0,119,123}, {222,157}}, //16 idle 17
  {Black_ArcMain_Idle4, {119,  0,119,123}, {221,157}}, //17 idle 18
  {Black_ArcMain_Idle4, {  0,124,119,124}, {222,158}}, //18 idle 19
  {Black_ArcMain_Idle4, {119,124,120,124}, {223,158}}, //19 idle 20
  {Black_ArcMain_Idle5, {  0,  0,119,123}, {223,157}}, //20 idle 21
  {Black_ArcMain_Idle5, {119,  0,119,123}, {224,157}}, //21 idle 22
  {Black_ArcMain_Idle5, {  0,123,119,122}, {224,157}}, //22 idle 23
  {Black_ArcMain_Idle5, {119,123,119,122}, {224,156}}, //23 idle 24
  {Black_ArcMain_Idle6, {  0,  0,120,121}, {224,156}}, //24 idle 25

  {Black_ArcMain_Left0, {  0,  0,177,115}, {262,149}}, //25 left 1
  {Black_ArcMain_Left0, {  0,115,177,106}, {261,140}}, //26 left 2
  {Black_ArcMain_Left1, {  0,  0,177,105}, {260,139}}, //27 left 3
  {Black_ArcMain_Left1, {  0,105,173,104}, {260,138}}, //28 left 4
  {Black_ArcMain_Left2, {  0,  0,157,104}, {264,138}}, //29 left 5
  {Black_ArcMain_Left2, {  0,107,117,107}, {229,145}}, //30 left 6

  {Black_ArcMain_Down0, {  0,  0,167,155}, {253,188}}, //31 down 1
  {Black_ArcMain_Down1, {  0,  0,173,166}, {257,200}}, //32 down 2
  {Black_ArcMain_Down2, {  0,  0,172,166}, {256,194}}, //33 down 3
  {Black_ArcMain_Down3, {  0,  0,169,158}, {254,192}}, //34 down 4
  {Black_ArcMain_Down4, {  0,  0,147,148}, {243,182}}, //35 down 5
  {Black_ArcMain_Down4, {147,  0,102,130}, {222,164}}, //36 down 6
  {Black_ArcMain_Down5, {  0,  0, 90,117}, {210,149}}, //37 down 7

  {Black_ArcMain_Up0, {  0,  0,185,137}, {271,155}}, //38 up 1
  {Black_ArcMain_Up1, {  0,  0,190,140}, {271,156}}, //39 up 2
  {Black_ArcMain_Up2, {  0,  0,185,139}, {268,154}}, //40 up 3
  {Black_ArcMain_Up3, {  0,  0,168,133}, {269,151}}, //41 up 4
  {Black_ArcMain_Up4, {  0,  0,184,124}, {274,148}}, //42 up 5
  {Black_ArcMain_Up4, {  0,124,194,107}, {283,142}}, //43 up 6

  {Black_ArcMain_Right0, {  0,  0,207,122}, {222,150}}, //44 right 1
  {Black_ArcMain_Right0, {  0,122,208,118}, {223,147}}, //45 right 2
  {Black_ArcMain_Right1, {  0,  0,208,116}, {223,145}}, //46 right 3
  {Black_ArcMain_Right1, {  0,116,206,116}, {220,144}}, //47 right 4
  {Black_ArcMain_Right2, {  0,  0,173,120}, {200,142}}, //48 right 5
  {Black_ArcMain_Right2, {  0,121, 98,134}, {203,152}}, //49 right 6
};

static const Animation char_black_anim[CharAnim_Max] = {
	{1, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, ASCR_CHGANI, CharAnim_Idle}}, //CharAnim_Idle
	{1, (const u8[]){ 25, 26, 27, 28, 29, 30, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{1, (const u8[]){ 31, 32, 33, 34, 35, 36, 37, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{1, (const u8[]){ 38, 39, 40, 41, 42, 43, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{1, (const u8[]){ 44, 45, 46, 47, 48, 49, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Black character functions
void Char_Black_SetFrame(void *user, u8 frame)
{
	Char_Black *this = (Char_Black*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_black_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Black_Tick(Character *character)
{
	Char_Black *this = (Char_Black*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Black_SetFrame);
	Character_Draw(character, &this->tex, &char_black_frame[this->frame]);
}

void Char_Black_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Black_Free(Character *character)
{
	Char_Black *this = (Char_Black*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Black_New(fixed_t x, fixed_t y)
{
	//Allocate black object
	Char_Black *this = Mem_Alloc(sizeof(Char_Black));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Black_New] Failed to allocate black object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Black_Tick;
	this->character.set_anim = Char_Black_SetAnim;
	this->character.free = Char_Black_Free;
	
	Animatable_Init(&this->character.animatable, char_black_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	this->character.idle2 = 0;
	
	this->character.health_i = 1;

	//health bar color
	this->character.health_bar = 0xFFD70009;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-115,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.size = FIXED_DEC(14,10);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\BLACK.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "idle2.tim",
  "idle3.tim",
  "idle4.tim",
  "idle5.tim",
  "idle6.tim",
  "left0.tim",
  "left1.tim",
  "left2.tim",
  "down0.tim",
  "down1.tim",
  "down2.tim",
  "down3.tim",
  "down4.tim",
  "down5.tim",
  "up0.tim",
  "up1.tim",
  "up2.tim",
  "up3.tim",
  "up4.tim",
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
