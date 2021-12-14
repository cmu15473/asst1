#ifndef GL_LOADER_H
#define GL_LOADER_H
#include <Windows.h>
#include "gl.h"
#include "wglext.h"
#include "glext.h"
#include "../LibString.h"

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")

class VBO
{
public:
	static bool CanUse;
	static PFNGLBINDBUFFERPROC    BindBuffer;
	static PFNGLBUFFERDATAPROC    BufferData;
	static PFNGLBUFFERSUBDATAPROC BufferSubData;
	static PFNGLDELETEBUFFERSPROC DeleteBuffers;
	static PFNGLGENBUFFERSPROC    GenBuffers;
	static PFNGLMAPBUFFERPROC     MapBuffer;
	static PFNGLUNMAPBUFFERPROC   UnmapBuffer;
	static void LoadProcs();
};

class Shader
{
private:
	static bool Load();
public:
	static bool CanUse;
	static PFNGLCREATESHADEROBJECTARBPROC CreateShaderObjectARB;
	static PFNGLCREATEPROGRAMOBJECTARBPROC CreateProgramObjectARB;
	static PFNGLATTACHOBJECTARBPROC AttachObjectARB;
	static PFNGLDETACHOBJECTARBPROC DetachObjectARB;
	static PFNGLDELETEOBJECTARBPROC DeleteObjectARB;
	static PFNGLSHADERSOURCEARBPROC ShaderSourceARB;
	static PFNGLCOMPILESHADERARBPROC CompileShaderARB;
	static PFNGLLINKPROGRAMARBPROC LinkProgramARB;
	static PFNGLVALIDATEPROGRAMARBPROC ValidateProgramARB;
	static PFNGLUSEPROGRAMOBJECTARBPROC UseProgramObjectARB;
	static PFNGLGETOBJECTPARAMETERIVARBPROC GetObjectParameterivARB;
	static PFNGLGETINFOLOGARBPROC GetInfoLogARB;
	static PFNGLUNIFORM1FARBPROC Uniform1fARB;
	static PFNGLUNIFORM2FARBPROC Uniform2fARB;
	static PFNGLUNIFORM3FARBPROC Uniform3fARB;
	static PFNGLUNIFORM4FARBPROC Uniform4fARB;
	static PFNGLUNIFORM1IARBPROC Uniform1iARB;
	static PFNGLUNIFORM1IVARBPROC Uniform1ivARB;
	static PFNGLUNIFORM2IARBPROC Uniform2iARB;
	static PFNGLUNIFORM3IARBPROC Uniform3iARB;
	static PFNGLUNIFORM4IARBPROC Uniform4iARB;
	static PFNGLUNIFORMMATRIX3FVARBPROC UniformMatrix3fv;
	static PFNGLUNIFORMMATRIX4FVARBPROC UniformMatrix4fv;
	static PFNGLGETUNIFORMLOCATIONARBPROC GetUniformLocationARB;
	static PFNGLBINDFRAGDATALOCATIONEXTPROC BindFragDataLocation;
	static void LoadProcs();
	static void LoadShaderFromFile(GLhandleARB ID, const CoreLib::Basic::String & FileName);
};

class HQuery
{
public:
	bool CanUse;
	PFNGLGENQUERIESPROC GenQueries;
	PFNGLDELETEQUERIESPROC DeleteQueries;
	PFNGLISQUERYPROC IsQuery;
	PFNGLBEGINQUERYPROC BeginQuery;
	PFNGLENDQUERYPROC EndQuery;
	PFNGLGETQUERYIVPROC GetQueryiv;
	PFNGLGETQUERYOBJECTIVPROC GetQueryObjectiv;
	PFNGLGETQUERYOBJECTUIVPROC GetQueryObjectuiv;
	HQuery();
	void LoadProcs();
};

class FBO
{
public:
	static bool CanUse;
	static PFNGLISRENDERBUFFEREXTPROC						IsRenderbuffer;
	static PFNGLBINDRENDERBUFFEREXTPROC						BindRenderbuffer;
	static PFNGLDELETERENDERBUFFERSEXTPROC					DeleteRenderbuffers;
	static PFNGLGENRENDERBUFFERSEXTPROC						GenRenderbuffers;
	static PFNGLRENDERBUFFERSTORAGEEXTPROC					RenderbufferStorage;
	static PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC		RenderbufferStorageMultisample;
	static PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC			GetRenderbufferParameteriv;
	static PFNGLISFRAMEBUFFEREXTPROC						IsFramebuffer;
	static PFNGLBINDFRAMEBUFFEREXTPROC						BindFramebuffer;
	static PFNGLDELETEFRAMEBUFFERSEXTPROC					DeleteFramebuffers;
	static PFNGLGENFRAMEBUFFERSEXTPROC						GenFramebuffers;
	static PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC				CheckFramebufferStatus;
	static PFNGLFRAMEBUFFERTEXTURE1DEXTPROC					FramebufferTexture1D;
	static PFNGLFRAMEBUFFERTEXTURE2DEXTPROC					FramebufferTexture2D;
	static PFNGLFRAMEBUFFERTEXTURE3DEXTPROC					FramebufferTexture3D;
	static PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC				FramebufferRenderbuffer;
	static PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC	GetFramebufferAttachmentParameteriv;
	static PFNGLBLITFRAMEBUFFEREXTPROC						BlitFramebuffer;
	static PFNGLGENERATEMIPMAPEXTPROC						GenerateMipmap;
	static PFNGLDRAWBUFFERSPROC								DrawBuffers;
	static void PrintFBOErrorMessage(int fboStatus);
	static void LoadProcs();
};

class MultiTexture
{
public:
	static bool CanUse;
	static PFNGLACTIVETEXTUREPROC ActiveTexture;
	static PFNGLCLIENTACTIVETEXTUREPROC ClientActiveTexture;
	static PFNGLMULTITEXCOORD1FPROC MultiTexcoord1f;
	static PFNGLMULTITEXCOORD2FPROC MultiTexcoord2f;
	static PFNGLMULTITEXCOORD3FPROC MultiTexcoord3f;
	static PFNGLMULTITEXCOORD4FPROC MultiTexcoord4f;
	static void LoadProcs();
};

#endif