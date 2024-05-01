#pragma once

#include "Adafruit_CCS811/Adafruit_CCS811.h"

#include "SelfThreadable/self_threadable.h"

// mCCS811 wraps CCS811 in a async class. This way, less things to worry about.
class mCCS811 : protected SelfThreadable {
    uint16_t m_eCO2 = 0, m_TVOC = 0;
    
    bool m_fail_flag = false;
    bool m_has_new_data = false;

    void async() {
        Adafruit_CCS811 ccs;
        
        while(!ccs.begin()){
            m_fail_flag = true;
            delay(200);
        }
        
        m_fail_flag = false;

        while(1) {
            delay(1000);
            if(!ccs.readData()){
                m_fail_flag = false;
                m_eCO2 = ccs.geteCO2();
                m_TVOC = ccs.getTVOC();
                m_has_new_data = true;
            }
            else {
                m_fail_flag = true;
            }
        }

        vTaskDelete(NULL);
    }
public:
    mCCS811() : SelfThreadable("ASYNC") { async_start(); }

    uint16_t get_eco2() const { return m_eCO2; }
    uint16_t get_tvoc() const { return m_TVOC; }

    bool has_issues() const { return m_fail_flag; }
    bool has_new_data_autoreset() { bool had = m_has_new_data; m_has_new_data = false; return had; }
};

