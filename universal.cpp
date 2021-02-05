#include "universal.h"

b32 keyPressed(ButtonState button) {
	return (button.endedDown) && (button.transitionCount > 0);
}

b32 keyReleased(ButtonState button) {
	return !(button.endedDown) && (button.transitionCount > 0);
}

b32 keyDown(ButtonState button) {
	return (button.endedDown) && (button.transitionCount == 0);
}