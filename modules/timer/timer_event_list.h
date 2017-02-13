//
// Created by pojol on 2017/2/13.
//

#ifndef GSF_TIMER_EVENT_LIST_H_H
#define GSF_TIMER_EVENT_LIST_H_H

#include <stdint.h>

//! args uint32_t milliseconds
const uint32_t event_delay_milliseconds = 1001;

//! args uint32_t hour , uint32_t minute
const uint32_t event_delay_day = 1002;

//! args uint32_t day , uint32_t hour
const uint32_t event_delay_week = 1003;

//! args uint32_t day , uint32_t hour
const uint32_t event_delay_month = 1004;

//!
const uint32_t event_remove_timer = 1005;

#endif //GSF_TIMER_EVENT_LIST_H_H
