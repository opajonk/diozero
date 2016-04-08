package com.diozero.api.imu;

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


import org.apache.commons.math3.geometry.euclidean.threed.Vector3D;

import com.diozero.util.RuntimeIOException;

public interface ImuInterface {
	
	String getImuName();
	/**
	 * Get the recommended poll interval in milliseconds
	 * @return The poll interval
	 */
	int getPollInterval();
	
	boolean hasGyro();
	boolean hasAccelerometer();
	boolean hasCompass();
	
	void startRead();
	void stopRead();
	
	ImuData getImuData() throws RuntimeIOException;
	Vector3D getGyroData() throws RuntimeIOException;
	Vector3D getAccelerometerData() throws RuntimeIOException;
	Vector3D getCompassData() throws RuntimeIOException;
	
	void addTapListener(TapListener listener);
	void addOrientationListener(OrientationListener listener);
}