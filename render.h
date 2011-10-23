
#ifndef __RENDER_H_
#define __RENDER_H_
#define dbg(var) printf("%d\n", var)
struct {
	char GLSL, BE, FBO, MT;
} supported;

// GL_ARB_shading_language_100, GL_ARB_shader_objects, GL_ARB_fragment_shader, GL_ARB_vertex_shader
extern PFNGLCREATEPROGRAMOBJECTARBPROC  glCreateProgramObject_;
extern PFNGLDELETEOBJECTARBPROC         glDeleteObject_;
extern PFNGLUSEPROGRAMOBJECTARBPROC     glUseProgramObject_;
extern PFNGLCREATESHADEROBJECTARBPROC   glCreateShaderObject_;
extern PFNGLSHADERSOURCEARBPROC         glShaderSource_;
extern PFNGLCOMPILESHADERARBPROC        glCompileShader_;
extern PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameteriv_;
extern PFNGLATTACHOBJECTARBPROC         glAttachObject_;
extern PFNGLGETINFOLOGARBPROC           glGetInfoLog_;
extern PFNGLLINKPROGRAMARBPROC          glLinkProgram_;
extern PFNGLGETUNIFORMLOCATIONARBPROC   glGetUniformLocation_;
extern PFNGLUNIFORM1FARBPROC            glUniform1f_;
extern PFNGLUNIFORM2FARBPROC            glUniform2f_;
extern PFNGLUNIFORM3FARBPROC            glUniform3f_;
extern PFNGLUNIFORM4FARBPROC            glUniform4f_;
extern PFNGLUNIFORM1FVARBPROC           glUniform1fv_;
extern PFNGLUNIFORM2FVARBPROC           glUniform2fv_;
extern PFNGLUNIFORM3FVARBPROC           glUniform3fv_;
extern PFNGLUNIFORM4FVARBPROC           glUniform4fv_;
extern PFNGLUNIFORM1IARBPROC            glUniform1i_;
extern PFNGLBINDATTRIBLOCATIONARBPROC   glBindAttribLocation_;
extern PFNGLGETACTIVEUNIFORMARBPROC     glGetActiveUniform_;
extern PFNGLGETSHADERIVPROC             glGetShaderiv_;
extern PFNGLGETPROGRAMIVPROC            glGetProgramiv_;

// GL_EXT_blend_minmax
extern PFNGLBLENDEQUATIONEXTPROC glBlendEquation_;

// GL_EXT_framebuffer_object
extern PFNGLBINDRENDERBUFFEREXTPROC        glBindRenderbuffer_;
extern PFNGLDELETERENDERBUFFERSEXTPROC     glDeleteRenderbuffers_;
extern PFNGLGENFRAMEBUFFERSEXTPROC         glGenRenderbuffers_;
extern PFNGLRENDERBUFFERSTORAGEEXTPROC     glRenderbufferStorage_;
extern PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC  glCheckFramebufferStatus_;
extern PFNGLBINDFRAMEBUFFEREXTPROC         glBindFramebuffer_;
extern PFNGLDELETEFRAMEBUFFERSEXTPROC      glDeleteFramebuffers_;
extern PFNGLGENFRAMEBUFFERSEXTPROC         glGenFramebuffers_;
extern PFNGLFRAMEBUFFERTEXTURE2DEXTPROC    glFramebufferTexture2D_;
extern PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC glFramebufferRenderbuffer_;
extern PFNGLGENERATEMIPMAPEXTPROC          glGenerateMipmap_;

// GL_ARB_multitexture
extern PFNGLACTIVETEXTUREARBPROC       glActiveTexture_;
extern PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTexture_;
extern PFNGLMULTITEXCOORD2FARBPROC     glMultiTexCoord2f_;
extern PFNGLMULTITEXCOORD3FARBPROC     glMultiTexCoord3f_;
extern PFNGLMULTITEXCOORD4FARBPROC     glMultiTexCoord4f_;


// GL_EXT_framebuffer_blit
#ifndef GL_EXT_framebuffer_blit
#define GL_READ_FRAMEBUFFER_EXT           0x8CA8
#define GL_DRAW_FRAMEBUFFER_EXT           0x8CA9
#define GL_DRAW_FRAMEBUFFER_BINDING_EXT   0x8CA6
#define GL_READ_FRAMEBUFFER_BINDING_EXT   0x8CAA
typedef void (APIENTRYP PFNGLBLITFRAMEBUFFEREXTPROC) (GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
#endif
extern PFNGLBLITFRAMEBUFFEREXTPROC         glBlitFramebuffer_;

//~ extern PFNGLBLENDFUNCSEPARATEEXTPROC glBlendFuncSeparate_;

#define GL_COMPILE_STATUS                 0x8B81
#define GL_VERTEX_SHADER                  0x8B31
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_INFO_LOG_LENGTH                0x8B84
#define GL_LINK_STATUS                    0x8B82

#endif /* __RENDER_H_ */