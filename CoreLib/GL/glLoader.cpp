#include "glLoader.h"
#include "stdio.h"
#include "../Basic.h"
#include "../LibIO.h"

#define hGetProcAddress wglGetProcAddress

using namespace CoreLib::Basic;
using namespace CoreLib::IO;

bool											VBO::CanUse = false;
PFNGLBINDBUFFERPROC								VBO::BindBuffer;
PFNGLBUFFERDATAPROC    							VBO::BufferData;
PFNGLBUFFERSUBDATAPROC  						VBO::BufferSubData;
PFNGLDELETEBUFFERSPROC  						VBO::DeleteBuffers;
PFNGLGENBUFFERSPROC     						VBO::GenBuffers;
PFNGLMAPBUFFERPROC      						VBO::MapBuffer;
PFNGLUNMAPBUFFERPROC    						VBO::UnmapBuffer;

bool											Shader::CanUse;
PFNGLCREATESHADEROBJECTARBPROC					Shader::CreateShaderObjectARB;
PFNGLCREATEPROGRAMOBJECTARBPROC					Shader::CreateProgramObjectARB;
PFNGLATTACHOBJECTARBPROC						Shader::AttachObjectARB;
PFNGLDETACHOBJECTARBPROC						Shader::DetachObjectARB;
PFNGLDELETEOBJECTARBPROC						Shader::DeleteObjectARB;
PFNGLSHADERSOURCEARBPROC						Shader::ShaderSourceARB;
PFNGLCOMPILESHADERARBPROC						Shader::CompileShaderARB;
PFNGLLINKPROGRAMARBPROC							Shader::LinkProgramARB;
PFNGLVALIDATEPROGRAMARBPROC						Shader::ValidateProgramARB;
PFNGLUSEPROGRAMOBJECTARBPROC					Shader::UseProgramObjectARB;
PFNGLGETOBJECTPARAMETERIVARBPROC				Shader::GetObjectParameterivARB;
PFNGLGETINFOLOGARBPROC							Shader::GetInfoLogARB;
PFNGLUNIFORM1FARBPROC							Shader::Uniform1fARB;
PFNGLUNIFORM2FARBPROC							Shader::Uniform2fARB;
PFNGLUNIFORM3FARBPROC							Shader::Uniform3fARB;
PFNGLUNIFORM4FARBPROC							Shader::Uniform4fARB;
PFNGLUNIFORM1IARBPROC							Shader::Uniform1iARB;
PFNGLUNIFORM1IVARBPROC							Shader::Uniform1ivARB;
PFNGLUNIFORM2IARBPROC							Shader::Uniform2iARB;
PFNGLUNIFORM3IARBPROC							Shader::Uniform3iARB;
PFNGLUNIFORM4IARBPROC							Shader::Uniform4iARB;
PFNGLUNIFORMMATRIX3FVARBPROC					Shader::UniformMatrix3fv;
PFNGLUNIFORMMATRIX4FVARBPROC					Shader::UniformMatrix4fv;
PFNGLBINDFRAGDATALOCATIONEXTPROC				Shader::BindFragDataLocation;
PFNGLGETUNIFORMLOCATIONARBPROC					Shader::GetUniformLocationARB;

bool											FBO::CanUse;
PFNGLISRENDERBUFFEREXTPROC						FBO::IsRenderbuffer;
PFNGLBINDRENDERBUFFEREXTPROC					FBO::BindRenderbuffer;
PFNGLDELETERENDERBUFFERSEXTPROC					FBO::DeleteRenderbuffers;
PFNGLGENRENDERBUFFERSEXTPROC					FBO::GenRenderbuffers;
PFNGLRENDERBUFFERSTORAGEEXTPROC					FBO::RenderbufferStorage;
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC		FBO::RenderbufferStorageMultisample;
PFNGLBLITFRAMEBUFFEREXTPROC						FBO::BlitFramebuffer;
PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC			FBO::GetRenderbufferParameteriv;
PFNGLISFRAMEBUFFEREXTPROC						FBO::IsFramebuffer;
PFNGLBINDFRAMEBUFFEREXTPROC						FBO::BindFramebuffer;
PFNGLDELETEFRAMEBUFFERSEXTPROC					FBO::DeleteFramebuffers;
PFNGLGENFRAMEBUFFERSEXTPROC						FBO::GenFramebuffers;
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC				FBO::CheckFramebufferStatus;
PFNGLFRAMEBUFFERTEXTURE1DEXTPROC				FBO::FramebufferTexture1D;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC				FBO::FramebufferTexture2D;
PFNGLFRAMEBUFFERTEXTURE3DEXTPROC				FBO::FramebufferTexture3D;
PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC				FBO::FramebufferRenderbuffer;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC FBO::GetFramebufferAttachmentParameteriv;
PFNGLGENERATEMIPMAPEXTPROC						FBO::GenerateMipmap;
PFNGLDRAWBUFFERSPROC							FBO::DrawBuffers;


bool											MultiTexture::CanUse;
PFNGLACTIVETEXTUREPROC							MultiTexture::ActiveTexture;
PFNGLCLIENTACTIVETEXTUREPROC					MultiTexture::ClientActiveTexture;
PFNGLMULTITEXCOORD1FPROC						MultiTexture::MultiTexcoord1f;
PFNGLMULTITEXCOORD2FPROC						MultiTexture::MultiTexcoord2f;
PFNGLMULTITEXCOORD3FPROC						MultiTexture::MultiTexcoord3f;
PFNGLMULTITEXCOORD4FPROC						MultiTexture::MultiTexcoord4f;



void Shader::LoadProcs()
{
	CanUse = Load();
}

bool Shader::Load()
{
	CreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC)hGetProcAddress("glCreateShaderObjectARB");
	CreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC)hGetProcAddress("glCreateProgramObjectARB");
	AttachObjectARB = (PFNGLATTACHOBJECTARBPROC)hGetProcAddress("glAttachObjectARB");
	DetachObjectARB = (PFNGLDETACHOBJECTARBPROC)hGetProcAddress("glDetachObjectARB");
	DeleteObjectARB = (PFNGLDELETEOBJECTARBPROC)hGetProcAddress("glDeleteObjectARB");
	ShaderSourceARB = (PFNGLSHADERSOURCEARBPROC)hGetProcAddress("glShaderSourceARB");
	CompileShaderARB = (PFNGLCOMPILESHADERARBPROC)hGetProcAddress("glCompileShaderARB");
	LinkProgramARB = (PFNGLLINKPROGRAMARBPROC)hGetProcAddress("glLinkProgramARB");
	ValidateProgramARB = (PFNGLVALIDATEPROGRAMARBPROC)hGetProcAddress("glValidateProgramARB");
	UseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC)hGetProcAddress("glUseProgramObjectARB");
	GetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC)hGetProcAddress("glGetObjectParameterivARB");
	GetInfoLogARB = (PFNGLGETINFOLOGARBPROC)hGetProcAddress("glGetInfoLogARB");
	Uniform1fARB = (PFNGLUNIFORM1FARBPROC)hGetProcAddress("glUniform1fARB");
	Uniform2fARB = (PFNGLUNIFORM2FARBPROC)hGetProcAddress("glUniform2fARB");
	Uniform3fARB = (PFNGLUNIFORM3FARBPROC)hGetProcAddress("glUniform3fARB");
	Uniform4fARB = (PFNGLUNIFORM4FARBPROC)hGetProcAddress("glUniform4fARB");
	Uniform1iARB = (PFNGLUNIFORM1IARBPROC)hGetProcAddress("glUniform1iARB");
	Uniform1ivARB = (PFNGLUNIFORM1IVARBPROC)hGetProcAddress("glUniform1ivARB");
	Uniform2iARB = (PFNGLUNIFORM2IARBPROC)hGetProcAddress("glUniform2iARB");
	Uniform3iARB = (PFNGLUNIFORM3IARBPROC)hGetProcAddress("glUniform3iARB");
	Uniform4iARB = (PFNGLUNIFORM4IARBPROC)hGetProcAddress("glUniform4iARB");
	UniformMatrix3fv = (PFNGLUNIFORMMATRIX3FVARBPROC)hGetProcAddress("glUniformMatrix3fvARB");
	UniformMatrix4fv = (PFNGLUNIFORMMATRIX3FVARBPROC)hGetProcAddress("glUniformMatrix4fvARB");
	GetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONARBPROC)hGetProcAddress("glGetUniformLocationARB");
	BindFragDataLocation = (PFNGLBINDFRAGDATALOCATIONEXTPROC)hGetProcAddress("glBindFragDataLocationEXT");
	return (CreateShaderObjectARB && CreateProgramObjectARB && AttachObjectARB &&
			DetachObjectARB && DeleteObjectARB && ShaderSourceARB && CompileShaderARB
			&& LinkProgramARB && ValidateProgramARB && UseProgramObjectARB &&
			GetObjectParameterivARB && GetInfoLogARB && Uniform1fARB && 
			GetUniformLocationARB && BindFragDataLocation && UniformMatrix3fv);
}

void Shader::LoadShaderFromFile(GLhandleARB ID, const String & FileName)
{
	String src = File::ReadAllText(FileName);
	const char * str = src.ToMultiByteString();
	ShaderSourceARB(ID,1, &str,NULL);
}

void VBO::LoadProcs()
{
	BindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
	BufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
	DeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
	GenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
	if (!BindBuffer || !BufferData || !DeleteBuffers || !GenBuffers)
	{
		BindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBufferARB");
		BufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferDataARB");
		DeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffersARB");
		GenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffersARB");
		if (!BindBuffer || !BufferData || !DeleteBuffers || !GenBuffers)
		{
			CanUse = false;
		}
		else
		{
			CanUse = true;
		}
	}
	else
	{
		CanUse = true;
	}
}


HQuery::HQuery()
{
	LoadProcs();
}

void HQuery::LoadProcs()
{
	BeginQuery = (PFNGLBEGINQUERYPROC)wglGetProcAddress("glBeginQuery");
	DeleteQueries = (PFNGLDELETEQUERIESPROC)hGetProcAddress("glDeleteQueries");
	EndQuery = (PFNGLENDQUERYPROC)hGetProcAddress("glEndQuery");
	GenQueries = (PFNGLGENQUERIESPROC)hGetProcAddress("glGenQueries");
	GetQueryiv = (PFNGLGETQUERYIVPROC)hGetProcAddress("glGetQueryiv");
	GetQueryObjectiv = (PFNGLGETQUERYOBJECTIVPROC)hGetProcAddress("glGetQueryObjectiv");
	GetQueryObjectuiv = (PFNGLGETQUERYOBJECTUIVPROC)hGetProcAddress("glGetQueryObjectuiv");
	IsQuery = (PFNGLISQUERYPROC)hGetProcAddress("glIsQuery");
	if (!BeginQuery || !DeleteQueries || !EndQuery || !GenQueries || !GetQueryObjectiv
		|| !IsQuery)
	{
		BeginQuery = (PFNGLBEGINQUERYPROC)wglGetProcAddress("glBeginQueryARB");
		DeleteQueries = (PFNGLDELETEQUERIESPROC)hGetProcAddress("glDeleteQueriesARB");
		EndQuery = (PFNGLENDQUERYPROC)hGetProcAddress("glEndQueryARB");
		GenQueries = (PFNGLGENQUERIESPROC)hGetProcAddress("glGenQueriesARB");
		GetQueryiv = (PFNGLGETQUERYIVPROC)hGetProcAddress("glGetQueryivARB");
		GetQueryObjectiv = (PFNGLGETQUERYOBJECTIVPROC)hGetProcAddress("glGetQueryObjectivARB");
		GetQueryObjectuiv = (PFNGLGETQUERYOBJECTUIVPROC)hGetProcAddress("glGetQueryObjectuivARB");
		IsQuery = (PFNGLISQUERYPROC)hGetProcAddress("glIsQueryARB");
	}
	CanUse = !(!BeginQuery || !DeleteQueries || !EndQuery || !GenQueries || !GetQueryObjectiv
		|| !IsQuery);
}



void FBO::LoadProcs()
{
	IsRenderbuffer = (PFNGLISRENDERBUFFEREXTPROC)wglGetProcAddress("glIsRenderbuffer");
	BindRenderbuffer = (PFNGLBINDRENDERBUFFEREXTPROC)wglGetProcAddress("glBindRenderbuffer");
	DeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSEXTPROC)wglGetProcAddress("glDeleteRenderbuffers");
	GenRenderbuffers = (PFNGLGENRENDERBUFFERSEXTPROC)wglGetProcAddress("glGenRenderbuffers");
	RenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEEXTPROC)wglGetProcAddress("glRenderbufferStorage");
	GetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC)wglGetProcAddress("glGetRenderbufferParameteriv");
	IsFramebuffer = (PFNGLISFRAMEBUFFEREXTPROC)wglGetProcAddress("glIsFramebuffer");
	BindFramebuffer = (PFNGLBINDFRAMEBUFFEREXTPROC)wglGetProcAddress("glBindFramebuffer");
	DeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSEXTPROC)wglGetProcAddress("glDeleteFramebuffers");
	GenFramebuffers = (PFNGLGENFRAMEBUFFERSEXTPROC)wglGetProcAddress("glGenFramebuffers");
	CheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)wglGetProcAddress("glCheckFramebufferStatus");
	FramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1DEXTPROC)wglGetProcAddress("glFramebufferTexture1D");
	FramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)wglGetProcAddress("glFramebufferTexture2D");
	FramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DEXTPROC)wglGetProcAddress("glFramebufferTexture3D");
	FramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)wglGetProcAddress("glFramebufferRenderbuffer");
	GetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC)wglGetProcAddress("glGetFramebufferAttachmentParameteriv");
	BlitFramebuffer = (PFNGLBLITFRAMEBUFFEREXTPROC)wglGetProcAddress("glBlitFramebuffer");
	RenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC)wglGetProcAddress("glRenderbufferStorageMultisample");
	GenerateMipmap = (PFNGLGENERATEMIPMAPEXTPROC)wglGetProcAddress("glGenerateMipmap");
	DrawBuffers = (PFNGLDRAWBUFFERSPROC)wglGetProcAddress("glDrawBuffers");
	CanUse = (IsRenderbuffer && BindRenderbuffer && DeleteRenderbuffers
		&& GenRenderbuffers && RenderbufferStorage && GetRenderbufferParameteriv
		&& IsFramebuffer && BindFramebuffer && GenFramebuffers && FramebufferTexture2D
		&& FramebufferRenderbuffer && GetFramebufferAttachmentParameteriv 
		&& GenerateMipmap && DrawBuffers);
}

void FBO::PrintFBOErrorMessage(int status)
{
	const char * msg = 0;
	switch(status)
	{
	case GL_FRAMEBUFFER_UNDEFINED:
		msg = "GL_FRAMEBUFFER_UNDEFINED (target is the default framebuffer, but the default framebuffer does not exist)";
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		msg = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT (any of the framebuffer attachment points are framebuffer incomplete)";
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		msg = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT (the framebuffer does not have at least one image attached to it)";
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		msg = "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER (the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for any color attachment point(s) named by GL_DRAW_BUFFERi)";
		break;
	case GL_FRAMEBUFFER_UNSUPPORTED:
		msg = "GL_FRAMEBUFFER_UNSUPPORTED (combination of internal formats of the attached images violates an implementation-dependent set of restrictions)";
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
		msg = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE (the value of GL_RENDERBUFFER_SAMPLES is not the same for all attached renderbuffers; if the value of GL_TEXTURE_SAMPLES is the not same for all attached textures; or, if the attached images are a mix of renderbuffers and textures, the value of GL_RENDERBUFFER_SAMPLES does not match the value of GL_TEXTURE_SAMPLES)";
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
		msg = "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS (any framebuffer attachment is layered, and any populated attachment is not layered, or if all populated color attachments are not from textures of the same target)";
		break;
	}
	if (msg)
		printf("%s\n", msg);
}

void MultiTexture::LoadProcs()
{
	CanUse = false;
	ActiveTexture = (PFNGLACTIVETEXTUREPROC)hGetProcAddress("glActiveTexture");
	ClientActiveTexture = (PFNGLCLIENTACTIVETEXTUREPROC)hGetProcAddress("glClientActiveTexture");
	MultiTexcoord1f = (PFNGLMULTITEXCOORD1FPROC)hGetProcAddress("glMultiTexCoord1f");
	MultiTexcoord2f = (PFNGLMULTITEXCOORD2FPROC)hGetProcAddress("glMultiTexCoord2f");
	MultiTexcoord3f = (PFNGLMULTITEXCOORD3FPROC)hGetProcAddress("glMultiTexCoord3f");
	MultiTexcoord4f = (PFNGLMULTITEXCOORD4FPROC)hGetProcAddress("glMultiTexCoord4f");
	CanUse = (ActiveTexture && ClientActiveTexture && MultiTexcoord2f);
}