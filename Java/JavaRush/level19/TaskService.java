import java.util.List;

// Сервис работает ТОЛЬКО с абстракциями: TaskRepository и Task.
// Он не знает про конкретные типы задач и конкретные реализации репозитория.
public class TaskService {
    private final TaskRepository repository;

    public TaskService(TaskRepository repository) {
        this.repository = repository;
    }

    // Получение и вывод информации о всех задачах
    public void printAllTasksInfo() {
        List<Task> tasks = repository.findAll();
        System.out.println("Список задач (" + tasks.size() + "):");
        for (Task task : tasks) {
            // Полиморфный вывод — детали определяются в toString() конкретных задач
            System.out.println(" - " + task);
        }
    }

    // Выполнение всех задач (вызов complete() у каждой)
    public void completeAllTasks() {
        for (Task task : repository.findAll()) {
            task.complete(); // Полиморфный вызов
        }
    }
}
