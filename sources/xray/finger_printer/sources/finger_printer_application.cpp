#include "pch.h"
#include "finger_printer_application.h"
#include "finger_printer_memory.h"

#include "xray/buffer_string.h"
#include "xray/engine/api.h"
#include <xray/core/sources/fs_file_system.h>

#include <xray/core/core.h>
#include <xray/fs_utils.h>
#include <xray/intrusive_list.h>

#include <stdio.h>

#include <xray/os_preinclude.h>
#undef NOUSER
#undef NOMSG
#undef NOWINMESSAGES
#undef NOCTLMGR
#include <xray/os_include.h>
#include <xray/compressor_ppmd.h>
#include <xray/memory_buffer.h>
#include <xray/fs_utils.h>
#include <stdlib.h>
#include <stdio.h>
#include <xray/math_randoms_generator.h>
#include <xray/math_randoms_table.h>
#include <xray/core/simple_engine.h>

static u32 const s_finger_print_max_length	=	32;

xray::command_line::key						s_file_key("file", "", "", "specifies a path to exe/dll file(s)(separate with ';')");
xray::command_line::key						s_read_key("read", "", "", "outputs finger-print from executable");
xray::command_line::key						s_write_key("write", "", "", "writes a finger-print to executable", "finger_print_message");
xray::command_line::key						s_build_id_key("build_id", "", "", "uses given build id rather then one in binary", "id");
xray::command_line::key						s_big_endian("big_endian", "", "", "treats binaries in big endian format", "id");
xray::command_line::key						s_working_directory("working_directory", "", "", "for binaries to run when getting build id", "id");
xray::command_line::key						s_force("force", "", "", "forces rewrite finger-print", "");


using namespace xray;
using namespace xray::resources;
using namespace xray::fs;

xray::memory::doug_lea_allocator_type	g_allocator;
static 	xray::core::simple_engine		core_engine;

typedef char const *	pcstr;
typedef char *			pstr;

static float s_sinuses[360] = {
0.0000000000f, 0.0174524058f, 0.0348994955f, 0.0523359589f,
0.0697564706f, 0.0871557444f, 0.1045284644f, 0.1218693480f,
0.1391731054f, 0.1564344764f, 0.1736481935f, 0.1908089966f,
0.2079117000f, 0.2249510586f, 0.2419219017f, 0.2588190436f,
0.2756373584f, 0.2923717201f, 0.3090170026f, 0.3255681396f,
0.3420201540f, 0.3583679497f, 0.3746065795f, 0.3907311559f,
0.4067366421f, 0.4226182699f, 0.4383711815f, 0.4539905190f,
0.4694715738f, 0.4848096073f, 0.5000000000f, 0.5150381327f,
0.5299192667f, 0.5446390510f, 0.5591929555f, 0.5735764503f,
0.5877852440f, 0.6018150449f, 0.6156615019f, 0.6293203831f,
0.6427876353f, 0.6560590267f, 0.6691306233f, 0.6819983721f,
0.6946583986f, 0.7071067691f, 0.7193398476f, 0.7313537002f,
0.7431448698f, 0.7547096014f, 0.7660444379f, 0.7771459818f,
0.7880107760f, 0.7986355424f, 0.8090170026f, 0.8191520572f,
0.8290375471f, 0.8386706114f, 0.8480480909f, 0.8571673036f,
0.8660254478f, 0.8746197224f, 0.8829476237f, 0.8910065293f,
0.8987940550f, 0.9063078165f, 0.9135454893f, 0.9205048680f,
0.9271838665f, 0.9335804582f, 0.9396926165f, 0.9455185533f,
0.9510565400f, 0.9563047886f, 0.9612616897f, 0.9659258127f,
0.9702957273f, 0.9743700624f, 0.9781476259f, 0.9816271663f,
0.9848077893f, 0.9876883626f, 0.9902680516f, 0.9925461411f,
0.9945219159f, 0.9961947203f, 0.9975640774f, 0.9986295104f,
0.9993908405f, 0.9998477101f, 1.0000000000f, 0.9998477101f,
0.9993908405f, 0.9986295104f, 0.9975640774f, 0.9961947203f,
0.9945219159f, 0.9925461411f, 0.9902680516f, 0.9876883626f,
0.9848077297f, 0.9816271663f, 0.9781475663f, 0.9743700624f,
0.9702957273f, 0.9659258127f, 0.9612616897f, 0.9563047290f,
0.9510564804f, 0.9455185533f, 0.9396926165f, 0.9335804582f,
0.9271838665f, 0.9205048680f, 0.9135454297f, 0.9063077569f,
0.8987940550f, 0.8910064697f, 0.8829475641f, 0.8746196032f,
0.8660253882f, 0.8571673036f, 0.8480480313f, 0.8386705518f,
0.8290374875f, 0.8191519976f, 0.8090170026f, 0.7986354828f,
0.7880107760f, 0.7771458626f, 0.7660444379f, 0.7547094822f,
0.7431448102f, 0.7313537002f, 0.7193397284f, 0.7071067691f,
0.6946582794f, 0.6819983125f, 0.6691304445f, 0.6560589671f,
0.6427876353f, 0.6293202639f, 0.6156614423f, 0.6018148661f,
0.5877851844f, 0.5735764503f, 0.5591928363f, 0.5446390510f,
0.5299191475f, 0.5150380135f, 0.5000000596f, 0.4848095477f,
0.4694715738f, 0.4539903700f, 0.4383711219f, 0.4226180911f,
0.4067365825f, 0.3907311559f, 0.3746064901f, 0.3583679199f,
0.3420200050f, 0.3255681098f, 0.3090167940f, 0.2923716009f,
0.2756373584f, 0.2588189244f, 0.2419218570f, 0.2249508798f,
0.2079116106f, 0.1908090115f, 0.1736480594f, 0.1564344466f,
0.1391729414f, 0.1218692809f, 0.1045284942f, 0.0871556401f,
0.0697564706f, 0.0523358099f, 0.0348994508f, 0.0174522195f,
-0.0000000874f, -0.0174523946f, -0.0348996259f, -0.0523359850f,
-0.0697566420f, -0.0871558115f, -0.1045286730f, -0.1218694523f,
-0.1391731054f, -0.1564346105f, -0.1736482233f, -0.1908091903f,
-0.2079117894f, -0.2249510437f, -0.2419220209f, -0.2588190734f,
-0.2756375372f, -0.2923717797f, -0.3090169728f, -0.3255682588f,
-0.3420201540f, -0.3583680987f, -0.3746066391f, -0.3907313049f,
-0.4067367315f, -0.4226182699f, -0.4383712709f, -0.4539905488f,
-0.4694717228f, -0.4848096967f, -0.5000001788f, -0.5150381923f,
-0.5299192667f, -0.5446391702f, -0.5591929555f, -0.5735766292f,
-0.5877853632f, -0.6018150449f, -0.6156615615f, -0.6293204427f,
-0.6427877545f, -0.6560590863f, -0.6691306233f, -0.6819984317f,
-0.6946583986f, -0.7071068883f, -0.7193398476f, -0.7313538194f,
-0.7431448698f, -0.7547096014f, -0.7660445571f, -0.7771459818f,
-0.7880107164f, -0.7986357212f, -0.8090171218f, -0.8191521168f,
-0.8290376067f, -0.8386705518f, -0.8480482697f, -0.8571674228f,
-0.8660254478f, -0.8746197224f, -0.8829475641f, -0.8910066485f,
-0.8987941146f, -0.9063078165f, -0.9135454297f, -0.9205048084f,
-0.9271839857f, -0.9335805178f, -0.9396926761f, -0.9455185533f,
-0.9510564804f, -0.9563048482f, -0.9612617493f, -0.9659258723f,
-0.9702957273f, -0.9743701220f, -0.9781476259f, -0.9816272259f,
-0.9848077893f, -0.9876883030f, -0.9902681112f, -0.9925462008f,
-0.9945219159f, -0.9961947203f, -0.9975640178f, -0.9986295700f,
-0.9993908405f, -0.9998477101f, -1.0000000000f, -0.9998477101f,
-0.9993908405f, -0.9986295104f, -0.9975640178f, -0.9961947203f,
-0.9945218563f, -0.9925461411f, -0.9902680516f, -0.9876883030f,
-0.9848077297f, -0.9816271067f, -0.9781475663f, -0.9743700027f,
-0.9702957273f, -0.9659258127f, -0.9612616301f, -0.9563046694f,
-0.9510564804f, -0.9455185533f, -0.9396926165f, -0.9335803390f,
-0.9271837473f, -0.9205048084f, -0.9135454297f, -0.9063078165f,
-0.8987939358f, -0.8910064101f, -0.8829475641f, -0.8746197224f,
-0.8660254478f, -0.8571671247f, -0.8480479717f, -0.8386704922f,
-0.8290376067f, -0.8191518188f, -0.8090168238f, -0.7986354232f,
-0.7880107164f, -0.7771459818f, -0.7660441995f, -0.7547094226f,
-0.7431447506f, -0.7313536406f, -0.7193398476f, -0.7071065307f,
-0.6946582198f, -0.6819982529f, -0.6691305637f, -0.6560590863f,
-0.6427873969f, -0.6293202043f, -0.6156613827f, -0.6018149853f,
-0.5877849460f, -0.5735762119f, -0.5591927171f, -0.5446389318f,
-0.5299192667f, -0.5150377750f, -0.4999997616f, -0.4848094583f,
-0.4694714844f, -0.4539905190f, -0.4383708239f, -0.4226180315f,
-0.4067364931f, -0.3907310665f, -0.3746066391f, -0.3583676219f,
-0.3420199156f, -0.3255680203f, -0.3090169430f, -0.2923717499f,
-0.2756370306f, -0.2588188350f, -0.2419217676f, -0.2249510288f,
-0.2079117596f, -0.1908086985f, -0.1736479700f, -0.1564343572f,
-0.1391730905f, -0.1218689531f, -0.1045281738f, -0.0871555507f,
-0.0697563812f, -0.0523359627f, -0.0348991230f, -0.0174521320f,
};

static float g_cosinuses[360] = {
1.0000000000f, 0.9998477101f, 0.9993908405f, 0.9986295104f,
0.9975640774f, 0.9961947203f, 0.9945219159f, 0.9925461411f,
0.9902680516f, 0.9876883626f, 0.9848077297f, 0.9816271663f,
0.9781476259f, 0.9743700624f, 0.9702957273f, 0.9659258127f,
0.9612616897f, 0.9563047290f, 0.9510565400f, 0.9455185533f,
0.9396926165f, 0.9335803986f, 0.9271838665f, 0.9205048680f,
0.9135454297f, 0.9063077569f, 0.8987940550f, 0.8910065293f,
0.8829475641f, 0.8746197224f, 0.8660253882f, 0.8571673036f,
0.8480480909f, 0.8386705518f, 0.8290375471f, 0.8191520572f,
0.8090170026f, 0.7986354828f, 0.7880107760f, 0.7771459222f,
0.7660444379f, 0.7547096014f, 0.7431448102f, 0.7313537002f,
0.7193397880f, 0.7071067691f, 0.6946583390f, 0.6819983721f,
0.6691305637f, 0.6560589671f, 0.6427876353f, 0.6293203831f,
0.6156614423f, 0.6018150449f, 0.5877852440f, 0.5735763907f,
0.5591928959f, 0.5446389914f, 0.5299192667f, 0.5150380731f,
0.4999999702f, 0.4848095775f, 0.4694715142f, 0.4539905190f,
0.4383711517f, 0.4226182401f, 0.4067366123f, 0.3907310665f,
0.3746065199f, 0.3583678603f, 0.3420201540f, 0.3255681396f,
0.3090169728f, 0.2923716605f, 0.2756372690f, 0.2588190734f,
0.2419219017f, 0.2249510437f, 0.2079116553f, 0.1908089370f,
0.1736481041f, 0.1564343721f, 0.1391731054f, 0.1218693256f,
0.1045284197f, 0.0871556848f, 0.0697563961f, 0.0523359738f,
0.0348994955f, 0.0174523834f, -0.0000000437f, -0.0174524710f,
-0.0348995812f, -0.0523360595f, -0.0697564781f, -0.0871557668f,
-0.1045285091f, -0.1218694076f, -0.1391731948f, -0.1564344466f,
-0.1736481935f, -0.1908090264f, -0.2079117447f, -0.2249511182f,
-0.2419219762f, -0.2588191628f, -0.2756373584f, -0.2923717499f,
-0.3090170324f, -0.3255682290f, -0.3420202434f, -0.3583679497f,
-0.3746066093f, -0.3907311559f, -0.4067367017f, -0.4226183295f,
-0.4383711219f, -0.4539906085f, -0.4694715738f, -0.4848097563f,
-0.5000000596f, -0.5150380135f, -0.5299193263f, -0.5446390510f,
-0.5591930151f, -0.5735764503f, -0.5877851844f, -0.6018151045f,
-0.6156614423f, -0.6293205023f, -0.6427876353f, -0.6560591459f,
-0.6691306829f, -0.6819983125f, -0.6946584582f, -0.7071067691f,
-0.7193399072f, -0.7313537598f, -0.7431449294f, -0.7547096610f,
-0.7660444379f, -0.7771460414f, -0.7880107760f, -0.7986356020f,
-0.8090170622f, -0.8191520572f, -0.8290376067f, -0.8386705518f,
-0.8480481505f, -0.8571673036f, -0.8660253882f, -0.8746197820f,
-0.8829475641f, -0.8910065889f, -0.8987940550f, -0.9063078761f,
-0.9135454893f, -0.9205048680f, -0.9271839261f, -0.9335804582f,
-0.9396926761f, -0.9455186129f, -0.9510565996f, -0.9563047886f,
-0.9612616897f, -0.9659258723f, -0.9702957273f, -0.9743701220f,
-0.9781476259f, -0.9816271663f, -0.9848077893f, -0.9876883626f,
-0.9902681112f, -0.9925461411f, -0.9945219159f, -0.9961947203f,
-0.9975640774f, -0.9986295700f, -0.9993908405f, -0.9998477101f,
-1.0000000000f, -0.9998477101f, -0.9993908405f, -0.9986295104f,
-0.9975640178f, -0.9961947203f, -0.9945218563f, -0.9925461411f,
-0.9902680516f, -0.9876883030f, -0.9848077297f, -0.9816271663f,
-0.9781475663f, -0.9743700624f, -0.9702956676f, -0.9659258127f,
-0.9612616301f, -0.9563047290f, -0.9510565400f, -0.9455185533f,
-0.9396926165f, -0.9335803986f, -0.9271838069f, -0.9205047488f,
-0.9135454297f, -0.9063078165f, -0.8987939954f, -0.8910065293f,
-0.8829475045f, -0.8746196628f, -0.8660252690f, -0.8571672440f,
-0.8480480909f, -0.8386704922f, -0.8290375471f, -0.8191519380f,
-0.8090169430f, -0.7986354828f, -0.7880106568f, -0.7771459222f,
-0.7660443187f, -0.7547095418f, -0.7431448102f, -0.7313536406f,
-0.7193397880f, -0.7071066499f, -0.6946583390f, -0.6819981933f,
-0.6691305041f, -0.6560590267f, -0.6427875161f, -0.6293203235f,
-0.6156615019f, -0.6018147469f, -0.5877850652f, -0.5735763311f,
-0.5591928959f, -0.5446391106f, -0.5299189687f, -0.5150378942f,
-0.4999999106f, -0.4848096073f, -0.4694716334f, -0.4539902210f,
-0.4383709729f, -0.4226181805f, -0.4067366421f, -0.3907312155f,
-0.3746063411f, -0.3583677709f, -0.3420200646f, -0.3255681694f,
-0.3090170920f, -0.2923714519f, -0.2756372094f, -0.2588189840f,
-0.2419219315f, -0.2249507159f, -0.2079114467f, -0.1908088475f,
-0.1736481339f, -0.1564345211f, -0.1391727775f, -0.1218691170f,
-0.1045283377f, -0.0871557146f, -0.0697565451f, -0.0523356497f,
-0.0348992869f, -0.0174522959f, 0.0000000119f, 0.0174523201f,
0.0348997861f, 0.0523361489f, 0.0697565675f, 0.0871557370f,
0.1045288369f, 0.1218696162f, 0.1391732693f, 0.1564345360f,
0.1736481488f, 0.1908093393f, 0.2079119384f, 0.2249512076f,
0.2419219464f, 0.2588190138f, 0.2756376863f, 0.2923719287f,
0.3090171218f, 0.3255681992f, 0.3420200944f, 0.3583682477f,
0.3746067882f, 0.3907312453f, 0.4067366719f, 0.4226181805f,
0.4383714199f, 0.4539906681f, 0.4694716632f, 0.4848096371f,
0.4999999106f, 0.5150383115f, 0.5299194455f, 0.5446391106f,
0.5591928959f, 0.5735767484f, 0.5877854824f, 0.6018151641f,
0.6156615019f, 0.6293203831f, 0.6427878737f, 0.6560592055f,
0.6691307425f, 0.6819983721f, 0.6946583390f, 0.7071070075f,
0.7193399668f, 0.7313538194f, 0.7431448698f, 0.7547095418f,
0.7660446167f, 0.7771461010f, 0.7880108356f, 0.7986355424f,
0.8090172410f, 0.8191522360f, 0.8290376663f, 0.8386706114f,
0.8480480909f, 0.8571674824f, 0.8660255671f, 0.8746197820f,
0.8829476237f, 0.8910065293f, 0.8987942338f, 0.9063078761f,
0.9135455489f, 0.9205048680f, 0.9271838665f, 0.9335805774f,
0.9396926761f, 0.9455186129f, 0.9510565400f, 0.9563047290f,
0.9612618089f, 0.9659258723f, 0.9702957869f, 0.9743700624f,
0.9781475663f, 0.9816272259f, 0.9848077893f, 0.9876883626f,
0.9902680516f, 0.9925462008f, 0.9945219159f, 0.9961947203f,
0.9975640774f, 0.9986295104f, 0.9993908405f, 0.9998477101f,
};

typedef unsigned int u32;
typedef unsigned __int64 u64;

unsigned char   s_second_method_original_message[16+s_finger_print_max_length]	=	
		{	41, 35, 190, 132, 225, 108, 214, 174, 82, 144, 73, 241, 241, 187, 233, 235, 
			179, 166, 219, 60, 135, 12, 62, 153, 36, 94, 13, 28, 6, 183, 71, 222,
			53, 56, 19, 40, 15, 142, 112, 13, 136, 44, 15, 231, 46, 13, 1, 212,	};

// bit_offs1, bit_offs2 <= 4
void	write_finger_print_first_method	(pstr target_mark, pcstr finger_print, int bit_offs1, int bit_offs2, unsigned int build_id)
{
	math::randoms_table<s_finger_print_max_length * 2, 
						sizeof(s_sinuses) / sizeof(s_sinuses[0]) - 4, 
						math::allow_duplicates_false>
										table(build_id);

	float * sinuses					=	(float *)target_mark;
	for ( int i=0; i<s_finger_print_max_length; ++i )
	{
		u32 const index1			=	table.next_random();
		char & character			=	*((char*)(& sinuses[4+index1])+(s_big_endian ? 3 : 0));
		character					=	(character & ~(0x0f << bit_offs1)) | ((finger_print[i] & 0x0f) << bit_offs1);
		u32 const index2			=	table.next_random();
		char & character2			=	*((char*)(& sinuses[4+index2])+(s_big_endian ? 2 : 1));
		character2					=	(character2 & ~(0xf0 >> bit_offs2)) | ((finger_print[i] & 0xf0) >> bit_offs2);
	}
}

void	write_finger_print_second_method	(pstr target_mark, pcstr finger_print)
{
	for ( int i=0; i<s_finger_print_max_length; ++i )
		target_mark[16+i]			=	finger_print[i] ^ (1 + 4 + 16 + 64);
}

// bit_offs1, bit_offs2 <= 4
bool	read_finger_print_first_method	(pstr mark, buffer_string * out_finger_print, int bit_offs1, int bit_offs2, unsigned int build_id)
{
	float * const sinuses			=	(float *)mark;
	bool differs_from_original		=	false;
	for ( int i=0; i<360; ++i )
	{
		if ( sinuses[i] != s_sinuses[i] )
		{
			differs_from_original	=	true;
			break;
		}
	}

	if ( !differs_from_original )
	{
		* out_finger_print			=	"not finger-printed";
		return							false;
	}

	math::randoms_table<s_finger_print_max_length*2, 
						sizeof(s_sinuses) / sizeof(s_sinuses[0]) - 4, 
						math::allow_duplicates_false>
										table(build_id);

	char message[s_finger_print_max_length+1];
	for ( int i=0; i<s_finger_print_max_length; ++i )
	{
		u32 const index1			=	table.next_random();
		u32 const index2			=	table.next_random();
		char a0 					=	((*((char*)(& sinuses[4+index1])+(s_big_endian ? 3 : 0))) & (0x0f << bit_offs1)) >> bit_offs1;
		char a1 					=	((*((char*)(& sinuses[4+index2])+(s_big_endian ? 2 : 1))) & (0xf0 >> bit_offs2)) << bit_offs2;
		message[i]					=	a1 | a0;
	}
	message[s_finger_print_max_length]	=	0;
	* out_finger_print				=	message;

	return								true;
}

bool	read_finger_print_second_method (pstr mark, buffer_string * out_finger_print)
{
	bool differs_from_original		=	false;
	for ( int i=0; i<16+s_finger_print_max_length; ++i )
	{
		char const char1			=	s_second_method_original_message[i];
		char const char2			=	mark[i];
		if ( char1 != char2 )
		{
			differs_from_original	=	true;
			break;
		}
	}

	if ( !differs_from_original )
	{
		* out_finger_print			=	"not finger-printed";
		return							false;
	}

	char message[s_finger_print_max_length+1];
	for ( int i=0; i<s_finger_print_max_length; ++i )
		message[i]					=	mark[16+i] ^ (1 + 4 + 16 + 64);

	message[s_finger_print_max_length]	=	0;
	* out_finger_print				=	message;

	return								true;
}

signalling_bool   read_file (pstr * out_file_data, u32 const max_file_size, u32 * out_file_size, pcstr file_name)
{
	fs::file_type * file				=	NULL;
	if ( !fs::open_file(& file, open_file_read | open_file_write, file_name, false) )
	{
		printf								("Error: cannot open file: '%file_name'");
		return								false;
	}

	if ( !fs::seek_file(file, 0, SEEK_END) )
	{
		printf								("Error: seek to end operation failed");
		return								false;
	}

	u32 const file_size					=	(u32)fs::tell_file(file);
	if ( file_size > max_file_size )
	{
		printf								("file_size too large: %d mb, maximum is: %d mb", 
											 file_size/(1024*1024), max_file_size/(1024*1024));
		return								false;
	}

	* out_file_data						=	ALLOC(char, file_size + 1024);

	if ( !fs::seek_file(file, 0, SEEK_SET) )
	{
		printf								("Error: seek to origin operation failed");
		return								false;
	}

	if ( fs::read_file(file, * out_file_data, file_size) != file_size )
	{
		printf								("Error: read operation failed");
		return								false;
	}
	else
		(* out_file_data)[file_size]	=	0;

	if ( out_file_size )
		*out_file_size					=	file_size;

	fs::close_file							(file);
	return									true;
}

signalling_bool   find_marks			(pcstr file_name, pstr file_data, u32 file_size, pstr * mark1, pstr * mark2)
{
	char mark1_string[16], mark2_string[16];
	memcpy_s								(mark1_string, sizeof(mark1_string), &s_sinuses[0], sizeof(mark1_string));
	memcpy_s								(mark2_string, sizeof(mark2_string), s_second_method_original_message, sizeof(mark1_string));

	* mark1								=	NULL;
	* mark2								=	NULL;
	for ( u32 i=0; i<file_size-16; ++i )
	{
		pcstr mark_strings[]			=	{ mark1_string, mark2_string };
		pstr * marks[]					=	{ mark1, mark2 };

		for ( u32 j=0; j<array_size(mark_strings); ++j )
		{
			if ( !memcmp(file_data+i, mark_strings[j], 16) )
			{
				if ( * marks[j] )
				{
					printf					("Error: omg find duplicate of mark in binary!");
					return					false;
				}
				* marks[j]				=	file_data + i;
			}
		}
	}

	if ( ! * mark1 && ! * mark2 )
	{
		printf								("Error: cannot find any mark in binary: %s\n", file_name);
		return								false;
	}
	return									true;
}


void   application::initialize ()
{
	xray::core::preinitialize				(& core_engine, 
											 logging::no_log,
											 GetCommandLine(), 
											 command_line::contains_application_true,
											 "finger-print",
											 __DATE__);

	g_allocator.user_current_thread_id();
	g_allocator.do_register					(100 * 1024*1024,	"main allocator");
	xray::memory::allocate_region			( );
	xray::core::initialize					("../../resources/sources/", "main", core::perform_debug_initialization);
}

void   application::finalize ()
{
	xray::core::finalize					();
}

bool   read_childs_output (buffer_string * child_output, pstr child_command_line, pcstr app_working_directory)
{ 
	HANDLE  hChildStdoutRd, hChildStdoutWr, hStdout;
	SECURITY_ATTRIBUTES saAttr; 

	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
	saAttr.bInheritHandle = TRUE; 
	saAttr.lpSecurityDescriptor = NULL; 
	
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE); 

	if ( ! CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &saAttr, 0) ) 
	{
		printf						("Error: stdout pipe creation failed\n"); 
		return						false;
	}

	SetHandleInformation( hChildStdoutRd, HANDLE_FLAG_INHERIT, 0);

	PROCESS_INFORMATION				piProcInfo; 
	STARTUPINFO						siStartInfo;
	ZeroMemory						(&piProcInfo, sizeof(PROCESS_INFORMATION));
	ZeroMemory						(&siStartInfo, sizeof(STARTUPINFO));
	siStartInfo.cb				=	sizeof(STARTUPINFO); 
	siStartInfo.hStdOutput		=	hChildStdoutWr;
	siStartInfo.dwFlags			|=	STARTF_USESTDHANDLES;

	BOOL bFuncRetn = CreateProcess(NULL, 
	  child_command_line,			// command line 
	  NULL,          				// process security attributes 
	  NULL,          				// primary thread security attributes 
	  TRUE,          				// handles are inherited 
	  0,             				// creation flags 
	  NULL,          				// use parent's environment 
	  app_working_directory,         
	  &siStartInfo,  				// STARTUPINFO pointer 
	  &piProcInfo);  				// receives PROCESS_INFORMATION 

	CloseHandle						(hChildStdoutWr);

	if ( bFuncRetn == 0 )
	{
		printf						("cannot run command_line: %s", child_command_line);
		return						false;
	}
	else
	{
		CloseHandle					(piProcInfo.hProcess);
		CloseHandle					(piProcInfo.hThread);
	}
	  
	DWORD dwRead; 
	CHAR chBuf						[4096];
	ZeroMemory						(chBuf, sizeof(chBuf));
	ReadFile						(hChildStdoutRd, chBuf, 4096, &dwRead, NULL);
	Sleep							(100);

	CloseHandle						(hChildStdoutRd);
	* child_output				=	chBuf;
	if ( !child_output->length() )
	{	
		printf						("Error: application has not written build id to stdout\n"); 
		return						false;
	}

	return							true;
} 

namespace xray {
namespace command_line {

bool   key_is_set (pcstr key);

} // namespace command_line
} // namespace xray

void   application::do_finger_printing (fixed_string512 const & file_name, fixed_string512 const & finger_print_message, pstr mark1, pstr mark2, u32 build_id, pstr file_data, u32 file_size)
{
	printf									("File: %s\n", file_name);

	fixed_string512	finger_print1		=	"mark is not available";
	bool const finger_printed_1			=	mark1 && read_finger_print_first_method(mark1, & finger_print1, 3, 2, build_id);

	fixed_string512	finger_print2		=	"mark is not available";
	bool const finger_printed_2			=	mark2 && read_finger_print_second_method(mark2, & finger_print2);

	if ( s_read_key )
	{
		if ( command_line::key_is_set(".scrk") )
		{
			printf							("Secure fingerprint:  %s\n", finger_print1.c_str());
			printf							("Public fingerprint: %s\n", finger_print2.c_str());
		}
		else
			printf							("Fingerprint: %s\n", finger_print2.c_str());

	   	m_return_code					=	core_engine.get_exit_code();
		return;
	}

	if ( finger_print_message.length() > s_finger_print_max_length )
	{
		printf								("Error, finger print message cant be longer then %d chars\n", s_finger_print_max_length);
		m_return_code					=	1;
		return;
	}

	if ( (finger_printed_1 || finger_printed_2) && !s_force )
	{
		printf								("Warning: already finger-printed. Use -force to rewrite old fingerprint");
		m_return_code					=	core_engine.get_exit_code();
		return;
	}

	pcstr message						=	finger_print_message.c_str();
	if ( mark1 )
	{
 		write_finger_print_first_method		(mark1, message, 3, 2, (u32)build_id);
	}

	if ( mark2 )
	 	write_finger_print_second_method	(mark2, message);

	fixed_string512 backup_path			=	file_name;
	backup_path							+=	".backup";
	_unlink									(backup_path.c_str());
	rename									(file_name.c_str(), backup_path.c_str());

	FILE * file							=	NULL;
	errno_t const file_open_err			=	fopen_s(& file, file_name.c_str(), "wb");
	ASSERT									(file_open_err == 0);
	fwrite									(file_data, 1, file_size, file);
	fclose									(file);

	printf									("Finger printing: ok\n");
   	m_return_code						=	core_engine.get_exit_code();
}

void   application::execute ()
{
#ifndef XRAY_STATIC_LIBRARIES
	command_line::check_keys			( );
	command_line::handle_help_key		( );
#endif // #ifndef XRAY_STATIC_LIBRARIES

	if ( !s_read_key && !s_write_key )
	{
		printf								("%s", "Error: use should use either -read or -write option\nType -help for help\n");
		m_return_code					=	1;
		return;
	}

	fixed_string512	file_name_string;
	if ( !s_file_key.is_set_as_string(& file_name_string) )
	{
		printf								("%s", "Error: use -file=<path_to_exe_file>\n");
		m_return_code					=	1;
		return;
	}

	u32 const max_file_names			=	2;
	fixed_string512	file_names[max_file_names];
	int semicolon_pos					=	file_name_string.find(';');
	if ( semicolon_pos == file_name_string.npos )
	{
		file_names[0]					=	file_name_string;
	}
	else
	{
		file_names[0]					=	file_name_string.substr(0, semicolon_pos);
		if ( file_names[1].find(';') != file_names[1].npos )
		{
			printf							("%s", "Error: maximum 2 files can be finger-printed at once\n");
			m_return_code				=	1;
			return;
		}

		file_names[1]					=	file_name_string.substr(semicolon_pos+1, file_name_string.npos);
	}

	for ( u32 i=0; i<max_file_names; ++i )
		file_names[i]					=	convert_to_portable(file_names[i].c_str());

	u32	max_file_size					=	100 * 1024*1024 - 1024;
	u32 file_size[max_file_names];
	pstr file_data[max_file_names];
	for ( int i=0; i<max_file_names; ++i )
	{
		file_data[i]					=	NULL;
		if ( file_names[i].length() && !read_file(& file_data[i], max_file_size, & file_size[i], file_names[i].c_str()) )
		{
			m_return_code				=	1;
			return;
		}
	}

	pstr marks1								[max_file_names];
	pstr marks2								[max_file_names];
	memory::zero							(marks1);
	memory::zero							(marks2);

	for ( int i=0; i<max_file_names; ++i )
	{
		if ( !file_data[i] )
			continue;
		if ( !find_marks(file_names[i].c_str(), file_data[i], file_size[i], & marks1[i], & marks2[i]) )
		{
			m_return_code				=	1;
			return;
		}
	}

	u32	build_id						=	0;
	if ( s_build_id_key.is_set() )
	{
		float value						=	0;
		if ( !s_build_id_key.is_set_as_number( & value ) )
		{
			printf							("%s", "Error: wrong build_id supplied, must be number\n");
			m_return_code				=	1;
			return;
		}

		build_id						=	(u32)value;
	}
	else
	{
		fixed_string512 build_id_string;
		fixed_string512 ask_build_id_line	=	file_names[0];
		ask_build_id_line				+=	" -print_build_id -max_resources_size=0 -no_warning_on_page_file_size";

		fs::path_string app_working_directory;
		fs::directory_part_from_path(& app_working_directory, file_names[0].c_str());
		s_working_directory.is_set_as_string(& app_working_directory);
		if ( !read_childs_output(& build_id_string, ask_build_id_line.c_str(), 
								   app_working_directory.length() ? app_working_directory.c_str() : NULL) )
		{
			m_return_code				=	1;
			return;
		}

		strings::convert_string_to_number(build_id_string.c_str(), & build_id);
	}

	fixed_string512	finger_print_message;
	if ( s_write_key.is_set() && !s_write_key.is_set_as_string(& finger_print_message) )
	{
		printf								("%s", "Error, use -write=<finger_print_message>\n");
		m_return_code					=	1;
		return;
	}

	for ( int i=0; i<max_file_names; ++i )
	{
		if ( !file_data[i] )
			continue;

		do_finger_printing					(file_names[i], finger_print_message, marks1[i], marks2[i], build_id, file_data[i], file_size[i]);
		FREE								(file_data[i]);
	}
}
