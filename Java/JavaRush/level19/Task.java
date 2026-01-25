// Базовая абстракция задачи: содержит общий заголовок и абстрактный метод завершения
public abstract class Task {
    // Заголовок задачи
    protected final String title;

    public Task(String title) {
        this.title = title;
    }

    // Геттер оставлен публичным, чтобы при необходимости можно было использовать его в абстракциях
    public String getTitle() {
        return title;
    }

    // Действие завершения задачи — конкретизация остаётся за наследниками
    public abstract void complete();

    // Базовое описание (может быть переопределено наследниками для детализации)
    @Override
    public String toString() {
        return "Задача: " + title;
    }
}
