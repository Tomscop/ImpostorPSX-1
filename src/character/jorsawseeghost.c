/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "jorsawseeghost.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Jorsawsee Ghost character structure
enum
{
  JorsawseeGhost_ArcMain_Idle0,
  JorsawseeGhost_ArcMain_Idle1,
  JorsawseeGhost_ArcMain_Left0,
  JorsawseeGhost_ArcMain_Left1,
  JorsawseeGhost_ArcMain_Down0,
  JorsawseeGhost_ArcMain_Down1,
  JorsawseeGhost_ArcMain_Up0,
  JorsawseeGhost_ArcMain_Up1,
  JorsawseeGhost_ArcMain_Right0,
  JorsawseeGhost_ArcMain_Right1,
	
	JorsawseeGhost_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[JorsawseeGhost_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_JorsawseeGhost;

//Jorsawsee Ghost character definitions
static const CharFrame char_jorsawseeghost_frame[] = {
  {JorsawseeGhost_ArcMain_Idle0, {  0,  0, 85, 99}, {174, 95}}, //0 idle 1
  {JorsawseeGhost_ArcMain_Idle0, { 85,  0, 83, 99}, {171, 95}}, //1 idle 2
  {JorsawseeGhost_ArcMain_Idle0, {168,  0, 84, 99}, {175, 96}}, //2 idle 3
  {JorsawseeGhost_ArcMain_Idle0, {  0, 99, 84, 99}, {175, 96}}, //3 idle 4
  {JorsawseeGhost_ArcMain_Idle0, { 84, 99, 82, 99}, {173, 97}}, //4 idle 5
  {JorsawseeGhost_ArcMain_Idle0, {166, 99, 83, 99}, {173, 97}}, //5 idle 6
  {JorsawseeGhost_ArcMain_Idle1, {  0,  0, 84, 98}, {174, 97}}, //6 idle 7
  {JorsawseeGhost_ArcMain_Idle1, { 84,  0, 82, 99}, {172, 97}}, //7 idle 8
  {JorsawseeGhost_ArcMain_Idle1, {166,  0, 83, 99}, {173, 96}}, //8 idle 9
  {JorsawseeGhost_ArcMain_Idle1, {  0, 99, 84, 98}, {174, 96}}, //9 idle 10

  {JorsawseeGhost_ArcMain_Left0, {  0,  0, 75, 97}, {170, 95}}, //10 left 1
  {JorsawseeGhost_ArcMain_Left0, { 75,  0, 76, 98}, {167, 96}}, //11 left 2
  {JorsawseeGhost_ArcMain_Left0, {151,  0, 76, 97}, {169, 96}}, //12 left 3
  {JorsawseeGhost_ArcMain_Left0, {  0, 98, 76, 97}, {169, 95}}, //13 left 4
  {JorsawseeGhost_ArcMain_Left0, { 76, 98, 76, 97}, {169, 95}}, //14 left 5
  {JorsawseeGhost_ArcMain_Left0, {152, 98, 76, 97}, {168, 95}}, //15 left 6
  {JorsawseeGhost_ArcMain_Left1, {  0,  0, 77, 97}, {169, 95}}, //16 left 7
  {JorsawseeGhost_ArcMain_Left1, { 77,  0, 76, 97}, {169, 95}}, //17 left 8

  {JorsawseeGhost_ArcMain_Down0, {  0,  0, 85, 89}, {165, 87}}, //18 down 1
  {JorsawseeGhost_ArcMain_Down0, { 85,  0, 83, 90}, {164, 89}}, //19 down 2
  {JorsawseeGhost_ArcMain_Down0, {168,  0, 85, 91}, {166, 89}}, //20 down 3
  {JorsawseeGhost_ArcMain_Down0, {  0, 91, 84, 90}, {165, 90}}, //21 down 4
  {JorsawseeGhost_ArcMain_Down0, { 84, 91, 85, 91}, {166, 90}}, //22 down 5
  {JorsawseeGhost_ArcMain_Down0, {169, 91, 84, 90}, {164, 90}}, //23 down 6
  {JorsawseeGhost_ArcMain_Down1, {  0,  0, 85, 91}, {166, 90}}, //24 down 7
  {JorsawseeGhost_ArcMain_Down1, { 85,  0, 84, 90}, {165, 90}}, //25 down 8

  {JorsawseeGhost_ArcMain_Up0, {  0,  0, 71,109}, {164,111}}, //26 up 1
  {JorsawseeGhost_ArcMain_Up0, { 71,  0, 72,109}, {164,109}}, //27 up 2
  {JorsawseeGhost_ArcMain_Up0, {143,  0, 73,109}, {165,109}}, //28 up 3
  {JorsawseeGhost_ArcMain_Up0, {  0,109, 72,109}, {164,109}}, //29 up 4
  {JorsawseeGhost_ArcMain_Up0, { 72,109, 72,109}, {164,109}}, //30 up 5
  {JorsawseeGhost_ArcMain_Up0, {144,109, 71,109}, {163,109}}, //31 up 6
  {JorsawseeGhost_ArcMain_Up1, {  0,  0, 72,109}, {164,109}}, //32 up 7

  {JorsawseeGhost_ArcMain_Right0, {  0,  0,108, 92}, {166, 89}}, //33 right 1
  {JorsawseeGhost_ArcMain_Right0, {108,  0,107, 92}, {170, 89}}, //34 right 2
  {JorsawseeGhost_ArcMain_Right0, {  0, 92,107, 92}, {169, 89}}, //35 right 3
  {JorsawseeGhost_ArcMain_Right0, {107, 92,108, 92}, {169, 89}}, //36 right 4
  {JorsawseeGhost_ArcMain_Right1, {  0,  0,106, 92}, {168, 89}}, //37 right 5
  {JorsawseeGhost_ArcMain_Right1, {106,  0,108, 91}, {169, 89}}, //38 right 6
  {JorsawseeGhost_ArcMain_Right1, {  0, 92,107, 91}, {169, 89}}, //39 right 7
};

static const Animation char_jorsawseeghost_anim[CharAnim_Max] = {
	{1, (const u8[]){ 0, 1, 2, 3, 4, 5, 5, 6, 6, 7, 7, 5, 8, 9, ASCR_BACK, 1}}, //CharAnim_Idle
	{1, (const u8[]){ 10, 10, 11, 11, 12, 13, 14, 15, 16, 12, 17, 14, 13, 16, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{1, (const u8[]){ 18, 18, 19, 20, 21, 21, 22, 22, 23, 23, 24, 24, 25, 25, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 26, 27, 28, 29, 30, 31, 32, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 33, 34, 35, 36, 37, 38, 39, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Jorsawsee Ghost character functions
void Char_JorsawseeGhost_SetFrame(void *user, u8 frame)
{
	Char_JorsawseeGhost *this = (Char_JorsawseeGhost*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_jorsawseeghost_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_JorsawseeGhost_Tick(Character *character)
{
	Char_JorsawseeGhost *this = (Char_JorsawseeGhost*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_JorsawseeGhost_SetFrame);
	if (stage.camswitch == 3)
		Character_DrawCol(character, &this->tex, &char_jorsawseeghost_frame[this->frame], 175, 175, 175);
}

void Char_JorsawseeGhost_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_JorsawseeGhost_Free(Character *character)
{
	Char_JorsawseeGhost *this = (Char_JorsawseeGhost*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_JorsawseeGhost_New(fixed_t x, fixed_t y)
{
	//Allocate jorsawseeghost object
	Char_JorsawseeGhost *this = Mem_Alloc(sizeof(Char_JorsawseeGhost));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_JorsawseeGhost_New] Failed to allocate jorsawseeghost object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_JorsawseeGhost_Tick;
	this->character.set_anim = Char_JorsawseeGhost_SetAnim;
	this->character.free = Char_JorsawseeGhost_Free;
	
	Animatable_Init(&this->character.animatable, char_jorsawseeghost_anim);
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
	this->arc_main = IO_Read("\\OPPONENT\\JORSAWSG.ARC;1");
	
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
