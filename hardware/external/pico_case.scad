resolution = 20; //[10, 20, 30, 50, 100]
$fn = resolution;

show_frame = false; // [true, false]
show_pico = true; // [true, false]
show_lid = false; // [true, false]
show_button = false; // [true, false]

module rounded_plate(w, h, t, d) {
    translate([0, (d/2), 0])
    cube([w, (h-d), t]);
    translate([(d/2), 0, 0])
    cube([(w-d), h, t]);
    translate([(d/2), (d/2), 0])
    cylinder(h=t, d=d);
    translate([w-(d/2), (d/2), 0])
    cylinder(h=t, d=d);
    translate([(d/2), h-(d/2), 0])
    cylinder(h=t, d=d);
    translate([w-(d/2), h-(d/2), 0])
    cylinder(h=t, d=d);
}

module usb_port() {
    translate([0.3, 0, 0])
    color("grey")
    difference() {
        union() {
            cube([7.4, 6, 2.7]);
            translate([-0.3, 5.4, -0.1])
            cube([8, 0.6, 2.9]);
        }
    }
}

module switch() {
    color("grey")
    cube([3.5, 4.5, 1]);
    color("white")
    translate([(3.5/2), (4.5/2), 0])
    cylinder(h=1.5, d=3);
}

p_w = 21;
p_h = 51;
p_t = 1.0;
os = 0.35;
b = 2;

module pico() {
    translate([((p_w-8)/2), ((p_h+1.3)-6), p_t])
    usb_port();

    translate([5.25, 36.6, 0.6])
    switch();

    difference() {
        color("green")
        cube([p_w, p_h, 1]);

        translate([((p_w/2)-5.7), 2, -0.01])
        cylinder(h=p_t+0.02, d=2.1);
       
        translate([(p_w-((p_w/2)-5.7)), 2, -0.01])
        cylinder(h=p_t+0.02, d=2.1);

        translate([((p_w/2)-5.7), (p_h-2), -0.01])
        cylinder(h=p_t+0.02, d=2.1);

        translate([(p_w-((p_w/2)-5.7)), (p_h-2), -0.01])
        cylinder(h=p_t+0.02, d=2.1);
    }
}

if (show_pico) {
    translate([(os+b), (os+b), 3.4])
    pico();
}

if (show_frame) {
    union() {
        difference() {
            // Outer frame
            rounded_plate(p_w+(2*(os+b)), p_h+(2*(os+b)), 10, (2*(os+b)));
            
            // Board cut-out
            translate([b, b, -1])
            cube([p_w+(2*os), p_h+(2*os), 12]);
            
            // USB port cut-out
//            translate([b+os+((p_w-10)/2), b+os+((p_h+1)-6), 3.4])
//            cube([10, 8, 4.7]);
            translate([b+os+((p_w-10)/2), b+(2*os)+p_h-1, 8.1])
            rotate([-90, 0, 0])
            rounded_plate(10, 4.7, 4.7, 2);
            
            // Lid release cut-out
            difference() {
                union() {
                    translate([b+os+((p_w-10)/2), -1, 10-1])
                    cube([10, 8, 2]);
                    translate([b+os+((p_w-6)/2), -1, 10-1])
                    rotate([-90, 0, 0])
                    cylinder(h=4, d=2); 
                    translate([b+os+((p_w-6)/2), -1, 10-2])
                    cube([6, 8, 3]);
                    translate([b+os+((p_w+6)/2), -1, 10-1])
                    rotate([-90, 0, 0])
                    cylinder(h=4, d=2); 
                }
                translate([b+os+((p_w+10)/2), -1, 10-1])
                rotate([-90, 0, 0])
                cylinder(h=4, d=2); 
                translate([b+os+((p_w-10)/2), -1, 10-1])
                rotate([-90, 0, 0])
                cylinder(h=4, d=2); 
            }
            
            // Outer frame
            translate([1, 1, 10-1.2])
            rounded_plate(p_w+(2*(os+b))-2, p_h+(2*(os+b))-2, 2, (2*(os+b))-2);

            // Left top retainer cut-out
            translate([b+os+(p_w/2)-(3.2/2)-7.1, b+p_h+(2*os), 10-3+0.2])
            cube([3.2, 0.7, 1.0]);

            // Right top retainer cut-out
            translate([b+os+(p_w/2)-(3.2/2)+7.1, b+p_h+(2*os), 10-3+0.2])
            cube([3.2, 0.7, 1.0]);

            // Left bottom retainer cut-out
            translate([b+os+(p_w/2)-(4/2)-7.1, b+os-0.7, 10-3+0.2])
            cube([4, 0.7, 1.0]);

            // Right bottom retainer cut-out
            translate([b+os+(p_w/2)-(4/2)+7.1, b+os-0.7, 10-3+0.2])
            cube([4, 0.7, 1.0]);

            // Left retainer cut-out
            translate([b-0.7, b+os+(p_h/2)-2, 10-3+0.2])
            cube([0.7, 4, 1.0]);

            // Right retainer cut-out
            translate([b+(2*os)+p_w, b+os+(p_h/2)-2, 10-3+0.2])
            cube([0.7, 4, 1.0]);
        }
        
        // Left support rail
        translate([b+os+(p_w/2)-(3/2)-5.5, 0, 0])
        cube([3, p_h+(2*(os+b)), 3.4]);
       
        // Right support rail
        translate([b+os+(p_w/2)-(3/2)+5.5, 0, 0])
        cube([3, p_h+(2*(os+b)), 3.4]);

        // Left top retainer
        translate([b+os+(p_w/2)-(2.2/2)-6.1, b+p_h+(2*os)-0.6, 3.4+1+0.5])
        cube([2.2, 0.6, 0.8]);

        // Right top retainer
        translate([b+os+(p_w/2)-(2.2/2)+6.1, b+p_h+(2*os)-0.6, 3.4+1+0.5])
        cube([2.2, 0.6, 0.8]);

        // Left bottom retainer
        translate([b+os+(p_w/2)-(3/2)-5.7, b, 3.4+1+0.])
        cube([3, 0.6, 0.8]);

        // Right bottom retainer
        translate([b+os+(p_w/2)-(3/2)+5.7, b, 3.4+1+0.5])
        cube([3, 0.6, 0.8]);
    }
}

if (show_lid) {
    difference() {
        union() {
            translate([1+os, 1+os, 10-1.2])
            rounded_plate(p_w+(2*b)-2, p_h+(2*b)-2, 1, (2*b)-2);
            translate([(os+b), (os+b), 10-3])
            rounded_plate(p_w, p_h, 2, (2*b)-3);
            
            // Left top retainer
            translate([b+os+(p_w/2)-(2.2/2)-7.1, b+p_h+os, 10-3+0.4])
            cube([2.2, 0.6, 0.6]);

            // Right top retainer
            translate([b+os+(p_w/2)-(2.2/2)+7.1, b+p_h+os, 10-3+0.4])
            cube([2.2, 0.6, 0.6]);

            // Left bottom retainer
            translate([b+os+(p_w/2)-(3/2)-7.1, b+os-0.5, 10-3+0.3])
            cube([3, 0.6, 0.6]);

            // Right bottom retainer
            translate([b+os+(p_w/2)-(3/2)+7.1, b+os-0.5, 10-3+0.3])
            cube([3, 0.6, 0.6]);

            // Left retainer
            translate([b+os-0.6, b+os+(p_h/2)-1.5, 10-3+0.3])
            cube([0.6, 3, 0.6]);            

            // Right retainer
            translate([b+os+p_w, b+os+(p_h/2)-1.5, 10-3+0.3])
            cube([0.6, 3, 0.6]);            
        }

        // USB port cut-out
        translate([b+os+((p_w-10)/2), b+p_h+(2*os)-2, 10-4.2])
        cube([10, 8, 2.4]);

        // Lid release cut-out
        translate([b+os+((p_w-10)/2), -1, 10-4.2])
        cube([10, 8, 3]);
        
        // Switch cut-out
        translate([(os+b)+7, (os+b)+38.85, 0])
        cylinder(h=10, d=5);
        
        // Inset
        translate([(os+b)+1, (os+b)+1, 10-4])
        cube([p_w-2, p_h-2, 3]);
    }
    // Switch surround
    translate([(os+b), (os+b)+38.85-6, 10-2])
    cube([p_w, 1, 1]);    

    translate([(os+b), (os+b)+38.85+5, 10-2])
    cube([p_w, 1, 1]);    

}


if (show_button) {
    color("red")
    union() {
        translate([(os+b)+7, (os+b)+38.85, 7.5])
        cylinder(h=2.3, d=4.4);
        difference() {
            translate([(os+b)+7, (os+b)+38.85, 6.5])
            cylinder(h=2, d=7.6);
            translate([(os+b)+7, (os+b)+38.85, 6.5])
            cylinder(h=1, d=6.6);
        }
    }
}
