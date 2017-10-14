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
 * The Original Code is mozilla.org code.
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

#ifndef nsDragService_h__
#define nsDragService_h__

#include "nsBaseDragService.h"
#include <windows.h>
#include <shlobj.h>

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
// {DE5BF786-477A-11d2-839D-00C04FD918D0}
DEFINE_GUID(IID_IDragSourceHelper,  0xde5bf786, 0x477a, 0x11d2, 0x83, 0x9d, 0x0, 0xc0, 0x4f, 0xd9, 0x18, 0xd0);

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
#define INTERFACE IDragSourceHelper

DECLARE_INTERFACE_( IDragSourceHelper, IUnknown )
{
    // IUnknown methods
    STDMETHOD (QueryInterface)(THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG, AddRef) ( THIS ) PURE;
    STDMETHOD_(ULONG, Release) ( THIS ) PURE;

    // IDragSourceHelper
    STDMETHOD (InitializeFromBitmap)(THIS_ LPSHDRAGIMAGE pshdi,
                                     IDataObject* pDataObject) PURE;
    STDMETHOD (InitializeFromWindow)(THIS_ HWND hwnd, POINT* ppt,
                                     IDataObject* pDataObject) PURE;
};
#endif

struct IDropSource;
struct IDataObject;
class  nsNativeDragTarget;
class  nsDataObjCollection;
class  nsString;

/**
 * Native Win32 DragService wrapper
 */

class nsDragService : public nsBaseDragService
{
public:
  nsDragService();
  virtual ~nsDragService();
  
  // nsIDragService
  NS_IMETHOD InvokeDragSession(nsIDOMNode *aDOMNode,
                               nsISupportsArray *anArrayTransferables,
                               nsIScriptableRegion *aRegion,
                               PRUint32 aActionType);

  // nsIDragSession
  NS_IMETHOD GetData(nsITransferable * aTransferable, PRUint32 anItem);
  NS_IMETHOD GetNumDropItems(PRUint32 * aNumItems);
  NS_IMETHOD IsDataFlavorSupported(const char *aDataFlavor, PRBool *_retval);
  NS_IMETHOD EndDragSession(PRBool aDoneDrag);

  // native impl.
  NS_IMETHOD SetIDataObject(IDataObject * aDataObj);
  NS_IMETHOD StartInvokingDragSession(IDataObject * aDataObj,
                                      PRUint32 aActionType);

  // A drop occured within the application vs. outside of it.
  void SetDroppedLocal();

protected:
  nsDataObjCollection* GetDataObjCollection(IDataObject * aDataObj);

  // determine if we have a single data object or one of our private
  // collections
  PRBool IsCollectionObject(IDataObject* inDataObj);

  // gets shell version
  PRUint64 GetShellVersion();

  // Create a bitmap for drag operations
  PRBool CreateDragImage(nsIDOMNode *aDOMNode,
                         nsIScriptableRegion *aRegion,
                         SHDRAGIMAGE *psdi);

  IDropSource * mNativeDragSrc;
  nsNativeDragTarget * mNativeDragTarget;
  IDataObject * mDataObject;
  PRPackedBool mSentLocalDropEvent;
};

#endif // nsDragService_h__
