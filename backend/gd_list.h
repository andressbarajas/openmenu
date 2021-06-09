/*
 * File: gd_list.h
 * Project: ini_parse
 * File Created: Wednesday, 19th May 2021 5:33:33 pm
 * Author: Hayden Kowalchuk
 * -----
 * Copyright (c) 2021 Hayden Kowalchuk, Hayden Kowalchuk
 * License: BSD 3-clause "New" or "Revised" License, http://www.opensource.org/licenses/BSD-3-Clause
 */

#pragma once
struct gd_item;
int list_read(void);
void list_destroy(void);
void list_print_slots(void);
void list_print_temp(void);
void list_print(const struct gd_item **list);

const struct gd_item **list_get(void);
const struct gd_item **list_get_sort_name(void);
const struct gd_item **list_get_sort_date(void);
const struct gd_item **list_get_sort_product(void);
const struct gd_item **list_get_sort_default(void);
int list_length(void);

const struct gd_item *list_item_get(unsigned int idx);