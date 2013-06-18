// findfunction-dlsym.inc

#include <GLES/gl.h>
#include <GLES/glext.h>

#define DLSYM_TABLE_SIZE  200
pthread_mutex_t dlsym_mutex = PTHREAD_MUTEX_INITIALIZER;

void* dlsym_table_ptr[DLSYM_TABLE_SIZE];
uint32_t dlsym_table_addr[DLSYM_TABLE_SIZE];

uint32_t dlsym_table_element_count = 0;


static
void
proxy_AndroidBitmap_lockPixels(state_t *state)
{
    LOG_I("calling proxy_AndroidBitmap_lockPixels(%p, %p, %p)", (void*)r0, (void*)r1, (void*)r2);
    r0_signed = AndroidBitmap_lockPixels((JNIEnv*)aa(r0), (jobject)aa(r1), (void**)aa(r2));
}

static
void
proxy_AndroidBitmap_unlockPixels(state_t *state)
{
    LOG_I("calling proxy_AndroidBitmap_unlockPixels(%p, %p)", (void*)r0, (void*)r1);
    r0_signed = AndroidBitmap_unlockPixels((JNIEnv*)aa(r0), (jobject)aa(r1));
}

static
void
proxy_AndroidBitmap_getInfo(state_t *state)
{
    LOG_I("calling proxy_AndroidBitmap_getInfo(%p, %p, %p)", (void*)r0, (void*)r1, (void*)r2);
    r0_signed = AndroidBitmap_getInfo((JNIEnv*)aa(r0), (jobject)aa(r1), (AndroidBitmapInfo*)aa(r2));
}


static
void
proxy_glBindTexture(state_t *state)
{
    LOG_I("calling proxy_glBindTexture(0x%x, %u)", r0, r1);
    (void)glBindTexture((GLenum)r0, (GLuint)r1);
}

static
void
proxy_glBlendFunc(state_t *state)
{
    LOG_I("calling proxy_glBlendFunc");
    (void)glBlendFunc((GLenum)r0, (GLenum)r1);
}

static
void
proxy_glClear(state_t *state)
{
    LOG_I("calling proxy_glClear");
    (void)glClear((GLbitfield)r0);
}

static
void
proxy_glClearColorx(state_t *state)
{
    LOG_I("calling proxy_glClearColorx");
    (void)glClearColorx((GLclampx)r0, (GLclampx)r1, (GLclampx)r2, (GLclampx)r3);
}

static
void
proxy_glColor4f(state_t *state)
{
    LOG_I("calling proxy_glColor4f");
    (void)glColor4f((GLfloat)r0, (GLfloat)r1, (GLfloat)r2, (GLfloat)r3);
}

static
void
proxy_glColor4x(state_t *state)
{
    LOG_I("calling proxy_glColor4x");
    (void)glColor4x((GLfixed)r0, (GLfixed)r1, (GLfixed)r2, (GLfixed)r3);
}

static
void
proxy_glCullFace(state_t *state)
{
    LOG_I("calling proxy_glCullFace");
    (void)glCullFace((GLenum)r0);
}

static
void
proxy_glDeleteTextures(state_t *state)
{
    LOG_I("calling proxy_glDeleteTextures");
    (void)glDeleteTextures((GLsizei)r0, (const GLuint *)aa(r1));
}

static
void
proxy_glDisable(state_t *state)
{
    LOG_I("calling proxy_glDisable");
    (void)glDisable((GLenum)r0);
}

static
void
proxy_glDisableClientState(state_t *state)
{
    LOG_I("calling proxy_glDisableClientState");
    (void)glDisableClientState((GLenum)r0);
}

static
void
proxy_glDrawArrays(state_t *state)
{
    LOG_I("calling proxy_glDrawArrays");
    (void)glDrawArrays((GLenum)r0, (GLint)r1, (GLsizei)r2);
}

static
void
proxy_glEnable(state_t *state)
{
    LOG_I("calling proxy_glEnable");
    (void)glEnable((GLenum)r0);
}

static
void
proxy_glEnableClientState(state_t *state)
{
    LOG_I("calling proxy_glEnableClientState");
    (void)glEnableClientState((GLenum)r0);
}

static
void
proxy_glFinish(state_t *state)
{
    LOG_I("calling proxy_glFinish");
    (void)glFinish();
}

static
void
proxy_glFlush(state_t *state)
{
    LOG_I("calling proxy_glFlush");
    (void)glFlush();
}

static
void
proxy_glFrontFace(state_t *state)
{
    LOG_I("calling proxy_glFrontFace");
    (void)glFrontFace((GLenum)r0);
}

static
void
proxy_glGenTextures(state_t *state)
{
    LOG_I("calling proxy_glGenTextures");
    (void)glGenTextures((GLsizei)r0, (GLuint *)aa(r1));
}

static
void
proxy_glGetError(state_t *state)
{
    LOG_I("calling proxy_glGetError");
    r0 = (uint32_t)glGetError();
}

static
void
proxy_glGetFloatv(state_t *state)
{
    LOG_I("calling proxy_glGetFloatv");
    (void)glGetFloatv((GLenum)r0, (GLfloat *)aa(r1));
}

static
void
proxy_glGetIntegerv(state_t *state)
{
    LOG_I("calling proxy_glGetIntegerv");
    (void)glGetIntegerv((GLenum)r0, (GLint *)aa(r1));
}

static
void
proxy_glGetString(state_t *state)
{
    LOG_I("calling proxy_glGetString");
    r0 = (uint32_t)glGetString((GLenum)r0);
}

static
void
proxy_glLoadIdentity(state_t *state)
{
    LOG_I("calling proxy_glLoadIdentity");
    (void)glLoadIdentity();
}

static
void
proxy_glLoadMatrixf(state_t *state)
{
    LOG_I("calling proxy_glLoadMatrixf");
    (void)glLoadMatrixf((const GLfloat *)aa(r0));
}

static
void
proxy_glLoadMatrixx(state_t *state)
{
    LOG_I("calling proxy_glLoadMatrixx");
    (void)glLoadMatrixx((const GLfixed *)aa(r0));
}

static
void
proxy_glMatrixMode(state_t *state)
{
    LOG_I("calling proxy_glMatrixMode");
    (void)glMatrixMode((GLenum)r0);
}

static
void
proxy_glPixelStorei(state_t *state)
{
    LOG_I("calling proxy_glPixelStorei");
    (void)glPixelStorei((GLenum)r0, (GLint)r1);
}

static
void
proxy_glPointSize(state_t *state)
{
    LOG_I("calling proxy_glPointSize");
    (void)glPointSize((GLfloat)r0);
}

static
void
proxy_glScalef(state_t *state)
{
    LOG_I("calling proxy_glScalef");
    (void)glScalef((GLfloat)r0, (GLfloat)r1, (GLfloat)r2);
}

static
void
proxy_glScissor(state_t *state)
{
    LOG_I("calling proxy_glScissor");
    (void)glScissor((GLint)r0, (GLint)r1, (GLsizei)r2, (GLsizei)r3);
}

static
void
proxy_glShadeModel(state_t *state)
{
    LOG_I("calling proxy_glShadeModel");
    (void)glShadeModel((GLenum)r0);
}

static
void
proxy_glTexCoordPointer(state_t *state)
{
    LOG_I("calling proxy_glTexCoordPointer(%d, %d, %d, %p)", r0, r1, r2, r3);
    (void)glTexCoordPointer((GLint)r0, (GLenum)r1, (GLsizei)r2, (const GLvoid *)aa(r3));
}

static
void
proxy_glTexEnvi(state_t *state)
{
    LOG_I("calling proxy_glTexEnvi");
    (void)glTexEnvi((GLenum)r0, (GLenum)r1, (GLint)r2);
}

static
void
proxy_glTexEnvx(state_t *state)
{
    LOG_I("calling proxy_glTexEnvx");
    (void)glTexEnvx((GLenum)r0, (GLenum)r1, (GLfixed)r2);
}

static
void
proxy_glTexImage2D(state_t *state)
{
    LOG_I("calling proxy_glTexImage2D");
    (void)glTexImage2D((GLenum)r0, (GLint)r1, (GLint)r2, (GLsizei)r3, (GLsizei)load(r13),
        (GLint)load(r13 + 4), (GLenum)load(r13 + 8), (GLenum)load(r13 + 12), (const GLvoid *)aa(load(r13 + 16)));
}

static
void
proxy_glTexParameterf(state_t *state)
{
    LOG_I("calling proxy_glTexParameterf");
    (void)glTexParameterf((GLenum)r0, (GLenum)r1, (GLfloat)r2);
}

static
void
proxy_glTexParameteri(state_t *state)
{
    LOG_I("calling proxy_glTexParameteri");
    (void)glTexParameteri((GLenum)r0, (GLenum)r1, (GLint)r2);
}

static
void
proxy_glTexParameteriv(state_t *state)
{
    LOG_I("calling proxy_glTexParameteriv");
    (void)glTexParameteriv((GLenum)r0, (GLenum)r1, (const GLint *)aa(r2));
}

static
void
proxy_glTexSubImage2D(state_t *state)
{
    LOG_I("calling proxy_glTexSubImage2D");
    (void)glTexSubImage2D((GLenum)r0, (GLint)r1, (GLint)r2 , (GLint)r3, (GLsizei)load(r13),
        (GLsizei)load(r13 + 4), (GLenum)load(r13 + 8), (GLenum)load(r13 + 12), (const GLvoid *)aa(load(r13 + 16)));
}

static
void
proxy_glTranslatex(state_t *state)
{
    LOG_I("calling proxy_glTranslatex");
    (void)glTranslatex((GLfixed)r0, (GLfixed)r1, (GLfixed)r2);
}

static
void
proxy_glVertexPointer(state_t *state)
{
    LOG_I("calling proxy_glVertexPointer");
    (void)glVertexPointer((GLint)r0, (GLenum)r1, (GLsizei)r2, (const GLvoid *)aa(r3));
}

static
void
proxy_glViewport(state_t *state)
{
    LOG_I("calling proxy_glViewport");
    (void)glViewport((GLint)r0, (GLint)r1, (GLsizei)r2, (GLsizei)r3);
}

static
void
proxy_glDrawTexfOES(state_t *state)
{
    LOG_I("calling proxy_glDrawTexfOES");
    (void)glDrawTexfOES((GLfloat)r0, (GLfloat)r1, (GLfloat)r2, (GLfloat)r3, (GLfloat)load(r13));
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
    int k;
    for (k = 0; k < dlsym_table_element_count; k ++) {
        if (addr == dlsym_table_addr[k]) {
            LOG_I("find_and_call_function: func_%04x", addr);
            ((void (*)(state_t *))(dlsym_table_ptr[k]))(state);
            return 1;
        }
    }

    return 0;
}
