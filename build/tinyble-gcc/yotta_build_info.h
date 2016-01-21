#ifndef __YOTTA_BUILD_INFO_H__
#define __YOTTA_BUILD_INFO_H__
// yotta build info, #include YOTTA_BUILD_INFO_HEADER to access
#define YOTTA_BUILD_YEAR 2016 // UTC year
#define YOTTA_BUILD_MONTH 1 // UTC month 1-12
#define YOTTA_BUILD_DAY 21 // UTC day 1-31
#define YOTTA_BUILD_HOUR 1 // UTC hour 0-24
#define YOTTA_BUILD_MINUTE 27 // UTC minute 0-59
#define YOTTA_BUILD_SECOND 58 // UTC second 0-61
#define YOTTA_BUILD_UUID 42b0c286-1da6-4118-a18b-7e780fff0763 // unique random UUID for each build
#define YOTTA_BUILD_VCS_ID 0ca6ef79c77fad30bc7554f500046bc5dee82a81 // git or mercurial hash
#define YOTTA_BUILD_VCS_CLEAN 0 // evaluates true if the version control system was clean, otherwise false
#endif // ndef __YOTTA_BUILD_INFO_H__
