#pragma once 
#include <Arduino.h>

QueueHandle_t controlMsg = xQueueCreate(2, sizeof(int));
