package deviation;

/**
 * @author Max Morozov
 */
public class DeviationCalc3D {
    private DeviationCalc x = new DeviationCalc();
    private DeviationCalc y = new DeviationCalc();
    private DeviationCalc z = new DeviationCalc();
    private DeviationCalc magnitude = new DeviationCalc();

    public void add(float[] value) {
        x.add(value[0]);
        y.add(value[1]);
        z.add(value[2]);
        magnitude.add(getMagnitude(value, 0));
    }

    public void add(float[] value, int offset) {
        x.add(value[offset]);
        y.add(value[offset + 1]);
        z.add(value[offset + 2]);
        magnitude.add(getMagnitude(value, offset));
    }

    public void add(int[] value) {
        x.add(value[0]);
        y.add(value[1]);
        z.add(value[2]);
        magnitude.add(getMagnitude(value));
    }

    public FloatValue getAverage() {
        FloatValue result = new FloatValue();
        result.x = x.getAverage();
        result.y = y.getAverage();
        result.z = z.getAverage();

        return result;
    }

    public FloatValue getDeviation() {
        FloatValue result = new FloatValue();
        result.x = x.getDeviation();
        result.y = y.getDeviation();
        result.z = z.getDeviation();

        return result;
    }

    public float getAverageMagnitude() {
        return magnitude.getAverage();
    }

    public float getMagnitudeDeviation() {
        return magnitude.getDeviation();
    }

    private float getMagnitude(float[] value, int offset) {
        float sum = 0;
        for (int i=0; i<3; ++i) {
            sum += value[i + offset] * value[i + offset];
        }
        return (float) Math.sqrt(sum);
    }

    private float getMagnitude(int[] value) {
        float sum = 0;
        for (int i=0; i<3; ++i) {
            sum += value[i] * value[i];
        }
        return (float) Math.sqrt(sum);
    }
}
