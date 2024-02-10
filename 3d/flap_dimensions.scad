/*
   Copyright 2018 Scott Bezek and the splitflap contributors

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

flap_width = 70.5;
flap_height = 26;
flap_thickness = 0.3; // 30 mil
flap_corner_radius = 3.1; // 2.88-3.48mm (used just for display)

flap_notch_height_auto = false;
flap_notch_height_default = 7.2;

flap_notch_height = (flap_notch_height_auto == true) ? sqrt(spool_outer_radius*spool_outer_radius - flap_pitch_radius*flap_pitch_radius) : flap_notch_height_default;

flap_notch_depth = 2;

flap_pin_width = 1.2;

