import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

// Простая реализация репозитория в памяти.
// Сервис не зависит от этого класса — он работает только с интерфейсом TaskRepository.
public class InMemoryTaskRepository implements TaskRepository {
    private final List<Task> tasks = new ArrayList<>();

    @Override
    public void add(Task task) {
        tasks.add(task);
    }

    @Override
    public List<Task> findAll() {
        // Возвращаем неизменяемое представление для наглядности
        return Collections.unmodifiableList(tasks);
    }
}
