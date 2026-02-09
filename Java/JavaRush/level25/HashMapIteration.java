import java.util.HashMap;

public class HashMapIteration {
    public static void main(String[] args) {
        // Создаем "картотеку" студентов: ключ — имя (String), значение — возраст (Integer)
        HashMap<String, Integer> students = new HashMap<>();

        // Добавляем три записи о студентах
        students.put("Иван", 20);
        students.put("Мария", 21);
        students.put("Алексей", 19);


        // Выводим полный список всех студентов и их возрастов
        students.forEach((k, v) -> System.out.println(k + " — " + v));

    }
}
