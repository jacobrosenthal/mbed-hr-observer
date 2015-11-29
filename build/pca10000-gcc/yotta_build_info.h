#ifndef __YOTTA_BUILD_INFO_H__
#define __YOTTA_BUILD_INFO_H__
// yotta build info, #include YOTTA_BUILD_INFO_HEADER to access
#define YOTTA_BUILD_YEAR 2015 // UTC year
#define YOTTA_BUILD_MONTH 11 // UTC month 1-12
#define YOTTA_BUILD_DAY 29 // UTC day 1-31
#define YOTTA_BUILD_HOUR 13 // UTC hour 0-24
#define YOTTA_BUILD_MINUTE 15 // UTC minute 0-59
#define YOTTA_BUILD_SECOND 18 // UTC second 0-61
#define YOTTA_BUILD_UUID 81e8daee-c8e4-4726-8c68-65fe05f3504f // unique random UUID for each build
#define YOTTA_BUILD_VCS_ID 0af14641be8e9e9740f318f5246bc6519be46934 // git or mercurial hash
#define YOTTA_BUILD_VCS_CLEAN 0 // evaluates true if the version control system was clean, otherwise false
#endif // ndef __YOTTA_BUILD_INFO_H__
