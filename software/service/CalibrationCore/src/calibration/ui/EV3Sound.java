package calibration.ui;

/**
 * EV3 audio subsystem
 */
public class EV3Sound implements Sound {
    @Override
    public void beep() {
        lejos.hardware.Sound.beep();
    }
}
