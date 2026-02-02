import java.time.LocalDate;
import java.util.Scanner;

class Result {

    /*
     * Complete the 'findDay' function below.
     *
     * The function is expected to return a STRING.
     * The function accepts following parameters:
     *  1. INTEGER month
     *  2. INTEGER day
     *  3. INTEGER year
     */

    public static String findDay(int month, int day, int year) {
        return LocalDate.of(year, month, day).getDayOfWeek().toString();
    }

}

public class DayOfWeek {
    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);


        System.out.print("Enter month: ");
        int month = sc.nextInt();
        System.out.print("Enter day: ");
        int day = sc.nextInt();
        System.out.print("Enter year: ");
        int year = sc.nextInt();

        String res = Result.findDay(month, day, year);
        System.out.println(res);
        sc.close();
    }
}

