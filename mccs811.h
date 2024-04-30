#pragma once

#include "Adafruit_CCS811/Adafruit_CCS811.h"

#include "SelfThreadable/self_threadable.h"

// mCCS811 wraps CCS811 in a async class. This way, less things to worry about.
class mCCS811 : protected SelfThreadable {
    bool m_fail_flag = true;
    uint16_t m_eCO2 = 0, m_TVOC = 0;

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
                m_eCO2 = ccs.geteCO2();
                m_TVOC = ccs.getTVOC();
            }
        }

        vTaskDelete(NULL);
    }
public:
    mCCS811() : SelfThreadable("ASYNC_CCS") { async_start(); }

    uint16_t get_eco2() const;
    uint16_t get_tvoc() const;
    
    bool has_issues() const;
};


inline uint16_t mCCS811::get_eco2() const
{
    return m_eCO2;
}

inline uint16_t mCCS811::get_tvoc() const
{
    return m_TVOC;
}

bool mCCS811::has_issues() const
{
    return m_fail_flag;
}