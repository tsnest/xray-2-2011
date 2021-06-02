/*  SCE CONFIDENTIAL
 *  PlayStation(R)3 Programmer Tool Runtime Library 300.001
 *  Copyright (C) 2008 Sony Computer Entertainment Inc.
 *  All Rights Reserved.
 */

#ifndef __CELL_GTF_LOADER_H__
#define __CELL_GTF_LOADER_H__

#include <stdint.h>
#include <cell/gcm.h>

#if defined(__cplusplus)
extern	"C" {
#endif


#define CELL_GTF_OK CELL_OK
#define CELL_GTF_FAILED -1

typedef int32_t CELL_GTF_RESULT;

/* Texture File Header */
typedef struct
{
	uint32_t  version;      /* Version (Correspond to dds2gtf converter version) */
	uint32_t  size;         /* Total size of Texture (Excluding size of header & attribute) */
	uint32_t  numTexture;   /* Number of textures in this file */

} CellGtfFileHeader;

/* Attribute for Texture data 
 *
 *  It is always true that the number of textures in GTF file 
 *  equals the number of CellGtfTextureAttribute in header.
 */
typedef struct
{         
	uint32_t id;            /* Texture ID */
	uint32_t offsetToTex;   /* Offset to texture from begining of file */
	uint32_t textureSize;	/* Size of texture */
	CellGcmTexture tex;		/* Texture structure defined in GCM library */

} CellGtfTextureAttribute;

extern uint32_t cellGtfGetTextureSize(const char* filename);
extern uint32_t cellGtfGetTextureSizeFromMemory(const uint8_t* gtf_buffer, uint32_t size);
extern CELL_GTF_RESULT cellGtfLoadTexture(const char* filename, CellGcmTexture* texture, uint8_t location, void* image);
extern CELL_GTF_RESULT cellGtfLoadTextureFromMemory(const uint8_t* gtf_buffer, uint32_t size, CellGcmTexture* texture, uint8_t location, void* image);

extern uint32_t cellGtfGetPackedTextureNumber(const char* filename);
extern uint32_t cellGtfGetPackedTextureNumberFromMemory(const uint8_t* gtf_buffer, uint32_t size);
extern uint32_t cellGtfGetPackedTextureSize(const char* filename, uint32_t index);
extern uint32_t cellGtfGetPackedTextureSizeFromMemory(const uint8_t* gtf_buffer, uint32_t size, uint32_t index);
extern CELL_GTF_RESULT cellGtfLoadPackedTexture(const char* filename, uint32_t index, CellGcmTexture* texture, uint8_t location, void* image);
extern CELL_GTF_RESULT cellGtfLoadPackedTextureFromMemory(const uint8_t* gtf_buffer, uint32_t size, uint32_t index, CellGcmTexture* texture, uint8_t location, void* image);

extern CELL_GTF_RESULT cellGtfLoadWholeTexture(const char* filename, CellGcmTexture* texture_array, uint8_t location, void* image);
extern CELL_GTF_RESULT cellGtfLoadWholeTextureFromMemory(const uint8_t* gtf_buffer, uint32_t size, CellGcmTexture* texture_array, uint8_t location, void* image);

extern CELL_GTF_RESULT cellGtfGetTextureAttributeFromMappedMainMemory(uint8_t* gtf_buffer_on_mapped_memory, uint32_t size, CellGtfTextureAttribute** attrib);

/* for compatibility */
extern CELL_GTF_RESULT cellGtfCalculateMemorySize(CellGtfTextureAttribute* attrib, uint32_t *size);
extern CELL_GTF_RESULT cellGtfReadFileHeader(const char* filename, CellGtfFileHeader* header);
extern CELL_GTF_RESULT cellGtfReadTextureAttribute(const char* filename, uint32_t index, CellGtfTextureAttribute* attrib);

extern CELL_GTF_RESULT cellGtfLoad(const char* filename, uint32_t id, CellGcmTexture* texture, uint8_t location, void* image);

extern CELL_GTF_RESULT cellGtfReadFileHeaderFromMemory(const uint8_t* gtfptr, CellGtfFileHeader* header );
extern CELL_GTF_RESULT cellGtfReadTextureAttributeFromMemory(const uint8_t* gtfptr, uint32_t id, CellGtfTextureAttribute* attribs);


#if defined(__cplusplus)
}
#endif 

#endif /* __CELL_GTF_LOADER_H__ */
