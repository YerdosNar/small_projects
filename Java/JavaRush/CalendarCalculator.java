import java.time.LocalDate;
import java.time.temporal.ChronoUnit;

public class CalendarCalculator {
    public static void main(String[] args) {
        LocalDate today = LocalDate.now();
        LocalDate theDayWeMet = LocalDate.of(2020, 6, 3);
        long days = ChronoUnit.DAYS.between(theDayWeMet, today);
        System.out.println("Days from that day: " + days);
    }
}
