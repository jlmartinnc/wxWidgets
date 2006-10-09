/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/animate.cpp
// Purpose:     wxAnimation and wxAnimationCtrl
// Author:      Francesco Montorsi
// Modified By:
// Created:     24/09/2006
// Id:          $Id$
// Copyright:   (c) Francesco Montorsi
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_ANIMATIONCTRL

#include "wx/animate.h"
#include "wx/log.h"
#include <gtk/gtk.h>
#include <gtk/gtkimage.h>


// ============================================================================
// implementation
// ============================================================================

void gdk_pixbuf_area_updated(GdkPixbufLoader *loader,
                             gint             x,
                             gint             y,
                             gint             width,
                             gint             height,
                             wxAnimation      *anim)
{
    if (anim && anim->GetPixbuf() == NULL)
    {
        // we need to set the pixbuf only if this is the first time this signal
        // has been called!
        anim->SetPixbuf(gdk_pixbuf_loader_get_animation(loader));
    }
}


//-----------------------------------------------------------------------------
// wxAnimation
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxAnimation, wxAnimationBase)

bool wxAnimation::LoadFile(const wxString &name, wxAnimationType WXUNUSED(type))
{
    UnRef();
    m_pixbuf = gdk_pixbuf_animation_new_from_file(
        wxConvFileName->cWX2MB(name), NULL);
    return IsOk();
}

bool wxAnimation::Load(wxInputStream &stream, wxAnimationType type)
{
    UnRef();

    char anim_type[12];
    switch (type)
    {
    case wxANIMATION_TYPE_GIF:
        strcpy(anim_type, "gif");
        break;

    case wxANIMATION_TYPE_ANI:
        strcpy(anim_type, "ani");
        break;

    default:
        break;
    }

    // create a GdkPixbufLoader
    GError *error = NULL;
    GdkPixbufLoader *loader;
    if (type != wxANIMATION_TYPE_INVALID && type != wxANIMATION_TYPE_ANY)
        loader = gdk_pixbuf_loader_new_with_type(anim_type, &error);
    else
        loader = gdk_pixbuf_loader_new();

    if (!loader)
    {
        wxLogDebug(wxT("Could not create the loader for '%s' animation type"), anim_type);
        return false;
    }

    // connect to loader signals
    g_signal_connect(loader, "area-updated", G_CALLBACK(gdk_pixbuf_area_updated), this);

    //m_bLoadComplete = false;
    guchar buf[2048];
    while (stream.IsOk())
    {
        // read a chunk of data
        stream.Read(buf, 2048);

        // fetch all data into the loader
        if (!gdk_pixbuf_loader_write(loader, buf, stream.LastRead(), &error))
        {
            gdk_pixbuf_loader_close(loader, &error);
            wxLogDebug(wxT("Could not write to the loader"));
            return false;
        }
    }

    // load complete
    if (!gdk_pixbuf_loader_close(loader, &error))
    {
        wxLogDebug(wxT("Could not close the loader"));
        return false;
    }
    //m_bLoadComplete = true;

    // wait until we get the last area_updated signal
    return true;
}


//-----------------------------------------------------------------------------
// wxAnimationCtrl
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxAnimationCtrl, wxAnimationCtrlBase)
BEGIN_EVENT_TABLE(wxAnimationCtrl, wxAnimationCtrlBase)
    EVT_TIMER(wxID_ANY, wxAnimationCtrl::OnTimer)
END_EVENT_TABLE()

bool wxAnimationCtrl::Create( wxWindow *parent, wxWindowID id,
                              const wxAnimation& anim,
                              const wxPoint& pos,
                              const wxSize& size,
                              long style,
                              const wxString& name)
{
    m_needParent = true;
    m_acceptsFocus = true;

    if (!PreCreation( parent, pos, size ) ||
        !wxControl::CreateBase(parent, id, pos, size, style & wxWINDOW_STYLE_MASK,
                               wxDefaultValidator, name))
    {
        wxFAIL_MSG( wxT("wxAnimationCtrl creation failed") );
        return false;
    }

    SetWindowStyle(style);

    m_widget = gtk_image_new();
    gtk_widget_show( GTK_WIDGET(m_widget) );

    m_parent->DoAddChild( this );

    PostCreation(size);
    SetBestSize(size);

    m_anim = NULL;
    m_iter = NULL;
    m_bPlaying = false;
    if (anim != wxNullAnimation)
        SetAnimation(anim);

    // init the timer used for animation
    m_timer.SetOwner(this);

    return true;
}

wxAnimationCtrl::~wxAnimationCtrl()
{
    ResetAnim();
    ResetIter();
}

bool wxAnimationCtrl::LoadFile(const wxString &filename, wxAnimationType type)
{
    wxAnimation anim;
    if (!anim.LoadFile(filename, type))
        return false;

    SetAnimation(anim);
    return true;
}

void wxAnimationCtrl::SetAnimation(const wxAnimation &anim)
{
    if (IsPlaying())
        Stop();

    ResetAnim();
    ResetIter();

    // copy underlying GdkPixbuf object
    m_anim = anim.GetPixbuf();

    // m_anim may be null in case wxNullAnimation has been passed
    if (m_anim)
    {
        // add a reference to the GdkPixbufAnimation
        g_object_ref(m_anim);

        if (!this->HasFlag(wxAC_NO_AUTORESIZE))
            FitToAnimation();

        // display first frame
        gtk_image_set_from_pixbuf(GTK_IMAGE(m_widget),
                                  gdk_pixbuf_animation_get_static_image(m_anim));
    }
    else
    {
        // we need to clear the control to the background colour
        ClearToBackgroundColour();
    }
}

void wxAnimationCtrl::FitToAnimation()
{
    if (!m_anim)
        return;

    int w = gdk_pixbuf_animation_get_width(m_anim),
        h = gdk_pixbuf_animation_get_height(m_anim);

    // update our size to fit animation
    //if (w > 0 && h > 0)
//        gtk_widget_set_size_request(m_widget, w, h);
        SetSize(w, h);
}

bool wxAnimationCtrl::Play()
{
    if (m_anim == NULL)
        return false;

    // init the iterator and start a one-shot timer
    ResetIter();
    m_iter = gdk_pixbuf_animation_get_iter (m_anim, NULL);
    m_bPlaying = true;

    // gdk_pixbuf_animation_iter_get_delay_time() may return -1 which means
    // that the timer should not start
    int n = gdk_pixbuf_animation_iter_get_delay_time(m_iter);
    if (n >= 0)
        m_timer.Start(n, true);

    return true;
}

void wxAnimationCtrl::Stop()
{
    // leave current frame displayed until Play() is called again
    if (IsPlaying())
        m_timer.Stop();
    m_bPlaying = false;
}

bool wxAnimationCtrl::IsPlaying() const
{
    // NB: we cannot just return m_timer.IsRunning() as this would not
    //     be safe as e.g. if we are displaying a frame forever,
    //     then we are "officially" still playing the animation, but
    //     the timer is not running anymore...
    return m_bPlaying;
}

wxSize wxAnimationCtrl::DoGetBestSize() const
{
    if (m_anim && !this->HasFlag(wxAC_NO_AUTORESIZE))
    {
        return wxSize(gdk_pixbuf_animation_get_width(m_anim),
                      gdk_pixbuf_animation_get_height(m_anim));
    }

    return wxSize(100,100);
}

void wxAnimationCtrl::ClearToBackgroundColour()
{
    wxSize sz = GetClientSize();
    GdkPixbuf *newpix = gdk_pixbuf_new(GDK_COLORSPACE_RGB, false, 8,
                                       sz.GetWidth(), sz.GetHeight());
    if (!newpix)
        return;

    wxColour clr = GetBackgroundColour();
    guint32 col = (clr.Red() << 24) | (clr.Green() << 16) | (clr.Blue() << 8);
    gdk_pixbuf_fill(newpix, col);

    wxLogDebug(wxT("Clearing to background %s"), clr.GetAsString().c_str());

    gtk_image_set_from_pixbuf(GTK_IMAGE(m_widget), newpix);
    g_object_unref(newpix);
}

bool wxAnimationCtrl::SetBackgroundColour( const wxColour &colour )
{
    // wxWindowGTK::SetBackgroundColour works but since our m_widget is a GtkImage
    // it won't show the background colour unlike the user would expect.
    // Thus we clear the GtkImage contents to the background colour...
    if (!wxControl::SetBackgroundColour(colour))
        return false;
    ClearToBackgroundColour();
    return true;
}


//-----------------------------------------------------------------------------
// wxAnimationCtrl - event handlers
//-----------------------------------------------------------------------------

void wxAnimationCtrl::OnTimer(wxTimerEvent &ev)
{
    wxASSERT(m_iter != NULL);

    // gdk_pixbuf_animation_iter_advance() will automatically restart
    // the animation, if necessary and we have no way to know !!
    if (gdk_pixbuf_animation_iter_advance(m_iter, NULL))
    {
        // start a new one-shot timer
        int n = gdk_pixbuf_animation_iter_get_delay_time(m_iter);
        if (n >= 0)
            m_timer.Start(n, true);

        gtk_image_set_from_pixbuf(GTK_IMAGE(m_widget),
                                  gdk_pixbuf_animation_iter_get_pixbuf(m_iter));
    }
    else
    {
        // no need to update the m_widget yet
        m_timer.Start(10, true);
    }
}

#endif      // wxUSE_ANIMATIONCTRL
