package calibration.datasource;

import calibration.ui.Display;
import org.apache.commons.math.linear.RealMatrix;

import java.io.*;
import java.util.List;

/**
 * A helper class that saves measurement results on the disk
 */
public class SavingDataDecorator implements DataSource {
    private final Display display;
    private final DataSource dataSource;
    private final String fileTemplate;

    public SavingDataDecorator(Display display, DataSource dataSource, String fileTemplate) {
        this.display = display;
        this.dataSource = dataSource;
        this.fileTemplate = fileTemplate;
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
        List<RealMatrix> result = dataSource.get(sampleCount, positions);

        display.clear();
        display.drawString("Saving data", 1, 1);
        display.drawString("Scale=", 1, 2);

        for (int scale = 0; scale < result.size(); ++scale) {
            display.drawInt(scale, 2, 7, 2);
            saveMatrix(result.get(scale), String.format(fileTemplate, scale));
        }

        return result;
    }

    /**
     * Writes matrix to the disk
     *
     * @param matrix matrix to write
     * @param fileName file name
     * @throws IOException
     */
    private static void saveMatrix(RealMatrix matrix, String fileName) throws IOException {
        try (OutputStream os = new FileOutputStream(fileName);
             BufferedOutputStream bos = new BufferedOutputStream(os, 64000);
             PrintStream ps = new PrintStream(bos)) {

            for (int row = 0; row < matrix.getRowDimension(); ++row) {
                for (int col = 0; col < matrix.getColumnDimension(); ++col) {
                    ps.print(matrix.getEntry(row, col));
                    ps.print(",");
                }
                ps.print("\n");
            }
            bos.flush();
        }
    }

}
