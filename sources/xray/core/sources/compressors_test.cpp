#include "pch.h"
#include <xray/testing.h>
#include <xray/testing_macro.h>
#include <xray/core_test_suite.h>
#include <xray/compressor_ppmd.h>

XRAY_DECLARE_LINKAGE_ID(compressor_test)

#pragma message( XRAY_TODO("PS3: PPMD compressor test") )

#if !XRAY_PLATFORM_PS3

namespace xray {

class compressor_test
{
	template <class Compressor>
	bool test (Compressor & compressor)
	{
		u32 const max_buffer_size			=	16 * 1024;
		u32  original_size					=	rand() % max_buffer_size; 
		char original							[max_buffer_size];
		for ( u32 i=0; i<original_size; ++i )
			original[i]						=	rand() % 256;

		char compressed							[max_buffer_size + 1024] = "";
		char decompressed						[max_buffer_size] = "";

		u32 compressed_size					=	0;

		bool const result_of_compression	=	compressor.compress(memory::buffer(original, original_size), 
																	memory::buffer(compressed), 
																	compressed_size);

		TEST_CURE_ASSERT						(result_of_compression, return false);
		
		u32	decompressed_size				=	0;
		bool const result_of_decompression	=	compressor.decompress(	memory::buffer(compressed, compressed_size), 
															  			memory::buffer(decompressed), 
															  			decompressed_size);
		TEST_CURE_ASSERT						(result_of_decompression, return false);

		TEST_CURE_ASSERT_CMP					(original_size, ==, decompressed_size, return false);

		bool const result_of_compare		=	!memory::compare(memory::buffer(original, original_size),
																 memory::buffer(decompressed, decompressed_size));
		TEST_CURE_ASSERT						(result_of_compare, return false);

		return									true;
	}

public:
	void test (core_test_suite const * const)
	{
		for ( u32 j=0; j<5; ++j )
		{
			ppmd_compressor	ppmd_compressor		(&memory::g_mt_allocator, 4);
			for ( u32 i=0; i<10; ++i )
			{
				if ( !test						(ppmd_compressor) )
					break;
			}
		}
	}
};

//REGISTER_TEST_CLASS(compressor_test, core_test_suite)

} // namespace xray

#endif // #if !XRAY_PLATFORM_PS3