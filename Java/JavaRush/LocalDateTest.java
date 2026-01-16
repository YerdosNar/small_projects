import java.time.LocalDate;
import java.time.ZoneId;
import java.util.Iterator;
import java.util.Set;

public class LocalDateTest {
    public static void main(String[] args) {
        LocalDate today = LocalDate.now();
        System.out.println("Today: "+ today);

        Set<String> zoneIds = ZoneId.getAvailableZoneIds();
        zoneIds.forEach(System.out::println);

        ZoneId timezone = ZoneId.of("Asia/Tashkent");
        today = LocalDate.now(timezone);
        System.out.println("Now in Uzbekistan: " + today);

        LocalDate thatDay = LocalDate.of(2020, 6, 3);
        System.out.println(thatDay.getDayOfYear());
    }
}
