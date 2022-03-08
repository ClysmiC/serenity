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

private:
    ResizeImageDialog(Gfx::IntSize const& suggested_size, GUI::Window* parent_window);

    Gfx::IntSize m_new_size;

    RefPtr<GUI::SpinBox> m_width_spinbox;
    RefPtr<GUI::SpinBox> m_height_spinbox;
    RefPtr<GUI::ComboBox> m_anchor_combobox;
};

}
