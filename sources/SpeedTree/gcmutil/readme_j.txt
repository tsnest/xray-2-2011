[SCE CONFIDENTIAL DOCUMENT]
PlayStation(R)3 Programmer Tool Runtime Library 300.001
                  Copyright (C) 2008 Sony Computer Entertainment Inc.
                                                 All Rights Reserved.

概要：
    gcmutil.aは、libgcmを用いてアプリケーションを開発する際の
    ユーティリティ関数を集めたライブラリです。
    
使用方法：
    1.使用するアプリケーションで gcmutil.h をインクルードする。
    2.使用するアプリケーションで gcmutil.a をリンクする。
    3.アプリケーション開始時に呼び出す関数: cellGcmUtilInit()
    4.アプリケーション終了時に呼び出す関数: cellGcmUtilEnd()

解説：
  gcmutilのAPI, 構造体は基本的に CellGcmUtil 名前空間上に定義されています。
  必要に応じて using namespace CellGcmUtil; を宣言することで容易に使用することが出来ます。
  
注意事項：
  gcmutil_old.h に宣言されている関数はC言語用で、互換性のために残されたものです。
  
リファレンス：
  主なAPIの仕様は以下の通りです。

======================================================================
                             定数と構造体
======================================================================

----------------------------------------------------------------------
頂点アットリビュート定数
----------------------------------------------------------------------
[解説]
  頂点属性のスロット番号(0-15)に対応する定数です。
  cellGcmSetVertexDataArray()などの関数で使用できます。

[詳細]
  CELL_GCMUTIL_ATTR_POSITION      頂点（float4）
  CELL_GCMUTIL_ATTR_BLENDWEIGHT   頂点ウェイト（float）
  CELL_GCMUTIL_ATTR_NORMAL        法線（float3）
  CELL_GCMUTIL_ATTR_COLOR         カラー（float4）
  CELL_GCMUTIL_ATTR_COLOR0        カラー（float4）
  CELL_GCMUTIL_ATTR_DIFFUSE       カラー（float4）
  CELL_GCMUTIL_ATTR_COLOR1        セカンダリカラー（float4）
  CELL_GCMUTIL_ATTR_SPECULAR      セカンダリカラー（float4）
  CELL_GCMUTIL_ATTR_FOGCOORD      フォグ座標（float）
  CELL_GCMUTIL_ATTR_TESSFACTOR    フォグ座標（float）
  CELL_GCMUTIL_ATTR_PSIZE         ポイントサイズ（float）
  CELL_GCMUTIL_ATTR_BLENDINDICES  スキニング用のパレットインデックス（float4）
  CELL_GCMUTIL_ATTR_TEXCOORD0     テクスチャ座標0（float4）
  CELL_GCMUTIL_ATTR_TEXCOORD1     テクスチャ座標1（float4）
  CELL_GCMUTIL_ATTR_TEXCOORD2     テクスチャ座標2（float4）
  CELL_GCMUTIL_ATTR_TEXCOORD3     テクスチャ座標3（float4）
  CELL_GCMUTIL_ATTR_TEXCOORD4     テクスチャ座標4（float4）
  CELL_GCMUTIL_ATTR_TEXCOORD5     テクスチャ座標5（float4）
  CELL_GCMUTIL_ATTR_TEXCOORD6     テクスチャ座標6（float4）
  CELL_GCMUTIL_ATTR_TANGENT       正接ベクトル（float4）
  CELL_GCMUTIL_ATTR_TEXCOORD7     テクスチャ座標7（float4）
  CELL_GCMUTIL_ATTR_BINORMAL      従法線ベクトル（float4）


----------------------------------------------------------------------
Memory_t 構造体
----------------------------------------------------------------------
[解説]
  メインメモリ、ローカルメモリ上のメモリ領域を確保して、
  libgcmやgcmutilから使用する際に使用します。
  
[詳細]
  struct Memory_t{
  	uint32_t offset;
  	void* addr;
  	uint32_t size;
  	uint8_t location;
    uint8_t  _padding0;
    uint16_t _padding1;
  };

[パラメータ]
  uint32_t offset:
    - 確保されているメモリ領域のオフセット
      マップされていないメインメモリ領域の場合は、0xFFFFFFFFが代入されます。
    
  void* addr:
    - 確保されているメモリ領域のアドレス
    
  uint32_t size:
    - 確保されているメモリ領域のサイズ
    
  uint8_t location:
    - 確保されているメモリ領域のロケーション
        メインメモリ  : CELL_GCM_LOCATION_MAIN
        ローカルメモリ: CELL_GCM_LOCATION_LOCAL

  uint8_t  _padding0:
  uint16_t _padding1:
    - 予約領域


----------------------------------------------------------------------
Viewport_t 構造体
----------------------------------------------------------------------
[解説]
  ビューポートの情報をまとめた構造体です。
  cellGcmSetViewport() に値を渡すときに使用します。
  
[詳細]
  struct{
  	uint16_t x, y, width, height;
  	float min, max;
  	float scale[4], offset[4];
  } Viewport_t;


----------------------------------------------------------------------
Shader_t 構造体
----------------------------------------------------------------------
[解説]
  頂点シェーダ、フラグメントシェーダを動的に利用する際に使用します。
  
[詳細]
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
  
[パラメータ]
  CGprogram program:
    - シェーダプログラム（NV Binary 形式の場合）

  void *ucode:
    - シェーダのマイクロコードのアドレス
      頂点シェーダの場合、ロード時に設定されます。

  bool is_vp:
    - シェーダの種類
        頂点シェーダ          : true
        フラグメントシェーダ  : false

  bool is_cgb:
    - シェーダ形式
        cgb 形式       : true
        NV Binary 形式 : false

  CellCgbProgram program_cgb:
  CellCgbVertexProgramConfiguration vertex_program_conf:
  CellCgbFragmentProgramConfiguration fragment_program_conf:
    - シェーダプログラムとシェーダコンフィグ（cgb 形式の場合）

  Memory_t body:
    - シェーダファイルの本体
      常にマップされていないメインメモリ上に確保されます。


----------------------------------------------------------------------
FileList_t 構造体
----------------------------------------------------------------------
[解説]
  ファイルリストを保持する構造体
  cellGcmUtilGetFileList() などのファイルリスト関数で利用します。
  
[詳細]
  struct FileList_t{
  	uint32_t count;
  	uint32_t size;
  	char **files;
  };

[パラメータ]
  uint32_t count:
    - ファイルリストが保持しているファイル数

  uint32_t size:
    - ファイルリストが確保しているバッファ数
      ※基本的に、ユーザが使用することはありません。

  char **files:
    - ファイル名が格納されたバッファへのポインタ配列



======================================================================
                            インライン関数
======================================================================

----------------------------------------------------------------------
inline uint32_t cellGcmUtilGetAlign(uint32_t size, uint32_t alignment)
----------------------------------------------------------------------
[解説]
  アラインメントを計算します。
  
[パラメータ]
  uint32_t size:
    - 元になるサイズ
    
  uint32_t alignment:
    - アラインメント
  
[返り値]
  アラインメントしたサイズを返します。


----------------------------------------------------------------------
inline float cellGcmUtilToRadian(float degree)
----------------------------------------------------------------------
[解説]
  角度をラジアン角に変換します。
  
[パラメータ]
  float degree:
    - 角度
  
[返り値]
  変換したラジアン角を返します。


----------------------------------------------------------------------
inline float cellGcmUtilToDegree(float radian)
----------------------------------------------------------------------
[解説]
  ラジアン角を角度に変換します。
  
[パラメータ]
  float radian:
    - ラジアン角
  
[返り値]
  変換した角度を返します。



======================================================================
                          API リファレンス
======================================================================

----------------------------------------------------------------------
bool cellGcmUtilInitLocal(void* local_base_addr, size_t local_size);
----------------------------------------------------------------------
[解説]
  アプリケーションに割り当てるローカルメモリ領域の情報をgcmutilに設定します。
  ここで設定した領域は、cellGcmUtilAllocateLocal() 関数を用いて
  アプリケーションに割り当てることができます。
  
  アプリケーションから使用可能なローカルメモリの全領域を
  gcmutil経由で割り当てる場合は、cellGcmGetConfiguration()から取得した
  ローカルメモリ情報を指定します。
  
  ※cellGcmUtilInit()を使用する場合は、この関数を呼び出す必要はありません。
  
[パラメータ]
  void* local_base_addr:
    - アプリケーションに割り当てるローカルメモリのベースを示す実効アドレス
    (使用可能なローカルメモリ全体を指定する場合はCellGcmConfig.localAddress)
  
  size_t local_size:
    - アプリケーションに割り当てるローカルメモリのサイズ
    (使用可能なローカルメモリ全体を指定する場合はCellGcmConfig.localSize)
  
[返り値]
  呼び出しが成功した場合は、trueを返します。
  それ以外の場合は、falseを返します。


----------------------------------------------------------------------
bool cellGcmUtilInitMain(void* main_base_addr, size_t main_size);
----------------------------------------------------------------------
[解説]
  gcmutil側から割り当てるメインメモリ領域を設定します。
  ここで設定した領域は、cellGcmUtilAllocateMain() 関数を用いて
  アプリケーションに割り当てることができます。
  
  ※cellGcmUtilInit()を使用する場合は、この関数を呼び出す必要はありません。
  
[パラメータ]
  void* main_base_addr:
    - アプリケーションに割り当てるメインメモリのベースを示す実効アドレス
    (memalign()などを使用して、1MBにアラインされている必要があります)
  
  size_t main_size:
    - アプリケーションに割り当てるメインメモリのサイズ
    (1MBにアラインされている必要があります)
  
[返り値]
  呼び出しが成功した場合は、trueを返します。
  それ以外の場合は、falseを返します。


----------------------------------------------------------------------
bool cellGcmUtilAllocateLocal(uint32_t size, uint32_t alignment, Memory_t *memory);
----------------------------------------------------------------------
[解説]
  アプリケーションで使用するローカルメモリ領域を割り当てます。
  
[パラメータ]
  uint32_t size:
    - アプリケーションで使用するローカルメモリのサイズ
  
  uint32_t alignment:
    - アラインするサイズ
  
  Memory_t *memory:
    - 確保されたメモリ領域
  
[返り値]
  呼び出しが成功した場合は、trueを返します。
  それ以外の場合は、falseを返します。


----------------------------------------------------------------------
bool cellGcmUtilAllocateMain(uint32_t size, uint32_t alignment, Memory_t *memory);
----------------------------------------------------------------------
[解説]
  アプリケーションで使用するメインメモリ領域を割り当てます。
  
[パラメータ]
  uint32_t size:
    - アプリケーションで使用するメインメモリのサイズ
  
  uint32_t alignment:
    - アラインするサイズ
  
  Memory_t *memory:
    - 確保されたメモリ領域
  
[返り値]
  呼び出しが成功した場合は、trueを返します。
  それ以外の場合は、falseを返します。


----------------------------------------------------------------------
bool cellGcmUtilAllocate(uint32_t size, uint32_t alignment, uint8_t location, Memory_t *memory);
----------------------------------------------------------------------
[解説]
  アプリケーションで使用するメモリ領域を割り当てます。
  
[パラメータ]
  uint32_t size:
    - アプリケーションで使用するメインメモリのサイズ
  
  uint32_t alignment:
    - アラインするサイズ
    
  uint8_t location:
    - 確保するメモリ領域のロケーション
        メインメモリ  : CELL_GCM_LOCATION_MAIN
        ローカルメモリ: CELL_GCM_LOCATION_LOCAL
  
  Memory_t *memory:
    - 確保されたメモリ領域
  
[返り値]
  呼び出しが成功した場合は、trueを返します。
  それ以外の場合は、falseを返します。


----------------------------------------------------------------------
bool cellGcmUtilReallocate(Memory_t *memory, uint8_t location, uint32_t alignment);
----------------------------------------------------------------------
[解説]
  確保済みのメモリ領域をロケーション、アラインメントを変えて再確保します。
  呼び出しが成功すると、再確保した領域でmemoryが上書きされます。
  また、データは再確保した領域にコピーされます。
  呼び出しに失敗した場合は、memoryを解放しません。
  
[パラメータ]
  Memory_t *memory:
    - 対象のメモリ領域
  
  uint32_t alignment:
    - アラインするサイズ
    
  uint8_t location:
    - 確保するメモリ領域のロケーション
        メインメモリ  : CELL_GCM_LOCATION_MAIN
        ローカルメモリ: CELL_GCM_LOCATION_LOCAL
  
[返り値]
  呼び出しが成功した場合は、trueを返します。
  それ以外の場合は、falseを返します。


----------------------------------------------------------------------
bool cellGcmUtilAllocateUnmappedMain(uint32_t size, uint32_t alignment, Memory_t *memory);
----------------------------------------------------------------------
[解説]
  アプリケーションで使用するメモリ領域を割り当てます。
  この関数で確保した領域は、RSXから使用することは出来ません。
  Memory_t構造体のoffsetメンバは、0xFFFFFFFFに設定されます。
  
[パラメータ]
  uint32_t size:
    - アプリケーションで使用するメインメモリのサイズ
  
  uint32_t alignment:
    - アラインするサイズ
    
  Memory_t *memory:
    - 確保されたメモリ領域
  
[返り値]
  呼び出しが成功した場合は、trueを返します。
  それ以外の場合は、falseを返します。


----------------------------------------------------------------------
void cellGcmUtilFree(Memory_t *memory);
----------------------------------------------------------------------
[解説]
  下記の関数でアプリケーションに割り当てたメモリ領域を解放します。
    - bool cellGcmUtilAllocateLocal()
    - bool cellGcmUtilAllocateMain()
    - bool cellGcmUtilAllocate()
    - bool cellGcmUtilReallocate()
    - bool cellGcmUtilAllocateUnmappedMain()

[パラメータ]
  Memory_t *memory:
    - 解放するメモリ領域
  
[返り値]
  なし


----------------------------------------------------------------------
void cellGcmUtilFinalizeMemory(void);
----------------------------------------------------------------------
[解説]
  gcmutilの終了処理を行います。
  主に、下記の関数を使用して起きたメモリリークをチェックします。
    - bool cellGcmUtilAllocateLocal()
    - bool cellGcmUtilAllocateMain()
    - bool cellGcmUtilAllocate()
    - bool cellGcmUtilReallocate()
    - bool cellGcmUtilAllocateUnmappedMain()

[パラメータ]
  なし
  
[返り値]
  なし


----------------------------------------------------------------------
bool cellGcmUtilReadFile(const char *fname, uint8_t **buffer, uint32_t *buf_size, uint32_t offset = 0, uint32_t size = 0xFFFFFFFFUL);
----------------------------------------------------------------------
[解説]
  指定されたファイルをロードします。
  バッファは自動的に確保されるので、delete してください。
  
[パラメータ]
  const char *fname:
    - ロードするファイル名
  
  uint8_t **buffer:
    - ファイルデータをロードしたアドレスが格納される変数へのポインタ
    
  uint32_t *buf_size:
    - ロードしたファイルサイズが格納される変数へのポインタ
  
  uint32_t offset:
    - ファイルのロード開始位置（指定しない場合は、ファイルの先頭）
    
  uint32_t size:
    - ロードするサイズ（指定しない場合は、ファイルサイズ）
    
[返り値]
  呼び出しが成功した場合は、trueを返します。
  それ以外の場合は、falseを返します。


----------------------------------------------------------------------
bool cellGcmUtilWriteFile(const char *fname, uint8_t *buffer, uint32_t buf_size);
----------------------------------------------------------------------
[解説]
  バッファの内容をファイルに保存します。
  
[パラメータ]
  const char *fname:
    - 保存するファイル名
  
  uint8_t *buffer:
    - 保存するデータのアドレス
    
  uint32_t buf_size:
    - 保存するデータのサイズ
    
[返り値]
  呼び出しが成功した場合は、trueを返します。
  それ以外の場合は、falseを返します。


----------------------------------------------------------------------
bool cellGcmUtilIsFileExsits(const char *fname);
----------------------------------------------------------------------
[解説]
  ファイルが存在するかどうかを確認します。
  
[パラメータ]
  const char *fname:
    - ファイル名
    
[返り値]
  ファイルが存在する場合は、trueを返します。
  それ以外の場合は、falseを返します。


----------------------------------------------------------------------
uint32_t cellGcmUtilGetFileList(const char *path, const char *ext, FileList_t *filelist);
----------------------------------------------------------------------
[解説]
  指定されたディレクトリに存在するファイル一覧を返します。
  サブディレクトリの検索は行いません。
  filelist を変えずに呼び出すことで、ファイル一覧にファイルを追加していくことが出来ます。
  複数の拡張子を検索する場合に利用してください。
  
[パラメータ]
  const char *path:
    - 検索するパス名
  
  const char *ext:
    - 検索するファイルの拡張子
    
  FileList_t *filelist:
    - ファイルリストを格納する変数へのポインタ
    
[返り値]
  呼び出しが成功した場合は、trueを返します。
  それ以外の場合は、falseを返します。


----------------------------------------------------------------------
void cellGcmUtilFreeFileList(FileList_t *filelist);
----------------------------------------------------------------------
[解説]
  ファイルリストを解放します。
  
[パラメータ]
  FileList_t *filelist:
    - 解放するファイルリストへのポインタ
    
[返り値]
  なし

----------------------------------------------------------------------
bool cellGcmUtilSetDisplayBuffer(uint8_t number, CellGcmTexture *texture_array);
----------------------------------------------------------------------
[解説]
  指定したテクスチャをディスプレイ出力バッファとして登録します。

[パラメータ]
  uint8_t number:
    - 指定するテクスチャの数
  
  CellGcmTexture *texture_array:
    - 指定するテクスチャのテクスチャ情報の配列

[返り値]
  呼び出しが成功した場合は、trueを返します。
  それ以外の場合は、falseを返します。


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
[解説]
  カラーバッファ、深度バッファの初期化を行います。
  また、確保した領域に関連付けられたサーフェスを返します。  
  
[パラメータ]
  uint8_t color_format:
    - カラーバッファのフォーマット
  
  uint8_t depth_format:
    - 深度バッファのフォーマット
    
  uint8_t buffer_number:
    - バッファリング数
      通常は、ダブルバッファかトリプルバッファを指定します。
    
  Memory_t *buffer:
    - 確保されたバッファのメモリ領域を (buffer_number + 1) 個返します。
      +1 個分は、デプスバッファとなります。
  
  CellGcmSurface *surface:
    - 確保されたメモリ領域に関連付けられたサーフェスを buffer_number 個返します
    
[返り値]
  呼び出しが成功した場合は、trueを返します。
  それ以外の場合は、falseを返します。


----------------------------------------------------------------------
bool cellGcmUtilDbgfontInit(int32_t word_count, void* addr, uint32_t size, uint8_t location);
----------------------------------------------------------------------
[解説]
  dbgfontを初期化します。
  
[パラメータ]
  int32_t word_count:
    - dbgfontで表示したい文字数
  
  void* addr:
    - dbgfontで使用するメモリのアドレス
      関数を呼ぶ前に確保しておく必要があります。
    
  uint32_t size:
    - dbgfontで使用するメモリのサイズ
      必要なサイズは、cellGcmUtilDbgfontGetRequireSize()で取得してください。
    
  uint8_t location:
    - dbgfontで使用するメモリのロケーション
    
[返り値]
  呼び出しが成功した場合は、trueを返します。
  それ以外の場合は、falseを返します。


----------------------------------------------------------------------
void cellGcmUtilDbgfontEnd(void);
----------------------------------------------------------------------
[解説]
  dbgfontを終了します。
  
[パラメータ]
  なし
  
[返り値]
  なし


----------------------------------------------------------------------
uint32_t cellGcmUtilDbgfontGetRequireSize(int32_t word_count);
----------------------------------------------------------------------
[解説]
  dbgfontで必要なメモリサイズを返します。
  
[パラメータ]
  int32_t word_count:
    - dbgfontで表示したい文字数
    
[返り値]
  呼び出しが成功した場合は、dbgfontで必要なメモリサイズを返します。
  それ以外の場合は、0を返します。


----------------------------------------------------------------------
void cellGcmUtilPuts(const char* str);
----------------------------------------------------------------------
[解説]
  dbgfontの現在位置に文字列を表示します。
  改行も適切に処理されます。
  
[パラメータ]
  const char* str:
    - 表示する文字列
  
[返り値]
  なし


----------------------------------------------------------------------
void cellGcmUtilPrintf(const char* format, ...);
----------------------------------------------------------------------
[解説]
  dbgfontの現在位置に書式付文字列を表示します。
  改行も適切に処理されます。
  
[パラメータ]
  const char* format, ...:
    - 書式付文字列
  
[返り値]
  なし


----------------------------------------------------------------------
void cellGcmUtilPrintfColor(uint32_t color, const char* format, ...);
----------------------------------------------------------------------
[解説]
  dbgfontの現在位置に色を指定して書式付文字列を表示します。
  改行も適切に処理されます。
  
[パラメータ]
  uint32_t color:
    - 文字列の色 A8R8G8B8
    
  const char* format, ...:
    - 書式付文字列
  
[返り値]
  なし


----------------------------------------------------------------------
void cellGcmUtilSetPrintPos(float x, float y);
----------------------------------------------------------------------
[解説]
  dbgfontの現在位置を指定します。
  CELL_GCMUTIL_POSFIX が指定された軸の座標は変更されません。
  
[パラメータ]
  float x, y:
    - 位置の座標
      画面の左上 (0.0f, 0.0f)
      画面の右下 (1.0f, 1.0f)

[返り値]
  なし


----------------------------------------------------------------------
void cellGcmUtilSetPrintSize(float size);
----------------------------------------------------------------------
[解説]
  dbgfontの文字サイズを指定します。
  
[パラメータ]
  float size:
    - 文字サイズ
      1.0f: 画面全体を80x32に分割したサイズ

[返り値]
  なし


----------------------------------------------------------------------
void cellGcmUtilSetPrintColor(uint32_t color);
----------------------------------------------------------------------
[解説]
  dbgfontの文字色を指定します。
  
[パラメータ]
  uint32_t color:
    - 文字列の色 A8R8G8B8

[返り値]
  なし


----------------------------------------------------------------------
void cellGcmUtilGetPrintPos(float *x, float *y);
----------------------------------------------------------------------
[解説]
  dbgfontの現在位置を取得します。
  
[パラメータ]
  float *x, *y:
    - 現在位置の座標を格納する変数へのポインタ

[返り値]
  なし


----------------------------------------------------------------------
bool cellGcmUtilInit(int32_t cb_size, int32_t main_size);
----------------------------------------------------------------------
[解説]
  libgcmとgcmutilをまとめて初期化します。
  
[パラメータ]
  int32_t cb_size:
    - デフォルトコマンドバッファのサイズ
  
  int32_t main_size:
    - RSXから使用するメインメモリのサイズ
  
[返り値]
  呼び出しが成功した場合は、trueを返します。
  それ以外の場合は、falseを返します。


----------------------------------------------------------------------
void cellGcmUtilEnd();
----------------------------------------------------------------------
[解説]
  gcmutilの終了処理を行います。
  内部では、cellGcmUtilFinalizeMemory()が呼ばれます。
  
[パラメータ]
  なし
  
[返り値]
  なし


----------------------------------------------------------------------
CellVideoOutResolution cellGcmUtilGetResolution(void);
----------------------------------------------------------------------
[解説]
  現在のディスプレイ解像度を取得します。
  
[パラメータ]
  なし
  
[返り値]
  呼び出しが成功した場合は、現在のディスプレイ解像度を返します。
  それ以外の場合は、{0, 0}を返します。


----------------------------------------------------------------------
float cellGcmUtilGetDisplayAspectRatio();
----------------------------------------------------------------------
[解説]
  現在の画面アスペクト比を取得します。
  
[パラメータ]
  なし
  
[返り値]
  呼び出しが成功した場合は、現在の画面アスペクト比を返します。
  それ以外の場合は、16.0f / 9.0f を返します。


----------------------------------------------------------------------
Viewport_t cellGcmUtilGetViewportGL(uint32_t surface_height, uint32_t vpX, uint32_t vpY, uint32_t vpWidth, uint32_t vpHeight, float vpZMin, float vpZMax);
Viewport_t cellGcmUtilGetViewport2D(uint16_t width, uint16_t height);
----------------------------------------------------------------------
[解説]
  Viewportにセットするのに適した値が設定されたビューポート構造体を取得します。
  3Dシーンで使用するビューポートは cellGcmUtilGetViewportGL() で取得します。
  2Dシーンで使用するビューポートは cellGcmUtilGetViewport2D() で取得します。
  
[パラメータ]
  Viewportに関する値
  
[返り値]
  それぞれの状況に応じた Viewport_t を返します。


----------------------------------------------------------------------
bool cellGcmUtilOffsetToAddress(uint8_t location, uint32_t offset, void **address);
----------------------------------------------------------------------
[解説]
  オフセット値から実効アドレスを取得します。
  
[パラメータ]
  uint8_t location:
    - 調べたいメモリ領域のロケーション
        メインメモリ  : CELL_GCM_LOCATION_MAIN
        ローカルメモリ: CELL_GCM_LOCATION_LOCAL
  
  uint32_t offset:
    - 調べたいメモリ領域のオフセット
      マップされていないメインメモリ領域の場合は、0xFFFFFFFFが代入されます。
    
  void** address:
    - 調べたいメモリ領域のアドレスを格納するポインタ

[返り値]
  呼び出しが成功した場合は、trueを返します。
  それ以外の場合は、falseを返します。


----------------------------------------------------------------------
bool cellGcmUtilLoadShader(const char *fname, Shader_t *shader);
----------------------------------------------------------------------
[解説]
  シェーダをファイルからロードします。
  NV Binary, cbg 形式どちらでもロードすることが出来ます。
  
[パラメータ]
  const char *fname:
    - ロードするシェーダファイル名
  
  Shader_t *shader:
    - ロードしたシェーダが格納される変数へのポインタ
  
[返り値]
  呼び出しが成功した場合は、trueを返します。
  それ以外の場合は、falseを返します。


----------------------------------------------------------------------
bool cellGcmUtilLoadShaderFromMemory(const unsigned char *buffer, uint32_t size, Shader_t *shader);
----------------------------------------------------------------------
[解説]
  シェーダをバッファからロードします。
  NV Binary, cbg 形式どちらでもロードすることが出来ます。
  
[パラメータ]
  const unsigned char *buffer:
    - ロードするシェーダのバッファのアドレス
    
  const unsigned char *buffer:
    - ロードするシェーダのサイズ
    
  Shader_t *shader:
    - ロードしたシェーダが格納される変数へのポインタ
  
[返り値]
  呼び出しが成功した場合は、trueを返します。
  それ以外の場合は、falseを返します。


----------------------------------------------------------------------
void cellGcmUtilDestroyShader(Shader_t *shader);
----------------------------------------------------------------------
[解説]
  下記の関数でロードしたシェーダを解放します。
   - cellGcmUtilLoadShader()
   - cellGcmUtilLoadShaderFromMemory()
   
[パラメータ]
  Shader_t *shader:
    - 解放するシェーダへのポインタ
  
[返り値]
  なし


----------------------------------------------------------------------
bool cellGcmUtilGetVertexUCode(const Shader_t *shader, Memory_t *ucode);
----------------------------------------------------------------------
[解説]
  頂点シェーダのマイクロコードを取得します。
  通常、shader構造体のucodeメンバに適切な値が設定されているので、
  本関数を明示的に呼ぶ必要はありません。
   
[パラメータ]
  const Shader_t *shader:
    - 対象のシェーダ
  
  Memory_t *ucode:
    - マイクロコードがコピーされるメモリ領域（非マップメインメモリ）
  
[返り値]
  呼び出しが成功した場合は、trueを返します。
  それ以外の場合は、falseを返します。


----------------------------------------------------------------------
bool cellGcmUtilGetFragmentUCode(const Shader_t *shader, uint8_t location, Memory_t *ucode);
----------------------------------------------------------------------
[解説]
  フラグメントシェーダのマイクロコードを取得します。
   
[パラメータ]
  const Shader_t *shader:
    - 対象のシェーダ
    
  uint32_t location:
    - フラグメントシェーダのマイクロコードのロケーション
    
  Memory_t *ucode:
    - フラグメントシェーダのマイクロコードのメモリ領域
  
[返り値]
  呼び出しが成功した場合は、trueを返します。
  それ以外の場合は、falseを返します。


----------------------------------------------------------------------
bool cellGcmUtilCreateTexture(CellGcmTexture *texture, Memory_t *image);
----------------------------------------------------------------------
[解説]
  作成したいテクスチャの情報が設定されたCellGcmTexture構造体からテクスチャを作成します。
  CellGcmTexture構造体のoffsetメンバ以外は、全て埋めてください。
  呼び出しが成功すると、必要なバッファをimageに確保して、textureに関連付けます。
  
[パラメータ]
  CellGcmTexture *texture:
    - 作成するテクスチャの構造体

  Memory_t *image:
    - 作成されたテクスチャイメージのメモリ領域

[返り値]
  呼び出しが成功した場合は、trueを返します。
  それ以外の場合は、falseを返します。


----------------------------------------------------------------------
bool cellGcmUtilCreateSimpleTexture(
    uint32_t width, 
    uint32_t height, 
    uint8_t location, 
    CellGcmTexture *texture, 
    Memory_t *image
);
----------------------------------------------------------------------

[解説]
  A8R8G8B8フォーマットのリニアテクスチャを作成します。
  また、mipmap=1, depth=1に設定されます。
  
[パラメータ]
  uint32_t width:
    - テクスチャの幅
  
  uint32_t height:
    - テクスチャの高さ
  
  uint8_t location:
    - テクスチャを作成するロケーション
  
  CellGcmTexture *texture:
    - 作成したテクスチャの情報を格納するポインタ
  
  Memory_t *image:
    - 作成したテクスチャのメモリ領域を格納するポインタ
  
[返り値]
  呼び出しが成功した場合は、trueを返します。
  それ以外の場合は、falseを返します。


----------------------------------------------------------------------
bool cellGcmUtilLoadTexture(const char* fname, uint32_t location, CellGcmTexture *tex, Memory_t *image);
----------------------------------------------------------------------
[解説]
  gtfテクスチャファイルをロードしてテクスチャを作成します。
  呼び出しが成功すると、必要なバッファをimageに確保して、texに関連付けます。
  
[パラメータ]
  const char* fname:
    - ロードするgtfテクスチャファイル名
    
  uint32_t location:
    - テクスチャをロードするロケーション
    
  CellGcmTexture *tex:
    - 作成されたテクスチャのCellGcmTexture構造体

  Memory_t *image:
    - 作成されたテクスチャイメージのメモリ領域

[返り値]
  呼び出しが成功した場合は、trueを返します。
  それ以外の場合は、falseを返します。


----------------------------------------------------------------------
uint32_t cellGcmUtilLoadPackedTexture(const char* fname, uint32_t index, uint32_t location, CellGcmTexture *texture, Memory_t *image);
----------------------------------------------------------------------
[解説]
  複数テクスチャを含むgtfテクスチャファイルをロードしてテクスチャを作成します。
  単一のテクスチャしか含まないgtfもロードすることが出来ます。
  呼び出しが成功すると、必要なバッファをimageに確保して、texに関連付けます。
  
[パラメータ]
  const char* fname:
    - ロードするgtfテクスチャファイル名
    
  uint32_t index:
    - gtfテクスチャ内でのテクスチャインデックス
    
  uint32_t location:
    - テクスチャをロードするロケーション
    
  CellGcmTexture *tex:
    - 作成されたテクスチャのCellGcmTexture構造体

  Memory_t *image:
    - 作成されたテクスチャイメージのメモリ領域

[返り値]
  呼び出しが成功した場合は、gtfファイルに含まれているテクスチャ数を返します。
  それ以外の場合は、0を返します。


----------------------------------------------------------------------
void cellGcmUtilSetTextureUnit(uint32_t tex_unit, const CellGcmTexture *tex);
----------------------------------------------------------------------
[解説]
  テクスチャユニットにテクスチャをセットします。
  
[パラメータ]
  uint32_t tex_unit:
    - テクスチャサンプラのインデックス(0 - 15)
    
  const CellGcmTexture *tex:
    - セットするテクスチャのCellGcmTexture構造体
    
[返り値]
  なし


----------------------------------------------------------------------
void cellGcmUtilInvalidateTextureUnit(uint32_t tex_unit);
----------------------------------------------------------------------
[解説]
  テクスチャユニットのテクスチャを無効にします。
  
[パラメータ]
  uint32_t tex_unit:
    - 無効にするテクスチャサンプラのインデックス(0 - 15)
    
[返り値]
  なし


----------------------------------------------------------------------
uint32_t cellGcmUtilGetTextureRequireSize(const CellGcmTexture tex);
----------------------------------------------------------------------
[解説]
  作成したいテクスチャの情報が設定されたCellGcmTexture構造体から
  テクスチャを作成するのに必要なメモリ領域のサイズを返します。
  CellGcmTexture構造体のoffsetメンバ以外は、全て埋めてください。
  
[パラメータ]
  CellGcmTexture *tex:
    - 作成するテクスチャの構造体
    
[返り値]
  呼び出しが成功した場合は、必要なメモリ領域のサイズを返します。
  それ以外の場合は、0を返します。


----------------------------------------------------------------------
uint8_t cellGcmUtilGetRawFormat(uint8_t texture_format);
----------------------------------------------------------------------
[解説]
  テクスチャフォーマットから純粋なテクスチャ形式を返します。
  
[パラメータ]
  uint8_t texture_format:
    - テクスチャフォーマット
    
[返り値]
  呼び出しが成功した場合は、純粋なテクスチャ形式を返します。
  それ以外の場合は、0を返します。


----------------------------------------------------------------------
const char* cellGcmUtilGetFormatName(uint8_t texture_format);
----------------------------------------------------------------------
[解説]
  テクスチャフォーマットからテクスチャ形式名を返します。
  
[パラメータ]
  uint8_t texture_format:
    - テクスチャフォーマット
    
[返り値]
  呼び出しが成功した場合は、テクスチャ形式名を返します。
  それ以外の場合は、"Unknown Format" を返します。


----------------------------------------------------------------------
uint16_t cellGcmUtilStr2Remap(const char *str);
----------------------------------------------------------------------
[解説]
  リマップをあらわす文字列から、remapを作成します。
  対応している文字は、{R, G, B, A, 0, 1}になります。
  通常は、"ARGB" を指定します。
  
[パラメータ]
  const char *str:
    - リマップをあらわす文字列
    
[返り値]
  呼び出しが成功した場合は、対応するremapを返します。
  それ以外の場合は、ARGB を返します。


----------------------------------------------------------------------
const char* cellGcmUtilRemap2Str(uint16_t remap);
----------------------------------------------------------------------
[解説]
  remapからリマップをあらわす文字列を作成します。
  
[パラメータ]
  uint16_t remap:
    - CellGcmTexture構造体のremapメンバ
    
[返り値]
  呼び出しが成功した場合は、remapに対応する文字列を返します。
  それ以外の場合は、"NNNN" を返します。


----------------------------------------------------------------------
bool cellGcmUtilSaveTexture(const char* fname, CellGcmTexture *texture, Memory_t *image);
----------------------------------------------------------------------
[解説]
  テクスチャをファイルに保存します。
  
[パラメータ]
  const char* fname:
    - 保存するgtfテクスチャファイル名
    
  CellGcmTexture *texture:
    - 保存するテクスチャのCellGcmTexture構造体

  Memory_t *image:
    - 保存するテクスチャイメージのメモリ領域

[返り値]
  呼び出しが成功した場合は、trueを返します。
  それ以外の場合は、falseを返します。


----------------------------------------------------------------------
CellGcmTexture cellGcmUtilSurfaceToTexture(const CellGcmSurface *surface, uint32_t index);
----------------------------------------------------------------------
[解説]
  サーフェスのカラーバッファをテクスチャに変換します。
  参照するバッファは共有されますので注意してください。
  
[パラメータ]
  const CellGcmSurface *surface:
    - 変換するサーフェス
    
  uint32_t index:
    - サーフェスのカラーバッファインデックス

[返り値]
  変換されたテクスチャのCellGcmTexture構造体


----------------------------------------------------------------------
CellGcmTexture cellGcmUtilDepthToTexture(const CellGcmSurface *surface, bool bAsColor);
----------------------------------------------------------------------
[解説]
  サーフェスの深度バッファをテクスチャに変換します。
  参照するバッファは共有されますので注意してください。
  
[パラメータ]
  const CellGcmSurface *surface:
    - 変換するサーフェス
    
  bool bAsColor:
    - カラーテクスチャとして変換するかどうか

[返り値]
  変換されたテクスチャのCellGcmTexture構造体


----------------------------------------------------------------------
CellGcmSurface cellGcmUtilTextureToSurface(const CellGcmTexture *color, const CellGcmTexture *depth);
----------------------------------------------------------------------
[解説]
  テクスチャをサーフェスに変換します。
  参照するバッファは共有されますので注意してください。
  
[パラメータ]
  const CellGcmTexture *color:
    - カラーバッファとして変換するテクスチャ
    
  const CellGcmTexture *depth:
    - 深度バッファとして変換するテクスチャ

[返り値]
  変換されたサーフェスののCellGcmSurface構造体


----------------------------------------------------------------------
bool cellGcmUtilCreateTiledTexture(
    uint32_t width, 
    uint32_t height, 
    uint8_t format, 
    uint8_t location, 
    uint32_t comp_mode, 
    uint32_t number, 
    CellGcmTexture *texture_array, 
    Memory_t *buffer
);
----------------------------------------------------------------------
[解説]
  Tiled領域に複数枚のテクスチャを作成します。
  
[パラメータ]
  uint32_t width:
    - テクスチャの幅
  
  uint32_t height:
    - テクスチャの高さ
  
  uint8_t format:
    - テクスチャフォーマット
      リニアフォーマット(CELL_GCM_TEXTURE_LN)である必要があります。
  
  uint8_t location:
    - テクスチャを作成するロケーション
  
  uint32_t comp_mode:
    - 設定するTiled領域の圧縮モード
  
  uint32_t number:
    - 同じTiled領域に設定するテクスチャの数
  
  CellGcmTexture *texture_array:
    - 同じTiled領域に設定するテクスチャ情報を格納する配列
  
  Memory_t *buffer:
    - 確保したテクスチャのメモリ領域を格納するポインタ
  
[返り値]
  呼び出しが成功した場合は、trueを返します。
  それ以外の場合は、falseを返します。


----------------------------------------------------------------------
bool cellGcmUtilCreateRenderTexture(
    uint32_t width, 
    uint32_t height, 
    uint8_t format, 
    uint8_t location, 
    uint32_t comp_mode, 
    CellGcmTexture *texture, 
    Memory_t *image
);
----------------------------------------------------------------------
[解説]
  Tiled領域にテクスチャを作成します。
  
[パラメータ]
  uint32_t width:
    - テクスチャの幅
  
  uint32_t height:
    - テクスチャの高さ
  
  uint8_t format:
    - テクスチャフォーマット
      リニアフォーマット(CELL_GCM_TEXTURE_LN)である必要があります。
  
  uint8_t location:
    - テクスチャを作成するロケーション
  
  uint32_t comp_mode:
    - 設定するTiled領域の圧縮モード
  
  CellGcmTexture *texture:
    - 作成したテクスチャの情報を格納するポインタ
  
  Memory_t *image:
    - 作成したテクスチャのメモリ領域を格納するポインタ
  
[返り値]
  呼び出しが成功した場合は、trueを返します。
  それ以外の場合は、falseを返します。


----------------------------------------------------------------------
bool cellGcmUtilCreateDepthTexture(
    uint32_t width, 
    uint32_t height, 
    uint8_t format, 
    uint8_t location, 
    uint32_t comp_mode, 
    bool bZCull,
    bool bTile,
    CellGcmTexture *texture, 
    Memory_t *image
);
----------------------------------------------------------------------
[解説]
  Tiled領域にZCullメモリにバインドされたテクスチャを作成します。
  
[パラメータ]
  uint32_t width:
    - テクスチャの幅
  
  uint32_t height:
    - テクスチャの高さ
  
  uint8_t format:
    - テクスチャフォーマット
      リニアフォーマット(CELL_GCM_TEXTURE_LN)である必要があります。
  
  uint8_t location:
    - テクスチャを作成するロケーション
  
  uint32_t comp_mode:
    - 設定するTiled領域の圧縮モード
  
  bool bZCull:
    - zcull設定を有効にするか
  
  bool bTile:
    - tile設定を有効にするか
  
  CellGcmTexture *texture:
    - 作成したテクスチャの情報を格納するポインタ
  
  Memory_t *image:
    - 作成したテクスチャのメモリ領域を格納するポインタ
  
[返り値]
  呼び出しが成功した場合は、trueを返します。
  それ以外の場合は、falseを返します。


----------------------------------------------------------------------
uint8_t cellGcmUtilBindTile(CellGcmTexture *texture, uint32_t comp_mode);
----------------------------------------------------------------------
[解説]
  テクスチャをTiled領域に設定します。
  ただし、pitchやalignなど全ての条件を満たしている必要があります。
  
[パラメータ]
  CellGcmTexture *texture:
    - Tiled領域に設定するテクスチャの情報
  
  uint32_t comp_mode:
    - 設定するTiled領域の圧縮モード

[返り値]
  呼び出しが成功した場合は、設定したTiled領域のインデックスを返します。
  それ以外の場合は、TILE_MAX(=15)を返します。


----------------------------------------------------------------------
uint8_t cellGcmUtilBindSharedTile(CellGcmTexture *texture, uint32_t comp_mode, uint32_t size);
----------------------------------------------------------------------
[解説]
  複数テクスチャをテクスチャをTiled領域に設定します。
  これらのテクスチャは、cellGcmUtilCreateTiledTexture()で作成されている必要があります。
  
[パラメータ]
  CellGcmTexture *texture:
    - Tiled領域に設定する最初のテクスチャの情報
  
  uint32_t comp_mode:
    - 設定するTiled領域の圧縮モード

  uint32_t comp_mode:
    - 設定するTiled領域のサイズ

[返り値]
  呼び出しが成功した場合は、設定したTiled領域のインデックスを返します。
  それ以外の場合は、TILE_MAX(=15)を返します。


----------------------------------------------------------------------
uint32_t cellGcmUtilBindZCull(CellGcmTexture *texture);
----------------------------------------------------------------------
[解説]
  テクスチャにZcullメモリをバインドします。
  ただし、pitchやalignなど全ての条件を満たしている必要があります。
  
[パラメータ]
  CellGcmTexture *texture:
    - Zcullメモリをバインドするテクスチャの情報

[返り値]
  呼び出しが成功した場合は、設定したZCullメモリのバインドスロットのインデックスを返します。
  それ以外の場合は、ZCULL_MAX(=8)を返します。


----------------------------------------------------------------------
void cellGcmUtilUnbindTile(CellGcmTexture *texture);
----------------------------------------------------------------------
[解説]
  テクスチャをTiled領域から除外します。
  
[パラメータ]
  CellGcmTexture *texture:
    - Tiled領域に設定されたテクスチャの情報

[返り値]
  なし


----------------------------------------------------------------------
void cellGcmUtilUnbindZCull(CellGcmTexture *texture);
----------------------------------------------------------------------
[解説]
  テクスチャをZCullメモリからアンバインドします。
  
[パラメータ]
  CellGcmTexture *texture:
    - ZCullメモリにバインドされたテクスチャの情報

[返り値]
  なし


----------------------------------------------------------------------
void cellGcmUtilSimpleCameraInit(Vectormath::Aos::Vector3 eye, Vectormath::Aos::Vector3 at, Vectormath::Aos::Vector3 up);
----------------------------------------------------------------------
[解説]
  サンプル用の簡易カメラを初期化します。
  
[パラメータ]
  Vectormath::Aos::Vector3 eye:
    - カメラの初期位置
    
  Vectormath::Aos::Vector3 at:
    - カメラの初期注視点
    
  Vectormath::Aos::Vector3 up:
    - カメラの初期の上方向ベクトル
    
[返り値]
  なし


----------------------------------------------------------------------
void cellGcmUtilSimpleCameraUpdate(void);
----------------------------------------------------------------------
[解説]
  サンプル用の簡易カメラを更新します。
  コントローラ入力も自動で処理します。
  
  <操作方法>
    左スティック  : カメラ位置の操作（前後左右）
    上/下/左/右   : カメラ位置の操作（上下左右）
    右スティック  : カメラ方向の操作
  
[パラメータ]
  なし
    
[返り値]
  なし


----------------------------------------------------------------------
void cellGcmUtilSimpleCameraRotate(CellPadUtilAxis angle, bool bReverseX, bool bReverseY, bool bReset);
----------------------------------------------------------------------
[解説]
  サンプル用の簡易カメラを引数に従い回転させます。
  
[パラメータ]
  CellPadUtilAxis angle:
    - アナログスティックなどの入力
  
  bool bReverseX:
    - 左右方向を反転するか
  
  bool bReverseY:
    - 上下方向を反転するか
  
  bool bReset:
    - 回転を初期化するか
    
[返り値]
  なし


----------------------------------------------------------------------
void cellGcmUtilSimpleCameraMove(int32_t dx, int32_t dy, int32_t dz, bool bReset);
----------------------------------------------------------------------
[解説]
  サンプル用の簡易カメラを引数に従い移動させます。
  
[パラメータ]
  int32_t dx, dy, dz:
    - x, y, zの移動量
  
  bool bReset:
    - 移動を初期化するか
    
[返り値]
  なし


----------------------------------------------------------------------
Vectormath::Aos::Matrix4 cellGcmUtilSimpleCameraGetMatrix(void);
----------------------------------------------------------------------
[解説]
  サンプル用の簡易カメラから現在のビューマトリックスを取得します。
  
[パラメータ]
  なし
    
[返り値]
  現在のビューマトリックス


----------------------------------------------------------------------
void cellGcmUtilSimpleCameraGetInitParam(Vectormath::Aos::Vector3 *eye, Vectormath::Aos::Vector3 *at, Vectormath::Aos::Vector3 *up);
----------------------------------------------------------------------
[解説]
  サンプル用の簡易カメラの初期情報を取得します。
  
[パラメータ]
  Vectormath::Aos::Vector3 *eye:
    - カメラの初期位置が格納される変数へのポインタ
    
  Vectormath::Aos::Vector3 *at:
    - カメラの初期注視点が格納される変数へのポインタ
    
  Vectormath::Aos::Vector3 *up:
    - カメラの初期の上方向ベクトルが格納される変数へのポインタ
    
[返り値]
  なし


----------------------------------------------------------------------
void cellGcmUtilSimpleCameraGetStatus(Vectormath::Aos::Vector3 *eye, Vectormath::Aos::Vector3 *at, Vectormath::Aos::Vector3 *up);
----------------------------------------------------------------------
[解説]
  サンプル用の簡易カメラの現在の情報を取得します。
  
[パラメータ]
  Vectormath::Aos::Vector3 *eye:
    - カメラの現在位置が格納される変数へのポインタ
    
  Vectormath::Aos::Vector3 *at:
    - カメラの現在注視点が格納される変数へのポインタ
    
  Vectormath::Aos::Vector3 *up:
    - カメラの現在の上方向ベクトルが格納される変数へのポインタ
    
[返り値]
  なし


----------------------------------------------------------------------
void cellGcmUtilSimpleCameraGetParam(Vectormath::Aos::Vector3 *trans, float *yaw, float *pitch);
----------------------------------------------------------------------
[解説]
  サンプル用の簡易カメラの現在の状態を取得します。
  
[パラメータ]
  Vectormath::Aos::Vector3 *trans:
    - カメラの移動量が格納される変数へのポインタ
    
  float *yaw:
    - カメラの横回転量が格納される変数へのポインタ
    
  float *pitch:
    - カメラの縦回転量が格納される変数へのポインタ
    
[返り値]
  なし


----------------------------------------------------------------------
void cellGcmUtilSimpleCameraSetParam(Vectormath::Aos::Vector3 trans, float yaw, float pitch);
----------------------------------------------------------------------
[解説]
  サンプル用の簡易カメラの現在の状態を設定します。
  
[パラメータ]
  Vectormath::Aos::Vector3 trans:
    - カメラの移動量
    
  float yaw:
    - カメラの横回転量
    
  float pitch:
    - カメラの縦回転量
    
[返り値]
  なし


[EOF]

