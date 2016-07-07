package calibration.ui;

/**
 * Interface to EV3 buttons
 */
public interface Buttons {
    enum Button {
        ENTER,
        LEFT,
        RIGHT,
        ESCAPE,
        UP,
        DOWN
    }

    void waitForPress(Button button);
}
