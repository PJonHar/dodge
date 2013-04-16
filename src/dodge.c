#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define SCREEN_WIDTH 144
#define SCREEN_HEIGHT 168
#define STATUS_BAR_HEIGHT 16
#define CAR_WIDTH 41
#define CAR_HEIGHT 20
#define MY_UUID { 0x49, 0xD7, 0xB6, 0x29, 0x8F, 0x92, 0x4F, 0xF7, 0xAA, 0x73, 0x93, 0x3D, 0xCD, 0x74, 0x1B, 0x48 }
PBL_APP_INFO(MY_UUID,
             "Dodge", "Paul Harrold",
             0, 1, /* App version */
             RESOURCE_ID_IMAGE_MENU_ICON_DODGE,
             APP_INFO_STANDARD_APP);

Window window;
TextLayer text_gameover_layer;
BitmapLayer car;
BmpContainer car_image_container;
BitmapLayer baddy[5];
BmpContainer baddy_image_container;
GRect car_left_rect, car_center_rect, car_right_rect;
GRect baddy_left_rect, baddy_center_rect, baddy_right_rect;
GRect get_car_rect, get_baddy_rect;
int iPointOne, iPointTwo, iPointThree;
int iCurrentTime, iTotalBaddys;
bool bGameOver=false;

void up_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
    (void)recognizer;
    (void)window;
    
    get_car_rect = layer_get_frame(&car.layer);
    
    if (get_car_rect.origin.x==iPointOne||get_car_rect.origin.x==iPointTwo){
    layer_set_frame(&car.layer, car_left_rect);
    }
    if (get_car_rect.origin.x==iPointThree){
        layer_set_frame(&car.layer, car_center_rect);
    }
    
}

void down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
    (void)recognizer;
    (void)window;
    
    get_car_rect = layer_get_frame(&car.layer);

    if (get_car_rect.origin.x==iPointThree||get_car_rect.origin.x==iPointTwo){
        layer_set_frame(&car.layer, car_right_rect);
    }
    if (get_car_rect.origin.x==iPointOne){
        layer_set_frame(&car.layer, car_center_rect);
    }
    
}

void select_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
    (void)recognizer;
    (void)window;
    
}

void click_config_provider(ClickConfig **config, Window *window) {
    (void)window;
    
    config[BUTTON_ID_SELECT]->click.handler = (ClickHandler) select_single_click_handler;

    config[BUTTON_ID_UP]->click.handler = (ClickHandler) up_single_click_handler;
   
    config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) down_single_click_handler;
 
}

void handle_tick(AppContextRef ctx, PebbleTickEvent *t) {
    (void)t;
    (void)ctx;
    
    iCurrentTime++;
    
    get_car_rect = layer_get_frame(&car.layer);
    
    if ((iCurrentTime%5==0)&&(iTotalBaddys<=5)&&!(bGameOver)){
        
        switch (1) {
            case 1:
                bitmap_layer_init(&baddy[iTotalBaddys+1], baddy_left_rect);
                break;
            case 2:
                bitmap_layer_init(&baddy[iTotalBaddys+1], baddy_center_rect);
                break;
            case 3:
                bitmap_layer_init(&baddy[iTotalBaddys+1], baddy_right_rect);
                break;
        }
        
        layer_add_child(&window.layer, &baddy[iTotalBaddys+1].layer);
        bitmap_layer_set_bitmap( &baddy[iTotalBaddys+1], &(baddy_image_container.bmp) );
        iTotalBaddys++;
    }
    
    if (!bGameOver) {
    for (int i = 0;i<5;i++){
        
        get_baddy_rect = layer_get_frame(&baddy[i].layer);
    
        if (get_baddy_rect.origin.y>=168){
            layer_remove_from_parent(&baddy[i].layer);
            iTotalBaddys--;
        }
    
        if ((get_baddy_rect.origin.y+CAR_HEIGHT>=get_car_rect.origin.y)&&(get_baddy_rect.origin.x==get_car_rect.origin.x)) {
            layer_remove_from_parent(&car.layer);
            for (int i = 0;i<=iTotalBaddys;i++){
                layer_remove_from_parent(&baddy[i].layer);
            }
            layer_add_child(&window.layer, &text_gameover_layer.layer);
            bGameOver=true;
        }
    
        get_baddy_rect.origin.y=get_baddy_rect.origin.y+10;
    
        layer_set_frame(&baddy[i].layer, get_baddy_rect);
    }
    }
 
}

void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Window Name");
  window_stack_push(&window, true /* Animated */);
    window_set_background_color(&window, GColorBlack);

    resource_init_current_app(&APP_RESOURCES);
    
    iCurrentTime = 0;
    
    // calculate column positions
    iPointOne=5;
    iPointTwo=5+CAR_WIDTH+5;
    iPointThree=5+CAR_WIDTH+5+CAR_WIDTH+5;
    
    // calculate column spaces
    car_left_rect = GRect(iPointOne, SCREEN_HEIGHT-CAR_HEIGHT-STATUS_BAR_HEIGHT, CAR_WIDTH, CAR_HEIGHT);
    car_center_rect = GRect(iPointTwo, SCREEN_HEIGHT-CAR_HEIGHT-STATUS_BAR_HEIGHT, CAR_WIDTH, CAR_HEIGHT);
    car_right_rect = GRect(iPointThree, SCREEN_HEIGHT-CAR_HEIGHT-STATUS_BAR_HEIGHT, CAR_WIDTH, CAR_HEIGHT);
    
    // calculate column spaces
    baddy_left_rect = GRect(iPointOne, 0, CAR_WIDTH, CAR_HEIGHT);
    baddy_center_rect = GRect(iPointTwo, 0, CAR_WIDTH, CAR_HEIGHT);
    baddy_right_rect = GRect(iPointThree, 0, CAR_WIDTH, CAR_HEIGHT);
    
    // Set up a layer for the car image
    bmp_init_container(RESOURCE_ID_IMAGE_CAR, &car_image_container);
    bitmap_layer_init(&car, car_center_rect);
    layer_add_child(&window.layer, &car.layer);
    
    // add bitmap
    bitmap_layer_set_bitmap( &car, &(car_image_container.bmp) );
    
    // Set up a layer for the baddy image
    bmp_init_container(RESOURCE_ID_IMAGE_BADDY, &baddy_image_container);
    bitmap_layer_init(&baddy[0], baddy_left_rect);
    layer_add_child(&window.layer, &baddy[0].layer);
    
    iTotalBaddys=1;
    
    // add bitmap
    bitmap_layer_set_bitmap( &baddy[0], &(baddy_image_container.bmp) );
    
    // game over text
    text_layer_init(&text_gameover_layer, GRect(0, 72, SCREEN_WIDTH, SCREEN_HEIGHT));
    text_layer_set_text_color(&text_gameover_layer, GColorWhite);
    text_layer_set_background_color(&text_gameover_layer, GColorClear);
    text_layer_set_text_alignment(&text_gameover_layer, GTextAlignmentCenter);
    text_layer_set_text(&text_gameover_layer, "GAME OVER");
    
    // Attach our desired button functionality
    window_set_click_config_provider(&window, (ClickConfigProvider) click_config_provider);
}

void handle_deinit(AppContextRef ctx) {
    (void)ctx;
    
    bmp_deinit_container(&car_image_container);
    bmp_deinit_container(&baddy_image_container);

}

void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
      .deinit_handler = &handle_deinit,
      .tick_info = {
          .tick_handler = &handle_tick,
          .tick_units = SECOND_UNIT
      }
  };
  app_event_loop(params, &handlers);
}
