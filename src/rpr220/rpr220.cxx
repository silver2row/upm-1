/*
 * Author: Jon Trulson <jtrulson@ics.com>
 * Copyright (c) 2015 Intel Corporation.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <iostream>
#include <string>
#include <stdexcept>

#include "rpr220.hpp"

using namespace upm;
using namespace std;

RPR220::RPR220(int pin)
{
  m_isrInstalled = false;

  if ( !(m_gpio = mraa_gpio_init(pin)) )
   {
      throw std::invalid_argument(std::string(__FUNCTION__) +
                                  ": mraa_gpio_init() failed, invalid pin?");
      return;
    }

  mraa_gpio_dir(m_gpio, MRAA_GPIO_IN);
}

RPR220::~RPR220()
{
  if (m_isrInstalled)
    uninstallISR();

  mraa_gpio_close(m_gpio);
}

bool RPR220::blackDetected()
{
  return (mraa_gpio_read(m_gpio) ? true : false);
}

#ifdef JAVACALLBACK
void RPR220::installISR(jobject runnable)
{
  installISR(mraa_java_isr_callback, runnable);
}
#endif

void RPR220::installISR(void (*isr)(void *), void *arg)
{
  if (m_isrInstalled)
    uninstallISR();

  // install our interrupt handler
  mraa_gpio_isr(m_gpio, MRAA_GPIO_EDGE_RISING, 
                isr, arg);
  m_isrInstalled = true;
}

void RPR220::uninstallISR()
{
  mraa_gpio_isr_exit(m_gpio);
  m_isrInstalled = false;
}

