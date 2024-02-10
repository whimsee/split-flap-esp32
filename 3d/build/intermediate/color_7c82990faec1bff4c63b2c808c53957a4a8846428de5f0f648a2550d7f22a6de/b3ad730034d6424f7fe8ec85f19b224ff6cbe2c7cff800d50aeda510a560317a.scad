/*
   Copyright 2015-2016 Scott Bezek and the splitflap contributors

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

use <54ca8e2632ac4bf37d4619ea1fe5a258d7a02f50b2e2dead595fbe26abd60e3e.ttf>;

module roboto(text, size) {
    text(text=text, size=size, font="Roboto:style=Bold");
}
module text_label(lines) {
    text_height=2;
    module text_lines(lines, text_height, spacing=1.5) {
        for (i = [0 : len(lines)-1]) {
            translate([0, text_height * spacing * (len(lines)-1-i), 0]) {
                roboto(lines[i], text_height);
            }
        }
    }
    text_lines(lines, text_height);
}



        module color_selector(c) {
            precision = 0.0000001;  // arbitrary
            function compare_floats(x, y, i=0) = 
                  (len(x) != len(y)) ? false  // if arrays differ in length, they can't be equal
                : (i >= len(x)) ? true  // if we've hit the end of the arrays without issue, we're equal
                : (x[i] - precision <= y[i]) && x[i] + precision >= y[i]
                    ? compare_floats(x, y, i+1)
                    : false;  // not equal, short circuit

            if (c == [1, 1, 1] || compare_floats(c, [1, 1, 1]))
                children();
        }
                    