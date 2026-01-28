import java.util.HashMap;
import java.util.Map;

public class Demo {
    public static void main(String[] args) {
        record Point(int x, int y) {
            @Override
            public String toString() {
                return "Point: \n\tx = "+x+"\n\ty = "+y;
            }
        }

        Map<Point, String> map = new HashMap<>();
        Point p1 = new Point(3, 4);
        map.put(p1, "Hello");

        Point p2 = new Point(3, 4);
        System.out.println(map.get(p2));

        System.out.println(p1);
        System.out.println(p2);
    }
}
