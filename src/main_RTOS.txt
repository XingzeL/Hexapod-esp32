#include "wifi_Task.h"
#include "moveLeg_Task.h"

#define CALI_ONLY 0
#define MOVE_ONLY 0
#define MOVEMENT_CORE 1

void setup() {
    Serial.begin(115200);
    if (!MOVE_ONLY) {
        xTaskCreate(microRosControl_wifi,  //wifi task default running on core 0
                    "Control with micro-ros_wifi",
                    1024 * 4,
                    NULL,
                    1,
                    NULL);
        if (DEBUG) {Serial.println("wifi task created!");}
    }

    
    if (!CALI_ONLY) {
        xTaskCreatePinnedToCore(moveLegTask,
                "Move Leg",
                1024 * 6,
                NULL,
                1,
                NULL,
                MOVEMENT_CORE); //move leg task, set on core 1
        if (DEBUG) {Serial.println("move leg task created!");}       
    }
    vTaskDelete(NULL); 
}

void loop() {

}