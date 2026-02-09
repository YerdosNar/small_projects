import java.util.ArrayList;
import java.util.List;

public class ListIteration {
    public static void main(String[] args) {
        // Используем интерфейс List для хранения названий предметов
        List<String> classes = new ArrayList<>();

        classes.add( "Математика");
        classes.add("Физика");
        classes.add("Информатика");

        classes.add(0, "Английский");

        // Удаляем предмет "Физика" из расписания
        classes.remove(classes.indexOf("Физика"));

        // Выводим итоговое расписание: каждый предмет с новой строки
        classes.forEach(c -> System.out.println(c));

    }
}
