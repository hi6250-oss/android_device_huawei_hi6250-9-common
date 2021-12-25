/*
 * Copyright 2013 The Android Open Source Project
 * Copyright 2016 Kostyan_nsk
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#define LOG_NDEBUG 0
#define LOG_TAG "lights"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>

#include <log/log.h>
#include <hardware/lights.h>


#define LCD_FILE		"/sys/class/leds/lcd_backlight0/brightness"

#define RED_LED_FILE		"/sys/class/leds/red/brightness"
#define RED_DELAY_ON_FILE	"/sys/class/leds/red/delay_on"
#define RED_DELAY_OFF_FILE	"/sys/class/leds/red/delay_off"

#define GREEN_LED_FILE		"/sys/class/leds/green/brightness"
#define GREEN_DELAY_ON_FILE	"/sys/class/leds/green/delay_on"
#define GREEN_DELAY_OFF_FILE	"/sys/class/leds/green/delay_off"

#ifndef NO_BLUE_LED
#define BLUE_LED_FILE		"/sys/class/leds/blue/brightness"
#define BLUE_DELAY_ON_FILE	"/sys/class/leds/blue/delay_on"
#define BLUE_DELAY_OFF_FILE	"/sys/class/leds/blue/delay_off"
#endif

enum lights {
	UNDEFINED = -1,
	BATTERY,
	NOTIFICATIONS,
	ATTENTION,
};

enum led_color {
	RED,
	GREEN,
	BLUE,
};

struct led_config {
	uint32_t colorRGB;
	uint32_t delay_on;
	uint32_t delay_off;
};

struct thread_data {
	bool signaled;
	const struct led_config *led;
	pthread_mutex_t lock;
	enum led_color color;
};

static struct led_config g_leds[3];	/* For battery, notifications and attention. */
static int g_cur_light = UNDEFINED;	/* Presently showing LED of the above. */
static bool initialized = false;
static pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t g_cond = PTHREAD_COND_INITIALIZER;
static pthread_barrier_t g_barr_on, g_barr_off;
static struct thread_data g_data[3];
static pthread_t g_threads[3];

static int write_int(const char *path, int value) {

	char buffer[8];

	int fd = open(path, O_WRONLY);
	if (fd >= 0) {
		int len = sprintf(buffer, "%d", value);
		int ret = write(fd, buffer, len);
		close(fd);
		if (ret < 0)
			ALOGE("%s: failed to write value = %d into file: %s, errno = %d (%s)",
						__func__, value, path, errno, strerror(errno));
		return ret < 0 ? -errno : 0;
	}
	else {
		ALOGE("%s: failed to open '%s' (%s)", __func__, path, strerror(errno));
		return -errno;
	}
}

static inline int rgb_to_brightness(struct light_state_t const* state) {

	int color = state->color & 0x00ffffff;

	return ((77 * ((color >> 16) & 0x00ff))
		+ (150 * ((color >> 8) & 0x00ff))
		+ (29 * (color & 0x00ff))) >> 8;
}

static inline uint8_t get_color(uint32_t colorRGB, enum led_color color) {

	uint8_t value = (colorRGB >> (16 - 8 * color)) & 0xFF;
/*
	if (value)
	    value = value > 127 ? 255 : 127;
*/
	return value;
}

static int set_light_backlight(struct light_device_t *dev,
			const struct light_state_t *state)
{
	int err = 0;
	int brightness = rgb_to_brightness(state);

	pthread_mutex_lock(&g_lock);
	err = write_int(LCD_FILE, brightness);
	pthread_mutex_unlock(&g_lock);

	return err;
}

static void *led_thread(void *arg) {

	struct thread_data *data = (struct thread_data *)arg;

	while (true) {
		pthread_mutex_lock(&data->lock);
		while (!data->signaled)
			pthread_cond_wait(&g_cond, &data->lock);
		pthread_mutex_unlock(&data->lock);

		switch(data->color) {
		    case RED: {
			uint8_t red = get_color(data->led->colorRGB, RED);

			write_int(RED_LED_FILE, red);
			pthread_barrier_wait(&g_barr_on);
			write_int(RED_DELAY_ON_FILE, data->led->delay_on);
			pthread_barrier_wait(&g_barr_off);
			write_int(RED_DELAY_OFF_FILE, data->led->delay_off);
			break;
		    }
		    case GREEN: {
			uint8_t green = get_color(data->led->colorRGB, GREEN);
#ifdef NO_BLUE_LED
			uint8_t blue = get_color(data->led->colorRGB, BLUE);
			if (green || !blue) {
#endif
				write_int(GREEN_LED_FILE, green);
				pthread_barrier_wait(&g_barr_on);
				write_int(GREEN_DELAY_ON_FILE, data->led->delay_on);
				pthread_barrier_wait(&g_barr_off);
				write_int(GREEN_DELAY_OFF_FILE, data->led->delay_off);
#ifdef NO_BLUE_LED
			}
			else { /* (!green && blue) */
				pthread_barrier_wait(&g_barr_on);
				pthread_barrier_wait(&g_barr_off);
			}
#endif
			break;
		    }
		    case BLUE: {
			uint8_t blue = get_color(data->led->colorRGB, BLUE);
#ifndef NO_BLUE_LED
			write_int(BLUE_LED_FILE, blue);
			pthread_barrier_wait(&g_barr_on);
			write_int(BLUE_DELAY_ON_FILE, data->led->delay_on);
			pthread_barrier_wait(&g_barr_off);
			write_int(BLUE_DELAY_OFF_FILE, data->led->delay_off);
#else
			uint8_t green = get_color(data->led->colorRGB, GREEN);
			if (blue && !green) {
				write_int(GREEN_LED_FILE, blue);
				pthread_barrier_wait(&g_barr_on);
				write_int(GREEN_DELAY_ON_FILE, data->led->delay_on);
				pthread_barrier_wait(&g_barr_off);
				write_int(GREEN_DELAY_OFF_FILE, data->led->delay_off);
			}
			else {
				pthread_barrier_wait(&g_barr_on);
				pthread_barrier_wait(&g_barr_off);
			}
#endif
			break;
		    }
		}
		data->signaled = false;
	}

	return NULL;
}

static void reset_leds(void) {

	write_int(RED_LED_FILE, 0);
	write_int(RED_DELAY_ON_FILE, 0);
	write_int(RED_DELAY_OFF_FILE, 0);
	write_int(GREEN_LED_FILE, 0);
	write_int(GREEN_DELAY_ON_FILE, 0);
	write_int(GREEN_DELAY_OFF_FILE, 0);
#ifndef NO_BLUE_LED
	write_int(BLUE_LED_FILE, 0);
	write_int(BLUE_DELAY_ON_FILE, 0);
	write_int(BLUE_DELAY_OFF_FILE, 0);
#endif
}

static void write_leds_locked(struct led_config *led) {

	static struct led_config led_off = {0};
	static bool blinking = false;
	enum led_color color;

	if (!led) {
		reset_leds();
		blinking = false;
		return;
	}

	/* If leds were blinking before, we have to reset them first */
	if (blinking)
		reset_leds();
	blinking = led->delay_on || led->delay_off;

	for (color = RED; color <= BLUE; color++) {
		g_data[color].led = led;
		g_data[color].signaled = true;
	}
	pthread_cond_broadcast(&g_cond);
}

static void set_light_locked(const struct light_state_t *state, enum lights light_id) {

	struct led_config *led;

	/* type is one of:
	 *   0. battery
	 *   1. notifications
	 *   2. attention
	 * which are multiplexed onto the same physical LED in the above order. */
	led = &g_leds[light_id];

	switch (state->flashMode) {
	    case LIGHT_FLASH_TIMED:
	    case LIGHT_FLASH_HARDWARE:
		led->delay_on = state->flashOnMS;
		led->delay_off = state->flashOffMS;
		break;
	    case LIGHT_FLASH_NONE:
	    default:
		led->delay_on = 0;
		led->delay_off = 0;
	    break;
	}

	ALOGV("%s: mode = %d, color = %08X, delay_on = %u, delay_off = %u, type = %d, cur_type = %d",
		__func__, state->flashMode, state->color, led->delay_on, led->delay_off, light_id, g_cur_light);

	led->colorRGB = state->color & 0x00ffffff;

	if (led->colorRGB > 0) {
		/* This LED is lit. */
		if (light_id >= g_cur_light) {
			/* And it has the highest priority, so show it. */
			write_leds_locked(led);
			g_cur_light = light_id;
		}
	}
	else {
		/* This LED is not (any longer) lit. */
		if (light_id == g_cur_light) {
			/* But it is currently showing, switch to a lower-priority LED. */
			int i;
			for (i = g_cur_light - 1; i >= 0; i--) {
				if (g_leds[i].colorRGB > 0) {
					/* Found a lower-priority LED to switch to. */
					write_leds_locked(&g_leds[i]);
					goto switched;
				}
			}
			/* No LEDs are lit, turn off. */
			write_leds_locked(NULL);
switched:
			g_cur_light = i;
		}
	}
}

static int set_light_battery(struct light_device_t *dev,
				const struct light_state_t *state)
{
	pthread_mutex_lock(&g_lock);
	set_light_locked(state, BATTERY);
	pthread_mutex_unlock(&g_lock);

	return 0;
}

static int set_light_notifications(struct light_device_t *dev,
				const struct light_state_t *state)
{
	pthread_mutex_lock(&g_lock);
	set_light_locked(state, NOTIFICATIONS);
	pthread_mutex_unlock(&g_lock);

	return 0;
}

static int set_light_attention(struct light_device_t *dev,
				const struct light_state_t *state)
{
	struct light_state_t fixed;

	pthread_mutex_lock(&g_lock);
	fixed = *state;
	/* PowerManagerService::setAttentionLightInternal turns off
	 * the attention light by setting flashOnMS = flashOffMS = 0
	 */
	if (fixed.flashOnMS == 0 || fixed.flashOffMS == 0)
		fixed.color = 0;
	set_light_locked(&fixed, ATTENTION);
	pthread_mutex_unlock(&g_lock);

	return 0;
}

static int close_lights(struct light_device_t *dev) {

	uint8_t i;

	for (i = 0; i < 3; i++) {
		pthread_kill(g_threads[i], SIGTERM);
		pthread_join(g_threads[i], NULL);
	}

	if (dev)
		free(dev);

	return 0;
}

static int open_lights(const struct hw_module_t* module, const char *name,
						struct hw_device_t** device)
{
	struct light_device_t *dev;
	int (*set_light)(struct light_device_t *dev, const struct light_state_t *state);
	enum led_color color;
	int err;

	pthread_mutex_lock(&g_lock);
	if (!initialized) {
		err = pthread_barrier_init(&g_barr_on, NULL, 3);
		if (err)
			goto error;
		err = pthread_barrier_init(&g_barr_off, NULL, 3);
		if (err)
error:
		{
			ALOGE("%s: failed to init barrier", __func__);
			pthread_mutex_unlock(&g_lock);
			return -err;
		}

		for (color = RED; color <= BLUE; color++) {
			g_data[color].signaled = false;
			g_data[color].color = color;
			err = pthread_mutex_init(&g_data[color].lock, NULL);
			if (err) {
				ALOGE("%s: failed to init mutex", __func__);
				pthread_mutex_unlock(&g_lock);
				return -err;
			}
			err = pthread_create(&g_threads[color], NULL, led_thread, &g_data[color]);
			if (err) {
				ALOGE("%s: failed to create thread", __func__);
				pthread_mutex_unlock(&g_lock);
				return -err;
			}
		}
		initialized = true;
		ALOGI("Lights HAL for hi6620oem initialized");
	}
	pthread_mutex_unlock(&g_lock);

	if (!strcmp(LIGHT_ID_BACKLIGHT, name))
		set_light = set_light_backlight;
	else
	    if (!strcmp(LIGHT_ID_NOTIFICATIONS, name))
		set_light = set_light_notifications;
	    else
		if (!strcmp(LIGHT_ID_ATTENTION, name))
		    set_light = set_light_attention;
		else
		    if (!strcmp(LIGHT_ID_BATTERY, name))
			set_light = set_light_battery;
		    else
			return -EINVAL;

	dev = (struct light_device_t *)malloc(sizeof(struct light_device_t));
	memset(dev, 0, sizeof(*dev));

	dev->common.tag = HARDWARE_DEVICE_TAG;
	dev->common.version = 0;
	dev->common.module = (struct hw_module_t*)module;
	dev->common.close = (int (*)(struct hw_device_t*))close_lights;
	dev->set_light = set_light;

	*device = (struct hw_device_t*)dev;

	return 0;
}

static struct hw_module_methods_t lights_module_methods = {
	.open = open_lights,
};

hw_module_t HAL_MODULE_INFO_SYM = {
	.tag = HARDWARE_MODULE_TAG,
	.version_major = 1,
	.version_minor = 0,
	.id = LIGHTS_HARDWARE_MODULE_ID,
	.name = "Lights HAL Module For hi6620oem Platform",
	.author = "Kostyan_nsk",
	.methods = &lights_module_methods,
};
