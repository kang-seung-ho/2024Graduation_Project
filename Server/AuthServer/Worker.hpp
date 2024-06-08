#pragma once
import <stop_token>;

void Worker(std::stop_token&& canceller, const size_t index, class Framework& framework);
