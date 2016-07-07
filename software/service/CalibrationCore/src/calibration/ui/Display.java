package calibration.ui;

/**
 * Interface to Lego EV3 display
 */
public interface Display {
    /**
     * Clear the display.
     */
    void clear();

    /**
     * Display a string on the LCD at specified x,y co-ordinate.
     *
     * @param str The string to be displayed
     * @param x The x character co-ordinate to display at.
     * @param y The y character co-ordinate to display at.
     */
    void drawString(String str, int x, int y);

    /**
     * Display an in on the LCD at x,y with leading spaces to occupy at least the number
     * of characters specified by the places parameter.
     *
     * @param i The value to display
     * @param places number of places to use to display the value
     * @param x The x character co-ordinate to display at.
     * @param y The y character co-ordinate to display at.
     */
    void drawInt(int i, int places, int x, int y);

}
