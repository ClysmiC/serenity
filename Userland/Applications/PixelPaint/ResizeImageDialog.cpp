/*
 * Copyright (c) 2022, Andrew Smith <andrew@alsmith.net>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "ResizeImageDialog.h"
#include <Applications/PixelPaint/ResizeImageDialogGML.h>
#include <LibGUI/BoxLayout.h>
#include <LibGUI/Button.h>
#include <LibGUI/ComboBox.h>
#include <LibGUI/Label.h>
#include <LibGUI/SpinBox.h>

namespace PixelPaint {

ResizeImageDialog::ResizeImageDialog(Gfx::IntSize const& suggested_size, GUI::Window* parent_window)
    : Dialog(parent_window)
{
    set_title("Resize Image");
    resize(260, 200);
    set_icon(parent_window->icon());

    auto& main_widget = set_main_widget<GUI::Widget>();
    if (!main_widget.load_from_gml(resize_image_dialog_gml))
        VERIFY_NOT_REACHED();

    auto width_spinbox = main_widget.find_descendant_of_type_named<GUI::SpinBox>("width_spinbox");
    auto height_spinbox = main_widget.find_descendant_of_type_named<GUI::SpinBox>("height_spinbox");

    VERIFY(width_spinbox);
    VERIFY(height_spinbox);

    width_spinbox->set_value(suggested_size.width());
    height_spinbox->set_value(suggested_size.height());
}

}
