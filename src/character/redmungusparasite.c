/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "redmungusparasite.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Redmungus Parasite character structure
enum
{
  RedmungusParasite_ArcMain_Idle0,
  RedmungusParasite_ArcMain_Idle1,
  RedmungusParasite_ArcMain_Idle2,
  RedmungusParasite_ArcMain_Idle3,
  RedmungusParasite_ArcMain_Idle4,
  RedmungusParasite_ArcMain_Idle5,
  RedmungusParasite_ArcMain_Idle6,
  RedmungusParasite_ArcMain_Left0,
  RedmungusParasite_ArcMain_Left1,
  RedmungusParasite_ArcMain_Left2,
  RedmungusParasite_ArcMain_Down0,
  RedmungusParasite_ArcMain_Down1,
  RedmungusParasite_ArcMain_Down2,
  RedmungusParasite_ArcMain_Down3,
  RedmungusParasite_ArcMain_Up0,
  RedmungusParasite_ArcMain_Up1,
  RedmungusParasite_ArcMain_Up2,
  RedmungusParasite_ArcMain_Right0,
  RedmungusParasite_ArcMain_Right1,
  RedmungusParasite_ArcMain_Right2,
  RedmungusParasite_ArcMain_Right3,
	
	RedmungusParasite_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[RedmungusParasite_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_RedmungusParasite;

//Redmungus Parasite character definitions
static const CharFrame char_redmungusparasite_frame[] = {
  {RedmungusParasite_ArcMain_Idle0, {  0,  0,217,162}, {144,118}}, //0 idle 1
  {RedmungusParasite_ArcMain_Idle1, {  0,  0,227,142}, {170, 99}}, //1 idle 2
  {RedmungusParasite_ArcMain_Idle2, {  0,  0,209,141}, {162,109}}, //2 idle 3
  {RedmungusParasite_ArcMain_Idle3, {  0,  0,204,140}, {161,110}}, //3 idle 4
  {RedmungusParasite_ArcMain_Idle4, {  0,  0,207,141}, {162,113}}, //4 idle 5
  {RedmungusParasite_ArcMain_Idle5, {  0,  0,219,141}, {166,114}}, //5 idle 6
  {RedmungusParasite_ArcMain_Idle6, {  0,  0,212,151}, {143,108}}, //6 idle 7

  {RedmungusParasite_ArcMain_Left0, {  0,  0,200,139}, {152,103}}, //7 left 1
  {RedmungusParasite_ArcMain_Left1, {  0,  0,217,140}, {161, 98}}, //8 left 2
  {RedmungusParasite_ArcMain_Left2, {  0,  0,205,148}, {150,107}}, //9 left 3

  {RedmungusParasite_ArcMain_Down0, {  0,  0,203,131}, {161, 82}}, //10 down 1
  {RedmungusParasite_ArcMain_Down1, {  0,  0,200,132}, {159, 91}}, //11 down 2
  {RedmungusParasite_ArcMain_Down2, {  0,  0,201,129}, {159, 92}}, //12 down 3
  {RedmungusParasite_ArcMain_Down3, {  0,  0,197,131}, {157, 91}}, //13 down 4

  {RedmungusParasite_ArcMain_Up0, {  0,  0,199,148}, {178,126}}, //14 up 1
  {RedmungusParasite_ArcMain_Up1, {  0,  0,227,126}, {187,108}}, //15 up 2
  {RedmungusParasite_ArcMain_Up1, {  0,  0,227,126}, {187,108}}, //16 up 3
  {RedmungusParasite_ArcMain_Up2, {  0,  0,227,140}, {186,107}}, //17 up 4
  
  {RedmungusParasite_ArcMain_Right0, {  0,  0,179,137}, {127, 91}}, //18 right 1
  {RedmungusParasite_ArcMain_Right1, {  0,  0,182,140}, {137, 93}}, //19 right 2
  {RedmungusParasite_ArcMain_Right2, {  0,  0,181,141}, {137, 94}}, //20 right 3
  {RedmungusParasite_ArcMain_Right3, {  0,  0,184,138}, {136, 94}}, //21 right 4
};

static const Animation char_redmungusparasite_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, ASCR_CHGANI, CharAnim_Idle}}, //CharAnim_Idle
	{2, (const u8[]){ 7, 8, 9, 9, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 10, 11, 12, 13, 13, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 14, 15, 16, 17, 17, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 18, 19, 20, 21, 21, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Redmungus Parasite character functions
void Char_RedmungusParasite_SetFrame(void *user, u8 frame)
{
	Char_RedmungusParasite *this = (Char_RedmungusParasite*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_redmungusparasite_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_RedmungusParasite_Tick(Character *character)
{
	Char_RedmungusParasite *this = (Char_RedmungusParasite*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_RedmungusParasite_SetFrame);
	if (stage.song_step <= 1216)
		Character_Draw(character, &this->tex, &char_redmungusparasite_frame[this->frame]);
	
	//DIE
	if ((stage.song_step >= 1209) && (stage.song_step <= 1216) && (stage.paused == false))
	{
		stage.opponent->x += FIXED_DEC(50,1);
	}
}

void Char_RedmungusParasite_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_RedmungusParasite_Free(Character *character)
{
	Char_RedmungusParasite *this = (Char_RedmungusParasite*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_RedmungusParasite_New(fixed_t x, fixed_t y)
{
	//Allocate redmungusparasite object
	Char_RedmungusParasite *this = Mem_Alloc(sizeof(Char_RedmungusParasite));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_RedmungusParasite_New] Failed to allocate redmungusparasite object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_RedmungusParasite_Tick;
	this->character.set_anim = Char_RedmungusParasite_SetAnim;
	this->character.free = Char_RedmungusParasite_Free;
	
	Animatable_Init(&this->character.animatable, char_redmungusparasite_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;

	//health bar color
	this->character.health_bar = 0xFFE51919;
	
	this->character.focus_x = FIXED_DEC(-73,1);
	this->character.focus_y = FIXED_DEC(-60,1);
	this->character.focus_zoom = FIXED_DEC(700,1024);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(1667,1000);
	
	//Load art
	this->arc_main = IO_Read("\\OPPONENT\\REDMUNGP.ARC;1");
	
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
  "up0.tim",
  "up1.tim",
  "up2.tim",
  "right0.tim",
  "right1.tim",
  "right2.tim",
  "right3.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
