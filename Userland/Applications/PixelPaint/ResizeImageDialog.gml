@GUI::Widget {
    fill_with_background_color: true
    min_width: 260
    min_height: 200
    layout: @GUI::VerticalBoxLayout {
        margins: [4]
    }

    @GUI::GroupBox {
        title: "Size (px)"
        shrink_to_fit: true
        layout: @GUI::VerticalBoxLayout {
            margins: [4]
        }

        @GUI::Widget {
            layout: @GUI::HorizontalBoxLayout {}
            fixed_height: 24

            @GUI::Label {
                text: "Width:"
                fixed_width: 80
                text_alignment: "CenterRight"
            }

            @GUI::SpinBox {
                name: "width_spinbox"
                min: 1
                max: 16384
                min_width: 140
            }
        }

        @GUI::Widget {
            layout: @GUI::HorizontalBoxLayout {}
            fixed_height: 24

            @GUI::Label {
                text: "Height:"
                fixed_width: 80
                text_alignment: "CenterRight"
            }

            @GUI::SpinBox {
                name: "height_spinbox"
                min: 1
                max: 16384
                min_width: 140
            }
        }

        @GUI::Widget {
            layout: @GUI::HorizontalBoxLayout {}
            fixed_height: 24
            min_width: 140

            @GUI::Widget {
                fixed_width: 80
            }

            @GUI::CheckBox {
                name: "keep_aspect_ratio_checkbox"
                text: "Keep aspect ratio"
                checked: true
                autosize: true
            }
        }
    }

    @GUI::GroupBox {
        title: "Resampling"
        shrink_to_fit: true
        layout: @GUI::VerticalBoxLayout {
            margins: [4]
        }

        @GUI::RadioButton {
            name: "resample_nearest_neighbor_radio"
            text: "Nearest neighbor"
            checked: true
            autosize: true
        }

        @GUI::RadioButton {
            name: "resample_bilinear_radio"
            text: "Bilinear"
            autosize: true
        }
    }

    @GUI::Button {
        text: "OK"
        max_width: 75
    }
}
