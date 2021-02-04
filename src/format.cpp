#include <string>

#include "format.h"

using std::string;

string Format::ElapsedTime(long seconds) {
    long hh{0}, mm{0}, ss{0};
    std::string hh_string, mm_string, ss_string;
    
    hh = seconds / 3600;
    mm = ( seconds / 60 ) - (hh * 60);
    ss = seconds - ( hh * 3600 ) - ( mm * 60 );

    (hh < 10) ? hh_string = "0" + std::to_string(hh) : hh_string = std::to_string(hh);
    (mm < 10) ? mm_string = "0" + std::to_string(mm) : mm_string = std::to_string(mm);
    (ss < 10) ? ss_string = "0" + std::to_string(ss) : ss_string = std::to_string(ss);

    return hh_string + ":" + mm_string + ":" + ss_string;
}