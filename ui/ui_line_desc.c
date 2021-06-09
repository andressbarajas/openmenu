/*
 * File: ui_line_desc.c
 * Project: ui
 * File Created: Wednesday, 19th May 2021 9:08:07 pm
 * Author: Hayden Kowalchuk
 * -----
 * Copyright (c) 2021 Hayden Kowalchuk, Hayden Kowalchuk
 * License: BSD 3-clause "New" or "Revised" License, http://www.opensource.org/licenses/BSD-3-Clause
 */

#include "ui_line_desc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../backend/gd_item.h"
#include "../backend/gd_list.h"
#include "../texture/txr_manager.h"
#include "draw_prototypes.h"
#include "font_prototypes.h"

/* List managment */
static int current_selected_item = 0;
enum control input_current;
#define INPUT_TIMEOUT (10)
static int navigate_timeout = -1;

/* For drawing */
static image txr_highlight, txr_bg; /* Highlight square and Background */
static image txr_icon_list[16];     /* Lower list of 9 icons */
static image *txr_focus;            /* current selected item */

/* Our actual gdemu items */
static const gd_item **list_current;
static int list_len;
enum sort_type { DEFAULT,
                 ALPHA,
                 DATE,
                 PRODUCT,
                 SORT_END };
enum sort_type sort_current = DEFAULT;

static void
draw_bg_layers() {
  draw_draw_image(0, 0, 640, 480, 1.0f, &txr_bg);
}

static void draw_big_box() {
  draw_draw_image(92, 92, 208, 208, 1.0f, txr_focus);
}

static void draw_small_boxes() {
#define LIST_ADJUST (2)
  int i;
  int num_icons = 10; /* really 9..., the math is easier for 10 */
  int starting_icon_idx = current_selected_item - 4;
  float x_start = -24.0f;
  float y_pos = 350.0f;
  float icon_size = 68.0f;
  float icon_spacing = 8.0f;
  int highlighted_icon = (num_icons / 2 - 1);

  /* possible change how many we draw based on if we are not quite at the 5th item in the list */
  if (current_selected_item < 5) {
    num_icons = 5 + current_selected_item;
    x_start += (4 - current_selected_item) * (icon_size + icon_spacing);
    highlighted_icon = current_selected_item;
    starting_icon_idx = 0;
    num_icons++;
  }

  draw_draw_square(x_start + (icon_size + icon_spacing) * highlighted_icon - 4.0f, y_pos - 4.0f, icon_size + 8, 1.0f, &txr_highlight);

  /* attempt to use texture loader */
  txr_get_small(list_current[starting_icon_idx + 0]->product, &txr_icon_list[0]);

  draw_draw_square(x_start, y_pos, icon_size, 1.0f, &txr_icon_list[0]);
  for (i = 1; (i < num_icons - 1) && (i + starting_icon_idx < list_len); i++) {
    txr_get_small(list_current[starting_icon_idx + i]->product, &txr_icon_list[i]);
    draw_draw_square(x_start + (icon_size + icon_spacing) * i, y_pos, icon_size, 1.0f, &txr_icon_list[i]);
  }
#undef LIST_ADJUST
}

FUNCTION(UI_NAME, init) {
  draw_load_texture("/cd/highlight.pvr", &txr_highlight);
  draw_load_texture("/cd/bg_right.pvr", &txr_bg);
  list_current = list_get();
  list_len = list_length();

  txr_focus = &txr_icon_list[0];

  font_init();
}

FUNCTION(UI_NAME, setup) {
  /* empty for now */
}

static void menu_decrement(void) {
  if (navigate_timeout > 0) {
    navigate_timeout--;
    return;
  }
  if (current_selected_item > 0) {
    current_selected_item--;
  }
  navigate_timeout = INPUT_TIMEOUT;
}

static void menu_increment(void) {
  if (navigate_timeout > 0) {
    navigate_timeout--;
    return;
  }
  if (++current_selected_item >= list_len) {
    current_selected_item = list_len - 1;
  }
  navigate_timeout = INPUT_TIMEOUT;
}

static void menu_accept(void) {
  dreamcast_rungd(list_current[current_selected_item]->slot_num);
}

static void menu_swap_sort(void) {
  if (navigate_timeout > 0) {
    navigate_timeout--;
    return;
  }
  sort_current++;
  if (sort_current == SORT_END) {
    sort_current = DEFAULT;
  }
  switch (sort_current) {
    case ALPHA:
      list_current = list_get_sort_name();
      break;
    case DATE:
      list_current = list_get_sort_date();
      break;
    case PRODUCT:
      list_current = list_get_sort_product();
      break;
    case DEFAULT:
    default:
      list_current = list_get_sort_default();
      break;
  }
  current_selected_item = 0;
  navigate_timeout = INPUT_TIMEOUT;
}

FUNCTION_INPUT(UI_NAME, handle_input) {
  enum control input_current = button;
  switch (input_current) {
    case LEFT:
      menu_decrement();
      break;
    case RIGHT:
      menu_increment();
      break;
    case A:
      menu_accept();
      break;
    case START:
      menu_swap_sort();
      break;

    /* These dont do anything */
    case UP:
      break;
    case DOWN:
      break;
    case B:
      break;
    case X:
      break;
    case Y:
      break;
    /* Always nothing */
    case NONE:
    default:
      break;
  }
}

static void update_data(void) {
  txr_get_small(list_current[current_selected_item]->product, txr_focus);
}

FUNCTION(UI_NAME, draw) {
  update_data();

  draw_bg_layers();

  draw_big_box();

  draw_small_boxes();

  font_begin_draw();
  font_draw_main(316, 92, 1.0f, list_current[current_selected_item]->name);
  font_draw_sub_wrap(316, 128, 1.0f, 280, "Game Synopsis here...");
  const char *text = NULL;
  switch (sort_current) {
    case ALPHA:
      text = "Name";
      break;
    case DATE:
      text = "Date";
      break;
    case PRODUCT:
      text = "Product ID";
      break;
    case DEFAULT:
    default:
      text = "SD Card Order";
      break;
  }
  font_draw_main(4, 440, 1.0f, text);
}