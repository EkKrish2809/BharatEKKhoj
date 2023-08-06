// dear imgui, v1.75 WIP
// (headers)

// See imgui.cpp file for documentation.
// Call and read ImGui::ShowDemoWindow() in imgui_demo.cpp for demo code.
// Newcomers, read 'Programmer guide' in imgui.cpp for notes on how to setup Dear ImGui in your codebase.
// Get latest version at https://github.com/ocornut/imgui

/*

Index of this file:
// Header mess
// Forward declarations and basic types
// ImGui API (Dear ImGui end-user API)
// Flags & Enumerations
// Memory allocations macros
// ImVector<>
// ImGuiStyle
// ImGuiIO
// Misc data structures (ImGuiInputTextCallbackData, ImGuiSizeCallbackData, ImGuiPayload)
// Obsolete functions
// Helpers (ImGuiOnceUponAFrame, ImGuiTextFilter, ImGuiTextBuffer, ImGuiStorage, ImGuiListClipper, ImColor)
// Draw List API (ImDrawCallback, ImDrawCmd, ImDrawIdx, ImDrawVert, ImDrawChannel, ImDrawListSplitter, ImDrawListFlags, ImDrawList, ImDrawData)
// Font API (ImFontConfig, ImFontGlyph, ImFontGlyphRangesBuilder, ImFontAtlasFlags, ImFontAtlas, ImFont)

*/

#pragma once

// Configuration file with compile-time options (edit imconfig.h or #define IMGUI_USER_CONFIG to your own filename)
#ifdef IMGUI_USER_CONFIG
#include IMGUI_USER_CONFIG
#endif
#if !defined(IMGUI_DISABLE_INCLUDE_IMCONFIG_H) || defined(IMGUI_INCLUDE_IMCONFIG_H)
#include "imconfig.h"
#endif

//-----------------------------------------------------------------------------
// Header mess
//-----------------------------------------------------------------------------

// Includes
#include <float.h>                  // FLT_MIN, FLT_MAX
#include <stdarg.h>                 // va_list, va_start, va_end
#include <stddef.h>                 // ptrdiff_t, NULL
#include <string.h>                 // memset, memmove, memcpy, strlen, strchr, strcpy, strcmp

// Version
// (Integer encoded as XYYZZ for use in #if preprocessor conditionals. Work in progress versions typically starts at XYY99 then bounce up to XYY00, XYY01 etc. when release tagging happens)
#define IMGUI_VERSION               "1.75 WIP"
#define IMGUI_VERSION_NUM           17401
#define IMGUI_CHECKVERSION()        ImGui::DebugCheckVersionAndDataLayout(IMGUI_VERSION, sizeof(ImGuiIO), sizeof(ImGuiStyle), sizeof(ImVec2), sizeof(ImVec4), sizeof(ImDrawVert), sizeof(ImDrawIdx))

// Define attributes of all API symbols declarations (e.g. for DLL under Windows)
// IMGUI_API is used for core imgui functions, IMGUI_IMPL_API is used for the default bindings files (imgui_impl_xxx.h)
// Using dear imgui via a shared library is not recommended, because of function call overhead and because we don't guarantee backward nor forward ABI compatibility.
#ifndef IMGUI_API
#define IMGUI_API
#endif
#ifndef IMGUI_IMPL_API
#define IMGUI_IMPL_API              IMGUI_API
#endif

// Helper Macros
#ifndef IM_ASSERT
#include <assert.h>
#define IM_ASSERT(_EXPR)            assert(_EXPR)                               // You can override the default assert handler by editing imconfig.h
#endif
#if defined(__clang__) || defined(__GNUC__)
#define IM_FMTARGS(FMT)             __attribute__((format(printf, FMT, FMT+1))) // To apply printf-style warnings to our functions.
#define IM_FMTLIST(FMT)             __attribute__((format(printf, FMT, 0)))
#else
#define IM_FMTARGS(FMT)
#define IM_FMTLIST(FMT)
#endif
#define IM_ARRAYSIZE(_ARR)          ((int)(sizeof(_ARR) / sizeof(*_ARR)))       // Size of a static C-style array. Don't use on pointers!
#define IM_UNUSED(_VAR)             ((void)_VAR)                                // Used to silence "unused variable warnings". Often useful as asserts may be stripped out from final builds.
#if (__cplusplus >= 201100)
#define IM_OFFSETOF(_TYPE,_MEMBER)  offsetof(_TYPE, _MEMBER)                    // Offset of _MEMBER within _TYPE. Standardized as offsetof() in C++11
#else
#define IM_OFFSETOF(_TYPE,_MEMBER)  ((size_t)&(((_TYPE*)0)->_MEMBER))           // Offset of _MEMBER within _TYPE. Old style macro.
#endif
#define IM_UNICODE_CODEPOINT_MAX     0xFFFF                                     // Last Unicode code point supported by this build.
#define IM_UNICODE_CODEPOINT_INVALID 0xFFFD                                     // Standard invalid Unicode code point.

// Warnings
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"
#if __has_warning("-Wzero-as-null-pointer-constant")
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"                  // warning: unknown option after '#pragma GCC diagnostic' kind
#pragma GCC diagnostic ignored "-Wclass-memaccess"          // [__GNUC__ >= 8] warning: 'memset/memcpy' clearing/writing an object of type 'xxxx' with no trivial copy-assignment; use assignment or value-initialization instead
#endif

//-----------------------------------------------------------------------------
// Forward declarations and basic types
//-----------------------------------------------------------------------------

struct ImDrawChannel;               // Temporary storage to output draw commands out of order, used by ImDrawListSplitter and ImDrawList::ChannelsSplit()
struct ImDrawCmd;                   // A single draw command within a parent ImDrawList (generally maps to 1 GPU draw call, unless it is a callback)
struct ImDrawData;                  // All draw command lists required to render the frame + pos/size coordinates to use for the projection matrix.
struct ImDrawList;                  // A single draw command list (generally one per window, conceptually you may see this as a dynamic "mesh" builder)
struct ImDrawListSharedData;        // Data shared among multiple draw lists (typically owned by parent ImGui context, but you may create one yourself)
struct ImDrawListSplitter;          // Helper to split a draw list into different layers which can be drawn into out of order, then flattened back.
struct ImDrawVert;                  // A single vertex (pos + uv + col = 20 bytes by default. Override layout with IMGUI_OVERRIDE_DRAWVERT_STRUCT_LAYOUT)
struct ImFont;                      // Runtime data for a single font within a parent ImFontAtlas
struct ImFontAtlas;                 // Runtime data for multiple fonts, bake multiple fonts into a single texture, TTF/OTF font loader
struct ImFontConfig;                // Configuration data when adding a font or merging fonts
struct ImFontGlyph;                 // A single font glyph (code point + coordinates within in ImFontAtlas + offset)
struct ImFontGlyphRangesBuilder;    // Helper to build glyph ranges from text/string data
struct ImColor;                     // Helper functions to create a color that can be converted to either u32 or float4 (*OBSOLETE* please avoid using)
struct ImGuiContext;                // Dear ImGui context (opaque structure, unless including imgui_internal.h)
struct ImGuiIO;                     // Main configuration and I/O between your application and ImGui
struct ImGuiInputTextCallbackData;  // Shared state of InputText() when using custom ImGuiInputTextCallback (rare/advanced use)
struct ImGuiListClipper;            // Helper to manually clip large list of items
struct ImGuiOnceUponAFrame;         // Helper for running a block of code not more than once a frame, used by IMGUI_ONCE_UPON_A_FRAME macro
struct ImGuiPayload;                // User data payload for drag and drop operations
struct ImGuiSizeCallbackData;       // Callback data when using SetNextWindowSizeConstraints() (rare/advanced use)
struct ImGuiStorage;                // Helper for key->value storage
struct ImGuiStyle;                  // Runtime data for styling/colors
struct ImGuiTextBuffer;             // Helper to hold and append into a text buffer (~string builder)
struct ImGuiTextFilter;             // Helper to parse and apply text filters (e.g. "aaaaa[,bbbb][,ccccc]")

// Typedefs and Enums/Flags (declared as int for compatibility with old C++, to allow using as flags and to not pollute the top of this file)
// Use your programming IDE "Go to definition" facility on the names in the central column below to find the actual flags/enum lists.
#ifndef ImTextureID
typedef void* ImTextureID;          // User data to identify a texture (this is whatever to you want it to be! read the FAQ about ImTextureID in imgui.cpp)
#endif
typedef unsigned int ImGuiID;       // Unique ID used by widgets (typically hashed from a stack of string)
typedef unsigned short ImWchar;     // A single U16 character for keyboard input/display. We encode them as multi bytes UTF-8 when used in strings.
typedef int ImGuiCol;               // -> enum ImGuiCol_             // Enum: A color identifier for styling
typedef int ImGuiCond;              // -> enum ImGuiCond_            // Enum: A condition for many Set*() functions
typedef int ImGuiDataType;          // -> enum ImGuiDataType_        // Enum: A primary data type
typedef int ImGuiDir;               // -> enum ImGuiDir_             // Enum: A cardinal direction
typedef int ImGuiKey;               // -> enum ImGuiKey_             // Enum: A key identifier (ImGui-side enum)
typedef int ImGuiNavInput;          // -> enum ImGuiNavInput_        // Enum: An input identifier for navigation
typedef int ImGuiMouseButton;       // -> enum ImGuiMouseButton_     // Enum: A mouse button identifier (0=left, 1=right, 2=middle)
typedef int ImGuiMouseCursor;       // -> enum ImGuiMouseCursor_     // Enum: A mouse cursor identifier
typedef int ImGuiStyleVar;          // -> enum ImGuiStyleVar_        // Enum: A variable identifier for styling
typedef int ImDrawCornerFlags;      // -> enum ImDrawCornerFlags_    // Flags: for ImDrawList::AddRect(), AddRectFilled() etc.
typedef int ImDrawListFlags;        // -> enum ImDrawListFlags_      // Flags: for ImDrawList
typedef int ImFontAtlasFlags;       // -> enum ImFontAtlasFlags_     // Flags: for ImFontAtlas
typedef int ImGuiBackendFlags;      // -> enum ImGuiBackendFlags_    // Flags: for io.BackendFlags
typedef int ImGuiColorEditFlags;    // -> enum ImGuiColorEditFlags_  // Flags: for ColorEdit4(), ColorPicker4() etc.
typedef int ImGuiConfigFlags;       // -> enum ImGuiConfigFlags_     // Flags: for io.ConfigFlags
typedef int ImGuiComboFlags;        // -> enum ImGuiComboFlags_      // Flags: for BeginCombo()
typedef int ImGuiDragDropFlags;     // -> enum ImGuiDragDropFlags_   // Flags: for BeginDragDropSource(), AcceptDragDropPayload()
typedef int ImGuiFocusedFlags;      // -> enum ImGuiFocusedFlags_    // Flags: for IsWindowFocused()
typedef int ImGuiHoveredFlags;      // -> enum ImGuiHoveredFlags_    // Flags: for IsItemHovered(), IsWindowHovered() etc.
typedef int ImGuiInputTextFlags;    // -> enum ImGuiInputTextFlags_  // Flags: for InputText(), InputTextMultiline()
typedef int ImGuiSelectableFlags;   // -> enum ImGuiSelectableFlags_ // Flags: for Selectable()
typedef int ImGuiTabBarFlags;       // -> enum ImGuiTabBarFlags_     // Flags: for BeginTabBar()
typedef int ImGuiTabItemFlags;      // -> enum ImGuiTabItemFlags_    // Flags: for BeginTabItem()
typedef int ImGuiTreeNodeFlags;     // -> enum ImGuiTreeNodeFlags_   // Flags: for TreeNode(), TreeNodeEx(), CollapsingHeader()
typedef int ImGuiWindowFlags;       // -> enum ImGuiWindowFlags_     // Flags: for Begin(), BeginChild()
typedef int (*ImGuiInputTextCallback)(ImGuiInputTextCallbackData *data);
typedef void (*ImGuiSizeCallback)(ImGuiSizeCallbackData* data);

// Scalar data types
typedef signed char         ImS8;   // 8-bit signed integer
typedef unsigned char       ImU8;   // 8-bit unsigned integer
typedef signed short        ImS16;  // 16-bit signed integer
typedef unsigned short      ImU16;  // 16-bit unsigned integer
typedef signed int          ImS32;  // 32-bit signed integer == int
typedef unsigned int        ImU32;  // 32-bit unsigned integer (often used to store packed colors)
#if defined(_MSC_VER) && !defined(__clang__)
typedef signed   __int64    ImS64;  // 64-bit signed integer (pre and post C++11 with Visual Studio)
typedef unsigned __int64    ImU64;  // 64-bit unsigned integer (pre and post C++11 with Visual Studio)
#elif (defined(__clang__) || defined(__GNUC__)) && (__cplusplus < 201100)
#include <stdint.h>
typedef int64_t             ImS64;  // 64-bit signed integer (pre C++11)
typedef uint64_t            ImU64;  // 64-bit unsigned integer (pre C++11)
#else
typedef signed   long long  ImS64;  // 64-bit signed integer (post C++11)
typedef unsigned long long  ImU64;  // 64-bit unsigned integer (post C++11)
#endif

// 2D vector (often used to store positions, sizes, etc.)
struct ImVec2
{
    float     x, y;
    ImVec2()  { x = y = 0.0f; }
    ImVec2(float _x, float _y) { x = _x; y = _y; }
    float  operator[] (size_t idx) const { IM_ASSERT(idx <= 1); return (&x)[idx]; }    // We very rarely use this [] operator, the assert overhead is fine.
    float& operator[] (size_t idx)       { IM_ASSERT(idx <= 1); return (&x)[idx]; }    // We very rarely use this [] operator, the assert overhead is fine.
#ifdef IM_VEC2_CLASS_EXTRA
    IM_VEC2_CLASS_EXTRA     // Define additional constructors and implicit cast operators in imconfig.h to convert back and forth between your math types and ImVec2.
#endif
};

// 4D vector (often used to store floating-point colors)
struct ImVec4
{
    float     x, y, z, w;
    ImVec4()  { x = y = z = w = 0.0f; }
    ImVec4(float _x, float _y, float _z, float _w) { x = _x; y = _y; z = _z; w = _w; }
#ifdef IM_VEC4_CLASS_EXTRA
    IM_VEC4_CLASS_EXTRA     // Define additional constructors and implicit cast operators in imconfig.h to convert back and forth between your math types and ImVec4.
#endif
};

//-----------------------------------------------------------------------------
// ImGui: Dear ImGui end-user API
// (Inside a namespace so you can add extra functions in your own separate file. Please don't modify imgui source files!)
//-----------------------------------------------------------------------------

namespace ImGui
{
    // Context creation and access
    // Each context create its own ImFontAtlas by default. You may instance one yourself and pass it to CreateContext() to share a font atlas between imgui contexts.
    // None of those functions is reliant on the current context.
    IMGUI_API ImGuiContext* CreateContext(ImFontAtlas* shared_font_atlas = NULL);
    IMGUI_API void          DestroyContext(ImGuiContext* ctx = NULL);   // NULL = destroy current context
    IMGUI_API ImGuiContext* GetCurrentContext();
    IMGUI_API void          SetCurrentContext(ImGuiContext* ctx);
    IMGUI_API bool          DebugCheckVersionAndDataLayout(const char* version_str, size_t sz_io, size_t sz_style, size_t sz_vec2, size_t sz_vec4, size_t sz_drawvert, size_t sz_drawidx);

    // Main
    IMGUI_API ImGuiIO&      GetIO();                                    // access the IO structure (mouse/keyboard/gamepad inputs, time, various configuration options/flags)
    IMGUI_API ImGuiStyle&   GetStyle();                                 // access the Style structure (colors, sizes). Always use PushStyleCol(), PushStyleVar() to modify style mid-frame.
    IMGUI_API void          NewFrame();                                 // start a new Dear ImGui frame, you can submit any command from this point until Render()/EndFrame().
    IMGUI_API void          EndFrame();                                 // ends the Dear ImGui frame. automatically called by Render(), you likely don't need to call that yourself directly. If you don't need to render data (skipping rendering) you may call EndFrame() but you'll have wasted CPU already! If you don't need to render, better to not create any imgui windows and not call NewFrame() at all!
    IMGUI_API void          Render();                                   // ends the Dear ImGui frame, finalize the draw data. You can get call GetDrawData() to obtain it and run your rendering function. (Obsolete: this used to call io.RenderDrawListsFn(). Nowadays, we allow and prefer calling your render function yourself.)
    IMGUI_API ImDrawData*   GetDrawData();                              // valid after Render() and until the next call to NewFrame(). this is what you have to render.

    // Demo, Debug, Information
    IMGUI_API void          ShowDemoWindow(bool* p_open = NULL);        // create Demo window (previously called ShowTestWindow). demonstrate most ImGui features. call this to learn about the library! try to make it always available in your application!
    IMGUI_API void          ShowAboutWindow(bool* p_open = NULL);       // create About window. display Dear ImGui version, credits and build/system information.
    IMGUI_API void          ShowMetricsWindow(bool* p_open = NULL);     // create Metrics/Debug window. display Dear ImGui internals: draw commands (with individual draw calls and vertices), window list, basic internal state, etc.
    IMGUI_API void          ShowStyleEditor(ImGuiStyle* ref = NULL);    // add style editor block (not a window). you can pass in a reference ImGuiStyle structure to compare to, revert to and save to (else it uses the default style)
    IMGUI_API bool          ShowStyleSelector(const char* label);       // add style selector block (not a window), essentially a combo listing the default styles.
    IMGUI_API void          ShowFontSelector(const char* label);        // add font selector block (not a window), essentially a combo listing the loaded fonts.
    IMGUI_API void          ShowUserGuide();                            // add basic help/info block (not a window): how to manipulate ImGui as a end-user (mouse/keyboard controls).
    IMGUI_API const char*   GetVersion();                               // get the compiled version string e.g. "1.23" (essentially the compiled value for IMGUI_VERSION)

    // Styles
    IMGUI_API void          StyleColorsDark(ImGuiStyle* dst = NULL);    // new, recommended style (default)
    IMGUI_API void          StyleColorsClassic(ImGuiStyle* dst = NULL); // classic imgui style
    IMGUI_API void          StyleColorsLight(ImGuiStyle* dst = NULL);   // best used with borders and a custom, thicker font

    // Windows
    // - Begin() = push window to the stack and start appending to it. End() = pop window from the stack.
    // - You may append multiple times to the same window during the same frame.
    // - Passing 'bool* p_open != NULL' shows a window-closing widget in the upper-right corner of the window,
    //   which clicking will set the boolean to false when clicked.
    // - Begin() return false to indicate the window is collapsed or fully clipped, so you may early out and omit submitting
    //   anything to the window. Always call a matching End() for each Begin() call, regardless of its return value!
    //   [Important: due to legacy reason, this is inconsistent with most other functions such as BeginMenu/EndMenu,
    //    BeginPopup/EndPopup, etc. where the EndXXX call should only be called if the corresponding BeginXXX function
    //    returned true. Begin and BeginChild are the only odd ones out. Will be fixed in a future update.]
    // - Note that the bottom of window stack always contains a window called "Debug".
    IMGUI_API bool          Begin(const char* name, bool* p_open = NULL, ImGuiWindowFlags flags = 0);
    IMGUI_API void          End();

    // Child Windows
    // - Use child windows to begin into a self-contained independent scrolling/clipping regions within a host window. Child windows can embed their own child.
    // - For each independent axis of 'size': ==0.0f: use remaining host window size / >0.0f: fixed size / <0.0f: use remaining window size minus abs(size) / Each axis can use a different mode, e.g. ImVec2(0,400).
    // - BeginChild() returns false to indicate the window is collapsed or fully clipped, so you may early out and omit submitting anything to the window.
    //   Always call a matching EndChild() for each BeginChild() call, regardless of its return value [as with Begin: this is due to legacy reason and inconsistent with most BeginXXX functions apart from the regular Begin() which behaves like BeginChild().]
    IMGUI_API bool          BeginChild(const char* str_id, const ImVec2& size = ImVec2(0,0), bool border = false, ImGuiWindowFlags flags = 0);
    IMGUI_API bool          BeginChild(ImGuiID id, const ImVec2& size = ImVec2(0,0), bool border = false, ImGuiWindowFlags flags = 0);
    IMGUI_API void          EndChild();

    // Windows Utilities
    // - 'current window' = the window we are appending into while inside a Begin()/End() block. 'next window' = next window we will Begin() into.
    IMGUI_API bool          IsWindowAppearing();
    IMGUI_API bool          IsWindowCollapsed();
    IMGUI_API bool          IsWindowFocused(ImGuiFocusedFlags flags=0); // is current window focused? or its root/child, depending on flags. see flags for options.
    IMGUI_API bool          IsWindowHovered(ImGuiHoveredFlags flags=0); // is current window hovered (and typically: not blocked by a popup/modal)? see flags for options. NB: If you are trying to check whether your mouse should be dispatched to imgui or to your app, you should use the 'io.WantCaptureMouse' boolean for that! Please read the FAQ!
    IMGUI_API ImDrawList*   GetWindowDrawList();                        // get draw list associated to the current window, to append your own drawing primitives
    IMGUI_API ImVec2        GetWindowPos();                             // get current window position in screen space (useful if you want to do your own drawing via the DrawList API)
    IMGUI_API ImVec2        GetWindowSize();                            // get current window size
    IMGUI_API float         GetWindowWidth();                           // get current window width (shortcut for GetWindowSize().x)
    IMGUI_API float         GetWindowHeight();                          // get current window height (shortcut for GetWindowSize().y)

    // Prefer using SetNextXXX functions (before Begin) rather that SetXXX functions (after Begin).
    IMGUI_API void          SetNextWindowPos(const ImVec2& pos, ImGuiCond cond = 0, const ImVec2& pivot = ImVec2(0,0)); // set next window position. call before Begin(). use pivot=(0.5f,0.5f) to center on given point, etc.
    IMGUI_API void          SetNextWindowSize(const ImVec2& size, ImGuiCond cond = 0);                  // set next window size. set axis to 0.0f to force an auto-fit on this axis. call before Begin()
    IMGUI_API void          SetNextWindowSizeConstraints(const ImVec2& size_min, const ImVec2& size_max, ImGuiSizeCallback custom_callback = NULL, void* custom_callback_data = NULL); // set next window size limits. use -1,-1 on either X/Y axis to preserve the current size. Sizes will be rounded down. Use callback to apply non-trivial programmatic constraints.
    IMGUI_API void          SetNextWindowContentSize(const ImVec2& size);                               // set next window content size (~ scrollable client area, which enforce the range of scrollbars). Not including window decorations (title bar, menu bar, etc.) nor WindowPadding. set an axis to 0.0f to leave it automatic. call before Begin()
    IMGUI_API void          SetNextWindowCollapsed(bool collapsed, ImGuiCond cond = 0);                 // set next window collapsed state. call before Begin()
    IMGUI_API void          SetNextWindowFocus();                                                       // set next window to be focused / top-most. call before Begin()
    IMGUI_API void          SetNextWindowBgAlpha(float alpha);                                          // set next window background color alpha. helper to easily modify ImGuiCol_WindowBg/ChildBg/PopupBg. you may also use ImGuiWindowFlags_NoBackground.
    IMGUI_API void          SetWindowPos(const ImVec2& pos, ImGuiCond cond = 0);                        // (not recommended) set current window position - call within Begin()/End(). prefer using SetNextWindowPos(), as this may incur tearing and side-effects.
    IMGUI_API void          SetWindowSize(const ImVec2& size, ImGuiCond cond = 0);                      // (not recommended) set current window size - call within Begin()/End(). set to ImVec2(0,0) to force an auto-fit. prefer using SetNextWindowSize(), as this may incur tearing and minor side-effects.
    IMGUI_API void          SetWindowCollapsed(bool collapsed, ImGuiCond cond = 0);                     // (not recommended) set current window collapsed state. prefer using SetNextWindowCollapsed().
    IMGUI_API void          SetWindowFocus();    fragmentShaderObject);

        // glBindAttribLocation(shaderProgramObject_scaler_field, MATRIX_ATTRIBUTE_POSITION, "a_position");
        // glBindAttribLocation(shaderProgramObject_scaler_field, MATRIX_ATTRIBUTE_TEXTURE0, "a_texcoord");

        glTransformFeedbackVaryings(shaderProgramObject_scaler_field, 1, &scalar_field_value_varying_name, GL_SEPARATE_ATTRIBS);

        Bool bLinkShaderStatus = LinkShaderProgramObject(shaderProgramObject_scaler_field);
        if (bLinkShaderStatus == False)
        {
            return False;
        }

        // map uniforms with CPU variables
        samplerPerAxisUniform = glGetUniformLocation(shaderProgramObject_scaler_field, "samples_per_axis");
        sphereUniformBlocks = glGetUniformBlockIndex(shaderProgramObject_scaler_field, "spheres_uniform_block");

        glUseProgram(shaderProgramObject_scaler_field);

        glUniform1i(shaderProgramObject_scaler_field, samplerPerAxisUniform);

        glUniformBlockBinding(shaderProgramObject_scaler_field, sphereUniformBlocks, 0);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, spheres_updater_sphere_positions_buffer_object_id);

        glGenBuffers(1, &scalar_field_buffer_object_id);
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, scalar_field_buffer_object_id);
        glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, samples_in_3d_space * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);

        glGenTransformFeedbacks(1, &scalar_field_transform_feedback_object_id);
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, scalar_field_transform_feedback_object_id);

        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, scalar_field_buffer_object_id);
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

        /* [Stage 2 Creating texture] */
        /* Generate texture object to hold scalar field data. */
        glGenTextures(1, &scalar_field_texture_object_id);

        /* Scalar field uses GL_TEXTURE_3D target of texture unit 1. */
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_3D, scalar_field_texture_object_id);

        /* Prepare texture storage for scalar field values. */
        glTexStorage3D(GL_TEXTURE_3D, 1, GL_R32F, samples_per_axis, samples_per_axis, samples_per_axis);
        /* [Stage 2 Creating texture] */

        /* Tune texture settings to use it as a data source. */
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return True;
    }

    Bool initMarchingCubeCells(void)
    {
        // /** Cell_type_index output variable's name. */
        // const GLchar *marching_cubes_cells_varying_name = "cell_type_index";

        // code
        GLuint vertexShaderObject = CreateAndCompileShaderObjects("./src/shaders/metaball/marchingcubescells.vs", VERTEX);

        GLuint fragmentShaderObject = CreateAndCompileShaderObjects("./src/shaders/metaball/marchingcubescells.fs", FRAGMENT);

        printf("shader compilation done\n");

        shaderProgramObject_marching_cubes = glCreateProgram();

        glAttachShader(shaderProgramObject_marching_cubes, vertexShaderObject);
        glAttachShader(shaderProgramObject_marching_cubes, fragmentShaderObject);

        // glBindAttribLocation(shaderProgramObject_marching_cubes, MATRIX_ATTRIBUTE_POSITION, "a_position");
        // glBindAttribLocation(shaderProgramObject_marching_cubes, MATRIX_ATTRIBUTE_TEXTURE0, "a_texcoord");

        glTransformFeedbackVaryings(shaderProgramObject_marching_cubes, 1, &marching_cubes_cells_varying_name, GL_SEPARATE_ATTRIBS);

        Bool bLinkShaderStatus = LinkShaderProgramObject(shaderProgramObject_marching_cubes);
        if (bLinkShaderStatus == False)
        {
            printf("Marching Cube Cells shader linking failed\n");
            return False;
        }
        printf("shader linking done\n");
        // map uniforms with CPU variables
        scalerFieldSamplerUniform_cubes = glGetUniformLocation(shaderProgramObject_marching_cubes, "scalar_field");
        cellsPerAxisUniform = glGetUniformLocation(shaderProgramObject_marching_cubes, "cells_per_axis");
        isoLevelUniform_cubes = glGetUniformLocation(shaderProgramObject_marching_cubes, "iso_level");

        glUseProgram(shaderProgramObject_marching_cubes);

        glUniform1i(cellsPerAxisUniform, cells_per_axis);
        glUniform1f(isoLevelUniform_cubes, isosurface_level);
        glUniform1i(scalerFieldSamplerUniform_cubes, 1);

        glGenBuffers(1, &marching_cubes_cells_types_buffer_id);
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, marching_cubes_cells_types_buffer_id);
        glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, cells_in_3d_space * sizeof(GLint), NULL, GL_STATIC_DRAW);
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);

        glGenTransformFeedbacks(1, &marching_cubes_cells_transform_feedback_object_id);
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, marching_cubes_cells_transform_feedback_object_id);

        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, marching_cubes_cells_types_buffer_id);
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

        /* [Stage 3 Creating texture] */
        /* Generate a texture object to hold cell type data. (We will explain why the texture later). */
        glGenTextures(1, &marching_cubes_cells_types_texture_object_id);

        /* Marching cubes cell type data uses GL_TEXTURE_3D target of texture unit 2. */
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_3D, marching_cubes_cells_types_texture_object_id);

        /* Prepare texture storage for marching cube cell type data. */
        glTexStorage3D(GL_TEXTURE_3D, 1, GL_R32I, cells_per_axis, cells_per_axis, cells_per_axis);
        /* [Stage 3 Creating texture] */

        /* Tune texture settings to use it as a data source. */
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return True;
    }

    Bool initMarchingCubeTriangles(void)
    {
        // /** Name of sphere_positions_uniform_block uniform block. */
        // const GLchar *marching_cubes_triangles_uniform_sphere_positions_name = "sphere_positions_uniform_block";

        // code
        GLuint vertexShaderObject = CreateAndCompileShaderObjects("./src/shaders/metaball/marchingcubetriangles.vs", VERTEX);

        GLuint fragmentShaderObject = CreateAndCompileShaderObjects("./src/shaders/metaball/marchingcubetriangles.fs", FRAGMENT);

        shaderProgramObject_marching_cube_triangles = glCreateProgram();

        glAttachShader(shaderProgramObject_marching_cube_triangles, vertexShaderObject);
        glAttachShader(shaderProgramObject_marching_cube_triangles, fragmentShaderObject);

        // glBindAttribLocation(shaderProgramObject_marching_cube_triangles, MATRIX_ATTRIBUTE_POSITION, "a_position");
        // glBindAttribLocation(shaderProgramObject_marching_cube_triangles, MATRIX_ATTRIBUTE_TEXTURE0, "a_texcoord");

        Bool bLinkShaderStatus = LinkShaderProgramObject(shaderProgramObject_marching_cube_triangles);
        if (bLinkShaderStatus == False)
        {
            return False;
        }

        // map uniforms with CPU variables
        samplesPerAxisUniform = glGetUniformLocation(shaderProgramObject_marching_cube_triangles, "samples_per_axis");
        cellTypesSamplerUniform = glGetUniformLocation(shaderProgramObject_marching_cube_triangles, "cell_types");
        scalerFieldSamplerUniform = glGetUniformLocation(shaderProgramObject_marching_cube_triangles, "scalar_field");
        triTableSamplerUniform = glGetUniformLocation(shaderProgramObject_marching_cube_triangles, "tri_table");
        mvpUniform = glGetUniformLocation(shaderProgramObject_marching_cube_triangles, "mvp");
        isoLevelUniform = glGetUniformLocation(shaderProgramObject_marching_cube_triangles, "iso_level");
        timeMarchingTriangleUniform = glGetUniformLocation(shaderProgramObject_marching_cube_triangles, "time");
        spherePositionIdUniform = glGetUniformLocation(shaderProgramObject_marching_cube_triangles, "sphere_positions_uniform_block");

        // have to pass model * view * projection as a single uniform
        calc_mvp();

        // initializing uniforms
        glUniform1f(isoLevelUniform, isosurface_level);
        glUniform1i(samplesPerAxisUniform, samples_per_axis);
        glUniform1i(triTableSamplerUniform, 4);
        glUniform1i(cellTypesSamplerUniform, 2);
        glUniform1i(scalerFieldSamplerUniform, 1);
        // glUniformMatrix4fv(mvpUniform, 1, GL_FALSE,  perspectiveProjectionMatrix * viewMatrix * modelMatrix  );
        glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, mvp.getAsArray());

        /* Allocate memory for buffer */
        glBindBuffer(GL_UNIFORM_BUFFER, spheres_updater_sphere_positions_buffer_object_id);

        /* Generate an Id for a texture object to hold look-up array data (tri_table). */
        glGenTextures(1, &marching_cubes_triangles_lookup_table_texture_id);

        /* Lookup array (tri_table) uses GL_TEXTURE_2D target of texture unit 4. */
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, marching_cubes_triangles_lookup_table_texture_id);

        /* Tune texture settings to use it as a data source. */
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        /* Load lookup table (tri_table) into texture. */
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32I, mc_vertices_per_cell, mc_cells_types_count);
        glTexSubImage2D(GL_TEXTURE_2D,        /* Use texture bound to GL_TEXTURE_2D               */
                        0,                    /* Base image level                                 */
                        0,                    /* From the texture origin                          */
                        0,                    /* From the texture origin                          */
                        mc_vertices_per_cell, /* Width will represent vertices in all 5 triangles */
                        mc_cells_types_count, /* Height will represent cell type                  */
                        GL_RED_INTEGER,       /* Texture will have only one component             */
                        GL_INT,               /* ... of type int                                  */
                        tri_table             /* Data will be copied directly from tri_table      */
        );

        /* Generate a vertex array object. We'll go with the explanation later. */
        // glGenVertexArrays(1, &marching_cubes_triangles_vao_id);

        /* In OpenGL ES, draw calls require a bound vertex array object.
         * Even though we're not using any per-vertex attribute data, we still need to bind a vertex array object.
         */
        // glBindVertexArray(marching_cubes_triangles_vao_id);

        return True;
    }

    void calc_mvp()
    {
        /* Define projection properties. */
        float degreesToRadiansCoefficient = atanf(1) / 45;                   /* Coefficient to recalculate degrees to radians.      */
        float frustum_fovy = 45.0f;                                          /* 45 degrees field of view in the y direction.        */
        float frustum_aspect = (float)giWindowWidth / (float)giWindowHeight; /* Aspect ratio.                                       */
        float frustum_z_near = 0.01f;                                        /* How close the viewer is to the near clipping plane. */
        float frustum_z_far = 100.0f;                                        /* How far the viewer is from the far clipping plane.  */
        float camera_distance = 2.5f;                                        /* Distance from camera to scene center.               */

        /* Matrix that stores temporary matrix data for translation transformations. */
        Matrix mat4_translate = Matrix::createTranslation(-0.5, -0.5, -0.5 + 5.0);

        /* Matrix that stores temporary matrix data for scale transformations. */
        Matrix mat4_scale = Matrix::createScaling(2.0, 2.0, 2.0);

        /* Matrix that transforms the vertices from model space to world space. */
        /* Translate and scale coordinates from [0..1] to [-1..1] range for full visibility. */
        Matrix mat4_model_view = mat4_scale * mat4_translate;

        /* Pull the camera back from the scene center. */
        mat4_model_view[14] -= float(camera_distance);

        /* Create the perspective matrix from frustum parameters. */
        Matrix mat4_perspective = Matrix::matrixPerspective(degreesToRadiansCoefficient * frustum_fovy, frustum_aspect, frustum_z_near, frustum_z_far);

        /* MVP (Model View Perspective) matrix is a result of multiplication of Perspective Matrix by Model View Matrix. */
        mvp = mat4_perspective * mat4_model_view;
    }

    void renderMetaball(void)
    {
        /* Update time. */
        printf("metaball :: display()\n");
        // printf("timer : %f\n", ELAPSED_TIME);
        model_time += 0.01f;
        // printf("timer : %f\n", ELAPSED_TIME);
        // calc_mvp(mvp);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, spheres_updater_transform_feedback_object_id);

        glEnable(GL_RASTERIZER_DISCARD);
        /* [Stage 1 Enable GL_RASTERIZER_DISCARD] */
        {

            glUseProgram(shaderProgramObject_sphere_updater);

            glUniform1f(timeUniform, model_time);

            glBeginTransformFeedback(GL_POINTS);
            {
                glDrawArrays(GL_POINTS, 0, n_spheres);
            }
            glEndTransformFeedback();
        }
        /* [Stage 1 Disable GL_RASTERIZER_DISCARD] */
        glDisable(GL_RASTERIZER_DISCARD);

        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

        glBindBuffer(GL_UNIFORM_BUFFER, spheres_updater_sphere_positions_buffer_object_id);

        /* Bind buffer object to store calculated scalar field values. */
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, scalar_field_transform_feedback_object_id);

        /* Shorten GL pipeline: we will use vertex shader only. */
        glEnable(GL_RASTERIZER_DISCARD);
        {
            /* Select program for scalar field generation stage. */
            glUseProgram(shaderProgramObject_scaler_field);

            /* Activate transform feedback mode. */
            glBeginTransformFeedback(GL_POINTS);
            {
                /* Run scalar field calculation for all vertices in space. */
                glDrawArrays(GL_POINTS, 0, samples_in_3d_space);
            }
            glEndTransformFeedback();
        }
        glDisable(GL_RASTERIZER_DISCARD);

        /* Unbind buffers used at this stage. */
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

        /* [Stage 2 Scalar field generation stage move data to texture] */
        glActiveTexture(GL_TEXTURE1);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, scalar_field_buffer_object_id);
        glTexSubImage3D(GL_TEXTURE_3D,    /* Use texture bound to GL_TEXTURE_3D                                     */
                        0,                /* Base image level                                                       */
                        0,                /* From the texture origin                                                */
                        0,                /* From the texture origin                                                */
                        0,                /* From the texture origin                                                */
                        samples_per_axis, /* Texture have same width as scalar field in buffer                      */
                        samples_per_axis, /* Texture have same height as scalar field in buffer                     */
                        samples_per_axis, /* Texture have same depth as scalar field in buffer                      */
                        GL_RED,           /* Scalar field gathered in buffer has only one component                 */
                        GL_FLOAT,         /* Scalar field gathered in buffer is of float type                       */
                        NULL              /* Scalar field gathered in buffer bound to GL_PIXEL_UNPACK_BUFFER target */
        );
        /* [Stage 2 Scalar field generation stage move data to texture] */

        /* 3. Marching cube algorithm cell splitting stage.
         */
        /* Bind buffer to store cell type data. */
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, marching_cubes_cells_transform_feedback_object_id);

        /* Shorten GL pipeline: we will use vertex shader only. */
        glEnable(GL_RASTERIZER_DISCARD);
        {
            /* Select program for Marching Cubes algorthim's cell splitting stage. */
            glUseProgram(shaderProgramObject_marching_cubes);

            /* Activate transform feedback mode. */
            glBeginTransformFeedback(GL_POINTS);
            {
                /* [Stage 3 Execute vertex shader] */
                /* Run Marching Cubes algorithm cell splitting stage for all cells. */
                glDrawArrays(GL_POINTS, 0, cells_in_3d_space);
                /* [Stage 3 Execute vertex shader] */
            }
            glEndTransformFeedback();
        }
        glDisable(GL_RASTERIZER_DISCARD);

        /* Unbind buffers used at this stage. */
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

        /* Copy data from buffer into texture bound to target GL_TEXTURE2 in texture unit 2.
         * We need to move this data to a texture object, as there is no way we could access data
         * stored within a buffer object in a OpenGL ES 3.0 shader.
         */
        glActiveTexture(GL_TEXTURE2);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, marching_cubes_cells_types_buffer_id);
        glTexSubImage3D(GL_TEXTURE_3D,  /* Use texture bound to GL_TEXTURE_3D                                   */
                        0,              /* Base image level                                                     */
                        0,              /* From the texture origin                                              */
                        0,              /* From the texture origin                                              */
                        0,              /* From the texture origin                                              */
                        cells_per_axis, /* Texture have same width as cells by width in buffer                  */
                        cells_per_axis, /* Texture have same height as cells by height in buffer                */
                        cells_per_axis, /* Texture have same depth as cells by depth in buffer                  */
                        GL_RED_INTEGER, /* Cell types gathered in buffer have only one component                */
                        GL_INT,         /* Cell types gathered in buffer are of int type                        */
                        NULL            /* Cell types gathered in buffer bound to GL_PIXEL_UNPACK_BUFFER target */
        );

        /* 4. Marching Cubes algorithm triangle generation stage.
         *
         * At this stage, we render exactly (3 vertices * 5 triangles per cell *
         * amount of cells the scalar field is split to) triangle vertices.
         * Then render triangularized geometry.
         */
        glActiveTexture(GL_TEXTURE0);

        /* Activate triangle generating and rendering program. */
        glUseProgram(shaderProgramObject_marching_cube_triangles);

        /* Specify input arguments to vertex shader. */
        glUniform1f(timeMarchingTriangleUniform, model_time);
        glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, mvp.getAsArray());
        // glUniformMatrix4fv(mvpUniform, 1, GL_FALSE,  perspectiveProjectionMatrix * viewMatrix * modelMatrix  );

        glBindVertexArray(marching_cubes_triangles_vao_id);
        /* [Stage 4 Run triangle generating and rendering program] */
        /* Run triangle generating and rendering program. */
        glDrawArrays(GL_TRIANGLES, 0, cells_in_3d_space * triangles_per_cell * vertices_per_triangle);
        /* [Stage 4 Run triangle generating and rendering program] */
         glBindVertexArray(0);
    }

    /** Draws one frame. */
    void renderFrame(float currTime)
    {
        /* Update time. */
        // model_time += 0.01f;
        model_time = currTime;

        /*
         * Rendering section
         */
        /* Clear the buffers that we are going to render to in a moment. */
        (glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        /* [Stage 1 Calculate sphere positions stage] */
        /* 1. Calculate sphere positions stage.
         *
         * At this stage we calculate new sphere positions in space
         * according to current time moment.
         */
        /* [Stage 1 Bind buffers to store calculated sphere position values] */
        /* Bind buffers to store calculated sphere position values. */
        (glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, spheres_updater_transform_feedback_object_id));
        /* [Stage 1 Bind buffers to store calculated sphere position values] */

        /* [Stage 1 Enable GL_RASTERIZER_DISCARD] */
        /* Shorten GL pipeline: we will use vertex shader only. */
        (glEnable(GL_RASTERIZER_DISCARD));
        /* [Stage 1 Enable GL_RASTERIZER_DISCARD] */
        {
            /* Select program for sphere positions generation stage. */
            (glUseProgram(shaderProgramObject_sphere_updater));

            /* [Stage 1 Specify input arguments to vertex shader] */
            /* Specify input arguments to vertex shader. */
            (glUniform1f(timeUniform, model_time));
            /* [Stage 1 Specify input arguments to vertex shader] */

            /* [Stage 1 Activate transform feedback mode] */
            /* Activate transform feedback mode. */
            (glBeginTransformFeedback(GL_POINTS));
            /* [Stage 1 Activate transform feedback mode] */
            {
                /* [Stage 1 Execute n_spheres times vertex shader] */
                /* Run sphere positions calculation. */
                (glDrawArrays(GL_POINTS, 0, n_spheres));
                /* [Stage 1 Execute n_spheres times vertex shader] */
            }
            /* [Stage 1 Deactivate transform feedback mode] */
            (glEndTransformFeedback());
            /* [Stage 1 Deactivate transform feedback mode] */
        }
        /* [Stage 1 Disable GL_RASTERIZER_DISCARD] */
        (glDisable(GL_RASTERIZER_DISCARD));
        /* [Stage 1 Disable GL_RASTERIZER_DISCARD] */

        /* Unbind buffers used at this stage. */
        (glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0));
        /* [Stage 1 Calculate sphere positions stage] */

        /* [Stage 2 Scalar field generation stage] */
        /* 2. Scalar field generation stage.
         *
         * At this stage we calculate scalar field and store it in buffer
         * and later copy from buffer to texture.
         */
        /* Bind sphere positions data buffer to GL_UNIFORM_BUFFER. */
        (glBindBuffer(GL_UNIFORM_BUFFER, spheres_updater_sphere_positions_buffer_object_id));

        /* Bind buffer object to store calculated scalar field values. */
        (glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, scalar_field_transform_feedback_object_id));

        /* Shorten GL pipeline: we will use vertex shader only. */
        (glEnable(GL_RASTERIZER_DISCARD));
        {
            /* Select program for scalar field generation stage. */
            (glUseProgram(shaderProgramObject_scaler_field));

            /* Activate transform feedback mode. */
            (glBeginTransformFeedback(GL_POINTS));
            {
                /* Run scalar field calculation for all vertices in space. */
                (glDrawArrays(GL_POINTS, 0, samples_in_3d_space));
            }
            (glEndTransformFeedback());
        }
        (glDisable(GL_RASTERIZER_DISCARD));

        /* Unbind buffers used at this stage. */
        (glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0));
        /* [Stage 2 Scalar field generation stage] */

        /* Copy scalar field values from buffer into texture bound to target GL_TEXTURE_3D of texture unit 1.
         * We need to move this data to a texture object, as there is no way we could access data
         * stored within a buffer object in an OpenGL ES 3.0 shader.
         */
        /* [Stage 2 Scalar field generation stage move data to texture] */
        (glActiveTexture(GL_TEXTURE1));
        (glBindBuffer(GL_PIXEL_UNPACK_BUFFER, scalar_field_buffer_object_id));
        (glTexSubImage3D(GL_TEXTURE_3D,    /* Use texture bound to GL_TEXTURE_3D                                     */
                         0,                /* Base image level                                                       */
                         0,                /* From the texture origin                                                */
                         0,                /* From the texture origin                                                */
                         0,                /* From the texture origin                                                */
                         samples_per_axis, /* Texture have same width as scalar field in buffer                      */
                         samples_per_axis, /* Texture have same height as scalar field in buffer                     */
                         samples_per_axis, /* Texture have same depth as scalar field in buffer                      */
                         GL_RED,           /* Scalar field gathered in buffer has only one component                 */
                         GL_FLOAT,         /* Scalar field gathered in buffer is of float type                       */
                         NULL              /* Scalar field gathered in buffer bound to GL_PIXEL_UNPACK_BUFFER target */
                         ));
        /* [Stage 2 Scalar field generation stage move data to texture] */

        /* 3. Marching cube algorithm cell splitting stage.
         *
         * At this stage we analyze isosurface in each cell of space and
         * assign one of 256 possible types to each cell. Cell type data
         * for each cell is stored in attached buffer.
         */
        /* Bind buffer to store cell type data. */
        (glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, marching_cubes_cells_transform_feedback_object_id));

        /* Shorten GL pipeline: we will use vertex shader only. */
        (glEnable(GL_RASTERIZER_DISCARD));
        {
            /* Select program for Marching Cubes algorthim's cell splitting stage. */
            (glUseProgram(shaderProgramObject_marching_cubes));

            /* Activate transform feedback mode. */
            (glBeginTransformFeedback(GL_POINTS));
            {
                /* [Stage 3 Execute vertex shader] */
                /* Run Marching Cubes algorithm cell splitting stage for all cells. */
                (glDrawArrays(GL_POINTS, 0, cells_in_3d_space));
                /* [Stage 3 Execute vertex shader] */
            }
            (glEndTransformFeedback());
        }
        (glDisable(GL_RASTERIZER_DISCARD));

        /* Unbind buffers used at this stage. */
        (glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0));

        /* Copy data from buffer into texture bound to target GL_TEXTURE2 in texture unit 2.
         * We need to move this data to a texture object, as there is no way we could access data
         * stored within a buffer object in a OpenGL ES 3.0 shader.
         */
        (glActiveTexture(GL_TEXTURE2));
        (glBindBuffer(GL_PIXEL_UNPACK_BUFFER, marching_cubes_cells_types_buffer_id));
        (glTexSubImage3D(GL_TEXTURE_3D,  /* Use texture bound to GL_TEXTURE_3D                                   */
                         0,              /* Base image level                                                     */
                         0,              /* From the texture origin                                              */
                         0,              /* From the texture origin                                              */
                         0,              /* From the texture origin                                              */
                         cells_per_axis, /* Texture have same width as cells by width in buffer                  */
                         cells_per_axis, /* Texture have same height as cells by height in buffer                */
                         cells_per_axis, /* Texture have same depth as cells by depth in buffer                  */
                         GL_RED_INTEGER, /* Cell types gathered in buffer have only one component                */
                         GL_INT,         /* Cell types gathered in buffer are of int type                        */
                         NULL            /* Cell types gathered in buffer bound to GL_PIXEL_UNPACK_BUFFER target */
                         ));

        /* 4. Marching Cubes algorithm triangle generation stage.
         *
         * At this stage, we render exactly (3 vertices * 5 triangles per cell *
         * amount of cells the scalar field is split to) triangle vertices.
         * Then render triangularized geometry.
         */
        (glActiveTexture(GL_TEXTURE0));

        /* Activate triangle generating and rendering program. */
        (glUseProgram(shaderProgramObject_marching_cube_triangles));

        /* Specify input arguments to vertex shader. */
        (glUniform1f(timeMarchingTriangleUniform, model_time));
        // (glUniformMatrix4fv(marching_cubes_triangles_uniform_modelMatrix_id, 1, GL_FALSE, modelMatrix));
        // (glUniformMatrix4fv(marching_cubes_triangles_uniform_viewMatrix_id, 1, GL_FALSE, viewMatrix));
        // (glUniformMatrix4fv(marching_cubes_triangles_uniform_projectionMatrix_id, 1, GL_FALSE, perspectiveProjectionMatrix));

        /* [Stage 4 Run triangle generating and rendering program] */
        /* Run triangle generating and rendering program. */
        glBindVertexArray(marching_cubes_triangles_vao_id);
        (glDrawArrays(GL_TRIANGLES, 0, cells_in_3d_space * triangles_per_cell * vertices_per_triangle));
        /* [Stage 4 Run triangle generating and rendering program] */
        glBindVertexArray(0);
    }

    void updateMetaballs(void)
    {
        model_time = model_time + 0.01f;
        if (model_time > 1000.0f)
            model_time = 0.0f;
    }

    void uninitializeFire(void)
    {

        if (vbo_texcoord_fire)
        {
            glDeleteBuffers(1, &vbo_texcoord_fire);
            vbo_texcoord_fire = 0;
        }

        if (vbo_position_fire)
        {
            glDeleteBuffers(1, &vbo_position_fire);
            vbo_position_fire = 0;
        }

        if (vao_fire)
        {
            glDeleteVertexArrays(1, &vao_fire);
            vao_fire = 0;
        }

        UninitializeShaders(shaderProgramObject_sphere_updater);
    }
};
