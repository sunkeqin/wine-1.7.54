/*
 * Copyright 2015 Martin Storsjo
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

#include "windows.h"

#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(vcruntime);

/*********************************************************************
 *              __telemetry_main_invoke_trigger
 */
void CDECL __telemetry_main_invoke_trigger(HINSTANCE hinst)
{
    FIXME("(%p)\n", hinst);
}

/*********************************************************************
 *              __telemetry_main_return_trigger
 */
void CDECL __telemetry_main_return_trigger(HINSTANCE hinst)
{
    FIXME("(%p)\n", hinst);
}
