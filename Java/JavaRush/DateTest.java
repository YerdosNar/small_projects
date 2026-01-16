import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.GregorianCalendar;

public class DateTest {
    public static void main(String[] args) {
        Date date = new Date();
        System.out.println(date);
        System.out.println(date.getYear());
        System.out.println(date.getMonth());
        System.out.println(date.getDate());
        System.out.println(date.getDay());
        SimpleDateFormat format = new SimpleDateFormat("MMM-dd-YYYY");
        System.out.println(format.format(date));

        Calendar calendar = new GregorianCalendar(date.getYear(), date.getMonth(), date.getDay());
        System.out.println(calendar);
        Date date1 = calendar.getTime();
        System.out.println(date1);
        System.out.println("Current Month: " + calendar.get(Calendar.MONTH));
        System.out.println("ERA: " + calendar.get(Calendar.ERA));
        System.out.println("YEAR: " + calendar.get(Calendar.YEAR));
        System.out.println("MONTH: " + calendar.get(Calendar.MONTH));
        System.out.println("DAY_OF_MONTH: " + calendar.get(Calendar.DAY_OF_MONTH));
        System.out.println("DAY_OF_WEEK: " + calendar.get(Calendar.DAY_OF_WEEK));
        System.out.println("HOUR: " + calendar.get(Calendar.HOUR));
        System.out.println("MINUTE: " + calendar.get(Calendar.MINUTE));
        System.out.println("SECOND: " + calendar.get(Calendar.SECOND));

        switch (calendar.get(Calendar.DAY_OF_WEEK)) {
            case Calendar.SATURDAY:
                System.out.println("SATURDAY");
                break;
            case Calendar.SUNDAY:
                System.out.println("SUNDAY");
                break;
            case Calendar.MONDAY:
                System.out.println("MONDAY");
                break;
            case Calendar.FRIDAY:
                System.out.println("FRIDAY");
                break;

            default:
                break;
        }

        calendar.add(Calendar.DAY_OF_MONTH, 2);
        System.out.println(calendar.getTime());
    }
}
