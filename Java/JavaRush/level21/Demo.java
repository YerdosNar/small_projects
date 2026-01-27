public class Demo {
    public void test() {
        Runnable r1 = new Runnable() {
            @Override
            public void run() {
                System.out.println(this); // Выведет: Demo$1 (анонимный класс)
            }
        };

        Runnable r2 = () -> System.out.println(this); // Выведет: Demo (внешний класс)

        r1.run();
        r2.run();
    }

    public static void main(String[] args) {
        new Demo().test();
    }
}
