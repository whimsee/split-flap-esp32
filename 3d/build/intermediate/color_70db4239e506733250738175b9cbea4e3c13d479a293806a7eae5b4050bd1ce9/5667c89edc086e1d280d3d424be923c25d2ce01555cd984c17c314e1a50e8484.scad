/*
   Copyright 2015-2021 Scott Bezek and the splitflap contributors

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

_is_projection_rendering = false;   // DO NOT MODIFY - this is set by the projection_renderer.py script

module projection_renderer(render_index = -1, render_etch = false, kerf_width = 0, panel_height, panel_horizontal, panel_vertical) {
    if (is_projection_rendering()) {
        echo(num_components=$children);
    }

    if (render_index == -1) {
        children();
    } else {
        translate([(panel_horizontal == 1 ? -kerf_width : 0), (panel_horizontal == 1 ? panel_height - kerf_width : 0) - panel_vertical * panel_height]) {
            rotate([0, 0, panel_horizontal == 1 ? 180 : 0]) {
                offset_size = (render_etch ? -kerf_width : kerf_width)/2;
                offset(delta=offset_size) {
                    projection() {
                        children(render_index);
                    }
                }
            }
        }
    }
}

function is_projection_rendering() = _is_projection_rendering;

        module color_selector(c) {
            precision = 0.0000001;  // arbitrary
            function compare_floats(x, y, i=0) = 
                  (len(x) != len(y)) ? false  // if arrays differ in length, they can't be equal
                : (i >= len(x)) ? true  // if we've hit the end of the arrays without issue, we're equal
                : (x[i] - precision <= y[i]) && x[i] + precision >= y[i]
                    ? compare_floats(x, y, i+1)
                    : false;  // not equal, short circuit

            if (c == [0.69975, 0.69975, 0.72] || compare_floats(c, [0.69975, 0.69975, 0.72]))
                children();
        }
                    