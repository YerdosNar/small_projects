import java.time.LocalDate;

public class MyBirthday {
    public static void main(String[] args) {
        LocalDate bday = LocalDate.of(2000, 6, 4);
        System.out.println("Weekday: " + bday.getDayOfWeek());
    }
}
