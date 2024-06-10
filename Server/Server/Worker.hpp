#pragma once
import <stop_token>;

void Worker(std::stop_token&& cancel_token, class Framework& framework, size_t index);
