#include "TF1.h" // unary functions
#include "TH1.h" // histograms
#include "TRandom.h" // pseudo-random number geneartor
#include "TMath.h" // for the error function
#include "TBox.h" // for building graphical boxes
#include "TFile.h" // for saving to files
#include "TMarker.h" // for drawing overflow arrows
#include "TCanvas.h" // for finer control of drawing
#include "TAxis.h" // manipulate title and color of histograms axes
#include "TVirtualPad.h" // get a pointer to the sub-pads of the canvas
#include "TStyle.h" // add underflows and overflows to stat box via gStyle
#include "TLine.h" // drawing limit lines on histograms via TLines
#include "TPaveText.h" // displaying a summary text box
#include <vector>
#include <algorithm> // for for_each over vectors
#include <iostream> // for cout.get(), to pause before ending
#include <sstream> // ostringstream i.e. string formatting
#include <ctime> // the date of analysis is printed with the histograms
#include <cmath> // taking the ceiling of a value

using namespace std;

void one_page_plot(int test_number, string test_name, string sub_test_name,
                   float y_min, float y_max, string summary_title,
                   string chamber_type, float lower_limit, float upper_limit,
                   int bins, int febs_per_layer, int layers, const vector<vector<float > > data,
                   string output_filename);

template<class T>
void delete_vector_of_pointers(vector<T *> * v);
template<class T>
void delete_vector_of_vectors_of_pointers(vector<vector<T *> * > * v);
TPaveText * create_information_box(string chamber_type, int test_number,
                                   string test_name, string sub_test_name,
                                   int number_of_out_of_limit_points);
TH1F * create_summary_histogram_and_count_out_of_limits(vector<TH1F *> * histograms, string title, float y_min, float y_max,
							float lower_limit, float upper_limit, int& number_of_out_of_limits_points);
vector<TH1F * > * create_histograms(int layers, int bins);
TH1F * create_histogram(int n, int bins);

template<class T, class Function>
void vector_for_each(vector<T> * ts, Function f);

template<class IN, class OUT, class UnaryOperator>
void vector_map(vector<IN> * inputs, vector<OUT> * outputs,  UnaryOperator f);

void draw_all_vector_members(vector<TObject *> * drawables);
void configure_style_of_histograms(vector<TH1F *> * histograms, float y_min, float y_max, int bins, int febs);
void configure_style_of_histogram(TH1F * histogram, float y_min, float y_max, int bins, int febs);
void fill_histograms(vector<TH1F *> * histogram, const vector<vector<float > > data, int bins, int layers);
void fill_histogram(TH1F * histogram, const vector<vector<float > > data, int bins, int layer);
TLine * create_limit_line(float x1, float y1, float x1, float x2);
vector<vector<TObject * > * > * create_layer_limit_lines(vector<TH1F * > * histograms, float high, float low);
vector<TObject * > * create_summary_limit_lines(TH1F * summary_histogram, float high, float low);
vector<TObject *> * mark_overflows(TH1F * histogram);
