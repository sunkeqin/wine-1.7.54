/*
 * Copyright 2014 Henri Verbeet for CodeWeavers
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include "config.h"
#include "wine/port.h"

#include "d2d1_private.h"

WINE_DEFAULT_DEBUG_CHANNEL(d2d);

static inline struct d2d_gradient *impl_from_ID2D1GradientStopCollection(ID2D1GradientStopCollection *iface)
{
    return CONTAINING_RECORD(iface, struct d2d_gradient, ID2D1GradientStopCollection_iface);
}

static HRESULT STDMETHODCALLTYPE d2d_gradient_QueryInterface(ID2D1GradientStopCollection *iface,
        REFIID iid, void **out)
{
    TRACE("iface %p, iid %s, out %p.\n", iface, debugstr_guid(iid), out);

    if (IsEqualGUID(iid, &IID_ID2D1GradientStopCollection)
            || IsEqualGUID(iid, &IID_ID2D1Resource)
            || IsEqualGUID(iid, &IID_IUnknown))
    {
        ID2D1GradientStopCollection_AddRef(iface);
        *out = iface;
        return S_OK;
    }

    WARN("%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid(iid));

    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG STDMETHODCALLTYPE d2d_gradient_AddRef(ID2D1GradientStopCollection *iface)
{
    struct d2d_gradient *gradient = impl_from_ID2D1GradientStopCollection(iface);
    ULONG refcount = InterlockedIncrement(&gradient->refcount);

    TRACE("%p increasing refcount to %u.\n", iface, refcount);

    return refcount;
}

static ULONG STDMETHODCALLTYPE d2d_gradient_Release(ID2D1GradientStopCollection *iface)
{
    struct d2d_gradient *gradient = impl_from_ID2D1GradientStopCollection(iface);
    ULONG refcount = InterlockedDecrement(&gradient->refcount);

    TRACE("%p decreasing refcount to %u.\n", iface, refcount);

    if (!refcount)
    {
        HeapFree(GetProcessHeap(), 0, gradient->stops);
        ID2D1Factory_Release(gradient->factory);
        HeapFree(GetProcessHeap(), 0, gradient);
    }

    return refcount;
}

static void STDMETHODCALLTYPE d2d_gradient_GetFactory(ID2D1GradientStopCollection *iface, ID2D1Factory **factory)
{
    struct d2d_gradient *gradient = impl_from_ID2D1GradientStopCollection(iface);

    TRACE("iface %p, factory %p.\n", iface, factory);

    ID2D1Factory_AddRef(*factory = gradient->factory);
}

static UINT32 STDMETHODCALLTYPE d2d_gradient_GetGradientStopCount(ID2D1GradientStopCollection *iface)
{
    struct d2d_gradient *gradient = impl_from_ID2D1GradientStopCollection(iface);

    TRACE("iface %p.\n", iface);

    return gradient->stop_count;
}

static void STDMETHODCALLTYPE d2d_gradient_GetGradientStops(ID2D1GradientStopCollection *iface,
        D2D1_GRADIENT_STOP *stops, UINT32 stop_count)
{
    struct d2d_gradient *gradient = impl_from_ID2D1GradientStopCollection(iface);

    TRACE("iface %p, stops %p, stop_count %u.\n", iface, stops, stop_count);

    memcpy(stops, gradient->stops, min(gradient->stop_count, stop_count) * sizeof(*stops));
    if (stop_count > gradient->stop_count)
        memset(stops, 0, (stop_count - gradient->stop_count) * sizeof(*stops));
}

static D2D1_GAMMA STDMETHODCALLTYPE d2d_gradient_GetColorInterpolationGamma(ID2D1GradientStopCollection *iface)
{
    FIXME("iface %p stub!\n", iface);

    return D2D1_GAMMA_1_0;
}

static D2D1_EXTEND_MODE STDMETHODCALLTYPE d2d_gradient_GetExtendMode(ID2D1GradientStopCollection *iface)
{
    FIXME("iface %p stub!\n", iface);

    return D2D1_EXTEND_MODE_CLAMP;
}

static const struct ID2D1GradientStopCollectionVtbl d2d_gradient_vtbl =
{
    d2d_gradient_QueryInterface,
    d2d_gradient_AddRef,
    d2d_gradient_Release,
    d2d_gradient_GetFactory,
    d2d_gradient_GetGradientStopCount,
    d2d_gradient_GetGradientStops,
    d2d_gradient_GetColorInterpolationGamma,
    d2d_gradient_GetExtendMode,
};

HRESULT d2d_gradient_init(struct d2d_gradient *gradient, ID2D1Factory *factory,
        const D2D1_GRADIENT_STOP *stops, UINT32 stop_count, D2D1_GAMMA gamma, D2D1_EXTEND_MODE extend_mode)
{
    FIXME("Ignoring gradient properties.\n");

    gradient->ID2D1GradientStopCollection_iface.lpVtbl = &d2d_gradient_vtbl;
    gradient->refcount = 1;
    ID2D1Factory_AddRef(gradient->factory = factory);

    gradient->stop_count = stop_count;
    if (!(gradient->stops = HeapAlloc(GetProcessHeap(), 0, stop_count * sizeof(*stops))))
        return E_OUTOFMEMORY;
    memcpy(gradient->stops, stops, stop_count * sizeof(*stops));

    return S_OK;
}

static void d2d_brush_destroy(struct d2d_brush *brush)
{
    ID2D1Factory_Release(brush->factory);
    HeapFree(GetProcessHeap(), 0, brush);
}

static void d2d_brush_init(struct d2d_brush *brush, ID2D1Factory *factory,
        enum d2d_brush_type type, const D2D1_BRUSH_PROPERTIES *desc, const struct ID2D1BrushVtbl *vtbl)
{
    static const D2D1_MATRIX_3X2_F identity =
    {
        1.0f, 0.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
    };

    brush->ID2D1Brush_iface.lpVtbl = vtbl;
    brush->refcount = 1;
    ID2D1Factory_AddRef(brush->factory = factory);
    brush->opacity = desc ? desc->opacity : 1.0f;
    brush->transform = desc ? desc->transform : identity;
    brush->type = type;
}

static inline struct d2d_brush *impl_from_ID2D1SolidColorBrush(ID2D1SolidColorBrush *iface)
{
    return CONTAINING_RECORD(iface, struct d2d_brush, ID2D1Brush_iface);
}

static HRESULT STDMETHODCALLTYPE d2d_solid_color_brush_QueryInterface(ID2D1SolidColorBrush *iface,
        REFIID iid, void **out)
{
    TRACE("iface %p, iid %s, out %p.\n", iface, debugstr_guid(iid), out);

    if (IsEqualGUID(iid, &IID_ID2D1SolidColorBrush)
            || IsEqualGUID(iid, &IID_ID2D1Brush)
            || IsEqualGUID(iid, &IID_ID2D1Resource)
            || IsEqualGUID(iid, &IID_IUnknown))
    {
        ID2D1SolidColorBrush_AddRef(iface);
        *out = iface;
        return S_OK;
    }

    WARN("%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid(iid));

    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG STDMETHODCALLTYPE d2d_solid_color_brush_AddRef(ID2D1SolidColorBrush *iface)
{
    struct d2d_brush *brush = impl_from_ID2D1SolidColorBrush(iface);
    ULONG refcount = InterlockedIncrement(&brush->refcount);

    TRACE("%p increasing refcount to %u.\n", iface, refcount);

    return refcount;
}

static ULONG STDMETHODCALLTYPE d2d_solid_color_brush_Release(ID2D1SolidColorBrush *iface)
{
    struct d2d_brush *brush = impl_from_ID2D1SolidColorBrush(iface);
    ULONG refcount = InterlockedDecrement(&brush->refcount);

    TRACE("%p decreasing refcount to %u.\n", iface, refcount);

    if (!refcount)
        d2d_brush_destroy(brush);

    return refcount;
}

static void STDMETHODCALLTYPE d2d_solid_color_brush_GetFactory(ID2D1SolidColorBrush *iface, ID2D1Factory **factory)
{
    struct d2d_brush *brush = impl_from_ID2D1SolidColorBrush(iface);

    TRACE("iface %p, factory %p.\n", iface, factory);

    ID2D1Factory_AddRef(*factory = brush->factory);
}

static void STDMETHODCALLTYPE d2d_solid_color_brush_SetOpacity(ID2D1SolidColorBrush *iface, float opacity)
{
    struct d2d_brush *brush = impl_from_ID2D1SolidColorBrush(iface);

    TRACE("iface %p, opacity %.8e.\n", iface, opacity);

    brush->opacity = opacity;
}

static void STDMETHODCALLTYPE d2d_solid_color_brush_SetTransform(ID2D1SolidColorBrush *iface,
        const D2D1_MATRIX_3X2_F *transform)
{
    struct d2d_brush *brush = impl_from_ID2D1SolidColorBrush(iface);

    TRACE("iface %p, transform %p.\n", iface, transform);

    brush->transform = *transform;
}

static float STDMETHODCALLTYPE d2d_solid_color_brush_GetOpacity(ID2D1SolidColorBrush *iface)
{
    struct d2d_brush *brush = impl_from_ID2D1SolidColorBrush(iface);

    TRACE("iface %p.\n", iface);

    return brush->opacity;
}

static void STDMETHODCALLTYPE d2d_solid_color_brush_GetTransform(ID2D1SolidColorBrush *iface,
        D2D1_MATRIX_3X2_F *transform)
{
    struct d2d_brush *brush = impl_from_ID2D1SolidColorBrush(iface);

    TRACE("iface %p, transform %p.\n", iface, transform);

    *transform = brush->transform;
}

static void STDMETHODCALLTYPE d2d_solid_color_brush_SetColor(ID2D1SolidColorBrush *iface, const D2D1_COLOR_F *color)
{
    struct d2d_brush *brush = impl_from_ID2D1SolidColorBrush(iface);

    TRACE("iface %p, color %p.\n", iface, color);

    brush->u.solid.color = *color;
}

static D2D1_COLOR_F * STDMETHODCALLTYPE d2d_solid_color_brush_GetColor(ID2D1SolidColorBrush *iface, D2D1_COLOR_F *color)
{
    struct d2d_brush *brush = impl_from_ID2D1SolidColorBrush(iface);

    TRACE("iface %p, color %p.\n", iface, color);

    *color = brush->u.solid.color;
    return color;
}

static const struct ID2D1SolidColorBrushVtbl d2d_solid_color_brush_vtbl =
{
    d2d_solid_color_brush_QueryInterface,
    d2d_solid_color_brush_AddRef,
    d2d_solid_color_brush_Release,
    d2d_solid_color_brush_GetFactory,
    d2d_solid_color_brush_SetOpacity,
    d2d_solid_color_brush_SetTransform,
    d2d_solid_color_brush_GetOpacity,
    d2d_solid_color_brush_GetTransform,
    d2d_solid_color_brush_SetColor,
    d2d_solid_color_brush_GetColor,
};

void d2d_solid_color_brush_init(struct d2d_brush *brush, ID2D1Factory *factory,
        const D2D1_COLOR_F *color, const D2D1_BRUSH_PROPERTIES *desc)
{
    d2d_brush_init(brush, factory, D2D_BRUSH_TYPE_SOLID, desc,
            (ID2D1BrushVtbl *)&d2d_solid_color_brush_vtbl);
    brush->u.solid.color = *color;
}

static inline struct d2d_brush *impl_from_ID2D1LinearGradientBrush(ID2D1LinearGradientBrush *iface)
{
    return CONTAINING_RECORD(iface, struct d2d_brush, ID2D1Brush_iface);
}

static HRESULT STDMETHODCALLTYPE d2d_linear_gradient_brush_QueryInterface(ID2D1LinearGradientBrush *iface,
        REFIID iid, void **out)
{
    TRACE("iface %p, iid %s, out %p.\n", iface, debugstr_guid(iid), out);

    if (IsEqualGUID(iid, &IID_ID2D1LinearGradientBrush)
            || IsEqualGUID(iid, &IID_ID2D1Brush)
            || IsEqualGUID(iid, &IID_ID2D1Resource)
            || IsEqualGUID(iid, &IID_IUnknown))
    {
        ID2D1LinearGradientBrush_AddRef(iface);
        *out = iface;
        return S_OK;
    }

    WARN("%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid(iid));

    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG STDMETHODCALLTYPE d2d_linear_gradient_brush_AddRef(ID2D1LinearGradientBrush *iface)
{
    struct d2d_brush *brush = impl_from_ID2D1LinearGradientBrush(iface);
    ULONG refcount = InterlockedIncrement(&brush->refcount);

    TRACE("%p increasing refcount to %u.\n", iface, refcount);

    return refcount;
}

static ULONG STDMETHODCALLTYPE d2d_linear_gradient_brush_Release(ID2D1LinearGradientBrush *iface)
{
    struct d2d_brush *brush = impl_from_ID2D1LinearGradientBrush(iface);
    ULONG refcount = InterlockedDecrement(&brush->refcount);

    TRACE("%p decreasing refcount to %u.\n", iface, refcount);

    if (!refcount)
        d2d_brush_destroy(brush);

    return refcount;
}

static void STDMETHODCALLTYPE d2d_linear_gradient_brush_GetFactory(ID2D1LinearGradientBrush *iface,
        ID2D1Factory **factory)
{
    struct d2d_brush *brush = impl_from_ID2D1LinearGradientBrush(iface);

    TRACE("iface %p, factory %p.\n", iface, factory);

    ID2D1Factory_AddRef(*factory = brush->factory);
}

static void STDMETHODCALLTYPE d2d_linear_gradient_brush_SetOpacity(ID2D1LinearGradientBrush *iface, float opacity)
{
    FIXME("iface %p, opacity %.8e stub!\n", iface, opacity);
}

static void STDMETHODCALLTYPE d2d_linear_gradient_brush_SetTransform(ID2D1LinearGradientBrush *iface,
        const D2D1_MATRIX_3X2_F *transform)
{
    struct d2d_brush *brush = impl_from_ID2D1LinearGradientBrush(iface);

    TRACE("iface %p, transform %p.\n", iface, transform);

    brush->transform = *transform;
}

static float STDMETHODCALLTYPE d2d_linear_gradient_brush_GetOpacity(ID2D1LinearGradientBrush *iface)
{
    FIXME("iface %p stub!\n", iface);

    return 0.0f;
}

static void STDMETHODCALLTYPE d2d_linear_gradient_brush_GetTransform(ID2D1LinearGradientBrush *iface,
        D2D1_MATRIX_3X2_F *transform)
{
    struct d2d_brush *brush = impl_from_ID2D1LinearGradientBrush(iface);

    TRACE("iface %p, transform %p.\n", iface, transform);

    *transform = brush->transform;
}

static void STDMETHODCALLTYPE d2d_linear_gradient_brush_SetStartPoint(ID2D1LinearGradientBrush *iface,
        D2D1_POINT_2F start_point)
{
    FIXME("iface %p, start_point {%.8e, %.8e} stub!\n", iface, start_point.x, start_point.y);
}

static void STDMETHODCALLTYPE d2d_linear_gradient_brush_SetEndPoint(ID2D1LinearGradientBrush *iface,
        D2D1_POINT_2F end_point)
{
    FIXME("iface %p, end_point {%.8e, %.8e} stub!\n", iface, end_point.x, end_point.y);
}

static D2D1_POINT_2F * STDMETHODCALLTYPE d2d_linear_gradient_brush_GetStartPoint(ID2D1LinearGradientBrush *iface,
        D2D1_POINT_2F *point)
{
    FIXME("iface %p, point %p stub!\n", iface, point);

    point->x = 0.0f;
    point->y = 0.0f;
    return point;
}

static D2D1_POINT_2F * STDMETHODCALLTYPE d2d_linear_gradient_brush_GetEndPoint(ID2D1LinearGradientBrush *iface,
        D2D1_POINT_2F *point)
{
    FIXME("iface %p, point %p stub!\n", iface, point);

    point->x = 0.0f;
    point->y = 0.0f;
    return point;
}

static void STDMETHODCALLTYPE d2d_linear_gradient_brush_GetGradientStopCollection(ID2D1LinearGradientBrush *iface,
        ID2D1GradientStopCollection **gradient)
{
    FIXME("iface %p, gradient %p stub!\n", iface, gradient);

    *gradient = NULL;
}

static const struct ID2D1LinearGradientBrushVtbl d2d_linear_gradient_brush_vtbl =
{
    d2d_linear_gradient_brush_QueryInterface,
    d2d_linear_gradient_brush_AddRef,
    d2d_linear_gradient_brush_Release,
    d2d_linear_gradient_brush_GetFactory,
    d2d_linear_gradient_brush_SetOpacity,
    d2d_linear_gradient_brush_SetTransform,
    d2d_linear_gradient_brush_GetOpacity,
    d2d_linear_gradient_brush_GetTransform,
    d2d_linear_gradient_brush_SetStartPoint,
    d2d_linear_gradient_brush_SetEndPoint,
    d2d_linear_gradient_brush_GetStartPoint,
    d2d_linear_gradient_brush_GetEndPoint,
    d2d_linear_gradient_brush_GetGradientStopCollection,
};

void d2d_linear_gradient_brush_init(struct d2d_brush *brush, ID2D1Factory *factory,
        const D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES *gradient_brush_desc, const D2D1_BRUSH_PROPERTIES *brush_desc,
        ID2D1GradientStopCollection *gradient)
{
    FIXME("Ignoring brush properties.\n");

    d2d_brush_init(brush, factory, D2D_BRUSH_TYPE_LINEAR, brush_desc,
            (ID2D1BrushVtbl *)&d2d_linear_gradient_brush_vtbl);
}

static inline struct d2d_brush *impl_from_ID2D1BitmapBrush(ID2D1BitmapBrush *iface)
{
    return CONTAINING_RECORD(iface, struct d2d_brush, ID2D1Brush_iface);
}

static HRESULT STDMETHODCALLTYPE d2d_bitmap_brush_QueryInterface(ID2D1BitmapBrush *iface,
        REFIID iid, void **out)
{
    TRACE("iface %p, iid %s, out %p.\n", iface, debugstr_guid(iid), out);

    if (IsEqualGUID(iid, &IID_ID2D1BitmapBrush)
            || IsEqualGUID(iid, &IID_ID2D1Brush)
            || IsEqualGUID(iid, &IID_ID2D1Resource)
            || IsEqualGUID(iid, &IID_IUnknown))
    {
        ID2D1BitmapBrush_AddRef(iface);
        *out = iface;
        return S_OK;
    }

    WARN("%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid(iid));

    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG STDMETHODCALLTYPE d2d_bitmap_brush_AddRef(ID2D1BitmapBrush *iface)
{
    struct d2d_brush *brush = impl_from_ID2D1BitmapBrush(iface);
    ULONG refcount = InterlockedIncrement(&brush->refcount);

    TRACE("%p increasing refcount to %u.\n", iface, refcount);

    return refcount;
}

static ULONG STDMETHODCALLTYPE d2d_bitmap_brush_Release(ID2D1BitmapBrush *iface)
{
    struct d2d_brush *brush = impl_from_ID2D1BitmapBrush(iface);
    ULONG refcount = InterlockedDecrement(&brush->refcount);

    TRACE("%p decreasing refcount to %u.\n", iface, refcount);

    if (!refcount)
    {
        if (brush->u.bitmap.sampler_state)
            ID3D10SamplerState_Release(brush->u.bitmap.sampler_state);
        if (brush->u.bitmap.bitmap)
            ID2D1Bitmap_Release(&brush->u.bitmap.bitmap->ID2D1Bitmap_iface);
        d2d_brush_destroy(brush);
    }

    return refcount;
}

static void STDMETHODCALLTYPE d2d_bitmap_brush_GetFactory(ID2D1BitmapBrush *iface,
        ID2D1Factory **factory)
{
    struct d2d_brush *brush = impl_from_ID2D1BitmapBrush(iface);

    TRACE("iface %p, factory %p.\n", iface, factory);

    ID2D1Factory_AddRef(*factory = brush->factory);
}

static void STDMETHODCALLTYPE d2d_bitmap_brush_SetOpacity(ID2D1BitmapBrush *iface, float opacity)
{
    struct d2d_brush *brush = impl_from_ID2D1BitmapBrush(iface);

    TRACE("iface %p, opacity %.8e.\n", iface, opacity);

    brush->opacity = opacity;
}

static void STDMETHODCALLTYPE d2d_bitmap_brush_SetTransform(ID2D1BitmapBrush *iface,
        const D2D1_MATRIX_3X2_F *transform)
{
    struct d2d_brush *brush = impl_from_ID2D1BitmapBrush(iface);

    TRACE("iface %p, transform %p.\n", iface, transform);

    brush->transform = *transform;
}

static float STDMETHODCALLTYPE d2d_bitmap_brush_GetOpacity(ID2D1BitmapBrush *iface)
{
    struct d2d_brush *brush = impl_from_ID2D1BitmapBrush(iface);

    TRACE("iface %p.\n", iface);

    return brush->opacity;
}

static void STDMETHODCALLTYPE d2d_bitmap_brush_GetTransform(ID2D1BitmapBrush *iface,
        D2D1_MATRIX_3X2_F *transform)
{
    struct d2d_brush *brush = impl_from_ID2D1BitmapBrush(iface);

    TRACE("iface %p, transform %p.\n", iface, transform);

    *transform = brush->transform;
}

static void STDMETHODCALLTYPE d2d_bitmap_brush_SetExtendModeX(ID2D1BitmapBrush *iface, D2D1_EXTEND_MODE mode)
{
    struct d2d_brush *brush = impl_from_ID2D1BitmapBrush(iface);

    TRACE("iface %p, mode %#x.\n", iface, mode);

    brush->u.bitmap.extend_mode_x = mode;
    if (brush->u.bitmap.sampler_state)
    {
        ID3D10SamplerState_Release(brush->u.bitmap.sampler_state);
        brush->u.bitmap.sampler_state = NULL;
    }
}

static void STDMETHODCALLTYPE d2d_bitmap_brush_SetExtendModeY(ID2D1BitmapBrush *iface, D2D1_EXTEND_MODE mode)
{
    struct d2d_brush *brush = impl_from_ID2D1BitmapBrush(iface);

    TRACE("iface %p, mode %#x.\n", iface, mode);

    brush->u.bitmap.extend_mode_y = mode;
    if (brush->u.bitmap.sampler_state)
    {
        ID3D10SamplerState_Release(brush->u.bitmap.sampler_state);
        brush->u.bitmap.sampler_state = NULL;
    }
}

static void STDMETHODCALLTYPE d2d_bitmap_brush_SetInterpolationMode(ID2D1BitmapBrush *iface,
        D2D1_BITMAP_INTERPOLATION_MODE mode)
{
    struct d2d_brush *brush = impl_from_ID2D1BitmapBrush(iface);

    TRACE("iface %p, mode %#x.\n", iface, mode);

    brush->u.bitmap.interpolation_mode = mode;
    if (brush->u.bitmap.sampler_state)
    {
        ID3D10SamplerState_Release(brush->u.bitmap.sampler_state);
        brush->u.bitmap.sampler_state = NULL;
    }
}

static void STDMETHODCALLTYPE d2d_bitmap_brush_SetBitmap(ID2D1BitmapBrush *iface, ID2D1Bitmap *bitmap)
{
    struct d2d_brush *brush = impl_from_ID2D1BitmapBrush(iface);

    TRACE("iface %p, bitmap %p.\n", iface, bitmap);

    if (bitmap)
        ID2D1Bitmap_AddRef(bitmap);
    if (brush->u.bitmap.bitmap)
        ID2D1Bitmap_Release(&brush->u.bitmap.bitmap->ID2D1Bitmap_iface);
    brush->u.bitmap.bitmap = unsafe_impl_from_ID2D1Bitmap(bitmap);
}

static D2D1_EXTEND_MODE STDMETHODCALLTYPE d2d_bitmap_brush_GetExtendModeX(ID2D1BitmapBrush *iface)
{
    struct d2d_brush *brush = impl_from_ID2D1BitmapBrush(iface);

    TRACE("iface %p.\n", iface);

    return brush->u.bitmap.extend_mode_x;
}

static D2D1_EXTEND_MODE STDMETHODCALLTYPE d2d_bitmap_brush_GetExtendModeY(ID2D1BitmapBrush *iface)
{
    struct d2d_brush *brush = impl_from_ID2D1BitmapBrush(iface);

    TRACE("iface %p.\n", iface);

    return brush->u.bitmap.extend_mode_y;
}

static D2D1_BITMAP_INTERPOLATION_MODE STDMETHODCALLTYPE d2d_bitmap_brush_GetInterpolationMode(ID2D1BitmapBrush *iface)
{
    struct d2d_brush *brush = impl_from_ID2D1BitmapBrush(iface);

    TRACE("iface %p.\n", iface);

    return brush->u.bitmap.interpolation_mode;
}

static void STDMETHODCALLTYPE d2d_bitmap_brush_GetBitmap(ID2D1BitmapBrush *iface, ID2D1Bitmap **bitmap)
{
    struct d2d_brush *brush = impl_from_ID2D1BitmapBrush(iface);

    TRACE("iface %p, bitmap %p.\n", iface, bitmap);

    if ((*bitmap = &brush->u.bitmap.bitmap->ID2D1Bitmap_iface))
        ID2D1Bitmap_AddRef(*bitmap);
}

static const struct ID2D1BitmapBrushVtbl d2d_bitmap_brush_vtbl =
{
    d2d_bitmap_brush_QueryInterface,
    d2d_bitmap_brush_AddRef,
    d2d_bitmap_brush_Release,
    d2d_bitmap_brush_GetFactory,
    d2d_bitmap_brush_SetOpacity,
    d2d_bitmap_brush_SetTransform,
    d2d_bitmap_brush_GetOpacity,
    d2d_bitmap_brush_GetTransform,
    d2d_bitmap_brush_SetExtendModeX,
    d2d_bitmap_brush_SetExtendModeY,
    d2d_bitmap_brush_SetInterpolationMode,
    d2d_bitmap_brush_SetBitmap,
    d2d_bitmap_brush_GetExtendModeX,
    d2d_bitmap_brush_GetExtendModeY,
    d2d_bitmap_brush_GetInterpolationMode,
    d2d_bitmap_brush_GetBitmap,
};

void d2d_bitmap_brush_init(struct d2d_brush *brush, ID2D1Factory *factory, ID2D1Bitmap *bitmap,
        const D2D1_BITMAP_BRUSH_PROPERTIES *bitmap_brush_desc, const D2D1_BRUSH_PROPERTIES *brush_desc)
{
    d2d_brush_init(brush, factory, D2D_BRUSH_TYPE_BITMAP,
            brush_desc, (ID2D1BrushVtbl *)&d2d_bitmap_brush_vtbl);
    if ((brush->u.bitmap.bitmap = unsafe_impl_from_ID2D1Bitmap(bitmap)))
        ID2D1Bitmap_AddRef(&brush->u.bitmap.bitmap->ID2D1Bitmap_iface);
    if (bitmap_brush_desc)
    {
        brush->u.bitmap.extend_mode_x = bitmap_brush_desc->extendModeX;
        brush->u.bitmap.extend_mode_y = bitmap_brush_desc->extendModeY;
        brush->u.bitmap.interpolation_mode = bitmap_brush_desc->interpolationMode;
    }
    else
    {
        brush->u.bitmap.extend_mode_x = D2D1_EXTEND_MODE_CLAMP;
        brush->u.bitmap.extend_mode_y = D2D1_EXTEND_MODE_CLAMP;
        brush->u.bitmap.interpolation_mode = D2D1_BITMAP_INTERPOLATION_MODE_LINEAR;
    }
}

struct d2d_brush *unsafe_impl_from_ID2D1Brush(ID2D1Brush *iface)
{
    if (!iface)
        return NULL;
    assert(iface->lpVtbl == (const ID2D1BrushVtbl *)&d2d_solid_color_brush_vtbl
            || iface->lpVtbl == (const ID2D1BrushVtbl *)&d2d_linear_gradient_brush_vtbl
            || iface->lpVtbl == (const ID2D1BrushVtbl *)&d2d_bitmap_brush_vtbl);
    return CONTAINING_RECORD(iface, struct d2d_brush, ID2D1Brush_iface);
}

static D3D10_TEXTURE_ADDRESS_MODE texture_address_mode_from_extend_mode(D2D1_EXTEND_MODE mode)
{
    switch (mode)
    {
        case D2D1_EXTEND_MODE_CLAMP:
            return D3D10_TEXTURE_ADDRESS_CLAMP;
        case D2D1_EXTEND_MODE_WRAP:
            return D3D10_TEXTURE_ADDRESS_WRAP;
        case D2D1_EXTEND_MODE_MIRROR:
            return D3D10_TEXTURE_ADDRESS_MIRROR;
        default:
            FIXME("Unhandled extend mode %#x.\n", mode);
            return D3D10_TEXTURE_ADDRESS_CLAMP;
    }
}

HRESULT d2d_brush_get_ps_cb(struct d2d_brush *brush, struct d2d_d3d_render_target *render_target,
        ID3D10Buffer **ps_cb)
{
    D3D10_SUBRESOURCE_DATA buffer_data;
    D3D10_BUFFER_DESC buffer_desc;
    struct
    {
        float _11, _21, _31, pad0;
        float _12, _22, _32, opacity;
        BOOL ignore_alpha;
    } bitmap_brush_cb;
    D2D1_COLOR_F color;
    HRESULT hr;

    buffer_desc.Usage = D3D10_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
    buffer_desc.CPUAccessFlags = 0;
    buffer_desc.MiscFlags = 0;

    buffer_data.SysMemPitch = 0;
    buffer_data.SysMemSlicePitch = 0;

    if (brush->type == D2D_BRUSH_TYPE_SOLID)
    {
        color = brush->u.solid.color;
        color.a *= brush->opacity;
        color.r *= color.a;
        color.g *= color.a;
        color.b *= color.a;

        buffer_desc.ByteWidth = sizeof(color);
        buffer_data.pSysMem = &color;
    }
    else if (brush->type == D2D_BRUSH_TYPE_BITMAP)
    {
        struct d2d_bitmap *bitmap = brush->u.bitmap.bitmap;
        D2D_MATRIX_3X2_F w, b;
        float dpi_scale, d;

        /* Scale for dpi. */
        w = render_target->drawing_state.transform;
        dpi_scale = render_target->dpi_x / 96.0f;
        w._11 *= dpi_scale;
        w._21 *= dpi_scale;
        w._31 *= dpi_scale;
        dpi_scale = render_target->dpi_y / 96.0f;
        w._12 *= dpi_scale;
        w._22 *= dpi_scale;
        w._32 *= dpi_scale;

        /* Scale for bitmap size and dpi. */
        b = brush->transform;
        dpi_scale = bitmap->pixel_size.width * (96.0f / bitmap->dpi_x);
        b._11 *= dpi_scale;
        b._21 *= dpi_scale;
        dpi_scale = bitmap->pixel_size.height * (96.0f / bitmap->dpi_y);
        b._12 *= dpi_scale;
        b._22 *= dpi_scale;

        d2d_matrix_multiply(&b, &w);

        /* Invert the matrix. (Because the matrix is applied to the sampling
         * coordinates. I.e., to scale the bitmap by 2 we need to divide the
         * coordinates by 2.) */
        d = b._11 * b._22 - b._21 * b._12;
        if (d != 0.0f)
        {
            bitmap_brush_cb._11 = b._22 / d;
            bitmap_brush_cb._21 = -b._21 / d;
            bitmap_brush_cb._31 = (b._21 * b._32 - b._31 * b._22) / d;
            bitmap_brush_cb._12 = -b._12 / d;
            bitmap_brush_cb._22 = b._11 / d;
            bitmap_brush_cb._32 = -(b._11 * b._32 - b._31 * b._12) / d;
        }
        bitmap_brush_cb.opacity = brush->opacity;
        bitmap_brush_cb.ignore_alpha = bitmap->format.alphaMode == D2D1_ALPHA_MODE_IGNORE;

        buffer_desc.ByteWidth = sizeof(bitmap_brush_cb);
        buffer_data.pSysMem = &bitmap_brush_cb;
    }
    else
    {
        FIXME("Unhandled brush type %#x.\n", brush->type);
        return E_NOTIMPL;
    }

    if (FAILED(hr = ID3D10Device_CreateBuffer(render_target->device, &buffer_desc, &buffer_data, ps_cb)))
        ERR("Failed to create constant buffer, hr %#x.\n", hr);

    return hr;
}

void d2d_brush_bind_resources(struct d2d_brush *brush, struct d2d_d3d_render_target *render_target,
        enum d2d_shape_type shape_type)
{
    static const float blend_factor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    ID3D10Device *device = render_target->device;
    ID3D10PixelShader *ps;
    HRESULT hr;

    ID3D10Device_OMSetBlendState(device, render_target->bs, blend_factor, D3D10_DEFAULT_SAMPLE_MASK);
    if (!(ps = render_target->shape_resources[shape_type].ps[brush->type]))
        FIXME("No pixel shader for shape type %#x and brush type %#x.\n", shape_type, brush->type);
    ID3D10Device_PSSetShader(device, ps);

    if (brush->type == D2D_BRUSH_TYPE_BITMAP)
    {
        ID3D10Device_PSSetShaderResources(device, 0, 1, &brush->u.bitmap.bitmap->view);
        if (!brush->u.bitmap.sampler_state)
        {
            D3D10_SAMPLER_DESC sampler_desc;

            if (brush->u.bitmap.interpolation_mode == D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR)
                sampler_desc.Filter = D3D10_FILTER_MIN_MAG_MIP_POINT;
            else
                sampler_desc.Filter = D3D10_FILTER_MIN_MAG_MIP_LINEAR;
            sampler_desc.AddressU = texture_address_mode_from_extend_mode(brush->u.bitmap.extend_mode_x);
            sampler_desc.AddressV = texture_address_mode_from_extend_mode(brush->u.bitmap.extend_mode_y);
            sampler_desc.AddressW = D3D10_TEXTURE_ADDRESS_CLAMP;
            sampler_desc.MipLODBias = 0.0f;
            sampler_desc.MaxAnisotropy = 0;
            sampler_desc.ComparisonFunc = D3D10_COMPARISON_NEVER;
            sampler_desc.BorderColor[0] = 0.0f;
            sampler_desc.BorderColor[1] = 0.0f;
            sampler_desc.BorderColor[2] = 0.0f;
            sampler_desc.BorderColor[3] = 0.0f;
            sampler_desc.MinLOD = 0.0f;
            sampler_desc.MaxLOD = 0.0f;

            if (FAILED(hr = ID3D10Device_CreateSamplerState(device,
                    &sampler_desc, &brush->u.bitmap.sampler_state)))
                ERR("Failed to create sampler state, hr %#x.\n", hr);
        }
        ID3D10Device_PSSetSamplers(device, 0, 1, &brush->u.bitmap.sampler_state);
    }
    else if (brush->type != D2D_BRUSH_TYPE_SOLID)
    {
        FIXME("Unhandled brush type %#x.\n", brush->type);
    }
}
