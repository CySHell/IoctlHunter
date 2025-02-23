#pragma once

#include "Shared.h"
#include "Main.h"




void InitQueue(JsonQueue* queue);
void EnqueueJsonObj(JsonQueue* queue, json_object* json);
json_object* DequeueJsonObj(JsonQueue* queue);
