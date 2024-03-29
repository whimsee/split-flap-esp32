/*
   Copyright 2015-2018 Scott Bezek and the splitflap contributors

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

// M4 bolts
m4_hole_diameter = 4.5;
m4_bolt_length = 10;
m4_button_head_diameter = 7.6 + .2;
m4_button_head_length = 2.2 + .2;
m4_nut_width_flats = 7 + .2;
m4_nut_width_corners = 7/cos(180/6);
m4_nut_width_corners_padded = m4_nut_width_corners + .2;
m4_nut_length = 3.2;
m4_nut_length_padded = m4_nut_length + .2;


        module color_selector(c) {
            precision = 0.0000001;  // arbitrary
            function compare_floats(x, y, i=0) = 
                  (len(x) != len(y)) ? false  // if arrays differ in length, they can't be equal
                : (i >= len(x)) ? true  // if we've hit the end of the arrays without issue, we're equal
                : (x[i] - precision <= y[i]) && x[i] + precision >= y[i]
                    ? compare_floats(x, y, i+1)
                    : false;  // not equal, short circuit

            if (c == [0, 0.5, 0] || compare_floats(c, [0, 0.5, 0]))
                children();
        }
                    