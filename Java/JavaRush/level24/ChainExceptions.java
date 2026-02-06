public class ChainExceptions {
    public static void main(String[] args) {
        // Создаем "первопричину" — уже существующее исключение
        Throwable rootCause = new NullPointerException("данные отсутствуют!");

        // Создаем наше исключение и "прикрепляем" к нему первопричину
        DataProcessingFailure failure = new DataProcessingFailure("Ошибка генерации отчёта", rootCause);


        // Выводим на экран первопричину через getCause()
        // Ожидаем увидеть NullPointerException с тем же сообщением
        System.out.println(failure.getCause());
    }
}

class DataProcessingFailure extends Exception {
    public DataProcessingFailure(String msg) {
        super(msg);
    }

    public DataProcessingFailure(String msg, Throwable th) {
        super(msg, th);
    }
}
