// Rows of widgets that wrap at the panel's edge: each item after a row's first
// goes beside the last one when it fits, else starts the next line, so a row
// never runs past the panel, whatever its width or the labels' lengths.
#pragma once

namespace loupe::window {

// Puts the next item, `width` pixels wide, beside the last item when it fits
// before the panel's right edge, else on the next line.
void flow(float width);
// How wide ImGui draws a button labelled `label`.
float button_width(const char* label);
// How wide ImGui draws a checkbox or radio button labelled `label`.
float check_width(const char* label);
// How wide ImGui draws `text` as plain text.
float text_width(const char* text);

} // namespace loupe::window
