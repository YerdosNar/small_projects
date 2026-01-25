// Специализированный тип задачи: домашняя задача с указанием места выполнения
public class HomeTask extends Task {
    private final String location;

    public HomeTask(String title, String location) {
        super(title);
        this.location = location;
    }

    public String getLocation() {
        return location;
    }

    @Override
    public void complete() {
        // Демонстрация выполнения: печатаем сообщение
        System.out.println("Выполнена домашняя задача: \"" + title + "\" (место: " + location + ")");
    }

    @Override
    public String toString() {
        // Детализированное описание — чтобы сервис мог печатать информацию, не зная конкретный тип
        return "Домашняя задача — \"" + title + "\", место: " + location;
    }
}
