// findfunction-dlsym.h

#define GL_GLEXT_PROTOTYPES
#include <GLES/gl.h>
#include <GLES/glext.h>
#include "gl-enums.h"

#define DLSYM_TABLE_SIZE  200
static pthread_mutex_t dlsym_mutex = PTHREAD_MUTEX_INITIALIZER;

static void* dlsym_table_ptr[DLSYM_TABLE_SIZE];
static uint32_t dlsym_table_addr[DLSYM_TABLE_SIZE];

static uint32_t dlsym_table_element_count = 0;


static
void
proxy_AndroidBitmap_lockPixels(state_t *state)
{
    LOG_I("calling proxy_AndroidBitmap_lockPixels(%p, %p, %p)", (void*)r0, (void*)r1, (void*)r2);
    r0_signed = AndroidBitmap_lockPixels((JNIEnv*)r0, (jobject)r1, (void**)r2);
}

static
void
proxy_AndroidBitmap_unlockPixels(state_t *state)
{
    LOG_I("calling proxy_AndroidBitmap_unlockPixels(%p, %p)", (void*)r0, (void*)r1);
    r0_signed = AndroidBitmap_unlockPixels((JNIEnv*)r0, (jobject)r1);
}

static
void
proxy_AndroidBitmap_getInfo(state_t *state)
{
    LOG_I("calling proxy_AndroidBitmap_getInfo(%p, %p, %p)", (void*)r0, (void*)r1, (void*)r2);
    r0_signed = AndroidBitmap_getInfo((JNIEnv*)r0, (jobject)r1, (AndroidBitmapInfo*)r2);
}


static
void
proxy_glBindTexture(state_t *state)
{
    LOG_I("calling proxy_glBindTexture(%p %s, %u)", vv(r0), glenum2str(r0), r1);
    (void)glBindTexture((GLenum)r0, (GLuint)r1);
}

static
void
proxy_glBlendFunc(state_t *state)
{
    LOG_I("calling proxy_glBlendFunc(%p %s, %p %s)", vv(r0), glenum2str(r0), vv(r1), glenum2str(r1));
    (void)glBlendFunc((GLenum)r0, (GLenum)r1);
}

static
void
proxy_glClear(state_t *state)
{
    LOG_I("calling proxy_glClear(%p)", vv(r0));
    (void)glClear((GLbitfield)r0);
}

static
void
proxy_glClearColorx(state_t *state)
{
    LOG_I("calling proxy_glClearColorx(%p, %p, %p, %p)", vv(r0), vv(r1), vv(r2), vv(r3));
    (void)glClearColorx((GLclampx)r0, (GLclampx)r1, (GLclampx)r2, (GLclampx)r3);
}

static
void
proxy_glColor4f(state_t *state)
{
    LOG_I("calling proxy_glColor4f(%f, %f, %f, %f)", x_float, y_float, z_float, w_float);
    (void)glColor4f(x_float, y_float, z_float, w_float);
}

static
void
proxy_glColor4x(state_t *state)
{
    LOG_I("calling proxy_glColor4x(%p, %p, %p, %p)", vv(r0), vv(r1), vv(r2), vv(r3));
    (void)glColor4x((GLfixed)r0, (GLfixed)r1, (GLfixed)r2, (GLfixed)r3);
}

static
void
proxy_glCullFace(state_t *state)
{
    LOG_I("calling proxy_glCullFace(%p %s)", vv(r0), glenum2str(r0));
    (void)glCullFace((GLenum)r0);
}

static
void
proxy_glDeleteTextures(state_t *state)
{
    LOG_I("calling proxy_glDeleteTextures(%d, %p)", r0, vv(r1));
    (void)glDeleteTextures((GLsizei)r0, (const GLuint *)r1);
}

static
void
proxy_glDisable(state_t *state)
{
    LOG_I("calling proxy_glDisable(%p %s)", vv(r0), glenum2str(r0));
    (void)glDisable((GLenum)r0);
}

static
void
proxy_glDisableClientState(state_t *state)
{
    LOG_I("calling proxy_glDisableClientState(%p %s)", vv(r0), glenum2str(r0));
    (void)glDisableClientState((GLenum)r0);
}

static
void
proxy_glDrawArrays(state_t *state)
{
    LOG_I("calling proxy_glDrawArrays(%p %s, %d, %u)", vv(r0), glenum2str(r0), r1, r2);
    (void)glDrawArrays((GLenum)r0, (GLint)r1, (GLsizei)r2);
}

static
void
proxy_glEnable(state_t *state)
{
    LOG_I("calling proxy_glEnable(%p %s)", vv(r0), glenum2str(r0));
    (void)glEnable((GLenum)r0);
}

static
void
proxy_glEnableClientState(state_t *state)
{
    LOG_I("calling proxy_glEnableClientState(%p %s)", vv(r0), glenum2str(r0));
    (void)glEnableClientState((GLenum)r0);
}

static
void
proxy_glFinish(state_t *state)
{
    LOG_I("calling proxy_glFinish()");
    (void)glFinish();
}

static
void
proxy_glFlush(state_t *state)
{
    LOG_I("calling proxy_glFlush()");
    (void)glFlush();
}

static
void
proxy_glFrontFace(state_t *state)
{
    LOG_I("calling proxy_glFrontFace(%p %s)", vv(r0), glenum2str(r0));
    (void)glFrontFace((GLenum)r0);
}

static
void
proxy_glGenTextures(state_t *state)
{
    LOG_I("calling proxy_glGenTextures(%u, %p)", r0, vv(r1));
    (void)glGenTextures((GLsizei)r0, (GLuint *)r1);
}

static
void
proxy_glGetError(state_t *state)
{
    LOG_I("calling proxy_glGetError()");
    r0 = (uint32_t)glGetError();
    if (r0) {
        LOG_I("        proxy_glGetError returned %p %s", vv(r0), glenum2str(r0));
    } else {
        LOG_I("        proxy_glGetError returned %p", vv(r0));
    }
}

static
void
proxy_glGetFloatv(state_t *state)
{
    LOG_I("calling proxy_glGetFloatv(%p %s, %p)", vv(r0), glenum2str(r0), vv(r1));
    (void)glGetFloatv((GLenum)r0, (GLfloat *)r1);
}

static
void
proxy_glGetIntegerv(state_t *state)
{
    LOG_I("calling proxy_glGetIntegerv(%p %s, %p)", vv(r0), glenum2str(r0), vv(r1));
    (void)glGetIntegerv((GLenum)r0, (GLint *)r1);
}

static
void
proxy_glGetString(state_t *state)
{
    LOG_I("calling proxy_glGetString(%p %s)", vv(r0), glenum2str(r0));
    r0 = (uint32_t)glGetString((GLenum)r0);
    LOG_I("        proxy_glGetString returned %p \"%s\"", vv(r0), (const char *)r0);
}

static
void
proxy_glLoadIdentity(state_t *state)
{
    LOG_I("calling proxy_glLoadIdentity()");
    (void)glLoadIdentity();
}

static
void
proxy_glLoadMatrixf(state_t *state)
{
    LOG_I("calling proxy_glLoadMatrixf(%p)", vv(r0));
    (void)glLoadMatrixf((const GLfloat *)r0);
}

static
void
proxy_glLoadMatrixx(state_t *state)
{
    LOG_I("calling proxy_glLoadMatrixx(%p)", vv(r0));
    (void)glLoadMatrixx((const GLfixed *)r0);
}

static
void
proxy_glMatrixMode(state_t *state)
{
    LOG_I("calling proxy_glMatrixMode(%p %s)", vv(r0), glenum2str(r0));
    (void)glMatrixMode((GLenum)r0);
}

static
void
proxy_glPixelStorei(state_t *state)
{
    LOG_I("calling proxy_glPixelStorei(%p %s, %d)", vv(r0), glenum2str(r0), r1);
    (void)glPixelStorei((GLenum)r0, (GLint)r1);
}

static
void
proxy_glPointSize(state_t *state)
{
    LOG_I("calling proxy_glPointSize(%f)", x_float);
    (void)glPointSize(x_float);
}

static
void
proxy_glScalef(state_t *state)
{
    LOG_I("calling proxy_glScalef(%f, %f, %f)", x_float, y_float, z_float);
    (void)glScalef(x_float, y_float, z_float);
}

static
void
proxy_glScissor(state_t *state)
{
    LOG_I("calling proxy_glScissor(%d, %d, %u, %u)", r0, r1, r2, r3);
    (void)glScissor((GLint)r0, (GLint)r1, (GLsizei)r2, (GLsizei)r3);
}

static
void
proxy_glShadeModel(state_t *state)
{
    LOG_I("calling proxy_glShadeModel(%p %s)", vv(r0), glenum2str(r0));
    (void)glShadeModel((GLenum)r0);
}

static
void
proxy_glTexCoordPointer(state_t *state)
{
    LOG_I("calling proxy_glTexCoordPointer(%d, %p %s, %u, %p)", r0, vv(r1), glenum2str(r1), r2, vv(r3));
    (void)glTexCoordPointer((GLint)r0, (GLenum)r1, (GLsizei)r2, (const GLvoid *)r3);
}

static
void
proxy_glTexEnvi(state_t *state)
{
    LOG_I("calling proxy_glTexEnvi(%p %s, %p %s, %d)", vv(r0), glenum2str(r0), vv(r1), glenum2str(r1), r2);
    (void)glTexEnvi((GLenum)r0, (GLenum)r1, (GLint)r2);
}

static
void
proxy_glTexEnvx(state_t *state)
{
    LOG_I("calling proxy_glTexEnvx(%p %s, %p %s, %p)", vv(r0), glenum2str(r0), vv(r1), glenum2str(r1), vv(r2));
    (void)glTexEnvx((GLenum)r0, (GLenum)r1, (GLfixed)r2);
}

static
void
proxy_glTexImage2D(state_t *state)
{
    uint32_t param5 = load(r13);
    uint32_t param6 = load(r13 + 4);
    uint32_t param7 = load(r13 + 8);
    uint32_t param8 = load(r13 + 12);
    uint32_t param9 = load(r13 + 16);
    LOG_I("calling proxy_glTexImage2D(%p %s, %d, %p %s, %u, %u, %d, %p %s, %p %s, %p",
        vv(r0), glenum2str(r0), r1, vv(r2), glenum2str(r2), r3, param5, param6, vv(param7),
        glenum2str(param7), vv(param8), glenum2str(param8), vv(param9));
    (void)glTexImage2D((GLenum)r0, (GLint)r1, (GLint)r2, (GLsizei)r3, (GLsizei)param5,
        (GLint)param6, (GLenum)param7, (GLenum)param8, (const GLvoid *)param9);
}

static
void
proxy_glTexParameterf(state_t *state)
{
    LOG_I("calling proxy_glTexParameterf(%p %s, %p %s, %f)", vv(r0), glenum2str(r0), vv(r1), glenum2str(r1), z_float);
    (void)glTexParameterf((GLenum)r0, (GLenum)r1, z_float);
}

static
void
proxy_glTexParameteri(state_t *state)
{
    LOG_I("calling proxy_glTexParameteri(%p %s, %p %s, %p %s)", vv(r0), glenum2str(r0), vv(r1),
                                                        glenum2str(r1), vv(r2), glenum2str(r2));
    (void)glTexParameteri((GLenum)r0, (GLenum)r1, (GLint)r2);
}

static
void
proxy_glTexParameteriv(state_t *state)
{
    LOG_I("calling proxy_glTexParameteriv(%p %s, %p %s, %p)", vv(r0), glenum2str(r0), vv(r1), glenum2str(r1), vv(r2));
    (void)glTexParameteriv((GLenum)r0, (GLenum)r1, (const GLint *)r2);
}

static
void
proxy_glTexSubImage2D(state_t *state)
{
    uint32_t param5 = load(r13);
    uint32_t param6 = load(r13 + 4);
    uint32_t param7 = load(r13 + 8);
    uint32_t param8 = load(r13 + 12);
    uint32_t param9 = load(r13 + 16);
    LOG_I("calling proxy_glTexSubImage2D(%p %s, %d, %d, %d, %u, %u, %p %s, %p %s, %p)",
        vv(r0), glenum2str(r0), r1, r2, r3, param5, param6, vv(param7), glenum2str(param7),
        vv(param8), glenum2str(param8), vv(param9));
    (void)glTexSubImage2D((GLenum)r0, (GLint)r1, (GLint)r2 , (GLint)r3, (GLsizei)param5,
        (GLsizei)param6, (GLenum)param7, (GLenum)param8, (const GLvoid *)param9);
}

static
void
proxy_glTranslatex(state_t *state)
{
    LOG_I("calling proxy_glTranslatex(%p, %p, %p)", vv(r0), vv(r1), vv(r2));
    (void)glTranslatex((GLfixed)r0, (GLfixed)r1, (GLfixed)r2);
}

static
void
proxy_glVertexPointer(state_t *state)
{
    LOG_I("calling proxy_glVertexPointer(%d, %p %s, %u, %p)", r0, vv(r1), glenum2str(r1), r2, vv(r3));
    (void)glVertexPointer((GLint)r0, (GLenum)r1, (GLsizei)r2, (const GLvoid *)r3);
}

static
void
proxy_glViewport(state_t *state)
{
    LOG_I("calling proxy_glViewport(%d, %d, %u, %u)", r0, r1, r2, r3);
    (void)glViewport((GLint)r0, (GLint)r1, (GLsizei)r2, (GLsizei)r3);
}

static
void
proxy_glDrawTexfOES(state_t *state)
{
    union {
        uint32_t param5;
        float    param5_float;
    } params;
    params.param5 = load(r13);
    LOG_I("calling proxy_glDrawTexfOES(%f, %f, %f, %f, %f)",
                        x_float, y_float, z_float, w_float, params.param5_float);
    (void)glDrawTexfOES(x_float, y_float, z_float, w_float, params.param5_float);
}



// ================================

static
void
add_dlsym_table_entry(uint32_t addr, const char *name)
{
    pthread_mutex_lock(&dlsym_mutex);
    if (!strcmp(name, "AndroidBitmap_lockPixels")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_AndroidBitmap_lockPixels;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "AndroidBitmap_getInfo")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_AndroidBitmap_getInfo;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "AndroidBitmap_unlockPixels")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_AndroidBitmap_unlockPixels;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "glBindTexture")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_glBindTexture;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "glBlendFunc")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_glBlendFunc;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "glClear")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_glClear;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "glClearColorx")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_glClearColorx;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "glColor4f")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_glColor4f;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "glColor4x")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_glColor4x;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "glCullFace")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_glCullFace;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "glDeleteTextures")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_glDeleteTextures;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "glDisable")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_glDisable;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "glDisableClientState")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_glDisableClientState;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "glDrawArrays")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_glDrawArrays;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "glEnable")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_glEnable;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "glEnableClientState")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_glEnableClientState;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "glFinish")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_glFinish;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "glFlush")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_glFlush;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "glFrontFace")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_glFrontFace;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "glGenTextures")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_glGenTextures;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "glGetError")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_glGetError;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "glGetFloatv")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_glGetFloatv;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "glGetIntegerv")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_glGetIntegerv;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "glGetString")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_glGetString;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "glLoadIdentity")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_glLoadIdentity;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "glLoadMatrixf")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_glLoadMatrixf;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "glLoadMatrixx")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_glLoadMatrixx;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "glMatrixMode")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_glMatrixMode;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "glPixelStorei")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_glPixelStorei;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "glPointSize")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_glPointSize;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "glScalef")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_glScalef;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "glScissor")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_glScissor;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "glShadeModel")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_glShadeModel;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "glTexCoordPointer")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_glTexCoordPointer;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "glTexEnvi")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_glTexEnvi;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "glTexEnvx")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_glTexEnvx;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "glTexImage2D")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_glTexImage2D;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "glTexParameterf")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_glTexParameterf;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "glTexParameteri")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_glTexParameteri;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "glTexParameteriv")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_glTexParameteriv;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "glTexSubImage2D")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_glTexSubImage2D;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "glTranslatex")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_glTranslatex;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "glVertexPointer")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_glVertexPointer;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "glViewport")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_glViewport;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "glDrawTexfOES")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_glDrawTexfOES;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "")) {
    } else if (!strcmp(name, "")) {
    } else if (!strcmp(name, "")) {
    } else if (!strcmp(name, "")) {
    } else if (!strcmp(name, "")) {
    } else if (!strcmp(name, "")) {
    } else if (!strcmp(name, "")) {
    } else if (!strcmp(name, "")) {
    } else if (!strcmp(name, "")) {
    } else {
        LOG_E("add_dlsym_table_entry: failed to add %s", name);
    }

    if (dlsym_table_element_count >= DLSYM_TABLE_SIZE) {
        LOG_E("dlsym_table_element_count >= DLSYM_TABLE_SIZE");
    }

    pthread_mutex_unlock(&dlsym_mutex);
}

static
int
find_and_call_dlsym(state_t *state, uint32_t addr)
{
    unsigned int k;
    for (k = 0; k < dlsym_table_element_count; k ++) {
        if (addr == dlsym_table_addr[k]) {
            LOG_I("find_and_call_function: func_%04x", addr);
            ((void (*)(state_t *))(dlsym_table_ptr[k]))(state);
            return 1;
        }
    }

    return 0;
}
