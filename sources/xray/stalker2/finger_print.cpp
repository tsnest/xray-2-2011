#include "pch.h"

// message is starting from index 16

static u32 const s_finger_print_max_length	=	32;

static u8 const s_finger_print[16+s_finger_print_max_length]			
										=	{	41, 35, 190, 132, 225, 108, 214, 174, 82, 144, 73, 241, 241, 187, 233, 235, 
												179, 166, 219, 60, 135, 12, 62, 153, 36, 94, 13, 28, 6, 183, 71, 222,
												53, 56, 19, 40, 15, 142, 112, 13, 136, 44, 15, 231, 46, 13, 1, 212, };

static u8 const s_finger_print_original[s_finger_print_max_length]	=	{	179, 166, 219, 60, 135, 12, 62, 153, 36, 94, 13, 28, 6, 183, 71, 222,	
																			53, 56, 19, 40, 15, 142, 112, 13, 136, 44, 15, 231, 46, 13, 1, 212,	};


void   decode_finger_print (xray::fixed_string512 * out_finger_print)
{
	bool differ_from_original		=	false;
	for ( int i=0; i<s_finger_print_max_length; ++i )
		if ( s_finger_print_original[i] != s_finger_print[16+i] )
		{
			differ_from_original	=	true;
			break;
		}

	if ( !differ_from_original )
		* out_finger_print			=	"<this build has not been finger printed>";
	else
	{
		char buffer[s_finger_print_max_length+1];
		for ( int i=0; i<s_finger_print_max_length; ++i )
			buffer[i]				=	s_finger_print[16+i] ^ (1 + 4 + 16 + 64);
		buffer[s_finger_print_max_length]	=	0;

		* out_finger_print			=	buffer;
	}

	xray::command_line::set_finger_print	( out_finger_print->c_str() );
}
