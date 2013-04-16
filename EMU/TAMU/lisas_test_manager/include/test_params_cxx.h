
/* This is test_params_cxx.h; 
 * It contains variables needed by both fast_ana and the individual test ???? 
 * routines.
 */

#define MAX_NUM_PASSES    5

/* Variables passed to the test function */
extern int    csc_num;
extern int    csc_type;
extern char   csc_name[12];

/* Pointers to functions called by fast_ana; these pointers are assigned
 * values by test_n_init (for test number n) 
 */
extern int    (*test_begin)(int);        /* Called before each pass through the data */
extern int    (*test_event)(int);        /* Called once per event                    */
extern int    (*test_end)(int);          /* Called after each pass through the data  */
extern int    (*test_finish)();       /* Called at the end of the test            */

extern int    num_passes;
extern int    num_events[MAX_NUM_PASSES];

