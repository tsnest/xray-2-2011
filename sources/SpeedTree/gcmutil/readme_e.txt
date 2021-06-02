[SCE CONFIDENTIAL DOCUMENT]
PlayStation(R)3 Programmer Tool Runtime Library 300.001
                  Copyright (C) 2008 Sony Computer Entertainment Inc.
                                                 All Rights Reserved.

Overview:
    gcmutil.a is a library which consists of utility functions,
    used for developing applications with libgcm.
    
How to Use:
    1. Include gcmutil.h with applications to use.
    2. Link gcmutil.a  with applications to use.
    3. A function to call to start applications
                                   : cellGcmUtilInit()
    4. A function to call to terminate applications
                                   : cellGcmUtilEnd()

Description:
  Basically the APIs and structures of gcmutil are defined on the
  CellGcmUtil namespace. You can use them accordingly, by declaring
  "using namespace CellGcmUtil;".
  
Notes:
  The functions declared in gcmutil_old.h are for the C language,
  and they are left with the aim to maintain compatibility.
  
Reference:
  Specifications of the main APIs are as shown below:

======================================================================
                            Constants and Structures
======================================================================

----------------------------------------------------------------------
Vertex Attribute Constants
----------------------------------------------------------------------
[Description]
  The constants correspond to the slot numbers(0-15) of the vertex
  attribute.
  You can use them for functions such as 
  cellGcmSetVertexDataArray() etc.

[Details]
  CELL_GCMUTIL_ATTR_POSITION      Vertex(float4)
  CELL_GCMUTIL_ATTR_BLENDWEIGHT   Vertex weight(float)
  CELL_GCMUTIL_ATTR_NORMAL        Normal(float3)
  CELL_GCMUTIL_ATTR_COLOR         Color(float4)
  CELL_GCMUTIL_ATTR_COLOR0        Color(float4)
  CELL_GCMUTIL_ATTR_DIFFUSE       Color(float4)
  CELL_GCMUTIL_ATTR_COLOR1        Secondary color(float4)
  CELL_GCMUTIL_ATTR_SPECULAR      Secondary color(float4)
  CELL_GCMUTIL_ATTR_FOGCOORD      Fog coordinate(float)
  CELL_GCMUTIL_ATTR_TESSFACTOR    Fog coordinate(float)
  CELL_GCMUTIL_ATTR_PSIZE         Point size(float)
  CELL_GCMUTIL_ATTR_BLENDINDICES  Palette index for skinning(float4)
  CELL_GCMUTIL_ATTR_TEXCOORD0     Texture coordinate0(float4)
  CELL_GCMUTIL_ATTR_TEXCOORD1     Texture coordinate1(float4)
  CELL_GCMUTIL_ATTR_TEXCOORD2     Texture coordinate2(float4)
  CELL_GCMUTIL_ATTR_TEXCOORD3     Texture coordinate3(float4)
  CELL_GCMUTIL_ATTR_TEXCOORD4     Texture coordinate4(float4)
  CELL_GCMUTIL_ATTR_TEXCOORD5     Texture coordinate5(float4)
  CELL_GCMUTIL_ATTR_TEXCOORD6     Texture coordinate6(float4)
  CELL_GCMUTIL_ATTR_TANGENT       Tangent vector(float4)
  CELL_GCMUTIL_ATTR_TEXCOORD7     Texture coordinate7(float4)
  CELL_GCMUTIL_ATTR_BINORMAL      Binormal vector(float4)


----------------------------------------------------------------------
Memory_t structure
----------------------------------------------------------------------
[Description]
  Use this structure when you secure a memory area on main/local
  and then use it from libgcm or gcmutil.
  
[Details]
  struct Memory_t{
  	uint32_t offset;
  	void* addr;
  	uint32_t size;
  	uint8_t location;
    uint8_t  _padding0;
    uint16_t _padding1;
  };

[Parameter]
  uint32_t offset:
    - Offset of the secured memory area
      If the main memory is not mapped, 0xFFFFFFFF is assigned.
    
  void* addr:
    - Address of the secured memory area
    
  uint32_t size:
    - Size of the secured memory area
    
  uint8_t location:
    - Location of the secured memory area
        Main memory  : CELL_GCM_LOCATION_MAIN
        Local memory : CELL_GCM_LOCATION_LOCAL

  uint8_t  _padding0:
  uint16_t _padding1:
    - Reserved area


----------------------------------------------------------------------
Viewport_t structure
----------------------------------------------------------------------
[Description]
  A structure which consists of viewport info.
  Use it when you pass a value to cellGcmSetViewport().
  
[Details]
  struct{
  	uint16_t x, y, width, height;
  	float min, max;
  	float scale[4], offset[4];
  } Viewport_t;


----------------------------------------------------------------------
Shader_t structure
----------------------------------------------------------------------
[Description]
  Use this structure for using the vertex shader and the fragment
  shader dynamically.
  
[Details]
  struct Shader_t{
  	CGprogram program;
  	void *ucode;
  	bool is_vp;

  	bool is_cgb;
  	CellCgbProgram program_cgb;
  	CellCgbVertexProgramConfiguration vertex_program_conf;
  	CellCgbFragmentProgramConfiguration fragment_program_conf;

  	Memory_t body;
  };
  
[Parameter]
  CGprogram program:
    - Shader program(for NV Binary format)

  void *ucode:
    - Address of microcode of the shader
      For vertex shader, this is set when loading.

  bool is_vp:
    - shader format
        Vertex shader          : true
        Fragment shader        : false

  bool is_cgb:
    - shader format
        cgb format       : true
        NV Binary format : false

  CellCgbProgram program_cgb:
  CellCgbVertexProgramConfiguration vertex_program_conf:
  CellCgbFragmentProgramConfiguration fragment_program_conf:
    - Shader program and shader configuration(in case of cgb format)

  Memory_t body:
    - Body of the shader file
      This is always secured on the non-mapped main memory.


----------------------------------------------------------------------
FileList_t structure
----------------------------------------------------------------------
[Description]
  This structure maintains a file list.
  Use this structure for file list functions like 
  cellGcmUtilGetFileList() etc.
  
[Details]
  struct FileList_t{
  	uint32_t count;
  	uint32_t size;
  	char **files;
  };

[Parameter]
  uint32_t count:
    - The number of files maintained by the file list

  uint32_t size:
    - The number of buffers maintained by the file list
      * Basically this is not used by a user.

  char **files:
    - Pointer array to the buffer, which stores the file name



======================================================================
                            Inline function
======================================================================

----------------------------------------------------------------------
inline uint32_t cellGcmUtilGetAlign(uint32_t size, uint32_t alignment)
----------------------------------------------------------------------
[Description]
  Calculates alignment.
  
[Parameter]
  uint32_t size:
    - The original size
    
  uint32_t alignment:
    - Alignment
  
[Returned value]
  Returns the alignment size.


----------------------------------------------------------------------
inline float cellGcmUtilToRadian(float degree)
----------------------------------------------------------------------
[Description]
  converts the angle to Radian angle.
  
[Parameter]
  float degree:
    - Angle
  
[Returned value]
  Returns the converted Radian angle.


----------------------------------------------------------------------
inline float cellGcmUtilToDegree(float radian)
----------------------------------------------------------------------
[Description]
  Converts the Radian to an angle.
  
[Parameter]
  float radian:
    - Radian angle
  
[Returned value]
  Returns the converted angle.


======================================================================
                          API Reference
======================================================================

----------------------------------------------------------------------
bool cellGcmUtilInitLocal(void* local_base_addr, size_t local_size);
----------------------------------------------------------------------
[Description]
  This sets info of the local memory area, to be assigned
  to applications, to gcmutil.
  The area, which is set here, can also be assigned to applications
  using cellGcmUtilAllocateLocal() function.
  
  If you assign the whole area of the available local memory from 
  applications via gcmutil, please specify the local memory info
  which you obtained with cellGcmGetConfiguration().
  
  * If you use cellGcmUtilInit(), you do not need to
    call this function.
  
[Parameter]
  void* local_base_addr:
    - Effective address, which shows the base of the local memory
      to be assigned to applications.
    (If you assign the whole area of the available local memory,
     specify CellGcmConfig.localAddress.)
  
  size_t local_size:
    - Size of the local memory to be assigned to applications.
    (If you assign the whole area of the available local memory,
    specify CellGcmConfig.localSize)
  
[Returned value]
  If this is successfully called, true is returned.
  Otherwise, false is returned.


----------------------------------------------------------------------
bool cellGcmUtilInitMain(void* main_base_addr, size_t main_size);
----------------------------------------------------------------------
[Description]
  Sets the main memory area to be assigned from the gcmutil side.
  As for the area which is set here, you can assign to applications
  using cellGcmUtilAllocateMain().
  
  * If you use cellGcmUtilInit(), you do not need
    to call this function.
  
[Parameter]
  void* main_base_addr:
    - Effective address to show the base of the main memory to
      be assigned to applications.
    (This must be aligned to 1MB, using memalign() etc.)
  
  size_t main_size:
    - Size of the main memory to be assigned to applications.
    (This must be aligned to 1MB.)

[Returned value]
  If this is successfully called, true is returned.
  Otherwise, false is returned.


----------------------------------------------------------------------
bool cellGcmUtilAllocateLocal(uint32_t size, uint32_t alignment, Memory_t *memory);
----------------------------------------------------------------------
[Description]
  Assigns the local memory area to be used by applications.
  
[Parameter]
  uint32_t size:
    - Size of the local memory to be used by applications
  
  uint32_t alignment:
    - Size to align
  
  Memory_t *memory:
    - Secured memory area
  
[Returned value]
  If this is successfully called, true is returned.
  Otherwise, false is returned.

----------------------------------------------------------------------
bool cellGcmUtilAllocateMain(uint32_t size, uint32_t alignment, Memory_t *memory);
----------------------------------------------------------------------
[Description]
  Assigns the main memory area to be used by applications.
  
[Parameter]
  uint32_t size:
    - Size of the main memory to be used by applications
  
  uint32_t alignment:
    - Size to align
  
  Memory_t *memory:
    - Secured memory area
  
[Returned value]
  If this is successfully called, true is returned.
  Otherwise, false is returned.

----------------------------------------------------------------------
bool cellGcmUtilAllocate(uint32_t size, uint32_t alignment, uint8_t location, Memory_t *memory);
----------------------------------------------------------------------
[Description]
  Assigns the memory area to be used by applications.
  
[Parameter]
  uint32_t size:
    - Size of the main memory to be used by applications
  
  uint32_t alignment:
    - Size to align
    
  uint8_t location:
    - Location of the memory area to secure
        Main memory  : CELL_GCM_LOCATION_MAIN
        Local memory : CELL_GCM_LOCATION_LOCAL
  
  Memory_t *memory:
    - Secured memory area
  
[Returned value]
  If this is successfully called, true is returned.
  Otherwise, false is returned.


----------------------------------------------------------------------
bool cellGcmUtilReallocate(Memory_t *memory, uint8_t location, uint32_t alignment);
----------------------------------------------------------------------
[Description]
  Resecure the memory area by modifying its location and alignment,
  that was once secured previously.
  When this is successfully called, the memory is overwritten
  in the resecured area.
  Also, the data is copied into the resecured area.
  When this fails in calling, the memory is not freed.
  
[Parameter]
  Memory_t *memory:
    - Target memory area
  
  uint32_t alignment:
    - Size to align
    
  uint8_t location:
    - Location of the memory area to secure
        Main memory  : CELL_GCM_LOCATION_MAIN
        Local memory : CELL_GCM_LOCATION_LOCAL
  
[Returned value]
  If this is successfully called, true is returned.
  Otherwise, false is returned.


----------------------------------------------------------------------
bool cellGcmUtilAllocateUnmappedMain(uint32_t size, uint32_t alignment, Memory_t *memory);
----------------------------------------------------------------------
[Description]
  Assigns the memory area to be used by applications.
  The area, which is secured by this function, can not be used
  from RSX(TM).
  The "offset" member of the "Memory_t" structure is set to
  0xFFFFFFFF.
  
[Parameter]
  uint32_t size:
    - Size of the main memory to be used by applications
  
  uint32_t alignment:
    - Size to align
    
  Memory_t *memory:
    - Secured memory area
  
[Returned value]
  If this is successfully called, true is returned.
  Otherwise, false is returned.


----------------------------------------------------------------------
void cellGcmUtilFree(Memory_t *memory);
----------------------------------------------------------------------
[Description]
  Frees the memory area which was assigned to applications
  with the functions below.
    - bool cellGcmUtilAllocateLocal()
    - bool cellGcmUtilAllocateMain()
    - bool cellGcmUtilAllocate()
    - bool cellGcmUtilReallocate()
    - bool cellGcmUtilAllocateUnmappedMain()

[Parameter]
  Memory_t *memory:
    - Memory area to free
  
[Returned value]
   None


----------------------------------------------------------------------
void cellGcmUtilFinalizeMemory(void);
----------------------------------------------------------------------
[Description]
  Terminates the gcmutil processings.
  Mainly, checks the memory leak which was caused by the usage
  of the following functions:
    - bool cellGcmUtilAllocateLocal()
    - bool cellGcmUtilAllocateMain()
    - bool cellGcmUtilAllocate()
    - bool cellGcmUtilReallocate()
    - bool cellGcmUtilAllocateUnmappedMain()

[Parameter]
  None
  
[Returned value]
  None


----------------------------------------------------------------------
bool cellGcmUtilReadFile(const char *fname, uint8_t **buffer, uint32_t *buf_size, uint32_t offset = 0, uint32_t size = 0xFFFFFFFFUL);
----------------------------------------------------------------------
[Description]
  Loads the specified files.
  Please delete the buffer since this is secured automatically.
  
[Parameter]
  const char *fname:
    - Name of the file to load
  
  uint8_t **buffer:
    - Pointer to the variable, which stores the address to load
      the file data
    
  uint32_t *buf_size:
    - Pointer to the variable, which stores the size of the
      loaded file
  
  uint32_t offset:
    - Starting point to load the file(if not specified, starts
      from the lead of the file)
    
  uint32_t size:
    - Size to load(if not specified, this is the file size)
    
[Returned value]
  If this is successfully called, true is returned.
  Otherwise, false is returned.


----------------------------------------------------------------------
bool cellGcmUtilWriteFile(const char *fname, uint8_t *buffer, uint32_t buf_size);
----------------------------------------------------------------------
[Description]
  Saves the contents of the buffer.
  
[Parameter]
  const char *fname:
    - Name of the file to save
  
  uint8_t *buffer:
    - Address of the data to save
    
  uint32_t buf_size:
    - Size of data to save
    
[Returned value]
  If this is successfully called, true is returned.
  Otherwise, false is returned.


----------------------------------------------------------------------
bool cellGcmUtilIsFileExsits(const char *fname);
----------------------------------------------------------------------
[Description]
  Checks if the file exists.
  
[Parameter]
  const char *fname:
    - File name
    
[Returned value]
  If the file exists, true is returned.
  Otherwise, false is returned.


----------------------------------------------------------------------
uint32_t cellGcmUtilGetFileList(const char *path, const char *ext, FileList_t *filelist);
----------------------------------------------------------------------
[Description]
  Returns a list of the files, which exists in the
  specified directory.
  This does not search into the sub directories.
  You can add the file to the list, by calling this function
  without changing the file list.
  This is useful for searching files with various extensions.
  
[Parameter]
  const char *path:
    - Name of the path to search
  
  const char *ext:
    - Extension of the file to search
    
  FileList_t *filelist:
    - Pointer to the variable, which stores the file list
    
[Returned value]
  If this is successfully called, true is returned.
  Otherwise, false is returned.


----------------------------------------------------------------------
void cellGcmUtilFreeFileList(FileList_t *filelist);
----------------------------------------------------------------------
[Description]
  Frees the file list.
  
[Parameter]
  FileList_t *filelist:
    - Pointer to the file list to free
    
[Returned value]
  None


----------------------------------------------------------------------
bool cellGcmUtilInitDisplay
(
    uint8_t color_format,
    uint8_t depth_format,
    uint8_t buffer_number,
    Memory_t *buffer,
    CellGcmSurface *surface
);
----------------------------------------------------------------------
[Description]
  Initializes the color buffer and the depth buffer.
  Also, this returns the surface, assigned to the secured region.
  
[Parameter]
  uint8_t color_format:
    - Format of the color buffer
  
  uint8_t depth_format:
    - Format of the depth buffer
    
  uint8_t buffer_number:
    - The number of the buffer
      Normally, double or triple buffers are specified.
    
  Memory_t *buffer:
    - Returns the (buffer_number + 1) memory area of the secured buffer.
      The "+1" memory area becomes a depth buffer.
  
  CellGcmSurface *surface:
    - Returns buffer_number surface, which is assigned to the secured
      memory area.
    
[Returned value]
  If this is successfully called, true is returned.
  Otherwise, false is returned.


----------------------------------------------------------------------
bool cellGcmUtilDbgfontInit(int32_t word_count, void* addr, uint32_t size, uint8_t location);
----------------------------------------------------------------------
[Description]
  Initializes dbgfont.
  
[Parameter]
  int32_t word_count:
    - String, desirable to display in dbgfont
  
  void* addr:
    - Address of the memory to be used for dbgfont.
      This must be secured before calling the function.
    
  uint32_t size:
    - Size of the memory to be used for dbgfont.
      Please obtain the necessary size with 
      cellGcmUtilDbgfontGetRequireSize().
    
  uint8_t location:
    - Location of the memory to be used for dbgfont
    
[Returned value]
  If this is successfully called, true is returned.
  Otherwise, false is returned.


----------------------------------------------------------------------
void cellGcmUtilDbgfontEnd(void);
----------------------------------------------------------------------
[Description]
  Terminates dbgfont.
  
[Parameter]
  None
  
[Returned value]
  None


----------------------------------------------------------------------
uint32_t cellGcmUtilDbgfontGetRequireSize(int32_t word_count);
----------------------------------------------------------------------
[Description]
  Returns the necessary memory size for dbgfont.
  
[Parameter]
  int32_t word_count:
    - The number of the characters, desirable to display in dbgfont
    
[Returned value]
  If this is successfully called, necessary memory size for dbgfont
  is returned.
  Otherwise, 0 is returned.

----------------------------------------------------------------------
void cellGcmUtilPuts(const char* str);
----------------------------------------------------------------------
[Description]
  Displays strings in the current position of dbgfont.
  Line feed is also done properly.
  
[Parameter]
  const char* str:
    - Strings to display
  
[Returned value]
  None


----------------------------------------------------------------------
void cellGcmUtilPrintf(const char* format, ...);
----------------------------------------------------------------------
[Description]
  Displays formatted strings in the current position of dbgfont.
  Line feed is also done properly.
  
[Parameter]
  const char* format, ...:
    - Formatted strings
  
[Returned value]
  None


----------------------------------------------------------------------
void cellGcmUtilPrintfColor(uint32_t color, const char* format, ...);
----------------------------------------------------------------------
[Description]
  Specifies the color to the current position of dbgfont,
  and then displays formatted strings.
  Line feed is also done properly.
  
[Parameter]
  uint32_t color:
    - The color of the string A8R8G8B8
    
  const char* format, ...:
    - Formatted strings
  
[Returned value]
  None


----------------------------------------------------------------------
void cellGcmUtilSetPrintPos(float x, float y);
----------------------------------------------------------------------
[Description]
  Specifies the current position of dbgfont.
  Does not change the coordinates of the axis, to which 
  CELL_GCMUTIL_POSFIX is specified.
  
[Parameter]
  float x, y:
    - Coordinates of the position
      Upper left of the screen  (0.0f, 0.0f)
      Lower right of the screen (1.0f, 1.0f)

[Returned value]
  None


----------------------------------------------------------------------
void cellGcmUtilSetPrintSize(float size);
----------------------------------------------------------------------
[Description]
  Specifies the character size of dbgfont.
  
[Parameter]
  float size:
    - Size of the characters
      1.0f: Displays 80x32 characters in the full screen

[Returned value]
  None


----------------------------------------------------------------------
void cellGcmUtilSetPrintColor(uint32_t color);
----------------------------------------------------------------------
[Description]
  Specifies the color of the characters of dbgfont.
  
[Parameter]
  uint32_t color:
    - The color of the strings A8R8G8B8

[Returned value]
  None


----------------------------------------------------------------------
void cellGcmUtilGetPrintPos(float *x, float *y);
----------------------------------------------------------------------
[Description]
  Obtains the current position of dbgfont.
  
[Parameter]
  float *x, *y:
    - Pointer to the variable, which stores the coordinates
      of the current position

[Returned value]
  None


----------------------------------------------------------------------
bool cellGcmUtilInit(int32_t cb_size, int32_t main_size);
----------------------------------------------------------------------
[Description]
  Initializes libgcm and gcmutil collectively.
  
[Parameter]
  int32_t cb_size:
    - Size of the default command buffer
  
  int32_t main_size:
    - Size of the main memory to be used from RSX(TM)
  
[Returned value]
  If this is successfully called, true is returned.
  Otherwise, false is returned.


----------------------------------------------------------------------
CellVideoOutResolution cellGcmUtilGetResolution(void);
----------------------------------------------------------------------
[Description]
  Obtains the solution of the current display.
  
[Parameter]
  None
  
[Returned value]
  If this is successfully called, the current display solution
  is returned.
  Otherwise, {0, 0} is returned.

----------------------------------------------------------------------
float cellGcmUtilGetDisplayAspectRatio();
----------------------------------------------------------------------
[Description]
  Obtains the aspect ratio of the current screen.
  
[Parameter]
  None
  
[Returned value]
  If this is successfully called, the aspect ratio of the
  current screen is returned.
  Otherwise, 16.0f / 9.0f is returned.

----------------------------------------------------------------------
Viewport_t cellGcmUtilGetViewportGL(uint32_t surface_height, uint32_t vpX, uint32_t vpY, uint32_t vpWidth, uint32_t vpHeight, float vpZMin, float vpZMax);
Viewport_t cellGcmUtilGetViewport2D(uint16_t width, uint16_t height);
----------------------------------------------------------------------
[Description]
  Obtains the viewport structure, in which a proper value
  for the Viewport is already set.
  The viewport, which is used for 3D scenes, is obtained with
  cellGcmUtilGetViewportGL().
  The viewport, which is used for 2D scenes, is obtained with
  cellGcmUtilGetViewport2D().
  
[Parameter]
  A value relating to Viewport
  
[Returned value]
  Viewport_t is returned accordingly to the situations.


----------------------------------------------------------------------
bool cellGcmUtilLoadShader(const char *fname, Shader_t *shader);
----------------------------------------------------------------------
[Description]
  Loads the shader from the file.
  You can load it in both NV Binary or cbg formats.
  
[Parameter]
  const char *fname:
    - Name of the shader file to load
  
  Shader_t *shader:
    - Pointer to the variable, which stores the loaded shader
  
[Returned value]
  If this is successfully called, true is returned.
  Otherwise, false is returned.


----------------------------------------------------------------------
bool cellGcmUtilLoadShaderFromMemory(const unsigned char *buffer, uint32_t size, Shader_t *shader);
----------------------------------------------------------------------
[Description]
  Loads the shader from the buffer.
  You can load it in both NV Binary or cbg formats.
  
[Parameter]
  const unsigned char *buffer:
    - Address of the buffer of the shader to load
    
  const unsigned char *buffer:
    - Size of the shader to load
    
  Shader_t *shader:
    - Pointer to the variable, which stores the loaded shader
  
[Returned value]
  If this is successfully called, true is returned.
  Otherwise, false is returned.


----------------------------------------------------------------------
void cellGcmUtilDestroyShader(Shader_t *shader);
----------------------------------------------------------------------
[Description]
  Frees the shader, which was loaded with the following functions:
   - cellGcmUtilLoadShader()
   - cellGcmUtilLoadShaderFromMemory()
   
[Parameter]
  Shader_t *shader:
    - Pointer to the shader to free
  
[Returned value]
  None


----------------------------------------------------------------------
bool cellGcmUtilGetVertexUCode(const Shader_t *shader, Memory_t *ucode);
----------------------------------------------------------------------
[Description]
  Obtains the microcode of the vertex shader.
  Normally, a proper value is set to the "ucode" member of the
  "shader" structure, so you do not need to call this
  function explicitly.
   
[Parameter]
  const Shader_t *shader:
    - Target shader
  
  Memory_t *ucode:
    - Memory area to copy the microcode(non-mapped main memory)
  
[Returned value]
  If this is successfully called, true is returned.
  Otherwise, false is returned.


----------------------------------------------------------------------
bool cellGcmUtilGetFragmentUCode(const Shader_t *shader, uint8_t location, Memory_t *ucode);
----------------------------------------------------------------------
[Description]
  Obtains the microcode of the fragment shader.
   
[Parameter]
  const Shader_t *shader:
    - Target shader
    
  uint32_t location:
    - Location of the microcode of the fragment shader
    
  Memory_t *ucode:
    - Memory area of microcode of the fragment shader
  
[Returned value]
  If this is successfully called, true is returned.
  Otherwise, false is returned.


----------------------------------------------------------------------
bool cellGcmUtilCreateTexture(CellGcmTexture *tex, Memory_t *image);
----------------------------------------------------------------------
[Description]
  Creates a texture from the "CellGcmTexture" structure,
  in which the info of the texture you want to create is already set.
  Please specify all the members other than "offset" of the
  "CellGcmTexture" structure.
  If this is successfully called, a necessary buffer is secured
  in the "image" and then assigned to "tex".
  
[Parameter]
  CellGcmTexture *tex:
    - Structure of the texture to create

  Memory_t *image:
    - Memory area of the created texture image

[Returned value]
  If this is successfully called, true is returned.
  Otherwise, false is returned.


----------------------------------------------------------------------
bool cellGcmUtilLoadTexture(const char* fname, uint32_t location, CellGcmTexture *tex, Memory_t *image);
----------------------------------------------------------------------
[Description]
  Loads the gtf texture file and then creates a texture.
  If this is successfully called, a necessary buffer is secured
  in the "image" and then assigned to "tex".
  
[Parameter]
  const char* fname:
    - Name of the gtf texture file to load
    
  uint32_t location:
    - Location of the texture to load
    
  CellGcmTexture *tex:
    - "CellGcmTexture" structure of the created texture

  Memory_t *image:
    - Memory area of the created texture image

[Returned value]
  If this is successfully called, true is returned.
  Otherwise, false is returned.


----------------------------------------------------------------------
uint32_t cellGcmUtilLoadPackedTexture(const char* fname, uint32_t index, uint32_t location, CellGcmTexture *texture, Memory_t *image);
----------------------------------------------------------------------
[Description]
  Loads the gtf texture file, which includes multiple textures,
  and then creates the texture.
  Even when the gtf includes only 1 texture, you can load it.
  If this is successfully called, necessary buffer is secured 
  in the "image" and then assigned to the "tex".
  
[Parameter]
  const char* fname:
    - Name of the gtf texture file to load
    
  uint32_t index:
    - Texture index inside the gtf texture
    
  uint32_t location:
    - Location to load the texture
    
  CellGcmTexture *tex:
    - "CellGcmTexture" structure of the created texture

  Memory_t *image:
    - Memory area of the created texture image

[Returned value]
  If this is successfully called, the number of the textures
  included in the gtf file is returned.
  Otherwise, 0 is returned.


----------------------------------------------------------------------
void cellGcmUtilSetTextureUnit(uint32_t tex_unit, const CellGcmTexture *tex);
----------------------------------------------------------------------
[Description]
  Sets a texture to the texture unit.
  
[Parameter]
  uint32_t tex_unit:
    - Index of the texture sampler(0 - 15)
    
  const CellGcmTexture *tex:
    - "CellGcmTexture" structure of the texture to set
    
[Returned value]
  None


----------------------------------------------------------------------
void cellGcmUtilInvalidateTextureUnit(uint32_t tex_unit);
----------------------------------------------------------------------
[Description]
  Invalidates the texture of the texture unit.
  
[Parameter]
  uint32_t tex_unit:
    - Index of the texture sampler to invalidate(0 - 15)
    
[Returned value]
  None


----------------------------------------------------------------------
uint32_t cellGcmUtilGetTextureRequireSize(const CellGcmTexture tex);
----------------------------------------------------------------------
[Description]
  Returns the necessary memory size for creating textures from the
  "CellGcmTexture" structure, in which the info of the texture
  you want to create is already set.
  Please specify all the members other than "offset" of the
  "CellGcmTexture" structure.
  
[Parameter]
  CellGcmTexture *tex:
    - Structure of the texture you create
    
[Returned value]
  If this is successfully called, necessary size of the memory area
  is returned.
  Otherwise, 0 is returned.


----------------------------------------------------------------------
uint8_t cellGcmUtilGetRawFormat(uint8_t texture_format);
----------------------------------------------------------------------
[Description]
  Returns the pure texture format from the texture format.
  
[Parameter]
  uint8_t texture_format:
    - Texture format
    
[Returned value]
  If this is successfully called, a pure texture format is returned.
  Otherwise, 0 is returned.


----------------------------------------------------------------------
const char* cellGcmUtilGetFormatName(uint8_t texture_format);
----------------------------------------------------------------------
[Description]
  The name of the texture format is returned from the
  texture format.
  
[Parameter]
  uint8_t texture_format:
    - Texture format
    
[Returned value]
  If this is successfully called, the name of the texture format
  is returned.
  Otherwise, "Unknown Format" is returned.


----------------------------------------------------------------------
uint16_t cellGcmUtilStr2Remap(const char *str);
----------------------------------------------------------------------
[Description]
  Creates remap from strings, which stand for "remap".
  The corresponding characters are {R, G, B, A, 0, 1}.
  Normally "ARGB" is specified.
  
[Parameter]
  const char *str:
    - Strings which stand for remap
    
[Returned value]
  If this is successfully called, corresponding remap is returned.
  Otherwise, ARGB is returned.


----------------------------------------------------------------------
const char* cellGcmUtilRemap2Str(uint16_t remap);
----------------------------------------------------------------------
[Description]
  Creates strings from remap, that stand for "remap".
  
[Parameter]
  uint16_t remap:
    - "remap" member of the "CellGcmTexture" structure
    
[Returned value]
  If this is successfully called, a string corresponding to "remap"
  is returned.
  Otherwise, "NNNN" is returned.


----------------------------------------------------------------------
bool cellGcmUtilSaveTexture(const char* fname, CellGcmTexture *texture, Memory_t *image);
----------------------------------------------------------------------
[Description]
  Saves the texture into the file.
  
[Parameter]
  const char* fname:
    - File name of gtf texture to save
    
  CellGcmTexture *texture:
    - "CellGcmTexture" structure of the texture to save

  Memory_t *image:
    - Memory area of the texture image to save

[Returned value]
  If this is successfully called, true is returned.
  Otherwise, false is returned.


----------------------------------------------------------------------
CellGcmTexture cellGcmUtilSurfaceToTexture(const CellGcmSurface *surface, uint32_t index);
----------------------------------------------------------------------
[Description]
  Converts the surface into a texture.
  Note that the buffer to refer will be shared.
  
[Parameter]
  const CellGcmSurface *surface:
    - Surface to convert
    
  uint32_t index:
    - Color buffer index of the surface

[Returned value]
  "CellGcmTexture" structure of the converted texture


----------------------------------------------------------------------
CellGcmTexture cellGcmUtilDepthToTexture(const CellGcmSurface *surface, bool bAsColor);
----------------------------------------------------------------------
[Description]
  Converts the depth buffer of the surface into a texture.
  Note that the buffer to refer will be shared.
  
[Parameter]
  const CellGcmSurface *surface:
    - Surface to convert
    
  bool bAsColor:
    - if converts as a color texture

[Returned value]
  "CellGcmTexture" structure of the converted texture


----------------------------------------------------------------------
CellGcmSurface cellGcmUtilTextureToSurface(const CellGcmTexture *color, const CellGcmTexture *depth);
----------------------------------------------------------------------
[Description]
  Converts the texture into the surface.
  Note that the buffer to refer will be shared.
  
[Parameter]
  const CellGcmTexture *color:
    - Texture to convert as a color buffer
    
  const CellGcmTexture *depth:
    - Texture to convert as a depth buffer

[Returned value]
  "CellGcmSurface" structure of the converted surface


----------------------------------------------------------------------
void cellGcmUtilSimpleCameraInit(Vectormath::Aos::Vector3 eye, Vectormath::Aos::Vector3 at, Vectormath::Aos::Vector3 up);
----------------------------------------------------------------------
[Description]
  Initializes the simple camera for the sample.
  
[Paramter]
  Vectormath::Aos::Vector3 eye:
    - Initial position of the camera
    
  Vectormath::Aos::Vector3 at:
    - Initial gaze point of the camera
    
  Vectormath::Aos::Vector3 up:
    - Initial direction of the up vector of the camera
    
[Returned value]
  None


----------------------------------------------------------------------
void cellGcmUtilSimpleCameraUpdate(void);
----------------------------------------------------------------------
[Description]
  Updates the simple camera for the sample.
  Controller inputs are also done automatically.
  
  <How to Operate>
    Left stick         : Operates the camera position
                         (Forward, Backward, Left, Right)
    Up/Down/Left/Right : Operates the camera position
                         (Up, Down, Left, Right)
    Right stick        : Operates the camera direction
  
[Parameter]
  None
    
[Returned value]
  None


----------------------------------------------------------------------
Vectormath::Aos::Matrix4 cellGcmUtilSimpleCameraGetMatrix(void);
----------------------------------------------------------------------
[Description]
  Obtains the current view matrix from the simple camera
  for the sample.
  
[Parameter]
  None
    
[Returned value]
  Current view matrix


----------------------------------------------------------------------
void cellGcmUtilSimpleCameraGetInitParam(Vectormath::Aos::Vector3 *eye, Vectormath::Aos::Vector3 *at, Vectormath::Aos::Vector3 *up);
----------------------------------------------------------------------
[Description]
  Obtains the initial info of the simple camera for the sample.
  
[Paramter]
  Vectormath::Aos::Vector3 *eye:
    - Pointer to the variable, which stores the initial position
      of the camera
    
  Vectormath::Aos::Vector3 *at:
    - Pointer to the variable, which stores the initial gaze point
      of the camera
    
  Vectormath::Aos::Vector3 *up:
    - Pointer to the variable, which stores the initial direction
      of the up vector of the camera.
    
[Returned value]
  None


----------------------------------------------------------------------
void cellGcmUtilSimpleCameraGetParam(Vectormath::Aos::Vector3 *trans, float *yaw, float *pitch);
----------------------------------------------------------------------
[Description]
  Obtains the current state of simple camera for the sample.
  
[Paramter]
  Vectormath::Aos::Vector3 *trans:
    - Pointer to the variable, which stores the amount of
      the camera movement
    
  float *yaw:
    - Pointer to the variable, which stores the amount of horizontal
      rotation of the camera
    
  float *pitch:
    - Pointer to the variable, which stores the amount of vertical
      rotation of the camera
    
[Returned value]
  None


----------------------------------------------------------------------
void cellGcmUtilSimpleCameraSetParam(Vectormath::Aos::Vector3 trans, float yaw, float pitch);
----------------------------------------------------------------------
[Description]
  Sets the current state of simple camera for the sample.
  
[Paramter]
  Vectormath::Aos::Vector3 trans:
    - Amount of camera movement
    
  float yaw:
    - Amount of horizontal rotation of the camera
    
  float pitch:
    - Amount of vertical rotation of the camera
    
[Returned value]
  None


[EOF]
