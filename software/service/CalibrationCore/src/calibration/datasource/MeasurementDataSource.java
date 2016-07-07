package calibration.datasource;

import calibration.system.Delay;
import calibration.ui.Buttons;
import calibration.ui.Display;
import calibration.SampleProviderWithScale;
import calibration.ui.Sound;
import org.apache.commons.math.linear.Array2DRowRealMatrix;
import org.apache.commons.math.linear.RealMatrix;

import java.io.*;
import java.util.ArrayList;
import java.util.List;

/**
 *
 */
public class MeasurementDataSource implements DataSource {
    public static final int COUNT_DOWN = 10;
    private final Display display;
    private final Buttons buttons;
    private final Sound sound;
    private final Delay delay;

    private final int scaleCount;
    private final SampleProviderWithScale sampleProvider;

    private static short[] initialEeprom = {
            0x4000,       0,       0,
            0,       0x4000,       0,
            0,            0,  0x4000,
            0,            0,       0
    };

    public MeasurementDataSource(Display display, Buttons buttons, Sound sound, Delay delay, int scaleCount, SampleProviderWithScale sampleProvider) {
        this.display = display;
        this.buttons = buttons;
        this.sound = sound;
        this.delay = delay;
        this.scaleCount = scaleCount;
        this.sampleProvider = sampleProvider;
    }

    /**
     * Returns measurement result as a list of samples.
     * Measurements are made in the specified positions, each position
     * contains sampleCount samples. Measurements are made for each
     * supported full-range scale. The result contains list of measurement
     * results for each scale range.
     *
     * @param sampleCount number of measurements in each position
     * @param positions   list of text descriptions of positions
     * @return list of measurement results for each full-range scale
     */
    @Override
    public List<RealMatrix> get(int sampleCount, String[] positions) throws IOException {
        display.clear();
        display.drawString("Initializing", 1, 1);
        display.drawString("Scale=", 1, 2);

        List<RealMatrix> w = new ArrayList<>();

        //Fill the EEPROM with initial values to measure raw data
        for (int scale = 0; scale < sampleCount; ++scale) {
            display.drawInt(scale, 2, 7, 2);

            w.add(new Array2DRowRealMatrix(positions.length * sampleCount, 4));

            sampleProvider.writeEeprom(scale, initialEeprom);
        }


        //Read 2000 samples in 6 positions
        for (int i = 0; i < positions.length; ++i) {
            int offset = sampleCount * i;
            measurement(positions[i], sampleCount, w, offset);
        }
        return w;
    }

    /**
     * Measures the sensor output for one position
     *
     * @param message message to set position
     * @param w list of output matrices
     * @param rowOffset row offset in the output matrices list
     */
    private void measurement(String message, int samplesCount, List<RealMatrix> w, int rowOffset) {
        prepareMeasure(message);

        float[] sample = new float[sampleProvider.sampleSize()];

        for (int scale = 0; scale <scaleCount; ++scale) {
            display.drawInt(scale, 2, 7, 2);

            sampleProvider.setScale(scale);
            //wait for sensor stabilization
            delay.msDelay(20);

            RealMatrix w_item = w.get(scale);

            for (int counter = 0; counter < samplesCount; ++counter) {
                sampleProvider.fetchSample(sample, 0);
                w_item.setEntry(rowOffset + counter, 0, sample[0]);
                w_item.setEntry(rowOffset + counter, 1, sample[1]);
                w_item.setEntry(rowOffset + counter, 2, sample[2]);
                w_item.setEntry(rowOffset + counter, 3, 1);

                //wait for the next sample
                delay.msDelay(3);
            }
        }

        sound.beep();
    }

    /**
     * Display message before measurement
     *
     * @param message message to display
     */
    private void prepareMeasure(String message) {
        display.clear();
        display.drawString(String.format("Hold %s", message), 1, 1);
        display.drawString("Press Enter", 1, 2);

        buttons.waitForPress(Buttons.Button.ENTER);

        //Countdown before measurement
        for (int i = 0; i < COUNT_DOWN; ++i) {
            display.drawInt(COUNT_DOWN - i, 11, 1, 2);
            delay.msDelay(1000);
        }

        display.clear();
        display.drawString("Measuring", 1, 1);
        display.drawString("Scale=", 1, 2);
    }

}
