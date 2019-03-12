#include "draw.h"
#include "color.h"

//File saves
int polygon_save_sides[1000];
int polygon_save_points[1000][1000];
int circle_save_points[1000][3];
int iter_polygon = 0;
int iter_circle = 0;

int tty_fd;

// filename
char *poly_file_name = "war_poly.txt";
char *circle_file_name = "war_circle.txt";
char *bitmap_file_name = "../image/anti.txt";
char *lines_file_name = "../image/lines_large.txt";
char *menu_file_name = "../image/main_menu.txt";

// menu
void handle_menu();
void handle_task_1();
void handle_task_2();
void handle_task_3();
void handle_task_4();
void handle_task_5();
void handle_task_6();
void handle_task_7();

// transformation
void handle_transformation();

int main(int argc, char** argv) {
    // Lock the screen from being re-rendered
    tty_fd = open("/dev/tty0", O_RDWR);
	ioctl(tty_fd,KDSETMODE,KD_GRAPHICS);

	int fb_fd = open("/dev/fb0",O_RDWR);

	//Get variable screen information
	ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo);
	vinfo.grayscale = 0;
	vinfo.bits_per_pixel = 32;
	ioctl(fb_fd, FBIOPUT_VSCREENINFO, &vinfo);
	ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo);

    // Get fixed screen information
	ioctl(fb_fd, FBIOGET_FSCREENINFO, &finfo);

    /* 
     * Calculate total size of the screen (in bytes) using got variable (vinfo and finfo)
     * We need to map exactly the right amount of memory and draw into that memory.
     * Failure to draw inside the memory may cause segfault on the machine.
     */
	long screensize = vinfo.yres_virtual * finfo.line_length;

    /* 
     * Map the buffer (according to screensize) to memory. 
     * fbp will contain a pointer to the beginning of the memory.
     */
	fbp = mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, (off_t)0);

    // Print menu screen
    handle_menu();
    
	return 0;
}

void handle_menu() {
    while(1) {
        clear_screen(COLOR_RED);
        print_bmp(menu_file_name, 0, 0, COLOR_YELLOW);
        char input = getchar();
        switch(input) {
            case '1':
                handle_task_1();
                break;
            case '2':
                handle_task_2();
                break;
            case '3':
                handle_task_3();
                break;
            case '4':
                // print transformation
                break;
            case '5':
                // print viewpoint
                break;
            case '6':
                // print flood fill
                break;
            case '7':
                // print uts
                break;
            case 'q':
                // exit
                ioctl(tty_fd,KDSETMODE,KD_TEXT);
                return;
        }
    }
}

void handle_task_1() {
    // print point
    clear_screen(COLOR_BLACK);
    print_bmp(bitmap_file_name, 0, 0, COLOR_RED);
    while (1) {
        char input = getchar();
        if (input == 'q') {
            return;
        }   
    }
}

void handle_task_2() {
    // print line
    clear_screen(COLOR_BLACK);
    print_file_line(lines_file_name, 0, 0);
    while (1) {
        char input = getchar();
        if (input == 'q') {
            return;
        }
    }
}

void handle_task_3() {
    // print polygon and circle
    clear_screen(COLOR_BLACK);
    print_file_polygon(poly_file_name, polygon_save_sides, polygon_save_points, &iter_polygon);
    print_file_circle(circle_file_name, circle_save_points, &iter_circle);
    while (1) {
        char input = getchar();
        if (input == 'q') {
            return;
        }
    }
}

void handle_transformation() {
    // Wait for input
    char input = getchar();
    while(1) {
        switch(input) {
            case '\033': // if the first value is esc
                getchar(); // skip the [
                switch(getchar()) { // the real value
                    case 'A':
                        // code for arrow up
                        translation(polygon_save_sides, polygon_save_points, circle_save_points, iter_polygon, iter_circle, 0, -10, pixel_color(255, 0, 0));
                        break;
                    case 'B':
                        // code for arrow down
                        translation(polygon_save_sides, polygon_save_points, circle_save_points, iter_polygon, iter_circle, 0, 10, pixel_color(255, 0, 0));
                        break;
                    case 'C':
                        // code for arrow right
                        translation(polygon_save_sides, polygon_save_points, circle_save_points, iter_polygon, iter_circle, 10, 0, pixel_color(255, 0, 0));
                        break;
                    case 'D':
                        // code for arrow left
                        translation(polygon_save_sides, polygon_save_points, circle_save_points, iter_polygon, iter_circle, -10, 0, pixel_color(255, 0, 0));
                        break;
                }
                break;
            case 'u': //scale up
                scale(polygon_save_sides, polygon_save_points, circle_save_points, iter_polygon, iter_circle, 1.2, pixel_color(255, 0, 0));
                break;
            case 'i': //scale down
                scale(polygon_save_sides, polygon_save_points, circle_save_points, iter_polygon, iter_circle, 0.8, pixel_color(255, 0, 0));
                break;
            case 'r': //rotate counter-clockwise
                rotate(polygon_save_sides, polygon_save_points, circle_save_points, iter_polygon, iter_circle, 0.04, pixel_color(255, 0, 0));
                break;
            case 't':
                rotate(polygon_save_sides, polygon_save_points, circle_save_points, iter_polygon, iter_circle, -0.04, pixel_color(255, 0, 0));
                break;
            case 'f':
                make_bullet(polygon_save_sides, polygon_save_points, circle_save_points, iter_polygon, &iter_circle, 400, 150, 12, pixel_color(255,0,0));
                while (getchar() != 'g') {
                    animate_bullet(polygon_save_sides, polygon_save_points, circle_save_points, iter_polygon, iter_circle, 400, 550, pixel_color(255,0,0));
                }
                iter_circle -= 1;
                break;
            case 'e':
                clear_screen(COLOR_BLACK);
                return;
        }
        input = getchar();
    }

}

