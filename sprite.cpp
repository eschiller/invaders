#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>

#include "sprite.h"
#include "board.h"

using namespace std;

/*
 * standard constructor for sprite class. Takes a passed key, image path, image
 * size/location, and whether the image should be rendered or not, and
 * initializes all private members.
 *
 * Velocities and current frame are assumed to be 0. Frames per move is 1.
 */
Sprite::Sprite(int key, string img_path, int width, int height, int xpos, int ypos, int render) {
    this->key = key;
    this->img_path = img_path;
    this->tex = NULL;
    this->width = width;
    this->height = height;
    this->xpos = xpos;
    this->ypos = ypos;
    this->xvel = xvel;
    this->yvel = yvel;
    this->frames_per_move = 1;
    this->curr_frame = 0;
    this->render = render;
}


/*
 * default destructor. Frees up texture resources.
 */
Sprite::~Sprite() {
    SDL_DestroyTexture(this->tex);
}


/*
 * load_texture_from_path will load an image specified in the passed string to
 * a SDL_Surface, create a SDL_Texture from that surface, and return the
 * texture.
 *
 * returns 0 on success, -1 on failure
 */
int Sprite::load_texture() {
    SDL_Surface *tempsurface;
    SDL_Texture *temptex;
    printf("about to render a thing.\n");
    if((tempsurface = IMG_Load(this->img_path.c_str())) == NULL) {
        fprintf(stderr, "Invaders: Unable to load image %s\n%s\n",
                this->img_path.c_str(), IMG_GetError());
        return -1;
    }
    if((temptex = SDL_CreateTextureFromSurface(get_board_renderer(), tempsurface)) == NULL) {
        fprintf(stderr, "Invaders: %s\n", SDL_GetError());
        return -1;
    }
    printf("loaded a texture for %s\n", this->img_path.c_str());
    SDL_FreeSurface(tempsurface);
    this->tex = temptex;
    return 0;
}


/*
 * Sprite::move_sprite will chat the x/y positioning of the sprite object by
 * th amount of the passed integers. Negative values are accepted.
 *
 * returns 0 on success, -1 if the move hit the bounds of the window.
 */
int Sprite::move_sprite(int x_delta, int y_delta) {
    int newx, newy;
    newx = x_delta + this->xpos;
    newy = y_delta + this->ypos;
    int rval = 0;

    //check the bounds of the gameplay walls and bring back inside if needed
    if (newx < 0) {
        newx = 0;
        rval = -1;
    } else if (newx > WIN_WIDTH) {
        newx = WIN_WIDTH;
        rval = -1;
    }

    if (newy < -8) {
        newy = -8;
        rval = -1;
    } else if (newy > WIN_HEIGHT) {
        newy = WIN_HEIGHT;
        rval = -1;
    }

    //set the x and y in array
    this->xpos = newx;
    this->ypos = newy;
    return rval;
}


/*
 * enable_sprite sets the value of "render" to 1, causing the sprite to become
 * rendered on the next frame, until disable_sprite is called.
 */
void Sprite::enable_sprite() {
    this->render = 1;
}


/*
 * disable_sprite sets the value of "render" to 0, causing the sprite to no
 * longer be rendered.
 *
 */
void Sprite::disable_sprite() {
    this->render = 0;
}


/*
 * get_key is a simple getter function that returns the value of the
 * sprite object's key member.
 */
int Sprite::get_sprite_key() {
    return this->key;
}


/*
 * get_sprite_texture is a smiple getter function that returns the value of the
 * sprite objet's tex member
 */
SDL_Texture *Sprite::get_sprite_texture(){
    return this->tex;
}


/*
 * get_sprite_x is a simple getter function.
 *
 * returns the xpos of the sprite object.
 */
int Sprite::get_sprite_x() {
    return this->xpos;
}


/*
 * get_sprite_y is a simple getter function.
 *
 * returns the ypos of the sprite object.
 */
int Sprite::get_sprite_y() {
    return this->ypos;
}


/*
 * get_sprite_width is a simple getter function.
 *
 * returns the width of the sprite object.
 */
int Sprite::get_sprite_width() {
    return this->width;
}


/*
 * get_sprite_height is a simple getter function.
 *
 * returns the height of the passed sprite
 */
int Sprite::get_sprite_height() {
    return this->height;
}


/*
 * set_sprite_x sets the x position of the sprite to the passed new x
 * position. If the x position is outside the bounds of the window, x is set to
 * the closest bounding edge.
 *
 * returns the new x position.
 */
int Sprite::set_sprite_x(int newx) {
    this->xpos = newx;
    return newx;
}



/*
 * set_sprite_y sets the y position of the sprite to the passed new y
 * position. If the y position is outside the bounds of the window, y is set to
 * the closest bounding edge.
 *
 * returns the new y position.
 */
int Sprite::set_sprite_y(int newy) {
    this->ypos = newy;
    return newy;
}


/*
 * set_sprite_pos is a convenience wrapper around set_sprite_x and set_sprite_y
 *
 * returns 0
 */
int Sprite::set_sprite_pos(int newx, int newy) {
    set_sprite_x(newx);
    set_sprite_y(newy);
    return 0;
}


/*
 * is_enabled is a simple getter function that returns the value of the
 * sprite object's render member.
 *
 * Returns 0 if render is set to 0, 1 if render is set to 1.
 */
int Sprite::is_enabled() {
    return this->render;
}


/*
 * get_img_path is a simple getter function for the image path string.
 */
string Sprite::get_img_path(){
    return this->img_path;
}


/*
 * get_key is a simple getter function for the key member of a Sprite.
 */
int Sprite::get_key(){
    return this->key;
}


/*
 * get_sprite_xvel is a simple getter function for the xvel of a Sprite.
 */
int Sprite::get_sprite_xvel() {
    return this->xvel;
}


/*
 * get_sprite_yvel is a simple getter function for the yvel of a Sprite.
 */
int Sprite::get_sprite_yvel() {
    return this->yvel;
}


/*
 * change_vel changes the value of xvel and yvel based on the passed deltas.
 * This values (xval and yval) are checked each frame and sprites are moved an
 * amount equal to their values.
 */
int Sprite::change_vel(int x_delta, int y_delta) {
    this->xvel += x_delta;
    this->yvel += y_delta;
    return 0;
}


/*
 * set_xvel sets the value of xvel based on the passed value.
 * The value xvel checked each frame and sprites are moved an
 * amount equal to their values.
 */
int Sprite::set_sprite_xvel(int xvel) {
    this->xvel = xvel;
    return 0;
}


/*
 * set_yvel sets the value of yvel based on the passed value.
 * The value yvel checked each frame and sprites are moved an
 * amount equal to their values.
 */
int Sprite::set_sprite_yvel(int yvel) {
    this->yvel = yvel;
    return 0;
}
