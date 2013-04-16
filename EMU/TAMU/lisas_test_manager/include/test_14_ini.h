
// Delay offset
#ifdef UCLA
#define  DELAY_OFFSET              0  // this offset is using only for plots
#define FAST_SITE "UCLA"
#endif

#ifdef UF
#define  DELAY_OFFSET              60  // this offset is using only for plots
#define FAST_SITE "UF"
#endif

#ifndef FAST_SITE
#define  DELAY_OFFSET              0  // this offset is using only for plots
#define FAST_SITE "Other"
#endif

#define ACTIVETBINS_LIMIT           3 // Reject After Pulsing
