#ifndef PVSS_COMMON_H
#define PVSS_COMMON_H
int child_pid=-1;

int return_card_database_index_by_slot(int slot);
void catchterm(int signo);
int slice_test_turn_on();
#endif
