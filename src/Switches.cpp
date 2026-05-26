#include "Switches.h"

#ifdef HAS_TOUCH
	#include <XPT2046_Touchscreen.h>

	extern SPIClass sharedSPI;

	static XPT2046_Touchscreen touchscreen(TOUCH_CS);
	static bool touchscreen_initialized = false;

	static void mapTouchPoint(const TS_Point& point, uint16_t& screenX, uint16_t& screenY) {
		int32_t rawX = point.x;
		int32_t rawY = point.y;

		if (TOUCH_CAL_FLAGS & 0x01) {
			int32_t swapValue = rawX;
			rawX = rawY;
			rawY = swapValue;
		}

		int32_t mappedX = map(rawX, TOUCH_CAL_X_MIN, TOUCH_CAL_X_MAX, 0, TFT_WIDTH - 1);
		int32_t mappedY = map(rawY, TOUCH_CAL_Y_MIN, TOUCH_CAL_Y_MAX, 0, TFT_HEIGHT - 1);

		if (TOUCH_CAL_FLAGS & 0x02)
			mappedX = (TFT_WIDTH - 1) - mappedX;
		if (TOUCH_CAL_FLAGS & 0x04)
			mappedY = (TFT_HEIGHT - 1) - mappedY;

		screenX = constrain(mappedX, 0, TFT_WIDTH - 1);
		screenY = constrain(mappedY, 0, TFT_HEIGHT - 1);
	}

	static int activeTouchButton() {
		if (!touchscreen_initialized) {
			pinMode(TOUCH_CS, OUTPUT);
			digitalWrite(TOUCH_CS, HIGH);
			touchscreen.begin(sharedSPI);
			touchscreen.setRotation(TOUCH_ROTATION);
			touchscreen_initialized = true;
		}

		if (!touchscreen.touched())
			return -1;

		TS_Point point = touchscreen.getPoint();
		uint16_t touchX = 0;
		uint16_t touchY = 0;
		mapTouchPoint(point, touchX, touchY);

		if (touchY < (TFT_HEIGHT / 3))
			return TOUCH_BTN_UP;
		if (touchY < ((TFT_HEIGHT / 3) * 2))
			return TOUCH_BTN_SELECT;

		return TOUCH_BTN_DOWN;
	}
#endif

Switches::Switches() {
	this->pin = 0;
	this->pin = false;
	this->pressed = false;
	this->hold_lim = 2000;
	this->cur_hold = 0;
	this->isheld = false;
	
	#ifndef HAS_TOUCH
	  pinMode(this->pin, INPUT);
	#endif
	
	return;
}

Switches::Switches(int pin, uint32_t hold_lim, bool pullup) {
	this->pin = pin;
	this->pullup = pullup;
	this->pressed = false;
	this->hold_lim = hold_lim;
	this->cur_hold = 0;
	this->isheld = false;
	
	#ifndef HAS_TOUCH
		if (pullup)
			pinMode(this->pin, INPUT_PULLUP);
		else
			pinMode(this->pin, INPUT_PULLDOWN);
	#endif
	
	return;
}

int Switches::getPin() {
	return this->pin;
}

bool Switches::getPullup() {
	return this->pullup;
}

bool Switches::isHeld() {
	return this->isheld;
}

bool Switches::getButtonState() {
	#ifdef HAS_TOUCH
	  return activeTouchButton() == this->pin;
	#endif

	int buttonState = digitalRead(this->pin);
	
	if ((this->pullup) && (buttonState == LOW))
		return true;
	else if ((!this->pullup) && (buttonState == HIGH))
		return true;
	else
		return false;
}

bool Switches::justPressed() {
	bool btn_state = this->getButtonState();
	
	// Button was JUST pressed
	if (btn_state && !this->pressed) {
		this->hold_init = millis();
		this->pressed = btn_state;
		return true;
	}
	else if (btn_state) { // Button is STILL pressed
		// Check if button is held
		//Serial.println("cur_hold: " + (String)this->cur_hold);
		if ((millis() - this->hold_init) < this->hold_lim) {
			this->isheld = false;
		}
		else {
			this->isheld = true;
		}
		
		this->pressed = btn_state;
		return false;
	}
	else { // Button is not pressed
		this->pressed = btn_state;
		this->isheld = false;
		return false;
	}
}

bool Switches::justReleased() {
	bool btn_state = this->getButtonState();
	
	// Button was JUST released
	if (!btn_state && this->pressed) {
		this->isheld = false;
		this->pressed = btn_state;
		return true;
	}
	else { // Button is STILL released
		this->pressed = btn_state;
		return false;
	}
	
}