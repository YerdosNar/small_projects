public class Solution {
    public static void main(String[] args) {
        // Используем стандартный функциональный интерфейс Runnable:
        // у него нет параметров и нет возвращаемого значения.
        Runnable a = () -> System.out.println("Лямбда без параметров!");


        // Вызываем действие — сообщение будет напечатано на экран.
        a.run();

    }
}
