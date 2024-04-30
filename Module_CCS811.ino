#include "mccs811.h"
#include "Serial/packaging.h"

using namespace CS;

PackagedWired* wire;
mCCS811* ccs = nullptr;
const auto this_device = device_id::CCS811_SENSOR;

void callback(void*, const uint8_t, const char*, const uint8_t);

void setup() {
    Serial.begin(115200);
    while(!Serial);

    Serial.printf("Starting SLAVE\n");
    
    ccs = new mCCS811();
    
    wire = new PackagedWired(config()
        .set_slave(this_device)
        .set_slave_callback(callback)
        .set_led(2)
    );
}

void callback(void* rw, const uint8_t expects, const char* received, const uint8_t length)
{
    if (length != sizeof(Requester)) return;
    
    PackagedWired& w = *(PackagedWired*) rw;
    Requester req(received);
    
    switch(req.get_offset()) {
    case 0:
    {
        const uint64_t val = static_cast<uint64_t>(ccs->get_eco2());
        Command cmd("/ccs811/eco2", val);
        w.slave_reply_from_callback(cmd);
        //Serial.printf("Received request {%zu}\nReplying with %llu\n", req.get_offset(), val);
    }
    break;
    case 1:
    {
        const uint64_t val = static_cast<uint64_t>(ccs->get_tvoc());
        Command cmd("/ccs811/tvoc", val);
        w.slave_reply_from_callback(cmd);
        //Serial.printf("Received request {%zu}\nReplying with %llu\n", req.get_offset(), val);
    }
    break;
    default:
    {
        Command cmd; // invalid
        w.slave_reply_from_callback(cmd);
        //Serial.printf("Received request {%zu}\nConsidered invalid!\n", req.get_offset());
    }
    }
}

// unused
void loop() { vTaskDelete(NULL); }