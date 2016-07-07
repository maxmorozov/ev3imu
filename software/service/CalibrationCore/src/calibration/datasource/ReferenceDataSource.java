package calibration.datasource;

import org.apache.commons.math.linear.Array2DRowRealMatrix;
import org.apache.commons.math.linear.RealMatrix;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.function.Function;

/**
 * Creates a matrix with reference values. The resulting matrix
 * has "number of sample * number of positions" rows and "sample size" columns.
 */
public class ReferenceDataSource implements DataSource {
    private final int scaleCount;
    private final int sampleSize;
    //Reference value for each scale range.
    private final double[] reference;
    private final Function<Double, double[][]> orientationProvider;

    /**
     * Creates a reference matrix provider.
     *
     * @param scaleCount number of scale ranges
     * @param sampleSize number of items in one sample
     * @param reference reference values for each scale
     * @param orientationProvider provider that prepares the refernce sample for each measurement position.
     */
    public ReferenceDataSource(int scaleCount, int sampleSize, double[] reference, Function<Double, double[][]> orientationProvider) {
        this.scaleCount = scaleCount;
        this.sampleSize = sampleSize;
        this.reference = reference;
        this.orientationProvider = orientationProvider;
    }

    @Override
    public List<RealMatrix> get(int sampleCount, String[] positions) throws IOException {
        List<RealMatrix> Y = new ArrayList<>();

        //Fill the EEPROM with initial values to measure raw data
        for (int scale = 0; scale < scaleCount; ++scale) {
            Array2DRowRealMatrix y = new Array2DRowRealMatrix(positions.length * sampleCount, sampleSize);
            double[][] orientations = orientationProvider.apply(reference[scale]);
            int row = 0;
            for (int i = 0; i < positions.length; ++i) {
                double[] orientation = orientations[i];
                for (int j = 0; j < sampleCount; ++j) {
                    y.setRow(row, orientation);
                    ++row;
                }
            }
            Y.add(y);
        }
        return Y;
    }
}
