#ifndef DRAW_UTIL_H
#define DRAW_UTIL_H

#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <utility>
#include <algorithm>
#include <vector>

#include "primitive.hpp"

using namespace std;

int N_DIR = 4;
int X_DIR[] = { 0, 0, 1, -1 };
int Y_DIR[] = { 1, -1, 0, 0 };

void _transform_point(double &x, double &y, double rot, double scale) {
    // Scale local_x and local_y to object's origin
    x *= scale;
    y *= scale;

    // Rotate counter-clockwise local_x and local_y with object's origin as the pivot
    double s = sin(rot);
    double c = cos(rot);
    
    double tmp_x = x * c + y * s;
    double tmp_y = x * s - y * c;
    x = tmp_x;
    y = tmp_y;
}

void mark_point(int x, int y, vector< vector<bool> > &mark) {
	mark[x][y] = true;
}

/**
 * Draw list of pixel location on the screen in 'color'
 * CASE: abs(y1 - y0) >= abs(x1 - x0) and y1 >= y0
 */
void mark_line_high(int x0, int y0, int x1, int y1, vector< vector<bool> > &mark)
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
        mark_point(x, y, mark);
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
void mark_line_low(int x0, int y0, int x1, int y1, vector< vector<bool> > &mark)
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
        mark_point(x, y, mark);
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
void mark_line_straight_vertical(int x0, int y0, int x1, int y1, vector< vector<bool> > &mark)
{
    for (int y = y0; y <= y1; ++y)
    {
        mark_point(x0, y, mark);
    }
}

/**
 * Draw list of pixel location on the screen in 'color'
 * CASE: y0 == y1 and x0 >= x1
 */
void mark_line_straight_horizontal(int x0, int y0, int x1, int y1, vector< vector<bool> > &mark)
{
    for (int x = x0; x <= x1; ++x)
    {
        mark_point(x, y0, mark);
    }
}

/**
 * Draw list of pixel location on the screen in 'color'
 */
void mark_line(int x0, int y0, int x1, int y1, vector< vector<bool> > &mark)
{
    if (x0 == x1)
    {
        if (y0 > y1)
        {
            mark_line_straight_vertical(x1, y1, x0, y0, mark);
        }
        else
        {
            mark_line_straight_vertical(x0, y0, x1, y1, mark);
        }
    }
    else if (y0 == y1)
    {
        if (x0 > x1)
        {
            mark_line_straight_horizontal(x1, y1, x0, y0, mark);
        }
        else
        {
            mark_line_straight_horizontal(x0, y0, x1, y1, mark);
        }
    }
    else
    {
        if (abs(y1 - y0) < abs(x1 - x0))
        {
            if (x0 > x1)
            {
                mark_line_low(x1, y1, x0, y0, mark);
            }
            else
            {
                mark_line_low(x0, y0, x1, y1, mark);
            }
        }
        else
        {
            if (y0 > y1)
            {
                mark_line_high(x1, y1, x0, y0, mark);
            }
            else
            {
                mark_line_high(x0, y0, x1, y1, mark);
            }
        }
    }
}

bool is_inside(int cur_x, int cur_y, vector< vector<bool> > &mark) {
	int border_count = 0;
	for (int i = cur_x; i < mark.size(); i++) {
		if (mark[i][cur_y]) {
            if ((i + 1) < mark.size() && mark[i + 1][cur_y]) {
                return false;
            }
			border_count++;
		}
	}
	return (border_count%2 == 1);
}

pair<int, int> get_polygon_center(Primitive p, double rot, double scale)
{
    cout << ">> STARTING get_polygon_center\n";
    // Find max_x, max_y
    // cout << ">> Finding max\n";
    int min_x = 99999;
    int min_y = 99999;
    int max_x = 0;
    int max_y = 0;
    for (int i = 0; i < p.n / 2; i++) {
        double cur_x = p.values[2*i];
        double cur_y = p.values[2*i + 1];
        _transform_point(cur_x, cur_y, rot, scale);
        max_x = max(max_x, (int)cur_x);
        max_y = max(max_y, (int)cur_y);
        min_x = min(min_x, (int)cur_x);
        min_y = min(min_y, (int)cur_y);
    }

    // Make matrix max_x * max_y
    cout << ">> Make matrix\n";
    vector< vector<bool> > mark(max_x - min_x + 2, vector<bool>(max_y - min_y + 2, false));
    vector< vector<bool> > visited(max_x - min_x + 2, vector<bool>(max_y - min_y + 2, false));

    // Mark borders
    cout << ">> Make borders\n";
    double prev_x = p.values[0];
    double prev_y = p.values[1];
    _transform_point(prev_x, prev_y, rot, scale);
    prev_x -= min_x;
    prev_y -= min_y;

    for (int i = 1; i < p.n / 2; i++) {
        double curr_x = p.values[2 * i] - min_x;
        double curr_y = p.values[2 * i + 1] - min_y;
        _transform_point(curr_x, curr_y, rot, scale);
        mark_line(prev_x, prev_y, curr_x, curr_y, mark);
        prev_x = curr_x;
        prev_y = curr_y;
    }

    int first_x = p.values[0] - min_x;
    int first_y = p.values[1] - min_y;
    mark_line(prev_x, prev_y, first_x, first_y, mark);

    // (somehow) find point that taki2 rumba
    // Approach: BFS from the first point
    cout << ">> Main dish\n";
    int initial_x = p.values[0] - min_x;
    int initial_y = p.values[1] - min_y;
    queue< pair<int,int> > candidates;

    candidates.push(make_pair(initial_x, initial_y));

    cout << ">> Starting BFS\n";
    while (!candidates.empty()) {
    	pair<int,int> cur = candidates.front();
        candidates.pop();

        int cur_x = cur.first;
    	int cur_y = cur.second;

        cout << ">>>> cur " << cur_x << " " << cur_y << "\n";

    	visited[cur_x][cur_y] = true;

        cout << ">>>> tebote 1\n";
    	if (!mark[cur_x][cur_y] && is_inside(cur_x, cur_y, mark)) {
            cout << ">> FOUND THE ONE!!!\n";
    		return make_pair(cur_x + min_x, cur_y + min_y);
    	}

        cout << ">>>> tebote 2\n";
    	for (int i = 0; i < N_DIR; i++) {
    		int next_x = cur_x + X_DIR[i];
    		int next_y = cur_y + Y_DIR[i];
    		bool x_valid = (next_x >= 0 && next_x < mark.size());
    		bool y_valid = (next_y >= 0 && next_y < mark[0].size());
            cout << ">>>> tebote 3\n";
    		if (x_valid && y_valid && !visited[next_x][next_y]) {
    			candidates.push(make_pair(next_x, next_y));
    		}
    	}
    }

    cout << ">> somethis is wrongggg\n";
    // shouldn't reach this EVER
 	return make_pair(initial_x + min_x, initial_y + min_y);
}


#endif