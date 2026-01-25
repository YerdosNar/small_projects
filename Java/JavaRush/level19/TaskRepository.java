import java.util.List;

// Контракт хранилища задач: только абстракции (Task), без привязки к конкретным типам
public interface TaskRepository {
    void add(Task task);

    List<Task> findAll();
}
