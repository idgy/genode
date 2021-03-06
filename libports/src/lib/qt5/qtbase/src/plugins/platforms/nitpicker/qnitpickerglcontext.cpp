/*
 * \brief  QNitpickerGLContext
 * \author Christian Prochaska
 * \date   2013-11-18
 */

/*
 * Copyright (C) 2013 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

/* Genode includes */
#include <base/printf.h>

/* EGL includes */
#define EGL_EGLEXT_PROTOTYPES

#include <EGL/egl.h>
#include <EGL/eglext.h>

/* Qt includes */
#include <QtPlatformSupport/private/qeglconvenience_p.h>
#include <QDebug>

/* local includes */
#include "qnitpickerplatformwindow.h"
#include "qnitpickerglcontext.h"

static const bool qnglc_verbose = false;

QT_BEGIN_NAMESPACE

QNitpickerGLContext::QNitpickerGLContext(QOpenGLContext *context)
    : QPlatformOpenGLContext()
{
	if (qnglc_verbose)
		PDBG("called");

	if (!eglBindAPI(EGL_OPENGL_API))
    	qFatal("eglBindAPI() failed");

    _egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (_egl_display == EGL_NO_DISPLAY)
		qFatal("eglGetDisplay() failed");

	int major = -1;
	int minor = -1;
	if (!eglInitialize(_egl_display, &major, &minor))
		qFatal("eglInitialize() failed");

	if (qnglc_verbose)
		PDBG("eglInitialize() returned major: %d, minor: %d", major, minor);

    _egl_config = q_configFromGLFormat(_egl_display, context->format(), false, EGL_PBUFFER_BIT);
    if (_egl_config == 0)
        qFatal("Could not find a matching EGL config");

	_format = q_glFormatFromConfig(_egl_display, _egl_config);

	_egl_context = eglCreateContext(_egl_display, _egl_config, EGL_NO_CONTEXT, 0);
    if (_egl_context == EGL_NO_CONTEXT)
        qFatal("eglCreateContext() failed");
}


bool QNitpickerGLContext::makeCurrent(QPlatformSurface *surface)
{
	if (qnglc_verbose)
		PDBG("called");

	doneCurrent();

	QNitpickerPlatformWindow *w = static_cast<QNitpickerPlatformWindow*>(surface);

	Genode_egl_window egl_window = { w->geometry().width(),
		                             w->geometry().height(),
		                             w->framebuffer() };

	if (qnglc_verbose)
		PDBG("w->framebuffer() = %p", w->framebuffer());

#if 0

	/*
	 * Unfortunalety, this code triggers a memory leak somewhere,
	 * so it cannot be used yet.
	 */

	if (w->egl_surface() != EGL_NO_SURFACE)
		if (!eglDestroySurface(_egl_display, w->egl_surface()))
			qFatal("eglDestroySurface() failed");

	EGLSurface egl_surface =
		eglCreateWindowSurface(_egl_display, _egl_config, &egl_window, 0);

	if (egl_surface == EGL_NO_SURFACE)
		qFatal("eglCreateiWindowSurface() failed");

	w->egl_surface(egl_surface);

#else

	/* temporary workaround, the surface gets created only once */

	if (w->egl_surface() == EGL_NO_SURFACE) {

		EGLSurface egl_surface =
			eglCreateWindowSurface(_egl_display, _egl_config, &egl_window, 0);

		if (egl_surface == EGL_NO_SURFACE)
			qFatal("eglCreateiWindowSurface() failed");

		w->egl_surface(egl_surface);
	}

#endif

	if (!eglMakeCurrent(_egl_display, w->egl_surface(), w->egl_surface(), _egl_context))
		qFatal("eglMakeCurrent() failed");

	return true;
}


void QNitpickerGLContext::doneCurrent()
{
	if (qnglc_verbose)
		PDBG("called");

	if (!eglMakeCurrent(_egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT))
		qFatal("eglMakeCurrent() failed");
}


void QNitpickerGLContext::swapBuffers(QPlatformSurface *surface)
{
	if (qnglc_verbose)
		PDBG("called");

	QNitpickerPlatformWindow *w = static_cast<QNitpickerPlatformWindow*>(surface);

	if (!eglSwapBuffers(_egl_display, w->egl_surface()))
		qFatal("eglSwapBuffers() failed");

	w->refresh(0, 0, w->geometry().width(), w->geometry().height());
}


void (*QNitpickerGLContext::getProcAddress(const QByteArray &procName)) ()
{
	if (qnglc_verbose)
		PDBG("procName = %s, pointer = %p", procName.constData(), eglGetProcAddress(procName.constData())); 

	return static_cast<QFunctionPointer>(eglGetProcAddress(procName.constData()));
}


QSurfaceFormat QNitpickerGLContext::format() const
{
    return _format;
}

QT_END_NAMESPACE

