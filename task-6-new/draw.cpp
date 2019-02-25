#include <linux/fb.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/kd.h>
#include <math.h>
#include <stdio.h>
#include <queue>
#include <utility>

#include "draw.hpp"
#include "draw_util.hpp"

using namespace std;

int tty_fd;
uint8_t *fbp;
struct fb_fix_screeninfo finfo;
struct fb_var_screeninfo vinfo;

extern Object viewport;
extern int x_max;
extern int y_max;
extern int x_min;
extern int y_min;


// Defining region codes 
int INSIDE = 0; // 0000 
int LEFT = 1;   // 0001 
int RIGHT = 2;  // 0010 
int BOTTOM = 4; // 0100 
int TOP = 8;    // 1000 
  
// Defining x_max, y_max and x_min, y_min for 
// clipping rectangle. Since diagonal points are 
// enough to define a rectangle 
// int x_max = viewport->drawing.primitives->values[6]; 
// int y_max = viewport->drawing.primitives->values[5];  
// int x_min = viewport->drawing.primitives->values[0]; 
// int y_min = viewport->drawing.primitives->values[0];


// Function to compute region code for a point(x, y) 
int compute_region_code(double x, double y) 
{ 
    // initialized as being inside  
    int code = INSIDE; 
  
    if (x < x_min)       // to the left of rectangle 
        code |= LEFT; 
    else if (x > x_max)  // to the right of rectangle 
        code |= RIGHT; 
    if (y < y_min)       // below the rectangle 
        code |= BOTTOM; 
    else if (y > y_max)  // above the rectangle 
        code |= TOP; 
  
    return code; 
} 
  
// Implementing Cohen-Sutherland algorithm 
// Clipping a line from P1 = (x2, y2) to P2 = (x2, y2) 
// Code taken from source: https://www.geeksforgeeks.org/line-clipping-set-1-cohen-sutherland-algorithm/
// With a little modification
void cohen_sutherland_clip(double x1, double y1, double x2, double y2, uint32_t color) 
{ 
    // Compute region codes for P1, P2 
    int code1 = compute_region_code(x1, y1); 
    int code2 = compute_region_code(x2, y2); 
  
    // Initialize line as outside the rectangular window 
    int accept = 0; 
  
    while (1) 
    { 
        if ((code1 == 0) && (code2 == 0)) 
        { 
            // If both endpoints lie within rectangle 
            accept = 1; 
            break; 
        } 
        else if (code1 & code2) 
        { 
            // If both endpoints are outside rectangle, 
            // in same region 
            break; 
        } 
        else
        { 
            // Some segment of line lies within the 
            // rectangle 
            int code_out; 
            double x, y; 
  
            // At least one endpoint is outside the  
            // rectangle, pick it. 
            if (code1 != 0) 
                code_out = code1; 
            else
                code_out = code2; 
  
            // Find intersection point; 
            // using formulas y = y1 + slope * (x - x1), 
            // x = x1 + (1 / slope) * (y - y1) 
            if (code_out & TOP) 
            { 
                // point is above the clip rectangle 
                x = x1 + (x2 - x1) * (y_max - y1) / (y2 - y1); 
                y = y_max; 
            } 
            else if (code_out & BOTTOM) 
            { 
                // point is below the rectangle 
                x = x1 + (x2 - x1) * (y_min - y1) / (y2 - y1); 
                y = y_min; 
            } 
            else if (code_out & RIGHT) 
            { 
                // point is to the right of rectangle 
                y = y1 + (y2 - y1) * (x_max - x1) / (x2 - x1); 
                x = x_max; 
            } 
            else if (code_out & LEFT) 
            { 
                // point is to the left of rectangle 
                y = y1 + (y2 - y1) * (x_min - x1) / (x2 - x1); 
                x = x_min; 
            } 
  
            // Now intersection point x,y is found 
            // We replace point outside rectangle 
            // by intersection point 
            if (code_out == code1) 
            { 
                x1 = x; 
                y1 = y; 
                code1 = compute_region_code(x1, y1); 
            } 
            else
            { 
                x2 = x; 
                y2 = y; 
                code2 = compute_region_code(x2, y2); 
            } 
        } 
    } 
    if (accept) 
    { 
        draw_line(x1, y1, x2, y2, color);
    }
} 

uint32_t get_color(primitive_color pcolor)
{
    uint32_t color;
    switch(pcolor){
        case border_color:
            color = get_pixel_color(1,1,1);
            break;
    	case white: // white 
    		color = get_pixel_color(255, 255, 255);
    		break;
    	case vermillion_red: // Vermillion Red
    		color = get_pixel_color(202, 1, 35);
    		break;
    	case bright_green: // Bright Green
    		color = get_pixel_color(161, 255, 89);
    		break;
    	case blue: // Blue
    		color = get_pixel_color(80, 125, 254);
    		break;
    	case champions_purple: // Champions Purple
    		color = get_pixel_color(127, 72, 195);
    		break;
    	case yellow: // Yellow
    		color = get_pixel_color(255, 255, 0);
    		break;
    	case brown: // Brown
    		color = get_pixel_color(90, 48, 24);
    		break;
    	case orange: // Orange
    		color = get_pixel_color(247, 157, 0);
    		break;
    	case plushy_shake: // Plushy Shake / Pink
    		color = get_pixel_color(255, 193, 220);
    		break;
    	case teal: // Teal
    		color = get_pixel_color(0, 128, 128);
    		break;
    	case midas_gold: // Midas Gold
    		color = get_pixel_color(255, 202, 21);
    		break;
    	case pristine_platinum: // Pristine Platinum / Silver
    		color = get_pixel_color(213, 227, 245);
    		break;
    	case rubiline: // Rubiline / Dark Pink
    		color = get_pixel_color(209, 31, 161);
    		break;
    	case glacial_flow: // Glacial Flow / Bright Blue
    		color = get_pixel_color(50, 171, 220);
    		break;
    	default: // Black
    		color = get_pixel_color(0, 0, 0);
    		break;
    }
    return color;
}

int initialize()
{
    // Lock the screen from being re-rendered
    tty_fd = open("/dev/tty0", O_RDWR);
    ioctl(tty_fd, KDSETMODE, KD_GRAPHICS);

    // Open framebuffer interface
    int fb_fd = open("/dev/fb0", O_RDWR);

    // Get variable screen information
    ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo);

    // Get fixed screen information
    ioctl(fb_fd, FBIOGET_FSCREENINFO, &finfo);

    /* 
     * Calculate total size of the screen (in bytes) using got variable (vinfo and finfo).
     * We need to map exactly the right amount of memory and draw into that memory.
     * Failure to draw inside the memory may cause segfault on the machine.
     */
    long screensize = vinfo.yres_virtual * finfo.line_length;

    /* 
     * Map the buffer (according to screensize) to memory. 
     * fbp will contain a pointer to the beginning of the memory.
     */
    fbp = (uint8_t *) mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, (off_t)0);
}

int finalize()
{
    // Unlock the screen from being re-rendered
    ioctl(tty_fd, KDSETMODE, KD_TEXT);
}

uint32_t get_pixel_color(uint8_t r, uint8_t g, uint8_t b)
{
    return (r << vinfo.red.offset) | (g << vinfo.green.offset) | (b << vinfo.blue.offset);
}

/**
 * Draw a pixel of color on position (x, y) with (0, 0) at the top left of the screen
 */
void draw_pixel(int x, int y, uint32_t color)
{
    // Calculate the correct location in the mapped memory of the pixel that we want to set
    long location = (x + vinfo.xoffset) * vinfo.bits_per_pixel / 8 + (y + vinfo.yoffset) * finfo.line_length;
    // Set the memory to the value of the pixel in the color
    *((uint32_t *)(fbp + location)) = color;
}

/**
 * Render whole screen to be 'color'
 */
void clear_screen(uint32_t color)
{
    for (int x = 0; x < vinfo.xres; ++x)
        for (int y = 0; y < vinfo.yres; ++y)
        	draw_pixel(x, y, color);
}

/**
 * Draw a pixel of color on position (x, y) with (0, 0) at the top left of the screen
 * Never draw outside of buffer
 */
void draw_point(int x, int y, uint32_t color)
{
    if (x >= vinfo.xres || y >= vinfo.yres || x < 0 || y < 0)
        return;
    draw_pixel(x, y, color);
}

/**
 * Draw list of pixel location on the screen in 'color'
 * CASE: abs(y1 - y0) >= abs(x1 - x0) and y1 >= y0
 */
void plot_line_high(int x0, int y0, int x1, int y1, uint32_t color)
{
    int dx = x1 - x0;
    int dy = y1 - y0;
    int xi = 1;
    if (dx < 0) {
        xi = -1;
        dx = -dx;
    }
    int D = 2 * dx - dy;
    int x = x0;

    for (int y = y0; y <= y1; ++y)
    {
        draw_point(x, y, color);
        if (D > 0)
        {
            x += xi;
            D -= 2 * dy;
        }
        D += 2 * dx;
    }
}

/**
 * Draw list of pixel location on the screen in 'color'
 * CASE: abs(y1 - y0) < abs(x1 - x0) and x1 >= x0
 */
void plot_line_low(int x0, int y0, int x1, int y1, uint32_t color)
{
    int dx = x1 - x0;
    int dy = y1 - y0;
    int yi = 1;
    if (dy < 0) {
        yi = -1;
        dy = -dy;
    }
    int D = 2 * dy - dx;
    int y = y0;

    for (int x = x0; x <= x1; ++x)
    {
        draw_point(x, y, color);
        if (D > 0)
        {
            y += yi;
            D -= 2 * dx;
        }
        D += 2 * dy;
    }
}

/**
 * Draw list of pixel location on the screen in 'color'
 * CASE: x0 == x1 and y0 >= y1
 */
void plot_line_straight_vertical(int x0, int y0, int x1, int y1, uint32_t color)
{
    for (int y = y0; y <= y1; ++y)
    {
        draw_point(x0, y, color);
    }
}

/**
 * Draw list of pixel location on the screen in 'color'
 * CASE: y0 == y1 and x0 >= x1
 */
void plot_line_straight_horizontal(int x0, int y0, int x1, int y1, uint32_t color)
{
    for (int x = x0; x <= x1; ++x)
    {
        draw_point(x, y0, color);
    }
}

/**
 * Draw list of pixel location on the screen in 'color'
 */
void draw_line(int x0, int y0, int x1, int y1, uint32_t color)
{
    if (x0 == x1)
    {
        if (y0 > y1)
        {
            plot_line_straight_vertical(x1, y1, x0, y0, color);
        }
        else
        {
            plot_line_straight_vertical(x0, y0, x1, y1, color);
        }
    }
    else if (y0 == y1)
    {
        if (x0 > x1)
        {
            plot_line_straight_horizontal(x1, y1, x0, y0, color);
        }
        else
        {
            plot_line_straight_horizontal(x0, y0, x1, y1, color);
        }
    }
    else
    {
        if (abs(y1 - y0) < abs(x1 - x0))
        {
            if (x0 > x1)
            {
                plot_line_low(x1, y1, x0, y0, color);
            }
            else
            {
                plot_line_low(x0, y0, x1, y1, color);
            }
        }
        else
        {
            if (y0 > y1)
            {
                plot_line_high(x1, y1, x0, y0, color);
            }
            else
            {
                plot_line_high(x0, y0, x1, y1, color);
            }
        }
    }
}

/**
 * en.wikipedia.org/wiki/Midpoint_circle_algorithm
 */
void draw_circle(int x0, int y0, int radius, uint32_t color)
{
    int x = radius - 1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int diameter = radius << 1;
    int err = dx - diameter;

    while (x >= y)
    {
        draw_line(x0 + y, y0 + x, x0 + y, y0 + x, color);
        draw_line(x0 + x, y0 + y, x0 + x, y0 + y, color);
        draw_line(x0 - y, y0 + x, x0 - y, y0 + x, color);
        draw_line(x0 - x, y0 + y, x0 - x, y0 + y, color);
        draw_line(x0 - x, y0 - y, x0 - x, y0 - y, color);
        draw_line(x0 - y, y0 - x, x0 - y, y0 - x, color);
        draw_line(x0 + y, y0 - x, x0 + y, y0 - x, color);
        draw_line(x0 + x, y0 - y, x0 + x, y0 - y, color);

        if (err <= 0)
        {
            ++y;
            err += dy;
            dy += 2;
        }
        
        if (err > 0)
        {
            --x;
            dx += 2;
            err += dx - diameter;
        }
    }
}

uint32_t get_location_color(int x, int y)
{
    // Calculate the correct location in the mapped memory of the pixel that we want to set
    long location = (x + vinfo.xoffset) * vinfo.bits_per_pixel / 8 + (y + vinfo.yoffset) * finfo.line_length;
    // Set the memory to the value of the pixel in the color
    return *((uint32_t *)(fbp + location));
}

void set_location_color(int x, int y, uint32_t color)
{
    long location = (x + vinfo.xoffset) * vinfo.bits_per_pixel / 8 + (y + vinfo.yoffset) * finfo.line_length;
    *((uint32_t *)(fbp + location)) = color;
}

void flood_fill(int x,int y, uint32_t replacement_color, uint32_t border_color)
{
    printf("Masuk Floodfill\n");
    // printf("%d %d\n",x,y);
    if(x>798 || x<1 || y>598 || y<1)
        return;
    uint32_t target_color = get_location_color(x,y);
    
    if (target_color == replacement_color || target_color == border_color) {
        set_location_color(x,y,replacement_color);
        return;
    }       

    queue<pair<int,int> > Q;
    int x_temp, y_temp;
    pair<int,int> Node(x,y);
    Q.push(Node);
    printf("Awal = %d %d\n",Q.front().first,Q.front().second);

    while(!Q.empty()) {
            
        pair<int,int> N = Q.front();
        Q.pop();
        pair<int,int> w = N;
        pair<int,int> e = N;

        target_color = get_location_color(w.first,w.second);
        while(target_color!=replacement_color && target_color!=border_color) {
            w.first--;
            target_color = get_location_color(w.first,w.second);
        }
        target_color = get_location_color(e.first,e.second);
        while(target_color!=replacement_color && target_color!=border_color) {
            e.first++;
            target_color = get_location_color(e.first,e.second);
        }

        y_temp = N.second;
        for(int x_temp = w.first+1; x_temp<e.first; x_temp++) {
            set_location_color(x_temp,y_temp,replacement_color);

            target_color = get_location_color(x_temp,y_temp+1);
            if(target_color!=replacement_color && target_color!=border_color) {
                Node.first = x_temp;
                Node.second = y_temp+1;
                Q.push(Node);
            }

            target_color = get_location_color(x_temp,y_temp-1);
            if(target_color!=replacement_color && target_color!=border_color) {
                Node.first = x_temp;
                Node.second = y_temp-1;
                Q.push(Node);
            }
        }
    }

    // set_location_color(x,y,replacement_color);
    // flood_fill(x,y-1,replacement_color,border_color);
    // flood_fill(x,y+1,replacement_color,border_color);
    // flood_fill(x+1,y,replacement_color,border_color);
    // flood_fill(x-1,y,replacement_color,border_color);   
}

void transform_point(double *x, double *y, double rot, double scale) {
    // Scale local_x and local_y to object's origin
    *x *= scale;
    *y *= scale;

    // Rotate counter-clockwise local_x and local_y with object's origin as the pivot
    double s = sin(rot);
    double c = cos(rot);
    
    double tmp_x = *x * c + *y * s;
    double tmp_y = *x * s - *y * c;
    *x = tmp_x;
    *y = tmp_y;
}

void draw_object(Object obj) {
    double global_x = obj.x;
    double global_y = obj.y;
    double rot = obj.rot;
    double scale = obj.scale;
    double total_x,total_y;
    double center_x = 0,center_y = 0;
    int x_fill,y_fill;
    uint32_t temp_color = get_color(border_color);

    Drawing drawing = obj.drawing;
    for (int i = 0; i < drawing.n; i++) {
        Primitive primitive = drawing.primitives[i];
        uint32_t true_color = get_color(primitive.color);
        switch (primitive.type)
        {
            case point: {
                double local_x = primitive.values[0];
                double local_y = primitive.values[1];

                transform_point(&local_x, &local_y, rot, scale);

                draw_line(local_x + global_x, local_y + global_y, local_x + global_x, local_y + global_y, primitive.color);
                break;
            }
                

            case line: {
                double local_x0 = primitive.values[0];
                double local_y0 = primitive.values[1];
                double local_x1 = primitive.values[2];
                double local_y1 = primitive.values[3];

                transform_point(&local_x0, &local_y0, rot, scale);
                transform_point(&local_x1, &local_y1, rot, scale);

                draw_line(local_x0 + global_x, local_y0 + global_y, local_x1 + global_x, local_y1 + global_y, true_color);
                break;
            }
                

            case circle: {
                double radius = primitive.values[0];
                double local_x = primitive.values[1];
                double local_y = primitive.values[2];

                // draw its border to be used by flood fill as mark
                transform_point(&local_x, &local_y, rot, scale);
                draw_circle(local_x + global_x, local_y + global_y, radius * scale, temp_color);
                
                flood_fill(local_x + global_x, local_y + global_y,true_color,temp_color);

                // draw its border again with its true color
                // Warning : This will cause a lot more computation
                // transform_point(&local_x, &local_y, rot, scale);
                // draw_circle(local_x + global_x, local_y + global_y, radius * scale, true_color);

                break;
            }
                

            case polygon: {
                double local_prev_x = primitive.values[0];
                double local_prev_y = primitive.values[1];
                
                transform_point(&local_prev_x, &local_prev_y, rot, scale);
                total_x = local_prev_x;
                total_y = local_prev_y;

                // draw its border to be used by flood fill as mark
                for (int i = 1; i < primitive.n / 2; i++) {
                    double local_curr_x = primitive.values[2 * i];
                    double local_curr_y = primitive.values[2 * i + 1];
                    transform_point(&local_curr_x, &local_curr_y, rot, scale);
                    draw_line(local_prev_x + global_x, local_prev_y + global_y, local_curr_x + global_x, local_curr_y + global_y, temp_color);
                    local_prev_x = local_curr_x;
                    local_prev_y = local_curr_y;
                    total_x += local_prev_x;
                    total_y += local_prev_y;
                }
                double local_first_x = primitive.values[0];
                double local_first_y = primitive.values[1];
                transform_point(&local_first_x, &local_first_y, rot, scale);
                draw_line(local_prev_x + global_x, local_prev_y + global_y, local_first_x + global_x, local_first_y + global_y, temp_color);

                // TODO : Fix Floodfill polygon
                if (primitive.custom_center) {
                    center_x = primitive.custom_center_x;
                    center_y = primitive.custom_center_y;
                    transform_point(&center_x, &center_y, rot, scale);
                } else {
                    center_x = total_x/((primitive.n)/2);
                    center_y = total_y/((primitive.n)/2);
                }
                // pair<int, int> center = get_polygon_center(primitive);
                // center_x = center.first;
                // center_y = center.second;
                // cout << "CENTER of the universe =====> " << center_x << " " << center_y << "\n";
                // transform_point(&center_x, &center_y, rot, scale);
                // printf("center_x %.1f y center_y %.1f  total_x %.1f total_y %.1f primitive.n/2 %d \n",center_x,center_y,total_x,total_y,primitive.n/2);
                x_fill = (int) (center_x+global_x);
                y_fill = (int) (center_y+global_y);
                // printf("x_fill %d y_fill %d\n",x_fill,y_fill);
                flood_fill(x_fill, y_fill, true_color, temp_color);

                // draw its border again with its true color
                // Warning : This will cause a lot more computation
                // for (int i = 1; i < primitive.n / 2; i++) {
                //     double local_curr_x = primitive.values[2 * i];
                //     double local_curr_y = primitive.values[2 * i + 1];
                //     transform_point(&local_curr_x, &local_curr_y, rot, scale);
                //     draw_line(local_prev_x + global_x, local_prev_y + global_y, local_curr_x + global_x, local_curr_y + global_y, true_color);
                //     local_prev_x = local_curr_x;
                //     local_prev_y = local_curr_y;
                //     total_x += local_prev_x;
                //     total_y += local_prev_y;
                // }
                // local_first_x = primitive.values[0];
                // local_first_y = primitive.values[1];
                // transform_point(&local_first_x, &local_first_y, rot, scale);
                // draw_line(local_prev_x + global_x, local_prev_y + global_y, local_first_x + global_x, local_first_y + global_y, true_color);

                break;
            }
        
            default:
                break;
        }
        
    }
}
