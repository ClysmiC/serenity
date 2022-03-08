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
    m_new_size.set_width(max(1, suggested_size.width()));
    m_new_size.set_height(max(1, suggested_size.height()));
    m_aspect_ratio_orig = m_new_size.width() / m_new_size.height();

    set_title("Resize Image");
    resize(260, 200);
    set_icon(parent_window->icon());

    auto& main_widget = set_main_widget<GUI::Widget>();
    if (!main_widget.load_from_gml(resize_image_dialog_gml))
        VERIFY_NOT_REACHED();

    auto width_spinbox = main_widget.find_descendant_of_type_named<GUI::SpinBox>("width_spinbox");
    auto height_spinbox = main_widget.find_descendant_of_type_named<GUI::SpinBox>("height_spinbox");
    auto ok_button = main_widget.find_descendent_of_type_named<GUI::Button>("ok_button");
    auto cancel_button = main_widget.find_descendent_of_type_named<GUI::Button>("cancel_button");
    auto keep_aspect_ratio_checkbox = main_widget.find_descendent_of_type_named<GUI::CheckBox>("keep_aspect_ratio_checkbox");

    VERIFY(width_spinbox);
    VERIFY(height_spinbox);
    VERIFY(ok_button);
    VERIFY(cancel_button);
    VERIFY(keep_aspect_ratio_checkbox);

    width_spinbox->set_value(m_new_size.width());
    width_spinbox->on_change = [this](int value) {
        if (keep_aspect_ratio_checkbox)
    };

    height_spinbox->set_value(m_new_size.height());
    height_spinbox->on_change = [this](int value) {
    };

    ok_button->on_click = [this](auto) {
        // TODO - grab m_new_size from spinboxes
        done(ExecResult::ExecOK);
    };

    cancel_button->on_click = [this](auto) {
        done(ExecResult::ExecCancel);
    };
}

}
