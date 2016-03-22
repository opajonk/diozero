package com.diozero.internal.provider.test;

/*
 * #%L
 * Device I/O Zero - Core
 * %%
 * Copyright (C) 2016 mattjlewis
 * %%
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
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * #L%
 */


import java.io.IOException;

import org.pmw.tinylog.Logger;

import com.diozero.internal.spi.AbstractDevice;
import com.diozero.internal.spi.DeviceFactoryInterface;
import com.diozero.internal.spi.GpioDigitalOutputDeviceInterface;
import com.diozero.util.DioZeroScheduler;
import com.diozero.util.RuntimeIOException;
import com.diozero.util.SleepUtil;

public class HCSR04TriggerPin extends AbstractDevice
implements GpioDigitalOutputDeviceInterface, Runnable {
	private int pinNumber;
	private boolean value;
	private long start;
	
	public HCSR04TriggerPin(String key, DeviceFactoryInterface deviceFactory,
			int pinNumber, boolean initialValue) {
		super(key, deviceFactory);
		
		this.pinNumber = pinNumber;
		this.value = initialValue;
	}
	
	@Override
	public int getPin() {
		return pinNumber;
	}

	@Override
	public boolean getValue() throws RuntimeIOException {
		return value;
	}
	
	@Override
	public void setValue(boolean value) throws RuntimeIOException {
		boolean old_value = this.value;
		this.value = value;

		// Start the signal echo process if the trigger pin goes high then low
		if (old_value && ! value) {
			start = System.currentTimeMillis();
			DioZeroScheduler.getDaemonInstance().execute(this);
		}
	}
	
	@Override
	public void run() {
		Logger.debug("run()");
		SleepUtil.sleepMillis(50);
		HCSR04EchoPin.getInstance().doEcho(start);
	}

	@Override
	protected void closeDevice() throws IOException {
	}
}
