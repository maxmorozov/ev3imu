package calibration.datasource;

import org.apache.commons.math.linear.RealMatrix;

import java.io.IOException;
import java.util.List;

/**
 * This interface provides raw device samples
 */
public interface DataSource {
    /**
     * Returns measurement result as a list of samples.
     * Measurements are made in the specified positions, each position
     * contains sampleCount samples. Measurements are made for each
     * supported full-range scale. The result contains list of measurement
     * results for each scale range.
     *
     * @param sampleCount number of measurements in each position
     * @param positions list of text descriptions of positions
     * @return list of measurement results for each full-range scale
     */
    List<RealMatrix> get(int sampleCount, String[] positions) throws IOException;
}
