#ifndef SPRITE_H_
#define SPRITE_H_

#include <string>

using namespace std;

class Sprite {
private:
    string key;
    string img_path;
    SDL_Texture *tex;
    int width;
    int height;
    int xpos;
    int ypos;
    int xvel;
    int yvel;
    int frames_per_move;
    int curr_frame;
    int render;

public:
    Sprite(string key, string image, int width, int height, int xpos, int ypos, int render);
    ~Sprite();
    int load_texture();
    int move_sprite(int x_delta, int y_delta);
    string get_img_path();
    void enable_sprite();
    void disable_sprite();
    string get_sprite_key();
    SDL_Texture *get_sprite_texture();
    int get_sprite_x();
    int get_sprite_y();
    int get_sprite_xvel();
    int get_sprite_yvel();
    int get_sprite_width();
    int get_sprite_height();
    int set_sprite_x(int newx);
    int set_sprite_y(int newy);
    int set_sprite_xvel(int xvel);
    int set_sprite_yvel(int xvel);
    int set_sprite_pos(int newx, int newy);
    int is_enabled();
    int change_vel(int x_delta, int y_delta);
};

#endif
