// Common libs
#uses "CMS_CSC_common/emu_common.ctl"
#uses "CMS_CSC_common/emu_math.ctl"

// UI libs
#uses "CMS_CSC_UI/emuui_main.ctl"
#uses "CMS_CSC_UI/emuui_configuration.ctl"
#uses "CMS_CSC_UI/emuui_sessionManager.ctl"
#uses "CMS_CSC_UI/emuui_utils.ctl"
#uses "CMS_CSC_UI/emuui_deviceInfo.ctl"

const int MAX_COLOR_INTENSITY = 255;

string emuui_getGradientColor(float value, float minValue, float maxValue) {
  //range of valid values
  float range = maxValue - minValue;
  dyn_float color; // color components (r, g, b), top value is 2, later cut at 1.
  dyn_int intColor;
  string strColor;

  if (value < minValue) { value = minValue; }
  if (value > maxValue) { value = maxValue; }
    
  value -= minValue;             // move value to start at 0
  value = (value / range) * 4;   // shrink the scale to [0;4]
  value -= 2;                    // transfer to left by 2 so that we have a range [-2;2]
  
  color[1] = value;              // red is highest at the end of the range (2) and decreesing torwards the middle (0).
  color[2] = 2 - emu_abs(value); // green is highest in the middle of the range (0) and decreesing both ways (-2 & 2).
  color[3] = value * -1;         // blue is highest at the start of the range (-2) and decreesing torwards the middle (0).
  
  // apply a cut on the range [0-1] & scale to [0;255]
  for (int i=1; i <= 3; i++) {
    // apply the cut
    if (color[i] > 1) {
      color[i] = 1;
    } else if (color[i] < 0) {
      color[i] = 0;
    }
    
    //scale
    intColor[i] = floor(color[i] * 255);
  }
  
  strColor = "{" + intColor[1] + ";" + intColor[2] + ";" + intColor[3] + "}";
  return strColor;
}
