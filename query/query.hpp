#pragma once

#include "object.hpp"
#include <list>
#include <unordered_map>
#include <iostream>

void query_connect_database();
void query_print_attribute_list();
void query_execute(std::string user_query, std::list<object>& obj_list, std::unordered_map<uint32_t, object>& obj_map);
void query_increment_access_counter(uint32_t obj_id);