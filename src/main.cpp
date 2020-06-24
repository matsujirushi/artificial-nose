#include <Arduino.h>

/* Edge Impulse Arduino examples
 * Copyright (c) 2020 EdgeImpulse Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* Includes ---------------------------------------------------------------- */
#include <alcohol_inference.h>

#include <Multichannel_Gas_GMXXX.h>
#include <Wire.h>
GAS_GMXXX<TwoWire> *gas = new GAS_GMXXX<TwoWire>();

#include <TFT_eSPI.h>
#include "seeed_line_chart.h"
TFT_eSPI tft;
// Stock font and GFXFF reference handle
TFT_eSprite spr = TFT_eSprite(&tft); // Sprite

typedef uint32_t (GAS_GMXXX<TwoWire>::*sensorGetFn)();

typedef struct SENSOR_INFO
{
  char *name;
  char *unit;
  std::function<uint32_t()> readFn;
} SENSOR_INFO;

SENSOR_INFO sensors[4] = {
    {"NO2", "ppm", std::bind(&GAS_GMXXX<TwoWire>::getGM102B, gas)},
    {"C2H5NH", "ppm", std::bind(&GAS_GMXXX<TwoWire>::getGM302B, gas)},
    {"VOC", "ppm", std::bind(&GAS_GMXXX<TwoWire>::getGM502B, gas)},
    {"CO", "ppm", std::bind(&GAS_GMXXX<TwoWire>::getGM702B, gas)}};
#define NB_SENSORS 4

char title_text[20] = "";

#define TRAINING 0
#define INFERENCE 1
int mode = TRAINING;

#define SENSORS 0
#define INFERENCE_RESULTS 1
int screen_mode = SENSORS;

#define MAX_CHART_SIZE 50
std::vector<doubles> chart_series = std::vector<doubles>(4, doubles());

/* Private variables ------------------------------------------------------- */
static bool debug_nn = false; // Set this to true to see e.g. features generated from the raw signal

void draw_chart();

/**
* @brief      Arduino setup function
*/
void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Edge Impulse Inferencing Demo");

  pinMode(D0, OUTPUT);
  digitalWrite(D0, HIGH);

  pinMode(WIO_KEY_A, INPUT_PULLUP);
  pinMode(WIO_KEY_B, INPUT_PULLUP);
  pinMode(WIO_KEY_C, INPUT_PULLUP);

  pinMode(WIO_5S_UP, INPUT_PULLUP);
  pinMode(WIO_5S_DOWN, INPUT_PULLUP);
  pinMode(WIO_5S_LEFT, INPUT_PULLUP);
  pinMode(WIO_5S_RIGHT, INPUT_PULLUP);
  pinMode(WIO_5S_PRESS, INPUT_PULLUP);

  gas->begin(Wire, 0x08); // use the hardware I2C

  // put your setup code here, to run once:
  tft.begin();
  tft.setRotation(3);
  spr.createSprite(tft.width(), tft.height());

  if (EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME != 3)
  {
    ei_printf("ERR: EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME should be equal to 3 (the 3 sensor axes)\n");
    return;
  }
}

/**
* @brief      Printf function uses vsnprintf and output using Arduino Serial
*
* @param[in]  format     Variable argument list
*/
void ei_printf(const char *format, ...)
{
  static char print_buf[512] = {0};

  va_list args;
  va_start(args, format);
  int r = vsnprintf(print_buf, sizeof(print_buf), format, args);
  va_end(args);

  if (r > 0)
  {
    Serial.write(print_buf);
  }
}

int fan = 0;

/**
* @brief      Get data and run inferencing
*
* @param[in]  debug  Get debug info if true
*/
void loop()
{
  if (digitalRead(WIO_KEY_A) == LOW)
    digitalWrite(D0, HIGH);

  if (digitalRead(WIO_KEY_B) == LOW)
    digitalWrite(D0, LOW);

  if (digitalRead(WIO_5S_UP) == LOW)
    mode = INFERENCE;
  if (digitalRead(WIO_5S_DOWN) == LOW)
    mode = TRAINING;

  if (digitalRead(WIO_5S_LEFT) == LOW)
    screen_mode = SENSORS;
  if (digitalRead(WIO_5S_RIGHT) == LOW)
    screen_mode = INFERENCE_RESULTS;

  if (mode == TRAINING)
  {
    strcpy(title_text, "Training mode");
  }

  int val;
  int no, alcohol, co, voc;

  spr.fillSprite(TFT_BLACK);
  spr.setFreeFont(&FreeSansBoldOblique18pt7b);
  spr.setTextColor(TFT_GREEN);
  spr.drawString(title_text, 60 - 15, 10, 1);
  for (int8_t line_index = 0; line_index < 5; line_index++)
  {
    spr.drawLine(0, 50 + line_index, tft.width(), 50 + line_index, TFT_GREEN);
  }

  spr.setFreeFont(&FreeSansBoldOblique9pt7b); // Select the font
  spr.setTextColor(TFT_WHITE);

  for (int i = 0; i < NB_SENSORS; i++)
  {
    uint32_t sensorVal = sensors[i].readFn();
    if (sensorVal > 999)
    {
      sensorVal = 999;
    }

    if (chart_series[i].size() == MAX_CHART_SIZE)
    {
      chart_series[i].pop();
    }
    chart_series[i].push(sensorVal);
  }

  if (screen_mode == SENSORS || mode == TRAINING)
  {
    for (int i = 0; i < NB_SENSORS; i++)
    {
      int x_ref = 60 +  (i % 2 * 170);
      int y_ref = 100 + (i / 2 * 80);

      spr.setTextColor(TFT_WHITE);
      spr.drawString(sensors[i].name, x_ref - 24, y_ref - 24, 1);
      spr.drawRoundRect(x_ref - 24, y_ref, 80, 40, 5, TFT_WHITE);
      spr.setTextColor(TFT_WHITE);
      spr.drawNumber(chart_series[i].front(), x_ref - 20, y_ref + 10, 1);
      spr.setTextColor(TFT_GREEN);
      spr.drawString(sensors[i].unit, x_ref + 12, y_ref + 8, 1);
    }
  }

  if (mode == TRAINING)
  {
    ei_printf("%d,%d,%d,%d\n", no, alcohol, voc, co);
  }
  else
  { // INFERENCE

    ei_printf("Sampling...\n");

    // Allocate a buffer here for the values we'll read from the gas sensor
    float buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE] = {0};

    for (size_t ix = 0; ix < EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE; ix += 4)
    {
      // Determine the next tick (and then sleep later)
      uint64_t next_tick = micros() + (EI_CLASSIFIER_INTERVAL_MS * 1000);

      buffer[ix + 0] = no;
      buffer[ix + 1] = alcohol;
      buffer[ix + 2] = voc;
      buffer[ix + 3] = co;
      //        ei_printf("Sampled: %f,%f,%f,%f\n", buffer[ix + 0], buffer[ix + 1], buffer[ix + 2], buffer[ix + 3]);

      delayMicroseconds(next_tick - micros());
    }

    // Turn the raw buffer in a signal which we can the classify
    signal_t signal;
    int err = numpy::signal_from_buffer(buffer, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);
    if (err != 0)
    {
      ei_printf("Failed to create signal from buffer (%d)\n", err);
      return;
    }

    // Run the classifier
    ei_impulse_result_t result = {0};

    err = run_classifier(&signal, &result, debug_nn);
    if (err != EI_IMPULSE_OK)
    {
      ei_printf("ERR: Failed to run classifier (%d)\n", err);
      return;
    }

    // print the predictions
    size_t best_prediction = 0;
    ei_printf("Predictions (DSP: %d ms., Classification: %d ms., Anomaly: %d ms.): \n",
              result.timing.dsp, result.timing.classification, result.timing.anomaly);

    int lineNumber = 60;
    char lineBuffer[30] = "";

    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++)
    {
      if (result.classification[ix].value >= result.classification[best_prediction].value)
      {
        best_prediction = ix;
      }

      ei_printf("    %s: %.5f\n", result.classification[ix].label, result.classification[ix].value);

      if (mode == INFERENCE && screen_mode == INFERENCE_RESULTS)
      {
        sprintf(lineBuffer, "    %s: %.5f\n", result.classification[ix].label, result.classification[ix].value);
        spr.drawString(lineBuffer, 10, lineNumber, 1); // Print the test text in the custom font
        lineNumber += 20;
      }
    }

#if EI_CLASSIFIER_HAS_ANOMALY == 1
    ei_printf("    anomaly score: %.3f\n", result.anomaly);
#endif

    strcpy(title_text, result.classification[best_prediction].label);
    ei_printf("Best prediction: %s", title_text);
  }

  spr.pushSprite(0, 0);
}

#if !defined(EI_CLASSIFIER_SENSOR) || EI_CLASSIFIER_SENSOR != EI_CLASSIFIER_SENSOR_ACCELEROMETER
#error "Invalid model for current sensor"
#endif
