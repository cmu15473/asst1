#ifndef CORELIB_OPENGL_H
#define CORELIB_OPENGL_H

#include "../Basic.h"
#include "../VectorMath.h"
#include <Windows.h>
#include "../GL/gl.h"
#include "../GL/glext.h"

using namespace CoreLib::Basic;
using namespace VectorMath;

namespace CoreLib
{
	namespace Graphics
	{
		namespace OpenGL
		{
			// OpenGL Extension Loader
			class GL
			{
			public:
				static PFNGLCREATESHADERPROC							CreateShader;
				static PFNGLCREATEPROGRAMPROC							CreateProgram;
				static PFNGLDELETESHADERPROC							DeleteShader;
				static PFNGLDELETEPROGRAMPROC							DeleteProgram;
				static PFNGLATTACHSHADERPROC							AttachShader;
				static PFNGLDETACHSHADERPROC							DetachShader;
				static PFNGLSHADERSOURCEPROC							ShaderSource;
				static PFNGLCOMPILESHADERPROC							CompileShader;
				static PFNGLLINKPROGRAMPROC								LinkProgram;
				static PFNGLVALIDATEPROGRAMPROC							ValidateProgram;
				static PFNGLUSEPROGRAMPROC								UseProgram;
				static PFNGLGETSHADERINFOLOGPROC						GetShaderInfoLog;
				static PFNGLGETPROGRAMINFOLOGPROC						GetProgramInfoLog;
				static PFNGLGETSHADERIVPROC								GetShaderiv;
				static PFNGLGETPROGRAMIVPROC							GetProgramiv;
				static PFNGLUNIFORM1FPROC								Uniform1f;
				static PFNGLUNIFORM2FPROC								Uniform2f;
				static PFNGLUNIFORM3FPROC								Uniform3f;
				static PFNGLUNIFORM4FPROC								Uniform4f;
				static PFNGLUNIFORM1IPROC								Uniform1i;
				static PFNGLUNIFORM1IVPROC								Uniform1iv;
				static PFNGLUNIFORM2IPROC								Uniform2i;
				static PFNGLUNIFORM3IPROC								Uniform3i;
				static PFNGLUNIFORM4IPROC								Uniform4i;
				static PFNGLUNIFORMMATRIX3FVPROC						UniformMatrix3fv;
				static PFNGLUNIFORMMATRIX4FVPROC						UniformMatrix4fv;
				static PFNGLGETUNIFORMLOCATIONPROC						GetUniformLocation;
				static PFNGLBINDFRAGDATALOCATIONEXTPROC					BindFragDataLocation;
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
				static PFNGLACTIVETEXTUREPROC							ActiveTexture;
				static PFNGLCLIENTACTIVETEXTUREPROC						ClientActiveTexture;
				static PFNGLMULTITEXCOORD1FPROC							MultiTexcoord1f;
				static PFNGLMULTITEXCOORD2FPROC							MultiTexcoord2f;
				static PFNGLMULTITEXCOORD3FPROC							MultiTexcoord3f;
				static PFNGLMULTITEXCOORD4FPROC							MultiTexcoord4f;
				static PFNGLBINDBUFFERPROC								BindBuffer;
				static PFNGLBUFFERDATAPROC								BufferData;
				static PFNGLBUFFERSUBDATAPROC							BufferSubData;
				static PFNGLDELETEBUFFERSPROC							DeleteBuffers;
				static PFNGLGENBUFFERSPROC								GenBuffers;
				static PFNGLMAPBUFFERPROC								MapBuffer;
				static PFNGLUNMAPBUFFERPROC								UnmapBuffer;

				static void Load()
				{
					CreateShader = (PFNGLCREATESHADEROBJECTARBPROC)wglGetProcAddress("glCreateShaderObjectARB");
					CreateProgram = (PFNGLCREATEPROGRAMOBJECTARBPROC)wglGetProcAddress("glCreateProgramObjectARB");
					AttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
					DetachShader = (PFNGLDETACHSHADERPROC)wglGetProcAddress("glDetachShader");
					DeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
					DeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");
					ShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
					CompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
					LinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
					ValidateProgram = (PFNGLVALIDATEPROGRAMPROC)wglGetProcAddress("glValidateProgram");
					UseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
					GetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
					GetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog");
					GetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
					GetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
					Uniform1f = (PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f");
					Uniform2f = (PFNGLUNIFORM2FPROC)wglGetProcAddress("glUniform2f");
					Uniform3f = (PFNGLUNIFORM3FPROC)wglGetProcAddress("glUniform3f");
					Uniform4f = (PFNGLUNIFORM4FPROC)wglGetProcAddress("glUniform4f");
					Uniform1i = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");
					Uniform1iv = (PFNGLUNIFORM1IVPROC)wglGetProcAddress("glUniform1iv");
					Uniform2i = (PFNGLUNIFORM2IPROC)wglGetProcAddress("glUniform2i");
					Uniform3i = (PFNGLUNIFORM3IPROC)wglGetProcAddress("glUniform3i");
					Uniform4i = (PFNGLUNIFORM4IPROC)wglGetProcAddress("glUniform4i");
					UniformMatrix3fv = (PFNGLUNIFORMMATRIX3FVPROC)wglGetProcAddress("glUniformMatrix3fv");
					UniformMatrix4fv = (PFNGLUNIFORMMATRIX3FVPROC)wglGetProcAddress("glUniformMatrix4fv");
					GetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
					BindFragDataLocation = (PFNGLBINDFRAGDATALOCATIONEXTPROC)wglGetProcAddress("glBindFragDataLocationEXT");
					
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

					ActiveTexture = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
					ClientActiveTexture = (PFNGLCLIENTACTIVETEXTUREPROC)wglGetProcAddress("glClientActiveTexture");
					MultiTexcoord1f = (PFNGLMULTITEXCOORD1FPROC)wglGetProcAddress("glMultiTexCoord1f");
					MultiTexcoord2f = (PFNGLMULTITEXCOORD2FPROC)wglGetProcAddress("glMultiTexCoord2f");
					MultiTexcoord3f = (PFNGLMULTITEXCOORD3FPROC)wglGetProcAddress("glMultiTexCoord3f");
					MultiTexcoord4f = (PFNGLMULTITEXCOORD4FPROC)wglGetProcAddress("glMultiTexCoord4f");

					BindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
					BufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
					DeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
					GenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
				}
			};

			enum class ShaderType
			{
				VertexShader = 0x8B31, FragmentShader = 0x8B30
			};

			class ShaderCompileException : public Exception
			{
			public:
				String ShaderName;
				String ErrorInfo;
				ShaderCompileException(String shaderName, String error)
					: Exception(L"GLSL error in '" + shaderName + L"':\n" + error)
				{
					ShaderName = shaderName;
					error = ErrorInfo;
				}
			};

			class Shader : public Object
			{
			private:
				unsigned int handle;
			public:
				Shader(ShaderType type, const String & name, const String & source);
				virtual ~Shader();

				inline unsigned int GetHandle()
				{
					return handle;
				}
			};

			class Program : public Object
			{
			private:
				unsigned int handle;
			public:
				Program();
				virtual ~Program();

				inline unsigned int GetHandle()
				{
					return handle;
				}

				void AttachShader(Shader * shader);
				void Link();
				void SetUniform(const char * name, int value);
				void SetUniform(const char * name, float value);
				void SetUniform(const char * name, const Vec3 & value);
				void SetUniform(const char * name, const Vec4 & value);
				void SetUniform(const char * name, const Matrix4 & value, bool transpose=false);
				void BindFragDataLocation(const char * name, int loc);
				void Use();
				static void Unuse();
			};

			template<int target>
			class Buffer : public Object
			{
			private:
				unsigned int handle;
			public:
				Buffer()
				{
					GL::GenBuffers(1, &handle);
				}
				inline unsigned int GetHandle()
				{
					return handle;
				}
				virtual ~Buffer()
				{
					GL::DeleteBuffers(1, &handle);
				}
				void SetData(int sizeInBytes, void * data)
				{
					GL::BindBuffer(target, handle);
					GL::BufferData(target, sizeInBytes, data, GL_STATIC_DRAW);
				}
				void Bind()
				{
					GL::BindBuffer(target, handle);
				}
				static void UnBind()
				{
					GL::BindBuffer(target, 0);
				}
			};

			typedef Buffer<GL_ARRAY_BUFFER> VertexBuffer;
			typedef Buffer<GL_ELEMENT_ARRAY_BUFFER> IndexBuffer;
			typedef Buffer<GL_UNIFORM_BUFFER> UniformBuffer;
			typedef Buffer<GL_TEXTURE_BUFFER> TextureBuffer;
			typedef Buffer<GL_TRANSFORM_FEEDBACK_BUFFER> TransformFeedbackBuffer;

			enum class ImageDataType
			{
				Byte=GL_UNSIGNED_BYTE, Float=GL_FLOAT
			};

			enum class TextureFilter
			{
				Nearest, Linear, TriLinear, Anisotropic4x, Anisotropic8x, Anisotropic16x
			};

			enum class ImageFormat
			{
				Luminance = GL_LUMINANCE, RGB = GL_RGB, RGBA = GL_RGBA,
				RGBA_32F = GL_RGBA32F, RGBA_16F = GL_RGBA16F, 
				R11_G11_B10 = GL_R11F_G11F_B10F,
				RGB10_A2 = GL_RGB10_A2,
				Depth16 = GL_DEPTH_COMPONENT16,
				Depth24 = GL_DEPTH_COMPONENT24,
				Depth32 = GL_DEPTH_COMPONENT32,
				DXT1_RGB = GL_COMPRESSED_RGB_S3TC_DXT1_EXT,
				DXT1_RGBA = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
				DXT3_RGBA = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
				DXT5_RGBA = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
			};

			template<int target>
			class Texture : public Object
			{
			private:
				unsigned int handle;
			public:
				Texture()
				{
					glGenTextures(1, &handle);
					Bind();
					glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
					glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
					glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_REPEAT);
					SetFilter(TextureFilter::Anisotropic16x);
				}
				~Texture()
				{
					glDeleteTextures(1, &handle);
				}
			public:
				inline unsigned int GetHandle()
				{
					return handle;
				}
				void Load(ImageFormat internalFormat, ImageFormat inputFormat, ImageDataType inputType, void *data, int width)
				{
					glBindTexture(target, handle);
					glTexImage1D(target, 0, (int)inputFormat, width, 0, (int)internalFormat, (int)inputType, data);
					GL::GenerateMipmap(target);
				}
				void Load(ImageFormat internalFormat, ImageFormat inputFormat, ImageDataType inputType, void *data, int width, int height)
				{
					glBindTexture(target, handle);
					glTexImage2D(target, 0, (int)inputFormat, width, height, 0, (int)internalFormat, (int)inputType, data);
					GL::GenerateMipmap(target);
				}
				void ReadPixels(ImageFormat outputFormat, ImageDataType type, void * data)
				{
					Bind();
					glGetTexImage(target, 0, (int)outputFormat, (int)type, data);
				}
				void SetFilter(TextureFilter filter)
				{
					Bind();
					switch (filter)
					{
					case TextureFilter::Nearest:
						glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
						glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
						break;
					case TextureFilter::Linear:
						glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
						glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
						break;
					case TextureFilter::TriLinear:
						glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
						glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
						break;
					case TextureFilter::Anisotropic4x:
						glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
						glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
						glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0f);
						break;
					case TextureFilter::Anisotropic8x:
						glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
						glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
						glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8.0f);
						break;
					case TextureFilter::Anisotropic16x:
						glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
						glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
						glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
						break;
					}
				}
				void Bind()
				{
					glBindTexture(target, handle);
				}
				static void UnBind()
				{
					glBindTexture(target, 0);
				}
			};

			class Texture2D : public Texture<GL_TEXTURE_2D>
			{
			public:
				static Texture2D * Create(const String & fileName);
			};

			class Texture1D : public Texture<GL_TEXTURE_1D>
			{};

			class RenderBuffer : public Object
			{
			private:
				unsigned int handle;
			public:
				RenderBuffer()
				{
					GL::GenRenderbuffers(1, &handle);
				}
				~RenderBuffer()
				{
					GL::DeleteRenderbuffers(1, &handle);
				}
				inline unsigned int GetHandle()
				{
					return handle;
				}
			public:
				void SetStorage(int w, int h, ImageFormat format, int multiSampleCount = 0)
				{
					GL::BindRenderbuffer(GL_RENDERBUFFER, handle);
					GL::RenderbufferStorageMultisample(GL_RENDERBUFFER, multiSampleCount, (int)format, w, h);
					GL::BindRenderbuffer(GL_RENDERBUFFER, 0);
				}
			};

			enum class FrameBufferTarget
			{
				Draw = GL_DRAW_FRAMEBUFFER,
				Read = GL_READ_FRAMEBUFFER
			};

			enum class FrameBufferAttachment
			{
				Color0 = GL_COLOR_ATTACHMENT0,
				Color1 = GL_COLOR_ATTACHMENT1,
				Color2 = GL_COLOR_ATTACHMENT2,
				Color3 = GL_COLOR_ATTACHMENT3,
				Color4 = GL_COLOR_ATTACHMENT4,
				Color5 = GL_COLOR_ATTACHMENT5,
				Color6 = GL_COLOR_ATTACHMENT6,
				Color7 = GL_COLOR_ATTACHMENT7,
				Color8 = GL_COLOR_ATTACHMENT8,
				Color9 = GL_COLOR_ATTACHMENT9,
				Color10 = GL_COLOR_ATTACHMENT10,
				Color11 = GL_COLOR_ATTACHMENT11,
				Color12 = GL_COLOR_ATTACHMENT12,
				Color13 = GL_COLOR_ATTACHMENT13,
				Color14 = GL_COLOR_ATTACHMENT14,
				Color15 = GL_COLOR_ATTACHMENT15,
				Depth = GL_DEPTH_ATTACHMENT,
				Stencil = GL_STENCIL_ATTACHMENT
			};

			class FrameBuffer : public Object
			{
			private:
				unsigned int handle;
				int target;
			public:
				FrameBuffer()
				{
					target = (int)FrameBufferTarget::Draw;
					GL::GenFramebuffers(1, &handle);
				}
				virtual ~FrameBuffer()
				{
					GL::DeleteFramebuffers(1, &handle);
				}
				inline unsigned int GetHandle()
				{
					return handle;
				}
			public:
				void SetTarget(FrameBufferTarget target)
				{
					this->target = (int)target;
				}
				void Attach(FrameBufferAttachment attachment, RenderBuffer * renderBuffer)
				{
					Bind();
					GL::FramebufferRenderbuffer(target, (int)attachment, GL_RENDERBUFFER, renderBuffer->GetHandle());
				}
				void Attach(FrameBufferAttachment attachment, Texture2D * texture, int level = 0)
				{
					Bind();
					GL::FramebufferTexture2D(target, (int)attachment, GL_TEXTURE_2D, texture->GetHandle(), level);
				}
				void Bind()
				{
					GL::BindFramebuffer(target, handle);
				}
				void UnBind()
				{
					GL::BindFramebuffer(target, 0);
				}
			};
			void InitializeOpenGLExtensions();
		}
	}
}

#endif