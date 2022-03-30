#!/bin/bash

#
# Little hack to control brightness in offmode charger
#

# Variables
BRIGHTNESS_NODE="/sys/class/leds/lcd_backlight0/brightness"
MAX_BRIGHTNESS_NODE="/sys/class/leds/lcd_backlight0/max_brightness"
GETPROP="$(getprop sys.use_charger_brightness)"

# Exit if device is not in offmode charger
if [ "$GETPROP" != 1]
    exit 0
fi

# Turn the display on
if [ "$(cat $BRIGHTNESS_NODE)" = 0 ]
    echo "$(cat $MAX_BRIGHTNESS_NODE)" > "$BRIGHTNESS_NODE"
fi

# Turn the display off
if [ "$(cat $BRIGHTNESS_NODE)" != 0 ]
    echo "0" > "$BRIGHTNESS_NODE"
fi
