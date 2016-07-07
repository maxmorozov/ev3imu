package calibration.ui;

import lejos.hardware.lcd.LCD;

/**
 * EV3 LCD display
 */
public class EV3Display implements Display {
    /**
     * Clear the display.
     */
    @Override
    public void clear() {
        LCD.clear();
    }

    /**
     * Display a string on the LCD at specified x,y co-ordinate.
     *
     * @param str The string to be displayed
     * @param x   The x character co-ordinate to display at.
     * @param y   The y character co-ordinate to display at.
     */
    @Override
    public void drawString(String str, int x, int y) {
        LCD.drawString(str, x, y);
    }

    /**
     * Display an in on the LCD at x,y with leading spaces to occupy at least the number
     * of characters specified by the places parameter.
     *
     * @param i      The value to display
     * @param places number of places to use to display the value
     * @param x      The x character co-ordinate to display at.
     * @param y      The y character co-ordinate to display at.
     */
    @Override
    public void drawInt(int i, int places, int x, int y) {
        LCD.drawInt(i, places, x, y);

    }
}
