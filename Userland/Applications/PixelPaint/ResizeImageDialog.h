/*
 * Copyright (c) 2022, Andrew Smith <andrew@alsmith.net>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibGUI/Dialog.h>

namespace PixelPaint {

class ResizeImageDialog final : public GUI::Dialog {
    C_OBJECT(ResizeImageDialog);

public:
    Gfx::IntSize const& new_size() const { return m_new_size; }

private:
    ResizeImageDialog(Gfx::IntSize const& suggested_size, GUI::Window* parent_window);

    Gfx::IntSize m_new_size;
};

}
