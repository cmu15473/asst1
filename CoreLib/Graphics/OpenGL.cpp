#include "OpenGL.h"
#include "../WinForm/Debug.h"
#include "../Imaging/Bitmap.h"

namespace CoreLib
{
	namespace Graphics
	{
		namespace OpenGL
		{
			PFNGLCREATESHADERPROC							GL::CreateShader;
			PFNGLCREATEPROGRAMPROC							GL::CreateProgram;
			PFNGLDELETESHADERPROC							GL::DeleteShader;
			PFNGLDELETEPROGRAMPROC							GL::DeleteProgram;
			PFNGLATTACHSHADERPROC							GL::AttachShader;
			PFNGLDETACHSHADERPROC							GL::DetachShader;
			PFNGLSHADERSOURCEPROC							GL::ShaderSource;
			PFNGLCOMPILESHADERPROC							GL::CompileShader;
			PFNGLLINKPROGRAMPROC							GL::LinkProgram;
			PFNGLVALIDATEPROGRAMPROC						GL::ValidateProgram;
			PFNGLUSEPROGRAMPROC								GL::UseProgram;
			PFNGLGETSHADERINFOLOGPROC						GL::GetShaderInfoLog;
			PFNGLGETPROGRAMINFOLOGPROC						GL::GetProgramInfoLog;
			PFNGLGETSHADERIVPROC							GL::GetShaderiv;
			PFNGLGETPROGRAMIVPROC							GL::GetProgramiv;
			PFNGLUNIFORM1FPROC								GL::Uniform1f;
			PFNGLUNIFORM2FPROC								GL::Uniform2f;
			PFNGLUNIFORM3FPROC								GL::Uniform3f;
			PFNGLUNIFORM4FPROC								GL::Uniform4f;
			PFNGLUNIFORM1IPROC								GL::Uniform1i;
			PFNGLUNIFORM1IVPROC								GL::Uniform1iv;
			PFNGLUNIFORM2IPROC								GL::Uniform2i;
			PFNGLUNIFORM3IPROC								GL::Uniform3i;
			PFNGLUNIFORM4IPROC								GL::Uniform4i;
			PFNGLUNIFORMMATRIX3FVPROC						GL::UniformMatrix3fv;
			PFNGLUNIFORMMATRIX4FVPROC						GL::UniformMatrix4fv;
			PFNGLGETUNIFORMLOCATIONPROC						GL::GetUniformLocation;
			PFNGLBINDFRAGDATALOCATIONEXTPROC				GL::BindFragDataLocation;
			PFNGLISRENDERBUFFEREXTPROC						GL::IsRenderbuffer;
			PFNGLBINDRENDERBUFFEREXTPROC					GL::BindRenderbuffer;
			PFNGLDELETERENDERBUFFERSEXTPROC					GL::DeleteRenderbuffers;
			PFNGLGENRENDERBUFFERSEXTPROC					GL::GenRenderbuffers;
			PFNGLRENDERBUFFERSTORAGEEXTPROC					GL::RenderbufferStorage;
			PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC		GL::RenderbufferStorageMultisample;
			PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC			GL::GetRenderbufferParameteriv;
			PFNGLISFRAMEBUFFEREXTPROC						GL::IsFramebuffer;
			PFNGLBINDFRAMEBUFFEREXTPROC						GL::BindFramebuffer;
			PFNGLDELETEFRAMEBUFFERSEXTPROC					GL::DeleteFramebuffers;
			PFNGLGENFRAMEBUFFERSEXTPROC						GL::GenFramebuffers;
			PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC				GL::CheckFramebufferStatus;
			PFNGLFRAMEBUFFERTEXTURE1DEXTPROC				GL::FramebufferTexture1D;
			PFNGLFRAMEBUFFERTEXTURE2DEXTPROC				GL::FramebufferTexture2D;
			PFNGLFRAMEBUFFERTEXTURE3DEXTPROC				GL::FramebufferTexture3D;
			PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC				GL::FramebufferRenderbuffer;
			PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC	GL::GetFramebufferAttachmentParameteriv;
			PFNGLBLITFRAMEBUFFEREXTPROC						GL::BlitFramebuffer;
			PFNGLGENERATEMIPMAPEXTPROC						GL::GenerateMipmap;
			PFNGLDRAWBUFFERSPROC							GL::DrawBuffers;
			PFNGLACTIVETEXTUREPROC							GL::ActiveTexture;
			PFNGLCLIENTACTIVETEXTUREPROC					GL::ClientActiveTexture;
			PFNGLMULTITEXCOORD1FPROC						GL::MultiTexcoord1f;
			PFNGLMULTITEXCOORD2FPROC						GL::MultiTexcoord2f;
			PFNGLMULTITEXCOORD3FPROC						GL::MultiTexcoord3f;
			PFNGLMULTITEXCOORD4FPROC						GL::MultiTexcoord4f;
			PFNGLBINDBUFFERPROC								GL::BindBuffer;
			PFNGLBUFFERDATAPROC								GL::BufferData;
			PFNGLBUFFERSUBDATAPROC							GL::BufferSubData;
			PFNGLDELETEBUFFERSPROC							GL::DeleteBuffers;
			PFNGLGENBUFFERSPROC								GL::GenBuffers;
			PFNGLMAPBUFFERPROC								GL::MapBuffer;
			PFNGLUNMAPBUFFERPROC							GL::UnmapBuffer;
			
			void InitializeOpenGLExtensions()
			{
				GL::Load();
			}

			Shader::Shader(ShaderType shaderType, const String & name, const String & source)
			{
				handle = GL::CreateShader((int)shaderType);
				const char * src = source.ToMultiByteString();
				int length = (int)strlen(src);
				GL::ShaderSource(handle, 1, &src, &length);
				GL::CompileShader(handle);
				GL::GetShaderiv(handle, GL_INFO_LOG_LENGTH, &length);
				List<char> buffer;
				buffer.SetSize(length);
				GL::GetShaderInfoLog(handle,length, &length, buffer.Buffer());
				int compileStatus;
				GL::GetShaderiv(handle, GL_COMPILE_STATUS, &compileStatus);
				String logOutput(buffer.Buffer());
				CoreLib::Diagnostics::DebugWriter dbgWriter;
				if (length > 0)
					dbgWriter << logOutput;
				if (compileStatus != GL_TRUE)
				{
					throw ShaderCompileException(name, String(buffer.Buffer()));
				}
			}

			Shader::~Shader()
			{
				GL::DeleteShader(handle);
			}

			Program::Program()
			{
				handle = GL::CreateProgram();
			}

			Program::~Program()
			{
				GL::DeleteProgram(handle);
			}

			void Program::AttachShader(Shader * shader)
			{
				GL::AttachShader(handle, shader->GetHandle());
			}

			void Program::Link()
			{
				int length, compileStatus;
				List<char> buffer;
				CoreLib::Diagnostics::DebugWriter dbgWriter;

				GL::LinkProgram(handle);

				GL::GetProgramiv(handle, GL_INFO_LOG_LENGTH, &length);
				buffer.SetSize(length);
				GL::GetProgramInfoLog(handle, length, &length, buffer.Buffer());
				GL::GetProgramiv(handle, GL_LINK_STATUS, &compileStatus);
				String logOutput(buffer.Buffer());
				
				if (length > 0)
					dbgWriter << logOutput;
				if (compileStatus != GL_TRUE)
				{
					throw ShaderCompileException(L"program linking", logOutput);
				}

				GL::ValidateProgram(handle);
				GL::GetProgramiv(handle, GL_INFO_LOG_LENGTH, &length);
				buffer.SetSize(length);
				GL::GetProgramInfoLog(handle,length, &length, buffer.Buffer());
				logOutput = buffer.Buffer();
				if (length > 0)
					dbgWriter << logOutput;
				GL::GetProgramiv(handle, GL_VALIDATE_STATUS, &compileStatus);
				if (compileStatus != GL_TRUE)
				{
					throw ShaderCompileException(L"program validation", logOutput);
				}
			}

			void Program::SetUniform(const char * name, int value)
			{
				int loc = GL::GetUniformLocation(handle, name);
				GL::Uniform1i(loc, value);
			}

			void Program::SetUniform(const char * name, float value)
			{
				int loc = GL::GetUniformLocation(handle, name);
				GL::Uniform1f(loc, value);
			}

			void Program::SetUniform(const char * name, const Vec3 & value)
			{
				int loc = GL::GetUniformLocation(handle, name);
				GL::Uniform3f(loc, value.x, value.y, value.z);
			}

			void Program::SetUniform(const char * name, const Vec4 & value)
			{
				int loc = GL::GetUniformLocation(handle, name);
				GL::Uniform4f(loc, value.x, value.y, value.z, value.w);
			}

			void Program::SetUniform(const char * name, const Matrix4 & value, bool transpose)
			{
				int loc = GL::GetUniformLocation(handle, name);
				GL::UniformMatrix4fv(loc, 16, transpose, value.values);
			}

			void Program::BindFragDataLocation(const char * name, int loc)
			{
				GL::BindFragDataLocation(handle, loc, name);
			}

			void Program::Use()
			{
				GL::UseProgram(handle);
			}

			void Program::Unuse()
			{
				GL::UseProgram(0);
			}

			Texture2D * Texture2D::Create(const String & fileName)
			{
				CoreLib::Imaging::Bitmap bmp(fileName);
				auto rs = new Texture2D();
				rs->Load(ImageFormat::RGBA, ImageFormat::RGBA, ImageDataType::Byte, bmp.GetPixels(), bmp.GetWidth(), bmp.GetHeight());
				return rs;
			}
		}
	}
}