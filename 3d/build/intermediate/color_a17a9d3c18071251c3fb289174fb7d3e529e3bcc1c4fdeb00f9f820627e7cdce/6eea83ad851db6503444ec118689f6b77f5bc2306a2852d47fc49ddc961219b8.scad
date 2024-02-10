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

// multiply two equal matricies by each element, limiting to a max of 1.0
function color_multiply(x, y) =
    [ for(j=[0:len(x) - 1]) min(x[j] * y[j], 1.0) ];

// inverts a color matrix by subtracting the input channel values from 1.0
function color_invert(x) =
    [ for(j=[0:len(x) - 1]) (1.0 - x[j]) ];

        module color_selector(c) {
            precision = 0.0000001;  // arbitrary
            function compare_floats(x, y, i=0) = 
                  (len(x) != len(y)) ? false  // if arrays differ in length, they can't be equal
                : (i >= len(x)) ? true  // if we've hit the end of the arrays without issue, we're equal
                : (x[i] - precision <= y[i]) && x[i] + precision >= y[i]
                    ? compare_floats(x, y, i+1)
                    : false;  // not equal, short circuit

            if (c == [0.75, 0.75, 0.8] || compare_floats(c, [0.75, 0.75, 0.8]))
                children();
        }
                    