/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is Mozilla Communicator client code.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */
#ifndef _nsNativeDragTarget_h_
#define _nsNativeDragTarget_h_

#include "nsCOMPtr.h"
#include "nsIDragSession.h"
#include <ole2.h>
#include <shlobj.h>

#ifndef IDropTargetHelper
#ifndef __MINGW32__   // MingW does not provide shobjidl.h.
#include <shobjidl.h> // Vista drag image interfaces
#endif  // MingW
#endif

class nsIDragService;
class nsIWidget;

struct IDataObject;

#if WINVER < 0x0500
#ifndef GUID_DEFINED
#define GUID_DEFINED
#if defined(__midl)
typedef struct {
    unsigned long  Data1;
    unsigned short Data2;
    unsigned short Data3;
    byte           Data4[ 8 ];
} GUID;
#else
typedef struct _GUID {
    unsigned long  Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char  Data4[ 8 ];
} GUID;
#endif
#endif

#ifndef DECLSPEC_SELECTANY
#if (_MSC_VER >= 1100)
#define DECLSPEC_SELECTANY  __declspec(selectany)
#else
#define DECLSPEC_SELECTANY
#endif
#endif

#ifndef EXTERN_C
#ifdef __cplusplus
#define EXTERN_C    extern "C"
#else
#define EXTERN_C    extern
#endif
#endif

#undef DEFINE_GUID
#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
        EXTERN_C const GUID DECLSPEC_SELECTANY name \
                = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }
// {4657278A-411B-11d2-839A-00C04FD918D0}
DEFINE_GUID(CLSID_DragDropHelper,   0x4657278a, 0x411b, 0x11d2, 0x83, 0x9a, 0x0, 0xc0, 0x4f, 0xd9, 0x18, 0xd0);

// {4657278B-411B-11d2-839A-00C04FD918D0}
DEFINE_GUID(IID_IDropTargetHelper,  0x4657278b, 0x411b, 0x11d2, 0x83, 0x9a, 0x0, 0xc0, 0x4f, 0xd9, 0x18, 0xd0);

#include <pshpack8.h>

#ifndef SHDRAGIMAGE_DEFINED
#define SHDRAGIMAGE_DEFINED
typedef struct
{
    SIZE        sizeDragImage;      // OUT - The length and Width of the
                                    //        rendered image
    POINT       ptOffset;           // OUT - The Offset from the mouse cursor to
                                    //        the upper left corner of the image
    HBITMAP     hbmpDragImage;      // OUT - The Bitmap containing the rendered
                                    //        drag images
    COLORREF    crColorKey;         // OUT - The COLORREF that has been blitted
                                    //        to the background of the images
} SHDRAGIMAGE, *LPSHDRAGIMAGE;
#endif

#include <poppack.h>        /* Return to byte packing */

// This is sent to a window to get the rendered images to a bitmap
// Call RegisterWindowMessage to get the ID
#define DI_GETDRAGIMAGE     TEXT("ShellGetDragImage")

#undef INTERFACE
#define INTERFACE IDropTargetHelper

DECLARE_INTERFACE_( IDropTargetHelper, IUnknown )
{
    // IUnknown methods
    STDMETHOD (QueryInterface)(THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG, AddRef) ( THIS ) PURE;
    STDMETHOD_(ULONG, Release) ( THIS ) PURE;

    // IDropTargetHelper
    STDMETHOD (DragEnter)(THIS_ HWND hwndTarget, IDataObject* pDataObject,
                          POINT* ppt, DWORD dwEffect) PURE;
    STDMETHOD (DragLeave)(THIS) PURE;
    STDMETHOD (DragOver)(THIS_ POINT* ppt, DWORD dwEffect) PURE;
    STDMETHOD (Drop)(THIS_ IDataObject* pDataObject, POINT* ppt,
                     DWORD dwEffect) PURE;
    STDMETHOD (Show)(THIS_ BOOL fShow) PURE;

};
#endif

/*
 * nsNativeDragTarget implements the IDropTarget interface and gets most of its
 * behavior from the associated adapter (m_dragDrop).
 */

class nsNativeDragTarget : public IDropTarget
{
public:
  nsNativeDragTarget(nsIWidget * aWnd);
  ~nsNativeDragTarget();

  // IUnknown members - see iunknown.h for documentation
  STDMETHODIMP QueryInterface(REFIID, void**);
  STDMETHODIMP_(ULONG) AddRef();
  STDMETHODIMP_(ULONG) Release();

  // IDataTarget members

  // Set pEffect based on whether this object can support a drop based on
  // the data available from pSource, the key and mouse states specified
  // in grfKeyState, and the coordinates specified by point. This is
  // called by OLE when a drag enters this object's window (as registered
  // by Initialize).
  STDMETHODIMP DragEnter(LPDATAOBJECT pSource, DWORD grfKeyState,
                         POINTL point, DWORD* pEffect);

  // Similar to DragEnter except it is called frequently while the drag
  // is over this object's window.
  STDMETHODIMP DragOver(DWORD grfKeyState, POINTL point, DWORD* pEffect);

  // Release the drag-drop source and put internal state back to the point
  // before the call to DragEnter. This is called when the drag leaves
  // without a drop occurring.
  STDMETHODIMP DragLeave();

  // If point is within our region of interest and pSource's data supports
  // one of our formats, get the data and set pEffect according to
  // grfKeyState (DROPEFFECT_MOVE if the control key was not pressed,
  // DROPEFFECT_COPY if the control key was pressed). Otherwise return
  // E_FAIL.
  STDMETHODIMP Drop(LPDATAOBJECT pSource, DWORD grfKeyState,
                    POINTL point, DWORD* pEffect);

  PRBool           mDragCancelled;

protected:

  void GetGeckoDragAction(LPDATAOBJECT pData, DWORD grfKeyState,
                          LPDWORD pdwEffect, PRUint32 * aGeckoAction);
  void ProcessDrag(LPDATAOBJECT pData, PRUint32 aEventType, DWORD grfKeyState,
                   POINTL pt, DWORD* pdwEffect);
  void DispatchDragDropEvent(PRUint32 aType, POINTL pt);

  // Native Stuff
  ULONG            m_cRef;      // reference count
  HWND             mHWnd;
  PRBool           mCanMove;
  PRBool           mMovePreferred;
  PRBool           mTookOwnRef;

  // Gecko Stuff
  nsIWidget      * mWindow;
  nsIDragService * mDragService;

  // Drag target helper 
  IDropTargetHelper * mDropTargetHelper;
};

#endif // _nsNativeDragTarget_h_


