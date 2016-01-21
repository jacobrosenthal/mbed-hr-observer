#ifndef __YOTTA_BUILD_INFO_H__
#define __YOTTA_BUILD_INFO_H__
// yotta build info, #include YOTTA_BUILD_INFO_HEADER to access
#define YOTTA_BUILD_YEAR 2016 // UTC year
#define YOTTA_BUILD_MONTH 1 // UTC month 1-12
#define YOTTA_BUILD_DAY 21 // UTC day 1-31
#define YOTTA_BUILD_HOUR 7 // UTC hour 0-24
#define YOTTA_BUILD_MINUTE 32 // UTC minute 0-59
#define YOTTA_BUILD_SECOND 1 // UTC second 0-61
#define YOTTA_BUILD_UUID f848a9e2-983b-4d1f-9c89-335f0105d57e // unique random UUID for each build
#define YOTTA_BUILD_VCS_ID 61505e2fecd0d5da25154b0deaadce44fe8f8d13 // git or mercurial hash
#define YOTTA_BUILD_VCS_CLEAN 0 // evaluates true if the version control system was clean, otherwise false
#endif // ndef __YOTTA_BUILD_INFO_H__
