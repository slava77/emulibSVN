
/* This is test_params.h; 
 * It contains variables needed by both fast_ana and the individual test 
 * routines.
 */

#define MAX_NUM_PASSES    5

/* Variables passed to the test function */
int    csc_num;
int    csc_type;
char   csc_name[12];

/* Pointers to functions called by fast_ana; these pointers are assigned
 * values by test_n_init (for test number n) 
 */
int    (*test_begin)();        /* Called before each pass through the data */
int    (*test_event)();        /* Called once per event                    */
int    (*test_end)();          /* Called after each pass through the data  */
int    (*test_finish)();       /* Called at the end of the test            */

int    num_passes;
int    num_events[MAX_NUM_PASSES];

extern char*  datadir_translation;

