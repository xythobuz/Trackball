/*
 * Trackball
 * Copyright 2022 Thomas Buck - thomas@xythobuz.de
 *
 * Required parts:
 *  - 1x Raspberry Pi Pico
 *  - 4x Cherry MX compatible switches and keycaps
 *  - 1x Billard ball, diameter 38mm 
 *  - 3x Si3N4 static bearing balls, diameter 3mm
 *  - 1x PMW3360 sensor with breakout board
 *  - 8x M2 screw, length 5mm
 *  - 8x M2 heat melt insert, length 4mm
 *
 * For the PMW3360 breakout board get this:
 * https://github.com/jfedor2/pmw3360-breakout
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * See <http://www.gnu.org/licenses/>.
 */

// https://www.thingiverse.com/thing:421524
use <external/cherry_mx.scad>

// https://www.printables.com/model/210898-raspberry-pi-pico-case
use <external/pico_case.scad>

// ######################
// ## Rendering Select ##
// ######################

//ball_and_roller();
//pico_wrap();
//sensor();
//mx_switch_cutout(wall);
//mx_switch_test();
//roller_mount_test();

//roller_holder();
//roller_mount_tri();

//trackball_top();
//trackball_bottom();

assembly();
//print();

// #######################
// #### Configuration ####
// #######################

$fn = 200;//42;

ball_dia = 38.0;
roller_dia = 3.0;

roller_ball_h = ball_dia / 2 - 5;
roller_count = 3;

wall = 3.0;
$c = 0.1;
$e = 0.01;

left_hand_version = false;

cut_roller_holder = false;
draw_supports = false;
draw_ball_roller = true;
draw_switches = true;
draw_sensor = true;
use_external_pico_model = true;

// #######################
// ## Raspberry Pi Pico ##
// #######################

pico_w = 21;
pico_l = 51;
pico_d = 1.0;
pico_hole_d = 2.1;
pico_hole_x = 4.8;
pico_hole_y = 2.0;
pico_hole_d_x = 11.4;
pico_hole_d_y = pico_l - 2 * pico_hole_y;
pico_usb_w = 8.0;
pico_usb_h = 2.8;
pico_usb_d = 6.0;
pico_usb_off = 1.3;

// ######################
// ### PMW3360 Sensor ###
// ######################

// https://github.com/jfedor2/pmw3360-breakout
sensor_w = 22;
sensor_l = 34;
sensor_pcb_h = 1.6;
sensor_hole_dia = 2.2;
sensor_hole_off_x = 3.0;
sensor_hole_off_y = 3.0;
sensor_hole_dist_x = 16.0;
sensor_hole_dist_y = 24.5;
sensor_cut_w = 8.0 + 0.5;
sensor_cut_h = 17.26;
sensor_cut_off_x = 7.0 - 0.25;
sensor_cut_off_y = 5.27;
sensor_cut_edge_to_pin1 = 2.75;
sensor_edge_to_pin1 = 1.52;

sensor_ball_to_lens_top = 2.4;
sensor_ball_to_chip_bottom = 9.81;

sensor_chip_w = 9.1;
sensor_chip_l = 16.2;
sensor_chip_h = 2.21;

sensor_pin_w = 0.5;
sensor_pin_h = 4.51;
sensor_pin_d = 0.2;
sensor_pin_dist = 10.7;
sensor_pin_off_top = 0.5;
sensor_pin_pitch = 0.89;

sensor_pin1_to_optical_center = 5.66;

sensor_lens_cutout_r = 2.0;
sensor_lens_cutout_w = 4.0;
sensor_lens_cutout_growth = 0.25;
sensor_lens_cutout_to_chip = 6.71 - 1.60;
sensor_lens_baseplate_h = 2.40;

sensor_lens_d = 19.0 + 1.0;
sensor_lens_w = 21.35 + 0.2;
sensor_lens_off = 10.97;

// ######################
// ## MX Switch Cutout ##
// ######################

// https://geekhack.org/index.php?topic=70654.0
mx_co_w = 14.0;
mx_co_w_add = 0.8;
mx_co_h = 14.0;
mx_co_h_off_1 = 1.0;
mx_co_h_off_2 = 3.5;
mx_co_h_off_3 = mx_co_h - 2 * (mx_co_h_off_1 + mx_co_h_off_2);
mx_co_r = 0.4;

// https://geekhack.org/index.php?topic=71550.0
mx_co_th = 1.5 - 0.1;
mx_co_b_add = 1.0;
mx_co_b_w = mx_co_w + mx_co_b_add;
mx_co_b_h = mx_co_h + mx_co_b_add;

mx_travel = 3.9;

// ######################
// ### Implementation ###
// ######################

base_dia = 62;

grub_screw_dia = 2.8;
grub_channel_dia = 4.0;

roller_thread_dia = roller_dia + 5.0;
roller_h = roller_dia + 7.0;
roller_ball_h_off = 0.4;
roller_ball_hold_off = 0.5;
roller_thread_hole = roller_dia - 1;
roller_small_hole = sphere_r_at_h(roller_ball_hold_off, roller_dia / 2) * 2;

roller_ridge_h = 1.5;
roller_mount_angle_off = 90;
roller_mount_dia = roller_thread_dia + 2.0;

ball_h = 15; // todo

switch_test_w = 25;

roller_mount_holder_gap = 0.8;
sensor_pcb_mount_gap = 2.0;

sensor_pcb_support_h = 1.6 + 3.4;
//         rotate                                     translate
sw = [
        [ [-5,0, -30 * (left_hand_version ? -1 : 1)], [0,-base_dia / 2 - 0,-17]],
        [ [-5,0,  15 * (left_hand_version ? -1 : 1)], [0,-base_dia / 2 - 0,-17]],
        [ [-5,0, 125 * (left_hand_version ? -1 : 1)], [0,-base_dia / 2 - 0,-17]],
        [ [-5,0,-125 * (left_hand_version ? -1 : 1)], [0,-base_dia / 2 - 0,-17]],
    ];

sw_mount_w = mx_co_w + 7;
sw_mount_co_l = 10;

bottom_base_wall = wall + 0.5;
bottom_base_below_zero = bottom_base_wall + 4.5;

pico_co_w = pico_w + 1;
pico_co_l = pico_l + 1;

reset_button_dia = 4.0;
reset_button_off_x = 7;
reset_button_off_y = 12.15;

pico_support_w = 6.5;
pico_support_l = 5;

pico_screw_depth = 6;
pico_screw_d = 1.8;

usb_cutout_grow_l = 10;
usb_cutout_grow_x = 20;
usb_cutout_grow_y = 40;
usb_cutout_w_add = 1;
usb_cutout_h_add = 0.6;

assembly_dist = 20;

roller_holder_sider_cut = 1.5;
roller_holder_h_compensation = -0.1;

bottom_add_wall = 4;

screw_dia = 3.2;
screw_off = base_dia / 2 - 10;
screw_head_d = 6.0;
screw_head_h = 3.5;
screw_angles = [ 15, -15, 180 + 15, 180 - 15 ];
screw_insert_dia = 4.8;
screw_insert_h = 6.0;

function sphere_r_at_h(h, r) = r * sin(acos(h / r));
function sphere_angle_at_rh(h, r) = acos(h / r);

module mx_switch_cutout(h) {
    translate([-mx_co_w / 2 - mx_co_w_add, -mx_co_h / 2, 0]) {
        linear_extrude(h + 1) {
            translate([mx_co_w_add, 0]) {
                square([mx_co_w, mx_co_h]);
                
                for (x = [mx_co_r / 2, mx_co_w - mx_co_r / 2])
                for (y = [mx_co_r / 2, mx_co_h - mx_co_r / 2])
                translate([x, y])
                circle(r = mx_co_r);
            }
            
            for (x = [0, mx_co_w + mx_co_w_add])
            for (y = [0, mx_co_h_off_2 + mx_co_h_off_3])
            translate([x, mx_co_h_off_1 + y, 0])
            square([mx_co_w_add, mx_co_h_off_2]);
        }
        
        translate([mx_co_w_add - mx_co_b_add / 2, -mx_co_b_add / 2, -1])
        cube([mx_co_b_w, mx_co_b_h, h - mx_co_th + 1]);
    }
}

module mx_switch_test() {
    difference() {
        translate([-switch_test_w / 2, -switch_test_w / 2, 0])
        cube([switch_test_w, switch_test_w, wall]);
        
        mx_switch_cutout(wall);
        
        translate([0, -switch_test_w / 2 + 1, wall - 1.0])
        linear_extrude(1.1)
        text("switch test", size = 3, halign = "center");
    }
    
    %translate([0, 0, wall])
    rotate([0, 0, 180])
    mx_switch($t);
}

module pico_own() {
    translate([-pico_w / 2, -pico_l / 2, 0])
    difference() {
        union() {
            color("green")
            cube([pico_w, pico_l, pico_d]);
            
            translate([(pico_w - pico_usb_w) / 2, pico_l - pico_usb_d + pico_usb_off, pico_d])
            cube([pico_usb_w, pico_usb_d, pico_usb_h]);
        }
        
        for (x = [0, pico_hole_d_x])
        for (y = [0, pico_hole_d_y])
        translate([pico_hole_x + x, pico_hole_y + y, -1])
        cylinder(d = pico_hole_d, h = pico_d + 2);
    }
}

module pico_wrap() {
    //if (use_external_pico_model)
    translate([-pico_w / 2, -pico_l / 2, 0])
    pico();
    //else
    pico_own();
}

module sensor_lens_cutout_intern() {
    cylinder(d = sensor_lens_cutout_r * 2, h = $e);
    
    translate([-sensor_lens_cutout_r, 0, 0])
    cube([sensor_lens_cutout_r * 2, sensor_lens_cutout_w, $e]);
}

module rounded_cube(x, y, z, r) {
    hull()
    for (tx = [r, x - r])
    for (ty = [r, y - r])
    translate([tx, ty, 0])
    cylinder(d = r * 2, h = z);
}

module sensor_lens_cutout() {
    translate([0, 0, sensor_lens_cutout_to_chip])
    hull() {
        translate([0, 0, sensor_lens_baseplate_h - $e])
        sensor_lens_cutout_intern();
        
        scale(1 + sensor_lens_cutout_growth * sensor_lens_baseplate_h)
        sensor_lens_cutout_intern();
    }
    
    translate([-sensor_lens_d / 2, -sensor_lens_w + sensor_lens_off, 0])
    rounded_cube(sensor_lens_d, sensor_lens_w, sensor_lens_cutout_to_chip, 6);
    
    translate([-3 / 2, -sensor_lens_w + sensor_lens_off - 0.5, 0])
    cube([3, 0.5, sensor_lens_cutout_to_chip]);
}

module sensor() {
    translate([-sensor_w / 2, -sensor_l / 2, 0])
    difference() {
        color("green")
        cube([sensor_w, sensor_l, sensor_pcb_h]);
        
        translate([sensor_cut_off_x, sensor_cut_off_y, -1])
        cube([sensor_cut_w, sensor_cut_h, sensor_pcb_h + 2]);
        
        for (x = [0, sensor_hole_dist_x])
        for (y = [0, sensor_hole_dist_y])
        translate([sensor_hole_off_x + x, sensor_hole_off_y + y, -1])
        cylinder(d = sensor_hole_dia, h = sensor_pcb_h + 2);
    }
    
    color("#303030")
    translate([-sensor_chip_w / 2, -sensor_l / 2 - sensor_chip_l + sensor_edge_to_pin1 + sensor_cut_off_y + sensor_cut_h - sensor_cut_edge_to_pin1, -sensor_chip_h])
    cube([sensor_chip_w, sensor_chip_l, sensor_chip_h]);
    
    translate([0, -sensor_l / 2 - 15 * sensor_pin_pitch + sensor_cut_off_y + sensor_cut_h - sensor_cut_edge_to_pin1, 0])
    for (p = [0 : 15])
    translate([0, p * sensor_pin_pitch, 0])
    for (x = [-sensor_pin_dist / 2, sensor_pin_dist / 2])
    if (((p % 2 == 0) && (x < 0))
            || ((p % 2 == 1) && (x > 0)))
    translate([-sensor_pin_d / 2 + x, -sensor_pin_w / 2, -sensor_chip_h + sensor_pin_off_top])
    cube([sensor_pin_d, sensor_pin_w, sensor_pin_h]);
    
    translate([0, -sensor_l / 2 + sensor_cut_off_y + sensor_cut_h - sensor_cut_edge_to_pin1 - sensor_pin1_to_optical_center, 0]) {
        color("cyan")
        translate([0, 0, -sensor_chip_h + 1])
        cylinder(d = 0.2, h = sensor_ball_to_chip_bottom - 1);
        
        %color("blue")
        sensor_lens_cutout();
    }
}
module ball_and_roller() {
    color("red")
    sphere(d = ball_dia, $fn = $fn * 2);
    
    for (r = [0 : roller_count - 1])
    rotate([0, 0, roller_mount_angle_off + 360 / roller_count * r])
    translate([sphere_r_at_h(roller_ball_h - ball_dia / 2, ball_dia / 2), 0, -ball_dia / 2 + roller_ball_h])
    rotate([0, 180 + sphere_angle_at_rh(roller_ball_h - ball_dia / 2, ball_dia / 2), 0])
    translate([0, 0, -roller_dia / 2])
    roller_holder();
}

module roller_holder() {
    translate([0, 0, -roller_h + roller_dia / 2])
    difference() {
        color("magenta")
        union() {
            translate([0, 0,  roller_h-roller_dia/2 + roller_ball_h_off-3])
            cylinder(d1 = roller_mount_dia, d2=roller_dia+1,  h = 3);
            
            cylinder(d = roller_mount_dia, h = roller_h-roller_dia/2 + roller_ball_h_off-3);
        }
        
        translate([-roller_mount_dia / 2 - 1, roller_mount_dia / 2 - roller_holder_sider_cut, -1])
        cube([roller_mount_dia + 2, roller_mount_dia / 2 + 1, roller_h + 2]);
        
        translate([0, 0, -$e])
        cylinder(d = roller_thread_hole, h = $e+ roller_h - roller_dia / 2 + roller_ball_h_off + roller_ball_hold_off);
    
        translate([0, 0, roller_h - roller_dia / 2 + roller_holder_h_compensation])
        sphere(d = roller_dia, $fn = $fn * 2);

        if (cut_roller_holder)
        translate([-roller_thread_dia / 2 - 1, -roller_thread_dia, -1])
        cube([roller_thread_dia + 2, roller_thread_dia, roller_h + 2]);
    }
    
    %color("blue")
    sphere(d = roller_dia, $fn = $fn * 2);
}

module roller_mount() {
    translate([0, 0, -1-roller_h + roller_dia / 2]) {
        difference() {
            cylinder(d=roller_mount_dia+wall,h=roller_h/2);
            
            translate([0, 0, 1])
            cylinder(d=roller_mount_dia+$c*2,h=roller_h/2+$e);

            if (cut_roller_holder)
            translate([-roller_thread_dia / 2 - 1, -roller_thread_dia, -1])
            cube([roller_thread_dia + 2, roller_thread_dia, roller_h + 2]);
        }
    }
}

module roller_mount_test() {
    roller_holder();
    roller_mount();
}

module roller_mount_tri_hull() {
    for (r = [0 : roller_count - 1])
    rotate([0, 0, roller_mount_angle_off + 360 / roller_count * r])
    translate([sphere_r_at_h(roller_ball_h - ball_dia / 2, ball_dia / 2), 0, -ball_dia / 2 + roller_ball_h])
    rotate([0, 180 + sphere_angle_at_rh(roller_ball_h - ball_dia / 2, ball_dia / 2), 0])
    translate([0, 0, -roller_h])
    cylinder(d = roller_mount_dia + wall + 1, h = roller_h - 3);
    
    translate([0, 0, -ball_dia / 2 - 11])
    cylinder(d = base_dia, h = $e);
}

module roller_mount_tri_body() {
    // space for roller holder
    for (r = [0 : roller_count - 1])
    rotate([0, 0, roller_mount_angle_off + 360 / roller_count * r])
    translate([sphere_r_at_h(roller_ball_h - ball_dia / 2, ball_dia / 2), 0, -ball_dia / 2 + roller_ball_h])
    rotate([0, 180 + sphere_angle_at_rh(roller_ball_h - ball_dia / 2, ball_dia / 2), 0])
    translate([0, 0, -roller_h])
    cylinder(d = roller_mount_dia + roller_mount_holder_gap, h = ball_dia / 2 + roller_h);
    
    // room for ball itself
    sphere($fn = $fn * 2, d = ball_dia + $c * 2 + 4);
        
    // grub screws
    for (r = [0 : roller_count - 1])
    rotate([0, 0, roller_mount_angle_off + 360 / roller_count * r])
    translate([sphere_r_at_h(roller_ball_h - ball_dia / 2, ball_dia / 2), 0, -ball_dia / 2 + roller_ball_h])
    rotate([0, 180 + sphere_angle_at_rh(roller_ball_h - ball_dia / 2, ball_dia / 2), 0])
    translate([0, 0, -roller_h/2])
    rotate([0,-90,0])
    translate([-2, 0, 2]) {
        cylinder(d = grub_screw_dia, h = ball_dia);
        
        translate([0, 0, roller_mount_dia / 4 + wall])
        cylinder(d = grub_channel_dia, h = ball_dia);
    }
        
    // sensor lens
    translate([0, 0, -ball_dia / 2 - ball_h])
    translate([0, sensor_l / 2 - sensor_cut_off_y - sensor_cut_h + sensor_cut_edge_to_pin1 + sensor_pin1_to_optical_center, ball_h + sensor_chip_h - sensor_ball_to_chip_bottom])
    translate([0, -sensor_l / 2 + sensor_cut_off_y + sensor_cut_h - sensor_cut_edge_to_pin1 - sensor_pin1_to_optical_center, 0])
    sensor_lens_cutout();
    
    // sensor pcb
    translate([-1, -1, -ball_dia / 2 - ball_h])
    translate([0, sensor_l / 2 - sensor_cut_off_y - sensor_cut_h + sensor_cut_edge_to_pin1 + sensor_pin1_to_optical_center, ball_h + sensor_chip_h - sensor_ball_to_chip_bottom])
    translate([-sensor_w / 2, -sensor_l / 2, -10])
    cube([sensor_w + 2, sensor_l + 2, sensor_pcb_h + 10 + sensor_pcb_mount_gap]);
}

module roller_mount_sensor_pcb_support() {
    translate([-sensor_w / 2, -sensor_l / 2, sensor_pcb_h])
    translate([0, 0, -ball_dia / 2 - ball_h])
    translate([0, sensor_l / 2 - sensor_cut_off_y - sensor_cut_h + sensor_cut_edge_to_pin1 + sensor_pin1_to_optical_center, ball_h + sensor_chip_h - sensor_ball_to_chip_bottom])
    for (x = [0, sensor_hole_dist_x])
    for (y = [0, sensor_hole_dist_y])
    translate([sensor_hole_off_x + x, sensor_hole_off_y + y, 0])
    difference() {
        union() {
            color("magenta")
            cylinder(d = sensor_hole_dia + 1.5, h = sensor_pcb_mount_gap);
            
            if (draw_supports)
            color("black")
            translate([0, 0, -sensor_pcb_support_h])
            cylinder(d = sensor_hole_dia + 0.5, h = sensor_pcb_support_h);
        }
        
        cylinder(d = sensor_hole_dia - 0.2, h = sensor_pcb_mount_gap + 1);
    }
    
    if (draw_supports)
    color("black")
    for (x = [-5, 0, 5])
    for (y = [-8, 0, 6.5])
    if (((x == 0) && (y != 0)) || ((x != 0) && (y == 0)))
    translate([x, y + 2, -30])
    cylinder(d = sensor_hole_dia + 0.5, h = 8.5);

    if (draw_sensor)
    %translate([0, 0, -ball_dia / 2 - ball_h])
    translate([0, sensor_l / 2 - sensor_cut_off_y - sensor_cut_h + sensor_cut_edge_to_pin1 + sensor_pin1_to_optical_center, ball_h + sensor_chip_h - sensor_ball_to_chip_bottom])
    sensor();
}

// TODO holes for pcb screws not going into body!!

module roller_mount_tri() {
    if (draw_ball_roller)
    %ball_and_roller();
    
    difference() {
        hull()
        roller_mount_tri_hull();
        
        
        roller_mount_tri_body();
        
        // TODO test cable cutout
        translate([-6, 0, -30.1])
        cube([12, 50, 2]);
        
        if (cut_roller_holder)
        translate([0, -base_dia / 2 - 1, -40])
        cube([base_dia / 2 + 1, base_dia + 2, 40]);
    }
    
    roller_mount_sensor_pcb_support();
}

module trackball_top() {
    translate([0, 0, ball_dia / 2 + ball_h]) {
        if (draw_ball_roller)
        %ball_and_roller();
        
        difference() {
            color("orange")
            hull() {
                roller_mount_tri_hull();
                for ( i = [0:len(sw)-1] ){
                    rotate([0,sw[i][0][1],sw[i][0][2]])
                    translate(sw[i][1])
                    rotate([90+sw[i][0][0],0,0])
                    translate([0, 0, -0.5])
                    cube([sw_mount_w, sw_mount_w, 1], center = true);
                }
            }
            
            roller_mount_tri_body();
        
            if (cut_roller_holder)
            translate([0, -base_dia / 2 - 1, -40])
            cube([base_dia / 2 + 1, base_dia + 2, 40]);
            
            
            for ( i = [0:len(sw)-1] ){
                rotate([0,sw[i][0][1],sw[i][0][2]])
                translate(sw[i][1])
                rotate([90+sw[i][0][0],0,0])
                translate([0, 0, -sw_mount_co_l]) {
                    mx_switch_cutout(sw_mount_co_l + 1);
                    
                    translate([0, 0, 2])
                    rotate([90, 0, 0])
                    cylinder(d = 4, h = 20);
                }
            }
            
        
            for (r = screw_angles)
            rotate([0, 0, r])
            translate([screw_off, 0, -ball_dia / 2 - 11 -1]) {
                cylinder(d = screw_insert_dia, h = screw_insert_h + 1);
            }
        }
        
        roller_mount_sensor_pcb_support();
    
        if (draw_switches)
        for ( i = [0:len(sw)-1] ){
            rotate([0,sw[i][0][1],sw[i][0][2]])
            translate(sw[i][1])
            rotate([90+sw[i][0][0],0,0])
                translate([0, 0, 1.])
            mx_switch($t);
        }
    }
}

module trackball_bottom_wrap() {
    %rotate([0, 180, 0])
    pico_wrap();
    
    color("magenta")
    translate([0, 0, -bottom_base_below_zero])
    difference() {
        cylinder(d = base_dia, h = bottom_base_below_zero + ball_h - 11);
        
        translate([0, 0, bottom_base_wall])
        cylinder(d = base_dia - bottom_base_wall * 2 - bottom_add_wall, h = bottom_base_below_zero + ball_h - 11);
        
        translate([-pico_co_w / 2, -pico_co_l / 2, bottom_base_wall])
        cube([pico_co_w, pico_co_l, bottom_base_below_zero + ball_h - 11]);
        
        translate([pico_w / 2 - reset_button_off_x, pico_l / 2 - reset_button_off_y, -1])
        cylinder(d = reset_button_dia, h = bottom_base_wall + 2);
        
        if (cut_roller_holder)
        translate([-base_dia / 2 - 1, -base_dia / 2 - 1, -10])
        cube([base_dia / 2 + 1, base_dia + 2, 40]);
    }
    
    color("cyan")
    for (x = [-1, 1])
    for (y = [-1, 1])
    translate([x * (pico_co_w - pico_support_w) / 2, y * (pico_co_l - pico_support_l) / 2, 0])
    translate([-pico_support_w / 2, -pico_support_l / 2, -bottom_base_below_zero + bottom_base_wall])
    cube([pico_support_w, pico_support_l, bottom_base_below_zero - bottom_base_wall - pico_d]);
        
    color("cyan")
    for (r = screw_angles)
    rotate([0, 0, r])
    translate([screw_off, 0, -bottom_base_below_zero + bottom_base_wall])
    cylinder(d = screw_head_d + 4, h = bottom_base_below_zero + ball_h - 11 - bottom_base_wall);
}

module usb_cutout() {
    hull() {
        translate([-usb_cutout_w_add / 2, 0, -usb_cutout_h_add / 2])
        cube([pico_usb_w + usb_cutout_w_add, 1, pico_usb_h + usb_cutout_h_add]);
        
        translate([-usb_cutout_grow_x / 2, usb_cutout_grow_l, -usb_cutout_grow_y / 2])
        cube([pico_usb_w + usb_cutout_grow_x, 1, pico_usb_h + usb_cutout_grow_y]);
    }
}

module trackball_bottom() {
    difference() {
        trackball_bottom_wrap();
        
        for (x = [0, pico_hole_d_x])
        for (y = [0, pico_hole_d_y])
        translate([-pico_w / 2, -pico_l / 2, 0])
        translate([pico_hole_x + x, pico_hole_y + y, -pico_d - pico_screw_depth])
        cylinder(d = pico_screw_d, h = pico_d + pico_screw_depth + 1);
        
        rotate([0, 180, 0])
        translate([-pico_w / 2, -pico_l / 2, 0])
        translate([(pico_w - pico_usb_w) / 2, pico_l - 1 + pico_usb_off, pico_d])
        usb_cutout();
        
        for (r = screw_angles)
        rotate([0, 0, r])
        translate([screw_off, 0, -bottom_base_below_zero - 1]) {
            cylinder(d = screw_dia, h = bottom_base_below_zero + 30);
            cylinder(d = screw_head_d, h = screw_head_h + 1);
        }
    }
}

module assembly() {
    translate([0, 0, assembly_dist / 2])
    trackball_top();
    
    translate([0, 0, -assembly_dist / 2])
    trackball_bottom();
}

module print() {
    translate([-40, 0, -4])
    trackball_top();
    
    translate([40, 0, bottom_base_below_zero])
    trackball_bottom();
    
    for (y = [-20, 0, 20])
    translate([0, y, 8.5])
    roller_holder();
}
