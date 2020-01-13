
/****************************************************************************
 *
 *   Copyright (C) 2015 Mark Charlebois. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file hello_example.cpp
 * Example for Linux
 *
 * @author Mark Charlebois <charlebm@gmail.com>
 */

#include "hello_example.h"
#include "hg_temp.h"
#include <drivers/drv_hrt.h>
#include <uORB/uORB.h>
#include <uORB/topics/sensor_hg_mlx90614.h>
#include <px4_platform_common/time.h>
#include <px4_platform_common/log.h>
#include <unistd.h>
#include <stdio.h>

px4::AppState HelloExample::appState;

int HelloExample::main()
{
	HG_Temp temp;

	appState.setRunning(true);

	/* advertise sensor_hg_mlx90614 topic */
	struct sensor_hg_mlx90614_s temp_sensor;
	memset(&temp_sensor, 0, sizeof(temp_sensor));
	orb_advert_t temp_sensor_pub_fd = orb_advertise(ORB_ID(sensor_hg_mlx90614), &temp_sensor);

	// wait for sensor data to be ready
	px4_sleep(1);

	while (!appState.exitRequested()) {
		uint64_t timestamp_us = hrt_absolute_time();
		double ambient_temp = temp.readAmbientTempC();
		double object_temp = temp.readObjectTempC();

		// print temperatures
		printf("Ambient %+2.2f | Object %+2.2f\n", ambient_temp, object_temp);

		// publish temperatures
		temp_sensor.timestamp = timestamp_us;
		temp_sensor.ambient_temp = ambient_temp;
		temp_sensor.object_temp = object_temp;
		orb_publish(ORB_ID(sensor_hg_mlx90614), temp_sensor_pub_fd, &temp_sensor);

		px4_sleep(2);
	}

	appState.appExiting();

	return 0;
}
