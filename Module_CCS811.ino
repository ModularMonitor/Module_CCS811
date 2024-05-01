#include "mccs811.h"
#include "Serial/packaging.h"
#include "Serial/flags.h"

using namespace CS;

PackagedWired* wire;
mCCS811* ccs = nullptr;
const auto this_device = device_id::CCS811_SENSOR;

void callback(void*, const uint8_t, const char*, const uint8_t);

void setup() {
    Serial.begin(115200);

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
        FlagWrapper fw;
        if (ccs->has_issues())              fw |= device_flags::HAS_ISSUES;
        if (ccs->has_new_data_autoreset())  fw |= device_flags::HAS_NEW_DATA;
        
        Command cmd("#FLAGS", (uint64_t)fw);
        w.slave_reply_from_callback(cmd);
    }
    break;
    case 1:
    {
        const uint64_t val = static_cast<uint64_t>(ccs->get_eco2());
        Command cmd("/ccs811/eco2", val);
        w.slave_reply_from_callback(cmd);
    }
    break;
    case 2:
    {
        const uint64_t val = static_cast<uint64_t>(ccs->get_tvoc());
        Command cmd("/ccs811/tvoc", val);
        w.slave_reply_from_callback(cmd);
    }
    break;
    default:
    {
        Command cmd; // invalid
        w.slave_reply_from_callback(cmd);
    }
    }
}

// unused
void loop() { vTaskDelete(NULL); }