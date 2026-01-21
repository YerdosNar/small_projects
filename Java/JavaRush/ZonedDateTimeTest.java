import java.time.ZoneId;
import java.time.ZonedDateTime;

public class ZonedDateTimeTest {
    public static void main(String[] args) {
        String location = "";
        for(int i = 0; i < args.length; i++) {
            System.out.println(args[i]);
        }
        if(args.length < 1) {
            location = "Asia/Seoul";
        }
        else {
            location = args[0];
        }

        ZonedDateTime zoneTime = ZonedDateTime.now(ZoneId.of(location));
        System.out.println(zoneTime);
    }
}
