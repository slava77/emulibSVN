#include "one-page-plot.h"
#include <stdio.h>

#define MARKER_SIZE 0.25
#define MARKER_Y_SHIFT 0.1

void one_page_plot(int test_number, string test_name, string sub_test_name,
                   float y_min, float y_max, string summary_title,
                   string chamber_type, float lower_limit, float upper_limit,
                   int bins, int febs_per_layer, int layers, const vector<vector<float > > data,
                   string output_filename) {

  TCanvas canvas("canvas_name", "canvas title");
  vector<TH1F *> * histograms = new vector<TH1F *>();

  // Split the canvas in two, one 2/3's of the width.
  TPad layer_histograms_pad("layer_histograms_pad",
			    "Layer Histograms Pad",
			    0, 0, .66, 1);
  TPad summary_pad("summary_pad",
		   "Summary Histogram Pad",
		   .66, 0, 1, 1);

  layer_histograms_pad.SetRightMargin(0);
  summary_pad.SetLeftMargin(0.05);

  canvas.cd(0);
  // add the histograms to the current pad, which the previous line set to the
  // canvas
  layer_histograms_pad.Draw();
  summary_pad.Draw();

  // break the layer side into six horizontal sections
  layer_histograms_pad.Divide(1, layers, 0, 0);

  layer_histograms_pad.GetPad(layers)->SetBottomMargin(.3);

  histograms = create_histograms(layers, bins);
  fill_histograms(histograms, data, bins, layers);
  configure_style_of_histograms(histograms, y_min, y_max, bins, febs_per_layer);

  vector<vector<TObject *> * > * overflow_markers_vector = new vector<vector<TObject *> * >();
  // this produces a vector of drawable overflow marks for each histogram,
  // thus producing a vector of vector of drawable overflow marks
  vector_map(histograms, overflow_markers_vector, mark_overflows);

  vector<vector<TObject *> * > * layer_limit_lines = create_layer_limit_lines(histograms, lower_limit, upper_limit);

  // synchronously loop over the histograms and the overflow markers
  for(int i = 0; i < histograms->size() && i < overflow_markers_vector->size(); ++i) {
    // [1, size] are the sub-pads we want, 0 is the containing pad
    layer_histograms_pad.cd(i + 1);
    gPad->SetGridx(1);
    gPad->SetRightMargin(.05);

    // draw the histograms and the markers on the current (i.e. (i+1)th) pad
    (*histograms)[i]->Draw("P");
    draw_all_vector_members((*overflow_markers_vector)[i]);
    draw_all_vector_members((*layer_limit_lines)[i]);
  }

  int number_of_out_of_limits_points;

  TH1F * summary_histogram = create_summary_histogram_and_count_out_of_limits(histograms, summary_title, y_min, y_max,
									      lower_limit, upper_limit, number_of_out_of_limits_points);
  vector<TObject *> * summary_limit_lines = create_summary_limit_lines(summary_histogram, lower_limit, upper_limit);

  summary_pad.Divide(1, 2, 0.05, 0.02);
  TVirtualPad & summary_histogram_pad = * summary_pad.GetPad(1);
  TVirtualPad & summary_text_pad = * summary_pad.GetPad(2);

  summary_histogram_pad.cd(0);
  summary_histogram->Draw();
  draw_all_vector_members(summary_limit_lines);

  int overflowbin = summary_histogram->GetNbinsX() + 1;
  float * summary_bins = summary_histogram->GetArray();

  TPaveText * information_box = create_information_box(chamber_type,
                                                       test_number,
                                                       test_name,
                                                       sub_test_name,
                                                       number_of_out_of_limits_points);
  summary_text_pad.cd(0);
  information_box->Draw();

  // turn on underflows and overflows
  gStyle->SetOptStat(111111);

  canvas.Print(output_filename.c_str());

  // return to default stat box style
  gStyle->SetOptStat(1111);

  delete information_box;
  delete summary_histogram;
  delete_vector_of_pointers(histograms);
  delete_vector_of_vectors_of_pointers(overflow_markers_vector);
  delete_vector_of_vectors_of_pointers(layer_limit_lines);
  delete_vector_of_pointers(summary_limit_lines);

  return;
}

template<class T>
void delete_vector_of_pointers(vector<T *> * v) {
  for(typename vector<T *>::iterator it = v->begin(); it != v->end(); ++it) {
    delete (*it);
  }

  delete v;
}

template<class T>
void delete_vector_of_vectors_of_pointers(vector<vector<T *> * > * v) {
  for(typename vector<vector<T *> *>::iterator child = v->begin(); child != v->end(); ++child) {
    for(typename vector<T *>::iterator it = (*child)->begin(); it != (*child)->end(); ++it) {
      delete (*it);
    }
    delete (*child);
  }

  delete v;
}

TPaveText * create_information_box(string chamber_type, int test_number,
                                   string test_name, string sub_test_name,
                                   int number_of_out_of_limit_points) {
  TPaveText * information_box = new TPaveText(0, 0, 1, 1, "NDC");

  ostringstream formatted_test_number;
  formatted_test_number << "Test " << test_number;

  ostringstream points_out_of_limits;
  points_out_of_limits << "Points out of limits: " << number_of_out_of_limit_points;

  time_t t = time(0);   // get time now
  struct tm * now = localtime( & t );
  ostringstream analysis_date;
  analysis_date << "Analyzed on "
                << now->tm_mday << "/" << (now->tm_mon + 1)
                << "/" << (now->tm_year + 1900)
                << " "
                << now->tm_hour << ":" << now->tm_min << ":" << now->tm_sec;

  information_box->AddText(test_name.c_str());
  information_box->AddText(chamber_type.c_str());
  information_box->AddText(formatted_test_number.str().c_str());
  information_box->AddText(sub_test_name.c_str());
  information_box->AddText(points_out_of_limits.str().c_str());
  information_box->AddText(analysis_date.str().c_str());

  information_box->SetTextAlign(11);
  information_box->SetTextFont(42);
  information_box->SetTextSize(0.07);

  return information_box;
}

TH1F * create_summary_histogram_and_count_out_of_limits(vector<TH1F *> * histograms, string title, float y_min, float y_max,
							float lower_limit, float upper_limit, int& number_of_out_of_limits_points) {
  TH1F * summary_histogram = new TH1F("summary_histogram",
                                      title.c_str(),
                                      40,
                                      y_min,
                                      y_max);

  for(std::vector<TH1F *>::iterator it = histograms->begin();
      it != histograms->end();
      ++it) {
    TH1F * histogram = *it;

    Float_t * bins = histogram->GetArray();
    int length = histogram->GetNbinsX();

    for(int i = 1; i <= length; ++i) {
      summary_histogram->Fill(bins[i]);
      if (bins[i] < lower_limit || bins[i] > upper_limit) {
	number_of_out_of_limits_points++;
      }
    }
  }

  summary_histogram->StatOverflows(kTRUE);

  return summary_histogram;
}

vector<TH1F * > * create_histograms(int layers, int bins) {
  vector<TH1F * > * histograms = new vector<TH1F * >();

  for(int i = 1; i <= layers; ++i) {
    histograms->push_back(create_histogram(i, bins));
  }

  return histograms;
}

TH1F * create_histogram(int n, int bins) {
  ostringstream name;
  ostringstream title;

  name << "hist" << n;
  // titles are in the format
  // "histogram-title;x-axis-title;y-axis-title;z-axis-title"
  // The spaces are a hack to center the layer because I don't know how to set
  // the vertical offset of the y-axis title
  title << ";;Layer " << n << "     ";

  return new TH1F(name.str().c_str(),
                  title.str().c_str(),
                  bins,
                  0,
                  bins);
}

template<class T, class Function>
void vector_for_each(vector<T> * ts, Function f) {
  for_each(ts->begin(), ts->end(), f);
}

// here we use "map" in the functional sense, see
// http://c2.com/cgi/wiki?MapFunction
// we call f on every element of the inputs and store each result in outputs
template<class IN, class OUT, class UnaryOperator>
void vector_map(vector<IN> * inputs, vector<OUT> * outputs,  UnaryOperator f) {
  outputs->resize(inputs->size());
  transform (inputs->begin(), inputs->end(), outputs->begin(), f);
}

void draw_all_vector_members(vector<TObject *> * drawables) {
  for(std::vector<TObject *>::iterator it = drawables->begin();
      it != drawables->end();
      ++it) {
    (*it)->Draw();
  }
}

void configure_style_of_histograms(vector<TH1F *> * histograms, float y_min, float y_max, int bins, int febs_per_layer) {
  for(int i = 0; i < histograms->size(); ++i) {
    configure_style_of_histogram((*histograms)[i], y_min, y_max, bins, febs_per_layer);
  }
}

void configure_style_of_histogram(TH1F * histogram, float y_min, float y_max, int bins, int febs_per_layer) {
  histogram->SetMaximum(y_max);
  histogram->SetMinimum(y_min);

  histogram->SetMarkerStyle(8);
  histogram->SetMarkerSize(MARKER_SIZE);
  histogram->SetMarkerColor(kBlack);

  histogram->SetStats(false);

  TAxis * x_axis = histogram->GetXaxis();
  TAxis * y_axis = histogram->GetYaxis();

  x_axis->SetLabelSize(0.20);
  x_axis->SetNdivisions(febs_per_layer, bins/febs_per_layer, 0, false);
  x_axis->SetTickLength(0.1);

  x_axis->SetLabelOffset(0.05);

  y_axis->SetLabelSize(0.10);
  y_axis->SetNdivisions(6, 2, 0, true);
  y_axis->SetTickLength(0.01);

  y_axis->SetTitleOffset(0.3);
  y_axis->SetTitleSize(0.15);
}

void fill_histograms(vector<TH1F *> * histograms, const vector<vector<float > > data, int bins, int layers) {
  for(int i = 0; i < histograms->size(); ++i) {
    fill_histogram((*histograms)[i], data, bins, i);
  }
}

void fill_histogram(TH1F * histogram, const vector<vector<float > > data, int bins, int layer) {
  for(int bin = 0; bin < bins; ++bin) {
    histogram->Fill(bin, data[layer][bin]);
  }
}

TLine * create_limit_line(float x1, float y1, float x2, float y2) {
  TLine * line = new TLine(x1, y1, x2, y2);
  line->SetLineStyle(7);
  return line;
}

vector<vector<TObject * > * > * create_layer_limit_lines(vector<TH1F * > * histograms, float high, float low) {
  vector<vector<TObject * > * > * all_limit_lines = new vector<vector<TObject *> *>();

  for(vector<TH1F *>::iterator it = histograms->begin();
      it != histograms->end();
      ++it) {
    TH1F * histogram = *it;
    Double_t x1 = histogram->GetXaxis()->GetXmin();
    Double_t x2 = histogram->GetXaxis()->GetXmax();

    TLine * upper_limit_line = create_limit_line(x1, high, x2, high);
    TLine * lower_limit_line = create_limit_line(x1, low, x2, low);

    vector<TObject * > * lines_for_one_histogram = new vector<TObject * >();

    lines_for_one_histogram->push_back(upper_limit_line);
    lines_for_one_histogram->push_back(lower_limit_line);
    all_limit_lines->push_back(lines_for_one_histogram);
  }

  return all_limit_lines;
}

vector<TObject * > * create_summary_limit_lines(TH1F * summary_histogram, float high, float low) {
  Double_t y1 = summary_histogram->GetMinimum() * 1.05;
  Double_t y2 = summary_histogram->GetMaximum() * 1.05;

  TLine * upper_limit_line = create_limit_line(high, 0, high, y2);
  TLine * lower_limit_line = create_limit_line(low, y1, low, y2);

  vector<TObject * > * limit_lines = new vector<TObject *>();

  limit_lines->push_back(upper_limit_line);
  limit_lines->push_back(lower_limit_line);

  return limit_lines;
}

vector<TObject *> * mark_overflows(TH1F * histogram) {
  Double_t maximum_value = histogram->GetMaximum();
  Double_t minimum_value = histogram->GetMinimum();

  vector<TObject *> * overflow_markers = new vector<TObject *>();

  Float_t * bins = histogram->GetArray();
  Int_t number_of_bins = histogram->GetNbinsX();

  // note that the bins array has the following structure:
  // bins[0] is the underflow bin
  // bins[1] through bins[number_of_bins] are the actual bins
  // bins[number_of_bins + 1] is the overflow bin
  for(int j = 1; j <= number_of_bins; j++) {
    if (bins[j] > maximum_value || bins[j] < minimum_value) {
      Double_t x1 = histogram->GetXaxis()->GetBinLowEdge(j);
      Double_t x2 = histogram->GetXaxis()->GetBinUpEdge(j);

      TMarker * m = new TMarker();
      m->SetX((x2 + x1) / 2.0);
      m->SetMarkerColor(kRed);
      m->SetMarkerSize(MARKER_SIZE * 2);

      if(bins[j] > maximum_value) {
        m->SetY(maximum_value - MARKER_Y_SHIFT);
        m->SetMarkerStyle(22); // up arrow
      } else {
        m->SetY(minimum_value + MARKER_Y_SHIFT);
        m->SetMarkerStyle(23); // down arrow
      }

      overflow_markers->push_back(m);
    }
  }

  return overflow_markers;
}
