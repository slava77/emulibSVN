
/* Function declarations */
void get_tpatterns(void);
int tcode(int distrip, int nlayers, int code);
int hs(int tcode);
int nl(int tcode);
int so(int tcode, int layer);
int pretrig_alg(int algorithm, int t, short int *active_lct);
void trig_alg(int algorithm, short int actvie_lct, int t, short int *key, 
 short int *pattern, short int *valid);
