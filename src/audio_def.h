#define XA_LENGTH(x) (((u64)(x) * 75) / 100 * IO_SECT_SIZE) //Centiseconds to sectors in bytes (w)

typedef struct
{
	XA_File file;
	u32 length;
} XA_TrackDef;

static const XA_TrackDef xa_tracks[] = {
	//MENU.XA
	{XA_Menu, XA_LENGTH(4705)}, //XA_GettinFreaky
	{XA_Menu, XA_LENGTH(7680)},  //XA_GameOver
	//MENU.XA
	{XA_GameOvr, XA_LENGTH(7680)}, //XA_GameOverJ
	{XA_GameOvr, XA_LENGTH(2461)},  //XA_GameOverP
	{XA_GameOvr, XA_LENGTH(3657)},  //XA_GameOverH
	//1.XA
	{XA_1, XA_LENGTH(18560)}, //XA_SussusMoogus
	{XA_1, XA_LENGTH(14682)}, //XA_Sabotage
	//2.XA
	{XA_2, XA_LENGTH(15000)}, //XA_Meltdown
	{XA_2, XA_LENGTH(18560)}, //XA_SussusToogus
	//3.XA
	{XA_3, XA_LENGTH(14822)}, //XA_LightsDown
	{XA_3, XA_LENGTH(18048)}, //XA_Reactor
	//4.XA
	{XA_4, XA_LENGTH(19279)}, //XA_Ejected
	{XA_4, XA_LENGTH(11169)}, //XA_Mando
	//5.XA
	{XA_5, XA_LENGTH(12988)},  //XA_Dlow
	{XA_5, XA_LENGTH(11358)}, //XA_Oversight
	//6.XA
	{XA_6, XA_LENGTH(14267)}, //XA_Danger
	{XA_6, XA_LENGTH(28928)}, //XA_DoubleKill
	//7.XA
	{XA_7, XA_LENGTH(15118)},  //XA_Defeat
	{XA_7, XA_LENGTH(15600)}, //XA_Finale
	//8.XA
	{XA_8, XA_LENGTH(29758)}, //XA_IdentityCrisis
	//9.XA
	{XA_9, XA_LENGTH(14442)}, //XA_Ashes
	{XA_9, XA_LENGTH(11430)},  //XA_Magmatic
	//10.XA
	{XA_10, XA_LENGTH(17471)}, //XA_BoilingPoint
	{XA_10, XA_LENGTH(11572)}, //XA_Delusion
	//11.XA
	{XA_11, XA_LENGTH(17500)}, //XA_Blackout
	{XA_11, XA_LENGTH(14914)}, //XA_Neurotic
	//12.XA
	{XA_12, XA_LENGTH(14344)}, //XA_Heartbeat
	{XA_12, XA_LENGTH(18032)}, //XA_Pinkwave
	//13.XA
	{XA_13, XA_LENGTH(15168)}, //XA_Pretender
	{XA_13, XA_LENGTH(16426)}, //XA_SaucesMoogus
	//14.XA
	{XA_14, XA_LENGTH(8622)}, //XA_O2
	{XA_14, XA_LENGTH(15044)}, //XA_VotingTime
	//15.XA
	{XA_15, XA_LENGTH(13353)}, //XA_Turbulence
	{XA_15, XA_LENGTH(17746)}, //XA_Victory
	//16.XA
	{XA_16, XA_LENGTH(10666)}, //XA_Roomcode
	//17.XA
	{XA_17, XA_LENGTH(12639)}, //XA_SussyBussy
	{XA_17, XA_LENGTH(11610)}, //XA_Rivals
	//18.XA
	{XA_18, XA_LENGTH(9598)}, //XA_Chewmate
	{XA_18, XA_LENGTH(9660)}, //XA_TomongusTuesday
	//19.XA
	{XA_19, XA_LENGTH(8657)}, //XA_Christmas
	{XA_19, XA_LENGTH(13521)}, //XA_Spookpostor
	//20.XA
	{XA_20, XA_LENGTH(10472)}, //XA_Titular
	{XA_20, XA_LENGTH(11000)}, //XA_GreatestPlan
	//21.XA
	{XA_21, XA_LENGTH(12819)}, //XA_Reinforcements
	{XA_21, XA_LENGTH(17092)}, //XA_Armed
	//22.XA
	{XA_22, XA_LENGTH(18723)}, //XA_AlphaMoogus
	{XA_22, XA_LENGTH(12777)}, //XA_ActinSus
	//23.XA
	{XA_23, XA_LENGTH(7885)}, //XA_Ow
	{XA_23, XA_LENGTH(11810)}, //XA_Who
	//24.XA
	{XA_24, XA_LENGTH(11311)}, //XA_InsaneStreamer
	{XA_24, XA_LENGTH(16600)}, //XA_SussusNuzzus
	//25.XA
	{XA_25, XA_LENGTH(10209)}, //XA_Idk
	{XA_25, XA_LENGTH(22378)}, //XA_Esculent
	//26.XA
	{XA_26, XA_LENGTH(16294)}, //XA_Drippypop
	{XA_26, XA_LENGTH(16324)}, //XA_Crewicide
	//27.XA
	{XA_27, XA_LENGTH(16541)}, //XA_MonotoneAttack
	{XA_27, XA_LENGTH(10338)}, //XA_Top10
	//28.XA
	{XA_28, XA_LENGTH(7885)}, //XA_Chippin
	{XA_28, XA_LENGTH(7909)}, //XA_Chipping
	//29.XA
	{XA_29, XA_LENGTH(12254)}, //XA_Torture
	//30.XA
	{XA_30, XA_LENGTH(9965)}, //XA_LemonLime
	{XA_30, XA_LENGTH(13714)}, //XA_Chlorophyll
	//31.XA
	{XA_31, XA_LENGTH(12745)}, //XA_Inflorescence
	{XA_31, XA_LENGTH(14000)}, //XA_Stargazer
};

static const char *xa_paths[] = {
	"\\MUSIC\\MENU.XA;1",   //XA_Menu
	"\\MUSIC\\GAMEOVR.XA;1",   //XA_GameOvr
	"\\MUSIC\\1.XA;1", //XA_1
	"\\MUSIC\\2.XA;1", //XA_2
	"\\MUSIC\\3.XA;1", //XA_3
	"\\MUSIC\\4.XA;1", //XA_4
	"\\MUSIC\\5.XA;1", //XA_5
	"\\MUSIC\\6.XA;1", //XA_6
	"\\MUSIC\\7.XA;1", //XA_7
	"\\MUSIC\\8.XA;1", //XA_8
	"\\MUSIC\\9.XA;1", //XA_9
	"\\MUSIC\\10.XA;1", //XA_10
	"\\MUSIC\\11.XA;1", //XA_11
	"\\MUSIC\\12.XA;1", //XA_12
	"\\MUSIC\\13.XA;1", //XA_13
	"\\MUSIC\\14.XA;1", //XA_14
	"\\MUSIC\\15.XA;1", //XA_15
	"\\MUSIC\\16.XA;1", //XA_16
	"\\MUSIC\\17.XA;1", //XA_17
	"\\MUSIC\\18.XA;1", //XA_18
	"\\MUSIC\\19.XA;1", //XA_19
	"\\MUSIC\\20.XA;1", //XA_20
	"\\MUSIC\\21.XA;1", //XA_21
	"\\MUSIC\\22.XA;1", //XA_22
	"\\MUSIC\\23.XA;1", //XA_23
	"\\MUSIC\\24.XA;1", //XA_24
	"\\MUSIC\\25.XA;1", //XA_25
	"\\MUSIC\\26.XA;1", //XA_26
	"\\MUSIC\\27.XA;1", //XA_27
	"\\MUSIC\\28.XA;1", //XA_28
	"\\MUSIC\\29.XA;1", //XA_29
	"\\MUSIC\\30.XA;1", //XA_30
	"\\MUSIC\\31.XA;1", //XA_31
	NULL,
};

typedef struct
{
	const char *name;
	boolean vocal;
} XA_Mp3;

static const XA_Mp3 xa_mp3s[] = {
	//MENU.XA
	{"freaky", false},   //XA_GettinFreaky
	{"gameover", false}, //XA_GameOver
	//GAMEOVR.XA
	{"gameoverj", false},   //XA_GameOverJ
	{"gameoverp", false}, //XA_GameOverP
	{"gameoverh", false}, //XA_GameOverH
	//1.XA
	{"sussusmoogus", true}, //XA_SussusMoogus
	{"sabotage", true},   //XA_Sabotage
	//2.XA
	{"meltdown", true}, //XA_Meltdown
	{"sussustoogus", true}, //XA_SussusToogus
	//3.XA
	{"lightsdown", true}, //XA_LightsDown
	{"reactor", true},    //XA_Reactor
	//4.XA
	{"ejected", true}, //XA_Ejected
	{"mando", true}, //XA_Mando
	//5.XA
	{"dlow", true},   //XA_Dlow
	{"oversight", true}, //XA_Oversight
	//6.XA
	{"danger", true}, //XA_Danger
	{"doublekill", true}, //XA_DoubleKill
	//7.XA
	{"defeat", true}, //XA_Defeat
	{"finale", true},         //XA_Finale
	//8.XA
	{"identitycrisis", true}, //XA_IdentityCrisis
	//9.XA
	{"ashes", true},  //XA_Ashes
	{"magmatic", true}, //XA_Magmatic
	//10.XA
	{"boilingpoint", true}, //XA_BoilingPoint
	{"delusion", true}, //XA_Delusion
	//11.XA
	{"blackout", true}, //XA_Blackout
	{"neurotic", true},  //XA_Neurotic
	//12.XA
	{"heartbeat", true}, //XA_Heartbeat
	{"pinkwave", true}, //XA_Pinkwave
	//13.XA
	{"pretender", true}, //XA_Pretender
	{"saucesmoogus", true}, //XA_SaucesMoogus
	//14.XA
	{"o2", true}, //XA_O2
	{"votingtime", true}, //XA_VotingTime
	//15.XA
	{"turbulence", true}, //XA_Turbulence
	{"victory", true}, //XA_Victory
	//16.XA
	{"roomcode", true}, //XA_Roomcode
	//17.XA
	{"sussybussy", true}, //XA_SussyBussy
	{"rivals", true}, //XA_Rivals
	//18.XA
	{"chewmate", true}, //XA_Chewmate
	{"tomongustuesday", true}, //XA_TomongusTuesday
	//19.XA
	{"christmas", true}, //XA_Christmas
	{"spookpostor", true}, //XA_Spookpostor
	//20.XA
	{"titular", true}, //XA_Titular
	{"greatestplan", true}, //XA_GreatestPlan
	//21.XA
	{"reinforcements", true}, //XA_Reinforcements
	{"armed", true}, //XA_Armed
	//22.XA
	{"alphamoogus", true}, //XA_AlphaMoogus
	{"actinsus", true}, //XA_ActinSus
	//23.XA
	{"ow", true}, //XA_Ow
	{"who", true}, //XA_Who
	//24.XA
	{"insanestreamer", true}, //XA_InsaneStreamer
	{"sussusnuzzus", true}, //XA_SussusNuzzus
	//25.XA
	{"idk", true}, //XA_Idk
	{"esculent", true}, //XA_Esculent
	//26.XA
	{"drippypop", true}, //XA_Drippypop
	{"crewicide", true}, //XA_Crewicide
	//27.XA
	{"monotoneattack", true}, //XA_MonotoneAttack
	{"top10", false}, //XA_Top10
	//28.XA
	{"chippin", true}, //XA_Chippin
	{"chipping", true}, //XA_Chipping
	//29.XA
	{"torture", true}, //XA_Torture
	//30.XA
	{"lemonlime", true}, //XA_LemonLime
	{"chlorophyll", true}, //XA_Chlorophyll
	//31.XA
	{"inflorescence", true}, //XA_Inflorescence
	{"stargazer", true}, //XA_Stargazer
	
	{NULL, false}
};
