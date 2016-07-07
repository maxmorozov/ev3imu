package calibration.ui;

import lejos.hardware.Key;

/**
 * EV3 keyboard
 */
public class EV3Buttons implements Buttons {
    @Override
    public void waitForPress(Button button) {
        getButton(button).waitForPress();
    }

    private Key getButton(Button button) {
        switch (button) {
            case ENTER:
                return lejos.hardware.Button.ENTER;
            case LEFT:
                return lejos.hardware.Button.LEFT;
            case RIGHT:
                return lejos.hardware.Button.RIGHT;
            case ESCAPE:
                return lejos.hardware.Button.ESCAPE;
            case UP:
                return lejos.hardware.Button.UP;
            case DOWN:
                return lejos.hardware.Button.DOWN;
        }
        throw new RuntimeException("Wrong button " + button);
    }
}
